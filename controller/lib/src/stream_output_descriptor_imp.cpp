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
 * stream_output_descriptor_imp.cpp
 *
 * Stream Output descriptor implementation
 */

#include <vector>
#include "util_imp.h"
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "aecp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "aem_controller_state_machine.h"
#include "stream_output_descriptor_imp.h"

namespace avdecc_lib
{
	stream_output_descriptor_imp::stream_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
	{
		stream_output_desc_read_returned = jdksavdecc_descriptor_stream_read(&stream_output_desc, frame, pos, frame_len);

		if(stream_output_desc_read_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "desc_stream_output_read error");
			assert(stream_output_desc_read_returned >= 0);
		}

		memset(&stream_flags, 0, sizeof(struct stream_output_desc_stream_flags));
		memset(&aem_cmd_set_stream_format_resp, 0, sizeof(struct jdksavdecc_aem_command_set_stream_format_response));
		memset(&aem_cmd_get_stream_format_resp, 0, sizeof(struct jdksavdecc_aem_command_get_stream_format_response));
		memset(&aem_cmd_set_stream_info_resp, 0, sizeof(struct jdksavdecc_aem_command_set_stream_info_response));
		memset(&aem_cmd_get_stream_info_resp, 0, sizeof(struct jdksavdecc_aem_command_get_stream_info_response));

