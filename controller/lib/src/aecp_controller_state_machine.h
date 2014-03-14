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
 * aecp_controller_state_machine.h
 *
 * AVDECC Entity Model Controller State Machine class
 */

#pragma once

#include "inflight.h"
#include "operation.h"

namespace avdecc_lib
{
    class inflight;

    class aecp_controller_state_machine
    {
    private:
        uint16_t aecp_seq_id; // The sequence id used for identifying the AECP command that a response is for
        std::vector<inflight> inflight_cmds;
        std::vector<operation> active_operations;

    public:
        aecp_controller_state_machine();

        ~aecp_controller_state_machine();

        /**
         * Initialize and fill Ethernet frame payload with Ethernet frame information for AEM commands.
         */
        int ether_frame_init(uint64_t end_station_mac, struct jdksavdecc_frame *cmd_frame, uint16_t len);

        /**
         * Initialize and fill Ethernet frame payload with 1722 AECP Header information.
         */
        void common_hdr_init(int message_type, struct jdksavdecc_frame *cmd_frame, uint64_t target_entity_id, uint32_t cd_len);

        /**
         * Process the Send Command state of the AEM Controller State Machine.
         */
        int state_send_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *cmd_frame);

        /**
         * Process the Received Unsolicited state of the AEM Controller State Machine.
         */
        int state_rcvd_unsolicited(void *&notification_id, struct jdksavdecc_frame *cmd_frame);

        /**
         * Process the Received Response state of the AEM Controller State Machine.
         */
        int state_rcvd_resp(void *&notification_id, struct jdksavdecc_frame *cmd_frame);

        /**
        * Check timeout for the inflight commands.
        */
        void tick();

        /**
         * Update inflight command for the response received.
         */
        int update_inflight_for_rcvd_resp(void *&notification_id, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *cmd_frame);

        int start_operation(void *&notification_id, uint16_t operation_id, uint16_t operation_type, const uint8_t *frame, ssize_t frame_len);
        bool is_active_operation_with_notification_id(void *notification_id);

        int update_operation_for_rcvd_resp(void *&notification_id, uint16_t operation_id, uint16_t percent_complete, struct jdksavdecc_frame *cmd_frame);

        /**
         * Check if the command with the corresponding notification id is already in the inflight command vector.
         */
        bool is_inflight_cmd_with_notification_id(void *notification_id);

    private:
        /**
         * Transmit an AEM Command.
         */
        int tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *cmd_frame, bool resend);

        /**
         * Handle the receipt and processing of a received unsolicited response for a command sent.
         */
        int proc_unsolicited(void *&notification_id, struct jdksavdecc_frame *cmd_frame);

        /**
         * Handle the receipt and processing of a received response for a command sent.
         */
        int proc_resp(void *&notification_id, struct jdksavdecc_frame *cmd_frame);

        /**
         * Notify the application that a command has timed out and the retry has timed out and the
         * inflight command is removed from the inflight list.
         */
        void state_timeout(uint32_t inflight_cmd_index);

        /**
         * Call notification or post_log_msg callback function for the command sent or response received.
         */
        int callback(void *notification_id, uint32_t notification_flag, uint8_t *frame);
    };

    extern aecp_controller_state_machine *aecp_controller_state_machine_ref;
}

