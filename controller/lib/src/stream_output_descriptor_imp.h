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
 * stream_output_descriptor_imp.h
 *
 * STREAM OUTPUT descriptor implementation class
 */

#pragma once

#include <iostream>
#include <map>
#include <string>
using namespace std;

#include "jdksavdecc_acmp.h"
#include "descriptor_base_imp.h"
#include "stream_output_descriptor.h"

namespace avdecc_lib
{
    class stream_output_descriptor_imp : public stream_output_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_stream stream_output_desc; // Structure containing the stream_output_desc fields

		std::map <string, int> stream_info_flags;

        struct stream_output_desc_stream_flags
        {
            bool clock_sync_source;
            bool class_a;
            bool class_b;
            bool supports_encrypted;
            bool primary_backup_supported;
            bool primary_backup_valid;
            bool secondary_backup_supported;
            bool secondary_backup_valid;
            bool tertiary_backup_supported;
            bool tertiary_backup_valid;
        };

        struct stream_output_desc_stream_flags stream_output_flags;
        struct jdksavdecc_aem_command_set_stream_format_response aem_cmd_set_stream_format_resp; // Store the response received after sending a SET_STREAM_FORMAT command.
        struct jdksavdecc_aem_command_get_stream_format_response aem_cmd_get_stream_format_resp; // Store the response received after sending a GET_STREAM_FORMAT command.
        struct jdksavdecc_aem_command_set_stream_info_response aem_cmd_set_stream_info_resp; // Store the response received after sending a SET_STREAM_INFO command.
        struct jdksavdecc_aem_command_get_stream_info_response aem_cmd_get_stream_info_resp; // Store the response received after sending a GET_STREAM_INFO command.

        struct jdksavdecc_acmpdu acmp_cmd_get_tx_state_resp; // Store the response received after sending a GET_TX_STATE command.
        struct jdksavdecc_acmpdu acmp_cmd_get_tx_connection_resp; // Store the response received after sending a GET_TX_CONNECTION command.

    public:
        stream_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~stream_output_descriptor_imp();

        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        uint8_t * STDCALL object_name();
        uint16_t STDCALL localized_description();

        uint16_t STDCALL clock_domain_index();
        uint16_t STDCALL stream_flags();
        bool STDCALL stream_flags_clock_sync_source();
        bool STDCALL stream_flags_class_a();
        bool STDCALL stream_flags_class_b();
        bool STDCALL stream_flags_supports_encrypted();
        bool STDCALL stream_flags_primary_backup_supported();
        bool STDCALL stream_flags_primary_backup_valid();
        bool STDCALL stream_flags_secondary_backup_supported();
        bool STDCALL stream_flags_secondary_backup_valid();
        bool STDCALL stream_flags_tertiary_backup_supported();
        bool STDCALL stream_flags_tertiary_backup_valid();
        const char * STDCALL current_format();
        uint16_t formats_offset();
        uint16_t STDCALL number_of_formats();
        uint64_t STDCALL backup_talker_entity_id_0();
        uint16_t STDCALL backup_talker_unique_0();
        uint64_t STDCALL backup_talker_entity_id_1();
        uint16_t STDCALL backup_talker_unique_1();
        uint64_t STDCALL backup_talker_entity_id_2();
        uint16_t STDCALL backup_talker_unique_2();
        uint64_t STDCALL backedup_talker_entity_id();
        uint16_t STDCALL backedup_talker_unique();
        uint16_t STDCALL avb_interface_index();
        uint32_t STDCALL buffer_length();
        uint64_t STDCALL set_stream_format_stream_format();
        uint64_t STDCALL get_stream_format_stream_format();
        uint32_t STDCALL get_stream_info_flags();
        uint64_t STDCALL get_stream_info_stream_format();
        uint64_t STDCALL get_stream_info_stream_id();
        uint32_t STDCALL get_stream_info_msrp_accumulated_latency();
        uint64_t STDCALL get_stream_info_stream_dest_mac();
        uint8_t STDCALL get_stream_info_msrp_failure_code();
        uint64_t STDCALL get_stream_info_msrp_failure_bridge_id();
        uint16_t STDCALL get_stream_info_stream_vlan_id() ;
        bool is_clock_sync_source_set();
        uint64_t STDCALL get_tx_state_stream_id();
        uint64_t STDCALL get_tx_state_stream_dest_mac();
        uint16_t STDCALL get_tx_state_connection_count();
        uint16_t STDCALL get_tx_state_stream_vlan_id();
        uint64_t STDCALL get_tx_connection_stream_id();
        uint16_t STDCALL get_tx_connection_talker_unique_id();
        uint16_t STDCALL get_tx_connection_listener_unique_id();
        uint64_t STDCALL get_tx_connection_stream_dest_mac();
        uint16_t STDCALL get_tx_connection_connection_count();
        uint16_t STDCALL get_tx_connection_stream_vlan_id();
        uint64_t STDCALL get_tx_connection_listener_entity_id();

		int STDCALL send_set_stream_format_cmd(void *notification_id, uint64_t new_stream_format);
        int proc_set_stream_format_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_get_stream_format_cmd(void *notification_id);
        int proc_get_stream_format_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_set_stream_info_vlan_id_cmd(void *notification_id, uint16_t vlan_id);
        int proc_set_stream_info_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_get_stream_info_cmd(void *notification_id);
        int proc_get_stream_info_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

		bool STDCALL get_stream_info_flag(const char *flag);

        int STDCALL send_start_streaming_cmd(void *notification_id);
        int proc_start_streaming_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_stop_streaming_cmd(void *notification_id);
        int proc_stop_streaming_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_get_tx_state_cmd(void *notification_id);
        int proc_get_tx_state_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_get_tx_connection_cmd(void *notification_id, uint64_t listener_entity_id, uint16_t listener_unique_id);
        int proc_get_tx_connection_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

    private:
        /**
         * Store the stream flags components of the STREAM OUTPUT descriptor object in a vector.
         */
        void stream_flags_init();

        /**
         * Update the internal STREAM OUTPUT descriptor's stream format field.
         */
        void update_stream_format(struct jdksavdecc_eui64 stream_format);
    };
}
