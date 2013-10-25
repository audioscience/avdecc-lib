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
 * avdecc_cmd_line.h
 *
 * AVDECC command line processing class
 */

#pragma once
#ifndef _AVDECC_CMD_LINE_H_
#define _AVDECC_CMD_LINE_H_

#include "net_interface.h"
#include "system.h"
#include "controller.h"
#include "aem_string.h"
#include "enumeration.h"

class avdecc_cmd_line
{
private:
        static std::string log_path;
        avdecc_lib::net_interface *netif_ref;
        avdecc_lib::system *system_ref;
        avdecc_lib::controller *controller_ref;
        static uint32_t end_station;
        static uint16_t entity;
        static uint16_t config;
        static uint32_t notification_id;

public:
        /**
         * An empty constructor for avdecc_cmd_line
         */
        avdecc_cmd_line();

        /**
         * Constructor for avdecc_cmd_line used for constructing an object with notification and log callback functions.
         */
        avdecc_cmd_line(void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, void *),
                        void (*log_callback) (void *, int32_t, const char *, int32_t));

        /**
         * Destructor for avdecc_cmd_line used for destroying objects
         */
        ~avdecc_cmd_line();

private:
        int print_interfaces_and_select();

        int display_desc_info(uint16_t desc_type, uint16_t desc_index);

public:

        /**
         * Display a list of valid commands.
         */
        int cmd_help();

        /**
         * Display the current build release version.
         */
        int cmd_version();

        /**
         * Display a table with information about each end station discovered using ADP.
         */
        int cmd_list();

        /**
         * Display a list of descriptors that has the Clock Sync Source flag set.
         */
        int cmd_list_clock_sync_source();

        /**
         * Display the current end station and configuration setting.
         */
        void cmd_select();

        /**
         * Change the setting of end station and configuration.
         */
        int cmd_select(uint32_t new_end_station, uint16_t new_entity, uint16_t new_config);

        /**
         * Display all the descriptors in each end station.
         */
        int cmd_view_all();

        /**
         * Display information for the specified descriptor using the current end station and configuration setting.
         */
        int cmd_view_descriptor(std::string desc_name, uint16_t desc_index);

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
         * Send a READ_DESCRIPTOR command to get the localized strings from an AVDECC Entity.
         */
        int cmd_read_descriptor(std::string desc_name, uint16_t desc_index);

        /**
         * Send a SET_STREAM_FORMAT command to change the format of a stream.
         */
        int cmd_set_stream_format(std::string desc_name, uint16_t desc_index, uint64_t new_stream_format);

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
         * Display the GET_STREAM_INFO Stream ID, MSRP Accumulated Latency, or Stream Dest MAC field.
         */
        int cmd_get_stream_info(std::string desc_name, uint16_t desc_index, std::string stream_info_field);

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
         * Display the location of the redirected output file.
         */
        void cmd_path();

        /**
         * Change the path of the redirected output file.
         */
        int cmd_path(std::string new_log_path);

        /**
         * Check if end station and configuration setting is in range and valid.
         */
        bool is_setting_valid(uint32_t end_station, uint16_t entity, uint16_t config);
};

#endif