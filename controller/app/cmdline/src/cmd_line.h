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
 * cmd_line.h
 *
 * AVDECC Controller command line processing class
 */

#pragma once
#include <fstream>
#include <sstream>
#include <map>
#include <queue>

#include "net_interface.h"
#include "system.h"
#include "controller.h"
#include "util.h"
#include "enumeration.h"
#include "cmd_line_help.h"

#include "locale_descriptor.h"
#include "strings_descriptor.h"
#include "entity_descriptor.h"
#include "cli_command.h"

class cli_argument;

class AtomicOut : public std::ostream
{
  public:
    AtomicOut() : std::ostream(0), buffer()
    {
      this->init(buffer.rdbuf());
    }

    ~AtomicOut()
    {
      // Use printf as cout seems to still be interleaved
      printf("%s", buffer.str().c_str());
    }

  private:
    std::ostringstream buffer;
};

#define atomic_cout AtomicOut()

class cmd_line
{
private:
    avdecc_lib::net_interface *netif;
    avdecc_lib::system *sys;
    avdecc_lib::controller *controller_obj;
    
    cli_command commands;

    uint32_t current_end_station;
    intptr_t notification_id;

    bool test_mode;

    std::streambuf *cout_buf;
    std::ofstream ofstream_ref;
    bool output_redirected;

public:
    static std::string log_path;

    cmd_line();

    /**
     * Constructor for cmd_line used for constructing an object with notification and log callback functions.
     */
    cmd_line(void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
             void (*log_callback) (void *, int32_t, const char *, int32_t),
             bool test_mode, char *interface, int32_t log_level);

    ~cmd_line();

private:
    int print_interfaces_and_select(char *interface);
    int get_current_end_station(avdecc_lib::end_station **end_station) const;
    int get_current_entity_and_descriptor(avdecc_lib::end_station *end_station,
        avdecc_lib::entity_descriptor **entity, avdecc_lib::configuration_descriptor **descriptor);
    int get_current_end_station_entity_and_descriptor(avdecc_lib::end_station **end_station,
        avdecc_lib::entity_descriptor **entity, avdecc_lib::configuration_descriptor **configuration);

    void cmd_line_commands_init();

    void print_desc_type_index_name_row(avdecc_lib::descriptor_base &desc,
                                        const uint8_t *localized_desc_string,
                                        avdecc_lib::locale_descriptor &locale);

public:

    /**
     * Access methods used for command-line completion
     */
    const cli_command *get_commands() const;
    avdecc_lib::controller *get_controller() const;

    /**
     * Try to execute a command
     */
    bool handle(std::vector<std::string> &args);

    /**
     * Find an endstation index from an argument string.
     */
    bool get_end_station_index(std::string arg, uint32_t &end_station_index) const;

    bool is_output_redirected() const;

private:

