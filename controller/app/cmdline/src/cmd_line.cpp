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

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
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
#include "locale_descriptor.h"
#include "strings_descriptor.h"
#include "stream_port_input_descriptor.h"
#include "stream_port_output_descriptor.h"
#include "audio_cluster_descriptor.h"
#include "audio_map_descriptor.h"
#include "clock_domain_descriptor.h"
#include "cmd_line.h"

avdecc_lib::util *cmd_line::utility;
std::string cmd_line::log_path = "."; // Log to a file in the current working directory

cmd_line::cmd_line() {}

cmd_line::cmd_line(void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
                   void (*log_callback) (void *, int32_t, const char *, int32_t))
{
    current_end_station = 0;
    current_entity = 0;
    current_config = 0;
    notification_id = 0;

    cmd_line_help_init();

    netif = avdecc_lib::create_net_interface();
    controller_obj = avdecc_lib::create_controller(netif, notification_callback, log_callback);
    sys = avdecc_lib::create_system(avdecc_lib::system::LAYER2_MULTITHREADED_CALLBACK, netif, controller_obj);
    utility = avdecc_lib::create_util();

    std::cout << "AVDECC Controller version: " << controller_obj->get_version() << std::endl;
    std::cout << "(c) AudioScience, Inc. 2013\n"<< std::endl;
    print_interfaces_and_select();
    is_setting_valid(current_end_station, current_entity, current_config);
    sys->process_start();
}

cmd_line::~cmd_line()
{
    sys->process_close();
    sys->destroy();
    utility->destroy();
}

int cmd_line::print_interfaces_and_select()
{
    int interface_num;
    char *dev_desc;
    dev_desc = (char *)malloc(256);

    for(uint32_t i = 0; i < netif->devs_count();)
    {
        dev_desc = netif->get_dev_desc_by_index(i);
        printf("%d (%s)\n", ++i, dev_desc);
    }

    std::cout << "Enter the interface number (1-" << std::dec << netif->devs_count() << "): ";
    std::cin >> interface_num;

    netif->select_interface_by_num(interface_num);

    return 0;
}

