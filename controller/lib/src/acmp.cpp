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
 * acmp.cpp
 *
 * AVDECC Connection Management Protocol functions and state machine
 */

#include <algorithm>    // std::find_if
#include <vector>
#include "jdksavdecc_acmp_controller.h"
#include "net_interface_imp.h"
#include "util_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "inflight.h"
#include "adp.h"
#include "acmp.h"

namespace avdecc_lib
{
    acmp * acmp_ref = new acmp();

    acmp::acmp()
    {
        acmp_seq_id = 0x0;
    }

    acmp::~acmp() {}

    int acmp::ether_frame_init(struct jdksavdecc_frame *ether_frame)
    {
        /*** Offset to write the field to ***/
        size_t ether_frame_pos = 0;
        jdksavdecc_frame_init(ether_frame);

        /************************************************************* Ethernet Frame *********************************************************/
        ether_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
        utility->convert_uint64_to_eui48(net_interface_ref->get_mac(), ether_frame->src_address.value); // Send from the Controller MAC address
        ether_frame->dest_address = jdksavdecc_multicast_adp_acmp; // Send to the ACMP multicast destination MAC address
        ether_frame->length = ACMP_FRAME_LEN; // Length of ACMP packet is 70 bytes

        /********************* Fill frame payload with Ethernet frame information *****************/
        jdksavdecc_frame_write(ether_frame, ether_frame->payload, ether_frame_pos, ETHER_HDR_SIZE);

        return 0;
    }