    /**
     * Display summary help for all commands.
     */
    int cmd_help_all(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display a detailed description of a command.
     */
    int cmd_help_one(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display a detailed description of a command.
     */
    int cmd_quit(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display the current build release version.
     */
    int cmd_version(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display a table with information about each end station discovered with ADP.
     */
    int cmd_list(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display the current end station and configuration setting.
     */
    int cmd_show_select(int total_matched, std::vector<cli_argument*> args);

    /**
     * Change the setting of end station, entity, and configuration.
     */
    int cmd_select(int total_matched, std::vector<cli_argument*> args);

    int do_select(uint32_t new_end_station, uint16_t new_entity, uint16_t new_config);

    /**
     * Change the base log level for messages to be logged by the logging callback.
     */
    int cmd_log_level(int total_matched, std::vector<cli_argument*> args);

    /*
     * Re-direct logging to a file.
     */
    int cmd_log(int total_matched, std::vector<cli_argument*> args);

    /*
     * Re-direct logging back to stdout
     */
    int cmd_unlog(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display all the descriptors in each end station.
     */
    int cmd_view_all(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display a list of descriptors that has the Clock Sync Source flag set.
     */
    int cmd_view_media_clock(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display all the descriptors with details in the end station.
     */
    int cmd_view_details(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display information for the specified descriptor using the current end station, entity, and configuration setting.
     */
    int cmd_view_descriptor(int total_matched, std::vector<cli_argument*> args);

    int do_view_descriptor(std::string desc_name, uint16_t desc_index);

    /**
     * Display all the available instreams for all End Stations.
     */
    int cmd_connect(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display all the available outstreams for all End Stations that can connect with the instreams.
     */
    int cmd_connect_dst(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a CONNECT_RX command to connect Listener sink stream.
     */
    int cmd_connect_rx(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a CONNECT_RX command to disconnect Listener sink stream.
     */
    int cmd_disconnect_rx(int total_matched, std::vector<cli_argument*> args);

    /**
     * Display all current connections.
     */
    int cmd_show_connections(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_TX_STATE command to get Talker source stream connection state.
     */
    int cmd_get_tx_state(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_RX_STATE command to get Listener sink stream connection state.
     */
    int cmd_get_rx_state(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_TX_CONNECTION command with a notification id to get a specific Talker connection information.
     */
    int cmd_get_tx_connection(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send an ACQUIRE_ENTITY command to obtain exclusive access to an entire Entity or a sub-tree of objects.
     */
    int cmd_acquire_entity(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a LOCK_ENTITY command to provide short term exclusive access to the AVDECC Entity to perform atomic operations.
     */
    int cmd_lock_entity(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a ENTITY_AVAILABLE command to determine if another AVDECC Entity is still alive.
     */
    int cmd_entity_avail(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a REBOOT command to reboot the entity
     */
    int cmd_reboot(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a CONTROLLER_AVAILABLE command to determine if an AVDECC Controller is still alive.
     */
    int cmd_controller_avail(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a READ_DESCRIPTOR command to read a descriptor from an AVDECC Entity.
     */
    int cmd_read_descriptor(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a SET_STREAM_FORMAT command to change the format of a stream.
     */
    int cmd_set_stream_format(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_STREAM_FORMAT command with nofitication id to fetch the current format of a stream.
     */
    int cmd_get_stream_format(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a SET_STREAM_INFO command to change a stream info field value to a new value.
     */
    int cmd_set_stream_info(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_STREAM_INFO command to fetch the current information of a stream.
     */
    int cmd_get_stream_info(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a SET_NAME command to change the value of a name field within a descriptor.
     */
    int cmd_set_name(std::string desc_name, uint16_t desc_index, uint16_t name_index, std::string new_name);

    /**
     * Send a GET_NAME command to fetch the value of a name field within a descriptor.
     */
    int cmd_get_name(std::string desc_name, uint16_t desc_index, uint16_t name_index);

    /**
     * Send a SET_SAMPLING_RATE command to change the sampling rate of a port or unit.
     */
    int cmd_set_sampling_rate(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_SAMPLING_RATE command to get the current sampling rate of a port or unit.
     */
    int cmd_get_sampling_rate(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a SET_CLOCK_SOURCE command to change the clock source of a clock domain.
     */
    int cmd_set_clock_source(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a GET_CLOCK_SOURCE command to get the current clock source of a clock domain.
     */
    int cmd_get_clock_source(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a START_STREAMING command with a notification id to start streaming on a previously connected stream that was connected
     * via ACMP or has previously been stopped with the STOP_STREAMING command.
     */
    int cmd_start_streaming(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a STOP_STREAMING command with a notification id to stop a connected stream for streaming media.
     */
    int cmd_stop_streaming(int total_matched, std::vector<cli_argument*> args);

    int cmd_firmware_upgrade(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a IDENTIFY command to enable identification.
     */
    int cmd_identify_on(int total_matched, std::vector<cli_argument*> args);

    /**
     * Send a IDENTIFY command to disable identification.
     */
    int cmd_identify_off(int total_matched, std::vector<cli_argument*> args);

    void do_identify(uint32_t end_station_index, bool turn_on);

    /**
     * Display the location of the redirected output file.
     */
    int cmd_show_path(int total_matched, std::vector<cli_argument*> args);

    /**
     * Change the path of the redirected output file.
     */
    int cmd_set_path(int total_matched, std::vector<cli_argument*> args);

    /**
     * Clear the screen.
     */
    int cmd_clr(int total_matched, std::vector<cli_argument*> args);

    /**
     * Get the next unique notification id.
     */
    uint32_t get_next_notification_id();

    /**
     * Check if end station, entity, and configuration setting is in range and valid.
     */
    bool is_setting_valid(uint32_t end_station, uint16_t entity, uint16_t config);
};

extern const char *const log_level_help;
