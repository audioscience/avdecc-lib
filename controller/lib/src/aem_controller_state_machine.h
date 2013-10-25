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
                struct aem_controller_state_machine_inflight_cmd
                {
                        //		uint64_t timeout;
                        bool retried;
                        struct jdksavdecc_frame inflight_cmd_frame;
                        uint16_t seq_id;
                        void *notification_id;
                        uint32_t notification_flag;
                        timer *avdecc_lib_timer_ref;
                        uint64_t dest_addr;
                };

                struct aem_controller_state_machine_variables
                {
                        bool rcvd_normal_resp;
                        bool rcvd_unsolicited_resp;
                        std::vector<struct aem_controller_state_machine_inflight_cmd> inflight_cmds_vector;
                        bool do_cmd;
                        bool do_terminate;
                };

                struct aem_controller_state_machine_variables controller_state_machine_vars;

        public:
                static uint16_t aecp_seq_id; // The sequence id used for identifying the AECP command that a response is for

                /**
                 * An empty constructor for aem_controller_state_machine
                 */
                aem_controller_state_machine();

                /**
                 * Destructor for aem_controller_state_machine used for destroying objects
                 */
                ~aem_controller_state_machine();

                /**
                 * Transmit an AEM Command.
                 */
                void tx_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame);

                /**
                 * Handle the receipt and processing of a received unsolicited notifying.
                 */
                int process_unsolicited(struct jdksavdecc_frame *ether_frame);

                /**
                 * Handle the receipt and processing of a received response for a command sent.
                 */
                int process_resp(void *notification_id, uint32_t &notification_flag, struct jdksavdecc_frame *ether_frame);

                /**
                 * Notify the application that a command has timed out and the retry has timed out.
                 */
                void timeout(uint32_t inflight_cmds_vector_index);

                /**
                 * Process the Waiting state of the AEM Controller State Machine.
                 */
                void aem_controller_state_waiting(void *notification_id, uint32_t &notification_flag, struct jdksavdecc_frame *ether_frame);

                /**
                 * Process the Send Command state of the AEM Controller State Machine.
                 */
                void aem_controller_state_send_cmd(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame);

                /**
                 * Process the Received Unsolicited state of the AEM Controller State Machine.
                 */
                void aem_controller_state_rcvd_unsolicited(struct jdksavdecc_frame *ether_frame);

                /**
                 * Process the Received Response state of the AEM Controller State Machine.
                 */
                void aem_controller_state_rcvd_resp(void *notification_id, uint32_t &notification_flag, struct jdksavdecc_frame *ether_frame);

                /**
                 * Check timeout for the inflight commands.
                 */
                void aem_controller_tick();

                /**
                 * Get the rcvd_normal_resp field from the aem_controller_state_machine_variables structure.
                 */
                inline bool get_rcvd_normal_resp()
                {
                        return controller_state_machine_vars.rcvd_normal_resp;
                }

                /**
                 * Set the rcvd_normal_resp field in the aem_controller_state_machine_variables structure.
                 */
                inline void set_rcvd_normal_resp(bool new_rcvd_normal_resp)
                {
                        controller_state_machine_vars.rcvd_normal_resp = new_rcvd_normal_resp;
                }

                /**
                 * Get the rcvd_unsolicited_resp field from the aem_controller_state_machine_variables structure.
                 */
                inline bool get_rcvd_unsolicited_resp()
                {
                        return controller_state_machine_vars.rcvd_unsolicited_resp;
                }

                /**
                 * Set the rcvd_unsolicited_resp field in the aem_controller_state_machine_variables structure.
                 */
                inline void set_rcvd_unsolicited_resp(bool new_rcvd_unsolicited_resp)
                {
                        controller_state_machine_vars.rcvd_unsolicited_resp = new_rcvd_unsolicited_resp;
                }

                /**
                 * Get the do_cmd field from the aem_controller_state_machine_variables structure.
                 */
                inline bool get_do_cmd()
                {
                        return controller_state_machine_vars.do_cmd;
                }

                /**
                 * Set the do_cmd field in the aem_controller_state_machine_variables structure.
                 */
                inline void set_do_cmd(bool new_do_cmd)
                {
                        controller_state_machine_vars.do_cmd = new_do_cmd;
                }

                /**
                 * Get the do_terminate field in the aem_controller_state_machine_variables structure.
                 */
                inline bool get_do_terminate()
                {
                        return controller_state_machine_vars.do_terminate;
                }

                /**
                 * Set the do_terminate field in the aem_controller_state_machine_variables structure.
                 */
                inline void set_do_terminate(bool new_do_terminate)
                {
                        controller_state_machine_vars.do_terminate = new_do_terminate;
                }

                /**
                 * Update inflight command for the response received.
                 */
                int update_inflight_for_rcvd_resp(void *notification_id, uint32_t &notification_flag, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *ether_frame);

                /**
                 * Call notifying or logging callback function for the command sent or response received.
                 */
                int callback(void *notification_id, uint32_t notification_flag, uint8_t *frame);

                /**
                 * Check if the command with the corresponding sequence id is already in the inflight command vector.
                 */
                bool find_inflight_cmd_by_seq_id(uint16_t seq_id, int *inflight_index);

                /**
                 * Check if the command with the corresponding notification id is already in the inflight command vector.
                 */
                bool find_inflight_cmd_by_notification_id(void *notification_id);

                /**
                 * Remove the command from the inflight command vector.
                 */
                int remove_inflight_cmd(uint32_t inflight_cmd_index);
        };

        extern aem_controller_state_machine *aem_controller_state_machine_ref;
}

#endif
