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
 * aecp_controller_state_machine.cpp
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
#include "operation.h"
#include "aecp_controller_state_machine.h"

namespace avdecc_lib
{
    aecp_controller_state_machine *aecp_controller_state_machine_ref = new aecp_controller_state_machine(); // To have one Controller State Machine for all end stations

    aecp_controller_state_machine::aecp_controller_state_machine()
    {
        aecp_seq_id = 0;
    }

    aecp_controller_state_machine::~aecp_controller_state_machine() {}

    int aecp_controller_state_machine::ether_frame_init(uint64_t end_station_mac, struct jdksavdecc_frame *cmd_frame, uint16_t len)
    {
        /*** Offset to write the field to ***/
        size_t ether_frame_pos = 0;
        jdksavdecc_frame_init(cmd_frame);

        /**************************************** Ethernet Frame *************************************/
        cmd_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
        utility->convert_uint64_to_eui48(net_interface_ref->mac_addr(), cmd_frame->src_address.value);
        utility->convert_uint64_to_eui48(end_station_mac, cmd_frame->dest_address.value);
		cmd_frame->length = len;

        /****************** Fill frame payload with Ethernet frame information ****************/
        jdksavdecc_frame_write(cmd_frame, cmd_frame->payload, ether_frame_pos, ETHER_HDR_SIZE);

        return 0;
    }

