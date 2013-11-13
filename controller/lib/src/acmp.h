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

#include "inflight.h"

namespace avdecc_lib
{
	class acmp
	{

	public:
		uint16_t seq_id; // sequence id inserted in ACMP commands

		acmp();
		~acmp();

		/**
		 * Transmit an ACMP Command.
		 */
		void tx_cmd(	void *notification_id,
				bool notification_flag,
				struct jdksavdecc_frame *ether_frame,
				bool resend);

		/**
		 * Handle the receipt and processing of a received response for a command sent.
		 */
		int process_resp(	void *&notification_id,
					struct jdksavdecc_frame *ether_frame);

		/**
		 * Check timeout for the inflight commands.
		 */
		void tick();

		/**
		 * Update inflight command for the response received.
		 */
		int update_inflight_for_rcvd_resp(void *&notification_id, uint32_t msg_type, bool u_field, struct jdksavdecc_frame *ether_frame);

		/**
		 * Call notification or post_log_msg callback function for the command sent or response received.
		 */
		int callback(void *notification_id, uint32_t notification_flag, uint8_t *frame);

		/**
		 * Check if the command with the corresponding sequence id is already in the inflight command vector.
		 */
		bool find_inflight_cmd_by_seq_id(uint16_t seq_id, int *inflight_index);

		/**
		 * Check if the command with the corresponding notification id is already in the inflight command vector.
		 */
		bool notification_id_is_inflight(void *notification_id);

		/**
		 * Remove the command from the inflight command vector.
		 */
		int remove_inflight_cmd(uint32_t inflight_cmd_index);

	private:
		std::vector<inflight> inflight_cmds;

		/**
		 * State machine TIMEOUT operation.
		 */
		void state_TIMEOUT(inflight &inflight_command);

		/**
		 * State machine COMMAND operation.
		 */
		void state_COMMAND(void *notification_id, uint32_t notification_flag, struct jdksavdecc_frame *ether_frame);

		/**
		 * State machine RESPONSE operation
		 */
		void acmp::state_RESPONSE(void *&notification_id, struct jdksavdecc_frame *ether_frame);

	};

	extern acmp *acmp;
}

#endif
