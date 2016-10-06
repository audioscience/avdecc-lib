/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2013 AudioScience Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * cmd_line.cpp
 *
 * AVDECC command line processing implementation
 */

#include <assert.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <limits.h>
#include <string.h>

#include "end_station.h"
#include "entity_descriptor.h"
#include "configuration_descriptor.h"
#include "audio_unit_descriptor.h"
#include "stream_input_descriptor.h"
#include "stream_output_descriptor.h"
#include "jack_input_descriptor.h"
#include "jack_output_descriptor.h"
#include "avb_interface_descriptor.h"
#include "clock_source_descriptor.h"
#include "memory_object_descriptor.h"
#include "locale_descriptor.h"
#include "strings_descriptor.h"
#include "stream_port_input_descriptor.h"
#include "stream_port_output_descriptor.h"
#include "audio_cluster_descriptor.h"
#include "audio_map_descriptor.h"
#include "clock_domain_descriptor.h"
#include "external_port_input_descriptor.h"
#include "external_port_output_descriptor.h"
#include "descriptor_field.h"
#include "descriptor_field_flags.h"
#include "cmd_line.h"
#include "cli_argument.h"
#include "cli_command.h"
#include "cli_command_format.h"
#include "control_descriptor.h"

#if defined _WIN32 || defined _WIN64
#ifdef _MSC_VER
#define strtoull(str, end, base) _strtoui64(str, end, base)
#endif
#endif

#define END_STATION_HELP "the End Station"
#define DST_END_STATION_HELP "the destination End Station"
#define SRC_END_STATION_HELP "the source End Station"

const char * const log_level_help =
    "Valid log levels are:\n"
    "  0 - LOGGING_LEVEL_ERROR,  1 - LOGGING_LEVEL_WARNING,\n"
    "  2 - LOGGING_LEVEL_NOTICE, 3 - LOGGING_LEVEL_INFO,\n"
    "  4 - LOGGING_LEVEL_DEBUG,  5 - LOGGING_LEVEL_VERBOSE.";

std::string cmd_line::log_path = "."; // Log to a file in the current working directory

cmd_line::cmd_line()
    : test_mode(false), output_redirected(false)
{
}

