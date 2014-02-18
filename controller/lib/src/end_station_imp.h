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
        uint16_t current_entity_desc; // The ENTITY descriptor associated with the End Station
        uint16_t current_config_desc; // The CONFIGURATION descriptor associated with the ENTITY descriptor in the same End Station

        uint16_t selected_entity_index; // The controller-selected entity index
        uint16_t selected_config_index; // The controller-selected configuraition descriptor index

        enum read_top_level_desc_in_config_states
        {
            READ_TOP_LEVEL_DESC_IN_CONFIG_IDLE,
            READ_TOP_LEVEL_DESC_IN_CONFIG_RUNNING,
            READ_TOP_LEVEL_DESC_IN_CONFIG_DONE
        };

        enum read_top_level_desc_in_config_states read_top_level_desc_in_config_state; // States used for processing the top level descriptors present in the CONFIGURATION descriptor.

        enum read_desc_in_locale_states
        {
            READ_DESC_IN_LOCALE_IDLE,
            READ_DESC_IN_LOCALE_RUNNING,
            READ_DESC_IN_LOCALE_DONE
        };

        enum read_desc_in_locale_states read_desc_in_locale_state;

        enum read_desc_in_audio_unit_states
        {
            READ_DESC_IN_AUDIO_UNIT_IDLE,
            READ_DESC_IN_AUDIO_UNIT_STARTING,
            READ_DESC_IN_AUDIO_UNIT_RUNNING,
            READ_DESC_IN_AUDIO_UNIT_DONE
        };

        enum read_desc_in_audio_unit_states read_desc_in_audio_unit_state; 

        enum read_desc_in_stream_port_input_states
        {
            READ_DESC_IN_STREAM_PORT_INPUT_IDLE,
            READ_DESC_IN_STREAM_PORT_INPUT_RUNNING,
            READ_DESC_IN_STREAM_PORT_INPUT_DONE
        };

        enum read_desc_in_stream_port_input_states read_desc_in_stream_port_input_state;

        enum read_desc_in_stream_port_output_states
        {
            READ_DESC_IN_STREAM_PORT_OUTPUT_IDLE,
            READ_DESC_IN_STREAM_PORT_OUTPUT_RUNNING,
            READ_DESC_IN_STREAM_PORT_OUTPUT_DONE
        };

        enum read_desc_in_stream_port_output_states read_desc_in_stream_port_output_state;

        uint16_t desc_type_from_config; // The top level descriptor type present in the CONFIGURATION descriptor
        uint16_t desc_type_index_from_config; // The top level descriptor type index present in the CONFIGURATION descriptor
        uint16_t desc_count_from_config; // The top level descriptor count present in the CONFIGURATION descriptor
        uint16_t desc_count_index_from_config; // The top level descriptor count index present in the CONFIGURATION descriptor

        uint32_t read_desc_count; // A counter for the number of READ_DESCRIPTOR commands sent used to match up with the number of responses for these commands
        bool read_top_level_desc_done; // Send READ_DESCRIPTOR command for each top level descriptor present in the CONFIGURATION descriptor
        bool read_desc_in_locale_done; // Send READ_DESCRIPTOR command for STRINGS descriptor
        bool read_desc_in_audio_unit_done; // Send READ_DESCRIPTOR command for STREAM_PORT_INPUT and STREAM_PORT_OUTPUT descriptors
        bool read_desc_in_stream_port_input_done; // Send READ_DESCRIPTOR command for AUDIO_CLUSTER, AUDIO_MAP, and CONTROL descriptors
        bool read_desc_in_stream_port_output_done; // Send READ_DESCRIPTOR command for AUDIO_CLUSTER, AUDIO_MAP, and CONTROL descriptors

        uint16_t desc_type_index_from_audio_unit;
        uint16_t desc_type_index_from_stream_port_input;
        uint16_t desc_type_index_from_stream_port_output;

        adp *adp_ref; // ADP associated with the End Station
        std::vector<entity_descriptor_imp *> entity_desc_vec; // Store a list of ENTITY descriptor objects

    public:
        end_station_imp(const uint8_t *frame, size_t frame_len);
        virtual ~end_station_imp();

        const char STDCALL get_connection_status() const;

        /**
         * Change the End Station connection status to connected.
         */
        void set_connected();

        /**
         * Change the End Station connection status to disconnected.
         */
        void set_disconnected();

        uint64_t STDCALL guid();
        uint64_t STDCALL mac();
        adp * get_adp();
        size_t STDCALL entity_desc_count();
        entity_descriptor * STDCALL get_entity_desc_by_index(size_t entity_desc_index);
        int STDCALL send_read_desc_cmd(void *notification_id, uint16_t desc_type, uint16_t desc_index);
        int proc_read_desc_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_entity_avail_cmd(void *notification_id);
        int proc_entity_avail_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        int proc_rcvd_aem_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        int proc_rcvd_acmp_resp(uint32_t msg_type, void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        void STDCALL set_current_entity_index(uint16_t entity_index);
        uint16_t STDCALL get_current_entity_index() const;
        void STDCALL set_current_config_index(uint16_t entity_index);
        uint16_t STDCALL get_current_config_index() const;

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
    };
}


