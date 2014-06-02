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
 * acmp_controller_state_machine.cpp
 *
 * AVDECC Connection Management Protocol Controller State Machine implementation
 */

#include <algorithm> // std::find_if
#include <vector>
#include "jdksavdecc_acmp.h"
#include "net_interface_imp.h"
#include "util.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "inflight.h"
#include "adp.h"
#include "acmp_controller_state_machine.h"

namespace avdecc_lib
{
    acmp_controller_state_machine * acmp_controller_state_machine_ref = new acmp_controller_state_machine();

    acmp_controller_state_machine::acmp_controller_state_machine()
    {
        acmp_seq_id = 0;
    }

    acmp_controller_state_machine::~acmp_controller_state_machine() {}

    int acmp_controller_state_machine::ether_frame_init(struct jdksavdecc_frame *cmd_frame)
    {
        /*** Offset to write the field to ***/
        size_t ether_frame_pos = 0;
        jdksavdecc_frame_init(cmd_frame);

        /************************************************************ Ethernet Frame ********************************************************/
        cmd_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
        utility::convert_uint64_to_eui48(net_interface_ref->mac_addr(), cmd_frame->src_address.value); // Send from the Controller MAC address
        cmd_frame->dest_address = jdksavdecc_multicast_adp_acmp; // Send to the ACMP multicast destination MAC address
        cmd_frame->length = ACMP_FRAME_LEN; // Length of ACMP packet is 70 bytes

        /****************** Fill frame payload with Ethernet frame information ****************/
        jdksavdecc_frame_write(cmd_frame, cmd_frame->payload, ether_frame_pos, ETHER_HDR_SIZE);

        return 0;
    }

