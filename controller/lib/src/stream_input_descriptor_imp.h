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
#ifndef _AVDECC_CONTROLLER_LIB_STREAM_INPUT_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_STREAM_INPUT_DESCRIPTOR_IMP_H_

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
		 * \return The descriptor type of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_descriptor_type();

		/**
		 * \return The descriptor index of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_descriptor_index();

		/**
		 * \return The name of the Stream Input descriptor object.
		 */
		uint8_t * STDCALL get_object_name();

		/**
		 * \return The localized description of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_localized_description();

		/**
		 * \return The Clock Domain index of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_clock_domain_index();

		/**
		 * \return The stream flags of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_stream_flags();

		/**
		 * \return True if the stream can be used as a clock synchronization source.
		 */
		bool STDCALL get_stream_flags_clock_sync_source();

		/**
		 * \return True if the stream supports streaming at Class A.
		 */
		bool STDCALL get_stream_flags_class_a();

		/**
		 * \return True if the stream supports streaming at Class B.
		 */
		bool STDCALL get_stream_flags_class_b();

		/**
		 * \return True if the stream supports streaming with encrypted PDUs.
		 */
		bool STDCALL get_stream_flags_supports_encrypted();

		/**
		 * \return True if the backup_talker_entity_id_0 and backup_talker_unique_id_0 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_primary_backup_supported();

		/**
		 * \return True if the backup_talker_entity_id_0 and backup_talker_unique_id_0 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_primary_backup_valid();

		/**
		 * \return True if the backup_talker_entity_id_1 and backup_talker_unique_id_1 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_secondary_backup_supported();

		/**
		 * \return True if the backup_talker_entity_id_1 and backup_talker_unique_id_1 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_secondary_backup_valid();

		/**
		 * \return True if the backup_talker_entity_id_2 and backup_talker_unique_id_2 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_tertiary_backup_supported();

		/**
		 * \return True if the backup_talker_entity_id_2 and backup_talker_unique_id_2 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_tertiary_back_up_valid();

		/**
		* \return The current format of the Stream Input descriptor object.
		*/
		char * STDCALL get_current_format();

		/**
		 * \return The formats offset of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_formats_offset();

		/**
		 * \return The number of formats of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_number_of_formats();

		/**
		 * \return The backup talker entity id 0 of the Stream Input descriptor object.
		 */
		uint64_t STDCALL get_backup_talker_entity_id_0();

		/**
		 * \return The backup talker unique 0 of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_backup_talker_unique_0();

		/**
		 * \return The backup talker entity id 1 of the Stream Input descriptor object.
		 */
		uint64_t STDCALL get_backup_talker_entity_id_1();

		/**
		 * \return The backup talker unique 1 of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_backup_talker_unique_1();

		/**
		 * \return The backup talker entity id 2 of the Stream Input descriptor object.
		 */
		uint64_t STDCALL get_backup_talker_entity_id_2();

		/**
		 * \return The backup talker unique 2 of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_backup_talker_unique_2();

		/**
		 * \return The backedup talker entity id of the Stream Input descriptor object.
		 */
		uint64_t STDCALL get_backedup_talker_entity_id();

		/**
		 * \return The backedup talker unique of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_backedup_talker_unique();

		/**
		 * \return The AVB Interface index of the Stream Input descriptor object.
		 */
		uint16_t STDCALL get_avb_interface_index();

		/**
		 * \return The buffer length of the Stream Input descriptor object.
		 */
		uint32_t STDCALL get_buffer_length();

		/**
		 * \return The stream format of a stream after sending a GET_STREAM_FORMAT command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_format_stream_format();

		/**
		 * \return The stream info flags of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint32_t STDCALL get_stream_info_flags();

		/**
		 * \return The stream info stream format of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_info_stream_format();

		/**
		 * \return The stream info stream id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_info_stream_id();

		/**
		 * \return The stream info MSRP accumulated latency of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint32_t STDCALL get_stream_info_msrp_accumulated_latency();

		/**
		 * \return The stream dest mac of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint8_t * STDCALL get_stream_info_stream_dest_mac();

		/**
		 * \return The stream info MSRP failure code of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint8_t STDCALL get_stream_info_msrp_failure_code();

		/**
		 * \return The stream info MSRP failure bridge id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_info_msrp_failure_bridge_id();

		/**
		 * \return True if the Clock Sync Source flag is set.
		 */
		bool is_clock_sync_source_set();

		/**
		 * Send a SET_STREAM_FORMAT command with a notification id to change the format of a stream.
		 */
		int STDCALL send_set_stream_format_cmd(void *notification_id, uint16_t desc_index, uint64_t new_stream_format);

		/**
		 * Process a SET_STREAM_FORMAT response for the SET_STREAM_FORMAT command.
		 */
		int proc_set_stream_format_resp(void *notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len);

		/**
		 * Send a GET_STREAM_FORMAT command with a notification id to fetch the current format of a stream.
		 */
		int STDCALL send_get_stream_format_cmd(void *notification_id, uint16_t desc_index);

		/**
		 * Process a GET_STREAM_FORMAT response for the GET_STREAM_FORMAT command.
		 */
		int proc_get_stream_format_resp(void *&notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len, int &status);

		/**
		 * Send a SET_STREAM_INFO command with a notification id to change the current values of the dynamic information of the stream.
		 */
		int STDCALL send_set_stream_info_cmd(void *notification_id, uint16_t desc_index, void *new_stream_info_field);

		/**
		 * Process a SET_STREAM_INFO response for the SET_STREAM_INFO command.
		 */
		int proc_set_stream_info_resp(void *notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len);

		/**
		 * Send a GET_STREAM_INFO command with a notification id to fetch the current information for a stream.
		 */
		int STDCALL send_get_stream_info_cmd(void *notification_id, uint16_t desc_index);

		/**
		 * Process a GET_STREAM_INFO response for the GET_STREAM_INFO command.
		 */
		int proc_get_stream_info_resp(void *&notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len, int &status);
	};
}

#endif