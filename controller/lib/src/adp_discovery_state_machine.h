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
 * adp_discovery_state_machine.h
 *
 * ADP Discovery State Machine class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_ADP_DISCOVERY_STATE_MACHINE_H_
#define _AVDECC_CONTROLLER_LIB_ADP_DISCOVERY_STATE_MACHINE_H_

#include "timer.h"

namespace avdecc_lib
{
	class adp_discovery_state_machine
	{
	private:
		struct adp_discovery_state_machine_entities
		{
			uint64_t entity_id;
			timer timer_ref;
		};

		struct adp_discovery_state_machine_vars
		{
			bool rcvd_avail;
			bool rcvd_departing;
			bool do_discover;
			uint64_t discover_id;
			std::vector<struct adp_discovery_state_machine_entities> entities_vector;
			bool do_terminate;
		};

		struct adp_discovery_state_machine_vars discovery_state_machine_vars;

	public:
		static uint16_t adp_seq_id; // The sequence id used for identifying the ADP command that a response is for

		adp_discovery_state_machine();

		~adp_discovery_state_machine();

		/**
		 * Trigger an AVDECC Entity discovery search.
		 */
		int adp_discovery_perform_discover(uint64_t entity_id);

		/**
		 * Transmit an ENTITY_DISCOVER message.
		 */
		int adp_discovery_tx_discover(struct jdksavdecc_frame *ether_frame);

		/**
		 * Check if an AVDECC Entity is present in the entities variable.
		 */
		bool adp_discovery_have_entity(uint64_t entity_id, uint32_t *entity_index);

		/**
		 * Update the AVDECC Entity record timeout information.
		 */
		int adp_discovery_update_entity_timeout(uint32_t entity_index, uint32_t timeout_ms);

		/**
		 * Add a new Entity record to the entities variable.
		 */
		int adp_discovery_add_entity(struct adp_discovery_state_machine_entities new_entity);

		/**
		 * Remove an Entity record form the entities variable.
		 */
		int adp_discovery_remove_entity(uint32_t entity_index);

		/**
		 * Process the Waiting state of the ADP Discovery State Machine.
		 */
		void adp_discovery_state_waiting(const uint8_t *frame, uint16_t frame_len);

		/**
		 * Process the Discover state of the ADP Discovery State Machine.
		 */
		int adp_discovery_state_discover();

		/**
		 * Process the Available state of the ADP Discovery State Machine.
		 */
		int adp_discovery_state_avail(const uint8_t *frame, uint16_t frame_len);

		/**
		 * Process the Departing state of the ADP Discovery State Machine.
		 */
		int adp_discovery_state_departing();

		/**
		 * Process the Timeout state of the ADP Discovery State Machine.
		 */
		int adp_discovery_state_timeout(uint32_t entity_index);

		/**
		 * Get the rcvd_avail field from the adp_discovery_state_machine_vars structure.
		 */
		inline bool get_rcvd_avail()
		{
			return discovery_state_machine_vars.rcvd_avail;
		}

		/**
		 * Update the rcvd_avail field in the adp_discovery_state_machine_vars structure.
		 */
		inline void set_rcvd_avail(bool new_rcvd_avail)
		{
			discovery_state_machine_vars.rcvd_avail = new_rcvd_avail;
		}

		/**
		 * Get the rcvd_departing field from the adp_discovery_state_machine_vars structure.
		 */
		inline bool get_rcvd_departing()
		{
			return discovery_state_machine_vars.rcvd_departing;
		}

		/**
		 * Update the rcvd_departing field in the adp_discovery_state_machine_vars structure.
		 */
		inline void set_rcvd_departing(bool new_rcvd_departing)
		{
			discovery_state_machine_vars.rcvd_departing = new_rcvd_departing;
		}

		/**
		 * Get the do_discover field from the adp_discovery_state_machine_vars structure.
		 */
		inline bool get_do_discover()
		{
			return discovery_state_machine_vars.do_discover;
		}

		/**
		 * Update the do_discover field in the adp_discovery_state_machine_vars structure.
		 */
		inline void set_do_discover(bool new_do_discover)
		{
			discovery_state_machine_vars.do_discover = new_do_discover;
		}

		/**
		 * Get the do_terminate field from the adp_discovery_state_machine_vars structure.
		 */
		inline bool get_do_terminate()
		{
			return discovery_state_machine_vars.do_terminate;
		}

		/**
		 * Update the do_terminate field in the adp_discovery_state_machine_vars structure.
		 */
		inline void set_do_terminate(bool new_do_terminate)
		{
			discovery_state_machine_vars.do_terminate = new_do_terminate;
		}

		/**
		 * Check timeout for the end stations.
		 */
		bool adp_discovery_tick(uint64_t &end_station_guid);
	};

	extern adp_discovery_state_machine *adp_discovery_state_machine_ref;
}

#endif
