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

#include "net_interface_imp.h"
#include "util_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "end_station.h"
#include "adp.h"
#include "acmp.h"

namespace avdecc_lib
{
    static int acmp_timeouts_ms[7] =
    {
        2000, // JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_TX_COMMAND (0)
        200,  // JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_TX_COMMAND (2)
        200,  // JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_COMMAND (4)
        4500, // JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_COMMAND (6)
        500,  // JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_COMMAND (8)
        200,  // JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_COMMAND (10)
        200   // JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_COMMAND (12)
    };

    acmp * acmp_ref = new acmp();

    acmp::acmp()
    {
	acmp_seq_id = 0x0;
    }

    acmp::~acmp() {}

    int acmp::ether_frame_init(end_station *end_station, struct jdksavdecc_frame *ether_frame)
    {
        /*** Offset to write the field to ***/
        size_t ether_frame_pos = 0x0;
        jdksavdecc_frame_init(ether_frame);

        /************************************************ Ethernet Frame *************************************/
        ether_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
        utility->convert_uint64_to_eui48(net_interface_ref->get_mac(), ether_frame->src_address.value);
        utility->convert_uint64_to_eui48(end_station->get_end_station_mac(), ether_frame->dest_address.value);
        ether_frame->length = ACMP_FRAME_LEN; // Length of ACMP packet is 70 bytes

        /*********************** Fill frame payload with Ethernet frame information ********************/
        jdksavdecc_frame_write(ether_frame, ether_frame->payload, ether_frame_pos, adp::ETHER_HDR_SIZE);

        return 0;
    }

    void acmp::common_hdr_init(uint32_t msg_type, struct jdksavdecc_frame *ether_frame)
    {
        struct jdksavdecc_acmpdu_common_control_header acmpdu_common_ctrl_hdr;
        int acmpdu_common_ctrl_hdr_returned;
        size_t acmpdu_common_pos;

        /***** Offset to write the field to ****/
        acmpdu_common_pos = adp::ETHER_HDR_SIZE;

        /************************************** 1722 Protocol Header **************************************/
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

    int acmp::tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame, bool resend)
    {
        int inflight_index;
        bool is_inflight;
        int send_frame_returned;

        if(!resend)
        {
            uint16_t this_seq_id = acmp_seq_id;

	    jdksavdecc_acmpdu_set_sequence_id(acmp_seq_id++, ether_frame->payload, adp::ETHER_HDR_SIZE);

            uint8_t msg_type_timeout_lookup = 0;
            uint8_t msg_type = jdksavdecc_uint8_get(ether_frame->payload, adp::ETHER_HDR_SIZE + 1);

            msg_type_timeout_lookup = msg_type / 2; // Commands are spaced by 2 in ACMP msg_type space, so can compress the lookup table
            if(msg_type_timeout_lookup > 6)
	    {
                msg_type = 6;
	    }
	
	    int timeout_ms = acmp_timeouts_ms[msg_type];

            inflight in_flight = inflight(ether_frame,
                                          acmp_seq_id,
                                          timeout_ms,
                                          notification_id,
                                          notification_flag);

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
	uint16_t seq_id = jdksavdecc_acmpdu_get_sequence_id(ether_frame->payload, adp::ETHER_HDR_SIZE);
        int inflight_index = 0;
        uint32_t notification_flag = 0;

        if(find_inflight_cmd_by_seq_id(seq_id, &inflight_index))
        {
            notification_id = inflight_cmds.at(inflight_index).notification_id;
	    notification_flag = inflight_cmds.at(inflight_index).notify_flag();
            callback(notification_id, notification_flag, ether_frame->payload);
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Command Success");
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

    bool acmp::find_inflight_cmd_by_seq_id(uint16_t seq_id, int *inflight_index)
    {
        for(uint32_t i = 0; i < inflight_cmds.size(); i++)
        {
            if((inflight_cmds.at(i).seq_id == seq_id))
            {
                *inflight_index = i;
                return true;
            }
        }

        return false;
    }

    bool acmp::is_inflight_cmd_with_notification_id(void *notification_id)
    {
        for(uint32_t i = 0; i < inflight_cmds.size(); i++)
        {
            if((inflight_cmds.at(i).notification_id == notification_id))
            {
                return true;
            }
        }

        return false;
    }

    int acmp::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
    {
        return 0;
    }

    void acmp::state_timeout(uint32_t inflight_cmd_index)
    {
        bool is_retry = inflight_cmds.at(inflight_cmd_index).retried();

        if(is_retry)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Command timeout");
            inflight_cmds.erase(inflight_cmds.begin() + inflight_cmd_index);
            printf("\n>");
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG,
                                      "Resend the command with sequence id = %d",
                                      inflight_cmds.at(inflight_cmd_index).seq_id);

            tx_cmd(inflight_cmds.at(inflight_cmd_index).notification_id,
                   inflight_cmds.at(inflight_cmd_index).notification_flag,
                   &inflight_cmds.at(inflight_cmd_index).cmd_frame(),
                   true);
        }
    }
};
