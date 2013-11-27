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

#include <algorithm>
#include <vector>
#include "jdksavdecc_aem_command.h"
#include "net_interface_imp.h"
#include "util_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "inflight.h"
#include "aem_controller_state_machine.h"

namespace avdecc_lib
{
    aem_controller_state_machine *aem_controller_state_machine_ref = new aem_controller_state_machine(); // To have one Controller State Machine for all end stations

    aem_controller_state_machine::aem_controller_state_machine()
    {
        aecp_seq_id = 0x0;
    }

    aem_controller_state_machine::~aem_controller_state_machine() {}

    int aem_controller_state_machine::tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame, bool resend)
    {
        int send_frame_returned;

        if (!resend)
        {
            uint16_t current_seq_id = aecp_seq_id;

            jdksavdecc_aecpdu_common_set_sequence_id(aecp_seq_id++, ether_frame->payload, ETHER_HDR_SIZE);
            inflight in_flight = inflight(ether_frame,
                                          current_seq_id,
                                          notification_id,
                                          notification_flag,
                                          AVDECC_MSG_TIMEOUT_MS);
            in_flight.start_timer();
            inflight_cmds_vec.push_back(in_flight);
        }

        send_frame_returned = net_interface_ref->send_frame(ether_frame->payload, ether_frame->length);
        if(send_frame_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "netif_send_frame error");
            assert(send_frame_returned >= 0);
        }

        callback(notification_id, notification_flag, ether_frame->payload);

        std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds_vec.begin(), inflight_cmds_vec.end(), SeqIdComp(aecp_seq_id));

        if(j != inflight_cmds_vec.end()) // found?
        {
            j->start_timer();
        }

        return 0;
    }

    int aem_controller_state_machine::proc_unsolicited(void *&notification_id, struct jdksavdecc_frame *ether_frame)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "proc_unsolicited is not implemented.");

        return 0;
    }

    int aem_controller_state_machine::proc_resp(void *&notification_id, struct jdksavdecc_frame *ether_frame)
    {
        uint16_t seq_id = jdksavdecc_aecpdu_common_get_sequence_id(ether_frame->payload, ETHER_HDR_SIZE);
        int inflight_index = 0;
        uint32_t notification_flag = 0;

        std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds_vec.begin(), inflight_cmds_vec.end(), SeqIdComp(seq_id));

        if(j != inflight_cmds_vec.end()) // found?
        {
            notification_id = j->cmd_notification_id;
            notification_flag = j->notification_flag();
            callback(notification_id, notification_flag, ether_frame->payload);
            inflight_cmds_vec.erase(inflight_cmds_vec.begin() + inflight_index);
            return 1;
        }

        return -1;
    }

    int aem_controller_state_machine::state_send_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame)
    {
       return tx_cmd(notification_id, notification_flag, ether_frame, false);
    }

    int aem_controller_state_machine::state_rcvd_unsolicited(void *&notification_id, struct jdksavdecc_frame *ether_frame)
    {
       return proc_unsolicited(notification_id, ether_frame);
    }

    int aem_controller_state_machine::state_rcvd_resp(void *&notification_id, struct jdksavdecc_frame *ether_frame)
    {
       return proc_resp(notification_id, ether_frame);
    }

    void aem_controller_state_machine::state_timeout(uint32_t inflight_cmd_index)
    {
        bool is_retried = inflight_cmds_vec.at(inflight_cmd_index).retried();

        if(is_retried)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Command timeout");
            inflight_cmds_vec.erase(inflight_cmds_vec.begin() + inflight_cmd_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "Resend the command with sequence id = %d",
                                      inflight_cmds_vec.at(inflight_cmd_index).cmd_seq_id);

            tx_cmd(inflight_cmds_vec.at(inflight_cmd_index).cmd_notification_id,
                   inflight_cmds_vec.at(inflight_cmd_index).notification_flag(),
                   &inflight_cmds_vec.at(inflight_cmd_index).frame(),
                   true);
        }
    }

    void aem_controller_state_machine::tick()
    {
        for(uint32_t i = 0; i < inflight_cmds_vec.size(); i++)
        {
            if(inflight_cmds_vec.at(i).timeout())
            {
                state_timeout(i);
            }
        }
    }

    int aem_controller_state_machine::update_inflight_for_rcvd_resp(void *&notification_id, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *ether_frame)
    {
        if(msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE && u_field == false)
        {
            state_rcvd_resp(notification_id, ether_frame);
        }
        else if(msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE && u_field == true)
        {
            state_rcvd_unsolicited(notification_id, ether_frame);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid message type");
            return -1;
        }

        return 0;
    }

    int aem_controller_state_machine::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
    {
        uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);
        uint16_t cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame, ETHER_HDR_SIZE + JDKSAVDECC_COMMON_CONTROL_HEADER_LEN); //jdksavdecc_uint16_get(frame, aecp::CMD_TYPE_POS);
        uint16_t desc_type = 0;
        uint16_t desc_index = 0;

        switch(cmd_type)
        {
        case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
            desc_type = jdksavdecc_aem_command_acquire_entity_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_acquire_entity_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
            desc_type = jdksavdecc_aem_command_lock_entity_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_lock_entity_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
            break;

        case JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE:
            break;

        case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
            desc_type = jdksavdecc_aem_command_read_descriptor_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_read_descriptor_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT:
            desc_type = jdksavdecc_aem_command_set_stream_format_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_set_stream_format_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT:
            desc_type = jdksavdecc_aem_command_get_stream_format_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_get_stream_format_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO:
            desc_type = jdksavdecc_aem_command_set_stream_info_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_set_stream_info_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO:
            desc_type = jdksavdecc_aem_command_get_stream_info_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_get_stream_info_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_SET_NAME:
            desc_type = jdksavdecc_aem_command_set_name_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_set_name_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_GET_NAME:
            desc_type = jdksavdecc_aem_command_get_name_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_get_name_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE:
            desc_type = jdksavdecc_aem_command_set_sampling_rate_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_set_sampling_rate_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE:
            desc_type = jdksavdecc_aem_command_get_sampling_rate_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_get_sampling_rate_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE:
            desc_type = jdksavdecc_aem_command_set_clock_source_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_set_clock_source_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE:
            desc_type = jdksavdecc_aem_command_get_clock_source_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_get_clock_source_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_START_STREAMING:
            desc_type = jdksavdecc_aem_command_start_streaming_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_start_streaming_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        case JDKSAVDECC_AEM_COMMAND_STOP_STREAMING:
            desc_type = jdksavdecc_aem_command_stop_streaming_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
            desc_index = jdksavdecc_aem_command_stop_streaming_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
            break;

        default:
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "NO_MATCH_FOUND for %s", utility->aem_cmd_value_to_name(cmd_type));
            break;
        }

        if((notification_flag == CMD_WITH_NOTIFICATION) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE))
        {
            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        jdksavdecc_uint64_get(&jdksavdecc_common_control_header_get_stream_id(frame, ETHER_HDR_SIZE), 0),
                                                        cmd_type,
                                                        desc_type,
                                                        desc_index,
                                                        notification_id);
        }
        else if(((notification_flag == CMD_WITH_NOTIFICATION) || (notification_flag == CMD_WITHOUT_NOTIFICATION)) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND))
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "COMMAND_SENT, 0x%llx, %s, %s, %d, %d",
                                      jdksavdecc_uint64_get(&jdksavdecc_common_control_header_get_stream_id(frame, ETHER_HDR_SIZE), 0),
                                      utility->aem_cmd_value_to_name(cmd_type),
                                      utility->aem_desc_value_to_name(desc_type),
                                      desc_index,
                                      jdksavdecc_aecpdu_common_get_sequence_id(frame, ETHER_HDR_SIZE));
        }
        else if((notification_flag == CMD_WITHOUT_NOTIFICATION) && (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE))
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "RESPONSE_RECEIVED, 0x%llx, %s, %s, %d, %d",
                                      jdksavdecc_uint64_get(&jdksavdecc_common_control_header_get_stream_id(frame, ETHER_HDR_SIZE), 0),
                                      utility->aem_cmd_value_to_name(cmd_type),
                                      utility->aem_desc_value_to_name(desc_type),
                                      desc_index,
                                      jdksavdecc_aecpdu_common_get_sequence_id(frame, ETHER_HDR_SIZE));
        }

        return 0;
    }

    bool aem_controller_state_machine::is_inflight_cmd_with_notification_id(void *notification_id)
    {
       std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds_vec.begin(), inflight_cmds_vec.end(), NotificationComp(notification_id));

        if(j != inflight_cmds_vec.end()) // found?
        {
            return true;
        }

        return false;
    }
}
