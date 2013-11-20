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
#include "notification_imp.h"
#include "log_imp.h"
#include "adp.h"
#include "adp_discovery_state_machine.h"

namespace avdecc_lib
{
    adp_discovery_state_machine *adp_discovery_state_machine_ref = new adp_discovery_state_machine(); // To have one ADP Discovery State Machine for all end stations

    uint16_t adp_discovery_state_machine::adp_seq_id = 0x0;

    adp_discovery_state_machine::adp_discovery_state_machine()
    {
        rcvd_avail = false;
        rcvd_departing = false;
        do_discover = false;
        do_terminate = false;
    }

    adp_discovery_state_machine::~adp_discovery_state_machine() {}

    int adp_discovery_state_machine::perform_discover(uint64_t entity_id)
    {
        do_discover = true;
        discover_id = entity_id;

        return 0;
    }

    int adp_discovery_state_machine::tx_discover(struct jdksavdecc_frame *ether_frame)
    {
        int send_frame_returned;
        send_frame_returned = net_interface_ref->send_frame(ether_frame->payload, ether_frame->length); // Send the frame with message information

        if(send_frame_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "netif_send_frame error");
            assert(send_frame_returned >= 0);
        }

        return 0;
    }

    bool adp_discovery_state_machine::have_entity(uint64_t entity_id, uint32_t *entity_index)
    {
        for(uint32_t i = 0; i < entities_vec.size(); i++)
        {
            if(entities_vec.at(i).entity_id == entity_id)
            {
                *entity_index = i;
                return true;
            }
        }

        return false;
    }

    int adp_discovery_state_machine::update_entity_timeout(uint32_t entity_index, uint32_t timeout_ms)
    {
        entities_vec.at(entity_index).inflight_timer.start(timeout_ms);
        return 0;
    }

    int adp_discovery_state_machine::add_entity(struct entities new_entity)
    {
        entities_vec.push_back(new_entity);
        return 0;
    }

    int adp_discovery_state_machine::remove_entity(uint32_t entity_index)
    {
        entities_vec.erase(entities_vec.begin() + entity_index);
        return 0;
    }

    void adp_discovery_state_machine::state_waiting(const uint8_t *frame, uint16_t frame_len)
    {
	if(!do_terminate)
	{
		if(do_discover)
		{
		    state_discover();
		}
		else if(rcvd_avail)
		{
		    state_avail(frame, frame_len);
		}
		else if(rcvd_departing)
		{
		    state_departing();
		}
		else {}
	}
    }

    int adp_discovery_state_machine::state_discover()
    {
        struct jdksavdecc_frame ether_frame;
        adp::ether_frame_init(&ether_frame);
        adp::common_hdr_init(&ether_frame, NULL);
        tx_discover(&ether_frame);
        do_discover = false;

        return 0;
    }

    int adp_discovery_state_machine::state_avail(const uint8_t *frame, uint16_t frame_len)
    {
        struct jdksavdecc_adpdu_common_control_header adp_hdr;
        uint64_t entity_guid;
        uint32_t entity_index;

        entity_guid = jdksavdecc_uint64_get(frame, adp::ETHER_HDR_SIZE + adp::PROTOCOL_HDR_SIZE);
        jdksavdecc_adpdu_common_control_header_read(&adp_hdr, frame, adp::ETHER_HDR_SIZE, frame_len);

        if(have_entity(entity_guid, &entity_index))
        {
            update_entity_timeout(entity_index, adp_hdr.valid_time*2*1000);
        }
        else
        {
            struct entities entity;
            entity.entity_id = entity_guid;
            entity.inflight_timer.start(adp_hdr.valid_time*2*1000);
            add_entity(entity);
            notification_imp_ref->post_notification_msg(END_STATION_CONNECTED, entity_guid, 0, 0, 0, 0);
        }

        rcvd_avail = false;
        return 0;
    }

    int adp_discovery_state_machine::state_departing()
    {
	log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "state_departing is not implemented.");

	return 0;
    }

    int adp_discovery_state_machine::state_timeout(uint32_t entity_index)
    {
        remove_entity(entity_index);
        return 0;
    }

    bool adp_discovery_state_machine::adp_discovery_tick(uint64_t &end_station_guid)
    {
        for(uint32_t i = 0; i < entities_vec.size(); i++)
        {
            if(entities_vec.at(i).inflight_timer.timeout())
            {
                end_station_guid = entities_vec.at(i).entity_id;
                state_timeout(i);
                notification_imp_ref->post_notification_msg(END_STATION_DISCONNECTED, end_station_guid, 0, 0, 0, 0);
                return true;
            }
        }

        return false;
    }
}
