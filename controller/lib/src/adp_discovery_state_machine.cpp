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
 * adp_discovery_state_machine.cpp
 *
 * ADP Discovery State Machine implementation
 */

#include <vector>
#include "net_interface_imp.h"
#include "enumeration.h"
#include "notification.h"
#include "log.h"
#include "adp.h"
#include "adp_discovery_state_machine.h"

namespace avdecc_lib
{
	adp_discovery_state_machine *adp_discovery_state_machine_ref = new adp_discovery_state_machine(); // To have one ADP Discovery State Machine for all end stations

	uint16_t adp_discovery_state_machine::adp_seq_id = 0x0;

	adp_discovery_state_machine::adp_discovery_state_machine()
	{
		discovery_state_machine_vars.rcvd_avail = false;
		discovery_state_machine_vars.rcvd_departing = false;
		discovery_state_machine_vars.do_discover = false;
		discovery_state_machine_vars.do_terminate = false;
	}

	adp_discovery_state_machine::~adp_discovery_state_machine() {}

	int adp_discovery_state_machine::adp_discovery_perform_discover(uint64_t entity_id)
	{
		discovery_state_machine_vars.do_discover = true;
		discovery_state_machine_vars.discover_id = entity_id;
		return 0;
	}

	int adp_discovery_state_machine::adp_discovery_tx_discover(struct jdksavdecc_frame *ether_frame)
	{
		int send_frame_returned;
		send_frame_returned = net_interface_ref->send_frame(ether_frame->payload, ether_frame->length); // Send the frame with message information

		if(send_frame_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "netif_send_frame error");
			assert(send_frame_returned >= 0);
		}

		return 0;
	}

	bool adp_discovery_state_machine::adp_discovery_have_entity(uint64_t entity_id, uint32_t *entity_index)
	{
		for(uint32_t index_i = 0; index_i < discovery_state_machine_vars.entities_vector.size(); index_i++)
		{
			if(discovery_state_machine_vars.entities_vector.at(index_i).entity_id == entity_id)
			{
				*entity_index = index_i;
				return true;
			}
		}

		return false;
	}

	int adp_discovery_state_machine::adp_discovery_update_entity(uint32_t entity_index)
	{
		discovery_state_machine_vars.entities_vector.at(entity_index).avdecc_lib_timer_ref->start(END_STATION_CONNECTION_TIMEOUT);
		return 0;
	}

	int adp_discovery_state_machine::adp_discovery_add_entity(struct adp_discovery_state_machine_entities new_entity)
	{
		discovery_state_machine_vars.entities_vector.push_back(new_entity);
		return 0;
	}

	int adp_discovery_state_machine::adp_discovery_remove_entity(uint32_t entity_index)
	{
		delete discovery_state_machine_vars.entities_vector.at(entity_index).avdecc_lib_timer_ref;
		discovery_state_machine_vars.entities_vector.erase(discovery_state_machine_vars.entities_vector.begin() + entity_index);
		return 0;
	}

	void adp_discovery_state_machine::adp_discovery_state_waiting(uint8_t *frame)
	{
		if(discovery_state_machine_vars.do_discover)
		{
			adp_discovery_state_discover();
		}

		else if(discovery_state_machine_vars.rcvd_avail)
		{
			adp_discovery_state_avail(frame);
		}

		else if(discovery_state_machine_vars.rcvd_departing)
		{
			//adp_discovery_state_departing();
		}
		else {}
	}

	int adp_discovery_state_machine::adp_discovery_state_discover()
	{
		struct jdksavdecc_frame ether_frame;
		adp::ether_frame_init(&ether_frame);
		adp::common_hdr_init(&ether_frame, NULL);
		adp_discovery_tx_discover(&ether_frame);
		discovery_state_machine_vars.do_discover = false;

		return 0;
	}

	int adp_discovery_state_machine::adp_discovery_state_avail(uint8_t *frame)
	{
		uint64_t entity_guid;
		uint32_t entity_index;

		entity_guid = jdksavdecc_uint64_get(frame, adp::ETHER_HDR_SIZE + adp::PROTOCOL_HDR_SIZE);

		if(adp_discovery_have_entity(entity_guid, &entity_index))
		{
			adp_discovery_update_entity(entity_index);
		}

		else
		{
			struct adp_discovery_state_machine_entities entity;
			entity.entity_id = entity_guid;
			entity.avdecc_lib_timer_ref = new timer();
			entity.avdecc_lib_timer_ref ->start(END_STATION_CONNECTION_TIMEOUT);
			adp_discovery_add_entity(entity);
			avdecc_lib::notification_ref->notifying(avdecc_lib::END_STATION_CONNECTED, entity_guid, 0, 0, 0, 0);
		}

		discovery_state_machine_vars.rcvd_avail = false;
		return 0;
	}

	//void adp_discovery_state_machine::adp_discovery_state_departing()
	//{
	//
	//}

	int adp_discovery_state_machine::adp_discovery_state_timeout(uint32_t entity_index)
	{
		adp_discovery_remove_entity(entity_index);
		return 0;
	}

	bool adp_discovery_state_machine::adp_discovery_tick(uint64_t &end_station_guid)
	{
		for(uint32_t index_i = 0; index_i < discovery_state_machine_vars.entities_vector.size(); index_i++)
		{
			if(discovery_state_machine_vars.entities_vector.at(index_i).avdecc_lib_timer_ref->timeout())
			{
				end_station_guid = discovery_state_machine_vars.entities_vector.at(index_i).entity_id;
				adp_discovery_state_timeout(index_i);
				avdecc_lib::notification_ref->notifying(avdecc_lib::END_STATION_DISCONNECTED, end_station_guid, 0, 0, 0, 0);
				return true;
			}
		}

		return false;
	}
}