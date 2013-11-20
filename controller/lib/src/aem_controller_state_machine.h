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
 * aem_controller_state_machine.h
 *
 * AVDECC Entity Model Controller State Machine class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AEM_CONTROLLER_STATE_MACHINE_H_
#define _AVDECC_CONTROLLER_LIB_AEM_CONTROLLER_STATE_MACHINE_H_

#include "timer.h"

namespace avdecc_lib
{
    class aem_controller_state_machine
    {
    private:
        struct inflight_cmd_data
        {
            bool retried;
            struct jdksavdecc_frame inflight_cmd_frame;
            uint16_t seq_id;
            void *notification_id;
            uint32_t notification_flag;
            timer inflight_timer;
        };

        uint16_t aecp_seq_id; // The sequence id used for identifying the AECP command that a response is for
        bool rcvd_normal_resp;
        bool rcvd_unsolicited_resp;
        bool do_cmd;
        bool do_terminate;
        std::vector<struct inflight_cmd_data> inflight_cmds_vec;

    public:
        aem_controller_state_machine();

        ~aem_controller_state_machine();

        /**
         * Process the Waiting state of the AEM Controller State Machine.
         */
        void state_waiting(void *&notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame);

	      /**
         * Check timeout for the inflight commands.
         */
        void tick();

        /**
         * Get the rcvd_normal_resp field from the aem_controller_state_machine_variables structure.
         */
        inline bool get_rcvd_normal_resp()
        {
            return rcvd_normal_resp;
        }

        /**
         * Set the rcvd_normal_resp field in the aem_controller_state_machine_variables structure.
         */
        inline void set_rcvd_normal_resp(bool new_rcvd_normal_resp)
        {
            rcvd_normal_resp = new_rcvd_normal_resp;
        }

        /**
         * Get the rcvd_unsolicited_resp field from the aem_controller_state_machine_variables structure.
         */
        inline bool get_rcvd_unsolicited_resp()
        {
            return rcvd_unsolicited_resp;
        }

        /**
         * Set the rcvd_unsolicited_resp field in the aem_controller_state_machine_variables structure.
         */
        inline void set_rcvd_unsolicited_resp(bool new_rcvd_unsolicited_resp)
        {
            rcvd_unsolicited_resp = new_rcvd_unsolicited_resp;
        }

        /**
         * Get the do_cmd field from the aem_controller_state_machine_variables structure.
         */
        inline bool get_do_cmd()
        {
            return do_cmd;
        }

        /**
         * Set the do_cmd field in the aem_controller_state_machine_variables structure.
         */
        inline void set_do_cmd(bool new_do_cmd)
        {
            do_cmd = new_do_cmd;
        }

        /**
         * Get the do_terminate field in the aem_controller_state_machine_variables structure.
         */
        inline bool get_do_terminate()
        {
            return do_terminate;
        }

        /**
         * Set the do_terminate field in the aem_controller_state_machine_variables structure.
         */
        inline void set_do_terminate(bool new_do_terminate)
        {
            do_terminate = new_do_terminate;
        }

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
         * Transmit an AEM Command.
         */
        void tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame, bool resend);

        /**
         * Handle the receipt and processing of a received unsolicited response for a command sent.
         */
        int proc_unsolicited(void *&notification_id, struct jdksavdecc_frame *ether_frame);

        /**
         * Handle the receipt and processing of a received response for a command sent.
         */
        int proc_resp(void *&notification_id, struct jdksavdecc_frame *ether_frame);

        /**
         * Notify the application that a command has timed out and the retry has timed out and the
         * inflight command is removed from the inflight list.
         */
        void timeout(uint32_t inflight_cmd_index);

        /**
         * Process the Send Command state of the AEM Controller State Machine.
         */
        void state_send_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame);

        /**
         * Process the Received Unsolicited state of the AEM Controller State Machine.
         */
        void state_rcvd_unsolicited(void *&notification_id, struct jdksavdecc_frame *ether_frame);

        /**
         * Process the Received Response state of the AEM Controller State Machine.
         */
        void state_rcvd_resp(void *&notification_id, struct jdksavdecc_frame *ether_frame);

	/**
         * Check if the command with the corresponding sequence id is already in the inflight command vector.
         */
        bool find_inflight_cmd_by_seq_id(uint16_t seq_id, int *inflight_index);

        /**
         * Call notification or post_log_msg callback function for the command sent or response received.
         */
        int callback(void *notification_id, uint32_t notification_flag, uint8_t *frame);
    };

    extern aem_controller_state_machine *aem_controller_state_machine_ref;
}

#endif
