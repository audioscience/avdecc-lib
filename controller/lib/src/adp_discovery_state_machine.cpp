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
#include <inttypes.h>

#include "net_interface_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "util_imp.h"
#include "adp.h"
#include "adp_discovery_state_machine.h"

namespace avdecc_lib
{
    adp_discovery_state_machine *adp_discovery_state_machine_ref = new adp_discovery_state_machine(); // To have one ADP Discovery State Machine for all end stations

    adp_discovery_state_machine::adp_discovery_state_machine()
    {
        first_tick = true;
    }

    adp_discovery_state_machine::~adp_discovery_state_machine() {}

    int adp_discovery_state_machine::ether_frame_init(struct jdksavdecc_frame *cmd_frame)
    {
        /*** Offset to write the field to ***/
        size_t ether_frame_pos = 0;
        jdksavdecc_frame_init(cmd_frame);

        /********************************************************** Ethernet Frame **********************************************************/
        cmd_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
        utility->convert_uint64_to_eui48(net_interface_ref->mac_addr(), cmd_frame->src_address.value); // Send from the Controller MAC address
        cmd_frame->dest_address = jdksavdecc_multicast_adp_acmp; // Send to the ADP multicast destination MAC address
        cmd_frame->length = ADP_FRAME_LEN; // Length of ADP packet is 82 bytes

        /****************** Fill frame payload with Ethernet frame information ****************/
        jdksavdecc_frame_write(cmd_frame, cmd_frame->payload, ether_frame_pos, ETHER_HDR_SIZE);

        return 0;
    }

    void adp_discovery_state_machine::common_hdr_init(struct jdksavdecc_frame *cmd_frame, uint64_t target_entity_id)
    {
        struct jdksavdecc_adpdu_common_control_header adpdu_common_ctrl_hdr;
        ssize_t adpdu_common_ctrl_hdr_returned;

        /********************************** 1722 Protocol Header ***********************************/
        adpdu_common_ctrl_hdr.cd = 1;
        adpdu_common_ctrl_hdr.subtype = JDKSAVDECC_SUBTYPE_ADP;
        adpdu_common_ctrl_hdr.sv = 0;
        adpdu_common_ctrl_hdr.version = 0;
        adpdu_common_ctrl_hdr.message_type = 2;
        adpdu_common_ctrl_hdr.valid_time = 0;
        adpdu_common_ctrl_hdr.control_data_length = 56;
        jdksavdecc_uint64_write(target_entity_id, &adpdu_common_ctrl_hdr.entity_id, 0, sizeof(uint64_t));

        /******************** Fill frame payload with AECP Common Control Header information ********************/
        adpdu_common_ctrl_hdr_returned = jdksavdecc_adpdu_common_control_header_write(&adpdu_common_ctrl_hdr,
                                                                                      cmd_frame->payload,
                                                                                      ETHER_HDR_SIZE,
                                                                                      sizeof(cmd_frame->payload));

        if(adpdu_common_ctrl_hdr_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "adpdu_common_ctrl_hdr_write error");
            assert(adpdu_common_ctrl_hdr_returned >= 0);
        }
    }

    int adp_discovery_state_machine::perform_discover(uint64_t entity_id)
    {
        return state_discover(entity_id);
    }

    int adp_discovery_state_machine::tx_discover(struct jdksavdecc_frame *cmd_frame)
    {
        int send_frame_returned;
        send_frame_returned = net_interface_ref->send_frame(cmd_frame->payload, cmd_frame->length); // Send the frame with message information

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

    int adp_discovery_state_machine::state_discover(uint64_t discover_id)
    {
        struct jdksavdecc_frame cmd_frame;
        ether_frame_init(&cmd_frame);
        common_hdr_init(&cmd_frame, discover_id);
        
        return tx_discover(&cmd_frame);

    }

    int adp_discovery_state_machine::state_avail(const uint8_t *frame, size_t frame_len)
    {
        struct jdksavdecc_adpdu_common_control_header adp_hdr;
        uint64_t entity_entity_id;
        uint32_t entity_index;

        entity_entity_id = jdksavdecc_uint64_get(frame, ETHER_HDR_SIZE + PROTOCOL_HDR_SIZE);
        jdksavdecc_adpdu_common_control_header_read(&adp_hdr, frame, ETHER_HDR_SIZE, frame_len);

        if(have_entity(entity_entity_id, &entity_index))
        {
            update_entity_timeout(entity_index, adp_hdr.valid_time * 2 * 1000); // Valid time period is between 2 and 62 seconds
        }
        else
        {
            struct entities entity;
            entity.entity_id = entity_entity_id;
            entity.inflight_timer.start(adp_hdr.valid_time * 2 * 1000); // Valid time period is between 2 and 62 seconds
            add_entity(entity);
            notification_imp_ref->post_notification_msg(END_STATION_CONNECTED, entity_entity_id, 0, 0, 0, 0, 0);
        }

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

    bool adp_discovery_state_machine::tick(uint64_t &end_station_entity_id)
    {
        if (first_tick)
        {
            state_discover(0);
            first_tick = false;
        }

        for(uint32_t i = 0; i < entities_vec.size(); i++)
        {
            if(entities_vec.at(i).inflight_timer.timeout())
            {
                end_station_entity_id = entities_vec.at(i).entity_id;
                state_timeout(i);
                notification_imp_ref->post_notification_msg(END_STATION_DISCONNECTED, end_station_entity_id, 0, 0, 0, 0, 0);
                return true;
            }
        }

        return false;
    }
}
