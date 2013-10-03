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
 * stream_input_descriptor_imp.h
 *
 * Stream Input descriptor implementation class
 */

#pragma once
#ifndef _STREAM_INPUT_DESCRIPTOR_IMP_H_
#define _STREAM_INPUT_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "stream_input_descriptor.h"

#define IEC61883_AM824_MBLA_48KHZ_1CH UINT64_C(0x00a0020140000100)
#define IEC61883_AM824_MBLA_48KHZ_2CH UINT64_C(0x00a0020240000200)

namespace avdecc_lib
{
        class stream_input_descriptor_imp : public virtual stream_input_descriptor, public virtual descriptor_base_imp
        {
        private:
                struct jdksavdecc_descriptor_stream stream_input_desc; // Structure containing the stream_input_desc fields
                int stream_input_desc_read_returned; // Status of extracting Stream Input descriptor information from a network buffer

                struct stream_input_desc_stream_flags
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
                        bool tertiary_back_up_valid;
                };

                struct stream_input_desc_stream_flags stream_flags;
                struct jdksavdecc_aem_command_get_stream_format_response aem_cmd_get_stream_format_resp;
                struct jdksavdecc_aem_command_get_stream_info_response aem_cmd_get_stream_info_resp;

        public:
                /**
                 * An empty constructor for stream_input_descriptor_imp
                 */
                stream_input_descriptor_imp();

                /**
                 * Constructor for stream_input_descriptor_imp used for constructing an object with a base End Station, frame, position offest,
                 * and memory buffer length.
                 */
                stream_input_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

                /**
                 * Destructor for stream_input_descriptor_imp used for destroying objects
                 */
                virtual ~stream_input_descriptor_imp();

                /**
                 * Get the descriptor_type of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_descriptor_type();

                /**
                 * Get the descriptor_index of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_descriptor_index();

                /**
                 * Get the object_name of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint8_t * STDCALL get_object_name();

                /**
                 * Get the localized_description of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_localized_description();

                /**
                 * Get the clock_domain_index of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_clock_domain_index();

                /**
                 * Get the stream_flags of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_stream_flags();

                /**
                * Get the current_format of the stream_input_descriptor object.
                */
                AVDECC_LIB_API char * STDCALL get_current_format();

                /**
                 * Get the formats_offset of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_formats_offset();

                /**
                 * Get the number_of_formats of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_number_of_formats();

                /**
                 * Get the backup_talker_entity_id_0 of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_backup_talker_entity_id_0();

                /**
                 * Get the backup_talker_unique_0 of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_backup_talker_unique_0();

                /**
                 * Get the backup_talker_entity_id_1 of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_backup_talker_entity_id_1();

                /**
                 * Get the backup_talker_unique_1 of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_backup_talker_unique_1();

                /**
                 * Get the backup_talker_entity_id_2 of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_backup_talker_entity_id_2();

                /**
                 * Get the backup_talker_unique_2 of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_backup_talker_unique_2();

                /**
                 * Get the backedup_talker_entity_id of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_backedup_talker_entity_id();

                /**
                 * Get the backedup_talker_unique of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_backedup_talker_unique();

                /**
                 * Get the avb_interface_index of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint16_t STDCALL get_avb_interface_index();

                /**
                 * Get the buffer_length of the stream_input_descriptor object.
                 */
                AVDECC_LIB_API uint32_t STDCALL get_buffer_length();

                /**
                 * Get the stream_format of a stream.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_stream_format_stream_format();

                /**
                 * Get the stream_info_flags of a stream.
                 */
                AVDECC_LIB_API uint32_t STDCALL get_stream_info_flags();

                /**
                 * Get the stream_format of a stream.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_stream_info_stream_format();

                /**
                 * Get the stream_id of a stream.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_stream_info_stream_id();

                /**
                 * Get the msrp_accumulated_latency of a stream.
                 */
                AVDECC_LIB_API uint32_t STDCALL get_stream_info_msrp_accumulated_latency();

                /**
                 * Get the stream_dest_mac of a stream.
                 */
                AVDECC_LIB_API uint8_t * STDCALL get_stream_info_stream_dest_mac();

                /**
                 * Get the msrp_failure_code of a stream.
                 */
                AVDECC_LIB_API uint8_t STDCALL get_stream_info_msrp_failure_code();

                /**
                 * Get the msrp_failure_bridge_id of a stream.
                 */
                AVDECC_LIB_API uint64_t STDCALL get_stream_info_msrp_failure_bridge_id();

                /**
                 * Check if the Clock Sync Source flag is set.
                 */
                bool is_clock_sync_source_set();

                /**
                 * Send a SET_STREAM_FORMAT command with or without a notifying id based on the notifying flag to change the format of a stream.
                 */
                AVDECC_LIB_API int STDCALL send_set_stream_format_cmd(void *notification_id, uint16_t desc_index, uint64_t new_stream_format);

                /**
                 * Process a SET_STREAM_FORMAT response for the SET_STREAM_FORMAT command.
                 */
                int proc_set_stream_format_resp(void *notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len);

                /**
                 * Send a GET_STREAM_FORMAT command with or without a notifying id based on the notifying flag to fetch the current format of a stream.
                 */
                AVDECC_LIB_API int STDCALL send_get_stream_format_cmd(void *notification_id, uint16_t desc_index);

                /**
                 * Process a GET_STREAM_FORMAT response for the GET_STREAM_FORMAT command.
                 */
                int proc_get_stream_format_resp(void *notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len);

                /**
                 * Send a SET_STREAM_INFO command with or without a notifying id based on the notifying flag to change the current values
                 * of the dynamic information of the stream.
                 */
                AVDECC_LIB_API int STDCALL send_set_stream_info_cmd(void *notification_id, uint16_t desc_index, void *new_stream_info_field);

                /**
                 * Process a SET_STREAM_INFO response for the SET_STREAM_INFO command.
                 */
                int proc_set_stream_info_resp(void *notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len);

                /**
                 * Send a GET_STREAM_INFO command with or without a notifying id based on the notifying flag to fetch the current information for a stream.
                 */
                AVDECC_LIB_API int STDCALL send_get_stream_info_cmd(void *notification_id, uint16_t desc_index);

                /**
                 * Process a GET_STREAM_INFO response for the GET_STREAM_INFO command.
                 */
                int proc_get_stream_info_resp(void *notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len);
        };
}

#endif