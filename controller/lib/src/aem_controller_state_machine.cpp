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
 * aem_controller_state_machine.cpp
 *
 * AVDECC Entity Model Controller State Machine implementation
 */

#include <vector>
#include "net_interface_imp.h"
#include "aem_string.h"
#include "enumeration.h"
#include "notification.h"
#include "log.h"
#include "util.h"
#include "adp.h"
#include "aecp.h"
#include "aem_controller_state_machine.h"

namespace avdecc_lib
{
	aem_controller_state_machine *aem_controller_state_machine_ref = new aem_controller_state_machine(); // To have one Controller State Machine for all end stations

	uint16_t aem_controller_state_machine::aecp_seq_id = 0x0;

	aem_controller_state_machine::aem_controller_state_machine()
	{
		controller_state_machine_vars.rcvd_normal_resp = false;
		controller_state_machine_vars.rcvd_unsolicited_resp = false;
		controller_state_machine_vars.do_cmd = false;
		controller_state_machine_vars.do_terminate = false;
	}

	aem_controller_state_machine::~aem_controller_state_machine() {}

	void aem_controller_state_machine::tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame)
	{
		int inflight_index;
		bool is_inflight;
		int send_frame_returned;
		struct aem_controller_state_machine_inflight_cmd inflight_cmd;
		inflight_cmd.seq_id = jdksavdecc_uint16_get(ether_frame->payload, aecp::SEQ_ID_POS);
		inflight_cmd.notification_id = notification_id;
		inflight_cmd.notification_flag = notification_flag;
		convert_eui48_to_uint64(ether_frame->payload, inflight_cmd.dest_addr);

		is_inflight = find_inflight_cmd_by_seq_id(inflight_cmd.seq_id, &inflight_index); // Check if the command is inflight

		inflight_cmd.avdecc_lib_timer_ref = new timer(); // Create a timer object
		inflight_cmd.avdecc_lib_timer_ref->start(AVDECC_MSG_TIMEOUT); // Start the timer

		send_frame_returned = net_interface_ref->send_frame(ether_frame->payload, ether_frame->length);

		if(send_frame_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "netif_send_frame error");
			assert(send_frame_returned >= 0);
		}

		callback(notification_id, notification_flag, ether_frame->payload);

		if(is_inflight)
		{
			controller_state_machine_vars.inflight_cmds_vector.at(inflight_index).retried = true;
		}
		else
		{
			aecp_seq_id++;
			inflight_cmd.retried = false;
			memcpy(&inflight_cmd.inflight_cmd_frame, ether_frame, sizeof(struct jdksavdecc_frame));
			controller_state_machine_vars.inflight_cmds_vector.push_back(inflight_cmd);  // Add the inflight command to the inflight command vector
		}
	}

	//int aem_controller_state_machine::process_unsolicited(struct jdksavdecc_frame *ether_frame)
	//{
	//	return 0;
	//}

	int aem_controller_state_machine::process_resp(void *&notification_id, uint32_t &notification_flag, struct jdksavdecc_frame *ether_frame)
	{
		uint16_t seq_id = jdksavdecc_uint16_get(ether_frame->payload, aecp::SEQ_ID_POS);
		int inflight_index = 0;

		if(find_inflight_cmd_by_seq_id(seq_id, &inflight_index))
		{
			notification_id = controller_state_machine_vars.inflight_cmds_vector.at(inflight_index).notification_id;
			notification_flag = controller_state_machine_vars.inflight_cmds_vector.at(inflight_index).notification_flag;
			callback(notification_id, notification_flag, ether_frame->payload);
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Command Success");
			remove_inflight_cmd(inflight_index);

			return 1;
		}

		return -1;
	}

	void aem_controller_state_machine::timeout(uint32_t inflight_cmds_vector_index)
	{
		if(controller_state_machine_vars.inflight_cmds_vector.at(inflight_cmds_vector_index).retried)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Command timeout");
			remove_inflight_cmd(inflight_cmds_vector_index);
			printf("\n>");
		}
		else
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG,
			                             "Resend the command with sequence id = %d",
			                             controller_state_machine_vars.inflight_cmds_vector.at(inflight_cmds_vector_index).seq_id);

			tx_cmd(controller_state_machine_vars.inflight_cmds_vector.at(inflight_cmds_vector_index).notification_id,
			       controller_state_machine_vars.inflight_cmds_vector.at(inflight_cmds_vector_index).notification_flag,
			       &controller_state_machine_vars.inflight_cmds_vector.at(inflight_cmds_vector_index).inflight_cmd_frame);
		}
	}

	void aem_controller_state_machine::aem_controller_state_waiting(void *&notification_id, uint32_t &notification_flag, struct jdksavdecc_frame *ether_frame)
	{
		uint64_t my_entity_id = 0;
		uint64_t dest_addr_resp = 0;

		if(net_interface_ref != NULL)
		{
			my_entity_id = net_interface_ref->get_mac();
			convert_eui48_to_uint64(jdksavdecc_eui64_get(ether_frame->payload, 0).value, dest_addr_resp);
		}

		if(controller_state_machine_vars.do_cmd)
		{
			jdksavdecc_uint16_set(aecp_seq_id, ether_frame->payload, aecp::SEQ_ID_POS);
			aem_controller_state_send_cmd(notification_id, notification_flag, ether_frame);
		}
		else if(controller_state_machine_vars.rcvd_unsolicited_resp && dest_addr_resp == my_entity_id)
		{
			//aem_controller_state_rcvd_unsolicited(ether_frame);
		}
		else if(controller_state_machine_vars.rcvd_normal_resp) // && dest_addr_response == my_entity_id)
		{
			aem_controller_state_rcvd_resp(notification_id, notification_flag, ether_frame);
		}
		else if((controller_state_machine_vars.rcvd_unsolicited_resp || controller_state_machine_vars.rcvd_normal_resp)
		        && dest_addr_resp != my_entity_id)
		{
		}
		else {}
	}

	void aem_controller_state_machine::aem_controller_state_send_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame)
	{
		tx_cmd(notification_id, notification_flag, ether_frame);
		controller_state_machine_vars.do_cmd = false;
	}

	//void aem_controller_state_machine::aem_controller_state_rcvd_unsolicited(struct jdksavdecc_frame *ether_frame)
	//{
	//	process_unsolicited(ether_frame);
	//	controller_state_machine_vars.rcvd_unsolicited_resp = false;
	//}

	void aem_controller_state_machine::aem_controller_state_rcvd_resp(void *&notification_id, uint32_t &notification_flag, struct jdksavdecc_frame *ether_frame)
	{
		process_resp(notification_id, notification_flag, ether_frame);
		controller_state_machine_vars.rcvd_normal_resp = false;
	}

	void aem_controller_state_machine::aem_controller_tick()
	{
		for(uint32_t index_i = 0; index_i < controller_state_machine_vars.inflight_cmds_vector.size(); index_i++)
		{
			if(controller_state_machine_vars.inflight_cmds_vector.at(index_i).avdecc_lib_timer_ref->timeout())
			{
				timeout(index_i);
			}
		}
	}

	int aem_controller_state_machine::update_inflight_for_rcvd_resp(void *&notification_id, uint32_t &notification_flag, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *ether_frame)
	{
		if(msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE && u_field == true)
		{
			controller_state_machine_vars.rcvd_unsolicited_resp = true;
			aem_controller_state_waiting(notification_id, notification_flag, ether_frame);
		}
		else if(msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE && u_field == false)
		{
			controller_state_machine_vars.rcvd_normal_resp = true;
			aem_controller_state_waiting(notification_id, notification_flag, ether_frame);
		}
		else
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "Invalid message type");
			return -1;
		}

		return 0;
	}

	int aem_controller_state_machine::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
	{
		uint8_t msg_type = jdksavdecc_uint8_get(frame, aecp::MSG_TYPE_POS);
		uint16_t cmd_type = jdksavdecc_uint16_get(frame, aecp::CMD_TYPE_POS);
		uint16_t desc_type = 0;
		uint16_t desc_index = 0;

		switch(cmd_type)
		{
			case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_COMMAND_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_COMMAND_OFFSET_DESCRIPTOR_INDEX);
				break;

			case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
				break;

			case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
				break;

			case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
				break;

			case JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_INDEX);
				break;

			case JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_INDEX);
				break;

			case JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_INDEX);
				break;

			case JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_INDEX);
				break;

			case JDKSAVDECC_AEM_COMMAND_SET_NAME:
				break;

			case JDKSAVDECC_AEM_COMMAND_GET_NAME:
				break;

			case JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE:
				break;

			case JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE_RESPONSE_OFFSET_DESCRIPTOR_INDEX);
				break;

			default:
				avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "NO_MATCH_FOUND for %s", avdecc_lib::aem_string::cmd_value_to_name(cmd_type));
				break;
		}

		if((notification_flag == CMD_WITH_NOTIFICATION) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND))
		{
			avdecc_lib::notification_ref->notifying(avdecc_lib::COMMAND_SENT,
			                                        jdksavdecc_uint64_get(frame, aecp::TARGET_GUID_POS),
			                                        cmd_type,
			                                        desc_type,
			                                        desc_index,
			                                        notification_id);
		}
		else if((notification_flag == CMD_WITH_NOTIFICATION) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE))
		{
			avdecc_lib::notification_ref->notifying(avdecc_lib::RESPONSE_RECEIVED,
			                                        jdksavdecc_uint64_get(frame, aecp::TARGET_GUID_POS),
			                                        cmd_type,
			                                        desc_type,
			                                        desc_index,
			                                        notification_id);
		}
		else if((notification_flag == CMD_WITHOUT_NOTIFICATION) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND))
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG,
			                             "COMMAND_SENT, 0x%llx, %s, %s, %d, %d",
			                             jdksavdecc_uint64_get(frame, aecp::TARGET_GUID_POS),
			                             avdecc_lib::aem_string::cmd_value_to_name(cmd_type),
			                             avdecc_lib::aem_string::desc_value_to_name(desc_type),
			                             desc_index,
			                             jdksavdecc_uint16_get(frame, aecp::SEQ_ID_POS));
		}
		else if((notification_flag == CMD_WITHOUT_NOTIFICATION) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE))
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG,
			                             "RESPONSE_RECEIVED, 0x%llx, %s, %s, %d, %d",
			                             jdksavdecc_uint64_get(frame, aecp::TARGET_GUID_POS),
			                             avdecc_lib::aem_string::cmd_value_to_name(cmd_type),
			                             avdecc_lib::aem_string::desc_value_to_name(desc_type),
			                             desc_index,
			                             jdksavdecc_uint16_get(frame, aecp::SEQ_ID_POS));
		}

		return 0;
	}

	bool aem_controller_state_machine::find_inflight_cmd_by_seq_id(uint16_t seq_id, int *inflight_index)
	{
		for(uint32_t index_i = 0; index_i < controller_state_machine_vars.inflight_cmds_vector.size(); index_i++)
		{
			if((controller_state_machine_vars.inflight_cmds_vector.at(index_i).seq_id == seq_id))
			{
				*inflight_index = index_i;
				return true;
			}
		}

		return false;
	}

	bool aem_controller_state_machine::find_inflight_cmd_by_notification_id(void *notification_id)
	{
		for(uint32_t index_i = 0; index_i < controller_state_machine_vars.inflight_cmds_vector.size(); index_i++)
		{
			if((controller_state_machine_vars.inflight_cmds_vector.at(index_i).notification_id == notification_id))
			{
				return true;
			}
		}

		return false;
	}

	int aem_controller_state_machine::remove_inflight_cmd(uint32_t inflight_cmd_index)
	{
		delete controller_state_machine_vars.inflight_cmds_vector.at(inflight_cmd_index).avdecc_lib_timer_ref;
		controller_state_machine_vars.inflight_cmds_vector.erase(controller_state_machine_vars.inflight_cmds_vector.begin() + inflight_cmd_index);

		return 0;
	}
}