void cmd_line::cmd_line_help_init()
{
    cmd_line_help_vec.push_back(new cmd_line_help("help",

                                                  "help\n" \
                                                  "Display a list of valid commands."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("version",

                                                  "version\n" \
                                                  "Display the current AVDECC Controller build release version."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("list",

                                                  "list\n" \
                                                  "Display a table with information about each end station."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("select",

                                                  "select\n" \
                                                  "Display the current End Station, Entity, and Configuration setting.\n\n" \
                                                  "select [e_s_i] [e_i] [c_i]\n" \
                                                  "Change the setting of end station, entity, and configuration.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t e_s_i stands for end station and is an integer." \
                                                  "\n\t e_i stands for entity and is an integer. " \
                                                  "\n\t c_i stands for configuration and is integers.\n\n" \
                                                  "To see a list of valid End Stations, enter \"list\" command." \
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("log level",

                                                  "log level [n_l_l]\n" \
                                                  "Update the base log level for messages to be logged by the logging callback.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t n_l_l stands for new log level and is an integer.\n\n" \
                                                  "Valid log levels are 0 - LOGGING_LEVEL_ERROR, 1 - LOGGING_LEVEL_WARNING,\n" \
                                                  "2 - LOGGING_LEVEL_NOTICE, 3 - LOGGING_LEVEL_INFO, 4 - LOGGING_LEVEL_DEBUG\n" \
                                                  "5 - LOGGING_LEVEL_VERBOSE."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("view all",

                                                  "view all\n" \
                                                  "Display all the top level descriptors present in all End Stations."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("view media clock",

                                                  "view media clock\n" \
                                                  "Display a list of descriptors that has the Clock Sync Source flag set."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("view details",

                                                  "view desc [e_s_i]\n" \
                                                  "Display all the descriptors with details in the end station.\n\n" \
                                                  "\nParameters"
                                                  "\n\t e_s_i stands for End Station index and is an integer.\n\n" \
                                                  "To see a list of valid End Stations, enter \"list\" command."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("view descriptor",

                                                  "view descriptor [d_t] [d_i]\n" \
                                                  "Display information for the specified descriptor using the current setting.\n\n" \
                                                  "\nParameters"
                                                  "\n\t d_t stands for descriptor type and is a string." \
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n" \
                                                  "To see a list of valid descriptor types and corresponding indexes, enter\n" \
                                                  "\"view all\" command."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("connect",

                                                  "connect\n" \
                                                  "Display all the available instreams for all End Stations.\n\n"
                                                  "connect [d_e_s_i] [d_d_i]\n" \
                                                  "Display all the available outstreams for all End Stations that can connect with\n" \
                                                  "the instreams.\n\n" \
                                                  "connect [d_e_s_i] [d_d_i] [s_e_s_i] [s_d_i] [f]\n" \
                                                  "Connect an instream to an outstream.\n\n" \
                                                  "\nParameters"
                                                  "\n\t d_e_s_i stands for destination End Station index and is an integer." \
                                                  "\n\t d_d_i stands for destination descriptor index and is an integer."
                                                  "\n\t s_e_s_i stands for source End Station index and is an integer. " \
                                                  "\n\t s_d_i stands for source descriptor index and is an integer. " \
                                                  "\n\t f stands for flags and is an integer.\n\n"
                                                  "Valid flags are Class B, Fast Connect, Saved State, Streaming Wait,\n" \
                                                  "Supports Encrypted, Encrypted PDU, and Talker Failed."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("disconnect",

                                                  "disconnect [d_e_s_i] [d_d_i] [s_e_s_i] [s_d_i]\n" \
                                                  "Send a CONNECT_RX command to disconnect Listener sink stream.\n\n" \
                                                  "\nParameters"
                                                  "\n\t d_e_s_i stands for destination End Station index and is an integer." \
                                                  "\n\t d_d_i stands for destination descriptor index and is an integer."
                                                  "\n\t s_e_s_i stands for source End Station index and is an integer. " \
                                                  "\n\t s_d_i stands for source descriptor index and is an integer."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("get tx state",

                                                  "get tx state [s_e_s_i] [s_d_i]\n" \
                                                  "Send a GET_TX_STATE command to get Talker source stream connection state.\n\n"
                                                  "\nParameters"
                                                  "\n\t s_e_s_i stands for source End Station index and is an integer. " \
                                                  "\n\t s_d_i stands for source descriptor index and is an integer."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("get rx state",

                                                  "get rx state [d_e_s_i] [d_d_i]\n" \
                                                  "Send a GET_RX_STATE command to get Listener sink stream connection state.\n\n"
                                                  "\nParameters"
                                                  "\n\t d_e_s_i stands for destination End Station index and is an integer." \
                                                  "\n\t d_d_i stands for destination descriptor index and is an integer."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("get tx connection",

                                                  "get tx connection [s_e_s_i] [s_d_i]\n" \
                                                  "Send a GET_TX_CONNECTION command with a notification id to get a specific\n" \
                                                  "Talker connection information.\n\n"
                                                  "\nParameters"
                                                  "\n\t s_e_s_i stands for source End Station index and is an integer. " \
                                                  "\n\t s_d_i stands for source descriptor index and is an integer."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("acquire entity",

                                                  "acquire entity [a_e_f] [d_t] [d_i]\n" \
                                                  "Send a ACQUIRE_ENTITY command to obtain exclusive access to an entire Entity\n" \
                                                  "or a sub-tree of objects using the current setting.\n\n"
                                                  "\nParameters" \
                                                  "\n\t a_e_f stands for Acquire Entity Flag and is a string." \
                                                  "\n\t d_t stands for descriptor type and is a string." \
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n" \
                                                  "Valid Acquire Entity Flags are acquire, persistent, and release.\n" \
                                                  "To see a list of valid descriptor types and corresponding indexes, enter\n" \
                                                  "\"view all\" command."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("lock entity",

                                                  "lock entity [l_e_f] [d_t] [d_i]\n" \
                                                  "Send a LOCK_ENTITY command to provide short term exclusive access to the\n" \
                                                  "AVDECC Entity to perform atomic operations using the current setting.\n\n"
                                                  "\nParameters" \
                                                  "\n\t l_e_f stands for Lock Entity Flag and is a string." \
                                                  "\n\t d_t stands for descriptor type and is a string." \
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n" \
                                                  "Valid Lock Entity Flags are lock and unlock.\n" \
                                                  "To see a list of valid descriptor types and corresponding indexes, enter\n" \
                                                  "\"view all\" command."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("entity available",

                                                  "entity available\n" \
                                                  "Send a ENTITY_AVAILABLE command to determine if another AVDECC Entity is\n" \
                                                  "still alive and responding to commands."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("controller available",

                                                  "controller available\n" \
                                                  "Send a CONTROLLER_AVAILABLE command to determine if an AVDECC Controller is\n" \
                                                  "still alive."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("read descriptor",

                                                  "read descriptor [d_t] [d_i]\n" \
                                                  "Send a READ_DESCRIPTOR command to read a descriptor from an AVDECC Entity\n" \
                                                  "using the current setting.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string." \
                                                  "\n\t d_i stands for descriptor index and is an integer."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("set stream_format",

                                                  "set stream_format [d_t] [d_i] [s_f]\n" \
                                                  "Send a SET_STREAM_FORMAT command to change the format of a stream using the\n" \
                                                  "current setting.\n\n"
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string." \
                                                  "\n\t d_i stands for descriptor indexand is an integer." \
                                                  "\n\t s_f stands for stream format and is a string.\n\n" \
                                                  "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("get stream_format",

                                                  "get stream_format [d_t] [d_i]\n" \
                                                  "Send a GET_STREAM_FORMAT command to display the current format of a stream\n" \
                                                  "using the current setting.\n\n"
                                                  "\nParameters"
                                                  "\n\t d_t stands for descriptor type and is a string."
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n" \
                                                  "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."
                                                 ));

#if 0 
    cmd_line_help_vec.push_back(new cmd_line_help("set stream_info",

    					      "set stream_info [d_t] [d_i] [f] [f_v]\n" \
    					      "Send a SET_STREAM_INFO command to change a stream info field value to a new\n" \
    					      "value using the current setting.\n\n"
    					      "\nParameters" \
    					      "\n\t d_t stands for descriptor type and is a string." \
    					      "\n\t d_i stands for descriptor index and is an integer." \
    					      "\n\t f stands for field and is a string." \
    					      "\n\t f_v stands for field value and is an integer.\n\n" \
    					      "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT.\n" \
    					      "Valid fields are stream_id, msrp_accumulated_latency, and stream_dest_mac."
    					      ));
#endif

    cmd_line_help_vec.push_back(new cmd_line_help("get stream_info",

                                                  "get stream_info [d_t] [d_i] [f]\n" \
                                                  "Display the GET_STREAM_INFO Stream ID or the MSRP Accumulated Latency field\n" \
                                                  "using the current setting.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string."
                                                  "\n\t d_i stands for descriptor index and is an integer."
                                                  "\n\t f stands for stream info field and is a string.\n\n" \
                                                  "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT.\n" \
                                                  "Valid fields are stream_id, msrp_accumulated_latency, and stream_dest_mac."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("set sampling_rate",

                                                  "set sampling_rate [d_t] [d_i]\n" \
                                                  "Send a SET_SAMPLING_RATE command to change the sampling rate of a port or unit.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string."
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n"
                                                  "Valid descriptor types are AUDIO_UNIT, VIDEO_CLUSTER, SENSOR_CLUSTER."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("get sampling_rate",

                                                  "get sampling_rate [d_t] [d_i]\n" \
                                                  "Send a GET_SAMPLING_RATE command to get the current sampling rate of a\n" \
                                                  "port or unit.\n\n"
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string." \
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n"
                                                  "Valid descriptor types are AUDIO_UNIT, VIDEO_CLUSTER, SENSOR_CLUSTER."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("set clock_source",

                                                  "set clock_source [d_t] [d_i] [c_s_i]\n" \
                                                  "Send a SET_CLOCK_SOURCE command to change the clock source of a clock domain.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string."
                                                  "\n\t d_i stands for descriptor index and is an integer."
                                                  "\n\t c_s_i stands for Clock Source index and is an integer. \n\n"
                                                  "Valid descriptor type is CLOCK_DOMAIN."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("get clock_source",

                                                  "get clock_source [d_t] [d_i]\n" \
                                                  "Send a GET_CLOCK_SOURCE command to get the current clock source of a clock\n" \
                                                  "domain.\n\n"
                                                  "\nParameters" \
                                                  "\n\t d_t stands for descriptor type and is a string."
                                                  "\n\t d_i stands for descriptor index and is an integer.\n\n"
                                                  "Valid descriptor type is CLOCK_DOMAIN."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("start streaming",

    					                          "start streaming [d_t] [d_i]\n" \
    					                          "Send a START_STREAMING command to start streaming on a previously connected\n" \
    					                          "stream that was connected via ACMP or has previously been stopped with the\n" \
    					                          "STOP_STREAMING command.\n\n" \
    					                          "\nParameters" \
    					                          "\n\t d_t stands for descriptor type and is a string." \
    					                          "\n\t d_i stands for descriptor index and is an integer.\n\n"
    					                          "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."
    					                          ));

    cmd_line_help_vec.push_back(new cmd_line_help("stop streaming",

    					                          "stop streaming [d_t] [d_i]\n" \
    					                          "Send a STOP_STREAMING command to stop a connected stream for streaming media.\n\n" \
    					                          "\nParameters" \
    					                          "\n\t d_t stands for descriptor type and is a string." \
    					                          "\n\t d_i standsfor descriptor index and is an integer.\n\n"
    					                          "Valid descriptor types are STREAM_INPUT and STREAM_OUTPUT."
    					                          ));
#if 0
    cmd_line_help_vec.push_back(new cmd_line_help("set name",

    					                          "set name [d_t] [d_i] [ni] [n]\n" \
    					                          "Send a SET_NAME command to change the value of a name field within a descriptor" \
    					                          "using the current setting.\n\n"
    					                          "\nParameters" \
    					                          "\n\t d_t stands for descriptor type and is a string." \
    					                          "\n\t d_i stands for descriptor index and is an integer." \
    					                          "\n\t ni stands for name index and is an integer." \
    					                          "\n\t n stands for name and is a string."
    					                          ));
#endif

#if 0
    cmd_line_help_vec.push_back(new cmd_line_help("get name",

    					                          "get name [d_t] [d_i] [ni]\n" \
    					                          "Send a GET_NAME command to fetch the value of a name field within a descriptor" \
    					                          "using the current setting.\n\n"
    				                                  "\nParameters" \
    					                          "\n\t d_t stands for descriptor type and is a string." \
    					                          "\n\t d_i stands for descriptor index and is an integer." \
    					                          "\n\t ni stands for name index and is an integer."
    					                          ));
#endif

    cmd_line_help_vec.push_back(new cmd_line_help("path",

                                                  "path\n" \
                                                  "Display the location of the redirected output file.\n\n"
                                                  "path [p_n]\n" \
                                                  "Change the location of the redirected output file.\n\n" \
                                                  "\nParameters" \
                                                  "p_n stands for path name and is a string."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("log",

                                                  "log [f_n]\n" \
                                                  "Redirect output to a specified file.\n\n" \
                                                  "\nParameters" \
                                                  "\n\t f_n stands for file name and is a string."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("unlog",

                                                  "unlog\n" \
                                                  "Set output scheme back to console screen."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("clr",

                                                  "clr\n" \
                                                  "Clear the console screen."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("quit",

                                                  "quit\n" \
                                                  "Exit out of the execution."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("q",

                                                  "q\n" \
                                                  "Exit out of the execution."
                                                 ));

    cmd_line_help_vec.push_back(new cmd_line_help("param",

                                                  "Parameters\n" \
                                                  "[e_s_i] is the index of the End Station and is an integer.\n" \
                                                  "[e_i] is the index of the Entity and is an integer.\n" \
                                                  "[c_i] is the index of the Configuration and is an integer.\n" \
                                                  "[d_t] is the type of the descriptor and is a string.\n" \
                                                  "[d_i] is the index of the descriptor and is an integer.\n\n" \
                                                  "To see a list of valid End Stations, enter \"list\" command.\n" \
                                                  "To see a list of valid descriptor types and corresponding indexes," \
                                                  "enter \"view all\" command."
                                                 ));
}

int cmd_line::cmd_help()
{
    std::cout << "Commands" << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;

    for(uint32_t i = 0; i < cmd_line_help_vec.size(); i++)
    {
        std::cout << cmd_line_help_vec.at(i)->get_command() << std::endl;
    }

    std::cout << "------------------------------------------------------------------------------" << std::endl;
    std::cout << "Type \"help command\" for details"<< std::endl;

    return 0;
}

int cmd_line::cmd_help_details(std::string cmd)
{
    for(uint32_t i = 0; i < cmd_line_help_vec.size(); i++)
    {
        bool cmd_match = ((cmd_line_help_vec.at(i)->get_command()) == cmd);

        if(cmd_match)
        {
            std::cout << cmd_line_help_vec.at(i)->get_description() << std::endl;
            return 1;
        }
    }

    std::cout << "Type \"help\" for a list of available commands" << std::endl;

    return -1;
}

int cmd_line::cmd_version()
{
    std::cout << "AVDECC Controller version: " << controller_obj->get_version() << std::endl;
    return 0;
}

int cmd_line::cmd_list()
{
    std::cout << "\n" << "End Station" << "  |  " << "Name" << std::setw(21)  << "  |  " <<  "Entity GUID" << std::setw(12) << "  |  " << "MAC" << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;

    for(unsigned int i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::end_station *endstn = controller_obj->get_end_station_by_index(i);

        if (endstn)
        {
            uint64_t end_station_guid = endstn->guid();
            avdecc_lib::entity_descriptor *ent_desc = NULL;
            if (endstn->entity_desc_count())
            {
                ent_desc = endstn->get_entity_desc_by_index(current_entity);
            }
            char *end_station_name;
            if (ent_desc)
            {
                end_station_name = (char *)ent_desc->entity_name();
            }
            uint64_t end_station_mac = endstn->mac();
            std::cout << (std::stringstream() << endstn->get_connection_status()
                      << std::setw(10) << std::dec << std::setfill(' ') << i << "  |  "
                      << std::setw(20) << std::hex << std::setfill(' ') << (ent_desc ? end_station_name : "UNKNOWN") << "  |  0x"
                      << std::setw(16) << std::hex << std::setfill('0') << end_station_guid << "  |  0x"
                      << std::setw(12) << std::hex << std::setfill('0') << end_station_mac).rdbuf() << std::endl;
        }
    }

    std::cout << "\nC - End Station Connected." << std::endl;
    std::cout << "D - End Station Disconnected." << std::endl;

    return 0;
}

int cmd_line::cmd_view_media_clock()
{
    uint8_t *desc_obj_name;
    uint16_t desc_type_value = 0;
    uint16_t desc_index = 0;
    bool is_clock_sync_source_set = false;

    std::cout << "\n" << "End Station" << "  " << std::setw(20) << "Descriptor Name" << "  " <<
              std::setw(18) << "Descriptor Type" << "  " << std::setw(18) << "Descriptor Index" << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;

    for(unsigned int i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        avdecc_lib::configuration_descriptor *config_desc = controller_obj->get_config_desc_by_index(i, current_entity, current_config);

        if (config_desc)
        {
            for(unsigned int j = 0; j < config_desc->stream_input_desc_count(); j++)
            {
                avdecc_lib::stream_input_descriptor *stream_input_desc = config_desc->get_stream_input_desc_by_index(j);
                if (stream_input_desc)
                {
                    is_clock_sync_source_set = stream_input_desc->stream_flags_clock_sync_source();
                    if(is_clock_sync_source_set)
                    {
                        desc_obj_name = stream_input_desc->object_name();
                        desc_type_value = stream_input_desc->descriptor_type();
                        desc_index = stream_input_desc->descriptor_index();

                        std::cout << std::setw(8) << i << std::setw(5) << "" << std::setw(20) << desc_obj_name <<
                                  "  " << std::setw(18) << utility->aem_desc_value_to_name(desc_type_value) <<
                                  "  " << std::setw(16) << std::dec << desc_index << std::endl;
                    }
                }
            }

            for(unsigned int j = 0; j < config_desc->stream_output_desc_count(); j++)
            {
                avdecc_lib::stream_output_descriptor *stream_output_desc = config_desc->get_stream_output_desc_by_index(j);
                if (stream_output_desc)
                {
                    is_clock_sync_source_set = stream_output_desc->stream_flags_clock_sync_source();
                    if(is_clock_sync_source_set)
                    {
                        desc_obj_name = stream_output_desc->object_name();
                        desc_type_value = stream_output_desc->descriptor_type();
                        desc_index = stream_output_desc->descriptor_index();

                        std::cout << std::setw(8) << i << std::setw(5) << "" << std::setw(20) << desc_obj_name <<
                                  "  " << std::setw(18) << std::hex << utility->aem_desc_value_to_name(desc_type_value) <<
                                  "  " << std::setw(16) << std::dec << desc_index << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}

void cmd_line::cmd_select()
{
    std::cout << "Current setting" << std::endl;
    std::cout << "\tEnd Station: " << std::dec << current_end_station << " (" << controller_obj->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity)->entity_name() << ")" << std::endl;
    std::cout << "\tEntity: " << std::dec << current_entity << std::endl;
    std::cout << "\tConfiguration: " << std::dec << current_config << std::endl;
}

int cmd_line::cmd_select(uint32_t new_end_station, uint16_t new_entity, uint16_t new_config)
{
    if(is_setting_valid(new_end_station, new_entity, new_config)) // Check if the new setting is valid
    {
        uint8_t *end_station_name = controller_obj->get_end_station_by_index(new_end_station)->get_entity_desc_by_index(current_entity)->entity_name();

        if((current_end_station == new_end_station) && (current_entity == new_entity) && (current_config == new_config))
        {
            std::cout << "Same setting" << std::endl;
            std::cout << "\tEnd Station: " << std::dec << current_end_station << " (" << end_station_name << ")" << std::endl;
            std::cout << "\tEntity: " << std::dec << current_entity << std::endl;
            std::cout << "\tConfiguration: " << std::dec << current_config << std::endl;
        }
        else
        {
            current_end_station = new_end_station;
            current_entity = new_entity;
            current_config = new_config;
            std::cout << "New setting" << std::endl;
            std::cout << "\tEnd Station: " << std::dec << current_end_station << " (" << end_station_name << ")" << std::endl;
            std::cout << "\tEntity: " << std::dec << current_entity << std::endl;
            std::cout << "\tConfiguration: " << std::dec << current_config << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid new setting" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_log_level(uint32_t new_log_level)
{
    if(new_log_level < avdecc_lib::TOTAL_NUM_OF_LOGGING_LEVELS)
    {
        controller_obj->set_logging_level(new_log_level);
    }
    else
    {
        std::cout << "Invalid new log level" << std::endl;
    }

    return 0;
}


void cmd_line::print_desc_type_index_name_row(avdecc_lib::descriptor_base &desc,
                                              avdecc_lib::strings_descriptor &strings,
                                              avdecc_lib::locale_descriptor &locale)
{
    const uint8_t localized_string_max_index = 7;

    std::cout << std::setw(20) << utility->aem_desc_value_to_name(desc.descriptor_type());
    std::cout << "   "<<  std::setw(16) << std::dec << desc.descriptor_index();

    if((desc.descriptor_type() == avdecc_lib::AEM_DESC_STREAM_PORT_INPUT) ||
       (desc.descriptor_type() == avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT) ||
       (desc.descriptor_type() == avdecc_lib::AEM_DESC_AUDIO_MAP))
    {
        std::cout << "   " << std::endl;
    }
    else
    {
        uint8_t localized_desc_index = (desc.localized_description()) & 0x7; // The 3 bit index subfield defining the index of the string within the STRINGS descriptor
        if(localized_desc_index < localized_string_max_index)
        {
            std::cout << "   " << std::setw(20) << std::hex << strings.get_string_by_index(localized_desc_index) << std::endl;
        }
        else
        {
            std::cout << "   " << std::setw(20) << std::hex << desc.object_name() << std::endl;
        }
    }
}

int cmd_line::cmd_view_all()
{
    uint8_t *obj_name = NULL;

    for(uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        obj_name = controller_obj->get_end_station_by_index(i)->get_entity_desc_by_index(current_entity)->entity_name();
        std::cout << "\nEnd Station: " << i << " (" << obj_name << ")" << std::endl;
        std::cout << std::setw(20) << "Descriptor Type" << "   " << std::setw(16)  <<  "Descriptor Index" << "   " << std::setw(20) << "Descriptor Name" << std::endl;
        std::cout << "------------------------------------------------------------------------------" << std::endl;

        avdecc_lib::entity_descriptor *entity = controller_obj->get_end_station_by_index(i)->get_entity_desc_by_index(current_entity);
        avdecc_lib::configuration_descriptor *configuration = entity->get_config_desc_by_index(current_config);
        avdecc_lib::locale_descriptor *locale = NULL;
        avdecc_lib::strings_descriptor *strings = NULL;

        if(configuration->locale_desc_count() >= 1)
        {
            locale = configuration->get_locale_desc_by_index(0);
        }

        if(configuration->strings_desc_count() >= 1)
        {
            strings = configuration->get_strings_desc_by_index(0);
        }

        switch(0)
        {
            case avdecc_lib::AEM_DESC_ENTITY:
                {
                    std::cout << std::setw(20) << std::hex << utility->aem_desc_value_to_name(entity->descriptor_type());
                    std::cout << "   " << std::setw(16) << std::dec << entity->descriptor_index();
                    std::cout << "   " << std::setw(20) << std::hex << entity->entity_name() << std::endl;
                }

            case avdecc_lib::AEM_DESC_CONFIGURATION:
                {
                    std::cout << std::setw(20) << utility->aem_desc_value_to_name(configuration->descriptor_type());
                    std::cout << "   " << std::setw(16) << std::dec << configuration->descriptor_index();
                    std::cout << "   " << std::setw(20) << std::hex << configuration->object_name() << std::endl;
                    std::cout << "\nTop Level Descriptors" << std::endl;
                }

            case avdecc_lib::AEM_DESC_AUDIO_UNIT:
                for(unsigned int j = 0; j < configuration->audio_unit_desc_count(); j++)
                {
                    avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(j);
                    print_desc_type_index_name_row(*audio_unit_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_STREAM_INPUT:
                for(unsigned int j = 0; j < configuration->stream_input_desc_count(); j++)
                {
                    avdecc_lib::stream_input_descriptor *stream_input_desc_ref = configuration->get_stream_input_desc_by_index(j);
                    print_desc_type_index_name_row(*stream_input_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
                for(unsigned int j = 0; j < configuration->stream_output_desc_count(); j++)
                {
                    avdecc_lib::stream_output_descriptor *stream_output_desc_ref = configuration->get_stream_output_desc_by_index(j);
                    print_desc_type_index_name_row(*stream_output_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_JACK_INPUT:
                for(unsigned int j = 0; j < configuration->jack_input_desc_count(); j++)
                {
                    avdecc_lib::jack_input_descriptor *jack_input_desc_ref = configuration->get_jack_input_desc_by_index(j);
                    print_desc_type_index_name_row(*jack_input_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_JACK_OUTPUT:
                for(unsigned int j = 0; j < configuration->jack_output_desc_count(); j++)
                {
                    avdecc_lib::jack_output_descriptor *jack_output_desc_ref = configuration->get_jack_output_desc_by_index(j);
                    print_desc_type_index_name_row(*jack_output_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_AVB_INTERFACE:
                for(unsigned int j = 0; j < configuration->avb_interface_desc_count(); j++)
                {
                    avdecc_lib::avb_interface_descriptor *avb_interface_desc_ref = configuration->get_avb_interface_desc_by_index(j);
                    print_desc_type_index_name_row(*avb_interface_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
                for(unsigned int j = 0; j < configuration->clock_source_desc_count(); j++)
                {
                    avdecc_lib::clock_source_descriptor *clk_src_desc_ref = configuration->get_clock_source_desc_by_index(j);
                    print_desc_type_index_name_row(*clk_src_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_LOCALE:
                for(unsigned int j = 0; j < configuration->locale_desc_count(); j++)
                {
                    avdecc_lib::locale_descriptor *locale_def_ref = configuration->get_locale_desc_by_index(j);
                    std::cout << std::setw(20) << utility->aem_desc_value_to_name(locale->descriptor_type());
                    std::cout << "   "<<  std::setw(16) << std::hex << locale_def_ref->descriptor_index();
                    std::cout << "   " << std::setw(20) << std::hex << locale_def_ref->locale_identifier() << std::endl;
                }

                //case avdecc_lib::AEM_DESC_STRINGS:
                //	for(int j = 0; j < configuration->strings_desc_count(); j++)
                //	{
                //		avdecc_lib::strings_descriptor *strings = configuration->get_strings_desc_by_index(j);
                //		print_desc_type_index_name_row(*strings, *strings, *locale);
                //	}

            case avdecc_lib::AEM_DESC_STREAM_PORT_INPUT:
                for(unsigned int j = 0; j < configuration->stream_port_input_desc_count(); j++)
                {
                    avdecc_lib::stream_port_input_descriptor *stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(j);
                    print_desc_type_index_name_row(*stream_port_input_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT:
                for(unsigned int j = 0; j < configuration->stream_port_output_desc_count(); j++)
                {
                    avdecc_lib::stream_port_output_descriptor *stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(j);
                    print_desc_type_index_name_row(*stream_port_output_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_AUDIO_CLUSTER:
                for(unsigned int j = 0; j < configuration->audio_cluster_desc_count(); j++)
                {
                    avdecc_lib::audio_cluster_descriptor *audio_cluster_desc_ref = configuration->get_audio_cluster_desc_by_index(j);
                    print_desc_type_index_name_row(*audio_cluster_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_AUDIO_MAP:
                for(unsigned int j = 0; j < configuration->audio_map_desc_count(); j++)
                {
                    avdecc_lib::audio_map_descriptor *audio_map_desc_ref = configuration->get_audio_map_desc_by_index(j);
                    print_desc_type_index_name_row(*audio_map_desc_ref, *strings, *locale);
                }

            case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
                for(unsigned int j = 0; j < configuration->clock_domain_desc_count(); j++)
                {
                    avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = configuration->get_clock_domain_desc_by_index(j);
                    print_desc_type_index_name_row(*clk_domain_desc_ref, *strings, *locale);
                }

                break;
        }
    }

    return 0;
}

int cmd_line::cmd_view_details(uint32_t end_station_index)
{
    std::string desc_name;
    uint16_t desc_index;
    avdecc_lib::entity_descriptor *entity = controller_obj->get_end_station_by_index(end_station_index)->get_entity_desc_by_index(current_entity);
    avdecc_lib::configuration_descriptor *configuration = entity->get_config_desc_by_index(current_config);

    std::cout << "\nEnd Station: " << end_station_index << " (" << entity->entity_name() << ")" << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;

    switch(0)
    {
        case avdecc_lib::AEM_DESC_ENTITY:
            {
                desc_name = utility->aem_desc_value_to_name(entity->descriptor_type());
                desc_index = entity->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_CONFIGURATION:
            {
                desc_name = utility->aem_desc_value_to_name(configuration->descriptor_type());
                desc_index = configuration->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_AUDIO_UNIT:
            for(unsigned int j = 0; j < configuration->audio_unit_desc_count(); j++)
            {
                avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = configuration->get_audio_unit_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(audio_unit_desc_ref->descriptor_type());
                desc_index = audio_unit_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);

            }

        case avdecc_lib::AEM_DESC_STREAM_INPUT:
            for(unsigned int j = 0; j < configuration->stream_input_desc_count(); j++)
            {
                avdecc_lib::stream_input_descriptor *stream_input_desc_ref = configuration->get_stream_input_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(stream_input_desc_ref->descriptor_type());
                desc_index = stream_input_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
            for(unsigned int j = 0; j < configuration->stream_output_desc_count(); j++)
            {
                avdecc_lib::stream_output_descriptor *stream_output_desc_ref = configuration->get_stream_output_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(stream_output_desc_ref->descriptor_type());
                desc_index = stream_output_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_JACK_INPUT:
            for(unsigned int j = 0; j < configuration->jack_input_desc_count(); j++)
            {
                avdecc_lib::jack_input_descriptor *jack_input_desc_ref = configuration->get_jack_input_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(jack_input_desc_ref->descriptor_type());
                desc_index = jack_input_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_JACK_OUTPUT:
            for(unsigned int j = 0; j < configuration->jack_output_desc_count(); j++)
            {
                avdecc_lib::jack_output_descriptor *jack_output_desc_ref = configuration->get_jack_output_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(jack_output_desc_ref->descriptor_type());
                desc_index = jack_output_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_AVB_INTERFACE:
            for(unsigned int j = 0; j < configuration->avb_interface_desc_count(); j++)
            {
                avdecc_lib::avb_interface_descriptor *avb_interface_desc_ref = configuration->get_avb_interface_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(avb_interface_desc_ref->descriptor_type());
                desc_index = avb_interface_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
            for(unsigned int j = 0; j < configuration->clock_source_desc_count(); j++)
            {
                avdecc_lib::clock_source_descriptor *clk_src_desc_ref = configuration->get_clock_source_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(clk_src_desc_ref->descriptor_type());
                desc_index = clk_src_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_LOCALE:
            for(unsigned int j = 0; j < configuration->locale_desc_count(); j++)
            {
                avdecc_lib::locale_descriptor *locale_def_ref = configuration->get_locale_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(locale_def_ref->descriptor_type());
                desc_index = locale_def_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_STRINGS:
            for(unsigned int j = 0; j < configuration->strings_desc_count(); j++)
            {
                avdecc_lib::strings_descriptor *strings = configuration->get_strings_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(strings->descriptor_type());
                desc_index = strings->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_STREAM_PORT_INPUT:
            for(unsigned int j = 0; j < configuration->stream_port_input_desc_count(); j++)
            {
                avdecc_lib::stream_port_input_descriptor *stream_port_input_desc_ref = configuration->get_stream_port_input_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(stream_port_input_desc_ref->descriptor_type());
                desc_index = stream_port_input_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT:
            for(unsigned int j = 0; j < configuration->stream_port_output_desc_count(); j++)
            {
                avdecc_lib::stream_port_output_descriptor *stream_port_output_desc_ref = configuration->get_stream_port_output_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(stream_port_output_desc_ref->descriptor_type());
                desc_index = stream_port_output_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_AUDIO_CLUSTER:
            for(unsigned int j = 0; j < configuration->audio_cluster_desc_count(); j++)
            {
                avdecc_lib::audio_cluster_descriptor *audio_cluster_desc_ref = configuration->get_audio_cluster_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(audio_cluster_desc_ref->descriptor_type());
                desc_index = audio_cluster_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_AUDIO_MAP:
            for(unsigned int j = 0; j < configuration->audio_map_desc_count(); j++)
            {
                avdecc_lib::audio_map_descriptor *audio_map_desc_ref = configuration->get_audio_map_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(audio_map_desc_ref->descriptor_type());
                desc_index = audio_map_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

        case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
            for(unsigned int j = 0; j < configuration->clock_domain_desc_count(); j++)
            {
                avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = configuration->get_clock_domain_desc_by_index(j);
                desc_name = utility->aem_desc_value_to_name(clk_domain_desc_ref->descriptor_type());
                desc_index = clk_domain_desc_ref->descriptor_index();

                std::cout << "\n----------------------- " << desc_name << " -----------------------";
                cmd_view_descriptor(desc_name, desc_index);
            }

            break;
    }

    return 0;
}

int cmd_line::cmd_view_descriptor(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());

    std::cout << "\ndescriptor_type: " << utility->aem_desc_value_to_name(desc_type_value);
    std::cout << "\ndescriptor_index: " << std::dec << desc_index;

    switch(desc_type_value)
    {
        case avdecc_lib::AEM_DESC_ENTITY:
            {
                avdecc_lib::entity_descriptor *entity = controller_obj->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity);
                if(entity)
                {
                    std::cout << "\nentity_id = 0x" << std::hex << entity->entity_id();
                    std::cout << "\nvendor_id = " << std::dec << entity->vendor_id();
                    std::cout << "\nentity_model_id = " << std::dec << entity->entity_model_id();
                    std::cout << "\nentity_capabilities = 0x" << std::hex << entity->entity_capabilities();
                    std::cout << "\ntalker_stream_sources = " << std::dec << entity->talker_stream_sources();
                    std::cout << "\ntalker_capabilities = 0x" << std::hex << entity->talker_capabilities();
                    std::cout << "\nlistener_stream_sinks = " << std::dec << entity->listener_stream_sinks();
                    std::cout << "\nlistener_capabilities = 0x" << std::hex << entity->listener_capabilities();
                    std::cout << "\ncontroller_capabilities = 0x" << std::hex << entity->controller_capabilities();
                    std::cout << "\navailable_index = " << std::dec << entity->available_index();
                    std::cout << "\nassociation_id = " << std::dec << entity->association_id();
                    std::cout << "\nentity_name = " << std::hex << entity->entity_name();
                    std::cout << "\nvendor_name_string = " << std::dec << entity->vendor_name_string();
                    std::cout << "\nmodel_name_string = " << std::dec << entity->model_name_string();
                    std::cout << "\nfirmware_version = " << std::dec << entity->firmware_version();
                    std::cout << "\ngroup_name = " << std::dec << entity->group_name();
                    std::cout << "\nserial_number = " << std::dec << entity->serial_number();
                    std::cout << "\nconfigurations_count = " << std::dec << entity->configurations_count();
                    std::cout << "\ncurrent_configuration = " << std::dec << entity->current_configuration() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_CONFIGURATION:
            {
                avdecc_lib::configuration_descriptor *configuration = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config);
                if(configuration)
                {
                    std::cout << "\nobject_name = " << std::hex << configuration->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << configuration->localized_description();
                    std::cout << "\ndescriptor_counts_count = " << std::dec << configuration->descriptor_counts_count()<< std::endl;

                    uint16_t desc_counts_count = configuration->descriptor_counts_count();
                    uint16_t desc_type_from_config = 0;
                    uint16_t desc_count_from_config = 0;

                    if(desc_counts_count > 0)
                    {
                        std::cout << "\nTop level descriptors" << std::endl;

                        for(uint32_t i = 0; i < desc_counts_count; i++)
                        {
                            desc_type_from_config = configuration->get_desc_type_from_config_by_index(i);
                            desc_count_from_config = configuration->get_desc_count_from_config_by_index(i);

                            std::cout << "\tdesc_type = 0x" << std::hex << desc_type_from_config << " (" <<
                                         utility->aem_desc_value_to_name(desc_type_from_config) << ")" << std::endl;
                            std::cout << "\tdesc_count = " << std::dec << desc_count_from_config << std::endl;
                        }
                    }
                }
            }
            break;

        case avdecc_lib::AEM_DESC_AUDIO_UNIT:
            {
                avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_unit_desc_by_index(desc_index);
                if(audio_unit_desc_ref)
                {
                    std::cout << "\nobject_name = " << std::hex << audio_unit_desc_ref->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << audio_unit_desc_ref->localized_description();
                    std::cout << "\nclock_domain_index = " << std::dec << audio_unit_desc_ref->clock_domain_index();
                    std::cout << "\nnumber_of_stream_input_ports = " << std::dec << audio_unit_desc_ref->number_of_stream_input_ports();
                    std::cout << "\nbase_stream_input_port = " << std::dec << audio_unit_desc_ref->base_stream_input_port();
                    std::cout << "\nnumber_of_stream_output_ports = " << std::dec << audio_unit_desc_ref->number_of_stream_output_ports();
                    std::cout << "\nbase_stream_output_port = " << std::dec << audio_unit_desc_ref->base_stream_output_port();
                    std::cout << "\nnumber_of_external_input_ports = " << std::dec << audio_unit_desc_ref->number_of_external_input_ports();
                    std::cout << "\nbase_external_input_port = " << std::dec << audio_unit_desc_ref->base_external_input_port();
                    std::cout << "\nnumber_of_external_output_ports = " << std::dec << audio_unit_desc_ref->number_of_external_output_ports();
                    std::cout << "\nbase_external_output_port = " << std::dec << audio_unit_desc_ref->base_external_output_port();
                    std::cout << "\nnumber_of_internal_input_ports = " << std::dec << audio_unit_desc_ref->number_of_internal_input_ports();
                    std::cout << "\nbase_internal_input_port = " << std::dec << audio_unit_desc_ref->base_internal_input_port();
                    std::cout << "\nnumber_of_internal_output_ports = " << std::dec << audio_unit_desc_ref->number_of_internal_output_ports();
                    std::cout << "\nbase_internal_output_port = " << std::dec << audio_unit_desc_ref->base_internal_output_port();
                    std::cout << "\nnumber_of_controls = " << std::dec << audio_unit_desc_ref->number_of_controls();
                    std::cout << "\nbase_control = " << std::dec << audio_unit_desc_ref->base_control();
                    std::cout << "\nnumber_of_signal_selectors = " << std::dec << audio_unit_desc_ref->number_of_signal_selectors();
                    std::cout << "\nbase_signal_selector = " << std::dec << audio_unit_desc_ref->base_signal_selector();
                    std::cout << "\nnumber_of_mixers = " << std::dec << audio_unit_desc_ref->number_of_mixers();
                    std::cout << "\nbase_mixer = " << std::dec << audio_unit_desc_ref->base_mixer();
                    std::cout << "\nnumber_of_matrices = " << std::dec << audio_unit_desc_ref->number_of_matrices();
                    std::cout << "\nbase_matrix = " << std::dec << audio_unit_desc_ref->base_matrix();
                    std::cout << "\nnumber_of_splitters = " << std::dec << audio_unit_desc_ref->number_of_splitters();
                    std::cout << "\nbase_splitter = " << std::dec << audio_unit_desc_ref->base_splitter();
                    std::cout << "\nnumber_of_combiners = " << std::dec << audio_unit_desc_ref->number_of_combiners();
                    std::cout << "\nbase_combiner = " << std::dec << audio_unit_desc_ref->base_combiner();
                    std::cout << "\nnumber_of_demultiplexers = " << std::dec << audio_unit_desc_ref->number_of_demultiplexers();
                    std::cout << "\nbase_demultiplexer = " << std::dec << audio_unit_desc_ref->base_demultiplexer();
                    std::cout << "\nnumber_of_multiplexers = " << std::dec << audio_unit_desc_ref->number_of_multiplexers();
                    std::cout << "\nbase_multiplexer = " << std::dec << audio_unit_desc_ref->base_multiplexer();
                    std::cout << "\nnumber_of_transcoders = " << std::dec << audio_unit_desc_ref->number_of_transcoders();
                    std::cout << "\nbase_transcoder = " << std::dec << audio_unit_desc_ref->base_transcoder();
                    std::cout << "\nnumber_of_control_blocks = " << std::dec << audio_unit_desc_ref->number_of_control_blocks();
                    std::cout << "\nbase_control_block = " << std::dec << audio_unit_desc_ref->base_control_block();
                    std::cout << "\ncurrent_sampling_rate = " << std::dec << audio_unit_desc_ref->current_sampling_rate();
                    std::cout << "\nsampling_rates_count = " << std::dec << audio_unit_desc_ref->sampling_rates_count() << std::endl;

                    for(uint32_t i = 0; i < audio_unit_desc_ref->sampling_rates_count(); i++)
                    {
                        std::cout << "sampling_rate_" << i << " = " << std::dec << audio_unit_desc_ref->get_sampling_rate_by_index(i) << std::endl;
                    }
                }
            }
            break;

        case avdecc_lib::AEM_DESC_STREAM_INPUT:
            {
                avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
                if(stream_input_desc_ref)
                {
                    std::cout << "\nobject_name = " << std::hex << stream_input_desc_ref->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << stream_input_desc_ref->localized_description();
                    std::cout << "\nclock_domain_index = " << std::dec << stream_input_desc_ref->clock_domain_index();
                    std::cout << "\nstream_flags = 0x" << std::hex << stream_input_desc_ref->stream_flags();
                    std::cout << "\n\tclock_sync_source = " << std::dec << stream_input_desc_ref->stream_flags_clock_sync_source();
                    std::cout << "\n\tclass_a = " << std::dec << stream_input_desc_ref->stream_flags_class_a();
                    std::cout << "\n\tclass_b = " << std::dec << stream_input_desc_ref->stream_flags_class_b();
                    std::cout << "\n\tsupports_encrypted = " << std::dec << stream_input_desc_ref->stream_flags_supports_encrypted();
                    std::cout << "\n\tprimary_backup_valid = " << std::dec << stream_input_desc_ref->stream_flags_primary_backup_valid();
                    std::cout << "\n\tprimary_backup_valid = " << std::dec << stream_input_desc_ref->stream_flags_primary_backup_valid();
                    std::cout << "\n\tsecondary_backup_supported = " << std::dec << stream_input_desc_ref->stream_flags_secondary_backup_supported();
                    std::cout << "\n\tsecondary_backup_valid = " << std::dec << stream_input_desc_ref->stream_flags_secondary_backup_valid();
                    std::cout << "\n\ttertiary_backup_supported = " << std::dec << stream_input_desc_ref->stream_flags_tertiary_backup_supported();
                    std::cout << "\n\ttertiary_back_up_valid = " << std::dec << stream_input_desc_ref->stream_flags_tertiary_back_up_valid();
                    std::cout << "\ncurrent_format = " << std::hex << stream_input_desc_ref->current_format();
                    std::cout << "\nnumber_of_formats = " << std::dec << stream_input_desc_ref->number_of_formats();
                    std::cout << "\nbackup_talker_entity_id_0 = 0x" << std::hex << stream_input_desc_ref->backup_talker_entity_id_0();
                    std::cout << "\nbackup_talker_unique_0 = " << std::dec << stream_input_desc_ref->backup_talker_unique_0();
                    std::cout << "\nbackup_talker_entity_id_1 = 0x" << std::hex << stream_input_desc_ref->backup_talker_entity_id_1();
                    std::cout << "\nbackup_talker_unique_1 = " << std::dec << stream_input_desc_ref->backup_talker_unique_1();
                    std::cout << "\nbackup_talker_entity_id_2 = 0x" << std::hex << stream_input_desc_ref->backup_talker_entity_id_2();
                    std::cout << "\nbackup_talker_unique_2 = " << std::dec << stream_input_desc_ref->backup_talker_unique_2();
                    std::cout << "\nbackedup_talker_entity_id = 0x" << std::hex << stream_input_desc_ref->backedup_talker_entity_id();
                    std::cout << "\nbackedup_talker_unique = " << std::dec << stream_input_desc_ref->backedup_talker_unique();
                    std::cout << "\navb_interface_index = " << std::dec << stream_input_desc_ref->avb_interface_index();
                    std::cout << "\nbuffer_length = " << std::dec << stream_input_desc_ref->buffer_length() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
            {
                avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
                if(stream_output_desc_ref)
                {
                    std::cout << "\nobject_name = " << std::hex << stream_output_desc_ref->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << stream_output_desc_ref->localized_description();
                    std::cout << "\nclock_domain_index = " << std::dec << stream_output_desc_ref->clock_domain_index();
                    std::cout << "\nstream_flags = 0x" << std::hex << stream_output_desc_ref->stream_flags();
                    std::cout << "\n\tclock_sync_source = " << std::dec << stream_output_desc_ref->stream_flags_clock_sync_source();
                    std::cout << "\n\tclass_a = " << std::dec << stream_output_desc_ref->stream_flags_class_a();
                    std::cout << "\n\tclass_b = " << std::dec << stream_output_desc_ref->stream_flags_class_b();
                    std::cout << "\n\tsupports_encrypted = " << std::dec << stream_output_desc_ref->stream_flags_supports_encrypted();
                    std::cout << "\n\tprimary_backup_valid = " << std::dec << stream_output_desc_ref->stream_flags_primary_backup_valid();
                    std::cout << "\n\tprimary_backup_valid = " << std::dec << stream_output_desc_ref->stream_flags_primary_backup_valid();
                    std::cout << "\n\tsecondary_backup_supported = " << std::dec << stream_output_desc_ref->stream_flags_secondary_backup_supported();
                    std::cout << "\n\tsecondary_backup_valid = " << std::dec << stream_output_desc_ref->stream_flags_secondary_backup_valid();
                    std::cout << "\n\ttertiary_backup_supported = " << std::dec << stream_output_desc_ref->stream_flags_tertiary_backup_supported();
                    std::cout << "\n\ttertiary_back_up_valid = " << std::dec << stream_output_desc_ref->stream_flags_tertiary_back_up_valid();
                    std::cout << "\ncurrent_format = " << std::hex << stream_output_desc_ref->current_format();
                    std::cout << "\nnumber_of_formats = " << std::dec << stream_output_desc_ref->number_of_formats();
                    std::cout << "\nbackup_talker_entity_id_0 = 0x" << std::hex << stream_output_desc_ref->backup_talker_entity_id_0();
                    std::cout << "\nbackup_talker_unique_0 = " << std::dec << stream_output_desc_ref->backup_talker_unique_0();
                    std::cout << "\nbackup_talker_entity_id_1 = 0x" << std::hex << stream_output_desc_ref->backup_talker_entity_id_1();
                    std::cout << "\nbackup_talker_unique_1 = " << std::dec << stream_output_desc_ref->backup_talker_unique_1();
                    std::cout << "\nbackup_talker_entity_id_2 = 0x" << std::hex << stream_output_desc_ref->backup_talker_entity_id_2();
                    std::cout << "\nbackup_talker_unique_2 = " << std::dec << stream_output_desc_ref->backup_talker_unique_2();
                    std::cout << "\nbackedup_talker_entity_id = 0x" << std::hex << stream_output_desc_ref->backedup_talker_entity_id();
                    std::cout << "\nbackedup_talker_unique = " << std::dec << stream_output_desc_ref->backedup_talker_unique();
                    std::cout << "\navb_interface_index = " << std::dec << stream_output_desc_ref->avb_interface_index();
                    std::cout << "\nbuffer_length = " << std::dec << stream_output_desc_ref->buffer_length() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_JACK_INPUT:
            {
                avdecc_lib::jack_input_descriptor *jack_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_jack_input_desc_by_index(desc_index);
                if(jack_input_desc_ref)
                {
                    std::cout << "\nobject_name = " << std::hex << jack_input_desc_ref->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << jack_input_desc_ref->localized_description();
                    std::cout << "\njack_flags = 0x" << std::hex << jack_input_desc_ref->jack_flags();
                    std::cout << "\n\tclock_sync_source_flag = 0x" << std::hex << jack_input_desc_ref->jack_flag_clock_sync_source();
                    std::cout << "\n\tcaptive_flag = 0x" << std::hex << jack_input_desc_ref->jack_flag_captive();
                    std::cout << "\nnumber_of_controls = " << std::dec << jack_input_desc_ref->number_of_controls();
                    std::cout << "\nbase_control = " << std::dec << jack_input_desc_ref->base_control() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_JACK_OUTPUT:
            {
                avdecc_lib::jack_output_descriptor *jack_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_jack_output_desc_by_index(desc_index);
                if(jack_output_desc_ref)
                {
                    std::cout << "\nobject_name = " << std::hex << jack_output_desc_ref->object_name();
                    std::cout << "\nlocalized_description = 0x" << std::hex << jack_output_desc_ref->localized_description();
                    std::cout << "\njack_flags = 0x" << std::hex << jack_output_desc_ref->jack_flags();
                    std::cout << "\n\tclock_sync_source_flag = 0x" << std::hex << jack_output_desc_ref->jack_flag_clock_sync_source();
                    std::cout << "\n\tcaptive_flag = 0x" << std::hex << jack_output_desc_ref->jack_flag_captive();
                    std::cout << "\njack_type = 0x" << std::hex << jack_output_desc_ref->jack_type();
                    std::cout << "\nnumber_of_controls = " << std::dec << jack_output_desc_ref->number_of_controls();
                    std::cout << "\nbase_control = " << std::dec << jack_output_desc_ref->base_control() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_AVB_INTERFACE:
            {
                avdecc_lib::avb_interface_descriptor *avb_interface_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_avb_interface_desc_by_index(desc_index);
                if(avb_interface_desc)
                {
                    std::cout << "\nobject_name = " << std::hex << avb_interface_desc->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << avb_interface_desc->localized_description();
                    std::cout << "\nmac_address = 0x" << std::hex << avb_interface_desc->mac_addr();
                    std::cout << "\ninterface_flags = 0x" << std::hex << avb_interface_desc->interface_flags();
                    std::cout << "\nclock_identity = 0x" << std::hex << avb_interface_desc->clock_identity();
                    std::cout << "\npriority1 = " << std::dec << avb_interface_desc->priority1();
                    std::cout << "\nclock_class = " << std::dec << avb_interface_desc->clock_class();
                    std::cout << "\noffset_scaled_log_variance = " << std::dec << avb_interface_desc->offset_scaled_log_variance();
                    std::cout << "\nclock_accuracy = " << std::dec << avb_interface_desc->clock_accuracy();
                    std::cout << "\npriority2 = " << std::dec << avb_interface_desc->priority2();
                    std::cout << "\ndomain_number = " << std::dec << avb_interface_desc->domain_number();
                    std::cout << "\nlog_sync_interval = " << std::dec << avb_interface_desc->log_sync_interval() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
            {
                avdecc_lib::clock_source_descriptor *clk_src_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_source_desc_by_index(desc_index);
                if(clk_src_desc)
                {
                    std::cout << "\nobject_name = " << std::hex << clk_src_desc->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << clk_src_desc->localized_description();
                    std::cout << "\nclock_source_flags = 0x" << std::hex << clk_src_desc->clock_source_flags();
                    std::cout << "\nclock_source_type = 0x" << std::hex << clk_src_desc->clock_source_type();
                    std::cout << "\nclock_source_identifier = 0x" << std::hex << clk_src_desc->clock_source_identifier();
                    std::cout << "\nclock_source_location_type = 0x" << std::hex << clk_src_desc->clock_source_location_type();
                    std::cout << "\nclock_source_location_index = " << std::dec << clk_src_desc->clock_source_location_index() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_LOCALE:
            {
                avdecc_lib::locale_descriptor *locale = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_locale_desc_by_index(desc_index);
                if(locale)
                {
                    std::cout << "\nlocale_identifier = " << std::dec << locale->locale_identifier();
                    std::cout << "\nnumber_of_strings = " << std::dec << locale->number_of_strings();
                    std::cout << "\nbase_strings = " << std::dec << locale->base_strings() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_STRINGS:
            {
                avdecc_lib::strings_descriptor *strings = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_strings_desc_by_index(desc_index);
                if(strings)
                {
                    std::cout << "\nstring_0 = " << std::hex << strings->get_string_by_index(0);
                    std::cout << "\nstring_1 = " << std::hex << strings->get_string_by_index(1);
                    std::cout << "\nstring_2 = " << std::hex << strings->get_string_by_index(2);
                    std::cout << "\nstring_3 = " << std::hex << strings->get_string_by_index(3);
                    std::cout << "\nstring_4 = " << std::hex << strings->get_string_by_index(4);
                    std::cout << "\nstring_5 = " << std::hex << strings->get_string_by_index(5);
                    std::cout << "\nstring_6 = " << std::hex << strings->get_string_by_index(6) << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_STREAM_PORT_INPUT:
            {
                avdecc_lib::stream_port_input_descriptor *stream_port_input_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_port_input_desc_by_index(desc_index);
                if(stream_port_input_desc)
                {
                    std::cout << "\nclock_domain_index = " << std::dec << stream_port_input_desc->clock_domain_index();
                    std::cout << "\nport_flags = " << std::hex << stream_port_input_desc->port_flags();
                    std::cout << "\nnumber_of_controls = " << std::dec << stream_port_input_desc->number_of_controls();
                    std::cout << "\nbase_control = " << std::dec << stream_port_input_desc->base_control();
                    std::cout << "\nnumber_of_clusters = " << std::dec << stream_port_input_desc->number_of_clusters();
                    std::cout << "\nbase_cluster = " << std::dec << stream_port_input_desc->base_cluster();
                    std::cout << "\nnumber_of_maps = " << std::dec << stream_port_input_desc->number_of_maps();
                    std::cout << "\nbase_map = " << std::dec << stream_port_input_desc->base_map() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_STREAM_PORT_OUTPUT:
            {
                avdecc_lib::stream_port_output_descriptor *stream_port_output_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_port_output_desc_by_index(desc_index);
                if(stream_port_output_desc)
                {
                    std::cout << "\nclock_domain_index = " << std::dec << stream_port_output_desc->clock_domain_index();
                    std::cout << "\nport_flags = " << std::hex << stream_port_output_desc->port_flags();
                    std::cout << "\nnumber_of_controls = " << std::dec << stream_port_output_desc->number_of_controls();
                    std::cout << "\nbase_control = " << std::dec << stream_port_output_desc->base_control();
                    std::cout << "\nnumber_of_clusters = " << std::dec << stream_port_output_desc->number_of_clusters();
                    std::cout << "\nbase_cluster = " << std::dec << stream_port_output_desc->base_cluster();
                    std::cout << "\nnumber_of_maps = " << std::dec << stream_port_output_desc->number_of_maps();
                    std::cout << "\nbase_map = " << std::dec << stream_port_output_desc->base_map() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_AUDIO_CLUSTER:
            {
                avdecc_lib::audio_cluster_descriptor *audio_cluster_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_cluster_desc_by_index(desc_index);
                if(audio_cluster_desc)
                {
                    std::cout << "\nobject_name = " << std::hex << audio_cluster_desc->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << audio_cluster_desc->localized_description();
                    std::cout << "\nsignal_type = " << std::dec << audio_cluster_desc->signal_type();
                    std::cout << "\nsignal_index = " << std::dec << audio_cluster_desc->signal_index();
                    std::cout << "\nsignal_output = " << std::dec << audio_cluster_desc->signal_output();
                    std::cout << "\npath_latency = " << std::dec << audio_cluster_desc->path_latency();
                    std::cout << "\nblock_latency = " << std::dec << audio_cluster_desc->block_latency();
                    std::cout << "\nchannel_count = " << std::dec << audio_cluster_desc->channel_count();
                    std::cout << "\nformat = 0x" << std::hex << audio_cluster_desc->format() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_AUDIO_MAP:
            {
                avdecc_lib::audio_map_descriptor *audio_map_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_map_desc_by_index(desc_index);
                if(audio_map_desc)
                {
                    std::cout << "\nnumber_of_mappings = " << std::dec << audio_map_desc->number_of_mappings() << std::endl;
                }
            }
            break;

        case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
            {
                avdecc_lib::clock_domain_descriptor *clk_domain_desc = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_domain_desc_by_index(desc_index);
                if(clk_domain_desc)
                {
                    std::cout << "\nobject_name = " << std::hex << clk_domain_desc->object_name();
                    std::cout << "\nlocalized_description = " << std::dec << clk_domain_desc->localized_description();
                    std::cout << "\nclock_source_index = " << std::dec << clk_domain_desc->clock_source_index();
                    std::cout << "\nclock_sources_count = " << std::dec << clk_domain_desc->clock_sources_count() << std::endl;

                    for(uint32_t i = 0; i < clk_domain_desc->clock_sources_count(); i++)
                    {
                        std::cout << "\tclock_sources = " << std::dec << clk_domain_desc->get_clock_source_by_index(i) << std::endl;
                    }
                }
            }
            break;

        default:
            std::cout << "Descriptor type is not found." << std::endl;
            break;
    }

    return 0;
}

int cmd_line::cmd_read_descriptor(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    intptr_t cmd_notification_id = 0;

    if(desc_type_value < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        controller_obj->get_end_station_by_index(current_end_station)->send_read_desc_cmd((void *)cmd_notification_id, desc_type_value, desc_index);
        sys->get_last_resp_status();
    }
    else
    {
        std::cout << "cmd_read_descriptor error" << std::endl;
        return -1;
    }

    return 0;
}

int cmd_line::cmd_connect()
{
    uint8_t *outstream_end_station_name;
    uint8_t *instream_end_station_name;
    const char * format;
    size_t stream_input_desc_count = 0;
    size_t stream_output_desc_count = 0;
    uint64_t end_station_mac;

    std::cout << "\n" << "End Station" << std::setw(26) << "" << "Instream" << std::setw(16) << "" << "Stream Format" << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;

    for(uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
    {
        end_station_mac = controller_obj->get_end_station_by_index(i)->mac();
        instream_end_station_name = controller_obj->get_end_station_by_index(i)->get_entity_desc_by_index(current_entity)->entity_name();
        stream_input_desc_count = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->stream_input_desc_count();

        for(uint32_t j = 0; j < stream_input_desc_count; j++)
        {
            uint8_t *desc_desc_name = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->get_stream_input_desc_by_index(j)->object_name();
            format = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->get_stream_input_desc_by_index(j)->current_format();

            std::cout << std::setw(5) << i << std::setw(20) << instream_end_station_name << utility->end_station_mac_to_string(end_station_mac) << "   " <<
                         std::setw(3) << j << std::setw(19) << desc_desc_name << "   " << std::setw(14) << format << std::endl;
        }
    }

        std::cout << "\n" << "End Station" << std::setw(26) << "" << "Outstream" << std::setw(15) << "" << "Stream Format" << std::endl;
        std::cout << "------------------------------------------------------------------------------" << std::endl;

        for(uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
        {
                end_station_mac = controller_obj->get_end_station_by_index(i)->mac();
                outstream_end_station_name = controller_obj->get_end_station_by_index(i)->get_entity_desc_by_index(current_entity)->entity_name();
                stream_output_desc_count = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->stream_output_desc_count();

                for(uint32_t j = 0; j < stream_output_desc_count; j++)
                {
                    uint8_t *src_desc_name = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->get_stream_output_desc_by_index(j)->object_name();
                    format = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->get_stream_output_desc_by_index(j)->current_format();

                    std::cout << std::setw(5) << i << std::setw(20) << outstream_end_station_name << utility->end_station_mac_to_string(end_station_mac) << "   " <<
                              std::setw(3) << j << std::setw(19) << src_desc_name << "   " << std::setw(15) << format << std::endl;
                }
        }

    return 0;
}

int cmd_line::cmd_connect(uint32_t instream_end_station_index, uint16_t instream_desc_index)
{
    bool is_instream_valid = (instream_end_station_index < (controller_obj->get_end_station_count())) &&
                             (instream_desc_index < (controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->stream_input_desc_count()));

    if(is_instream_valid)
    {
        uint8_t *outstream_end_station_name;
        uint8_t *src_desc_name;
        const char *format;
        size_t stream_output_desc_count = 0;
        uint64_t end_station_mac;

        std::cout << "\n" << "End Station" << std::setw(26) << "   " << "Outstream" << std::setw(16) << "   " << "Stream Format" << std::endl;
        std::cout << "------------------------------------------------------------------------------" << std::endl;

        for(uint32_t i = 0; i < controller_obj->get_end_station_count(); i++)
        {
            if(i == instream_end_station_index)
            {
                end_station_mac = controller_obj->get_end_station_by_index(i)->mac();
                outstream_end_station_name = controller_obj->get_end_station_by_index(i)->get_entity_desc_by_index(current_entity)->entity_name();
                stream_output_desc_count = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->stream_output_desc_count();

                for(uint32_t j = 0; j < stream_output_desc_count; j++)
                {
                    src_desc_name = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->get_stream_output_desc_by_index(j)->object_name();
                    format = controller_obj->get_config_desc_by_index(i, current_entity, current_config)->get_stream_output_desc_by_index(j)->current_format();

                    std::cout << std::setw(5) << i << std::setw(20) << outstream_end_station_name << utility->end_station_mac_to_string(end_station_mac) << "   " <<
                              std::setw(2) << j << std::setw(19) << src_desc_name << "   " << std::setw(10) << format << std::endl;
                }
            }
        }
    }
    else
    {
        std::cout << "Invalid Instream" << std::endl;
    }
    
    return 0;
}

int cmd_line::cmd_connect_rx(uint32_t instream_end_station_index,
                             uint16_t instream_desc_index,
                             uint32_t outstream_end_station_index,
                             uint16_t outstream_desc_index,
                             std::string flags)
{
    bool is_valid = (instream_end_station_index != outstream_end_station_index) && (instream_end_station_index < (controller_obj->get_end_station_count())) &&
                    (instream_desc_index < (controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->stream_input_desc_count())) &&
                    (outstream_end_station_index < (controller_obj->get_end_station_count())) &&
                    (outstream_desc_index < (controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->stream_output_desc_count()));
    
    if(is_valid)
    {
        uint16_t connection_flag = 0;
        if(flags == "")
        {
            connection_flag = 0x0;
        }
        else if(flags == "class_b")
        {
            connection_flag = 0x1;
        }
        else if(flags == "fast_connect")
        {
            connection_flag = 0x2;
        }
        else if(flags == "saved_state")
        {
            connection_flag = 0x4;
        }
        else if(flags == "wait")
        {
            connection_flag = 0x8;
        }
        else if(flags == "supports_encrypted")
        {
            connection_flag = 0x8;
        }
        else if(flags == "encrypted_pdu")
        {
            connection_flag = 0x10;
        }
        else if(flags == "talker_failed")
        {
            connection_flag = 0x10;
        }
        else
        {
              std::cout << "\nInvalid Flag" << std::endl;
              return -1;
        }

        intptr_t cmd_notification_id = 0;
        uint64_t talker_guid;
        bool check_stream_format;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *instream = controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->get_stream_input_desc_by_index(instream_desc_index);
        avdecc_lib::stream_output_descriptor *outstream = controller_obj->get_config_desc_by_index(outstream_end_station_index, current_entity, current_config)->get_stream_output_desc_by_index(outstream_desc_index);
        check_stream_format = (strcmp(instream->current_format(), outstream->current_format()) == 0);
        if(!check_stream_format)
        {
            std::cout << "\nStream format does not match. \nInstream has stream format: " << instream->current_format() <<
                         "\nOutstream has stream format: " << outstream->current_format() << std::endl;
        }

        talker_guid = controller_obj->get_end_station_by_index(outstream_end_station_index)->get_entity_desc_by_index(current_entity)->entity_id();
        instream->send_connect_rx_cmd((void *)cmd_notification_id, talker_guid, outstream_desc_index, connection_flag);
        sys->get_last_resp_status();
    }
    else
    {
        std::cout << "Invalid ACMP Connection" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_disconnect_rx(uint32_t instream_end_station_index,
                                uint16_t instream_desc_index,
                                uint32_t outstream_end_station_index,
                                uint16_t outstream_desc_index)
{
    bool is_valid = true;

    if(is_valid)
    {
        intptr_t cmd_notification_id = 0;
        uint64_t talker_guid;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *instream = controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->get_stream_input_desc_by_index(instream_desc_index);
        talker_guid = controller_obj->get_end_station_by_index(outstream_end_station_index)->get_entity_desc_by_index(current_entity)->entity_id();

        instream->send_disconnect_rx_cmd((void *)cmd_notification_id, talker_guid, outstream_desc_index);
        sys->get_last_resp_status();
    }
    else
    {
        std::cout << "Invalid ACMP Disconnection" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_tx_state(uint32_t outstream_end_station_index, uint16_t outstream_desc_index)
{
    bool is_valid = (outstream_end_station_index < (controller_obj->get_end_station_count())) &&
                    (outstream_desc_index < (controller_obj->get_config_desc_by_index(outstream_end_station_index, current_entity, current_config)->stream_output_desc_count()));

    if(is_valid)
    {
        int status = -1;
        intptr_t cmd_notification_id = 0;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *outstream = controller_obj->get_config_desc_by_index(outstream_end_station_index, current_entity, current_config)->get_stream_output_desc_by_index(outstream_desc_index);
        outstream->send_get_tx_state_cmd((void *)cmd_notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::ACMP_STATUS_SUCCESS)
        {
            std::cout << "\nstream_id = 0x" << std::hex << outstream->get_tx_state_stream_id();
            std::cout << "\nstream_dest_mac = 0x" << std::hex << outstream->get_tx_state_stream_dest_mac();
            std::cout << "\nconnection_count = " << std::dec << outstream->get_tx_state_connection_count();
            std::cout << "\nstream_vlan_id = " << std::dec << outstream->get_tx_state_stream_vlan_id() << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid GET Talker Connection State" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_rx_state(uint32_t instream_end_station_index, uint16_t instream_desc_index)
{
    bool is_valid = (instream_end_station_index < (controller_obj->get_end_station_count())) &&
                    (instream_desc_index < (controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->stream_input_desc_count()));
              
    if(is_valid)
    {
        int status = -1;
        intptr_t cmd_notification_id = 0;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *instream = controller_obj->get_config_desc_by_index(instream_end_station_index, current_entity, current_config)->get_stream_input_desc_by_index(instream_desc_index);

        instream->send_get_rx_state_cmd((void *)cmd_notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::ACMP_STATUS_SUCCESS)
        {
            std::cout << "\nstream_id = 0x" << std::hex << instream->get_rx_state_stream_id();
            std::cout << "\ntalker_unique_id = " << std::dec <<  std::dec << instream->get_rx_state_talker_unique_id();
            std::cout << "\nlistener_unique_id = " << std::dec << instream->get_rx_state_listener_unique_id();
            std::cout << "\nstream_dest_mac = 0x" << std::hex << instream->get_rx_state_stream_dest_mac();
            std::cout << "\nconnection_count = " << std::dec << instream->get_rx_state_connection_count();
            std::cout << "\nflags = " << std::dec << instream->get_rx_state_flags();
            std::cout << "\nstream_vlan_id = " << std::dec << instream->get_rx_state_stream_vlan_id() << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid Get Listener Connection State" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_get_tx_connection(uint32_t outstream_end_station_index, uint16_t outstream_desc_index)
{
    bool is_valid = (outstream_end_station_index < (controller_obj->get_end_station_count())) &&
                    (outstream_desc_index < (controller_obj->get_config_desc_by_index(outstream_end_station_index, current_entity, current_config)->stream_output_desc_count()));

    if(is_valid)
    {
        int status = -1;
        intptr_t cmd_notification_id = 0;

        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *outstream = controller_obj->get_config_desc_by_index(outstream_end_station_index, current_entity, current_config)->get_stream_output_desc_by_index(outstream_desc_index);
        outstream->send_get_tx_connection_cmd((void *)cmd_notification_id, 0, 0);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::ACMP_STATUS_SUCCESS)
        {
            std::cout << "\nstream_id = 0x" << std::hex << outstream->get_tx_connection_stream_id();
            std::cout << "\nlistener_entity_id = 0x" << std::hex << outstream->get_tx_connection_listener_entity_id();
            std::cout << "\nlistener_unique_id = " << std::dec << outstream->get_tx_connection_listener_unique_id();
            std::cout << "\nstream_dest_mac = 0x" << std::hex << outstream->get_tx_connection_stream_dest_mac();
            std::cout << "\nconnection_count = " << std::dec << outstream->get_tx_connection_connection_count();
            std::cout << "\nstream_vlan_id = " << std::dec << outstream->get_tx_connection_stream_vlan_id() << std::endl;
        }
    }
    else
    {
        std::cout << "Invalid GET Talker Connection State" << std::endl;
    }

    return 0;
}

int cmd_line::cmd_acquire_entity(std::string flag_name, std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    uint32_t flag_id = 0;
    intptr_t cmd_notification_id = 0;

    if(flag_name == "acquire")
    {
        flag_id = 0;
    }
    else if(flag_name == "persistent")
    {
        flag_id = 0x1;
    }
    else if(flag_name == "release")
    {
        flag_id = 0x80000000;
    }
    else
    {
        std::cout << "\nInvalid flag" << std::endl;
        return -1;
    }

    if(desc_type_value == avdecc_lib::AEM_DESC_ENTITY)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::entity_descriptor *entity = controller_obj->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity);
        entity->send_acquire_entity_cmd((void *)cmd_notification_id, flag_id);
        sys->get_last_resp_status();

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_acquire_entity_cmd((void *)notification_id, flag_id);
        sys->get_last_resp_status();

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_get_stream_format_cmd((void *)notification_id);
        sys->get_last_resp_status();

        return 1;
    }
    else
    {
        std::cout << "cmd_acquire_entity error" << std::endl;
        return -1;
    }

}

int cmd_line::cmd_lock_entity(std::string flag_name, std::string desc_name, uint16_t desc_index)
{
    uint32_t flag_id;
    uint16_t desc_type_value;
    intptr_t cmd_notification_id;

    desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());

    if(flag_name == "lock")
    {
        flag_id = 0;
    }
    else if(flag_name == "unlock")
    {
        flag_id = 0x1;
    }
    else
    {
        std::cout << "\nInvalid flag" << std::endl;
        return -1;
    }

    if(desc_type_value == avdecc_lib::AEM_DESC_ENTITY)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::entity_descriptor *entity = controller_obj->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity);
        entity->send_lock_entity_cmd((void *)cmd_notification_id, flag_id);
        sys->get_last_resp_status();

        return 1;
    }

    //        std::cout << "Need to implement cmd_lock_entity" << std::endl;

    return 0;
}


int cmd_line::cmd_entity_avail()
{
    intptr_t cmd_notification_id = get_next_notification_id();

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    controller_obj->get_end_station_by_index(current_end_station)->send_entity_avail_cmd((void *)cmd_notification_id);
    sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_controller_avail()
{
    intptr_t cmd_notification_id = get_next_notification_id();

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    controller_obj->send_controller_avail_cmd((void *)cmd_notification_id, current_end_station);
     sys->get_last_resp_status();

    return 0;
}

int cmd_line::cmd_set_stream_format(std::string desc_name, uint16_t desc_index, std::string new_stream_format_name)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    std::string stream_format_substring = new_stream_format_name.substr(20);
    uint64_t stream_format_value = utility->ieee1722_format_name_to_value(("IEC..." + stream_format_substring).c_str());
    int status = -1;
    intptr_t cmd_notification_id = 0;
    std::string stream_format;

    if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_set_stream_format_cmd((void *)cmd_notification_id, stream_format_value);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            stream_format = utility->ieee1722_format_value_to_name(stream_input_desc_ref->set_stream_format_stream_format());
            if(stream_format == "UNKNOWN")
            {
                std::cout << "Stream format: 0x" << std::hex << stream_input_desc_ref->set_stream_format_stream_format() << std::endl;
            }
            else
            {
                std::cout << "Stream format: " << stream_format << std::endl;
            }
        }

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_set_stream_format_cmd((void *)cmd_notification_id, stream_format_value);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            stream_format = utility->ieee1722_format_value_to_name(stream_output_desc_ref->set_stream_format_stream_format());
            if(stream_format == "UNKNOWN")
            {
                std::cout << "Stream format: 0x" << std::hex << stream_output_desc_ref->get_stream_format_stream_format() << std::endl;
            }
            else
            {
                std::cout << "Stream format: " << stream_format << std::endl;
            }
        }

        return 1;
    }
    else
    {
        std::cout << "cmd_get_stream_format error" << std::endl;
        return -1;
    }
}

int cmd_line::cmd_get_stream_format(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    int status = -1;
    intptr_t cmd_notification_id = 0;
    std::string stream_format;

    if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            stream_format = utility->ieee1722_format_value_to_name(stream_input_desc_ref->get_stream_format_stream_format());
            if(stream_format == "UNKNOWN")
            {
                std::cout << "Stream format: 0x" << std::hex << stream_input_desc_ref->get_stream_format_stream_format() << std::endl;
            }
            else
            {
                std::cout << "Stream format: " << stream_format << std::endl;
            }
        }

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            stream_format = utility->ieee1722_format_value_to_name(stream_output_desc_ref->get_stream_format_stream_format());
            if(stream_format == "UNKNOWN")
            {
                std::cout << "Stream format: 0x" << std::hex << stream_output_desc_ref->get_stream_format_stream_format() << std::endl;
            }
            else
            {
                std::cout << "Stream format: " << stream_format << std::endl;
            }
        }

        return 1;
    }
    else
    {
        std::cout << "cmd_get_stream_format error" << std::endl;
        return -1;
    }
}

int cmd_line::cmd_set_stream_info(std::string desc_name, uint16_t desc_index, std::string stream_info_field,
                                         uint64_t new_stream_info_field_value)
{
    std::cout << "Need to implement cmd_set_stream_info" << std::endl;

    return 0;
}

int cmd_line::cmd_get_stream_info(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    int status = -1;
    intptr_t cmd_notification_id = 0;
    std::string stream_format;
   
    if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_get_stream_info_cmd((void *)cmd_notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            stream_format = utility->ieee1722_format_value_to_name(stream_input_desc_ref->get_stream_info_stream_format());
            if(stream_format == "UNKNOWN")
            {
                std::cout << "Stream format: 0x" << std::hex << stream_input_desc_ref->get_stream_info_stream_format() << std::endl;
            }
            else
            {
                std::cout << "Stream format: " << stream_format << std::endl;
            }

            std::cout << "Stream ID: " << std::dec << stream_input_desc_ref->get_stream_info_stream_id() << std::endl;
            std::cout << "MSRP Accumulated Latency: 0x" << std::dec << stream_input_desc_ref->get_stream_info_msrp_accumulated_latency() << std::endl;
            std::cout << "Stream Destination MAC: 0x" << std::hex << stream_input_desc_ref->get_stream_info_stream_dest_mac() << std::endl;
        }

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)notification_id);
        avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_get_stream_info_cmd((void *)notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            stream_format = utility->ieee1722_format_value_to_name(stream_output_desc_ref->get_stream_info_stream_format());
            if(stream_format == "UNKNOWN")
            {
                std::cout << "Stream format: 0x" << std::hex << stream_output_desc_ref->get_stream_info_stream_format() << std::endl;
            }
            else
            {
                std::cout << "Stream format: " << stream_format << std::endl;
            }

            std::cout << "Stream ID: " << std::dec << stream_output_desc_ref->get_stream_info_stream_id() << std::endl;
            std::cout << "MSRP Accumulated Latency: 0x" << std::dec << stream_output_desc_ref->get_stream_info_msrp_accumulated_latency() << std::endl;
            std::cout << "Stream Destination MAC: 0x" << std::hex << stream_output_desc_ref->get_stream_info_stream_dest_mac() << std::endl;
        }
        return 1;
    }
    else
    {
        std::cout << "cmd_get_stream_info error" << std::endl;
        return -1;
    }
}

int cmd_line::cmd_set_name(std::string desc_name, uint16_t desc_index, uint16_t name_index, std::string new_name)
{

    std::cout << "Need to implement cmd_set_name" << std::endl;

    return 0;
}

int cmd_line::cmd_get_name(std::string desc_name, uint16_t desc_index, uint16_t name_index)
{
    std::cout << "Need to implement cmd_get_name" << std::endl;

    return 0;
}

int cmd_line::cmd_set_sampling_rate(std::string desc_name, uint16_t desc_index, uint32_t new_sampling_rate)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    int status = -1;
    intptr_t cmd_notification_id = 0;

    if(desc_type_value == avdecc_lib::AEM_DESC_AUDIO_UNIT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_unit_desc_by_index(desc_index);
        audio_unit_desc_ref->send_set_sampling_rate_cmd((void *)cmd_notification_id, new_sampling_rate);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            std::cout << "Sampling rate: " << std::dec << audio_unit_desc_ref->set_sampling_rate_sampling_rate();
        }

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_VIDEO_CLUSTER)
    {
        std::cout << "\nVideo Cluster descriptor is not implemented." << std::endl;
        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_SENSOR_CLUSTER)
    {
        std::cout << "\nSensor Cluster descriptor is not implemented." << std::endl;
        return 1;
    }
    else
    {
        std::cout << "cmd_get_sampling_rate error" << std::endl;
        return -1;
    }

    return 0;
}

int cmd_line::cmd_get_sampling_rate(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    int status = -1;
    intptr_t cmd_notification_id = 0;

    if(desc_type_value == avdecc_lib::AEM_DESC_AUDIO_UNIT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_unit_desc_by_index(desc_index);
        audio_unit_desc_ref->send_get_sampling_rate_cmd((void *)cmd_notification_id);
        status = sys->get_last_resp_status();

        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            std::cout << "Sampling rate: " << std::dec << audio_unit_desc_ref->get_sampling_rate_sampling_rate();
        }

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_VIDEO_CLUSTER)
    {
        std::cout << "\nVideo Cluster descriptor is not implemented." << std::endl;
        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_SENSOR_CLUSTER)
    {
        std::cout << "\nSensor Cluster descriptor is not implemented." << std::endl;
        return 1;
    }
    else
    {
        std::cout << "cmd_get_sampling_rate error" << std::endl;
        return -1;
    }

    return 0;
}

int cmd_line::cmd_set_clock_source(std::string desc_name, uint16_t desc_index, uint16_t new_clk_src_index)
{
    int status = -1;
    intptr_t cmd_notification_id = get_next_notification_id();

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_domain_desc_by_index(desc_index);
    clk_domain_desc_ref->send_set_clock_source_cmd((void *)cmd_notification_id, new_clk_src_index);
    status = sys->get_last_resp_status();

    if(status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        std::cout << "Clock source index : " << std::dec << clk_domain_desc_ref->set_clock_source_clock_source_index();
    }

    return 0;
}

uint32_t cmd_line::get_next_notification_id()
{
    return (uint32_t)notification_id++;
}

int cmd_line::cmd_get_clock_source(std::string desc_name, uint16_t desc_index)
{
    intptr_t cmd_notification_id = get_next_notification_id();
    uint16_t clk_src_index = 0;
    int status = -1;

    sys->set_wait_for_next_cmd((void *)cmd_notification_id);
    avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_domain_desc_by_index(desc_index);
    clk_domain_desc_ref->send_get_clock_source_cmd((void *)cmd_notification_id);
    status = sys->get_last_resp_status();
    clk_src_index = clk_domain_desc_ref->get_clock_source_clock_source_index();

    if(status == avdecc_lib::AEM_STATUS_SUCCESS)
    {
        std::cout << "Clock source index : " << std::dec << clk_domain_desc_ref->get_clock_source_by_index(clk_src_index);
    }

    return 0;
}

int cmd_line::cmd_start_streaming(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    intptr_t cmd_notification_id = 0;

    if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_start_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_start_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();

        return 1;
    }
    else
    {
        std::cout << "cmd_start_streaming error" << std::endl;
        return -1;
    }
}

int cmd_line::cmd_stop_streaming(std::string desc_name, uint16_t desc_index)
{
    uint16_t desc_type_value = utility->aem_desc_name_to_value(desc_name.c_str());
    intptr_t cmd_notification_id = 0;

    if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
        stream_input_desc_ref->send_stop_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();

        return 1;
    }
    else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
    {
        cmd_notification_id = get_next_notification_id();
        sys->set_wait_for_next_cmd((void *)cmd_notification_id);
        avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_obj->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
        stream_output_desc_ref->send_stop_streaming_cmd((void *)cmd_notification_id);
        sys->get_last_resp_status();

        return 1;
    }
    else
    {
        std::cout << "cmd_stop_streaming error" << std::endl;
        return -1;
    }
}

void cmd_line::cmd_path()
{
    std::cout << "Log path: " << log_path << std::endl;
}

int cmd_line::cmd_path(std::string new_log_path)
{
    log_path = new_log_path;
    return 0;
}

bool cmd_line::is_setting_valid(uint32_t end_station, uint16_t entity, uint16_t config)
{
    bool is_setting_valid = (end_station < controller_obj->get_end_station_count()) &&
                            (entity < controller_obj->get_end_station_by_index(end_station)->entity_desc_count()) &&
                            (config == controller_obj->get_end_station_by_index(end_station)->get_entity_desc_by_index(entity)->current_configuration());

    return is_setting_valid;
}
