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
 * clock_domain_descriptor_imp.cpp
 *
 * CLOCK DOMAIN descriptor implementation
 */

#include <vector>
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "aem_controller_state_machine.h"
#include "clock_domain_descriptor_imp.h"

namespace avdecc_lib
{
    clock_domain_descriptor_imp::clock_domain_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        desc_clock_domain_read_returned = jdksavdecc_descriptor_clock_domain_read(&clock_domain_desc, frame, pos, frame_len);

        if(desc_clock_domain_read_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, clock_domain_desc_read error", end_station_obj->guid());
            assert(desc_clock_domain_read_returned >= 0);
        }

        store_clock_sources(frame, pos);
        memset(&aem_cmd_set_clk_src_resp, 0, sizeof(struct jdksavdecc_aem_command_set_clock_source_response));
        memset(&aem_cmd_get_clk_src_resp, 0, sizeof(struct jdksavdecc_aem_command_get_clock_source_response));
    }

    clock_domain_descriptor_imp::~clock_domain_descriptor_imp() {}

    uint16_t STDCALL clock_domain_descriptor_imp::descriptor_type() const
    {
        assert(clock_domain_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN);
        return clock_domain_desc.descriptor_type;
    }

    uint16_t STDCALL clock_domain_descriptor_imp::descriptor_index() const
    {
        return clock_domain_desc.descriptor_index;
    }

    uint8_t * STDCALL clock_domain_descriptor_imp::object_name()
    {
        return clock_domain_desc.object_name.value;
    }

    uint16_t STDCALL clock_domain_descriptor_imp::localized_description()
    {
        return clock_domain_desc.localized_description;
    }

    uint16_t STDCALL clock_domain_descriptor_imp::clock_source_index()
    {
        return clock_domain_desc.clock_source_index;
    }

    uint16_t clock_domain_descriptor_imp::clock_sources_offset()
    {
        assert(clock_domain_desc.clock_sources_offset == 76);
        return clock_domain_desc.clock_sources_offset;
    }

    uint16_t STDCALL clock_domain_descriptor_imp::clock_sources_count()
    {
        assert(clock_domain_desc.clock_sources_count <= 249);
        return clock_domain_desc.clock_sources_count;
    }

    void clock_domain_descriptor_imp::store_clock_sources(const uint8_t *frame, size_t pos)
    {
        uint16_t offset = 0;

        for(uint32_t i = 0; i < clock_sources_count(); i++)
        {
            clk_src_vec.push_back(jdksavdecc_uint16_get(frame, clock_sources_offset() + pos + offset));
            offset += 0x2;
        }
    }

    uint16_t STDCALL clock_domain_descriptor_imp::get_clock_source_by_index(size_t clk_src_index)
    {
        return clk_src_vec.at(clk_src_index);
    }

    uint16_t STDCALL clock_domain_descriptor_imp::set_clock_source_clock_source_index()
    {
        return aem_cmd_set_clk_src_resp.clock_source_index;
    }

    uint16_t STDCALL clock_domain_descriptor_imp::get_clock_source_clock_source_index()
    {
        return aem_cmd_get_clk_src_resp.clock_source_index;
    }

    int STDCALL clock_domain_descriptor_imp::send_set_clock_source_cmd(void *notification_id, uint16_t new_clk_src_index)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_set_clock_source aem_cmd_set_clk_src;
        ssize_t aem_cmd_set_clk_src_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /***************************************** AECP Common Data ******************************************/
        aem_cmd_set_clk_src.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
        // Fill aem_cmd_set_clk_src.sequence_id in AEM Controller State Machine
        aem_cmd_set_clk_src.command_type = JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE;

        /***************** AECP Message Specific Data ****************/
        aem_cmd_set_clk_src.descriptor_type = descriptor_type();
        aem_cmd_set_clk_src.descriptor_index = descriptor_index();
        aem_cmd_set_clk_src.clock_source_index = new_clk_src_index;

        /*************************** Fill frame payload with AECP data and send the frame ***********************/
        aem_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), cmd_frame);
        aem_cmd_set_clk_src_returned = jdksavdecc_aem_command_set_clock_source_write(&aem_cmd_set_clk_src,
                                                                                     cmd_frame->payload,
                                                                                     ETHER_HDR_SIZE,
                                                                                     sizeof(cmd_frame->payload));

        if(aem_cmd_set_clk_src_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_clk_src_write error\n");
            assert(aem_cmd_set_clk_src_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, base_end_station_imp_ref->guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;

    }

    int clock_domain_descriptor_imp::proc_set_clock_source_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        ssize_t aem_cmd_set_clk_src_resp_returned;
        uint32_t msg_type;
        bool u_field;

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);

        aem_cmd_set_clk_src_resp_returned = jdksavdecc_aem_command_set_clock_source_response_read(&aem_cmd_set_clk_src_resp,
                                                                                                  frame,
                                                                                                  ETHER_HDR_SIZE,
                                                                                                  frame_len);

        if(aem_cmd_set_clk_src_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_clk_src_resp_read error\n");
            assert(aem_cmd_set_clk_src_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_set_clk_src_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_set_clk_src_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_set_clk_src_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

        free(cmd_frame);
        return 0;
    }

    int STDCALL clock_domain_descriptor_imp::send_get_clock_source_cmd(void *notification_id)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_get_clock_source aem_cmd_get_clk_src;
        ssize_t aem_cmd_get_clk_src_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /***************************************** AECP Common Data ******************************************/
        aem_cmd_get_clk_src.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
        // Fill aem_cmd_get_clk_src.sequence_id in AEM Controller State Machine
        aem_cmd_get_clk_src.command_type = JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE;

        /****************** AECP Message Specific Data ***************/
        aem_cmd_get_clk_src.descriptor_type = descriptor_type();
        aem_cmd_get_clk_src.descriptor_index = descriptor_index();

        /***************************** Fill frame payload with AECP data and send the frame ***********************/
        aem_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), cmd_frame);
        aem_cmd_get_clk_src_returned = jdksavdecc_aem_command_get_clock_source_write(&aem_cmd_get_clk_src,
                                                                                     cmd_frame->payload,
                                                                                     ETHER_HDR_SIZE,
                                                                                     sizeof(cmd_frame->payload));

        if(aem_cmd_get_clk_src_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_clk_src_write error\n");
            assert(aem_cmd_get_clk_src_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, base_end_station_imp_ref->guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;
    }

    int clock_domain_descriptor_imp::proc_get_clock_source_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        ssize_t aem_cmd_get_clk_src_resp_returned;
        uint32_t msg_type;
        bool u_field;

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);

        aem_cmd_get_clk_src_resp_returned = jdksavdecc_aem_command_get_clock_source_response_read(&aem_cmd_get_clk_src_resp,
                                                                                                  frame,
                                                                                                  ETHER_HDR_SIZE,
                                                                                                  frame_len);

        if(aem_cmd_get_clk_src_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_clk_src_resp_read error\n");
            assert(aem_cmd_get_clk_src_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_get_clk_src_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_get_clk_src_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_get_clk_src_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

        free(cmd_frame);
        return 0;
    }
}