cmd_line::cmd_line(void (*notification_callback)(void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
                   void (*acmp_notification_callback)(void *, int32_t, uint16_t, uint64_t, uint16_t, uint64_t, uint16_t,
                                                      uint32_t, void *),
                   void (*log_callback)(void *, int32_t, const char *, int32_t),
                   bool test_mode, char * interface, int32_t log_level)
    : test_mode(test_mode), output_redirected(false)
{
    cout_buf = std::cout.rdbuf();
    current_end_station = 0;

    // Start non-zero so as not to be confused with commands without notification
    notification_id = 1;

    cmd_line_commands_init();

    netif = avdecc_lib::create_net_interface();
    controller_obj = avdecc_lib::create_controller(netif, notification_callback, acmp_notification_callback, log_callback, log_level);
    controller_obj->apply_end_station_capabilities_filters(avdecc_lib::ENTITY_CAPABILITIES_GPTP_SUPPORTED |
                                                               avdecc_lib::ENTITY_CAPABILITIES_AEM_SUPPORTED,
                                                           0, 0);

    sys = avdecc_lib::create_system(avdecc_lib::system::LAYER2_MULTITHREADED_CALLBACK, netif, controller_obj);
    atomic_cout << "AVDECC Controller version: " << controller_obj->get_version() << std::endl;
    print_interfaces_and_select(interface);
    sys->process_start();
}

cmd_line::~cmd_line()
{
    sys->process_close();
    sys->destroy();
    controller_obj->destroy();
    netif->destroy();
    ofstream_ref.close();
}

const cli_command * cmd_line::get_commands() const
{
    return &commands;
}

avdecc_lib::controller * cmd_line::get_controller() const
{
    return controller_obj;
}

bool cmd_line::handle(std::vector<std::string> & args)
{
    std::queue<std::string, std::deque<std::string>> args_queue(std::deque<std::string>(args.begin(), args.end()));
    bool done = false;
    bool ok = commands.run_command(this, args_queue, done);
    if (!ok)
    {
        printf("Invalid command\n");
        commands.print_help_all("", 1);
    }
    return done;
}

int cmd_line::print_interfaces_and_select(char * interface)
{
    int interface_num = -1;

    for (uint32_t i = 1; i < netif->devs_count() + 1; i++)
    {
        size_t dev_index = i - 1;
        char * dev_desc = netif->get_dev_desc_by_index(dev_index);
        if (!interface)
        {
            printf("%d (%s)", i, dev_desc);
            
            uint64_t dev_mac = netif->get_dev_mac_addr_by_index(dev_index);
            if (dev_mac)
            {
                avdecc_lib::utility::MacAddr mac(dev_mac);
                char mac_str[20];
                mac.tostring(mac_str);
                printf(" (%s)", mac_str);
            }
            
            size_t ip_addr_count = netif->device_ip_address_count(dev_index);
            if (ip_addr_count > 0)
            {
                for(size_t ip_index = 0; ip_index < ip_addr_count; ip_index++)
                {
                    const char * dev_ip = netif->get_dev_ip_address_by_index(dev_index, ip_index);
                    if (dev_ip)
                        printf(" <%s>", dev_ip);
                }
            }
            printf("\n");
        }
        else
        {
            // try to find the selected interface by ip address
            if (netif->does_interface_have_ip_address(dev_index, interface))
            {
                interface_num = i;
                break;
            }

            // try to find the selected interface by MAC address
            avdecc_lib::utility::MacAddr mac(interface);
            if (mac.fromstring(interface)) // valid format? (xx:xx:...)
            {
                uint64_t mac_val = mac.tovalue();
                if (mac_val)
                {
                    if (netif->does_interface_have_mac_address(dev_index, mac_val))
                    {
                        interface_num = i;
                        break;
                    }
                }
            }
        }
    }

    if (!interface)
    {
        atomic_cout << "Enter the interface number (1-" << std::dec << netif->devs_count() << "): ";
        std::cin >> interface_num;
    }
    else
    {
        // if the interface was not found by IP Address or MAC address...
        if (interface_num == -1)
        {
            // treat the selected interface as an index
            char * tmp;
            long if_num = strtol(interface, &tmp, 10);
            if (interface != tmp)
                interface_num = if_num;
        }
    }

    if (interface_num == -1 ||
        interface_num > netif->devs_count() + 1)
    {
        printf("Invalid Interface: (%s).  Exiting...\n", interface);
        exit(EXIT_FAILURE);
    }
    
    netif->select_interface_by_num(interface_num);

    return 0;
}

int cmd_line::get_current_end_station(avdecc_lib::end_station ** end_station) const
{
    if (current_end_station >= controller_obj->get_end_station_count())
    {
        atomic_cout << "No End Stations available" << std::endl;
        *end_station = NULL;
        return 1;
    }

    *end_station = controller_obj->get_end_station_by_index(current_end_station);
    return 0;
}

int cmd_line::get_current_entity_and_descriptor(avdecc_lib::end_station * end_station,
                                                avdecc_lib::entity_descriptor ** entity, avdecc_lib::configuration_descriptor ** configuration)
{
    *entity = NULL;
    *configuration = NULL;

    uint16_t current_entity = end_station->get_current_entity_index();
    if (current_entity >= end_station->entity_desc_count())
    {
        atomic_cout << "Current entity not available" << std::endl;
        return 1;
    }

    *entity = end_station->get_entity_desc_by_index(current_entity);

    uint16_t current_config = end_station->get_current_config_index();
    if (current_config >= (*entity)->config_desc_count())
    {
        atomic_cout << "Current configuration not available" << std::endl;
        return 1;
    }

    *configuration = (*entity)->get_config_desc_by_index(current_config);

    return 0;
}

int cmd_line::get_current_end_station_entity_and_descriptor(avdecc_lib::end_station ** end_station,
                                                            avdecc_lib::entity_descriptor ** entity, avdecc_lib::configuration_descriptor ** configuration)
{
    if (get_current_end_station(end_station))
        return 1;

    if (get_current_entity_and_descriptor(*end_station, entity, configuration))
    {
        atomic_cout << "Current End Station not fully enumerated" << std::endl;
        return 1;
    }
    return 0;
}

void cmd_line::cmd_line_commands_init()
{
    // Create the commands. Each command can have multiple sub-commands and/or multiple formats.
    // Each format is what will ultimately call back into the cmd_line to perform the processing
    // once the arguments have matched.

    // help
    cli_command * help_cmd = new cli_command();
    commands.add_sub_command("help", help_cmd);

    cli_command_format * help_one_fmt = new cli_command_format(
        "Display details of specified command.",
        &cmd_line::cmd_help_one);
    help_one_fmt->add_argument(new cli_argument_string(this, "cmd", "the command for which to show details", "", 1, UINT_MAX));
    help_cmd->add_format(help_one_fmt);

    cli_command_format * help_all_fmt = new cli_command_format(
        "Display a list of valid commands.\n"
        "Use \"help -a\" to display a complete list of commands",
        &cmd_line::cmd_help_all);
    help_cmd->add_format(help_all_fmt);

    // version
    cli_command * version_cmd = new cli_command();
    commands.add_sub_command("version", version_cmd);

    cli_command_format * version_format = new cli_command_format(
        "Display the current AVDECC Controller build release version.",
        &cmd_line::cmd_version);
    version_cmd->add_format(version_format);

    // list
    cli_command * list_cmd = new cli_command();
    commands.add_sub_command("list", list_cmd);

    cli_command_format * list_fmt = new cli_command_format(
        "Display a table with information about each End Station.",
        &cmd_line::cmd_list);
    list_cmd->add_format(list_fmt);

    // select
    cli_command * select_cmd = new cli_command("To see a list of valid End Stations, enter \"list\" command.");
    commands.add_sub_command("select", select_cmd);

    cli_command_format * select_fmt = new cli_command_format(
        "Change the setting of End Station, entity, and configuration.",
        &cmd_line::cmd_select);
    select_fmt->add_argument(new cli_argument_end_station(this, "e_s", END_STATION_HELP));
    select_fmt->add_argument(new cli_argument_int(this, "e_i", "the entity index"));
    select_fmt->add_argument(new cli_argument_int(this, "c_i", "the configuration index"));
    select_cmd->add_format(select_fmt);

    cli_command_format * show_select_fmt = new cli_command_format(
        "Display the current End Station, Entity, and Configuration setting.",
        &cmd_line::cmd_show_select);
    select_cmd->add_format(show_select_fmt);

    // audio mappings
    cli_command * audio_mappings = new cli_command();
    commands.add_sub_command("audio_mappings", audio_mappings);

    // remove audio mappings
    cli_command * remove_audio_mappings_cmd = new cli_command();
    audio_mappings->add_sub_command("remove", remove_audio_mappings_cmd);
    cli_command_format * remove_audio_mappings_fmt = new cli_command_format(
        "Send a REMOVE_AUDIO_MAPPINGS command to remove the queued mapping entries\n"
        "from the dynamic mapping between the Audio Clusters and Input/Output Streams\n",
        &cmd_line::cmd_remove_audio_mappings);
    remove_audio_mappings_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                    "Valid descriptor types are STREAM_PORT_INPUT and STREAM_PORT_OUTPUT."));
    remove_audio_mappings_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                                 "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                                 "\"view all\" command."));
    remove_audio_mappings_cmd->add_format(remove_audio_mappings_fmt);

    // store pending audio mapping
    cli_command * store_pending_audio_mapping_cmd = new cli_command();
    audio_mappings->add_sub_command("store_pending", store_pending_audio_mapping_cmd);

    cli_command_format * store_pending_audio_mapping_fmt = new cli_command_format(
        "Send an ADD_PENDING_AUDIO_MAPPING command to add mapping entries to a local queue.\n"
        "for adding or removal\n",
        &cmd_line::cmd_store_pending_audio_mapping);
    store_pending_audio_mapping_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                          "Valid descriptor types are STREAM_PORT_INPUT and STREAM_PORT_OUTPUT."));
    store_pending_audio_mapping_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                                       "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                                       "\"view all\" command."));

    store_pending_audio_mapping_fmt->add_argument(new cli_argument_int(this, "s_i", "the stream index."));
    store_pending_audio_mapping_fmt->add_argument(new cli_argument_int(this, "s_c", "the stream channel"));
    store_pending_audio_mapping_fmt->add_argument(new cli_argument_int(this, "c_o", "the cluster offset."));
    store_pending_audio_mapping_fmt->add_argument(new cli_argument_int(this, "c_c", "the cluster channel."));

    store_pending_audio_mapping_cmd->add_format(store_pending_audio_mapping_fmt);

    // view pending audio mappings
    cli_command * get_pending_audio_mappings_cmd = new cli_command();
    audio_mappings->add_sub_command("get_pending", get_pending_audio_mappings_cmd);

    cli_command_format * get_pending_audio_mappings_fmt = new cli_command_format(
        "Send an GET_PENDING_AUDIO_MAPPINGS command to read queued entries.",
        &cmd_line::cmd_get_pending_audio_mappings);
    get_pending_audio_mappings_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                         "Valid descriptor types are STREAM_PORT_INPUT and STREAM_PORT_OUTPUT."));
    get_pending_audio_mappings_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                                      "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                                      "\"view all\" command."));
    get_pending_audio_mappings_cmd->add_format(get_pending_audio_mappings_fmt);

    // clear pending audio mappings
    cli_command * clear_pending_audio_mappings_cmd = new cli_command();
    audio_mappings->add_sub_command("clear_pending", clear_pending_audio_mappings_cmd);

    cli_command_format * clear_pending_audio_mappings_fmt = new cli_command_format(
        "Send an CLEAR_PENDING_AUDIO_MAPPINGS command to clear queued entries.",
        &cmd_line::cmd_clear_pending_audio_mappings);
    clear_pending_audio_mappings_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                           "Valid descriptor types are STREAM_PORT_INPUT and STREAM_PORT_OUTPUT."));
    clear_pending_audio_mappings_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                                        "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                                        "\"view all\" command."));
    clear_pending_audio_mappings_cmd->add_format(clear_pending_audio_mappings_fmt);

    // add audio mappings
    cli_command * add_audio_mappings_cmd = new cli_command();
    audio_mappings->add_sub_command("add", add_audio_mappings_cmd);

    cli_command_format * add_audio_mappings_fmt = new cli_command_format(
        "Send an ADD_AUDIO_MAPPINGS command to add queued mapping entries to the dynamic\n"
        "mapping between the Audio Clusters and Input/Output Streams\n",
        &cmd_line::cmd_add_audio_mappings);
    add_audio_mappings_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                 "Valid descriptor types are STREAM_PORT_INPUT and STREAM_PORT_OUTPUT."));
    add_audio_mappings_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                              "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                              "\"view all\" command."));
    add_audio_mappings_cmd->add_format(add_audio_mappings_fmt);

    // log
    cli_command * log_cmd = new cli_command();
    commands.add_sub_command("log", log_cmd);

    cli_command_format * log_fmt = new cli_command_format(
        "Redirect output to a specified file.",
        &cmd_line::cmd_log);
    log_fmt->add_argument(new cli_argument_string(this, "f_n", "the file name"));
    log_cmd->add_format(log_fmt);

    // log level
    cli_command * log_level_cmd = new cli_command();
    log_cmd->add_sub_command("level", log_level_cmd);

    cli_command_format * log_level_fmt = new cli_command_format(
        "Update the base log level for messages to be logged by the logging callback.",
        &cmd_line::cmd_log_level);
    log_level_fmt->add_argument(new cli_argument_int(this, "n_l_l", "the new log level",
                                                     log_level_help));
    log_level_cmd->add_format(log_level_fmt);

    // unlog
    cli_command * unlog_cmd = new cli_command();
    commands.add_sub_command("unlog", unlog_cmd);

    cli_command_format * unlog_fmt = new cli_command_format(
        "Set output scheme back to console screen.",
        &cmd_line::cmd_unlog);
    unlog_cmd->add_format(unlog_fmt);

    // view
    cli_command * view_cmd = new cli_command();
    commands.add_sub_command("view", view_cmd);

    // view all
    cli_command * view_all_cmd = new cli_command();
    view_cmd->add_sub_command("all", view_all_cmd);

    cli_command_format * view_all_fmt = new cli_command_format(
        "Display all the top level descriptors present in all End Stations.",
        &cmd_line::cmd_view_all);
    view_all_cmd->add_format(view_all_fmt);

    // view media
    cli_command * view_media_cmd = new cli_command();
    view_cmd->add_sub_command("media", view_media_cmd);

    // view media clock
    cli_command * view_media_clock_cmd = new cli_command();
    view_media_cmd->add_sub_command("clock", view_media_clock_cmd);

    cli_command_format * view_media_clock_fmt = new cli_command_format(
        "Display a list of descriptors that has the Clock Sync Source flag set.",
        &cmd_line::cmd_view_media_clock);
    view_media_clock_cmd->add_format(view_media_clock_fmt);

    // view details
    cli_command * view_details_cmd = new cli_command("To see a list of valid End Stations, enter \"list\" command.");
    view_cmd->add_sub_command("details", view_details_cmd);

    cli_command_format * view_details_fmt = new cli_command_format("Display all descriptors in the currently selected configuration.",
                                                                   &cmd_line::cmd_view_details);
    //view_details_fmt->add_argument(new cli_argument_end_station(this, "e_s", END_STATION_HELP));
    //commented out for new 'view_details' format (just views currently selected entity/endpoint
    view_details_cmd->add_format(view_details_fmt);

    // view descriptor
    cli_command * view_descriptor_cmd = new cli_command(
        "To see a list of valid descriptor types and corresponding indexes,\n"
        "use the \"view all\" command.");
    view_cmd->add_sub_command("descriptor", view_descriptor_cmd);

    cli_command_format * view_descriptor_fmt = new cli_command_format(
        "Display information for the specified descriptor using the current setting.",
        &cmd_line::cmd_view_descriptor);
    view_descriptor_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    view_descriptor_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index"));
    view_descriptor_cmd->add_format(view_descriptor_fmt);

    // view stream format
    cli_command * view_stream_formats_cmd = new cli_command();
    view_cmd->add_sub_command("stream_formats", view_stream_formats_cmd);

    cli_command_format * view_stream_formats_fmt = new cli_command_format("Display all possible stream formats.",
                                                                          &cmd_line::cmd_view_stream_formats);
    view_stream_formats_cmd->add_format(view_stream_formats_fmt);

    // show
    cli_command * show_cmd = new cli_command();
    commands.add_sub_command("show", show_cmd);

    // show connections
    cli_command * show_connections_cmd = new cli_command();
    show_cmd->add_sub_command("connections", show_connections_cmd);

    cli_command_format * show_connections_fmt = new cli_command_format(
        "Show all active connections.",
        &cmd_line::cmd_show_connections);
    show_connections_cmd->add_format(show_connections_fmt);

    // connect
    cli_command * connect_cmd = new cli_command();
    commands.add_sub_command("connect", connect_cmd);

    cli_command_format * connect_rx_fmt = new cli_command_format(
        "Connect an instream to an outstream.",
        &cmd_line::cmd_connect_rx);
    connect_rx_fmt->add_argument(new cli_argument_end_station(this, "s_e_s", SRC_END_STATION_HELP));
    connect_rx_fmt->add_argument(new cli_argument_int(this, "s_d_i", "the source descriptor index"));
    connect_rx_fmt->add_argument(new cli_argument_end_station(this, "d_e_s", DST_END_STATION_HELP));
    connect_rx_fmt->add_argument(new cli_argument_int(this, "d_d_i", "the destination descriptor index"));
    connect_rx_fmt->add_argument(new cli_argument_string(this, "f", "the set of flags",
                                                         "Valid flags are class_b, fast_connect, saved_state, streaming_wait,\n"
                                                         "supports_encrypted, encrypted_pdu, and talker_failed.",
                                                         0, UINT_MAX));
    connect_cmd->add_format(connect_rx_fmt);

    cli_command_format * connect_dst_fmt = new cli_command_format(
        "Display all the available outstreams for all End Stations that can connect with\n"
        "the instreams.",
        &cmd_line::cmd_connect_dst);
    connect_dst_fmt->add_argument(new cli_argument_end_station(this, "d_e_s", DST_END_STATION_HELP));
    connect_dst_fmt->add_argument(new cli_argument_int(this, "d_d_i", "the destination descriptor index"));
    connect_cmd->add_format(connect_dst_fmt);

    cli_command_format * connect_none_fmt = new cli_command_format(
        "Display all the available instreams for all End Stations.",
        &cmd_line::cmd_connect);
    connect_cmd->add_format(connect_none_fmt);

    // disconnect
    cli_command * disconnect_cmd = new cli_command();
    commands.add_sub_command("disconnect", disconnect_cmd);

    cli_command_format * disconnect_fmt = new cli_command_format(
        "Send a CONNECT_RX command to disconnect Listener sink stream.",
        &cmd_line::cmd_disconnect_rx);
    disconnect_fmt->add_argument(new cli_argument_end_station(this, "s_e_s", SRC_END_STATION_HELP));
    disconnect_fmt->add_argument(new cli_argument_int(this, "s_d_i", "the source descriptor index"));
    disconnect_fmt->add_argument(new cli_argument_end_station(this, "d_e_s", DST_END_STATION_HELP));
    disconnect_fmt->add_argument(new cli_argument_int(this, "d_d_i", "the destination descriptor index"));
    disconnect_cmd->add_format(disconnect_fmt);

    // get
    cli_command * get_cmd = new cli_command();
    commands.add_sub_command("get", get_cmd);

    // get connection status
    cli_command * get_connection_status_cmd = new cli_command();
    get_cmd->add_sub_command("connection_status", get_connection_status_cmd);
    
    cli_command_format * get_connection_status_fmt = new cli_command_format(
        "Get the connection status of an end station.",
        &cmd_line::cmd_get_connection_status);
    get_connection_status_fmt->add_argument(new cli_argument_string(this, "e_g", "the end station - by GUID or index"));
    get_connection_status_cmd->add_format(get_connection_status_fmt);
    
    // get name
    cli_command * get_name_cmd = new cli_command();
    get_cmd->add_sub_command("name", get_name_cmd);

    cli_command_format * get_name_state_fmt = new cli_command_format(
        "Send a GET_NAME command to fetch the value of a name field within a descriptor.",
        &cmd_line::cmd_get_name);
    get_name_state_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    get_name_state_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index"));
    get_name_cmd->add_format(get_name_state_fmt);

    // get group name
    cli_command * get_group_name_cmd = new cli_command();
    get_cmd->add_sub_command("group_name", get_group_name_cmd);

    cli_command_format * get_group_name_state_fmt = new cli_command_format(
        "Send a GET_GROUP_NAME command to fetch the group name of an ENTITY descriptor.",
        &cmd_line::cmd_get_group_name);
    get_group_name_cmd->add_format(get_group_name_state_fmt);

    // get rx
    cli_command * get_rx_cmd = new cli_command();
    get_cmd->add_sub_command("rx", get_rx_cmd);

    // get rx state
    cli_command * get_rx_state_cmd = new cli_command();
    get_rx_cmd->add_sub_command("state", get_rx_state_cmd);

    cli_command_format * get_rx_state_fmt = new cli_command_format(
        "Send a GET_RX_STATE command to get Listener sink stream connection state.",
        &cmd_line::cmd_get_rx_state);
    get_rx_state_fmt->add_argument(new cli_argument_end_station(this, "d_e_s", DST_END_STATION_HELP));
    get_rx_state_fmt->add_argument(new cli_argument_int(this, "d_d_i", "the destination descriptor index"));
    get_rx_state_cmd->add_format(get_rx_state_fmt);

    // get tx
    cli_command * get_tx_cmd = new cli_command();
    get_cmd->add_sub_command("tx", get_tx_cmd);

    // get tx state
    cli_command * get_tx_state_cmd = new cli_command();
    get_tx_cmd->add_sub_command("state", get_tx_state_cmd);

    cli_command_format * get_tx_state_fmt = new cli_command_format(
        "Send a GET_TX_STATE command to get Talker source stream connection state.",
        &cmd_line::cmd_get_tx_state);
    get_tx_state_fmt->add_argument(new cli_argument_end_station(this, "s_e_s", SRC_END_STATION_HELP));
    get_tx_state_fmt->add_argument(new cli_argument_int(this, "s_d_i", "the source descriptor index"));
    get_tx_state_cmd->add_format(get_tx_state_fmt);

    // get tx connection
    cli_command * get_tx_connection_cmd = new cli_command();
    get_tx_cmd->add_sub_command("connection", get_tx_connection_cmd);

    cli_command_format * get_tx_connection_fmt = new cli_command_format(
        "Send a GET_TX_CONNECTION command with a notification id to get a specific\n"
        "Talker connection information.",
        &cmd_line::cmd_get_tx_connection);
    get_tx_connection_fmt->add_argument(new cli_argument_end_station(this, "s_e_s", SRC_END_STATION_HELP));
    get_tx_connection_fmt->add_argument(new cli_argument_int(this, "s_d_i", "the source descriptor index"));
    get_tx_connection_cmd->add_format(get_tx_connection_fmt);

    // entity
    cli_command * entity_cmd = new cli_command();
    commands.add_sub_command("entity", entity_cmd);

    // entity acquire
    cli_command * entity_acquire_cmd = new cli_command();
    entity_cmd->add_sub_command("acquire", entity_acquire_cmd);

    cli_command_format * entity_acquire_fmt = new cli_command_format(
        "Send a ACQUIRE_ENTITY command to obtain exclusive access to an entire Entity\n"
        "or a sub-tree of objects using the current setting.",
        &cmd_line::cmd_acquire_entity);
    entity_acquire_fmt->add_argument(new cli_argument_string(this, "a_e_f", "the Acquire Entity Flag",
                                                             "Valid Acquire Entity Flags are acquire, persistent, and release."));
    entity_acquire_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    entity_acquire_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                          "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                          "\"view all\" command."));
    entity_acquire_cmd->add_format(entity_acquire_fmt);

    // acquire
    cli_command * acquire_cmd = new cli_command();
    commands.add_sub_command("acquire", acquire_cmd);

    // acquire entity
    cli_command * acquire_entity_cmd = new cli_command();
    acquire_cmd->add_sub_command("entity", acquire_entity_cmd);

    acquire_entity_cmd->add_format(entity_acquire_fmt);

    // entity lock
    cli_command * entity_lock_cmd = new cli_command();
    entity_cmd->add_sub_command("lock", entity_lock_cmd);

    cli_command_format * entity_lock_fmt = new cli_command_format(
        "Send a LOCK_ENTITY command to provide short term exclusive access to the\n"
        "AVDECC Entity to perform atomic operations using the current setting.",
        &cmd_line::cmd_lock_entity);
    entity_lock_fmt->add_argument(new cli_argument_string(this, "l_e_f", "the Lock Entity Flag",
                                                          "Valid Lock Entity Flags are lock and unlock."));
    entity_lock_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    entity_lock_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                       "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                       "\"view all\" command."));
    entity_lock_cmd->add_format(entity_lock_fmt);

    // lock
    cli_command * lock_cmd = new cli_command();
    commands.add_sub_command("lock", lock_cmd);

    // lock entity
    cli_command * lock_entity_cmd = new cli_command();
    lock_cmd->add_sub_command("entity", entity_lock_cmd);

    lock_entity_cmd->add_format(entity_lock_fmt);

    // entity available
    cli_command * entity_available_cmd = new cli_command();
    entity_cmd->add_sub_command("available", entity_available_cmd);

    cli_command_format * entity_available_fmt = new cli_command_format(
        "Send a ENTITY_AVAILABLE command to determine if another AVDECC Entity is\n"
        "still alive and responding to commands.",
        &cmd_line::cmd_entity_avail);
    entity_available_cmd->add_format(entity_available_fmt);

    // entity reboot
    cli_command * entity_reboot_cmd = new cli_command();
    entity_cmd->add_sub_command("reboot", entity_reboot_cmd);

    cli_command_format * reboot_fmt = new cli_command_format(
        "Send a REBOOT command to an AVDECC Entity",
        &cmd_line::cmd_reboot);
    entity_reboot_cmd->add_format(reboot_fmt);

    // reboot
    cli_command * reboot_cmd = new cli_command();
    commands.add_sub_command("reboot", reboot_cmd);

    // reboot entity
    cli_command * reboot_entity_cmd = new cli_command();
    reboot_cmd->add_sub_command("entity", reboot_entity_cmd);

    reboot_entity_cmd->add_format(reboot_fmt);

    // controller
    cli_command * controller_cmd = new cli_command();
    commands.add_sub_command("controller", controller_cmd);

    // controller available
    cli_command * controller_available_cmd = new cli_command();
    controller_cmd->add_sub_command("available", controller_available_cmd);

    cli_command_format * controller_available_fmt = new cli_command_format(
        "Send a CONTROLLER_AVAILABLE command to determine if an AVDECC Controller is\n"
        "still alive.",
        &cmd_line::cmd_controller_avail);
    controller_available_cmd->add_format(controller_available_fmt);

    // read
    cli_command * read_cmd = new cli_command();
    commands.add_sub_command("read", read_cmd);

    // read descriptor
    cli_command * read_descriptor_cmd = new cli_command();
    read_cmd->add_sub_command("descriptor", read_descriptor_cmd);

    cli_command_format * read_descriptor_fmt = new cli_command_format(
        "Send a READ_DESCRIPTOR command to read a descriptor from an AVDECC Entity\n"
        "using the current setting.",
        &cmd_line::cmd_read_descriptor);
    read_descriptor_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    read_descriptor_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                           "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                           "\"view all\" command."));
    read_descriptor_cmd->add_format(read_descriptor_fmt);

    // set
    cli_command * set_cmd = new cli_command();
    commands.add_sub_command("set", set_cmd);

    // set name
    cli_command * set_name_cmd = new cli_command();
    set_cmd->add_sub_command("name", set_name_cmd);

    cli_command_format * set_name_state_fmt = new cli_command_format(
        "Send a SET_NAME command to change the value of a name field within a descriptor.",
        &cmd_line::cmd_set_name);
    set_name_state_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    set_name_state_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index"));
    set_name_state_fmt->add_argument(new cli_argument_string(this, "o_n", "the new name"));
    set_name_cmd->add_format(set_name_state_fmt);

    //set group name
    cli_command * set_group_name_cmd = new cli_command();
    set_cmd->add_sub_command("group_name", set_group_name_cmd);

    cli_command_format * set_group_name_state_fmt = new cli_command_format(
        "Send a SET_GROUP_NAME command to change the group name of an ENTITY descriptor.", &cmd_line::cmd_set_group_name);
    set_group_name_state_fmt->add_argument(new cli_argument_string(this, "o_n", "the new group name"));
    set_group_name_cmd->add_format(set_group_name_state_fmt);

    // set stream_format
    cli_command * set_stream_format_cmd = new cli_command();
    set_cmd->add_sub_command("stream_format", set_stream_format_cmd);

    cli_command_format * set_stream_format_fmt = new cli_command_format(
        "Send a SET_STREAM_FORMAT command to change the format of a stream",
        &cmd_line::cmd_set_stream_format);
    set_stream_format_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."));
    set_stream_format_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                             "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                             "\"view all\" command."));
    set_stream_format_fmt->add_argument(new cli_argument_string(this, "fmt", "the stream format input type",
                                                                "Enter index from List or full 1722.1 Format Name\n"
                                                                "To see a list of valid stream formats, enter \n"
                                                                "\"view stream_formats\" command."));

    set_stream_format_cmd->add_format(set_stream_format_fmt);

    // get audio_map
    cli_command * get_audio_map_cmd = new cli_command();
    audio_mappings->add_sub_command("get", get_audio_map_cmd);

    cli_command_format * get_audio_map_fmt = new cli_command_format(
        "Send a GET_AUDIO_MAP command to fetch the dynamic mapping between the Audio Clusters\n"
        "and the input or output streams.",
        &cmd_line::cmd_get_audio_map);
    get_audio_map_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                            "Valid descriptor types are STREAM_PORT_INPUT and STREAM_PORT_OUTPUT."));
    get_audio_map_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                         "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                         "\"view all\" command."));
    get_audio_map_fmt->add_argument(new cli_argument_int(this, "m_i", "the map index"));
    get_audio_map_cmd->add_format(get_audio_map_fmt);

    // get stream_format
    cli_command * get_stream_format_cmd = new cli_command();
    get_cmd->add_sub_command("stream_format", get_stream_format_cmd);

    cli_command_format * get_stream_format_fmt = new cli_command_format(
        "Send a GET_STREAM_FORMAT command to display the current format of a stream\n"
        "using the current setting.",
        &cmd_line::cmd_get_stream_format);
    get_stream_format_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    get_stream_format_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                             "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                             "\"view all\" command."));
    get_stream_format_cmd->add_format(get_stream_format_fmt);

    // set stream_info
    cli_command * set_stream_info_cmd = new cli_command();
    set_cmd->add_sub_command("stream_info", set_stream_info_cmd);

    cli_command_format * set_stream_info_fmt = new cli_command_format(
        "Use the SET_STREAM_INFO to change the current setting.",
        &cmd_line::cmd_set_stream_info);
    set_stream_info_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                              "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."));
    set_stream_info_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                           "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                           "\"view all\" command."));
    set_stream_info_fmt->add_argument(new cli_argument_string(this, "flag", "the setting to adjust"));
    set_stream_info_fmt->add_argument(new cli_argument_string(this, "value", "the value to set"));
    set_stream_info_cmd->add_format(set_stream_info_fmt);

    // get stream_info
    cli_command * get_stream_info_cmd = new cli_command();
    get_cmd->add_sub_command("stream_info", get_stream_info_cmd);

    cli_command_format * get_stream_info_fmt = new cli_command_format(
        "Display the GET_STREAM_INFO result using the current setting.",
        &cmd_line::cmd_get_stream_info);
    get_stream_info_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                              "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."));
    get_stream_info_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                           "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                           "\"view all\" command."));
    get_stream_info_cmd->add_format(get_stream_info_fmt);

    // unsolicited
    cli_command * unsolicited_cmd = new cli_command();
    commands.add_sub_command("unsolicited", unsolicited_cmd);

    //register unsolicited notifications
    cli_command * register_unsolicited_notif = new cli_command();
    unsolicited_cmd->add_sub_command("register", register_unsolicited_notif);

    cli_command_format * register_unsolicited_fmt = new cli_command_format(
        "Add the controller as being interested in receiving unsolicited response notifications.",
        &cmd_line::cmd_register_unsolicited_notif);
    register_unsolicited_notif->add_format(register_unsolicited_fmt);

    //deregister unsolicited notifications
    cli_command * deregister_unsolicited_notif = new cli_command();
    unsolicited_cmd->add_sub_command("deregister", deregister_unsolicited_notif);

    cli_command_format * deregister_unsolicited_fmt = new cli_command_format(
        "Remove the controller as being interested in receiving unsolicited response notifications.",
        &cmd_line::cmd_deregister_unsolicited_notif);
    deregister_unsolicited_notif->add_format(deregister_unsolicited_fmt);

    // set sampling_rate
    cli_command * set_sampling_rate_cmd = new cli_command();
    set_cmd->add_sub_command("sampling_rate", set_sampling_rate_cmd);

    cli_command_format * set_sampling_rate_fmt = new cli_command_format(
        "Send a SET_SAMPLING_RATE command to change the sampling rate of a port or unit.",
        &cmd_line::cmd_set_sampling_rate);
    set_sampling_rate_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                "Valid descriptor types are AUDIO_UNIT, VIDEO_CLUSTER, SENSOR_CLUSTER."));
    set_sampling_rate_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                             "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                             "\"view all\" command."));
    set_sampling_rate_fmt->add_argument(new cli_argument_int(this, "rate", "the new rate to set"));
    set_sampling_rate_cmd->add_format(set_sampling_rate_fmt);

    // get sampling_rate
    cli_command * get_sampling_rate_cmd = new cli_command();
    get_cmd->add_sub_command("sampling_rate", get_sampling_rate_cmd);

    cli_command_format * get_sampling_rate_fmt = new cli_command_format(
        "Send a GET_SAMPLING_RATE command to get the current sampling rate of a\n"
        "port or unit.",
        &cmd_line::cmd_get_sampling_rate);
    get_sampling_rate_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                                "Valid descriptor types are AUDIO_UNIT, VIDEO_CLUSTER, SENSOR_CLUSTER."));
    get_sampling_rate_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                             "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                             "\"view all\" command."));
    get_sampling_rate_cmd->add_format(get_sampling_rate_fmt);

    // get counters
    cli_command * get_counters_cmd = new cli_command();
    get_cmd->add_sub_command("counters", get_counters_cmd);

    cli_command_format * get_counters_fmt = new cli_command_format(
        "Send a GET_COUNTERS command to get a current counter for a descriptor.\n"
        "port or unit.",
        &cmd_line::cmd_get_counters);
    get_counters_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                           "Valid descriptor types are AVB_INTERFACE, CLOCK_DOMAIN, STREAM_INPUT."));
    get_counters_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                        "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                        "\"view all\" command."));
    get_counters_cmd->add_format(get_counters_fmt);

    // set clock_source
    cli_command * set_clock_source_cmd = new cli_command();
    set_cmd->add_sub_command("clock_source", set_clock_source_cmd);

    cli_command_format * set_clock_source_fmt = new cli_command_format(
        "Send a SET_CLOCK_SOURCE command to change the clock source of a clock domain.",
        &cmd_line::cmd_set_clock_source);
    set_clock_source_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                               "Valid descriptor type is CLOCK_DOMAIN."));
    set_clock_source_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                            "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                            "\"view all\" command."));
    set_clock_source_fmt->add_argument(new cli_argument_int(this, "c_s_i", "the Clock Source Index"));
    set_clock_source_cmd->add_format(set_clock_source_fmt);

    // get clock_source
    cli_command * get_clock_source_cmd = new cli_command();
    get_cmd->add_sub_command("clock_source", get_clock_source_cmd);

    cli_command_format * get_clock_source_fmt = new cli_command_format(
        "Send a GET_CLOCK_SOURCE command to change the clock source of a clock domain.",
        &cmd_line::cmd_get_clock_source);
    get_clock_source_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                               "Valid descriptor type is CLOCK_DOMAIN."));
    get_clock_source_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                            "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                            "\"view all\" command."));
    get_clock_source_cmd->add_format(get_clock_source_fmt);

    // get avb_info
    cli_command * get_avb_info_cmd = new cli_command();
    get_cmd->add_sub_command("avb_info", get_avb_info_cmd);

    cli_command_format * get_avb_info_fmt = new cli_command_format(
        "Send a GET_AVB_INFO command to get the dynamic AVB information for an AVB_INTERFACE.",
        &cmd_line::cmd_get_avb_info);
    get_avb_info_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                           "Valid descriptor type is AVB_INTERFACE."));
    get_avb_info_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                        "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                        "\"view all\" command."));
    get_avb_info_cmd->add_format(get_avb_info_fmt);

    // start
    cli_command * start_cmd = new cli_command();
    commands.add_sub_command("start", start_cmd);

    // start streaming
    cli_command * start_streaming_cmd = new cli_command();
    start_cmd->add_sub_command("streaming", start_streaming_cmd);

    cli_command_format * start_streaming_fmt = new cli_command_format(
        "Send a START_STREAMING command to start streaming on a previously connected\n"
        "stream that was connected via ACMP or has previously been stopped with the\n"
        "STOP_STREAMING command.",
        &cmd_line::cmd_start_streaming);
    start_streaming_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                              "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."));
    start_streaming_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                           "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                           "\"view all\" command."));
    start_streaming_cmd->add_format(start_streaming_fmt);

    // stop
    cli_command * stop_cmd = new cli_command();
    commands.add_sub_command("stop", stop_cmd);

    // stop streaming
    cli_command * stop_streaming_cmd = new cli_command();
    stop_cmd->add_sub_command("streaming", stop_streaming_cmd);

    cli_command_format * stop_streaming_fmt = new cli_command_format(
        "Send a START_STREAMING command to start streaming on a previously connected\n"
        "stream that was connected via ACMP or has previously been stopped with the\n"
        "STOP_STREAMING command.",
        &cmd_line::cmd_stop_streaming);
    stop_streaming_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type",
                                                             "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."));
    stop_streaming_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                          "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                          "\"view all\" command."));
    stop_streaming_cmd->add_format(stop_streaming_fmt);

    // identify
    cli_command * identify_cmd = new cli_command();
    commands.add_sub_command("identify", identify_cmd);

    // identify on
    cli_command * identify_on_cmd = new cli_command();
    identify_cmd->add_sub_command("on", identify_on_cmd);

    cli_command_format * identify_on_fmt = new cli_command_format(
        "Send an IDENTIFY packet to enable identification.",
        &cmd_line::cmd_identify_on);
    identify_on_fmt->add_argument(new cli_argument_end_station(this, "e_s", END_STATION_HELP));
    identify_on_cmd->add_format(identify_on_fmt);

    // identify off
    cli_command * identify_off_cmd = new cli_command();
    identify_cmd->add_sub_command("off", identify_off_cmd);

    cli_command_format * identify_off_fmt = new cli_command_format(
        "Send an IDENTIFY packet to disable identification.",
        &cmd_line::cmd_identify_off);
    identify_off_fmt->add_argument(new cli_argument_end_station(this, "e_s", END_STATION_HELP));
    identify_off_cmd->add_format(identify_off_fmt);

    // path
    cli_command * path_cmd = new cli_command();
    commands.add_sub_command("path", path_cmd);

    cli_command_format * set_path_fmt = new cli_command_format(
        "Change the location of the redirected output file.",
        &cmd_line::cmd_set_path);
    set_path_fmt->add_argument(new cli_argument_string(this, "path", "the new path to set"));
    path_cmd->add_format(set_path_fmt);

    cli_command_format * show_path_fmt = new cli_command_format(
        "Display the location of the redirected output file.",
        &cmd_line::cmd_show_path);
    path_cmd->add_format(show_path_fmt);

    // clr
    cli_command * clr_cmd = new cli_command();
    commands.add_sub_command("clr", clr_cmd);

    cli_command_format * clr_fmt = new cli_command_format(
        "Clear the console screen.",
        &cmd_line::cmd_clr);
    clr_cmd->add_format(clr_fmt);

    // quit
    cli_command * quit_cmd = new cli_command();
    commands.add_sub_command("quit", quit_cmd);
    commands.add_sub_command("q", quit_cmd);

    cli_command_format * quit_fmt = new cli_command_format(
        "Quit the controller.",
        &cmd_line::cmd_quit);
    quit_cmd->add_format(quit_fmt);

    // param
    cli_command * param_cmd = new cli_command();
    commands.add_sub_command("param", param_cmd);

    cli_command_format * param_fmt = new cli_command_format(
        "Param",
        &cmd_line::cmd_connect_rx);
    param_fmt->add_argument(new cli_argument_end_station(this, "e_s_i", END_STATION_HELP,
                                                         "To see a list of valid End Stations, enter \"list\" command."));
    param_fmt->add_argument(new cli_argument_int(this, "e_i", "the Entity index"));
    param_fmt->add_argument(new cli_argument_int(this, "c_i", "the Configuration index"));
    param_fmt->add_argument(new cli_argument_string(this, "d_t", "the descriptor type"));
    param_fmt->add_argument(new cli_argument_int(this, "d_i", "the descriptor index",
                                                 "To see a list of valid descriptor types and corresponding indexes, enter\n"
                                                 "\"view all\" command."));
    param_cmd->add_format(param_fmt);

    // upgrade
    cli_command * upgrade_cmd = new cli_command();
    commands.add_sub_command("upgrade", upgrade_cmd);

    cli_command_format * upgrade_cmd_fmt = new cli_command_format(
        "Perform the EFU process on the end station to upgrade a firmware image",
        &cmd_line::cmd_firmware_upgrade);
    upgrade_cmd_fmt->add_argument(new cli_argument_string(this, "upgrade_image_path", "the path to the upgrade image file"));
    upgrade_cmd->add_format(upgrade_cmd_fmt);
}

int cmd_line::cmd_help_all(int total_matched, std::vector<cli_argument *> args)
{
    commands.print_help_all("", 1);
    return 0;
}

int cmd_line::cmd_help_one(int total_matched, std::vector<cli_argument *> args)
{
    std::vector<std::string> tmp = args[0]->get_all_value_str();
    if (tmp[0] == "-a")
    {
        commands.print_help_all("", UINT_MAX);
    }
    else
    {
        std::string prefix;
        std::queue<std::string, std::deque<std::string>> args_queue(std::deque<std::string>(tmp.begin(), tmp.end()));
        const cli_command * cmd = commands.get_sub_command(args_queue, prefix);
        if (cmd)
        {
            cmd->print_help_details(prefix);
        }
        else
        {
            printf("Could not find command to print help for. Matched up to '%s'\n", prefix.c_str());
        }
    }
    return 0;
}

int cmd_line::cmd_quit(int total_matched, std::vector<cli_argument *> args)
{
    return 1;
}