    void acmp_controller_state_machine::common_hdr_init(uint32_t msg_type, struct jdksavdecc_frame *cmd_frame)
    {
        struct jdksavdecc_acmpdu_common_control_header acmpdu_common_ctrl_hdr;
        ssize_t acmpdu_common_ctrl_hdr_returned;
        ssize_t acmpdu_common_pos;

        /***** Offset to write the field to ****/
        acmpdu_common_pos = ETHER_HDR_SIZE;

        /******************* 1722 Protocol Header *********************/
        acmpdu_common_ctrl_hdr.cd = 1;
        acmpdu_common_ctrl_hdr.subtype = JDKSAVDECC_SUBTYPE_ACMP;
        acmpdu_common_ctrl_hdr.sv = 0;
        acmpdu_common_ctrl_hdr.version = 0;
        acmpdu_common_ctrl_hdr.message_type = msg_type;
        acmpdu_common_ctrl_hdr.status = JDKSAVDECC_ACMP_STATUS_SUCCESS;
        acmpdu_common_ctrl_hdr.control_data_length = 44;
        jdksavdecc_eui64_init(&acmpdu_common_ctrl_hdr.stream_id);

        /********************** Fill frame payload with AECP Common Control Header information *********************/
        acmpdu_common_ctrl_hdr_returned = jdksavdecc_acmpdu_common_control_header_write(&acmpdu_common_ctrl_hdr,
                                                                                        cmd_frame->payload,
                                                                                        acmpdu_common_pos,
                                                                                        sizeof(cmd_frame->payload));

        if(acmpdu_common_ctrl_hdr_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "acmpdu_common_ctrl_hdr_write error");
            assert(acmpdu_common_ctrl_hdr_returned >= 0);
        }
    }

    int acmp_controller_state_machine::state_command(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *cmd_frame)
    {
        return tx_cmd(notification_id, notification_flag, cmd_frame, false);
    }

    int acmp_controller_state_machine::state_resp(void *&notification_id, struct jdksavdecc_frame *cmd_frame)
    {
        return proc_resp(notification_id, cmd_frame);
    }

    void acmp_controller_state_machine::state_timeout(uint32_t inflight_cmd_index)
    {
        struct jdksavdecc_frame frame = inflight_cmds.at(inflight_cmd_index).frame();
        bool is_retried = inflight_cmds.at(inflight_cmd_index).retried();

        if(is_retried)
        {
            struct jdksavdecc_eui64 _end_station_entity_id = jdksavdecc_acmpdu_get_listener_entity_id(frame.payload, ETHER_HDR_SIZE);
            uint64_t end_station_entity_id = jdksavdecc_uint64_get(&_end_station_entity_id, 0);
            uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame.payload, ETHER_HDR_SIZE);

            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        end_station_entity_id,
                                                        (uint16_t)msg_type + CMD_LOOKUP,
                                                        0,
                                                        0,
                                                        UINT_MAX,
                                                        inflight_cmds.at(inflight_cmd_index).cmd_notification_id);

            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                      "Command Timeout, 0x%llx, %s, %s, %s, %d",
                                      end_station_entity_id,
                                      utility::acmp_cmd_value_to_name(msg_type),
                                      "NULL",
                                      "NULL",
                                      inflight_cmds.at(inflight_cmd_index).cmd_seq_id);

            inflight_cmds.erase(inflight_cmds.begin() + inflight_cmd_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "Resend the command with sequence id = %d",
                                      inflight_cmds.at(inflight_cmd_index).cmd_seq_id);
           
            tx_cmd(inflight_cmds.at(inflight_cmd_index).cmd_notification_id,
                   inflight_cmds.at(inflight_cmd_index).notification_flag(),
                   &frame,
                   true);
        }
    }

    int acmp_controller_state_machine::tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *cmd_frame, bool resend)
    {
        int send_frame_returned;

        if(!resend)
        {
            uint16_t this_seq_id = acmp_seq_id;
            uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(cmd_frame->payload, ETHER_HDR_SIZE);
            uint32_t timeout_ms = utility::acmp_cmd_to_timeout(msg_type); // ACMP command timeout lookup
            jdksavdecc_acmpdu_set_sequence_id(acmp_seq_id++, cmd_frame->payload, ETHER_HDR_SIZE);

            inflight in_flight = inflight(cmd_frame,
                                          this_seq_id,
                                          notification_id,
                                          notification_flag,
                                          timeout_ms);

            in_flight.start_timer();
            inflight_cmds.push_back(in_flight);
        }
        else
        {
            uint16_t resend_with_seq_id = jdksavdecc_acmpdu_get_sequence_id(cmd_frame->payload, ETHER_HDR_SIZE);
            std::vector<inflight>::iterator j =
                std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(resend_with_seq_id));

            if(j != inflight_cmds.end()) // found?
            {
                (*j).start_timer();
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

    int acmp_controller_state_machine::proc_resp(void *&notification_id, struct jdksavdecc_frame *cmd_frame)
    {
        uint16_t seq_id = jdksavdecc_acmpdu_get_sequence_id(cmd_frame->payload, ETHER_HDR_SIZE);
        uint32_t notification_flag = 0;

         std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(seq_id));

        if(j != inflight_cmds.end()) // found?
        {
            notification_id = (*j).cmd_notification_id;
            notification_flag = (*j).notification_flag();
            callback(notification_id, notification_flag, cmd_frame->payload);
            inflight_cmds.erase(j);
            return 1;
        }

        return -1;
    }

    void acmp_controller_state_machine::tick()
    {
        for(uint32_t i = 0; i < inflight_cmds.size(); i++)
        {
            if(inflight_cmds.at(i).timeout())
            {
                state_timeout(i);
            }
        }
    }

    bool acmp_controller_state_machine::is_inflight_cmd_with_notification_id(void *notification_id)
    {
         std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), NotificationComp(notification_id));

        if(j != inflight_cmds.end()) // found?
        {
            return true;
        }

        return false;
    }

    int acmp_controller_state_machine::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
    {
        uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);
        uint16_t seq_id = jdksavdecc_acmpdu_get_sequence_id(frame, ETHER_HDR_SIZE);
        uint32_t status = jdksavdecc_common_control_header_get_status(frame, ETHER_HDR_SIZE);
        uint64_t end_station_entity_id;

        if((notification_flag == CMD_WITH_NOTIFICATION) &&
           ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) ||
            (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE)))
        {
            struct jdksavdecc_eui64 _end_station_entity_id = jdksavdecc_acmpdu_get_talker_entity_id(frame, ETHER_HDR_SIZE);
            end_station_entity_id = jdksavdecc_uint64_get(&_end_station_entity_id, 0);

            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        end_station_entity_id,
                                                        (uint16_t)msg_type + CMD_LOOKUP,
                                                        0,
                                                        0,
                                                        status,
                                                        notification_id);

            if(status != ACMP_STATUS_SUCCESS)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                          "RESPONSE_RECEIVED, 0x%llx, %s, %s, %s, %s, %d",
                                          end_station_entity_id,
                                          utility::acmp_cmd_value_to_name(msg_type),
                                          "NULL",
                                          "NULL",  
                                          utility::acmp_cmd_status_value_to_name(status),
                                          seq_id);
            }
        }
        else if((notification_flag == CMD_WITH_NOTIFICATION) &&
                ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE) ||
                 (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE) ||
                 (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE) ||
                 (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE)))
        {
            struct jdksavdecc_eui64 _end_station_entity_id = jdksavdecc_acmpdu_get_listener_entity_id(frame, ETHER_HDR_SIZE);
            end_station_entity_id = jdksavdecc_uint64_get(&_end_station_entity_id, 0);
            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        end_station_entity_id,
                                                        (uint16_t)msg_type + CMD_LOOKUP,
                                                        0,
                                                        0,
                                                        status,
                                                        notification_id);

            if(status != ACMP_STATUS_SUCCESS)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR,
                                          "RESPONSE_RECEIVED, 0x%llx, %s, %s, %s, %s, %d",
                                          end_station_entity_id,
                                          utility::acmp_cmd_value_to_name(msg_type),
                                          "NULL",
                                          "NULL",  
                                          utility::acmp_cmd_status_value_to_name(status),
                                          seq_id);
            }
        }
        else if((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) ||
                (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE))
        {
            struct jdksavdecc_eui64 _end_station_entity_id = jdksavdecc_acmpdu_get_talker_entity_id(frame, ETHER_HDR_SIZE);
            end_station_entity_id = jdksavdecc_uint64_get(&_end_station_entity_id, 0);
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "RESPONSE_RECEIVED, 0x%llx, %s, %s, %s, %s, %d",
                                      end_station_entity_id,
                                      utility::acmp_cmd_value_to_name(msg_type),
                                      "NULL",
                                      "NULL",  
                                      utility::acmp_cmd_status_value_to_name(status),
                                      seq_id);
        }
        else
        {
            struct jdksavdecc_eui64 _end_station_entity_id = jdksavdecc_acmpdu_get_listener_entity_id(frame, ETHER_HDR_SIZE);
            end_station_entity_id = jdksavdecc_uint64_get(&_end_station_entity_id, 0);
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "COMMAND_SENT, 0x%llx, %s, %s, %s, %s, %d",
                                      end_station_entity_id,
                                      utility::acmp_cmd_value_to_name(msg_type),
                                      "NULL",
                                      "NULL",
                                      utility::acmp_cmd_status_value_to_name(status),
                                      seq_id);
        }

        return 0;
    }
};
