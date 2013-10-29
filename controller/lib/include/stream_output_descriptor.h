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
 * stream_output_descriptor.h
 *
 * Public Stream Output descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_STREAM_OUTPUT_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_STREAM_OUTPUT_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
	class stream_output_descriptor : public virtual descriptor_base
	{
	public:
		/**
		 * Get the name of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

		/**
		 * Get the localized description of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

		/**
		 * Get the Clock Domain index of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_domain_index() = 0;

		/**
		 * Get the stream flags of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_stream_flags() = 0;

		/**
		* Get the current format of the Stream Output descriptor object.
		*/
		AVDECC_CONTROLLER_LIB32_API virtual char * STDCALL get_current_format() = 0;

		/**
		 * Get the formats offset of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_formats_offset() = 0;

		/**
		 * Get the number of formats of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_formats() = 0;

		/**
		 * Get the backup talker entity id 0 of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backup_talker_entity_id_0() = 0;

		/**
		 * Get the backup talker unique_0 of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backup_talker_unique_0() = 0;

		/**
		 * Get the backup talker entity id 1 of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backup_talker_entity_id_1() = 0;

		/**
		 * Get the backup talker unique_1 of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backup_talker_unique_1() = 0;

		/**
		 * Get the backup talker entity id 2 of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backup_talker_entity_id_2() = 0;

		/**
		 * Get the backup talker unique 2 of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backup_talker_unique_2() = 0;

		/**
		 * Get the backedup talker entity id of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backedup_talker_entity_id() = 0;

		/**
		 * Get the backedup talker unique of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backedup_talker_unique() = 0;

		/**
		 * Get the AVB Interface index of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_avb_interface_index() = 0;

		/**
		 * Get the buffer length of the Stream Output descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_buffer_length() = 0;

		/**
		 * Get the stream format of a stream after sending a GET_STREAM_FORMAT command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_format_stream_format() = 0;

		/**
		 * Get the stream info flags of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_flags() = 0;

		/**
		 * Get the stream info stream format of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_format() = 0;

		/**
		 * Get the stream info stream id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_id() = 0;

		/**
		 * Get the stream info MSRP accumulated latency of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_msrp_accumulated_latency() = 0;

		/**
		 * Get the stream info stream destination MAC of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_stream_info_stream_dest_mac() = 0;

		/**
		 * Get the stream info MSRP failure code of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_stream_info_msrp_failure_code() = 0;

		/**
		 * Get the stream info MSRP failure bridge id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_msrp_failure_bridge_id() = 0;

		/**
		 * Send a SET_STREAM_FORMAT command with or without notifying id based on the notifying flag to change the format of a stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_stream_format_cmd(void *notification_id, uint16_t desc_index, uint64_t new_stream_format) = 0;

		/**
		 * Send a GET_STREAM_FORMAT command with or without a notifying id based on the notifying flag to fetch the current format of a stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_stream_format_cmd(void *notification_id, uint16_t desc_index) = 0;

		/**
		 * Send a SET_STREAM_INFO command with or without a notifying id based on the notifying flag to change the current values
		 * of the dynamic information of the stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_stream_info_cmd(void *notification_id, uint16_t desc_index, void *new_stream_info_field) = 0;

		/**
		 * Send a GET_STREAM_INFO command with or without a notifying id based on the notifying flag to fetch the current information for a stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_stream_info_cmd(void *notification_id, uint16_t desc_index) = 0;
	};
}

#endif