int cmd_line::cmd_version(int total_matched, std::vector<cli_argument *> args)
{
    atomic_cout << "AVDECC Controller version: " << controller_obj->get_version() << std::endl;
    return 0;
}

int cmd_line::cmd_list(int total_matched, std::vector<cli_argument *> args)
{
    atomic_cout << "\n"
                << "End Station"
                << "  |  "
                << "Name" << std::setw(21) << "  |  "
                << "Entity ID" << std::setw(12) << "  |  "
                << "Firmware Version"
                << "  |  "
                << "MAC" << std::endl;
    atomic_cout << std::string(100, '-') << std::endl;

    for (unsigned int i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(i);

        if (end_station)
        {
            uint64_t end_station_entity_id = end_station->entity_id();
            avdecc_lib::entity_descriptor_response * ent_desc_resp = NULL;
            if (end_station->entity_desc_count())
            {
                uint16_t current_entity = end_station->get_current_entity_index();
                ent_desc_resp = end_station->get_entity_desc_by_index(current_entity)->get_entity_response();
            }
            const char * end_station_name = "";
            const char * fw_ver = "";
            if (ent_desc_resp)
            {
                end_station_name = (const char *)ent_desc_resp->entity_name();
                fw_ver = (const char *)ent_desc_resp->firmware_version();
            }
            uint64_t end_station_mac = end_station->mac();
            atomic_cout << (std::stringstream() << end_station->get_connection_status()
                                                << std::setw(10) << std::dec << std::setfill(' ') << i << "  |  "
                                                << std::setw(20) << std::hex << std::setfill(' ') << (ent_desc_resp ? end_station_name : "UNKNOWN") << "  |  0x"
                                                << std::setw(16) << std::hex << std::setfill('0') << end_station_entity_id << "  |  "
                                                << std::setw(16) << std::hex << std::setfill(' ') << (ent_desc_resp ? fw_ver : "UNKNOWN") << "  |  "
                                                << std::setw(12) << std::hex << std::setfill('0') << end_station_mac)
                               .rdbuf()
                        << std::endl;
            delete (ent_desc_resp);
        }
    }

    atomic_cout << "\nC - End Station Connected." << std::endl;
    atomic_cout << "D - End Station Disconnected." << std::endl;

    return 0;
}

int cmd_line::cmd_view_media_clock(int total_matched, std::vector<cli_argument *> args)
{
    uint8_t * desc_obj_name;
    uint16_t desc_type_value = 0;
    uint16_t desc_index = 0;
    bool is_clock_sync_source_set = false;

    atomic_cout << "\n"
                << "End Station"
                << "  " << std::setw(20) << "Descriptor Name"
                << "  "
                << std::setw(18) << "Descriptor Type"
                << "  " << std::setw(18) << "Descriptor Index" << std::endl;
    atomic_cout << "------------------------------------------------------------------------------" << std::endl;

    for (unsigned int i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::configuration_descriptor * configuration = controller_obj->get_current_config_desc(i, false);

        if (configuration)
        {
            for (unsigned int j = 0; j < configuration->stream_input_desc_count(); j++)
            {
                avdecc_lib::stream_input_descriptor * stream_input_desc = configuration->get_stream_input_desc_by_index(j);
                if (stream_input_desc)
                {
                    avdecc_lib::stream_input_descriptor_response * stream_input_resp_ref = stream_input_desc->get_stream_input_response();
                    is_clock_sync_source_set = stream_input_resp_ref->stream_flags_clock_sync_source();
                    if (is_clock_sync_source_set)
                    {

                        desc_obj_name = stream_input_resp_ref->object_name();
                        desc_type_value = stream_input_desc->descriptor_type();
                        desc_index = stream_input_desc->descriptor_index();

                        atomic_cout << std::setw(8) << i << std::setw(5) << "" << std::setw(20) << desc_obj_name
                                    << "  " << std::setw(18) << avdecc_lib::utility::aem_desc_value_to_name(desc_type_value)
                                    << "  " << std::setw(16) << std::dec << desc_index << std::endl;
                    }
                    delete (stream_input_resp_ref);
                }
            }

            for (unsigned int j = 0; j < configuration->stream_output_desc_count(); j++)
            {
                avdecc_lib::stream_output_descriptor * stream_output_desc = configuration->get_stream_output_desc_by_index(j);
                if (stream_output_desc)
                {
                    avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = stream_output_desc->get_stream_output_response();
                    is_clock_sync_source_set = stream_output_resp_ref->stream_flags_clock_sync_source();
                    if (is_clock_sync_source_set)
                    {
                        desc_obj_name = stream_output_resp_ref->object_name();
                        desc_type_value = stream_output_desc->descriptor_type();
                        desc_index = stream_output_desc->descriptor_index();

                        atomic_cout << std::setw(8) << i << std::setw(5) << "" << std::setw(20) << desc_obj_name
                                    << "  " << std::setw(18) << std::hex << avdecc_lib::utility::aem_desc_value_to_name(desc_type_value)
                                    << "  " << std::setw(16) << std::dec << desc_index << std::endl;
                    }
                    delete (stream_output_resp_ref);
                }
            }
        }
    }

    return 0;
}

int cmd_line::cmd_show_select(int total_matched, std::vector<cli_argument *> args)
{
    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor_response * entity_desc_resp;
    if (get_current_end_station(&end_station))
        return 0;

    uint16_t current_entity = end_station->get_current_entity_index();
    uint16_t current_config = end_station->get_current_config_index();
    entity_desc_resp = end_station->get_entity_desc_by_index(current_entity)->get_entity_response();

    atomic_cout << "Current setting" << std::endl;
    atomic_cout << "\tEnd Station: " << std::dec << current_end_station << " (" << entity_desc_resp->entity_name()
                << ", "
                << "0x" << std::setw(16) << std::hex << std::setfill('0') << end_station->entity_id()
                << ")" << std::endl;
    atomic_cout << "\tEntity: " << std::dec << current_entity << std::endl;
    atomic_cout << "\tConfiguration: " << std::dec << current_config << std::endl;

    delete entity_desc_resp;
    return 0;
}

int cmd_line::cmd_select(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t new_end_station = args[0]->get_value_uint();
    uint16_t new_entity = args[1]->get_value_int();
    uint16_t new_config = args[2]->get_value_int();
    do_select(new_end_station, new_entity, new_config);
    return 0;
}

