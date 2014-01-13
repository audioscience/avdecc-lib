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

#include "net_interface.h"
#include "system.h"
#include "controller.h"
#include "util.h"
#include "enumeration.h"
#include "cmd_line_help.h"

#include "locale_descriptor.h"
#include "strings_descriptor.h"

class cmd_line
{
private:
    avdecc_lib::net_interface *netif;
    avdecc_lib::system *sys;
    avdecc_lib::controller *controller_obj;
    

    uint32_t current_end_station;
    uint16_t current_entity;
    uint16_t current_config;
    intptr_t notification_id;

    std::vector<cmd_line_help *> cmd_line_help_vec;

public:
    static avdecc_lib::util *utility;
    static std::string log_path;

    cmd_line();

    /**
     * Constructor for cmd_line used for constructing an object with notification and log callback functions.
     */
    cmd_line(void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
             void (*log_callback) (void *, int32_t, const char *, int32_t));

    ~cmd_line();

private:
    int print_interfaces_and_select();

    void cmd_line_help_init();

    void print_desc_type_index_name_row(avdecc_lib::descriptor_base &desc,
                                        avdecc_lib::strings_descriptor &strings,
                                        avdecc_lib::locale_descriptor &locale);

public:

    /**
     * Display a list of valid commands with brief descriptions.
     */
    int cmd_help();

    /**
     * Display a detailed description of a command.
     */
    int cmd_help_details(std::string cmd);

    /**
     * Display the current build release version.
     */
    int cmd_version();

    /**
     * Display a table with information about each end station discovered with ADP.
     */
    int cmd_list();

    /**
     * Display the current end station and configuration setting.
     */
    void cmd_select();

    /**
     * Change the setting of end station, entity, and configuration.
     */
    int cmd_select(uint32_t new_end_station, uint16_t new_entity, uint16_t new_config);

    /**
     * Change the base log level for messages to be logged by the logging callback.
     */
    int cmd_log_level(uint32_t new_log_level);

    /**
     * Display all the descriptors in each end station.
     */
    int cmd_view_all();

    /**
     * Display a list of descriptors that has the Clock Sync Source flag set.
     */
    int cmd_view_media_clock();

    /**
     * Display all the descriptors with details in the end station.
     */
    int cmd_view_details(uint32_t end_station_index);

    /**
     * Display information for the specified descriptor using the current end station, entity, and configuration setting.
     */
    int cmd_view_descriptor(std::string desc_name, uint16_t desc_index);

    /**
     * Display all the available instreams for all End Stations.
     */
    int cmd_connect();

    /**
     * Display all the available outstreams for all End Stations that can connect with the instreams.
     */
    int cmd_connect(uint32_t instream_end_station_index, uint16_t instream_desc_index);

    /**
     * Send a CONNECT_RX command to connect Listener sink stream.
     */
    int cmd_connect_rx(uint32_t instream_end_station_index,
                       uint16_t instream_desc_index,
                       uint32_t outstream_end_station_index,
                       uint16_t outstream_desc_index,
                       std::string flags);

    /**
     * Send a CONNECT_RX command to disconnect Listener sink stream.
     */
    int cmd_disconnect_rx(uint32_t instream_end_station_index,
                          uint16_t instream_desc_index,
                          uint32_t outstream_end_station_index,
                          uint16_t outstream_desc_index);

    /**
     * Send a GET_TX_STATE command to get Talker source stream connection state.
     */
    int cmd_get_tx_state(uint32_t outstream_end_station_index, uint16_t outstream_desc_index);

    /**
     * Send a GET_RX_STATE command to get Listener sink stream connection state.
     */
    int cmd_get_rx_state(uint32_t instream_end_station_index, uint16_t instream_desc_index);

    /**
     * Send a GET_TX_CONNECTION command with a notification id to get a specific Talker connection information.
     */
    int cmd_get_tx_connection(uint32_t outstream_end_station_index, uint16_t outstream_desc_index);

    /**
     * Send an ACQUIRE_ENTITY command to obtain exclusive access to an entire Entity or a sub-tree of objects.
     */
    int cmd_acquire_entity(std::string flag_name, std::string desc_name, uint16_t desc_index);

    /**
     * Send a LOCK_ENTITY command to provide short term exclusive access to the AVDECC Entity to perform atomic operations.
     */
    int cmd_lock_entity(std::string flag_name, std::string desc_name, uint16_t desc_index);

    /**
     * Send a ENTITY_AVAILABLE command to determine if another AVDECC Entity is still alive.
     */
    int cmd_entity_avail();

    /**
     * Send a CONTROLLER_AVAILABLE command to determine if an AVDECC Controller is still alive.
     */
    int cmd_controller_avail();

    /**
     * Send a READ_DESCRIPTOR command to read a descriptor from an AVDECC Entity.
     */
    int cmd_read_descriptor(std::string desc_name, uint16_t desc_index);

    /**
     * Send a SET_STREAM_FORMAT command to change the format of a stream.
     */
    int cmd_set_stream_format(std::string desc_name, uint16_t desc_index, std::string new_stream_format_name);

    /**
     * Send a GET_STREAM_FORMAT command with nofitication id to fetch the current format of a stream.
     */
    int cmd_get_stream_format(std::string desc_name, uint16_t desc_index);

    /**
     * Send a SET_STREAM_INFO command to change a stream info field value to a new value.
     */
    int cmd_set_stream_info(std::string desc_name, uint16_t desc_index, std::string stream_info_field,
                            uint64_t new_stream_info_field_value);

    /**
     * Send a GET_STREAM_INFO command to fetch the current information of a stream.
     */
    int cmd_get_stream_info(std::string desc_name, uint16_t desc_index);

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
    int cmd_set_sampling_rate(std::string desc_name, uint16_t desc_index, uint32_t new_sampling_rate);

    /**
     * Send a GET_SAMPLING_RATE command to get the current sampling rate of a port or unit.
     */
    int cmd_get_sampling_rate(std::string desc_name, uint16_t desc_index);

    /**
     * Send a SET_CLOCK_SOURCE command to change the clock source of a clock domain.
     */
    int cmd_set_clock_source(std::string desc_name, uint16_t desc_index, uint16_t new_clk_src_index);

    /**
     * Send a GET_CLOCK_SOURCE command to get the current clock source of a clock domain.
     */
    int cmd_get_clock_source(std::string desc_name, uint16_t desc_index);

    /**
     * Send a START_STREAMING command with a notification id to start streaming on a previously connected stream that was connected
     * via ACMP or has previously been stopped with the STOP_STREAMING command.
     */
    int cmd_start_streaming(std::string desc_name, uint16_t desc_index);

    /**
     * Send a STOP_STREAMING command with a notification id to stop a connected stream for streaming media.
     */
    int cmd_stop_streaming(std::string desc_name, uint16_t desc_index);

    /**
     * Display the location of the redirected output file.
     */
    void cmd_path();

    /**
     * Change the path of the redirected output file.
     */
    int cmd_path(std::string new_log_path);

    /**
     * Get the next unique notification id.
     */
    uint32_t get_next_notification_id();

    /**
     * Check if end station, entity, and configuration setting is in range and valid.
     */
    bool is_setting_valid(uint32_t end_station, uint16_t entity, uint16_t config);
};

