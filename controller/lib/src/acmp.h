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
 * acmp.h
 *
 * AVDECC Connection Management Protocol functions and state machine
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_ACMP_H_
#define _AVDECC_CONTROLLER_LIB_ACMP_H_

namespace avdecc_lib
{
    class inflight;

    class acmp
    {
    private:
        uint16_t acmp_seq_id; // The sequence id used for identifying the ACMP command that a response is for
        std::vector<inflight> inflight_cmds;

    public:
        acmp();

        ~acmp();

        /**
         * Initialize and fill Ethernet frame payload with Ethernet frame information for AEM commands.
         */
        static int ether_frame_init(struct jdksavdecc_frame *ether_frame);

        /**
         * Initialize and fill Ethernet frame payload with 1722 ACMP Header information.
         */
        static void common_hdr_init(uint32_t msg_type, struct jdksavdecc_frame *ether_frame);

        /**
         * Process the Command state of the ACMP Controller State Machine.
         */
        int state_command(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame);

        /**
         * Process the Response state of the ACMP Controller State Machine.
         */
        int state_resp(void *&notification_id, uint32_t msg_type, struct jdksavdecc_frame *ether_frame);

        /**
         * Check timeout for the inflight commands.
         */
        void tick();

        /**
         * Update inflight command for the response received.
         */
        int update_inflight_for_rcvd_resp(void *&notification_id, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *ether_frame);

        /**
         * Check if the command with the corresponding notification id is already in the inflight command vector.
         */
        bool is_inflight_cmd_with_notification_id(void *notification_id);

    private:
        /**
         * Process the Timeout state of the ACMP Controller State Machine.
         */
        void state_timeout(uint32_t inflight_cmd_index);

        /**
         * Transmit an ACMP Command.
         */
        int tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame, bool resend);

        /**
         * Handle the receipt and processing of a received response for a command sent.
         */
        int proc_resp(void *&notification_id, uint32_t msg_type, struct jdksavdecc_frame *ether_frame);

        /**
         * Call notification or post_log_msg callback function for the command sent or response received.
         */
        int callback(void *notification_id, uint32_t notification_flag, uint8_t *frame);
    };

    extern acmp *acmp_ref;
}

#endif