    void acmp::common_hdr_init(uint32_t msg_type, struct jdksavdecc_frame *ether_frame)
    {
        struct jdksavdecc_acmpdu_common_control_header acmpdu_common_ctrl_hdr;
        int acmpdu_common_ctrl_hdr_returned;
        size_t acmpdu_common_pos;

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

        /*********************** Fill frame payload with AECP Common Control Header information **********************/
        acmpdu_common_ctrl_hdr_returned = jdksavdecc_acmpdu_common_control_header_write(&acmpdu_common_ctrl_hdr,
                                                                                        ether_frame->payload,
                                                                                        acmpdu_common_pos,
                                                                                        sizeof(ether_frame->payload));

        if(acmpdu_common_ctrl_hdr_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "acmpdu_common_ctrl_hdr_write error");
            assert(acmpdu_common_ctrl_hdr_returned >= 0);
        }
    }

    int acmp::state_command(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame)
    {
        return tx_cmd(notification_id, notification_flag, ether_frame, false);
    }

    int acmp::state_resp(void *&notification_id, uint32_t msg_type, struct jdksavdecc_frame *ether_frame)
    {
        return proc_resp(notification_id, msg_type, ether_frame);
    }

    void acmp::state_timeout(uint32_t inflight_cmd_index)
    {
        bool is_retried = inflight_cmds.at(inflight_cmd_index).retried();

        if(is_retried)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Command timeout");
            inflight_cmds.erase(inflight_cmds.begin() + inflight_cmd_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "Resend the command with sequence id = %d",
                                      inflight_cmds.at(inflight_cmd_index).cmd_seq_id);
            struct jdksavdecc_frame frame = inflight_cmds.at(inflight_cmd_index).frame();

            tx_cmd(inflight_cmds.at(inflight_cmd_index).cmd_notification_id,
                   inflight_cmds.at(inflight_cmd_index).notification_flag(),
                   &frame,
                   true);
        }
    }

    int acmp::tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame, bool resend)
    {
        int send_frame_returned;

        if(!resend)
        {
            uint16_t this_seq_id = acmp_seq_id;
            uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(ether_frame->payload, ETHER_HDR_SIZE);
            uint32_t timeout_ms = utility->acmp_cmd_to_timeout(msg_type); // ACMP command timeout lookup
            jdksavdecc_acmpdu_set_sequence_id(acmp_seq_id++, ether_frame->payload, ETHER_HDR_SIZE);

            inflight in_flight = inflight(ether_frame,
                                          this_seq_id,
                                          notification_id,
                                          notification_flag,
                                          timeout_ms);

            in_flight.start_timer();
            inflight_cmds.push_back(in_flight);
        }

        send_frame_returned = net_interface_ref->send_frame(ether_frame->payload, ether_frame->length);
        if(send_frame_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "netif_send_frame error");
            assert(send_frame_returned >= 0);
        }

        callback(notification_id, notification_flag, ether_frame->payload);

        std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(acmp_seq_id));

        if(j != inflight_cmds.end()) // found?
        {
            (*j).start_timer();
        }

        return 0;
    }

    int acmp::proc_resp(void *&notification_id, uint32_t msg_type, struct jdksavdecc_frame *ether_frame)
    {
        uint16_t seq_id = jdksavdecc_acmpdu_get_sequence_id(ether_frame->payload, ETHER_HDR_SIZE);
        int inflight_index = 0;
        uint32_t notification_flag = 0;

         std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(seq_id));

        if(j != inflight_cmds.end()) // found?
        {
            notification_id = (*j).cmd_notification_id;
            notification_flag = (*j).notification_flag();
            callback(notification_id, notification_flag, ether_frame->payload);
            inflight_cmds.erase(inflight_cmds.begin() + inflight_index);
            return 1;
        }

        return -1;
    }

    void acmp::tick()
    {
        for(uint32_t i = 0; i < inflight_cmds.size(); i++)
        {
            if(inflight_cmds.at(i).timeout())
            {
                state_timeout(i);
            }
        }
    }

    bool acmp::is_inflight_cmd_with_notification_id(void *notification_id)
    {
         std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), NotificationComp(notification_id));

        if(j != inflight_cmds.end()) // found?
        {
            return true;
        }

        return false;
    }

    int acmp::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
    {
        uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);
        uint16_t seq_id = jdksavdecc_acmpdu_get_sequence_id(frame, ETHER_HDR_SIZE);
        uint64_t end_station_guid;

        if((notification_flag == CMD_WITH_NOTIFICATION) &&
           ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) ||
            (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE)))
        {
            struct jdksavdecc_eui64 _end_station_guid = jdksavdecc_acmpdu_get_talker_entity_id(frame, ETHER_HDR_SIZE);
            end_station_guid = jdksavdecc_uint64_get(&_end_station_guid, 0);

            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        end_station_guid,
                                                        msg_type + CMD_LOOKUP,
                                                        NULL,
                                                        NULL,
                                                        notification_id);
        }
        else if((notification_flag == CMD_WITH_NOTIFICATION) &&
                ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE) ||
                 (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE) ||
                 (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE) ||
                 (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE)))
        {
            end_station_guid = jdksavdecc_uint64_get(&jdksavdecc_acmpdu_get_listener_entity_id(frame, ETHER_HDR_SIZE), 0);
            notification_imp_ref->post_notification_msg(RESPONSE_RECEIVED,
                                                        end_station_guid,
                                                        (uint16_t)msg_type + CMD_LOOKUP,
                                                        NULL,
                                                        NULL,
                                                        notification_id);
        }
        else if((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) ||
                (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE))
        {
            struct jdksavdecc_eui64 _end_station_guid = jdksavdecc_acmpdu_get_talker_entity_id(frame, ETHER_HDR_SIZE);
            end_station_guid = jdksavdecc_uint64_get(&_end_station_guid, 0);
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "RESPONSE_RECEIVED, 0x%llx, %s, %s, %s, %d",
                                      end_station_guid,
                                      utility->acmp_cmd_value_to_name(msg_type),
                                      "NULL",
                                      "NULL",    
                                      seq_id);
        }
        else
        {
            struct jdksavdecc_eui64 _end_station_guid = jdksavdecc_acmpdu_get_listener_entity_id(frame, ETHER_HDR_SIZE);
            end_station_guid = jdksavdecc_uint64_get(&_end_station_guid, 0);
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "COMMAND_SENT, 0x%llx, %s, %s, %s, %d",
                                      end_station_guid,
                                      utility->acmp_cmd_value_to_name(msg_type),
                                      "NULL",
                                      "NULL",
                                      seq_id);
        }

        return 0;
    }
};
