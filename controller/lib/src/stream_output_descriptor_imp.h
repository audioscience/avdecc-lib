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
 * Stream Output descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_STREAM_OUTPUT_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_STREAM_OUTPUT_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "stream_output_descriptor.h"

namespace avdecc_lib
{
	class stream_output_descriptor_imp : public virtual stream_output_descriptor, public virtual descriptor_base_imp
	{
	private:
		struct jdksavdecc_descriptor_stream stream_output_desc; // Structure containing the stream_output_desc fields
		int stream_output_desc_read_returned; // Status of extracting Stream Output descriptor information from a network buffer

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
		struct jdksavdecc_aem_command_set_stream_format_response aem_cmd_set_stream_format_resp; // Store the response received after sending a SET_STREAM_FORMAT command.
		struct jdksavdecc_aem_command_get_stream_format_response aem_cmd_get_stream_format_resp; // Store the response received after sending a GET_STREAM_FORMAT command.
		struct jdksavdecc_aem_command_set_stream_info_response aem_cmd_set_stream_info_resp; // Store the response received after sending a SET_STREAM_INFO command.
		struct jdksavdecc_aem_command_get_stream_info_response aem_cmd_get_stream_info_resp; // Store the response received after sending a GET_STREAM_INFO command.

	public:
		/**
		 * Constructor for Stream Output descriptor object.
		 * 
		 * \param end_station_obj A pointer to the base End Station object.
		 * \param frame The raw memory that contains the descriptor information to read from.
		 * \param pos The position offset to read the descriptor fields from.
		 * \param frame_len The memory buffer length of the descriptor frame.
		 */
		stream_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		virtual ~stream_output_descriptor_imp();

	private:
		/**
		 * Store the stream flags components of the Stream Output descriptor object in a vector.
		 */
		void stream_flags_init();

	public:

		/**
		 * \return The descriptor type of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_descriptor_type();

		/**
		 * \return The descriptor index of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_descriptor_index();

		/**
		 * Get the name of the Stream Output descriptor object.
		 */
		uint8_t * STDCALL get_object_name();

		/**
		 * Get the localized description of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_localized_description();

		/**
		 * Get the Clock Domain index of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_clock_domain_index();

		/**
		 * Get the stream flags of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_stream_flags();

		/**
		 * Check if the stream can be used as a clock synchronization source.
		 */
		bool STDCALL get_stream_flags_clock_sync_source();

		/**
		 * Check if the stream supports streaming at Class A.
		 */
		bool STDCALL get_stream_flags_class_a();

		/**
		 * Check if the stream supports streaming at Class B.
		 */
		bool STDCALL get_stream_flags_class_b();

		/**
		 * Check if the stream supports streaming with encrypted PDUs.
		 */
		bool STDCALL get_stream_flags_supports_encrypted();

		/**
		 * Check if the backup_talker_entity_id_0 and backup_talker_unique_id_0 fields of
		 *	   the Stream Output descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_primary_backup_supported();

		/**
		 * Check if the backup_talker_entity_id_0 and backup_talker_unique_id_0 fields of
		 *	   the Stream Output descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_primary_backup_valid();

		/**
		 * Check if the backup_talker_entity_id_1 and backup_talker_unique_id_1 fields of
		 *	   the Stream Output descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_secondary_backup_supported();

		/**
		 * Check if the backup_talker_entity_id_1 and backup_talker_unique_id_1 fields of
		 *	   the Stream Output descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_secondary_backup_valid();

		/**
		 * Check if the backup_talker_entity_id_2 and backup_talker_unique_id_2 fields of
		 *	   the Stream Output descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_tertiary_backup_supported();

		/**
		 * Check if the backup_talker_entity_id_2 and backup_talker_unique_id_2 fields of
		 *	   the Stream Output descriptor object are supported.
		 */
		bool STDCALL get_stream_flags_tertiary_back_up_valid();

		/**
		* Get the current format of the Stream Output descriptor object.
		*/
		const char * STDCALL get_current_format();

		/**
		 * Get the formats offset of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_formats_offset();

		/**
		 * Get the number of formats of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_number_of_formats();

		/**
		 * Get the backup talker entity id 0 of the Stream Output descriptor object.
		 */
		uint64_t STDCALL get_backup_talker_entity_id_0();

