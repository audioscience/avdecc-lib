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
 * end_station_imp.h
 *
 * End Station implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_END_STATION_IMP_H_
#define _AVDECC_CONTROLLER_LIB_END_STATION_IMP_H_

#include "entity_descriptor_imp.h"
#include "end_station.h"

namespace avdecc_lib
{
    class adp;

    class end_station_imp : public virtual end_station
    {
    private:
        uint64_t end_station_guid; // The unique identifier of the AVDECC Entity the command is targeted to
        uint64_t end_station_mac; // The source MAC address of the End Station
        char end_station_connection_status; // The connection status of an End Station
        uint16_t current_entity_desc; // The Entity descriptor associated with the End Station
        uint16_t current_config_desc; // The Configuration descriptor associated with the Entity descriptor in the same End Station

        enum read_top_level_desc_in_config_states
        {
            READ_TOP_LEVEL_DESC_IN_CONFIG_IDLE,
            READ_TOP_LEVEL_DESC_IN_CONFIG_STARTING,
            READ_TOP_LEVEL_DESC_IN_CONFIG_RUNNING,
            READ_TOP_LEVEL_DESC_IN_CONFIG_DONE
        };

        enum read_top_level_desc_in_config_states read_top_level_desc_in_config_state; // States used for processing the top level descriptors present in the Configuration descriptor.

        uint16_t desc_type_from_config; // The top level descriptor type  present in the Configuration Descriptor
        uint16_t desc_type_index_from_config; // The top level descriptor type index present in the Configuration Descriptor
        uint16_t desc_count_from_config; // The top level descriptor count present in the Configuration Descriptor
        uint16_t desc_count_index_from_config; // The top level descriptor count index present in the Configuration Descriptor

        adp *adp_ref; // ADP associated with the End Station
        std::vector<entity_descriptor_imp *> entity_desc_vec; // Store a list of Entity descriptor class objects

    public:
        /**
         * Constructor for end_station_imp used for constructing an object with network interface , Ethernet frame
         * and memory buffer length information.
         */
        end_station_imp(const uint8_t *frame, size_t frame_len);

        virtual ~end_station_imp();

    private:
        /**
         * Initialize End Station with Entity and Configuration descriptors information.
         */
        int end_station_init();

        /**
         * Initialize End Station by sending non blocking Read Descriptor commands to read
         * all the descriptors for the End Station.
         */
        int read_desc_init(uint16_t desc_type, uint16_t desc_index);

        /**
         * Send a READ_DESCRIPTOR command with or without a notification id based on the post_notification_msg flag
         * to read a descriptor from an AVDECC Entity.
         */
        int send_read_desc_cmd_with_flag(void *notification_id, uint32_t notification_flag, uint16_t desc_type, uint16_t desc_index);

    public:
        /**
         * Get the status of the End Station connection.
         */
        char STDCALL get_connection_status();

        /**
         * Change the End Station connection status to connected.
         */
        void set_connected();

        /**
         * Change the End Station connection status to disconnected.
         */
        void set_disconnected();

        /**
         * Get the GUID of the End Station.
         */
        uint64_t STDCALL get_end_station_guid();

        /**
         * Get the source MAC address of the End Station.
         */
        uint64_t STDCALL get_end_station_mac();

        /**
         * Get the ADP associated with the End Station.
         */
        adp * get_adp();

        /**
         * Get the number of Entity descriptors for this End Station.
         */
        uint32_t STDCALL get_entity_desc_count();

        /**
         * Get the corresponding Entity descriptor by index.
         */
        entity_descriptor * STDCALL get_entity_desc_by_index(uint32_t entity_desc_index);

        /**
         * Send a READ_DESCRIPTOR command to read a descriptor from an AVDECC Entity. Reading a descriptor can be performed
         * by any AVDECC Controller even when the AVDECC Entityis locked or acquired as the act of reading the descriptor
         * does not affect the AVDECC Entity state.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param desc_type The type of the descriptor to be read by the command.
         * \param desc_index The index of the descriptor to be read by the command.
         */
        int STDCALL send_read_desc_cmd(void *notification_id, uint16_t desc_type, uint16_t desc_index);

        /**
         * Process a READ_DESCRIPTOR response for the READ_DESCRIPTOR command. On success, the descriptor field is
         * set to the contents of the descriptor. On failure, the descriptor contains the descriptor type and index
         * and are in the same location as in the command frame.
         */
        int proc_read_desc_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Send a ENTITY_AVAILABLE command to verify that an AVDECC Entity is still available and responding to commands.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        int STDCALL send_entity_avail_cmd(void *notification_id);

        /**
         * Process a ENTITY_AVAILABLE response for the ENTITY_AVAILABLE command.
         */
        int proc_entity_avail_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Process response received for the corresponding command.
         */
        int proc_rcvd_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);
    };
}

#endif
