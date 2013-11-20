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

    acmp::acmp()
    {

    }
    acmp::~acmp()
    {
    }

    /**
     * Transmit an ACMP Command.
     */
    void acmp::tx_cmd(
        void *notification_id,
        bool notification_flag,
        struct jdksavdecc_frame *ether_frame,
        bool resend)
    {
        int inflight_index;
        bool is_inflight;
        int send_frame_returned;

        if (!resend)
        {
            uint16_t this_seq_id = seq_id;

            //jdksavdecc_uint16_set(seq_id++, ether_frame->payload, aecp::SEQ_ID_POS);

            uint8_t msg_type = 0;
            //uint8_t msg_type = jdksavdecc_uint8_get(ether_frame->payload, aecp::MSG_TYPE_POS);

            // commands are spaced by 2 in ACMP msg_type space, so can compress the lookup table
            msg_type = msg_type / 2;
            if (msg_type > 6)
                msg_type = 6;
            int timeout_ms = acmp_timeouts_ms[msg_type];

            inflight in_flight = inflight(ether_frame,
                                          seq_id,
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

        //uint16_t seq_id = jdksavdecc_uint16_get(ether_frame->payload, aecp::SEQ_ID_POS);

        std::vector<inflight>::iterator j =
            std::find_if(inflight_cmds.begin(), inflight_cmds.end(), SeqIdComp(seq_id));

        if(j != inflight_cmds.end())	// found?
        {
            (*j).start_timer();
        }
    }

    /**
     * Process received response.
     */
    int acmp::process_resp(void *&notification_id, struct jdksavdecc_frame *ether_frame)
    {
        return 0;
    }

    /**
      * Notify the application that a command has timed out and the retry has timed out.
      */
    void acmp::state_TIMEOUT(inflight &inflight_command)
    {
    }


    /**
     * State machine COMMAND operation.
     */
    void acmp::state_COMMAND(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame)
    {
    }

    /**
     * State machine RESPONSE operation
     */
    void acmp::state_RESPONSE(void *&notification_id, struct jdksavdecc_frame *ether_frame)
    {
    }

    /**
     * Check timeout for the inflight commands.
     */
    void acmp::tick()
    {
    }

    bool acmp::notification_id_is_inflight(void *notification_id)
    {
        return false;
    }

    /**
     * Call notification or post_log_msg callback function for the command sent or response received.
     */
    int acmp::callback(void *notification_id, uint32_t notification_flag, uint8_t *frame)
    {
        return 0;
    }
};