		/**
		 * Get the backup talker unique 0 of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_backup_talker_unique_0();

		/**
		 * Get the backup talker entity id 1 of the Stream Output descriptor object.
		 */
		uint64_t STDCALL get_backup_talker_entity_id_1();

		/**
		 * Get the backup talker unique 1 of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_backup_talker_unique_1();

		/**
		 * Get the backup talker entity id 2 of the Stream Output descriptor object.
		 */
		uint64_t STDCALL get_backup_talker_entity_id_2();

		/**
		 * Get the backup talker unique 2 of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_backup_talker_unique_2();

		/**
		 * Get the backedup talker entity id of the Stream Output descriptor object.
		 */
		uint64_t STDCALL get_backedup_talker_entity_id();

		/**
		 * Get the backedup talker unique of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_backedup_talker_unique();

		/**
		 * Get the AVB Interface index of the Stream Output descriptor object.
		 */
		uint16_t STDCALL get_avb_interface_index();

		/**
		 * Get the buffer length of the Stream Output descriptor object.
		 */
		uint32_t STDCALL get_buffer_length();

		/**
		 * Get the stream format of a stream after sending a SET_STREAM_FORMAT command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL set_stream_format_stream_format();

		/**
		 * Get the stream format of a stream after sending a GET_STREAM_FORMAT command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_format_stream_format();

		/**
		 * Get the stream info flags of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint32_t STDCALL get_stream_info_flags();

		/**
		 * Get the stream info stream format of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_info_stream_format();

		/**
		 * Get the stream info stream id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_info_stream_id();

		/**
		 * Get the stream info MSRP accumulated latency of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint32_t STDCALL get_stream_info_msrp_accumulated_latency();

		/**
		 * Get the stream dest mac of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint8_t * STDCALL get_stream_info_stream_dest_mac();

		/**
		 * Get the stream info MSRP failure code of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint8_t STDCALL get_stream_info_msrp_failure_code();

		/**
		 * Get the stream info MSRP failure bridge id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		uint64_t STDCALL get_stream_info_msrp_failure_bridge_id();

		/**
		 * Check if the Clock Sync Source flag is set.
		 */
		bool is_clock_sync_source_set();

		/**
		 * Send a SET_STREAM_FORMAT command with a notification id to change the format of a stream.
		 */
		int STDCALL send_set_stream_format_cmd(void *notification_id, uint64_t new_stream_format);

		/**
		 * Process a SET_STREAM_FORMAT response for the SET_STREAM_FORMAT command.
		 */
		int proc_set_stream_format_resp(void *notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

		/**
		 * Send a GET_STREAM_FORMAT command with a notification id to fetch the current format of a stream.
		 */
		int STDCALL send_get_stream_format_cmd(void *notification_id);

		/**
		 * Process a GET_STREAM_FORMAT response for the GET_STREAM_FORMAT command.
		 */
		int proc_get_stream_format_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

		/**
		 * Send a SET_STREAM_INFO command with a notification id to change the current values of the dynamic information of the stream.
		 */
		int STDCALL send_set_stream_info_cmd(void *notification_id, void *new_stream_info_field);

		/**
		 * Process a SET_STREAM_INFO response for the SET_STREAM_INFO command.
		 */
		int proc_set_stream_info_resp(void *notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

		/**
		 * Send a GET_STREAM_INFO command with a notification id to fetch the current information for a stream.
		 */
		int STDCALL send_get_stream_info_cmd(void *notification_id);

		/**
		 * Process a GET_STREAM_INFO response for the GET_STREAM_INFO command.
		 */
		int proc_get_stream_info_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

		/**
		 * Send a START_STREAMING command with a notification id to start streaming on a previously connected stream that was connected
		 * via ACMP or has previously been stopped with the STOP_STREAMING command.
		 */
		int STDCALL send_start_streaming_cmd(void *notification_id);

		/**
		 * Process a START_STREAMING response for the START_STREAMING command.
		 */
		int proc_start_streaming_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

		/**
		 * Send a STOP_STREAMING command with a notification id to stop a connected stream for streaming media.
		 */
		int STDCALL send_stop_streaming_cmd(void *notification_id);

		/**
		 * Process a START_STREAMING response for the START_STREAMING command.
		 */
		int proc_stop_streaming_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);
	};
}

#endif
