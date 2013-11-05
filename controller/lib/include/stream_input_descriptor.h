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
 * stream_input_descriptor.h
 *
 * Public Stream Input descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_STREAM_INPUT_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_STREAM_INPUT_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
	class stream_input_descriptor : public virtual descriptor_base
	{
	public:
		/**
		 * \return The name of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

		/**
		 * \return The localized description of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

		/**
		 * \return The Clock Domain index of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_domain_index() = 0;

		/**
		 * \return The stream flags of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_stream_flags() = 0;

		/**
		 * \return True if the stream can be used as a clock synchronization source.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_clock_sync_source() = 0;

		/**
		 * \return True if the stream supports streaming at Class A.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_class_a() = 0;

		/**
		 * \return True if the stream supports streaming at Class B.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_class_b() = 0;

		/**
		 * \return True if the stream supports streaming with encrypted PDUs.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_supports_encrypted() = 0;

		/**
		 * \return True if the backup_talker_entity_id_0 and backup_talker_unique_id_0 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_primary_backup_supported() = 0;

		/**
		 * \return True if the backup_talker_entity_id_0 and backup_talker_unique_id_0 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_primary_backup_valid() = 0;

		/**
		 * \return True if the backup_talker_entity_id_1 and backup_talker_unique_id_1 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_secondary_backup_supported() = 0;

		/**
		 * \return True if the backup_talker_entity_id_1 and backup_talker_unique_id_1 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_secondary_backup_valid() = 0;

		/**
		 * \return True if the backup_talker_entity_id_2 and backup_talker_unique_id_2 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_tertiary_backup_supported() = 0;

		/**
		 * \return True if the backup_talker_entity_id_2 and backup_talker_unique_id_2 fields of
		 *	   the Stream Input descriptor object are supported.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_flags_tertiary_back_up_valid() = 0;

		/**
		* \return The current format of the Stream Input descriptor object.
		*/
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL get_current_format() = 0;

		/**
		 * \return The formats offset of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_formats_offset() = 0;

		/**
		 * \return The number of formats of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_formats() = 0;

		/**
		 * \return The backup talker entity id 0 of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backup_talker_entity_id_0() = 0;

		/**
		 * \return The backup talker unique 0 of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backup_talker_unique_0() = 0;

		/**
		 * \return The backup talker entity id 1 of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backup_talker_entity_id_1() = 0;

		/**
		 * \return The backup talker unique 1 of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backup_talker_unique_1() = 0;

		/**
		 * \return The backup talker entity id 2 of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backup_talker_entity_id_2() = 0;

		/**
		 * \return The backup talker unique 2 of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backup_talker_unique_2() = 0;

		/**
		 * \return The backedup talker entity id of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_backedup_talker_entity_id() = 0;

		/**
		 * \return The backedup talker unique of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_backedup_talker_unique() = 0;

		/**
		 * \return The AVB Interface index of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_avb_interface_index() = 0;

		/**
		 * \return The buffer length of the Stream Input descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_buffer_length() = 0;

		/**
		 * \return The stream format of a stream after sending a SET_STREAM_FORMAT command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL set_stream_format_stream_format() = 0;

		/**
		 * \return The stream format of a stream after sending a GET_STREAM_FORMAT command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_format_stream_format() = 0;

		/**
		 * \return The stream info flags of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_flags() = 0;

		/**
		 * \return The stream info stream format of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_format() = 0;

		/**
		 * \return The stream info stream id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_id() = 0;

		/**
		 * \return The stream info MSRP accumulated latency of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_msrp_accumulated_latency() = 0;

		/**
		 * \return The stream info stream destination MAC of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_stream_info_stream_dest_mac() = 0;

		/**
		 * \return The stream info MSRP failure code of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_stream_info_msrp_failure_code() = 0;

		/**
		 * \return The stream info MSRP failure bridge id of a stream after sending a GET_STREAM_info command and
		 * receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_msrp_failure_bridge_id() = 0;

		/**
		 * Send a SET_STREAM_FORMAT command with a notification id to change the format of a stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_stream_format_cmd(void *notification_id, uint64_t new_stream_format) = 0;

		/**
		 * Send a GET_STREAM_FORMAT command with a notification id to fetch the current format of a stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_stream_format_cmd(void *notification_id) = 0;

		/**
		 * Send a SET_STREAM_INFO command with a notification id to change the current values of the dynamic information of the stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_stream_info_cmd(void *notification_id, void *new_stream_info_field) = 0;

		/**
		 * Send a GET_STREAM_INFO command with a notification id to fetch the current information for a stream.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_stream_info_cmd(void *notification_id) = 0;

		/**
		 * Send a START_STREAMING command with a notification id to start streaming on a previously connected stream that was connected
		 * via ACMP.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_start_streaming_cmd(void *notification_id) = 0;

		/**
		 * Send a STOP_STREAMING command with a notification id to stop a connected stream for streaming media.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_stop_streaming_cmd(void *notification_id) = 0;
	};
}

#endif