int cmd_line::do_select(uint32_t new_end_station, uint16_t new_entity, uint16_t new_config)
{
    if (is_setting_valid(new_end_station, new_entity, new_config)) // Check if the new setting is valid
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(new_end_station);
        uint16_t current_entity = end_station->get_current_entity_index();
        uint16_t current_config = end_station->get_current_config_index();
        avdecc_lib::entity_descriptor_response * entity_desc_resp = end_station->get_entity_desc_by_index(current_entity)->get_entity_response();

        uint8_t * end_station_name = entity_desc_resp->entity_name();

        if ((current_end_station == new_end_station) && (current_entity == new_entity) && (current_config == new_config))
        {
            atomic_cout << "Same setting" << std::endl;
            atomic_cout << "\tEnd Station Index: " << std::dec << current_end_station << " (" << end_station_name << ")" << std::endl;
            atomic_cout << "\tEntity Index: " << std::dec << current_entity << std::endl;
            atomic_cout << "\tConfiguration Index: " << std::dec << current_config << std::endl;
        }
        else
        {
            current_end_station = new_end_station;
            end_station->set_current_entity_index(new_entity);
            end_station->set_current_config_index(new_config);
            atomic_cout << "New setting" << std::endl;
            atomic_cout << "\tEnd Station Index: " << std::dec << current_end_station << " (" << end_station_name << ")" << std::endl;
            atomic_cout << "\tEntity Index: " << std::dec << current_entity << std::endl;
            atomic_cout << "\tConfiguration Index: " << std::dec << current_config << std::endl;
        }
        delete entity_desc_resp;
    }
    else
    {
        atomic_cout << "Invalid new setting" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_log_level(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t new_log_level = args[0]->get_value_int();
    if (new_log_level < avdecc_lib::TOTAL_NUM_OF_LOGGING_LEVELS)
    {
        controller_obj->set_logging_level(new_log_level);
    }
    else
    {
        atomic_cout << "Invalid new log level" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_log(int total_matched, std::vector<cli_argument *> args)
{
    std::string file = log_path + "/" + args[0]->get_value_str() + ".txt";
    try
    {
        ofstream_ref.open(file);
        ofstream_ref.good();
        std::cout.rdbuf(ofstream_ref.rdbuf());
        output_redirected = true;
        atomic_cout << "Re-directing logging to " << file << std::endl;
    }
    catch (std::ofstream::failure e)
    {
        atomic_cout << "ofstream exception" << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}

int cmd_line::cmd_unlog(int total_matched, std::vector<cli_argument *> args)
{
    std::cout.rdbuf(cout_buf);
    ofstream_ref.close();
    output_redirected = false;

    return 0;
}

void cmd_line::print_desc_type_index_name_row(avdecc_lib::descriptor_base & desc,
                                              avdecc_lib::configuration_descriptor & configuration,
                                              size_t localized_description,
                                              const uint8_t * object_name,
                                              avdecc_lib::locale_descriptor & locale)
{
    //const uint8_t localized_string_max_index = 7;

    atomic_cout << std::setw(20) << avdecc_lib::utility::aem_desc_value_to_name(desc.descriptor_type())
                << "   " << std::setw(16) << std::dec << desc.descriptor_index();

    if ((desc.descriptor_type() == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT) ||
        (desc.descriptor_type() == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT) ||
        (desc.descriptor_type() == avdecc_lib::AEM_DESC_AUDIO_MAP))
    {
        atomic_cout << "   " << std::endl;
    }
    else
    {
        if (object_name && object_name[0] != '\0')
        {
            atomic_cout << "   " << std::setw(20) << std::hex << object_name << std::endl;
        }
        else
        {
            size_t string_desc_index;
            size_t string_index;
            int ret = configuration.get_strings_desc_string_by_reference(localized_description,
                                                                         string_desc_index, string_index);
            if (ret == 0)
            {
                avdecc_lib::strings_descriptor * desc = configuration.get_strings_desc_by_index(string_desc_index);
                avdecc_lib::strings_descriptor_response * strings_resp_ref = desc->get_strings_response();
                atomic_cout << "   " << std::setw(20) << std::hex << strings_resp_ref->get_string_by_index(string_index) << std::endl;
                delete strings_resp_ref;
            }
            else
            {
                atomic_cout << "   " << std::endl;
            }
        }
    }
}

int cmd_line::cmd_view_all(int total_matched, std::vector<cli_argument *> args)
{
    uint8_t * obj_name = NULL;

    for (uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(i);
        avdecc_lib::entity_descriptor * entity;
        avdecc_lib::configuration_descriptor * configuration;
        if (get_current_entity_and_descriptor(end_station, &entity, &configuration))
            continue;

        avdecc_lib::entity_descriptor_response * entity_resp = entity->get_entity_response();

        obj_name = entity_resp->entity_name();
        atomic_cout << "\nEnd Station: " << i << " (" << obj_name << ")" << std::endl;
        atomic_cout << std::setw(20) << "Descriptor Type"
                    << "   " << std::setw(16)
                    << "Descriptor Index"
                    << "   " << std::setw(20) << "Descriptor Name" << std::endl;
        atomic_cout << "------------------------------------------------------------------------------" << std::endl;
        delete entity_resp;

        if ((configuration->locale_desc_count() == 0) || (configuration->strings_desc_count() == 0))
            continue;

        avdecc_lib::locale_descriptor * locale = configuration->get_locale_desc_by_index(0);

        switch (0)
        {
        case avdecc_lib::AEM_DESC_ENTITY:
        {
            avdecc_lib::entity_descriptor_response * entity_resp = entity->get_entity_response();
            atomic_cout << std::setw(20) << std::hex << avdecc_lib::utility::aem_desc_value_to_name(entity->descriptor_type())
                        << "   " << std::setw(16) << std::dec << entity->descriptor_index()
                        << "   " << std::setw(20) << std::hex << entity_resp->entity_name() << std::endl;
            delete entity_resp;
        }

        case avdecc_lib::AEM_DESC_CONFIGURATION:
        {
            atomic_cout << std::setw(20) << avdecc_lib::utility::aem_desc_value_to_name(configuration->descriptor_type())
                        << "   " << std::setw(16) << std::dec << configuration->descriptor_index()
                        << "   " << std::setw(20) << std::hex << configuration->object_name() << std::endl;
            atomic_cout << "\nTop Level Descriptors" << std::endl;
        }

        case avdecc_lib::AEM_DESC_AUDIO_UNIT:
            for (unsigned int j = 0; j < configuration->audio_unit_desc_count(); j++)
            {
                avdecc_lib::audio_unit_descriptor * audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(j);
                avdecc_lib::audio_unit_descriptor_response * audio_unit_resp_ref = audio_unit_desc_ref->get_audio_unit_response();
                print_desc_type_index_name_row(*audio_unit_desc_ref,
                                               *configuration,
                                               audio_unit_resp_ref->localized_description(),
                                               audio_unit_resp_ref->object_name(),
                                               *locale);
                delete audio_unit_resp_ref;
            }

        case avdecc_lib::AEM_DESC_STREAM_INPUT:
            for (unsigned int j = 0; j < configuration->stream_input_desc_count(); j++)
            {
                avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(j);
                avdecc_lib::stream_input_descriptor_response * stream_input_resp_ref = stream_input_desc_ref->get_stream_input_response();
                print_desc_type_index_name_row(*stream_input_desc_ref,
                                               *configuration,
                                               stream_input_resp_ref->localized_description(),
                                               stream_input_resp_ref->object_name(),
                                               *locale);
                delete stream_input_resp_ref;
            }

        case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
            for (unsigned int j = 0; j < configuration->stream_output_desc_count(); j++)
            {
                avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(j);
                avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = stream_output_desc_ref->get_stream_output_response();
                print_desc_type_index_name_row(*stream_output_desc_ref,
                                               *configuration,
                                               stream_output_resp_ref->localized_description(),
                                               stream_output_resp_ref->object_name(),
                                               *locale);
                delete stream_output_resp_ref;
            }

        case avdecc_lib::AEM_DESC_JACK_INPUT:
            for (unsigned int j = 0; j < configuration->jack_input_desc_count(); j++)
            {
                avdecc_lib::jack_input_descriptor * jack_input_desc_ref = configuration->get_jack_input_desc_by_index(j);
                avdecc_lib::jack_input_descriptor_response * jack_input_resp_ref = jack_input_desc_ref->get_jack_input_response();
                print_desc_type_index_name_row(*jack_input_desc_ref,
                                               *configuration,
                                               jack_input_resp_ref->localized_description(),
                                               jack_input_resp_ref->object_name(),
                                               *locale);
                delete jack_input_resp_ref;
            }

        case avdecc_lib::AEM_DESC_JACK_OUTPUT:
            for (unsigned int j = 0; j < configuration->jack_output_desc_count(); j++)
            {
                avdecc_lib::jack_output_descriptor * jack_output_desc_ref = configuration->get_jack_output_desc_by_index(j);
                avdecc_lib::jack_output_descriptor_response * jack_output_resp_ref = jack_output_desc_ref->get_jack_output_response();
                print_desc_type_index_name_row(*jack_output_desc_ref,
                                               *configuration,
                                               jack_output_resp_ref->localized_description(),
                                               jack_output_resp_ref->object_name(),
                                               *locale);
                delete jack_output_resp_ref;
            }

        case avdecc_lib::AEM_DESC_AVB_INTERFACE:
            for (unsigned int j = 0; j < configuration->avb_interface_desc_count(); j++)
            {
                avdecc_lib::avb_interface_descriptor * avb_interface_desc_ref = configuration->get_avb_interface_desc_by_index(j);
                avdecc_lib::avb_interface_descriptor_response * avb_interface_resp_ref = avb_interface_desc_ref->get_avb_interface_response();
                print_desc_type_index_name_row(*avb_interface_desc_ref,
                                               *configuration,
                                               avb_interface_resp_ref->localized_description(),
                                               avb_interface_resp_ref->object_name(),
                                               *locale);
                delete avb_interface_resp_ref;
            }

        case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
            for (unsigned int j = 0; j < configuration->clock_source_desc_count(); j++)
            {
                avdecc_lib::clock_source_descriptor * clk_src_desc_ref = configuration->get_clock_source_desc_by_index(j);
                avdecc_lib::clock_source_descriptor_response * clk_src_resp_ref = clk_src_desc_ref->get_clock_source_response();
                print_desc_type_index_name_row(*clk_src_desc_ref,
                                               *configuration,
                                               clk_src_resp_ref->localized_description(),
                                               clk_src_resp_ref->object_name(),
                                               *locale);
                delete clk_src_resp_ref;
            }

        case avdecc_lib::AEM_DESC_MEMORY_OBJECT:
            for (unsigned int j = 0; j < configuration->memory_object_desc_count(); j++)
            {
                avdecc_lib::memory_object_descriptor * mem_obj_desc_ref = configuration->get_memory_object_desc_by_index(j);
                avdecc_lib::memory_object_descriptor_response * mem_obj_resp_ref = mem_obj_desc_ref->get_memory_object_response();
                print_desc_type_index_name_row(*mem_obj_desc_ref,
                                               *configuration,
                                               mem_obj_resp_ref->localized_description(),
                                               mem_obj_resp_ref->object_name(),
                                               *locale);
                delete mem_obj_resp_ref;
            }

        case avdecc_lib::AEM_DESC_LOCALE:
            for (unsigned int j = 0; j < configuration->locale_desc_count(); j++)
            {
                avdecc_lib::locale_descriptor * locale_def_ref = configuration->get_locale_desc_by_index(j);
                avdecc_lib::locale_descriptor_response * locale_resp_ref = locale_def_ref->get_locale_response();
                atomic_cout << std::setw(20) << avdecc_lib::utility::aem_desc_value_to_name(locale_def_ref->descriptor_type())
                            << "   " << std::setw(16) << std::hex << locale_def_ref->descriptor_index()
                            << "   " << std::setw(20) << std::hex << locale_resp_ref->locale_identifier() << std::endl;
                delete locale_resp_ref;
            }

        //            case avdecc_lib::AEM_DESC_STRINGS:
        //                for(int j = 0; j < configuration->strings_desc_count(); j++)
        //                {
        //                    avdecc_lib::strings_descriptor *strings = configuration->get_strings_desc_by_index(j);
        //                    print_desc_type_index_name_row(*strings, *strings, *locale);
        //                }

        case avdecc_lib::AEM_DESC_STREAM_PORT_INPUT:
            for (unsigned int j = 0; j < configuration->stream_port_input_desc_count(); j++)
            {
                avdecc_lib::stream_port_input_descriptor * stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(j);
                avdecc_lib::stream_port_input_descriptor_response * input_resp_ref = stream_port_input_desc_ref->get_stream_port_input_response();
                print_desc_type_index_name_row(*stream_port_input_desc_ref,
                                               *configuration,
                                               stream_port_input_desc_ref->localized_description(),
                                               input_resp_ref->object_name(),
                                               *locale);
                delete input_resp_ref;
            }

        case avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT:
            for (unsigned int j = 0; j < configuration->stream_port_output_desc_count(); j++)
            {
                avdecc_lib::stream_port_output_descriptor * stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(j);
                avdecc_lib::stream_port_output_descriptor_response * output_resp_ref = stream_port_output_desc_ref->get_stream_port_output_response();
                print_desc_type_index_name_row(*stream_port_output_desc_ref,
                                               *configuration,
                                               stream_port_output_desc_ref->localized_description(),
                                               output_resp_ref->object_name(),
                                               *locale);
                delete output_resp_ref;
            }

        case avdecc_lib::AEM_DESC_AUDIO_CLUSTER:
            for (unsigned int j = 0; j < configuration->audio_cluster_desc_count(); j++)
            {
                avdecc_lib::audio_cluster_descriptor * audio_cluster_desc_ref = configuration->get_audio_cluster_desc_by_index(j);
                avdecc_lib::audio_cluster_descriptor_response * audio_cluster_resp_ref = audio_cluster_desc_ref->get_audio_cluster_response();
                print_desc_type_index_name_row(*audio_cluster_desc_ref,
                                               *configuration,
                                               audio_cluster_resp_ref->localized_description(),
                                               audio_cluster_resp_ref->object_name(),
                                               *locale);
                delete audio_cluster_resp_ref;
            }

        case avdecc_lib::AEM_DESC_AUDIO_MAP:
            for (unsigned int j = 0; j < configuration->audio_map_desc_count(); j++)
            {
                avdecc_lib::audio_map_descriptor * audio_map_desc_ref = configuration->get_audio_map_desc_by_index(j);
                avdecc_lib::audio_map_descriptor_response * audio_map_resp_ref = audio_map_desc_ref->get_audio_map_response();
                print_desc_type_index_name_row(*audio_map_desc_ref,
                                               *configuration,
                                               audio_map_desc_ref->localized_description(),
                                               audio_map_resp_ref->object_name(),
                                               *locale);
                delete audio_map_resp_ref;
            }

        case avdecc_lib::AEM_DESC_EXTERNAL_PORT_INPUT:
            for (unsigned int j = 0; j < configuration->external_port_input_desc_count(); j++)
            {
                avdecc_lib::external_port_input_descriptor * desc_ref = configuration->get_external_port_input_desc_by_index(j);
                avdecc_lib::external_port_input_descriptor_response * resp_ref = desc_ref->get_external_port_input_response();
                print_desc_type_index_name_row(*desc_ref,
                                               *configuration,
                                               desc_ref->localized_description(),
                                               resp_ref->object_name(),
                                               *locale);
                delete resp_ref;
            }

        case avdecc_lib::AEM_DESC_EXTERNAL_PORT_OUTPUT:
            for (unsigned int j = 0; j < configuration->external_port_output_desc_count(); j++)
            {
                avdecc_lib::external_port_output_descriptor * desc_ref = configuration->get_external_port_output_desc_by_index(j);
                avdecc_lib::external_port_output_descriptor_response * resp_ref = desc_ref->get_external_port_output_response();
                print_desc_type_index_name_row(*desc_ref,
                                               *configuration,
                                               desc_ref->localized_description(),
                                               resp_ref->object_name(),
                                               *locale);
                delete resp_ref;
            }

        case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
            for (unsigned int j = 0; j < configuration->clock_domain_desc_count(); j++)
            {
                avdecc_lib::clock_domain_descriptor * clk_domain_desc_ref = configuration->get_clock_domain_desc_by_index(j);
                avdecc_lib::clock_domain_descriptor_response * clk_domain_resp_ref = clk_domain_desc_ref->get_clock_domain_response();
                print_desc_type_index_name_row(*clk_domain_desc_ref,
                                               *configuration,
                                               clk_domain_resp_ref->localized_description(),
                                               clk_domain_resp_ref->object_name(),
                                               *locale);
                delete clk_domain_resp_ref;
            }
        case avdecc_lib::AEM_DESC_CONTROL:
            for (unsigned int j = 0; j < configuration->control_desc_count(); j++)
            {
                avdecc_lib::control_descriptor * control_desc_ref = configuration->get_control_desc_by_index(j);
                avdecc_lib::control_descriptor_response * control_resp_ref = control_desc_ref->get_control_response();
                print_desc_type_index_name_row(*control_desc_ref,
                                               *configuration,
                                               control_resp_ref->localized_description(),
                                               control_resp_ref->object_name(),
                                               *locale);
                delete control_resp_ref;
            }
            break;
        }
    }

    return 0;
}

int cmd_line::cmd_view_stream_formats(int total_matched, std::vector<cli_argument *> args)
{
    unsigned int format_table_size = avdecc_lib::utility::get_ieee1722_format_table_size();

    atomic_cout << "   "
                << "Stream Type" << std::setw(4) << "|"
                << "    "
                << "Stream Description" << std::setw(10) << std::endl;
    atomic_cout << "----------------------------------------------------" << std::endl;

    for (unsigned int i = 0; i < format_table_size - 1; i++)
    {
        atomic_cout << i << " ";
        atomic_cout << avdecc_lib::utility::ieee1722_format_index_to_name(i) << " ";
        atomic_cout << avdecc_lib::utility::ieee1722_format_index_to_description(i) << std::endl;
    }

    return 0;
}

int cmd_line::cmd_view_details(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t end_station_index = current_end_station;

    avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(end_station_index);
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;

    if (get_current_entity_and_descriptor(end_station, &entity, &configuration))
        return 0;
    avdecc_lib::entity_descriptor_response * entity_resp_ref = entity->get_entity_response();

    atomic_cout << "\nEnd Station: "
                << " (" << entity_resp_ref->entity_name() << ")" << std::endl;
    atomic_cout << "Entity Index: " << end_station->get_current_entity_index() << "  Entity ID: 0x" << std::hex << entity_resp_ref->entity_id() << std::endl;
    atomic_cout << "Configuration: " << end_station->get_current_config_index() << std::endl;
    atomic_cout << "------------------------------------------------------------------------------" << std::endl;
    delete entity_resp_ref;

    switch (0)
    {
    case avdecc_lib::AEM_DESC_ENTITY:
    {
        std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(entity->descriptor_type());
        uint16_t desc_index = entity->descriptor_index();

        atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
        do_view_descriptor(desc_name, desc_index);
    }

    case avdecc_lib::AEM_DESC_CONFIGURATION:
    {
        std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(configuration->descriptor_type());
        uint16_t desc_index = configuration->descriptor_index();

        atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
        do_view_descriptor(desc_name, desc_index);
    }

    case avdecc_lib::AEM_DESC_AUDIO_UNIT:
        for (unsigned int j = 0; j < configuration->audio_unit_desc_count(); j++)
        {
            avdecc_lib::audio_unit_descriptor * audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(audio_unit_desc_ref->descriptor_type());
            uint16_t desc_index = audio_unit_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_STREAM_INPUT:
        for (unsigned int j = 0; j < configuration->stream_input_desc_count(); j++)
        {
            avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(stream_input_desc_ref->descriptor_type());
            uint16_t desc_index = stream_input_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
        for (unsigned int j = 0; j < configuration->stream_output_desc_count(); j++)
        {
            avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(stream_output_desc_ref->descriptor_type());
            uint16_t desc_index = stream_output_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_JACK_INPUT:
        for (unsigned int j = 0; j < configuration->jack_input_desc_count(); j++)
        {
            avdecc_lib::jack_input_descriptor * jack_input_desc_ref = configuration->get_jack_input_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(jack_input_desc_ref->descriptor_type());
            uint16_t desc_index = jack_input_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_JACK_OUTPUT:
        for (unsigned int j = 0; j < configuration->jack_output_desc_count(); j++)
        {
            avdecc_lib::jack_output_descriptor * jack_output_desc_ref = configuration->get_jack_output_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(jack_output_desc_ref->descriptor_type());
            uint16_t desc_index = jack_output_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_AVB_INTERFACE:
        for (unsigned int j = 0; j < configuration->avb_interface_desc_count(); j++)
        {
            avdecc_lib::avb_interface_descriptor * avb_interface_desc_ref = configuration->get_avb_interface_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(avb_interface_desc_ref->descriptor_type());
            uint16_t desc_index = avb_interface_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
        for (unsigned int j = 0; j < configuration->clock_source_desc_count(); j++)
        {
            avdecc_lib::clock_source_descriptor * clk_src_desc_ref = configuration->get_clock_source_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(clk_src_desc_ref->descriptor_type());
            uint16_t desc_index = clk_src_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_LOCALE:
        for (unsigned int j = 0; j < configuration->locale_desc_count(); j++)
        {
            avdecc_lib::locale_descriptor * locale_def_ref = configuration->get_locale_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(locale_def_ref->descriptor_type());
            uint16_t desc_index = locale_def_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_STRINGS:
        for (unsigned int j = 0; j < configuration->strings_desc_count(); j++)
        {
            avdecc_lib::strings_descriptor * strings = configuration->get_strings_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(strings->descriptor_type());
            uint16_t desc_index = strings->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_STREAM_PORT_INPUT:
        for (unsigned int j = 0; j < configuration->stream_port_input_desc_count(); j++)
        {
            avdecc_lib::stream_port_input_descriptor * stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(stream_port_input_desc_ref->descriptor_type());
            uint16_t desc_index = stream_port_input_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT:
        for (unsigned int j = 0; j < configuration->stream_port_output_desc_count(); j++)
        {
            avdecc_lib::stream_port_output_descriptor * stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(stream_port_output_desc_ref->descriptor_type());
            uint16_t desc_index = stream_port_output_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_AUDIO_CLUSTER:
        for (unsigned int j = 0; j < configuration->audio_cluster_desc_count(); j++)
        {
            avdecc_lib::audio_cluster_descriptor * audio_cluster_desc_ref = configuration->get_audio_cluster_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(audio_cluster_desc_ref->descriptor_type());
            uint16_t desc_index = audio_cluster_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_AUDIO_MAP:
        for (unsigned int j = 0; j < configuration->audio_map_desc_count(); j++)
        {
            avdecc_lib::audio_map_descriptor * audio_map_desc_ref = configuration->get_audio_map_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(audio_map_desc_ref->descriptor_type());
            uint16_t desc_index = audio_map_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
        for (unsigned int j = 0; j < configuration->clock_domain_desc_count(); j++)
        {
            avdecc_lib::clock_domain_descriptor * clk_domain_desc_ref = configuration->get_clock_domain_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(clk_domain_desc_ref->descriptor_type());
            uint16_t desc_index = clk_domain_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_EXTERNAL_PORT_INPUT:
        for (unsigned int j = 0; j < configuration->external_port_input_desc_count(); j++)
        {
            avdecc_lib::external_port_input_descriptor * port_desc_ref = configuration->get_external_port_input_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(port_desc_ref->descriptor_type());
            uint16_t desc_index = port_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

    case avdecc_lib::AEM_DESC_EXTERNAL_PORT_OUTPUT:
        for (unsigned int j = 0; j < configuration->external_port_output_desc_count(); j++)
        {
            avdecc_lib::external_port_output_descriptor * port_desc_ref = configuration->get_external_port_output_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(port_desc_ref->descriptor_type());
            uint16_t desc_index = port_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }
    case avdecc_lib::AEM_DESC_CONTROL:
        for (unsigned int j = 0; j < configuration->control_desc_count(); j++)
        {
            avdecc_lib::control_descriptor * control_desc_ref = configuration->get_control_desc_by_index(j);
            std::string desc_name = avdecc_lib::utility::aem_desc_value_to_name(control_desc_ref->descriptor_type());
            uint16_t desc_index = control_desc_ref->descriptor_index();

            atomic_cout << "\n----------------------- " << desc_name << " -----------------------";
            do_view_descriptor(desc_name, desc_index);
        }

        break;
    }

    return 0;
}

int cmd_line::cmd_view_descriptor(int total_matched, std::vector<cli_argument *> args)
{
    do_view_descriptor(args[0]->get_value_str(), args[1]->get_value_int());
    return 0;
}

int cmd_line::do_view_descriptor(std::string desc_name, uint16_t desc_index)
{
    avdecc_lib::end_station * end_station;
    if (get_current_end_station(&end_station))
        return 0;

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    atomic_cout << "\ndescriptor_type: " << avdecc_lib::utility::aem_desc_value_to_name(desc_type_value);
    atomic_cout << "\ndescriptor_index: " << std::dec << desc_index;

    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    get_current_entity_and_descriptor(end_station, &entity, &configuration);

    // test field output
    if (desc_type_value == avdecc_lib::AEM_DESC_EXTERNAL_PORT_INPUT)
    {
        avdecc_lib::descriptor_base * desc = configuration->get_external_port_input_desc_by_index(desc_index)->get_external_port_input_response();
        uint32_t v = 0;

        for (unsigned int i = 0; i < desc->field_count(); i++)
        {
            avdecc_lib::descriptor_field * f = desc->field(i);
            switch (f->get_type())
            {
            case avdecc_lib::descriptor_field::TYPE_CHAR:
                atomic_cout << "\n"
                            << f->get_name() << " = " << f->get_char();
                break;
            case avdecc_lib::descriptor_field::TYPE_UINT16:
                atomic_cout << "\n"
                            << f->get_name() << " = " << f->get_uint16();
                break;
            case avdecc_lib::descriptor_field::TYPE_UINT32:
                atomic_cout << "\n"
                            << f->get_name() << " = " << f->get_uint32();
                break;
            // Don't have to handle these separately here, but internal to the library they have different types.
            case avdecc_lib::descriptor_field::TYPE_FLAGS16:
            case avdecc_lib::descriptor_field::TYPE_FLAGS32:
                v = f->get_flags();
                atomic_cout << "\nFlags " << f->get_name() << " = 0x" << std::setfill('0') << std::setw(8) << std::hex << v;
                for (unsigned int j = 0; j < f->get_flags_count(); j++)
                {
                    uint32_t the_bit;
                    avdecc_lib::descriptor_field_flags * fl = f->get_flag_by_index(j);

                    the_bit = ((v & fl->get_flag_mask()) != 0);
                    atomic_cout << "\n\t" << std::setw(32) << fl->get_flag_name() << " = " << the_bit << " (mask is 0x" << std::setfill('0') << std::setw(8) << std::hex << fl->get_flag_mask() << ")";
                }
                break;
            default:
                atomic_cout << "\nUNHANDLED FIELD TYPE";
            }
        }
        delete desc;
        return 0;
    }

    switch (desc_type_value)
    {
    case avdecc_lib::AEM_DESC_ENTITY:
    {
        if (entity)
        {
            avdecc_lib::entity_descriptor_response * entity_desc_resp = entity->get_entity_response();
            atomic_cout << "\nentity_id = 0x" << std::hex << entity_desc_resp->entity_id();
            atomic_cout << "\nentity_model_id = " << std::dec << entity_desc_resp->entity_model_id();
            atomic_cout << "\nentity_capabilities = 0x" << std::hex << entity_desc_resp->entity_capabilities();
            atomic_cout << "\ntalker_stream_sources = " << std::dec << entity_desc_resp->talker_stream_sources();
            atomic_cout << "\ntalker_capabilities = 0x" << std::hex << entity_desc_resp->talker_capabilities();
            atomic_cout << "\nlistener_stream_sinks = " << std::dec << entity_desc_resp->listener_stream_sinks();
            atomic_cout << "\nlistener_capabilities = 0x" << std::hex << entity_desc_resp->listener_capabilities();
            atomic_cout << "\ncontroller_capabilities = 0x" << std::hex << entity_desc_resp->controller_capabilities();
            atomic_cout << "\navailable_index = " << std::dec << entity_desc_resp->available_index();
            atomic_cout << "\nassociation_id = " << std::dec << entity_desc_resp->association_id();
            atomic_cout << "\nentity_name = " << std::hex << entity_desc_resp->entity_name();
            atomic_cout << "\nvendor_name_string = " << std::dec << entity_desc_resp->vendor_name_string();
            atomic_cout << "\nmodel_name_string = " << std::dec << entity_desc_resp->model_name_string();
            atomic_cout << "\nfirmware_version = " << std::dec << entity_desc_resp->firmware_version();
            atomic_cout << "\ngroup_name = " << std::dec << entity_desc_resp->group_name();
            atomic_cout << "\nserial_number = " << std::dec << entity_desc_resp->serial_number();
            atomic_cout << "\nconfigurations_count = " << std::dec << entity_desc_resp->configurations_count();
            atomic_cout << "\ncurrent_configuration = " << std::dec << entity_desc_resp->current_configuration() << std::endl;
            delete (entity_desc_resp);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_CONFIGURATION:
    {
        if (configuration)
        {
            atomic_cout << "\nobject_name = " << std::hex << configuration->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << configuration->localized_description();
            atomic_cout << "\ndescriptor_counts_count = " << std::dec << configuration->descriptor_counts_count() << std::endl;

            uint16_t desc_counts_count = configuration->descriptor_counts_count();
            uint16_t desc_type_from_config = 0;
            uint16_t desc_count_from_config = 0;

            if (desc_counts_count > 0)
            {
                atomic_cout << "\nTop level descriptors" << std::endl;

                for (uint32_t i = 0; i < desc_counts_count; i++)
                {
                    desc_type_from_config = configuration->get_desc_type_from_config_by_index(i);
                    desc_count_from_config = configuration->get_desc_count_from_config_by_index(i);

                    atomic_cout << "\tdesc_type = 0x" << std::hex << desc_type_from_config << " ("
                                << avdecc_lib::utility::aem_desc_value_to_name(desc_type_from_config) << ")" << std::endl;
                    atomic_cout << "\tdesc_count = " << std::dec << desc_count_from_config << std::endl;
                }
            }
        }
    }
    break;

    case avdecc_lib::AEM_DESC_AUDIO_UNIT:
    {
        if (!configuration)
            break;

        avdecc_lib::audio_unit_descriptor * audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(desc_index);
        if (audio_unit_desc_ref)
        {
            avdecc_lib::audio_unit_descriptor_response * audio_unit_resp_ref = audio_unit_desc_ref->get_audio_unit_response();
            atomic_cout << "\nobject_name = " << std::hex << audio_unit_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << audio_unit_resp_ref->localized_description();
            atomic_cout << "\nclock_domain_index = " << std::dec << audio_unit_resp_ref->clock_domain_index();
            atomic_cout << "\nnumber_of_stream_input_ports = " << std::dec << audio_unit_resp_ref->number_of_stream_input_ports();
            atomic_cout << "\nbase_stream_input_port = " << std::dec << audio_unit_resp_ref->base_stream_input_port();
            atomic_cout << "\nnumber_of_stream_output_ports = " << std::dec << audio_unit_resp_ref->number_of_stream_output_ports();
            atomic_cout << "\nbase_stream_output_port = " << std::dec << audio_unit_resp_ref->base_stream_output_port();
            atomic_cout << "\nnumber_of_external_input_ports = " << std::dec << audio_unit_resp_ref->number_of_external_input_ports();
            atomic_cout << "\nbase_external_input_port = " << std::dec << audio_unit_resp_ref->base_external_input_port();
            atomic_cout << "\nnumber_of_external_output_ports = " << std::dec << audio_unit_resp_ref->number_of_external_output_ports();
            atomic_cout << "\nbase_external_output_port = " << std::dec << audio_unit_resp_ref->base_external_output_port();
            atomic_cout << "\nnumber_of_internal_input_ports = " << std::dec << audio_unit_resp_ref->number_of_internal_input_ports();
            atomic_cout << "\nbase_internal_input_port = " << std::dec << audio_unit_resp_ref->base_internal_input_port();
            atomic_cout << "\nnumber_of_internal_output_ports = " << std::dec << audio_unit_resp_ref->number_of_internal_output_ports();
            atomic_cout << "\nbase_internal_output_port = " << std::dec << audio_unit_resp_ref->base_internal_output_port();
            atomic_cout << "\nnumber_of_controls = " << std::dec << audio_unit_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << audio_unit_resp_ref->base_control();
            atomic_cout << "\nnumber_of_signal_selectors = " << std::dec << audio_unit_resp_ref->number_of_signal_selectors();
            atomic_cout << "\nbase_signal_selector = " << std::dec << audio_unit_resp_ref->base_signal_selector();
            atomic_cout << "\nnumber_of_mixers = " << std::dec << audio_unit_resp_ref->number_of_mixers();
            atomic_cout << "\nbase_mixer = " << std::dec << audio_unit_resp_ref->base_mixer();
            atomic_cout << "\nnumber_of_matrices = " << std::dec << audio_unit_resp_ref->number_of_matrices();
            atomic_cout << "\nbase_matrix = " << std::dec << audio_unit_resp_ref->base_matrix();
            atomic_cout << "\nnumber_of_splitters = " << std::dec << audio_unit_resp_ref->number_of_splitters();
            atomic_cout << "\nbase_splitter = " << std::dec << audio_unit_resp_ref->base_splitter();
            atomic_cout << "\nnumber_of_combiners = " << std::dec << audio_unit_resp_ref->number_of_combiners();
            atomic_cout << "\nbase_combiner = " << std::dec << audio_unit_resp_ref->base_combiner();
            atomic_cout << "\nnumber_of_demultiplexers = " << std::dec << audio_unit_resp_ref->number_of_demultiplexers();
            atomic_cout << "\nbase_demultiplexer = " << std::dec << audio_unit_resp_ref->base_demultiplexer();
            atomic_cout << "\nnumber_of_multiplexers = " << std::dec << audio_unit_resp_ref->number_of_multiplexers();
            atomic_cout << "\nbase_multiplexer = " << std::dec << audio_unit_resp_ref->base_multiplexer();
            atomic_cout << "\nnumber_of_transcoders = " << std::dec << audio_unit_resp_ref->number_of_transcoders();
            atomic_cout << "\nbase_transcoder = " << std::dec << audio_unit_resp_ref->base_transcoder();
            atomic_cout << "\nnumber_of_control_blocks = " << std::dec << audio_unit_resp_ref->number_of_control_blocks();
            atomic_cout << "\nbase_control_block = " << std::dec << audio_unit_resp_ref->base_control_block();
            atomic_cout << "\ncurrent_sampling_rate = " << std::dec << audio_unit_resp_ref->current_sampling_rate();
            atomic_cout << "\nsampling_rates_count = " << std::dec << audio_unit_resp_ref->sampling_rates_count() << std::endl;

            for (uint32_t i = 0; i < audio_unit_resp_ref->sampling_rates_count(); i++)
            {
                atomic_cout << "sampling_rate_" << i << " = " << std::dec << audio_unit_resp_ref->get_sampling_rate_by_index(i) << std::endl;
            }
            delete (audio_unit_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_STREAM_INPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        if (stream_input_desc_ref)
        {
            avdecc_lib::stream_input_descriptor_response * stream_input_resp_ref = stream_input_desc_ref->get_stream_input_response();
            atomic_cout << "\nobject_name = " << std::hex << stream_input_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << stream_input_resp_ref->localized_description();
            atomic_cout << "\nclock_domain_index = " << std::dec << stream_input_resp_ref->clock_domain_index();
            atomic_cout << "\nstream_flags = 0x" << std::hex << stream_input_resp_ref->stream_flags();
            atomic_cout << "\n\tclock_sync_source = " << std::dec << stream_input_resp_ref->stream_flags_clock_sync_source();
            atomic_cout << "\n\tclass_a = " << std::dec << stream_input_resp_ref->stream_flags_class_a();
            atomic_cout << "\n\tclass_b = " << std::dec << stream_input_resp_ref->stream_flags_class_b();
            atomic_cout << "\n\tsupports_encrypted = " << std::dec << stream_input_resp_ref->stream_flags_supports_encrypted();
            atomic_cout << "\n\tprimary_backup_valid = " << std::dec << stream_input_resp_ref->stream_flags_primary_backup_valid();
            atomic_cout << "\n\tprimary_backup_valid = " << std::dec << stream_input_resp_ref->stream_flags_primary_backup_valid();
            atomic_cout << "\n\tsecondary_backup_supported = " << std::dec << stream_input_resp_ref->stream_flags_secondary_backup_supported();
            atomic_cout << "\n\tsecondary_backup_valid = " << std::dec << stream_input_resp_ref->stream_flags_secondary_backup_valid();
            atomic_cout << "\n\ttertiary_backup_supported = " << std::dec << stream_input_resp_ref->stream_flags_tertiary_backup_supported();
            atomic_cout << "\n\ttertiary_backup_valid = " << std::dec << stream_input_resp_ref->stream_flags_tertiary_backup_valid();
            atomic_cout << "\ncurrent_format_name= " << std::hex << stream_input_resp_ref->current_format_name();
            atomic_cout << "\nbackup_talker_entity_id_0 = 0x" << std::hex << stream_input_resp_ref->backup_talker_entity_id_0();
            atomic_cout << "\nbackup_talker_unique_0 = " << std::dec << stream_input_resp_ref->backup_talker_unique_0();
            atomic_cout << "\nbackup_talker_entity_id_1 = 0x" << std::hex << stream_input_resp_ref->backup_talker_entity_id_1();
            atomic_cout << "\nbackup_talker_unique_1 = " << std::dec << stream_input_resp_ref->backup_talker_unique_1();
            atomic_cout << "\nbackup_talker_entity_id_2 = 0x" << std::hex << stream_input_resp_ref->backup_talker_entity_id_2();
            atomic_cout << "\nbackup_talker_unique_2 = " << std::dec << stream_input_resp_ref->backup_talker_unique_2();
            atomic_cout << "\nbackedup_talker_entity_id = 0x" << std::hex << stream_input_resp_ref->backedup_talker_entity_id();
            atomic_cout << "\nbackedup_talker_unique = " << std::dec << stream_input_resp_ref->backedup_talker_unique();
            atomic_cout << "\navb_interface_index = " << std::dec << stream_input_resp_ref->avb_interface_index();
            atomic_cout << "\nbuffer_length = " << std::dec << stream_input_resp_ref->buffer_length();
            atomic_cout << "\nnumber_of_formats = " << std::dec << stream_input_resp_ref->number_of_formats() << std::endl;
            for (int i = 0; i < stream_input_resp_ref->number_of_formats(); i++)
            {
                uint64_t current_format_value = stream_input_resp_ref->get_supported_stream_fmt_by_index(i);
                const char * current_format_name = avdecc_lib::utility::ieee1722_format_value_to_name(current_format_value);
                if (strcmp(current_format_name, "UNKNOWN"))
                {
                    atomic_cout << "\tstream_format_" << i << " = " << current_format_name << std::endl;
                }
                else
                {
                    atomic_cout << "\tstream_format_" << i << " = 0x" << std::hex << current_format_value << std::endl;
                }
            }

            delete (stream_input_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        if (stream_output_desc_ref)
        {
            avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = stream_output_desc_ref->get_stream_output_response();
            atomic_cout << "\nobject_name = " << std::hex << stream_output_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << stream_output_resp_ref->localized_description();
            atomic_cout << "\nclock_domain_index = " << std::dec << stream_output_resp_ref->clock_domain_index();
            atomic_cout << "\nstream_flags = 0x" << std::hex << stream_output_resp_ref->stream_flags();
            atomic_cout << "\n\tclock_sync_source = " << std::dec << stream_output_resp_ref->stream_flags_clock_sync_source();
            atomic_cout << "\n\tclass_a = " << std::dec << stream_output_resp_ref->stream_flags_class_a();
            atomic_cout << "\n\tclass_b = " << std::dec << stream_output_resp_ref->stream_flags_class_b();
            atomic_cout << "\n\tsupports_encrypted = " << std::dec << stream_output_resp_ref->stream_flags_supports_encrypted();
            atomic_cout << "\n\tprimary_backup_valid = " << std::dec << stream_output_resp_ref->stream_flags_primary_backup_valid();
            atomic_cout << "\n\tprimary_backup_valid = " << std::dec << stream_output_resp_ref->stream_flags_primary_backup_valid();
            atomic_cout << "\n\tsecondary_backup_supported = " << std::dec << stream_output_resp_ref->stream_flags_secondary_backup_supported();
            atomic_cout << "\n\tsecondary_backup_valid = " << std::dec << stream_output_resp_ref->stream_flags_secondary_backup_valid();
            atomic_cout << "\n\ttertiary_backup_supported = " << std::dec << stream_output_resp_ref->stream_flags_tertiary_backup_supported();
            atomic_cout << "\n\ttertiary_backup_valid = " << std::dec << stream_output_resp_ref->stream_flags_tertiary_backup_valid();
            atomic_cout << "\ncurrent_format_name = " << std::hex << stream_output_resp_ref->current_format_name();
            atomic_cout << "\nbackup_talker_entity_id_0 = 0x" << std::hex << stream_output_resp_ref->backup_talker_entity_id_0();
            atomic_cout << "\nbackup_talker_unique_0 = " << std::dec << stream_output_resp_ref->backup_talker_unique_0();
            atomic_cout << "\nbackup_talker_entity_id_1 = 0x" << std::hex << stream_output_resp_ref->backup_talker_entity_id_1();
            atomic_cout << "\nbackup_talker_unique_1 = " << std::dec << stream_output_resp_ref->backup_talker_unique_1();
            atomic_cout << "\nbackup_talker_entity_id_2 = 0x" << std::hex << stream_output_resp_ref->backup_talker_entity_id_2();
            atomic_cout << "\nbackup_talker_unique_2 = " << std::dec << stream_output_resp_ref->backup_talker_unique_2();
            atomic_cout << "\nbackedup_talker_entity_id = 0x" << std::hex << stream_output_resp_ref->backedup_talker_entity_id();
            atomic_cout << "\nbackedup_talker_unique = " << std::dec << stream_output_resp_ref->backedup_talker_unique();
            atomic_cout << "\navb_interface_index = " << std::dec << stream_output_resp_ref->avb_interface_index();
            atomic_cout << "\nbuffer_length = " << std::dec << stream_output_resp_ref->buffer_length();
            atomic_cout << "\nnumber_of_formats = " << std::dec << stream_output_resp_ref->number_of_formats() << std::endl;
            for (int i = 0; i < stream_output_resp_ref->number_of_formats(); i++)
            {
                uint64_t current_format_value = stream_output_resp_ref->get_supported_stream_fmt_by_index(i);
                const char * current_format_name = avdecc_lib::utility::ieee1722_format_value_to_name(current_format_value);
                if (strcmp(current_format_name, "UNKNOWN"))
                {
                    atomic_cout << "\tstream_format_" << i << " = " << current_format_name << std::endl;
                }
                else
                {
                    atomic_cout << "\tstream_format_" << i << " = 0x" << std::hex << current_format_value << std::endl;
                }
            }

            delete (stream_output_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_JACK_INPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::jack_input_descriptor * jack_input_desc_ref = configuration->get_jack_input_desc_by_index(desc_index);
        if (jack_input_desc_ref)
        {
            avdecc_lib::jack_input_descriptor_response * jack_input_resp_ref = jack_input_desc_ref->get_jack_input_response();
            atomic_cout << "\nobject_name = " << std::hex << jack_input_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << jack_input_resp_ref->localized_description();
            atomic_cout << "\njack_flags = 0x" << std::hex << jack_input_resp_ref->jack_flags();
            atomic_cout << "\n\tclock_sync_source_flag = 0x" << std::hex << jack_input_resp_ref->jack_flag_clock_sync_source();
            atomic_cout << "\n\tcaptive_flag = 0x" << std::hex << jack_input_resp_ref->jack_flag_captive();
            atomic_cout << "\nnumber_of_controls = " << std::dec << jack_input_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << jack_input_resp_ref->base_control() << std::endl;
            delete (jack_input_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_JACK_OUTPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::jack_output_descriptor * jack_output_desc_ref = configuration->get_jack_output_desc_by_index(desc_index);
        if (jack_output_desc_ref)
        {
            avdecc_lib::jack_output_descriptor_response * jack_output_resp_ref = jack_output_desc_ref->get_jack_output_response();
            atomic_cout << "\nobject_name = " << std::hex << jack_output_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = 0x" << std::hex << jack_output_resp_ref->localized_description();
            atomic_cout << "\njack_flags = 0x" << std::hex << jack_output_resp_ref->jack_flags();
            atomic_cout << "\n\tclock_sync_source_flag = 0x" << std::hex << jack_output_resp_ref->jack_flag_clock_sync_source();
            atomic_cout << "\n\tcaptive_flag = 0x" << std::hex << jack_output_resp_ref->jack_flag_captive();
            atomic_cout << "\njack_type = 0x" << std::hex << jack_output_resp_ref->jack_type();
            atomic_cout << "\nnumber_of_controls = " << std::dec << jack_output_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << jack_output_resp_ref->base_control() << std::endl;
            delete (jack_output_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_AVB_INTERFACE:
    {
        if (!configuration)
            break;

        avdecc_lib::avb_interface_descriptor * avb_interface_desc = configuration->get_avb_interface_desc_by_index(desc_index);
        if (avb_interface_desc)
        {
            avdecc_lib::avb_interface_descriptor_response * avb_interface_desc_resp = avb_interface_desc->get_avb_interface_response();
            atomic_cout << "\nobject_name = " << std::hex << avb_interface_desc_resp->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << avb_interface_desc_resp->localized_description();
            atomic_cout << "\nmac_address = 0x" << std::hex << avb_interface_desc_resp->mac_addr();
            atomic_cout << "\ninterface_flags = 0x" << std::hex << avb_interface_desc_resp->interface_flags();
            atomic_cout << "\nclock_identity = 0x" << std::hex << avb_interface_desc_resp->clock_identity();
            atomic_cout << "\npriority1 = " << std::dec << (unsigned int)avb_interface_desc_resp->priority1();
            atomic_cout << "\nclock_class = " << std::dec << (unsigned int)avb_interface_desc_resp->clock_class();
            atomic_cout << "\noffset_scaled_log_variance = " << std::dec << avb_interface_desc_resp->offset_scaled_log_variance();
            atomic_cout << "\nclock_accuracy = " << std::dec << (unsigned int)avb_interface_desc_resp->clock_accuracy();
            atomic_cout << "\npriority2 = " << std::dec << (unsigned int)avb_interface_desc_resp->priority2();
            atomic_cout << "\ndomain_number = " << std::dec << (unsigned int)avb_interface_desc_resp->domain_number();
            atomic_cout << "\nlog_sync_interval = " << std::dec << (unsigned int)avb_interface_desc_resp->log_sync_interval();
            atomic_cout << "\nlog_announce_interval = " << std::dec << (unsigned int)avb_interface_desc_resp->log_announce_interval();
            atomic_cout << "\nlog_pdelay_interval = " << std::dec << (unsigned int)avb_interface_desc_resp->log_pdelay_interval();
            atomic_cout << "\nport_number = " << std::dec << avb_interface_desc_resp->port_number() << std::endl;
            delete (avb_interface_desc_resp);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
    {
        if (!configuration)
            break;

        avdecc_lib::clock_source_descriptor * clk_src_desc = configuration->get_clock_source_desc_by_index(desc_index);
        if (clk_src_desc)
        {
            avdecc_lib::clock_source_descriptor_response * clk_src_resp_ref = clk_src_desc->get_clock_source_response();
            atomic_cout << "\nobject_name = " << std::hex << clk_src_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << clk_src_resp_ref->localized_description();
            atomic_cout << "\nclock_source_flags = 0x" << std::hex << clk_src_resp_ref->clock_source_flags();
            atomic_cout << "\nclock_source_type = 0x" << std::hex << clk_src_resp_ref->clock_source_type();
            atomic_cout << "\nclock_source_identifier = 0x" << std::hex << clk_src_resp_ref->clock_source_identifier();
            atomic_cout << "\nclock_source_location_type = 0x" << std::hex << clk_src_resp_ref->clock_source_location_type();
            atomic_cout << "\nclock_source_location_index = " << std::dec << clk_src_resp_ref->clock_source_location_index() << std::endl;
            delete (clk_src_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_MEMORY_OBJECT:
    {
        if (!configuration)
            break;

        avdecc_lib::memory_object_descriptor * mem_obj_desc = configuration->get_memory_object_desc_by_index(desc_index);
        if (mem_obj_desc)
        {
            avdecc_lib::memory_object_descriptor_response * mem_obj_resp_ref = mem_obj_desc->get_memory_object_response();
            atomic_cout << "\nobject_name = " << std::hex << mem_obj_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << mem_obj_resp_ref->localized_description();
            atomic_cout << "\nmemory_object_type = " << mem_obj_resp_ref->memory_object_type_to_str();
            atomic_cout << "\ntarget_descriptor_type = 0x" << std::hex << mem_obj_resp_ref->target_descriptor_type();
            atomic_cout << "\ntarget_descriptor_index = " << std::dec << mem_obj_resp_ref->target_descriptor_index();
            atomic_cout << "\nstart_address = 0x" << std::hex << mem_obj_resp_ref->start_address();
            atomic_cout << "\nmaximum_length = " << std::dec << mem_obj_resp_ref->maximum_length() << " bytes";
            ;
            atomic_cout << "\nlength = " << std::dec << mem_obj_resp_ref->length() << " bytes" << std::endl;
            delete (mem_obj_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_LOCALE:
    {
        if (!configuration)
            break;

        avdecc_lib::locale_descriptor * locale = configuration->get_locale_desc_by_index(desc_index);
        if (locale)
        {
            avdecc_lib::locale_descriptor_response * locale_resp = locale->get_locale_response();
            atomic_cout << "\nlocale_identifier = " << std::dec << locale_resp->locale_identifier();
            atomic_cout << "\nnumber_of_strings = " << std::dec << locale_resp->number_of_strings();
            atomic_cout << "\nbase_strings = " << std::dec << locale_resp->base_strings() << std::endl;
            delete (locale_resp);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_STRINGS:
    {
        if (!configuration)
            break;

        avdecc_lib::strings_descriptor * strings = configuration->get_strings_desc_by_index(desc_index);
        if (strings)
        {
            avdecc_lib::strings_descriptor_response * strings_resp_ref = strings->get_strings_response();
            atomic_cout << "\nstring_0 = " << std::hex << strings_resp_ref->get_string_by_index(0);
            atomic_cout << "\nstring_1 = " << std::hex << strings_resp_ref->get_string_by_index(1);
            atomic_cout << "\nstring_2 = " << std::hex << strings_resp_ref->get_string_by_index(2);
            atomic_cout << "\nstring_3 = " << std::hex << strings_resp_ref->get_string_by_index(3);
            atomic_cout << "\nstring_4 = " << std::hex << strings_resp_ref->get_string_by_index(4);
            atomic_cout << "\nstring_5 = " << std::hex << strings_resp_ref->get_string_by_index(5);
            atomic_cout << "\nstring_6 = " << std::hex << strings_resp_ref->get_string_by_index(6) << std::endl;
            delete (strings_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_STREAM_PORT_INPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc = configuration->get_stream_port_input_desc_by_index(desc_index);
        if (stream_port_input_desc)
        {
            avdecc_lib::stream_port_input_descriptor_response * stream_port_input_resp_ref = stream_port_input_desc->get_stream_port_input_response();
            atomic_cout << "\nclock_domain_index = " << std::dec << stream_port_input_resp_ref->clock_domain_index();
            atomic_cout << "\nport_flags = " << std::hex << stream_port_input_resp_ref->port_flags();
            atomic_cout << "\nnumber_of_controls = " << std::dec << stream_port_input_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << stream_port_input_resp_ref->base_control();
            atomic_cout << "\nnumber_of_clusters = " << std::dec << stream_port_input_resp_ref->number_of_clusters();
            atomic_cout << "\nbase_cluster = " << std::dec << stream_port_input_resp_ref->base_cluster();
            atomic_cout << "\nnumber_of_maps = " << std::dec << stream_port_input_resp_ref->number_of_maps();
            atomic_cout << "\nbase_map = " << std::dec << stream_port_input_resp_ref->base_map() << std::endl;
            delete (stream_port_input_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc = configuration->get_stream_port_output_desc_by_index(desc_index);
        if (stream_port_output_desc)
        {
            avdecc_lib::stream_port_output_descriptor_response * stream_port_output_resp_ref = stream_port_output_desc->get_stream_port_output_response();
            atomic_cout << "\nclock_domain_index = " << std::dec << stream_port_output_resp_ref->clock_domain_index();
            atomic_cout << "\nport_flags = " << std::hex << stream_port_output_resp_ref->port_flags();
            atomic_cout << "\nnumber_of_controls = " << std::dec << stream_port_output_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << stream_port_output_resp_ref->base_control();
            atomic_cout << "\nnumber_of_clusters = " << std::dec << stream_port_output_resp_ref->number_of_clusters();
            atomic_cout << "\nbase_cluster = " << std::dec << stream_port_output_resp_ref->base_cluster();
            atomic_cout << "\nnumber_of_maps = " << std::dec << stream_port_output_resp_ref->number_of_maps();
            atomic_cout << "\nbase_map = " << std::dec << stream_port_output_resp_ref->base_map() << std::endl;
            delete (stream_port_output_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_AUDIO_CLUSTER:
    {
        if (!configuration)
            break;

        avdecc_lib::audio_cluster_descriptor * audio_cluster_desc = configuration->get_audio_cluster_desc_by_index(desc_index);
        if (audio_cluster_desc)
        {
            avdecc_lib::audio_cluster_descriptor_response * audio_cluster_resp_ref = audio_cluster_desc->get_audio_cluster_response();
            atomic_cout << "\nobject_name = " << std::hex << audio_cluster_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << audio_cluster_resp_ref->localized_description();
            atomic_cout << "\nsignal_type = " << std::dec << audio_cluster_resp_ref->signal_type();
            atomic_cout << "\nsignal_index = " << std::dec << audio_cluster_resp_ref->signal_index();
            atomic_cout << "\nsignal_output = " << std::dec << audio_cluster_resp_ref->signal_output();
            atomic_cout << "\npath_latency = " << std::dec << audio_cluster_resp_ref->path_latency();
            atomic_cout << "\nblock_latency = " << std::dec << audio_cluster_resp_ref->block_latency();
            atomic_cout << "\nchannel_count = " << std::dec << audio_cluster_resp_ref->channel_count();
            atomic_cout << "\nformat = 0x" << std::hex << (unsigned int)audio_cluster_resp_ref->format() << std::endl;
            delete (audio_cluster_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_AUDIO_MAP:
    {
        if (!configuration)
            break;

        avdecc_lib::audio_map_descriptor * audio_map_desc = configuration->get_audio_map_desc_by_index(desc_index);
        if (audio_map_desc)
        {
            avdecc_lib::audio_map_descriptor_response * audio_map_resp_ref = audio_map_desc->get_audio_map_response();
            uint16_t nmappings = audio_map_resp_ref->number_of_mappings();
            atomic_cout << "\nnumber_of_mappings = " << std::dec << nmappings << std::endl;

            for (int i = 0; i < (int)nmappings; i++)
            {
                struct avdecc_lib::audio_map_mapping map;

                int ret = audio_map_resp_ref->mapping(i, map);

                if (ret == 0)
                {
                    atomic_cout << "map[" << i << "].stream_index = " << std::dec << map.stream_index << std::endl;
                    atomic_cout << "map[" << i << "].stream_channel = " << std::dec << map.stream_channel << std::endl;
                    atomic_cout << "map[" << i << "].cluster_offset = " << std::dec << map.cluster_offset << std::endl;
                    atomic_cout << "map[" << i << "].cluster_channel = " << std::dec << map.cluster_channel << std::endl;
                }
            }
            delete (audio_map_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
    {
        if (!configuration)
            break;

        avdecc_lib::clock_domain_descriptor * clk_domain_desc = configuration->get_clock_domain_desc_by_index(desc_index);
        if (clk_domain_desc)
        {
            avdecc_lib::clock_domain_descriptor_response * clk_domain_resp_ref = clk_domain_desc->get_clock_domain_response();
            atomic_cout << "\nobject_name = " << std::hex << clk_domain_resp_ref->object_name();
            atomic_cout << "\nlocalized_description = " << std::dec << clk_domain_resp_ref->localized_description();
            atomic_cout << "\nclock_source_index = " << std::dec << clk_domain_resp_ref->clock_source_index();
            atomic_cout << "\nclock_sources_count = " << std::dec << clk_domain_resp_ref->clock_sources_count() << std::endl;

            for (uint32_t i = 0; i < clk_domain_resp_ref->clock_sources_count(); i++)
            {
                atomic_cout << "\tclock_sources = " << std::dec << clk_domain_resp_ref->get_clock_source_by_index(i) << std::endl;
            }
            delete (clk_domain_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_EXTERNAL_PORT_INPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::external_port_input_descriptor * desc = configuration->get_external_port_input_desc_by_index(desc_index);
        if (desc)
        {
            avdecc_lib::external_port_input_descriptor_response * external_port_input_resp_ref = desc->get_external_port_input_response();
            atomic_cout << "\nport_flags = " << std::hex << external_port_input_resp_ref->port_flags();
            atomic_cout << "\nclock_domain_index = " << std::dec << external_port_input_resp_ref->clock_domain_index();
            atomic_cout << "\nnumber_of_controls = " << std::dec << external_port_input_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << external_port_input_resp_ref->base_control();
            atomic_cout << "\nsignal_type = " << std::dec << external_port_input_resp_ref->signal_type();
            atomic_cout << "\nsignal_index = " << std::dec << external_port_input_resp_ref->signal_index();
            atomic_cout << "\nsignal_output = " << std::dec << external_port_input_resp_ref->signal_output();
            atomic_cout << "\nblock_latency = " << std::dec << external_port_input_resp_ref->block_latency();
            atomic_cout << "\njack_index = " << std::dec << external_port_input_resp_ref->jack_index();
            delete (external_port_input_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_EXTERNAL_PORT_OUTPUT:
    {
        if (!configuration)
            break;

        avdecc_lib::external_port_output_descriptor * desc = configuration->get_external_port_output_desc_by_index(desc_index);
        if (desc)
        {
            avdecc_lib::external_port_output_descriptor_response * external_port_output_resp_ref = desc->get_external_port_output_response();
            atomic_cout << "\nport_flags = " << std::hex << external_port_output_resp_ref->port_flags();
            atomic_cout << "\nclock_domain_index = " << std::dec << external_port_output_resp_ref->clock_domain_index();
            atomic_cout << "\nnumber_of_controls = " << std::dec << external_port_output_resp_ref->number_of_controls();
            atomic_cout << "\nbase_control = " << std::dec << external_port_output_resp_ref->base_control();
            atomic_cout << "\nsignal_type = " << std::dec << external_port_output_resp_ref->signal_type();
            atomic_cout << "\nsignal_index = " << std::dec << external_port_output_resp_ref->signal_index();
            atomic_cout << "\nsignal_output = " << std::dec << external_port_output_resp_ref->signal_output();
            atomic_cout << "\nblock_latency = " << std::dec << external_port_output_resp_ref->block_latency();
            atomic_cout << "\njack_index = " << std::dec << external_port_output_resp_ref->jack_index();
            delete (external_port_output_resp_ref);
        }
    }
    break;

    case avdecc_lib::AEM_DESC_CONTROL:
    {
        if (!configuration)
            break;

        avdecc_lib::control_descriptor * control_desc = configuration->get_control_desc_by_index(desc_index);
        if (control_desc)
        {
            avdecc_lib::control_descriptor_response * control_resp_ref = control_desc->get_control_response();
            atomic_cout << "\nobject_name = " << std::hex << control_resp_ref->object_name();
            atomic_cout << "\nlocalized_description= " << control_resp_ref->localized_description();
            atomic_cout << "\nblock_latency= " << std::dec << control_resp_ref->block_latency();
            atomic_cout << "\ncontrol_latency= " << std::dec << control_resp_ref->control_latency();
            atomic_cout << "\ncontrol_domain = " << std::dec << control_resp_ref->control_domain();
            atomic_cout << "\ncontrol_value_type = " << std::dec << control_resp_ref->control_value_type();
            atomic_cout << "\ncontrol_type = " << std::dec << control_resp_ref->control_type();
            atomic_cout << "\nreset_time = " << std::dec << control_resp_ref->reset_time();
            atomic_cout << "\nvalues_offset = " << std::dec << control_resp_ref->values_offset();
            atomic_cout << "\nnumber_of_values = " << std::dec << control_resp_ref->number_of_values();
            atomic_cout << "\nsignal_type = " << std::dec << control_resp_ref->signal_type();
            atomic_cout << "\nsignal_index = " << std::dec << control_resp_ref->signal_index();
            atomic_cout << "\nsignal_output = " << std::dec << control_resp_ref->signal_output();
            // atomic_cout << "\nvalue_details = " << std::dec << control_desc->value_details();
            delete (control_resp_ref);
        }
    }
    break;

    default:
        atomic_cout << "\nDescriptor type is not found." << std::endl;
        break;
    }

    return 0;
}

int cmd_line::cmd_read_descriptor(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    avdecc_lib::end_station * end_station;
    if (get_current_end_station(&end_station))
        return 0;

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    intptr_t cmd_notification_id = 0;

    if (desc_type_value < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        end_station->send_read_desc_cmd((void *)cmd_notification_id, desc_type_value, desc_index);
        sys->get_last_resp_status();
    }
    else
    {
        atomic_cout << "cmd_read_descriptor error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_connect(int total_matched, std::vector<cli_argument *> args)
{
    uint8_t * outstream_end_station_name;
    uint8_t * instream_end_station_name;
    const char * format;
    size_t stream_input_desc_count = 0;
    size_t stream_output_desc_count = 0;
    uint64_t end_station_mac;

    atomic_cout << "\n"
                << "End Station" << std::setw(28) << ""
                << "Instream" << std::setw(19) << ""
                << "Stream Format" << std::endl;
    atomic_cout << "---------------------------------------------------------------------------------" << std::endl;

    for (uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(i);
        avdecc_lib::entity_descriptor * entity;
        avdecc_lib::configuration_descriptor * configuration;
        if (get_current_entity_and_descriptor(end_station, &entity, &configuration))
            continue;

        avdecc_lib::entity_descriptor_response * entity_desc_resp = entity->get_entity_response();
        end_station_mac = end_station->mac();
        instream_end_station_name = entity_desc_resp->entity_name();
        stream_input_desc_count = configuration->stream_input_desc_count();

        for (uint32_t j = 0; j < stream_input_desc_count; j++)
        {
            avdecc_lib::stream_input_descriptor * input_descriptor = configuration->get_stream_input_desc_by_index(j);
            avdecc_lib::stream_input_descriptor_response * stream_input_resp_ref = input_descriptor->get_stream_input_response();
            format = stream_input_resp_ref->current_format_name();
            uint8_t * desc_desc_name = stream_input_resp_ref->object_name();
            uint8_t * input_stream_name;
            size_t string_desc_index;
            size_t string_index;
            if (desc_desc_name[0] == '\0')
            {
                int ret = configuration->get_strings_desc_string_by_reference(stream_input_resp_ref->localized_description(),
                                                                              string_desc_index, string_index);
                if (ret == 0)
                {
                    avdecc_lib::strings_descriptor * desc = configuration->get_strings_desc_by_index(string_desc_index);
                    avdecc_lib::strings_descriptor_response * strings_resp_ref = desc->get_strings_response();
                    input_stream_name = strings_resp_ref->get_string_by_index(string_index);

                    atomic_cout << std::setw(5) << i << std::setw(25) << instream_end_station_name
                                << avdecc_lib::utility::end_station_mac_to_string(end_station_mac) << "   "
                                << std::setw(3) << j << std::setw(19) << input_stream_name << "   "
                                << std::setw(14) << format << std::endl;

                    delete strings_resp_ref;
                }
            }
            else
            {
                input_stream_name = desc_desc_name;

                atomic_cout << std::setw(5) << i << std::setw(25) << instream_end_station_name
                            << avdecc_lib::utility::end_station_mac_to_string(end_station_mac) << "   "
                            << std::setw(3) << j << std::setw(19) << input_stream_name << "   "
                            << std::setw(14) << format << std::endl;
            }
            delete stream_input_resp_ref;
        }
        delete entity_desc_resp;
    }

    atomic_cout << "\n"
                << "End Station" << std::setw(28) << ""
                << "Outstream" << std::setw(15) << ""
                << "Stream Format" << std::endl;
    atomic_cout << "---------------------------------------------------------------------------------" << std::endl;

    for (uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(i);
        avdecc_lib::entity_descriptor * entity;
        avdecc_lib::configuration_descriptor * configuration;
        if (get_current_entity_and_descriptor(end_station, &entity, &configuration))
            continue;

        avdecc_lib::entity_descriptor_response * entity_desc_resp = entity->get_entity_response();
        end_station_mac = end_station->mac();
        outstream_end_station_name = entity_desc_resp->entity_name();
        stream_output_desc_count = configuration->stream_output_desc_count();

        for (uint32_t j = 0; j < stream_output_desc_count; j++)
        {
            avdecc_lib::stream_output_descriptor * output_descriptor = configuration->get_stream_output_desc_by_index(j);
            avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = output_descriptor->get_stream_output_response();
            format = stream_output_resp_ref->current_format_name();
            uint8_t * src_desc_name = stream_output_resp_ref->object_name();
            uint8_t * output_stream_name;
            size_t string_desc_index;
            size_t string_index;
            if (src_desc_name[0] == '\0')
            {
                int ret = configuration->get_strings_desc_string_by_reference(stream_output_resp_ref->localized_description(),
                                                                              string_desc_index, string_index);
                if (ret == 0)
                {
                    avdecc_lib::strings_descriptor * desc = configuration->get_strings_desc_by_index(string_desc_index);
                    avdecc_lib::strings_descriptor_response * strings_resp_ref = desc->get_strings_response();
                    output_stream_name = strings_resp_ref->get_string_by_index(string_index);

                    atomic_cout << std::setw(5) << i << std::setw(25) << instream_end_station_name
                                << avdecc_lib::utility::end_station_mac_to_string(end_station_mac) << "   "
                                << std::setw(3) << j << std::setw(19) << output_stream_name
                                << "   " << std::setw(14) << format << std::endl;

                    delete strings_resp_ref;
                }
            }
            else
            {
                output_stream_name = src_desc_name;

                atomic_cout << std::setw(5) << i << std::setw(25) << instream_end_station_name
                            << avdecc_lib::utility::end_station_mac_to_string(end_station_mac) << "   "
                            << std::setw(3) << j << std::setw(19) << output_stream_name
                            << "   " << std::setw(14) << format << std::endl;
            }
            delete stream_output_resp_ref;
        }
        delete entity_desc_resp;
    }

    return 0;
}

int cmd_line::cmd_connect_dst(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t instream_end_station_index = args[0]->get_value_uint();
    uint16_t instream_desc_index = args[1]->get_value_int();

    avdecc_lib::configuration_descriptor * configuration = controller_obj->get_current_config_desc(instream_end_station_index, false);
    bool is_valid = (configuration &&
                     (instream_end_station_index < controller_obj->get_end_station_count()) &&
                     (instream_desc_index < configuration->stream_input_desc_count()));

    if (is_valid)
    {
        uint8_t * outstream_end_station_name;
        uint8_t * src_desc_name;
        const char * format;
        size_t stream_output_desc_count = 0;
        uint64_t end_station_mac;

        atomic_cout << "\n"
                    << "End Station" << std::setw(26) << "   "
                    << "Outstream" << std::setw(16) << "   "
                    << "Stream Format" << std::endl;
        atomic_cout << "------------------------------------------------------------------------------" << std::endl;

        for (uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
        {
            if (i == instream_end_station_index)
            {
                avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(i);
                avdecc_lib::entity_descriptor * entity;
                avdecc_lib::configuration_descriptor * configuration_i;
                if (get_current_entity_and_descriptor(end_station, &entity, &configuration_i))
                    continue;

                end_station_mac = end_station->mac();
                outstream_end_station_name = entity->get_entity_response()->entity_name();
                stream_output_desc_count = configuration_i->stream_output_desc_count();

                for (uint32_t j = 0; j < stream_output_desc_count; j++)
                {
                    avdecc_lib::stream_output_descriptor * output_descriptor = configuration_i->get_stream_output_desc_by_index(j);
                    avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = output_descriptor->get_stream_output_response();
                    src_desc_name = stream_output_resp_ref->object_name();
                    format = stream_output_resp_ref->current_format_name();

                    atomic_cout << std::setw(5) << i << std::setw(20) << outstream_end_station_name
                                << avdecc_lib::utility::end_station_mac_to_string(end_station_mac)
                                << "   " << std::setw(2) << j << std::setw(19) << src_desc_name
                                << "   " << std::setw(10) << format << std::endl;
                    delete (stream_output_resp_ref);
                }
            }
        }
    }
    else
    {
        atomic_cout << "Invalid Instream" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_connect_rx(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t outstream_end_station_index = args[0]->get_value_uint();
    uint16_t outstream_desc_index = args[1]->get_value_int();
    uint32_t instream_end_station_index = args[2]->get_value_uint();
    uint16_t instream_desc_index = args[3]->get_value_int();
    const std::vector<std::string> & flags = args[4]->get_all_value_str();

    avdecc_lib::configuration_descriptor * in_descriptor = controller_obj->get_current_config_desc(instream_end_station_index, false);
    avdecc_lib::configuration_descriptor * out_descriptor = controller_obj->get_current_config_desc(outstream_end_station_index, false);
    bool is_valid = (in_descriptor && out_descriptor &&
                     (test_mode || (instream_end_station_index != outstream_end_station_index)) &&
                     (instream_end_station_index < controller_obj->get_end_station_count()) &&
                     (outstream_end_station_index < controller_obj->get_end_station_count()) &&
                     (instream_desc_index < in_descriptor->stream_input_desc_count()) &&
                     (outstream_desc_index < out_descriptor->stream_output_desc_count()));

    if (is_valid)
    {
        uint16_t connection_flags = 0;
        for (std::vector<std::string>::const_iterator it = flags.begin(); it != flags.end(); ++it)
        {
            const std::string flag = *it;
            if (flag == "class_b")
            {
                connection_flags |= 0x1;
            }
            else if (flag == "fast_connect")
            {
                connection_flags |= 0x2;
            }
            else if (flag == "saved_state")
            {
                connection_flags |= 0x4;
            }
            else if (flag == "streaming_wait")
            {
                connection_flags |= 0x8;
            }
            else if (flag == "supports_encrypted")
            {
                connection_flags |= 0x8;
            }
            else if (flag == "encrypted_pdu")
            {
                connection_flags |= 0x10;
            }
            else if (flag == "talker_failed")
            {
                connection_flags |= 0x10;
            }
            else if (flag != "")
            {
                atomic_cout << "\nInvalid Flag" << std::endl;
                return 0;
            }
        }

        intptr_t cmd_notification_id = 0;
        uint64_t talker_entity_id;
        bool check_stream_format;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * instream = in_descriptor->get_stream_input_desc_by_index(instream_desc_index);
        avdecc_lib::stream_input_descriptor_response * stream_input_resp_ref = instream->get_stream_input_response();
        avdecc_lib::stream_output_descriptor * outstream = out_descriptor->get_stream_output_desc_by_index(outstream_desc_index);
        avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = outstream->get_stream_output_response();
        check_stream_format = (strcmp(stream_input_resp_ref->current_format_name(), stream_output_resp_ref->current_format_name()) == 0);
        if (!check_stream_format)
        {
            atomic_cout << "\n[WARNING] Stream formats do not match. \nInstream has stream format: " << stream_input_resp_ref->current_format_name()
                        << "\nOutstream has stream format: " << stream_output_resp_ref->current_format_name() << std::endl;
        }

        avdecc_lib::end_station * outstream_end_station = controller_obj->get_end_station_by_index(outstream_end_station_index);
        uint16_t outstream_current_entity = outstream_end_station->get_current_entity_index();
        avdecc_lib::entity_descriptor_response * entity_resp_ref = outstream_end_station->get_entity_desc_by_index(outstream_current_entity)->get_entity_response();
        talker_entity_id = entity_resp_ref->entity_id();
        delete (entity_resp_ref);
        instream->send_connect_rx_cmd((void *)cmd_notification_id, talker_entity_id, outstream_desc_index, connection_flags);
        sys->get_last_resp_status();

        delete (stream_input_resp_ref);
    }
    else
    {
        atomic_cout << "Invalid ACMP Connection" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_disconnect_rx(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t outstream_end_station_index = args[0]->get_value_uint();
    uint16_t outstream_desc_index = args[1]->get_value_int();
    uint32_t instream_end_station_index = args[2]->get_value_uint();
    uint16_t instream_desc_index = args[3]->get_value_int();

    avdecc_lib::configuration_descriptor * in_descriptor = controller_obj->get_current_config_desc(instream_end_station_index, false);
    avdecc_lib::configuration_descriptor * out_descriptor = controller_obj->get_current_config_desc(outstream_end_station_index, false);
    bool is_valid = (in_descriptor && out_descriptor &&
                     (test_mode || (instream_end_station_index != outstream_end_station_index)) &&
                     (instream_end_station_index < controller_obj->get_end_station_count()) &&
                     (outstream_end_station_index < controller_obj->get_end_station_count()) &&
                     (instream_desc_index < in_descriptor->stream_input_desc_count()) &&
                     (outstream_desc_index < out_descriptor->stream_output_desc_count()));

    if (is_valid)
    {
        intptr_t cmd_notification_id = 0;
        uint64_t talker_entity_id;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * instream = in_descriptor->get_stream_input_desc_by_index(instream_desc_index);

        avdecc_lib::end_station * outstream_end_station = controller_obj->get_end_station_by_index(outstream_end_station_index);
        uint16_t current_entity = outstream_end_station->get_current_entity_index();
        avdecc_lib::entity_descriptor_response * entity_resp_ref = outstream_end_station->get_entity_desc_by_index(current_entity)->get_entity_response();
        talker_entity_id = entity_resp_ref->entity_id();
        delete (entity_resp_ref);

        instream->send_disconnect_rx_cmd((void *)cmd_notification_id, talker_entity_id, outstream_desc_index);
        sys->get_last_resp_status();
    }
    else
    {
        atomic_cout << "Invalid ACMP Disconnection" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_show_connections(int total_matched, std::vector<cli_argument *> args)
{
    for (uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(i);
        avdecc_lib::entity_descriptor * entity;
        avdecc_lib::configuration_descriptor * configuration;
        if (get_current_entity_and_descriptor(end_station, &entity, &configuration))
            continue;

        size_t stream_input_desc_count = configuration->stream_input_desc_count();
        for (uint32_t j = 0; j < stream_input_desc_count; j++)
        {
            avdecc_lib::stream_input_descriptor * instream = configuration->get_stream_input_desc_by_index(j);
            intptr_t cmd_notification_id = get_next_notification_id();
            sys->set_wait_for_next_cmd((void *)cmd_notification_id);
            instream->send_get_rx_state_cmd((void *)cmd_notification_id);
        }

        size_t stream_output_desc_count = configuration->stream_output_desc_count();

        for (uint32_t j = 0; j < stream_output_desc_count; j++)
        {
            avdecc_lib::stream_output_descriptor * outstream = configuration->get_stream_output_desc_by_index(j);
            intptr_t cmd_notification_id = get_next_notification_id();
            sys->set_wait_for_next_cmd((void *)cmd_notification_id);
            outstream->send_get_tx_state_cmd((void *)cmd_notification_id);
        }
    }

    for (uint32_t in_index = 0; in_index < controller_obj->get_end_station_count(); in_index++)
    {
        avdecc_lib::end_station * in_end_station = controller_obj->get_end_station_by_index(in_index);
        avdecc_lib::entity_descriptor * in_entity;
        avdecc_lib::configuration_descriptor * in_descriptor;
        if (get_current_entity_and_descriptor(in_end_station, &in_entity, &in_descriptor))
            continue;

        size_t stream_input_desc_count = in_descriptor->stream_input_desc_count();
        for (uint32_t in_stream_index = 0; in_stream_index < stream_input_desc_count; in_stream_index++)
        {
            avdecc_lib::stream_input_descriptor * instream = in_descriptor->get_stream_input_desc_by_index(in_stream_index);
            avdecc_lib::stream_input_get_rx_state_response * stream_input_resp_ref = instream->get_stream_input_get_rx_state_response();
            if (!stream_input_resp_ref->get_rx_state_connection_count())
            {
                delete stream_input_resp_ref;
                continue;
            }
            delete stream_input_resp_ref;

            for (uint32_t out_index = 0; out_index < controller_obj->get_end_station_count(); out_index++)
            {
                avdecc_lib::end_station * out_end_station = controller_obj->get_end_station_by_index(out_index);
                avdecc_lib::entity_descriptor * out_entity;
                avdecc_lib::configuration_descriptor * out_descriptor;
                if (get_current_entity_and_descriptor(out_end_station, &out_entity, &out_descriptor))
                    continue;

                size_t stream_output_desc_count = out_descriptor->stream_output_desc_count();
                for (uint32_t out_stream_index = 0; out_stream_index < stream_output_desc_count; out_stream_index++)
                {
                    avdecc_lib::stream_input_get_rx_state_response * stream_input_resp_ref = instream->get_stream_input_get_rx_state_response();
                    avdecc_lib::stream_output_descriptor * outstream = out_descriptor->get_stream_output_desc_by_index(out_stream_index);
                    avdecc_lib::stream_output_get_tx_state_response * stream_output_resp_ref = outstream->get_stream_output_get_tx_state_response();
                    if (!stream_output_resp_ref->get_tx_state_connection_count() ||
                        (stream_input_resp_ref->get_rx_state_stream_id() != stream_output_resp_ref->get_tx_state_stream_id()))
                    {
                        delete stream_input_resp_ref;
                        delete stream_output_resp_ref;

                        continue;
                    }

                    atomic_cout << "0x" << std::setw(16) << std::hex << std::setfill('0') << out_end_station->entity_id()
                                << "[" << in_stream_index << "] -> "
                                << "0x" << std::setw(16) << std::hex << std::setfill('0') << in_end_station->entity_id()
                                << "[" << out_stream_index << "]" << std::endl;

                    delete stream_input_resp_ref;
                    delete stream_output_resp_ref;
                }
            }
        }
    }
    return 0;
}

int cmd_line::cmd_get_tx_state(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t outstream_end_station_index = args[0]->get_value_uint();
    uint16_t outstream_desc_index = args[1]->get_value_int();
    avdecc_lib::configuration_descriptor * configuration = controller_obj->get_current_config_desc(outstream_end_station_index, false);
    bool is_valid = (configuration &&
                     (outstream_end_station_index < controller_obj->get_end_station_count()) &&
                     (outstream_desc_index < configuration->stream_output_desc_count()));

    if (is_valid)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * outstream = configuration->get_stream_output_desc_by_index(outstream_desc_index);
        outstream->send_get_tx_state_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::ACMP_STATUS_SUCCESS)
        {
            avdecc_lib::stream_output_get_tx_state_response * stream_output_resp_ref = outstream->get_stream_output_get_tx_state_response();
            atomic_cout << "\nstream_id = 0x" << std::hex << stream_output_resp_ref->get_tx_state_stream_id();
            atomic_cout << "\nstream_dest_mac = 0x" << std::hex << stream_output_resp_ref->get_tx_state_stream_dest_mac();
            atomic_cout << "\nconnection_count = " << std::dec << stream_output_resp_ref->get_tx_state_connection_count();
            atomic_cout << "\nstream_vlan_id = " << std::dec << stream_output_resp_ref->get_tx_state_stream_vlan_id() << std::endl;
            delete (stream_output_resp_ref);
        }
    }
    else
    {
        atomic_cout << "Invalid GET Talker Connection State" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_rx_state(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t instream_end_station_index = args[0]->get_value_uint();
    uint16_t instream_desc_index = args[1]->get_value_int();
    avdecc_lib::configuration_descriptor * configuration = controller_obj->get_current_config_desc(instream_end_station_index, false);
    bool is_valid = (configuration &&
                     (instream_end_station_index < controller_obj->get_end_station_count()) &&
                     (instream_desc_index < configuration->stream_input_desc_count()));

    if (is_valid)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * instream = configuration->get_stream_input_desc_by_index(instream_desc_index);

        instream->send_get_rx_state_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::ACMP_STATUS_SUCCESS)
        {
            avdecc_lib::stream_input_get_rx_state_response * stream_input_resp_ref = instream->get_stream_input_get_rx_state_response();
            atomic_cout << "\nstream_id = 0x" << std::hex << stream_input_resp_ref->get_rx_state_stream_id();
            atomic_cout << "\ntalker_entity_id = 0x" << std::hex << stream_input_resp_ref->get_rx_state_talker_entity_id();
            atomic_cout << "\ntalker_unique_id = " << std::dec << std::dec << stream_input_resp_ref->get_rx_state_talker_unique_id();
            atomic_cout << "\nlistener_entity_id = 0x" << std::hex << stream_input_resp_ref->get_rx_state_listener_entity_id();
            atomic_cout << "\nlistener_unique_id = " << std::dec << stream_input_resp_ref->get_rx_state_listener_unique_id();
            atomic_cout << "\nstream_dest_mac = 0x" << std::hex << stream_input_resp_ref->get_rx_state_stream_dest_mac();
            atomic_cout << "\nconnection_count = " << std::dec << stream_input_resp_ref->get_rx_state_connection_count();
            atomic_cout << "\nflags = " << std::dec << stream_input_resp_ref->get_rx_state_flags();
            atomic_cout << "\nstream_vlan_id = " << std::dec << stream_input_resp_ref->get_rx_state_stream_vlan_id() << std::endl;
            delete (stream_input_resp_ref);
        }
    }
    else
    {
        atomic_cout << "Invalid Get Listener Connection State" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_tx_connection(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t outstream_end_station_index = args[0]->get_value_uint();
    uint16_t outstream_desc_index = args[1]->get_value_int();
    avdecc_lib::configuration_descriptor * configuration = controller_obj->get_current_config_desc(outstream_end_station_index, false);
    bool is_valid = (configuration &&
                     (outstream_end_station_index < controller_obj->get_end_station_count()) &&
                     (outstream_desc_index < configuration->stream_output_desc_count()));

    if (is_valid)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * outstream = configuration->get_stream_output_desc_by_index(outstream_desc_index);
        outstream->send_get_tx_connection_cmd((void *)cmd_notification_id, 0, 0);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::ACMP_STATUS_SUCCESS)
        {
            avdecc_lib::stream_output_get_tx_connection_response * stream_output_resp_ref = outstream->get_stream_output_get_tx_connection_response();
            atomic_cout << "\nstream_id = 0x" << std::hex << stream_output_resp_ref->get_tx_connection_stream_id();
            atomic_cout << "\nlistener_entity_id = 0x" << std::hex << stream_output_resp_ref->get_tx_connection_listener_entity_id();
            atomic_cout << "\nlistener_unique_id = " << std::dec << stream_output_resp_ref->get_tx_connection_listener_unique_id();
            atomic_cout << "\nstream_dest_mac = 0x" << std::hex << stream_output_resp_ref->get_tx_connection_stream_dest_mac();
            atomic_cout << "\nconnection_count = " << std::dec << stream_output_resp_ref->get_tx_connection_connection_count();
            atomic_cout << "\nstream_vlan_id = " << std::dec << stream_output_resp_ref->get_tx_connection_stream_vlan_id() << std::endl;
            delete (stream_output_resp_ref);
        }
    }
    else
    {
        atomic_cout << "Invalid GET Talker Connection State" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_acquire_entity(int total_matched, std::vector<cli_argument *> args)
{
    std::string flag_name = args[0]->get_value_str();
    std::string desc_name = args[1]->get_value_str();
    uint16_t desc_index = args[2]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    uint32_t flag_id = 0;

    if (flag_name == "acquire")
    {
        flag_id = 0;
    }
    else if (flag_name == "persistent")
    {
        flag_id = 0x1;
    }
    else if (flag_name == "release")
    {
        flag_id = 0x80000000;
    }
    else
    {
        atomic_cout << "\nInvalid flag" << std::endl;
        return 0;
    }

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_ENTITY)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        entity->send_acquire_entity_cmd((void *)cmd_notification_id, flag_id);
        sys->get_last_resp_status();
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_acquire_entity_cmd((void *)cmd_notification_id, flag_id);
        sys->get_last_resp_status();
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();
    }
    else
    {
        atomic_cout << "cmd_acquire_entity error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_lock_entity(int total_matched, std::vector<cli_argument *> args)
{
    std::string flag_name = args[0]->get_value_str();
    std::string desc_name = args[1]->get_value_str();
    //uint16_t desc_index = args[2]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    ;

    uint32_t flag_id;
    if (flag_name == "lock")
    {
        flag_id = 0;
    }
    else if (flag_name == "unlock")
    {
        flag_id = 0x1;
    }
    else
    {
        atomic_cout << "\nInvalid flag" << std::endl;
        return 0;
    }

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_ENTITY)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        entity->send_lock_entity_cmd((void *)cmd_notification_id, flag_id);
        sys->get_last_resp_status();
    }

    return 0;
}

int cmd_line::cmd_entity_avail(int total_matched, std::vector<cli_argument *> args)
{
    avdecc_lib::end_station * end_station;
    if (get_current_end_station(&end_station))
        return 0;

    intptr_t cmd_notification_id = get_next_notification_id();

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    end_station->send_entity_avail_cmd((void *)cmd_notification_id);
    sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_register_unsolicited_notif(int total_matched, std::vector<cli_argument *> args)
{
    intptr_t cmd_notification_id = get_next_notification_id();
    avdecc_lib::end_station * end_station;
    if (get_current_end_station(&end_station))
        return 0;

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    end_station->send_register_unsolicited_cmd((void *)cmd_notification_id);
    sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_deregister_unsolicited_notif(int total_matched, std::vector<cli_argument *> args)
{
    intptr_t cmd_notification_id = get_next_notification_id();
    avdecc_lib::end_station * end_station;
    if (get_current_end_station(&end_station))
        return 0;

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    end_station->send_deregister_unsolicited_cmd((void *)cmd_notification_id);
    sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_reboot(int total_matched, std::vector<cli_argument *> args)
{
    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    intptr_t cmd_notification_id = get_next_notification_id();

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    entity->send_reboot_cmd((void *)cmd_notification_id);
    sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_controller_avail(int total_matched, std::vector<cli_argument *> args)
{
    intptr_t cmd_notification_id = get_next_notification_id();

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    controller_obj->send_controller_avail_cmd((void *)cmd_notification_id, current_end_station);
    sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_get_audio_map(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();
    uint16_t map_index = args[2]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(desc_index);
        stream_port_input_desc_ref->send_get_audio_map_cmd((void *)cmd_notification_id, map_index);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_port_input_get_audio_map_response * stream_port_input_resp_ref = stream_port_input_desc_ref->get_stream_port_input_audio_map_response();
            uint16_t nmappings = stream_port_input_resp_ref->number_of_mappings();

            for (int i = 0; i < (int)nmappings; i++)
            {
                struct avdecc_lib::stream_port_input_audio_mapping map;

                int ret = stream_port_input_resp_ref->mapping(i, map);

                if (ret == 0)
                {
                    atomic_cout << "map[" << i << "].stream_index = " << std::dec << map.stream_index << std::endl;
                    atomic_cout << "map[" << i << "].stream_channel = " << std::dec << map.stream_channel << std::endl;
                    atomic_cout << "map[" << i << "].cluster_offset = " << std::dec << map.cluster_offset << std::endl;
                    atomic_cout << "map[" << i << "].cluster_channel = " << std::dec << map.cluster_channel << std::endl;
                }
            }

            delete stream_port_input_resp_ref;
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(desc_index);
        stream_port_output_desc_ref->send_get_audio_map_cmd((void *)cmd_notification_id, map_index);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_port_output_get_audio_map_response * stream_port_output_resp_ref = stream_port_output_desc_ref->get_stream_port_output_audio_map_response();
            uint16_t nmappings = stream_port_output_resp_ref->number_of_mappings();

            for (int i = 0; i < (int)nmappings; i++)
            {
                struct avdecc_lib::stream_port_output_audio_mapping map;

                int ret = stream_port_output_resp_ref->mapping(i, map);

                if (ret == 0)
                {
                    atomic_cout << "map[" << i << "].stream_index = " << std::dec << map.stream_index << std::endl;
                    atomic_cout << "map[" << i << "].stream_channel = " << std::dec << map.stream_channel << std::endl;
                    atomic_cout << "map[" << i << "].cluster_offset = " << std::dec << map.cluster_offset << std::endl;
                    atomic_cout << "map[" << i << "].cluster_channel = " << std::dec << map.cluster_channel << std::endl;
                }
            }

            delete stream_port_output_resp_ref;
        }
    }
    else
    {
        atomic_cout << "Invalid Descriptor" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_store_pending_audio_mapping(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t stream_index = args[2]->get_value_int();
    uint16_t stream_channel = args[3]->get_value_int();
    uint16_t cluster_offset = args[4]->get_value_int();
    uint16_t cluster_channel = args[5]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    struct avdecc_lib::audio_map_mapping map;

    map.stream_index = stream_index;
    map.stream_channel = stream_channel;
    map.cluster_offset = cluster_offset;
    map.cluster_channel = cluster_channel;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT)
    {
        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc = configuration->get_stream_port_input_desc_by_index(desc_index);
        stream_port_input_desc->store_pending_map(map);
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT)
    {
        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc = configuration->get_stream_port_output_desc_by_index(desc_index);
        stream_port_output_desc->store_pending_map(map);
    }
    else
    {
        std::cout << "invalid descriptor" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_pending_audio_mappings(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT)
    {
        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc = configuration->get_stream_port_input_desc_by_index(desc_index);

        for (int i = 0; i < (int)stream_port_input_desc->get_number_of_pending_maps(); i++)
        {
            struct avdecc_lib::audio_map_mapping map;

            int ret = stream_port_input_desc->get_pending_maps(i, map);

            if (ret == 0)
            {
                atomic_cout << "map[" << i << "].stream_index = " << std::dec << map.stream_index << std::endl;
                atomic_cout << "map[" << i << "].stream_channel = " << std::dec << map.stream_channel << std::endl;
                atomic_cout << "map[" << i << "].cluster_offset = " << std::dec << map.cluster_offset << std::endl;
                atomic_cout << "map[" << i << "].cluster_channel = " << std::dec << map.cluster_channel << std::endl;
            }
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT)
    {
        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc = configuration->get_stream_port_output_desc_by_index(desc_index);

        for (int i = 0; i < (int)stream_port_output_desc->get_number_of_pending_maps(); i++)
        {
            struct avdecc_lib::audio_map_mapping map;

            int ret = stream_port_output_desc->get_pending_maps(i, map);

            if (ret == 0)
            {
                atomic_cout << "map[" << i << "].stream_index = " << std::dec << map.stream_index << std::endl;
                atomic_cout << "map[" << i << "].stream_channel = " << std::dec << map.stream_channel << std::endl;
                atomic_cout << "map[" << i << "].cluster_offset = " << std::dec << map.cluster_offset << std::endl;
                atomic_cout << "map[" << i << "].cluster_channel = " << std::dec << map.cluster_channel << std::endl;
            }
        }
    }
    else
    {
        std::cout << "invalid descriptor" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_clear_pending_audio_mappings(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT)
    {
        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc = configuration->get_stream_port_input_desc_by_index(desc_index);

        stream_port_input_desc->clear_pending_maps();
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT)
    {
        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc = configuration->get_stream_port_output_desc_by_index(desc_index);

        stream_port_output_desc->clear_pending_maps();
    }
    else
    {
        std::cout << "invalid descriptor" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_add_audio_mappings(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT)
    {
        int more_maps_to_send = 1;
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(desc_index);

        while (more_maps_to_send)
        {
            more_maps_to_send = stream_port_input_desc_ref->send_add_audio_mappings_cmd((void *)cmd_notification_id);
            sys->get_last_resp_status();
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT)
    {
        int more_maps_to_send = 1;
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(desc_index);

        while (more_maps_to_send)
        {
            more_maps_to_send = stream_port_output_desc_ref->send_add_audio_mappings_cmd((void *)cmd_notification_id);
            sys->get_last_resp_status();
        }
    }
    else
    {
        atomic_cout << "Invalid Descriptor" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_remove_audio_mappings(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT)
    {
        int more_maps_to_send = 1;
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_port_input_descriptor * stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(desc_index);

        while (more_maps_to_send)
        {
            more_maps_to_send = stream_port_input_desc_ref->send_remove_audio_mappings_cmd((void *)cmd_notification_id);
            sys->get_last_resp_status();
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT)
    {
        int more_maps_to_send = 1;
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_port_output_descriptor * stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(desc_index);

        while (more_maps_to_send)
        {
            more_maps_to_send = stream_port_output_desc_ref->send_remove_audio_mappings_cmd((void *)cmd_notification_id);
            sys->get_last_resp_status();
        }
    }
    else
    {
        atomic_cout << "Invalid Descriptor" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_set_stream_format(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();
    std::string new_stream_format = args[2]->get_value_str();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    unsigned int stream_format_index = 0;
    uint64_t stream_format_value = 0;
    std::string stream_format;

    if (new_stream_format.at(0) == 'I')
    {
        std::string stream_format_substring = new_stream_format.substr(20);
        stream_format_value = avdecc_lib::utility::ieee1722_format_name_to_value(("IEC..." + stream_format_substring).c_str());
    }
    else
    {
        stream_format_index = atoi(new_stream_format.c_str());
        stream_format_value = avdecc_lib::utility::ieee1722_format_index_to_value(stream_format_index);
    }

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_set_stream_format_cmd((void *)cmd_notification_id, stream_format_value);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_input_descriptor_response * stream_input_resp_ref = stream_input_desc_ref->get_stream_input_response();
            stream_format = stream_input_resp_ref->current_format_name();
            if (stream_format == "UNKNOWN")
            {
                atomic_cout << "Stream format: 0x" << std::hex << stream_input_resp_ref->current_format_value() << std::endl;
            }
            else
            {
                atomic_cout << "Stream format: " << stream_format << std::endl;
            }
            delete stream_input_resp_ref;
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_set_stream_format_cmd((void *)cmd_notification_id, stream_format_value);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_output_descriptor_response * stream_output_resp_ref = stream_output_desc_ref->get_stream_output_response();
            stream_format = stream_output_resp_ref->current_format_name();
            if (stream_format == "UNKNOWN")
            {
                atomic_cout << "Stream format: 0x" << std::hex << stream_output_resp_ref->current_format_value() << std::endl;
            }
            else
            {
                atomic_cout << "Stream format: " << stream_format << std::endl;
            }
            delete stream_output_resp_ref;
        }
    }
    else
    {
        atomic_cout << "cmd_set_stream_format error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_stream_format(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    std::string stream_format;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_input_get_stream_format_response * stream_input_resp_ref = stream_input_desc_ref->get_stream_input_get_stream_format_response();
            stream_format = avdecc_lib::utility::ieee1722_format_value_to_name(stream_input_resp_ref->get_stream_format());
            if (stream_format == "UNKNOWN")
            {
                atomic_cout << "Stream format: 0x" << std::hex << stream_input_resp_ref->get_stream_format() << std::endl;
            }
            else
            {
                atomic_cout << "Stream format: " << stream_format << std::endl;
            }
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_output_get_stream_format_response * stream_output_resp_ref = stream_output_desc_ref->get_stream_output_get_stream_format_response();
            stream_format = avdecc_lib::utility::ieee1722_format_value_to_name(stream_output_resp_ref->get_stream_format());

            if (stream_format == "UNKNOWN")
            {
                atomic_cout << "Stream format: 0x" << std::hex << stream_output_resp_ref->get_stream_format() << std::endl;
            }
            else
            {
                atomic_cout << "Stream format: " << stream_format << std::endl;
            }
        }
    }
    else
    {
        atomic_cout << "cmd_get_stream_format error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_set_stream_info(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();
    std::string stream_info_field = args[2]->get_value_str();
    std::string new_stream_info_field_value = args[3]->get_value_str();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    std::string stream_format;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        atomic_cout << "STREAM INPUT unsupported at this time" << std::endl;
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        if (stream_info_field == "stream_vlan_id")
        {
            uint16_t vlan_id = (uint16_t)atoi(new_stream_info_field_value.c_str());
            intptr_t cmd_notification_id = get_next_notification_id();
            sys->set_wait_for_next_cmd((void *)cmd_notification_id);
            avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
            stream_output_desc_ref->send_set_stream_info_vlan_id_cmd((void *)cmd_notification_id, vlan_id);
            int status = sys->get_last_resp_status();
            if (status != avdecc_lib::AEM_STATUS_SUCCESS)
            {
                atomic_cout << "cmd_set_stream_info error" << std::endl;
                return 0;
            }
        }
        else if (stream_info_field == "msrp_accumulated_latency")
        {
            uint32_t msrp_accumulated_latency = (uint32_t)strtoul(new_stream_info_field_value.c_str(), NULL, 0);
            intptr_t cmd_notification_id = get_next_notification_id();
            sys->set_wait_for_next_cmd((void *)cmd_notification_id);
            avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
            stream_output_desc_ref->send_set_stream_info_msrp_accumulated_latency_cmd((void *)cmd_notification_id, msrp_accumulated_latency);
            int status = sys->get_last_resp_status();
            if (status != avdecc_lib::AEM_STATUS_SUCCESS)
            {
                atomic_cout << "cmd_set_stream_info error" << std::endl;
                return 0;
            }
        }
        else
        {
            atomic_cout << "Supported fields are:" << std::endl
                        << "stream_vlan_id, msrp_accumulated_latency" << std::endl;
        }
    }
    else
    {
        atomic_cout << "cmd_set_stream_info invalid descriptor type" << std::endl;
        return 0;
    }
    return 0;
}

int cmd_line::cmd_get_stream_info(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    std::string stream_format;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_get_stream_info_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_input_get_stream_info_response * stream_input_resp_ref = stream_input_desc_ref->get_stream_input_get_stream_info_response();
            stream_format = avdecc_lib::utility::ieee1722_format_value_to_name(stream_input_resp_ref->get_stream_info_stream_format());
            if (stream_format == "UNKNOWN")
            {
                atomic_cout << "Stream format: 0x" << std::hex << stream_input_resp_ref->get_stream_info_stream_format() << std::endl;
            }
            else
            {
                atomic_cout << "Stream format: " << stream_format << std::endl;
            }

            if (stream_input_resp_ref->get_stream_info_flags_stream_id_valid())
                atomic_cout << "Stream ID: 0x" << std::hex << std::setfill('0') << std::setw(16) << stream_input_resp_ref->get_stream_info_stream_id() << std::endl;
            if (stream_input_resp_ref->get_stream_info_flags_stream_dest_mac_valid())
                atomic_cout << "Stream Destination MAC: " << std::hex << stream_input_resp_ref->get_stream_info_stream_dest_mac() << std::endl;
            if (stream_input_resp_ref->get_stream_info_flags_msrp_failure_valid())
            {
                atomic_cout << "Stream MSRP failure code: " << stream_input_resp_ref->get_stream_info_msrp_failure_code() << std::endl;
                atomic_cout << "Stream MSRP failure bridge id " << stream_input_resp_ref->get_stream_info_msrp_failure_bridge_id() << std::endl;
            }
            if (stream_input_resp_ref->get_stream_info_flags_msrp_acc_lat_valid())
                atomic_cout << "Stream MSRP accumulated latency: " << stream_input_resp_ref->get_stream_info_msrp_accumulated_latency() << std::endl;
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_get_stream_info_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::stream_output_get_stream_info_response * stream_output_resp_ref = stream_output_desc_ref->get_stream_output_get_stream_info_response();

            stream_format = avdecc_lib::utility::ieee1722_format_value_to_name(stream_output_resp_ref->get_stream_info_stream_format());

            if (stream_format == "UNKNOWN")
            {
                atomic_cout << "Stream format: 0x" << std::hex << stream_output_resp_ref->get_stream_info_stream_format() << std::endl;
            }
            else
            {
                atomic_cout << "Stream format: " << stream_format << std::endl;
            }

            if (stream_output_resp_ref->get_stream_info_flags_stream_id_valid())
                atomic_cout << "Stream ID: 0x" << std::hex << std::setfill('0') << std::setw(16) << stream_output_resp_ref->get_stream_info_stream_id() << std::endl;
            if (stream_output_resp_ref->get_stream_info_flags_stream_dest_mac_valid())
                atomic_cout << "Stream Destination MAC: " << std::hex << stream_output_resp_ref->get_stream_info_stream_dest_mac() << std::endl;
            if (stream_output_resp_ref->get_stream_info_flags_stream_vlan_id_valid())
                atomic_cout << "Stream VLAN ID: " << stream_output_resp_ref->get_stream_info_stream_vlan_id() << std::endl;
            if (stream_output_resp_ref->get_stream_info_flags_msrp_failure_valid())
            {
                atomic_cout << "Stream MSRP failure code: " << stream_output_resp_ref->get_stream_info_msrp_failure_code() << std::endl;
                atomic_cout << "Stream MSRP failure bridge id " << stream_output_resp_ref->get_stream_info_msrp_failure_bridge_id() << std::endl;
            }
            if (stream_output_resp_ref->get_stream_info_flags_msrp_acc_lat_valid())
                atomic_cout << "Stream MSRP accumulated latency: " << stream_output_resp_ref->get_stream_info_msrp_accumulated_latency() << std::endl;
        }
    }
    else
    {
        atomic_cout << "cmd_get_stream_info error" << std::endl;
    }
    return 0;
}

int cmd_line::cmd_set_group_name(int total_matched, std::vector<cli_argument *> args)
{
    int status = avdecc_lib::AEM_STATUS_NOT_IMPLEMENTED;
    std::string new_name = args[0]->get_value_str();
    uint16_t name_index = 0;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    avdecc_lib::descriptor_base * desc_base = NULL;

    if (get_current_end_station_entity_and_descriptor(&end_station, &entity,
                                                      &configuration))
        return 0;

    desc_base = entity;

    if (!desc_base)
    {
        atomic_cout << "cmd_set_name cannot lookup descriptor" << std::endl;
        return 0;
    }

    name_index = 1;
    struct avdecc_lib::avdecc_lib_name_string64 new_name64 = {{0}};
    strncpy((char *)new_name64.value, new_name.c_str(),
            sizeof(new_name64.value));

    intptr_t cmd_notification_id = get_next_notification_id();
    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    desc_base->send_set_name_cmd((void *)cmd_notification_id, name_index, 0,
                                 +&new_name64);
    status = sys->get_last_resp_status();

    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        cmd_display_desc_name(desc_base, name_index, true);
    }
    else
    {
        atomic_cout << "cmd_set_group_name failed with AEM status: " << avdecc_lib::utility::aem_cmd_status_value_to_name(status) << std::endl;
    }

    return 0;
}

int cmd_line::cmd_set_name(int total_matched, std::vector<cli_argument *> args)
{
    int status = avdecc_lib::AEM_STATUS_NOT_IMPLEMENTED;
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();
    std::string new_name = args[2]->get_value_str();
    bool is_entity = false;

    uint16_t desc_type =
        avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    avdecc_lib::descriptor_base * desc_base = NULL;

    if (get_current_end_station_entity_and_descriptor(&end_station, &entity,
                                                      &configuration))
        return 0;

    if (desc_type == avdecc_lib::AEM_DESC_ENTITY)
    {
        desc_base = entity;
        is_entity = true;
    }
    else if (desc_type == avdecc_lib::AEM_DESC_CONFIGURATION)
    {
        desc_base = dynamic_cast<avdecc_lib::descriptor_base *>(configuration);
    }
    else
    {
        desc_base = configuration->lookup_desc(desc_type, desc_index);
    }

    if (!desc_base)
    {
        atomic_cout << "cmd_set_name cannot lookup descriptor" << std::endl;
        return 0;
    }

    uint16_t name_index = 0;
    struct avdecc_lib::avdecc_lib_name_string64 new_name64 = {{0}};
    strncpy((char *)new_name64.value, new_name.c_str(),
            sizeof(new_name64.value));

    intptr_t cmd_notification_id = get_next_notification_id();
    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    desc_base->send_set_name_cmd((void *)cmd_notification_id, name_index, 0,
                                 +&new_name64);
    status = sys->get_last_resp_status();

    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        cmd_display_desc_name(desc_base, name_index, is_entity);
    }
    else
    {
        atomic_cout << "cmd_set_name failed with AEM status: " << avdecc_lib::utility::aem_cmd_status_value_to_name(status) << std::endl;
    }

    return 0;
}

int cmd_line::cmd_display_desc_name(avdecc_lib::descriptor_base * desc, uint16_t name_index, bool is_entity)
{
    uint8_t * name;
    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;

    if (get_current_end_station_entity_and_descriptor(&end_station, &entity,
                                                      &configuration))
        return 0;

    if (is_entity == true)
    {
        avdecc_lib::entity_descriptor_response * entity_resp_ref = entity->get_entity_response();
        if (name_index == 0)
        {
            name = entity_resp_ref->entity_name();
        }
        else
        {
            name = entity_resp_ref->group_name();
        }
        delete entity_resp_ref;
    }
    else
    {
        avdecc_lib::descriptor_response_base * desc_resp_base = desc->get_descriptor_response();
        name = desc_resp_base->object_name();
        delete desc_resp_base;
    }

    if (!name)
    {
        atomic_cout << "cmd_set_name() failed" << std::endl;
    }
    else
    {
        atomic_cout << "Descriptor " << avdecc_lib::utility::aem_desc_value_to_name(desc->descriptor_type()) << "." << desc->descriptor_index() << " name at index " << name_index << ": " << std::dec << name << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_name(int total_matched, std::vector<cli_argument *> args)
{
    int status = avdecc_lib::AEM_STATUS_NOT_IMPLEMENTED;
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    avdecc_lib::descriptor_base * desc_base = NULL;

    if (get_current_end_station_entity_and_descriptor(&end_station, &entity,
                                                      &configuration))
        return 0;

    if (desc_type == avdecc_lib::AEM_DESC_ENTITY)
    {
        desc_base = dynamic_cast<avdecc_lib::descriptor_base *>(entity);
    }
    else if (desc_type == avdecc_lib::AEM_DESC_CONFIGURATION)
    {
        desc_base = dynamic_cast<avdecc_lib::descriptor_base *>(configuration);
    }
    else
    {
        desc_base = configuration->lookup_desc(desc_type, desc_index);
    }

    if (!desc_base)
    {
        atomic_cout << "cmd_get_name cannot lookup descriptor" << std::endl;
        return 0;
    }

    uint16_t name_index = 0;
    intptr_t cmd_notification_id = get_next_notification_id();
    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    desc_base->send_get_name_cmd((void *)cmd_notification_id, name_index, 0);
    status = sys->get_last_resp_status();

    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        avdecc_lib::descriptor_base_get_name_response * get_name_resp = desc_base->get_name_response();
        uint8_t * name = get_name_resp->get_name();
        if (!name)
        {
            atomic_cout << "cmd_get_name get_name() failed" << std::endl;
        }
        else
        {
            atomic_cout << "Descriptor " << avdecc_lib::utility::aem_desc_value_to_name(desc_base->descriptor_type()) << "." << desc_base->descriptor_index() << " name at index " << name_index << ": " << std::dec << name << std::endl;
        }

        delete get_name_resp;
    }
    else
    {
        atomic_cout << "cmd_get_name failed with AEM status: " << avdecc_lib::utility::aem_cmd_status_value_to_name(status) << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_group_name(int total_matched, std::vector<cli_argument *> args)
{
    int status = avdecc_lib::AEM_STATUS_NOT_IMPLEMENTED;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    avdecc_lib::descriptor_base * desc_base = NULL;

    if (get_current_end_station_entity_and_descriptor(&end_station, &entity,
                                                      &configuration))
        return 0;

    desc_base = dynamic_cast<avdecc_lib::descriptor_base *>(entity);

    if (!desc_base)
    {
        atomic_cout << "cmd_get_name cannot lookup descriptor" << std::endl;
        return 0;
    }

    uint16_t name_index = 1;
    intptr_t cmd_notification_id = get_next_notification_id();
    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    desc_base->send_get_name_cmd((void *)cmd_notification_id, name_index, 0);
    status = sys->get_last_resp_status();

    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        avdecc_lib::descriptor_base_get_name_response * get_name_resp = desc_base->get_name_response();
        uint8_t * name = get_name_resp->get_name();
        if (!name)
        {
            atomic_cout << "cmd_get_group_name get group_name() failed" << std::endl;
        }
        else
        {
            atomic_cout << "Descriptor " << avdecc_lib::utility::aem_desc_value_to_name(desc_base->descriptor_type()) << "." << desc_base->descriptor_index() << " name at index " << name_index << ": " << std::dec << name << std::endl;
        }

        delete get_name_resp;
    }
    else
    {
        atomic_cout << "cmd_get_name failed with AEM status: " << avdecc_lib::utility::aem_cmd_status_value_to_name(status) << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_connection_status(int total_matched, std::vector<cli_argument *> args)
{
    std::string end_station = args[0]->get_value_str();
    uint32_t end_station_index;
    if (get_end_station_index(end_station, end_station_index))
    {
        avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(end_station_index);
        avdecc_lib::entity_descriptor * entity;
        avdecc_lib::configuration_descriptor * configuration;
        if (get_current_entity_and_descriptor(end_station, &entity, &configuration))
        {
            atomic_cout << "End Station " << end_station << " is not fully enumerated." << std::endl;
            return 0;
        }
        
        avdecc_lib::entity_descriptor_response * entity_desc_resp = entity->get_entity_response();
        if (end_station->get_connection_status() == 'C')
            atomic_cout << "End Station " << entity_desc_resp->entity_name() << " is connected." << std::endl;
        else
            atomic_cout << "End Station " << entity_desc_resp->entity_name() << " is disconnected." << std::endl;

        delete entity_desc_resp;
    }
    else
        atomic_cout << "End Station " << end_station << " is not found." << std::endl;

    return 0;
}

int cmd_line::cmd_set_sampling_rate(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();
    uint32_t new_sampling_rate = args[2]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_AUDIO_UNIT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::audio_unit_descriptor * audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(desc_index);
        audio_unit_desc_ref->send_set_sampling_rate_cmd((void *)cmd_notification_id, new_sampling_rate);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::audio_unit_descriptor_response * audio_unit_resp_ref = audio_unit_desc_ref->get_audio_unit_response();
            atomic_cout << "Sampling rate: " << std::dec << audio_unit_resp_ref->current_sampling_rate();
            delete audio_unit_resp_ref;
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_VIDEO_CLUSTER)
    {
        atomic_cout << "\nVideo Cluster descriptor is not implemented." << std::endl;
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_SENSOR_CLUSTER)
    {
        atomic_cout << "\nSensor Cluster descriptor is not implemented." << std::endl;
    }
    else
    {
        atomic_cout << "cmd_set_sampling_rate error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_sampling_rate(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_AUDIO_UNIT)
    {
        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::audio_unit_descriptor * audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(desc_index);
        audio_unit_desc_ref->send_get_sampling_rate_cmd((void *)cmd_notification_id);
        int status = sys->get_last_resp_status();

        if (status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            avdecc_lib::audio_unit_descriptor_response * audio_unit_resp_ref = audio_unit_desc_ref->get_audio_unit_response();
            atomic_cout << "Set sampling rate: " << std::dec << audio_unit_resp_ref->current_sampling_rate();
            delete audio_unit_resp_ref;
        }
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_VIDEO_CLUSTER)
    {
        atomic_cout << "\nVideo Cluster descriptor is not implemented." << std::endl;
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_SENSOR_CLUSTER)
    {
        atomic_cout << "\nSensor Cluster descriptor is not implemented." << std::endl;
    }
    else
    {
        atomic_cout << "cmd_get_sampling_rate error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_counters(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_AVB_INTERFACE)
    {
        avdecc_lib::avb_interface_descriptor * avb_desc_ref = configuration->get_avb_interface_desc_by_index(desc_index);

        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avb_desc_ref->send_get_counters_cmd((void *)cmd_notification_id);
        avdecc_lib::avb_counters_response * avb_interface_counters_resp = avb_desc_ref->get_avb_interface_counters_response();
        if (avb_desc_ref)
        {
            int status = sys->get_last_resp_status();
            if (status == avdecc_lib::AEM_STATUS_SUCCESS)
            {
                if (avb_interface_counters_resp->get_counter_valid(avdecc_lib::AVB_INTERFACE_LINK_UP))
                    atomic_cout << "Link up Counter: " << avb_interface_counters_resp->get_counter_by_name(avdecc_lib::AVB_INTERFACE_LINK_UP)
                                << std::endl;
                if (avb_interface_counters_resp->get_counter_valid(avdecc_lib::AVB_INTERFACE_LINK_DOWN))
                    atomic_cout << "Link down Counter: " << avb_interface_counters_resp->get_counter_by_name(avdecc_lib::AVB_INTERFACE_LINK_DOWN)
                                << std::endl;
                if (avb_interface_counters_resp->get_counter_valid(avdecc_lib::AVB_INTERFACE_FRAMES_TX))
                    atomic_cout << "Frames TX Counter: " << avb_interface_counters_resp->get_counter_by_name(avdecc_lib::AVB_INTERFACE_FRAMES_TX)
                                << std::endl;
                if (avb_interface_counters_resp->get_counter_valid(avdecc_lib::AVB_INTERFACE_FRAMES_RX))
                    atomic_cout << "Frames RX Counter: " << avb_interface_counters_resp->get_counter_by_name(avdecc_lib::AVB_INTERFACE_FRAMES_RX)
                                << std::endl;
                if (avb_interface_counters_resp->get_counter_valid(avdecc_lib::AVB_INTERFACE_RX_CRC_ERROR))
                    atomic_cout << "RX crc error Counter: " << avb_interface_counters_resp->get_counter_by_name(avdecc_lib::AVB_INTERFACE_RX_CRC_ERROR)
                                << std::endl;
                if (avb_interface_counters_resp->get_counter_valid(avdecc_lib::AVB_GPTP_GM_CHANGED))
                    atomic_cout << "GPTP GM Changed Counter: " << avb_interface_counters_resp->get_counter_by_name(avdecc_lib::AVB_GPTP_GM_CHANGED)
                                << std::endl;
                delete avb_interface_counters_resp;
            }
        }
    }

    else if (desc_type_value == avdecc_lib::AEM_DESC_CLOCK_DOMAIN)
    {
        avdecc_lib::clock_domain_descriptor * clock_domain_desc_ref = configuration->get_clock_domain_desc_by_index(desc_index);

        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        clock_domain_desc_ref->send_get_counters_cmd((void *)cmd_notification_id);
        avdecc_lib::clock_domain_counters_response * clock_domain_counters_resp = clock_domain_desc_ref->get_clock_domain_counters_response();
        if (clock_domain_desc_ref)
        {
            int status = sys->get_last_resp_status();
            if (status == avdecc_lib::AEM_STATUS_SUCCESS)
            {
                if (clock_domain_counters_resp->get_counter_valid(avdecc_lib::CLOCK_DOMAIN_LOCKED))
                    atomic_cout << "Locked Counter: " << clock_domain_counters_resp->get_counter_by_name(avdecc_lib::CLOCK_DOMAIN_LOCKED) << std::endl;
                if (clock_domain_counters_resp->get_counter_valid(avdecc_lib::CLOCK_DOMAIN_UNLOCKED))
                    atomic_cout << "Unlocked Counter: " << clock_domain_counters_resp->get_counter_by_name(avdecc_lib::CLOCK_DOMAIN_UNLOCKED) << std::endl;
                delete clock_domain_counters_resp;
            }
        }
    }

    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);

        if (stream_input_desc_ref)
        {
            intptr_t cmd_notification_id = get_next_notification_id();
            sys->set_wait_for_next_cmd((void *)cmd_notification_id);
            stream_input_desc_ref->send_get_counters_cmd((void *)cmd_notification_id);
            avdecc_lib::stream_input_counters_response * stream_input_counters_resp = stream_input_desc_ref->get_stream_input_counters_response();
            if (stream_input_desc_ref)
            {
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_MEDIA_LOCKED))
                    atomic_cout << "Media Locked Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_MEDIA_LOCKED) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_MEDIA_UNLOCKED))
                    atomic_cout << "Media Unlocked Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_MEDIA_UNLOCKED) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_STREAM_RESET))
                    atomic_cout << "Stream Reset Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_STREAM_RESET) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_SEQ_NUM_MISMATCH))
                    atomic_cout << "Seq Num Mismatch Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_SEQ_NUM_MISMATCH) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_MEDIA_RESET))
                    atomic_cout << "Media Reset Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_MEDIA_RESET) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_TIMESTAMP_UNCERTAIN))
                    atomic_cout << "Timestamp Uncertain Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_TIMESTAMP_UNCERTAIN) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_TIMESTAMP_VALID))
                    atomic_cout << "Timestamp Valid Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_TIMESTAMP_VALID) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_TIMESTAMP_NOT_VALID))
                    atomic_cout << "Timestamp Not Valid Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_TIMESTAMP_NOT_VALID) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_UNSUPPORTED_FORMAT))
                    atomic_cout << "Unsupported Format Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_UNSUPPORTED_FORMAT) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_LATE_TIMESTAMP))
                    atomic_cout << "Late Timestamp Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_LATE_TIMESTAMP) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_EARLY_TIMESTAMP))
                    atomic_cout << "Early Timestamp Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_EARLY_TIMESTAMP) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_FRAMES_RX))
                    atomic_cout << "Frames RX Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_FRAMES_RX) << std::endl;
                if (stream_input_counters_resp->get_counter_valid(avdecc_lib::STREAM_INPUT_FRAMES_TX))
                    atomic_cout << "Frames TX Counter: " << stream_input_counters_resp->get_counter_by_name(avdecc_lib::STREAM_INPUT_FRAMES_TX) << std::endl;
                delete stream_input_counters_resp;
            }
        }
    }
    else
    {
        atomic_cout << "Invalid Descriptor" << std::endl;
    }
    return 0;
}

int cmd_line::cmd_set_clock_source(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();
    uint16_t new_clk_src_index = args[2]->get_value_int();

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    intptr_t cmd_notification_id = get_next_notification_id();
    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    avdecc_lib::clock_domain_descriptor * clk_domain_desc_ref = configuration->get_clock_domain_desc_by_index(desc_index);
    if (!clk_domain_desc_ref)
        return 0;

    clk_domain_desc_ref->send_set_clock_source_cmd((void *)cmd_notification_id, new_clk_src_index);
    int status = sys->get_last_resp_status();

    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        avdecc_lib::clock_domain_descriptor_response * clk_domain_resp_ref = clk_domain_desc_ref->get_clock_domain_response();
        atomic_cout << "Set clock source index : " << std::dec << clk_domain_resp_ref->clock_source_index() << std::endl;
        delete clk_domain_resp_ref;
    }

    return 0;
}

uint32_t cmd_line::get_next_notification_id()
{
    return (uint32_t)notification_id++;
}

int cmd_line::cmd_get_clock_source(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    intptr_t cmd_notification_id = get_next_notification_id();
    uint16_t clk_src_index = 0;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    avdecc_lib::clock_domain_descriptor * clk_domain_desc_ref = configuration->get_clock_domain_desc_by_index(desc_index);

    if (!clk_domain_desc_ref)
        return 0;

    avdecc_lib::clock_domain_descriptor_response * clk_domain_resp_ref = clk_domain_desc_ref->get_clock_domain_response();
    clk_domain_desc_ref->send_get_clock_source_cmd((void *)cmd_notification_id);
    int status = sys->get_last_resp_status();

    avdecc_lib::clock_domain_get_clock_source_response * clk_domain_stream_resp_ref = clk_domain_desc_ref->get_clock_domain_get_clock_source_response();
    clk_src_index = clk_domain_stream_resp_ref->get_clock_source_clock_source_index();

    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        atomic_cout << "Clock source index : " << std::dec << clk_domain_resp_ref->get_clock_source_by_index(clk_src_index) << std::endl;
    }

    delete clk_domain_resp_ref;
    delete clk_domain_stream_resp_ref;
    return 0;
}

int cmd_line::cmd_get_avb_info(int total_matched, std::vector<cli_argument *> args)
{
    uint16_t desc_index = args[1]->get_value_int();
    intptr_t cmd_notification_id = get_next_notification_id();

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    avdecc_lib::avb_interface_descriptor * avb_interface_desc_ref = configuration->get_avb_interface_desc_by_index(desc_index);

    if (!avb_interface_desc_ref)
        return 0;

    avb_interface_desc_ref->send_get_avb_info_cmd((void *)cmd_notification_id);
    int status = sys->get_last_resp_status();
    if (status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        avdecc_lib::avb_interface_get_avb_info_response * get_avb_info_resp_ref = avb_interface_desc_ref->get_avb_interface_get_avb_info_response();

        atomic_cout << "gPTP Grandmaster ID: 0x" << std::hex << get_avb_info_resp_ref->get_gptp_grandmaster_id() << std::endl;
        atomic_cout << "Propagation Delay: " << get_avb_info_resp_ref->get_propagation_delay() << std::endl;
        atomic_cout << "gPTP Domain Number: " << std::dec << (unsigned int)get_avb_info_resp_ref->get_gptp_domain_number() << std::endl;
        atomic_cout << "msrp mappings count: " << get_avb_info_resp_ref->get_msrp_mappings_count() << std::endl;

        if (get_avb_info_resp_ref->get_avb_info_flags_as_capable())
            atomic_cout << "as_capable flag: TRUE" << std::endl;
        else
            atomic_cout << "as_capable flag: FALSE" << std::endl;

        if (get_avb_info_resp_ref->get_avb_info_flags_gptp_enabled())
            atomic_cout << "gptp_enabled flag: TRUE" << std::endl;
        else
            atomic_cout << "gptp_enabled flag: FALSE" << std::endl;

        if (get_avb_info_resp_ref->get_avb_info_flags_srp_enabled())
            atomic_cout << "srp_enabled flag: TRUE" << std::endl;
        else
            atomic_cout << "srp_enabled flag: FALSE" << std::endl;

        delete get_avb_info_resp_ref;
    }

    return 0;
}

int cmd_line::cmd_start_streaming(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    intptr_t cmd_notification_id = 0;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_start_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_start_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();
    }
    else
    {
        atomic_cout << "cmd_start_streaming error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_stop_streaming(int total_matched, std::vector<cli_argument *> args)
{
    std::string desc_name = args[0]->get_value_str();
    uint16_t desc_index = args[1]->get_value_int();

    uint16_t desc_type_value = avdecc_lib::utility::aem_desc_name_to_value(desc_name.c_str());
    intptr_t cmd_notification_id = 0;

    avdecc_lib::end_station * end_station;
    avdecc_lib::entity_descriptor * entity;
    avdecc_lib::configuration_descriptor * configuration;
    if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
        return 0;

    if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor * stream_input_desc_ref = configuration->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_stop_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();
    }
    else if (desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor * stream_output_desc_ref = configuration->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_stop_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();
    }
    else
    {
        atomic_cout << "cmd_stop_streaming error" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_firmware_upgrade(int total_matched, std::vector<cli_argument *> args)
{
    std::string image_file_path = args[0]->get_value_str();
    std::ifstream is(image_file_path, std::ios::in | std::ios::binary);

    if (is.is_open())
    {
        avdecc_lib::end_station * end_station;
        avdecc_lib::entity_descriptor * entity;
        avdecc_lib::configuration_descriptor * configuration;
        if (get_current_end_station_entity_and_descriptor(&end_station, &entity, &configuration))
            return 0;

        intptr_t cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::memory_object_descriptor * memory_object_desc_ref = configuration->get_memory_object_desc_by_index(0);
        atomic_cout << "Erasing image..." << std::endl;
        memory_object_desc_ref->start_operation_cmd((void *)cmd_notification_id, 3);
        int status = sys->get_last_resp_status();

        if (status)
        {
            atomic_cout << "Error: Erase failed." << std::endl;
            return 0;
        }
        else
        {
            atomic_cout << "Succesfully erased." << std::endl;
        }

        std::streampos current = 0;
        char buffer[256];
        is.seekg(0, std::ios::beg);

        is.read(buffer, 256);

        while (is.gcount())
        {
            sys->set_wait_for_next_cmd((void *)cmd_notification_id);
            end_station->send_aecp_address_access_cmd((void *)cmd_notification_id,
                                                      1,
                                                      (unsigned int)is.gcount(),
                                                      int(current),
                                                      (uint8_t *)buffer);
            status = sys->get_last_resp_status();
            if (status)
            {
                atomic_cout << "Error: Could not write TLV at address " << current << std::endl;
                return 0;
            }
            current += 256;
            is.read(buffer, 256);
        }

        is.close();

        atomic_cout << "Successfully upgraded image." << std::endl;

        std::string yn;
        atomic_cout << "Do you want to reboot the device? [y/n]: ";
        std::cin >> yn;

        while (1)
        {
            if (yn == "Y" || yn == "y")
            {
                cmd_reboot(0, std::vector<cli_argument *>());
                break;
            }
            else if (yn == "N" || yn == "n")
            {
                break;
            }
            else
            {
                atomic_cout << "Please answer \"y\" or \"n\": ";
                std::cin >> yn;
            }
        }
    }
    else
    {
        atomic_cout << "Error: Unable to open upgrade image at " << image_file_path << std::endl;
        return 0;
    }

    return 0;
}

int cmd_line::cmd_identify_on(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t end_station_index = args[0]->get_value_uint();
    do_identify(end_station_index, true);
    return 0;
}

int cmd_line::cmd_identify_off(int total_matched, std::vector<cli_argument *> args)
{
    uint32_t end_station_index = args[0]->get_value_uint();
    do_identify(end_station_index, false);
    return 0;
}

void cmd_line::do_identify(uint32_t end_station_index, bool turn_on)
{
    if (end_station_index >= controller_obj->get_end_station_count())
    {
        atomic_cout << "Invalid End Station" << std::endl;
    }

    avdecc_lib::end_station * end_station = controller_obj->get_end_station_by_index(end_station_index);

    intptr_t cmd_notification_id = get_next_notification_id();
    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    end_station->send_identify((void *)cmd_notification_id, turn_on);
    sys->get_last_resp_status();
}

int cmd_line::cmd_show_path(int total_matched, std::vector<cli_argument *> args)
{
    atomic_cout << "Log path: " << log_path << std::endl;
    return 0;
}

int cmd_line::cmd_set_path(int total_matched, std::vector<cli_argument *> args)
{
    std::string new_log_path = args[0]->get_value_str();

    log_path = new_log_path;
    return 0;
}

int cmd_line::cmd_clr(int total_matched, std::vector<cli_argument *> args)
{
#if defined(__MACH__) || defined(__linux__)
    std::system("clear");
#else
    std::system("cls");
#endif
    return 0;
}

bool cmd_line::is_setting_valid(uint32_t end_station, uint16_t entity, uint16_t config)
{
    bool is_setting_valid = (end_station < controller_obj->get_end_station_count()) &&
                            (entity < controller_obj->get_end_station_by_index(end_station)->entity_desc_count()) &&
                            (config < controller_obj->get_end_station_by_index(end_station)->get_entity_desc_by_index(entity)->config_desc_count());

    return is_setting_valid;
}

bool cmd_line::get_end_station_index(std::string arg, uint32_t & end_station_index) const
{
    uint64_t entity_entity_id = 0;
    const char * str = arg.c_str();
    char * end;

    // Try treating the argument as a Entity ID
    entity_entity_id = strtoull(str, &end, 16);
    if (end != str)
    {
        bool found = controller_obj->is_end_station_found_by_entity_id(entity_entity_id, end_station_index);
        if (found)
            return true;
    }

    // Not a valid Entity ID, so assume it is an index
    end_station_index = strtoul(str, &end, 10);
    if (end != str)
        return true;

    return false;
}

bool cmd_line::is_output_redirected() const
{
    return output_redirected;
}