		stream_flags_init();
	}

	stream_output_descriptor_imp::~stream_output_descriptor_imp() {}

	void stream_output_descriptor_imp::stream_flags_init()
	{
		stream_flags.clock_sync_source = stream_output_desc.stream_flags >> 0 & 0x01;;
		stream_flags.class_a = stream_output_desc.stream_flags >> 1 & 0x01;
		stream_flags.class_b = stream_output_desc.stream_flags >> 2 & 0x01;
		stream_flags.supports_encrypted = stream_output_desc.stream_flags >> 3 & 0x01;
		stream_flags.primary_backup_supported = stream_output_desc.stream_flags >> 4 & 0x01;
		stream_flags.primary_backup_valid = stream_output_desc.stream_flags >> 5 & 0x01;
		stream_flags.secondary_backup_supported = stream_output_desc.stream_flags >> 6 & 0x01;
		stream_flags.secondary_backup_valid = stream_output_desc.stream_flags >> 7 & 0x01;
		stream_flags.tertiary_backup_supported = stream_output_desc.stream_flags >> 8 & 0x01;
		stream_flags.tertiary_back_up_valid = stream_output_desc.stream_flags >> 9 & 0x01;
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_descriptor_type() const
	{
		assert(stream_output_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT);
		return stream_output_desc.descriptor_type;
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_descriptor_index() const
	{
		return stream_output_desc.descriptor_index;
	}

	uint8_t * STDCALL stream_output_descriptor_imp::get_object_name()
	{
		return stream_output_desc.object_name.value;
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_localized_description ()
	{
		return stream_output_desc.localized_description;
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_clock_domain_index()
	{
		return stream_output_desc.clock_domain_index;
	}

	uint16_t stream_output_descriptor_imp::get_stream_flags()
	{
		return stream_output_desc.stream_flags;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_clock_sync_source()
	{
		return stream_flags.clock_sync_source;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_class_a()
	{
		return stream_flags.class_a;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_class_b()
	{
		return stream_flags.class_b;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_supports_encrypted()
	{
		return stream_flags.supports_encrypted;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_primary_backup_supported()
	{
		return stream_flags.primary_backup_supported;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_primary_backup_valid()
	{
		return stream_flags.primary_backup_valid;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_secondary_backup_supported()
	{
		return stream_flags.secondary_backup_supported;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_secondary_backup_valid()
	{
		return stream_flags.secondary_backup_valid;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_tertiary_backup_supported()
	{
		return stream_flags.tertiary_backup_supported;
	}

	bool STDCALL stream_output_descriptor_imp::get_stream_flags_tertiary_back_up_valid()
	{
		return stream_flags.tertiary_back_up_valid;
	}

	const char * stream_output_descriptor_imp::get_current_format()
	{
		uint64_t current_format = jdksavdecc_uint64_get(&stream_output_desc.current_format, 0);

		return utility->ieee1722_format_value_to_name(current_format);
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_formats_offset()
	{
		assert(stream_output_desc.formats_offset == 132);
		return stream_output_desc.formats_offset;
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_number_of_formats()
	{
		assert(stream_output_desc.number_of_formats <= 47);
		return stream_output_desc.number_of_formats;
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_backup_talker_entity_id_0()
	{
		return jdksavdecc_uint64_get(&stream_output_desc.backup_talker_entity_id_0, 0);
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_backup_talker_unique_0()
	{
		return stream_output_desc.backup_talker_unique_0;
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_backup_talker_entity_id_1()
	{
		return jdksavdecc_uint64_get(&stream_output_desc.backup_talker_entity_id_1, 0);
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_backup_talker_unique_1()
	{
		return stream_output_desc.backup_talker_unique_1;
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_backup_talker_entity_id_2()
	{
		return jdksavdecc_uint64_get(&stream_output_desc.backup_talker_entity_id_2, 0);
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_backup_talker_unique_2()
	{
		return stream_output_desc.backup_talker_unique_2;
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_backedup_talker_entity_id()
	{
		return jdksavdecc_uint64_get(&stream_output_desc.backedup_talker_entity_id, 0);
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_backedup_talker_unique()
	{
		return stream_output_desc.backedup_talker_unique;
	}

	uint16_t STDCALL stream_output_descriptor_imp::get_avb_interface_index()
	{
		return stream_output_desc.avb_interface_index;
	}

	uint32_t STDCALL stream_output_descriptor_imp::get_buffer_length()
	{
		return stream_output_desc.buffer_length;
	}

	uint64_t STDCALL stream_output_descriptor_imp::set_stream_format_stream_format()
	{
		return jdksavdecc_uint64_get(&aem_cmd_set_stream_format_resp.stream_format, 0);
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_stream_format_stream_format()
	{
		return jdksavdecc_uint64_get(&aem_cmd_get_stream_format_resp.stream_format, 0);
	}

	uint32_t STDCALL stream_output_descriptor_imp::get_stream_info_flags()
	{
		return aem_cmd_get_stream_info_resp.aem_stream_info_flags;
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_stream_info_stream_format()
	{
		return jdksavdecc_uint64_get(&aem_cmd_get_stream_info_resp.stream_format, 0);
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_stream_info_stream_id()
	{
		return jdksavdecc_uint64_get(&aem_cmd_get_stream_info_resp.stream_id, 0);
	}

	uint32_t STDCALL stream_output_descriptor_imp::get_stream_info_msrp_accumulated_latency()
	{
		return aem_cmd_get_stream_info_resp.msrp_accumulated_latency;
	}

	uint8_t * STDCALL stream_output_descriptor_imp::get_stream_info_stream_dest_mac()
	{
		return aem_cmd_get_stream_info_resp.stream_dest_mac.value;
	}

	uint8_t STDCALL stream_output_descriptor_imp::get_stream_info_msrp_failure_code()
	{
		return aem_cmd_get_stream_info_resp.msrp_failure_code;
	}

	uint64_t STDCALL stream_output_descriptor_imp::get_stream_info_msrp_failure_bridge_id()
	{
		return jdksavdecc_uint64_get(&aem_cmd_get_stream_info_resp.msrp_failure_bridge_id, 0);
	}

	bool stream_output_descriptor_imp::is_clock_sync_source_set()
	{
		return stream_flags.clock_sync_source;
	}

	int STDCALL stream_output_descriptor_imp::send_set_stream_format_cmd(void *notification_id, uint64_t new_stream_format)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_set_stream_format aem_cmd_set_stream_format;
		int aem_cmd_set_stream_format_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/******************************************** AECP Common Data *********************************************/
		aem_cmd_set_stream_format.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
		// Fill aem_cmd_set_stream_format.sequence_id in AEM Controller State Machine
		aem_cmd_set_stream_format.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT;

		/**************************************** AECP Message Specific Data *************************************/
		aem_cmd_set_stream_format.descriptor_type = get_descriptor_type();
		aem_cmd_set_stream_format.descriptor_index = get_descriptor_index();
		jdksavdecc_uint64_write(new_stream_format, &aem_cmd_set_stream_format.stream_format, 0, sizeof(uint64_t));

		/******************************** Fill frame payload with AECP data and send the frame ***************************/
		aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
		aem_cmd_set_stream_format_returned = jdksavdecc_aem_command_set_stream_format_write(&aem_cmd_set_stream_format,
		                                                                                    ether_frame->payload,
		                                                                                    aecp::CMD_POS,
		                                                                                    sizeof(ether_frame->payload));

		if(aem_cmd_set_stream_format_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_stream_format_write error\n");
			assert(aem_cmd_set_stream_format_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
		system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int stream_output_descriptor_imp::proc_set_stream_format_resp(void *notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		int aem_cmd_set_stream_format_resp_returned;
		uint32_t msg_type;
		bool u_field;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, frame_len);

		aem_cmd_set_stream_format_resp_returned = jdksavdecc_aem_command_set_stream_format_response_read(&aem_cmd_set_stream_format_resp,
		                                                                                                 frame,
		                                                                                                 aecp::CMD_POS,
		                                                                                                 frame_len);

		if(aem_cmd_set_stream_format_resp_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_stream_format_resp_read error\n");
			assert(aem_cmd_set_stream_format_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_get_stream_format_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_get_stream_format_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_get_stream_format_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}

	int STDCALL stream_output_descriptor_imp::send_get_stream_format_cmd(void *notification_id)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_get_stream_format aem_cmd_get_stream_format;
		int aem_cmd_get_stream_format_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/******************************************** AECP Common Data *********************************************/
		aem_cmd_get_stream_format.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
		// Fill aem_cmd_get_stream_format.sequence_id in AEM Controller State Machine
		aem_cmd_get_stream_format.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT;

		/******************* AECP Message Specific Data ********************/
		aem_cmd_get_stream_format.descriptor_type = get_descriptor_type();
		aem_cmd_get_stream_format.descriptor_index = get_descriptor_index();

		/******************************** Fill frame payload with AECP data and send the frame ***************************/
		aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
		aem_cmd_get_stream_format_returned = jdksavdecc_aem_command_get_stream_format_write(&aem_cmd_get_stream_format,
		                                                                                    ether_frame->payload,
		                                                                                    aecp::CMD_POS,
		                                                                                    sizeof(ether_frame->payload));

		if(aem_cmd_get_stream_format_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_format_write error\n");
			assert(aem_cmd_get_stream_format_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
		system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int stream_output_descriptor_imp::proc_get_stream_format_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		int aem_cmd_get_stream_format_resp_returned;
		uint32_t msg_type;
		bool u_field;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, frame_len);

		aem_cmd_get_stream_format_resp_returned = jdksavdecc_aem_command_get_stream_format_response_read(&aem_cmd_get_stream_format_resp,
		                                                                                                 frame,
		                                                                                                 aecp::CMD_POS,
		                                                                                                 frame_len);

		if(aem_cmd_get_stream_format_resp_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_format_resp_read error\n");
			assert(aem_cmd_get_stream_format_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_get_stream_format_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_get_stream_format_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_get_stream_format_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}

	int STDCALL stream_output_descriptor_imp::send_set_stream_info_cmd(void *notification_id, void *new_stream_info_field)
	{
		log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_STREAM_INFO command.");

		return 0;
	}

	int stream_output_descriptor_imp::proc_set_stream_info_resp(void *notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
	{
		log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_STREAM_INFO response.");

		return 0;
	}

	int STDCALL stream_output_descriptor_imp::send_get_stream_info_cmd(void *notification_id)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_get_stream_info aem_cmd_get_stream_info;
		int aem_cmd_get_stream_info_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/******************************************** AECP Common Data *******************************************/
		aem_cmd_get_stream_info.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
		// Fill aem_cmd_get_stream_info.sequence_id in AEM Controller State Machine
		aem_cmd_get_stream_info.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO;

		/******************** AECP Message Specific Data *****************/
		aem_cmd_get_stream_info.descriptor_type = get_descriptor_type();
		aem_cmd_get_stream_info.descriptor_index = get_descriptor_index();

		/************************** Fill frame payload with AECP data and send the frame *****************************/
		aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
		aem_cmd_get_stream_info_returned = jdksavdecc_aem_command_get_stream_info_write(&aem_cmd_get_stream_info,
		                                                                                ether_frame->payload,
		                                                                                aecp::CMD_POS,
		                                                                                sizeof(ether_frame->payload));

		if(aem_cmd_get_stream_info_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_info_write error\n");
			assert(aem_cmd_get_stream_info_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
		system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int stream_output_descriptor_imp::proc_get_stream_info_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_get_stream_info_response aem_cmd_get_stream_info_resp;
		int aem_cmd_get_stream_info_resp_returned;
		uint32_t msg_type;
		bool u_field;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, frame_len);
		aem_cmd_get_stream_info_resp_returned = jdksavdecc_aem_command_get_stream_info_response_read(&aem_cmd_get_stream_info_resp,
		                                                                                             frame,
		                                                                                             aecp::CMD_POS,
		                                                                                             frame_len);

		if(aem_cmd_get_stream_info_resp_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_info_resp_read error");
			assert(aem_cmd_get_stream_info_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_get_stream_info_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}

	int STDCALL stream_output_descriptor_imp::send_start_streaming_cmd(void *notification_id)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_start_streaming aem_cmd_start_streaming;
		int aem_cmd_start_streaming_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/******************************************** AECP Common Data *******************************************/
		aem_cmd_start_streaming.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
		// Fill aem_cmd_start_streaming.sequence_id in AEM Controller State Machine
		aem_cmd_start_streaming.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO;

		/******************** AECP Message Specific Data *****************/
		aem_cmd_start_streaming.descriptor_type = get_descriptor_type();
		aem_cmd_start_streaming.descriptor_index = get_descriptor_index();

		/************************** Fill frame payload with AECP data and send the frame *****************************/
		aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
		aem_cmd_start_streaming_returned = jdksavdecc_aem_command_start_streaming_write(&aem_cmd_start_streaming,
		                                                                                ether_frame->payload,
		                                                                                aecp::CMD_POS,
		                                                                                sizeof(ether_frame->payload));

		if(aem_cmd_start_streaming_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_start_streaming_write error\n");
			assert(aem_cmd_start_streaming_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
		system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int stream_output_descriptor_imp::proc_start_streaming_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_start_streaming_response aem_cmd_start_streaming_resp;
		int aem_cmd_start_streaming_resp_returned;
		uint32_t msg_type;
		bool u_field;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, frame_len);
		aem_cmd_start_streaming_resp_returned = jdksavdecc_aem_command_start_streaming_response_read(&aem_cmd_start_streaming_resp,
		                                                                                             frame,
		                                                                                             aecp::CMD_POS,
		                                                                                             frame_len);

		if(aem_cmd_start_streaming_resp_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_start_streaming_resp_read error");
			assert(aem_cmd_start_streaming_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_get_stream_info_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}

	int STDCALL stream_output_descriptor_imp::send_stop_streaming_cmd(void *notification_id)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_stop_streaming aem_cmd_stop_streaming;
		int aem_cmd_stop_streaming_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/******************************************* AECP Common Data *******************************************/
		aem_cmd_stop_streaming.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
		// Fill aem_cmd_stop_streaming.sequence_id in AEM Controller State Machine
		aem_cmd_stop_streaming.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO;

		/******************** AECP Message Specific Data ****************/
		aem_cmd_stop_streaming.descriptor_type = get_descriptor_type();
		aem_cmd_stop_streaming.descriptor_index = get_descriptor_index();

		/************************** Fill frame payload with AECP data and send the frame ***************************/
		aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
		aem_cmd_stop_streaming_returned = jdksavdecc_aem_command_stop_streaming_write(&aem_cmd_stop_streaming,
		                                                                              ether_frame->payload,
		                                                                              aecp::CMD_POS,
		                                                                              sizeof(ether_frame->payload));

		if(aem_cmd_stop_streaming_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_stop_streaming_write error\n");
			assert(aem_cmd_stop_streaming_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
		system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int stream_output_descriptor_imp::proc_stop_streaming_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_stop_streaming_response aem_cmd_stop_streaming_resp;
		int aem_cmd_stop_streaming_resp_returned;
		uint32_t msg_type;
		bool u_field;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, frame_len);
		aem_cmd_stop_streaming_resp_returned = jdksavdecc_aem_command_stop_streaming_response_read(&aem_cmd_stop_streaming_resp,
		                                                                                           frame,
		                                                                                           aecp::CMD_POS,
		                                                                                           frame_len);

		if(aem_cmd_stop_streaming_resp_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_stop_streaming_resp_read error");
			assert(aem_cmd_stop_streaming_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_get_stream_info_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}
}