    void aecp_controller_state_machine::common_hdr_init(int message_type, struct jdksavdecc_frame *cmd_frame, uint64_t target_entity_id, uint32_t cd_len)
    {
        struct jdksavdecc_aecpdu_common_control_header aecpdu_common_ctrl_hdr;
        ssize_t aecpdu_common_ctrl_hdr_returned;

        /************************************** 1722 Protocol Header **************************************/
        aecpdu_common_ctrl_hdr.cd = 1;
        aecpdu_common_ctrl_hdr.subtype = JDKSAVDECC_SUBTYPE_AECP;
        aecpdu_common_ctrl_hdr.sv = 0;
        aecpdu_common_ctrl_hdr.version = 0;
        aecpdu_common_ctrl_hdr.message_type = message_type;
        aecpdu_common_ctrl_hdr.status = JDKSAVDECC_AEM_STATUS_SUCCESS;
        aecpdu_common_ctrl_hdr.control_data_length = cd_len;
        jdksavdecc_uint64_write(target_entity_id, &aecpdu_common_ctrl_hdr.target_entity_id, 0, sizeof(uint64_t));

        /********************** Fill frame payload with AECP Common Control Header information *********************/
        aecpdu_common_ctrl_hdr_returned = jdksavdecc_aecpdu_common_control_header_write(&aecpdu_common_ctrl_hdr,
                                                                                        cmd_frame->payload,
                                                                                        ETHER_HDR_SIZE, // Offset to write the field to
                                                                                        sizeof(cmd_frame->payload));

        if(aecpdu_common_ctrl_hdr_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "common_hdr_init error");
            assert(aecpdu_common_ctrl_hdr_returned >= 0);
        }
    }

    int aecp_controller_state_machine::tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *cmd_frame, bool resend)
    {
        int send_frame_returned;

        if (!resend)
        {
            uint16_t current_seq_id = aecp_seq_id;

            jdksavdecc_aecpdu_common_set_sequence_id(aecp_seq_id++, cmd_frame->payload, ETHER_HDR_SIZE);
            inflight in_flight = inflight(cmd_frame,
                                          current_seq_id,
                                          notification_id,
                                          notification_flag,
                                          AVDECC_MSG_TIMEOUT_MS);
            in_flight.start_timer();
            inflight_cmds.push_back(in_flight);
        }
        else
        {
            uint16_t resend_with_seq_id = jdksavdecc_aecpdu_common_get_sequence_id(cmd_frame->payload, ETHER_HDR_SIZE);
            std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(resend_with_seq_id));

            if(j != inflight_cmds.end()) // found?
            {
                j->start_timer();
            }
        }

        send_frame_returned = net_interface_ref->send_frame(cmd_frame->payload, cmd_frame->length);
        if(send_frame_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "netif_send_frame error");
            assert(send_frame_returned >= 0);
        }

        callback(notification_id, notification_flag, cmd_frame->payload);

        return 0;
    }

    int aecp_controller_state_machine::proc_unsolicited(void *&notification_id, struct jdksavdecc_frame *cmd_frame)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "proc_unsolicited is not implemented.");

        return 0;
    }

    int aecp_controller_state_machine::proc_resp(void *&notification_id, struct jdksavdecc_frame *cmd_frame)
    {
        uint16_t seq_id = jdksavdecc_aecpdu_common_get_sequence_id(cmd_frame->payload, ETHER_HDR_SIZE);
        uint32_t notification_flag = 0;

        std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(seq_id));

        if(j != inflight_cmds.end()) // found?
        {
            notification_id = j->cmd_notification_id;
            notification_flag = j->notification_flag();
            callback(notification_id, notification_flag, cmd_frame->payload);
            inflight_cmds.erase(j);
            return 1;
        }

        return -1;
    }

    int aecp_controller_state_machine::state_send_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *cmd_frame)
    {
       return tx_cmd(notification_id, notification_flag, cmd_frame, false);
    }

    int aecp_controller_state_machine::state_rcvd_unsolicited(void *&notification_id, struct jdksavdecc_frame *cmd_frame)
    {
       return proc_unsolicited(notification_id, cmd_frame);
    }

    int aecp_controller_state_machine::state_rcvd_resp(void *&notification_id, struct jdksavdecc_frame *cmd_frame)
    {
       return proc_resp(notification_id, cmd_frame);
    }

    void aecp_controller_state_machine::state_timeout(uint32_t inflight_cmd_index)
    {
        struct jdksavdecc_frame frame = inflight_cmds.at(inflight_cmd_index).frame();
        bool is_retried = inflight_cmds.at(inflight_cmd_index).retried();
        uint32_t notification_flag = inflight_cmds.at(inflight_cmd_index).notification_flag();

        if(is_retried)
        {
            jdksavdecc_eui64 id = jdksavdecc_common_control_header_get_stream_id(frame.payload, ETHER_HDR_SIZE);
            uint16_t cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame.payload, ETHER_HDR_SIZE );
            cmd_type &= 0x7FFF;
            uint16_t desc_type = jdksavdecc_aem_command_read_descriptor_get_descriptor_type(frame.payload, ETHER_HDR_SIZE);
            uint16_t desc_index = jdksavdecc_aem_command_read_descriptor_get_descriptor_index(frame.payload, ETHER_HDR_SIZE);
            
            notification_imp_ref->post_notification_msg(COMMAND_TIMEOUT,
                                                        jdksavdecc_uint64_get(&id, 0),
                                                        cmd_type,
                                                        desc_type,
                                                        desc_index,
                                                        UINT_MAX,
                                                        inflight_cmds.at(inflight_cmd_index).cmd_notification_id);

            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                      "Command Timeout, 0x%llx, %s, %s, %d, %d",
                                      jdksavdecc_uint64_get(&id, 0),
                                      utility->aem_cmd_value_to_name(cmd_type),
                                      utility->aem_desc_value_to_name(desc_type),
                                      desc_index,
                                      inflight_cmds.at(inflight_cmd_index).cmd_seq_id);

            inflight_cmds.erase(inflight_cmds.begin() + inflight_cmd_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "Resend the command with sequence id = %d",
                                      inflight_cmds.at(inflight_cmd_index).cmd_seq_id);

            tx_cmd(inflight_cmds.at(inflight_cmd_index).cmd_notification_id,
                   notification_flag,
                   &frame,
                   true);
        }
    }

    void aecp_controller_state_machine::tick()
    {
        for(uint32_t i = 0; i < inflight_cmds.size(); i++)
        {
            if(inflight_cmds.at(i).timeout())
            {
                state_timeout(i);
            }
        }
    }

    int aecp_controller_state_machine::update_inflight_for_rcvd_resp(void *&notification_id, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *cmd_frame)
    {
        switch (msg_type)
        {
            case JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE:
            case JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE: // Fallthrough intentional
                if (u_field)
                {
                    state_rcvd_unsolicited(notification_id, cmd_frame);
                    state_rcvd_resp(notification_id, cmd_frame);
                }
                else
                {
                    state_rcvd_resp(notification_id, cmd_frame);
                }
                break;
            default:
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid message type");
                return -1;
        }

        return 0;
    }

    int aecp_controller_state_machine::start_operation(void *&notification_id, uint16_t operation_id, uint16_t operation_type, const uint8_t *frame, ssize_t frame_len)
    {
        struct jdksavdecc_frame cmd_frame;
        memcpy(cmd_frame.payload, frame, frame_len);

        operation oper = operation(&cmd_frame,
                                  operation_id,
                                  operation_type,
                                  notification_id,
                                  CMD_WITH_NOTIFICATION);
        active_operations.push_back(oper);

        log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Added new operation with type %x and id %d", operation_type, operation_id);

        return 0;
    }

    int aecp_controller_state_machine::update_operation_for_rcvd_resp(void *&notification_id, uint16_t operation_id, uint16_t percent_complete, struct jdksavdecc_frame *cmd_frame)
    {
        uint32_t notification_flag = 0;

        std::vector<operation>::iterator j =
            std::find_if(active_operations.begin(), active_operations.end(), operation_id_comp(operation_id));

        if(j != active_operations.end()) // found?
        {
            notification_id = j->cmd_notification_id;
            notification_flag = j->notification_flag();
            callback(notification_id, notification_flag, cmd_frame->payload);
            if (percent_complete == 0 || percent_complete == 1000)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Removed operation with id %d, percent_complete: %d", operation_id, percent_complete);
                active_operations.erase(j);
            }
            return 1;
        }

        return -1;
    }

    bool aecp_controller_state_machine::is_active_operation_with_notification_id(void *notification_id)
    {
        std::vector<operation>::iterator j =
            std::find_if(active_operations.begin(), active_operations.end(), notification_comp(notification_id));

        if(j != active_operations.end()) // found?
        {
            return true;
        }

        return false;
    }

    int aecp_controller_state_machine::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
    {
        uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);

        uint16_t cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame, ETHER_HDR_SIZE);
        cmd_type &= 0x7FFF;
        uint32_t status = jdksavdecc_common_control_header_get_status(frame, ETHER_HDR_SIZE);
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

        jdksavdecc_eui64 id = jdksavdecc_common_control_header_get_stream_id(frame, ETHER_HDR_SIZE);
        if((notification_flag == CMD_WITH_NOTIFICATION) &&
            ((msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE) ||
            (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE)))
        {
            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        jdksavdecc_uint64_get(&id, 0),
                                                        cmd_type,
                                                        desc_type,
                                                        desc_index,
                                                        status,
                                                        notification_id);

            if(status != AEM_STATUS_SUCCESS)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                          "RESPONSE_RECEIVED, 0x%llx, %s, %s, %d, %d, %s",
                                          jdksavdecc_uint64_get(&id, 0),
                                          utility->aem_cmd_value_to_name(cmd_type),
                                          utility->aem_desc_value_to_name(desc_type),
                                          desc_index,
                                          jdksavdecc_aecpdu_common_get_sequence_id(frame, ETHER_HDR_SIZE),
                                          utility->aem_cmd_status_value_to_name(status));
            }
        }
        else if(((notification_flag == CMD_WITH_NOTIFICATION) || (notification_flag == CMD_WITHOUT_NOTIFICATION)) &&
                ((msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND) || (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND)))
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "COMMAND_SENT, 0x%llx, %s, %s, %d, %d",
                                      jdksavdecc_uint64_get(&id, 0),
                                      utility->aem_cmd_value_to_name(cmd_type),
                                      utility->aem_desc_value_to_name(desc_type),
                                      desc_index,
                                      jdksavdecc_aecpdu_common_get_sequence_id(frame, ETHER_HDR_SIZE));
        }
        else if((notification_flag == CMD_WITHOUT_NOTIFICATION) &&
                ((msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE) ||
                (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE)))
        {
            if(status == AEM_STATUS_SUCCESS)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                          "RESPONSE_RECEIVED, 0x%llx, %s, %s, %d, %d, %s",
                                          jdksavdecc_uint64_get(&id, 0),
                                          utility->aem_cmd_value_to_name(cmd_type),
                                          utility->aem_desc_value_to_name(desc_type),
                                          desc_index,
                                          jdksavdecc_aecpdu_common_get_sequence_id(frame, ETHER_HDR_SIZE),
                                          utility->aem_cmd_status_value_to_name(status));
            }
            else
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                          "RESPONSE_RECEIVED, 0x%llx, %s, %s, %d, %d, %s",
                                          jdksavdecc_uint64_get(&id, 0),
                                          utility->aem_cmd_value_to_name(cmd_type),
                                          utility->aem_desc_value_to_name(desc_type),
                                          desc_index,
                                          jdksavdecc_aecpdu_common_get_sequence_id(frame, ETHER_HDR_SIZE),
                                          utility->aem_cmd_status_value_to_name(status));
            }
        }

        return 0;
    }

    bool aecp_controller_state_machine::is_inflight_cmd_with_notification_id(void *notification_id)
    {
       std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), NotificationComp(notification_id));

        if(j != inflight_cmds.end()) // found?
        {
            return true;
        }

        return false;
    }
}
