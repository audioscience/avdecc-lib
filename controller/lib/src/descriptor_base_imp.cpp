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
 * descriptor_base.cpp
 *
 * Descriptor base implementation
 */

#include <iostream>
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "aecp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "aem_controller_state_machine.h"
#include "descriptor_base_imp.h"

namespace avdecc_lib
{
    descriptor_base_imp::descriptor_base_imp(end_station_imp *base)
    {
        base_end_station_imp_ref = base;
    }

    descriptor_base_imp::~descriptor_base_imp() {}

    bool operator== (const descriptor_base_imp &n1, const descriptor_base_imp &n2)
    {
        return n1.get_descriptor_index() == n2.get_descriptor_index();
    }

    bool operator< (const descriptor_base_imp &n1, const descriptor_base_imp &n2)
    {
        return n1.get_descriptor_index() < n2.get_descriptor_index();
    }

    uint16_t STDCALL descriptor_base_imp::get_descriptor_type() const
    {
        return 0;
    }

    uint16_t STDCALL descriptor_base_imp::get_descriptor_index() const
    {
        return 0;
    }

    uint16_t STDCALL descriptor_base_imp::get_localized_description()
    {
        return 0;
    }

    uint8_t * STDCALL descriptor_base_imp::get_object_name()
    {
        return NULL;
    }

    int STDCALL descriptor_base_imp::send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag)
    {

        return 0;
    }

    int descriptor_base_imp::proc_acquire_entity_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
    {

        return 0;
    }

    int descriptor_base_imp::default_send_acquire_entity_cmd(descriptor_base_imp *desc_base_imp_ref, void *notification_id, uint32_t acquire_entity_flag)
    {
        struct jdksavdecc_frame *ether_frame;
        struct jdksavdecc_aem_command_acquire_entity aem_cmd_acquire_entity;
        int aem_cmd_acquire_entity_returned;
        ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /***************************************** AECP Common Data *********************************************/
        aem_cmd_acquire_entity.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
        // Fill aem_cmd_acquire_entity.sequence_id in AEM Controller State Machine
        aem_cmd_acquire_entity.command_type = JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY;

        /****************************** AECP Message Specific Data ******************************/
        aem_cmd_acquire_entity.aem_acquire_flags = acquire_entity_flag;
        jdksavdecc_eui64_init(&aem_cmd_acquire_entity.owner_entity_id);
        aem_cmd_acquire_entity.descriptor_type = desc_base_imp_ref->get_descriptor_type();
        aem_cmd_acquire_entity.descriptor_index = desc_base_imp_ref->get_descriptor_index();

        /***************************** Fill frame payload with AECP data and send the frame ***********************/
        aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
        aem_cmd_acquire_entity_returned = jdksavdecc_aem_command_acquire_entity_write(&aem_cmd_acquire_entity,
                                                                                      ether_frame->payload,
                                                                                      aecp::CMD_POS,
                                                                                      sizeof(ether_frame->payload));

        if(aem_cmd_acquire_entity_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_acquire_entity_write error\n");
            assert(aem_cmd_acquire_entity_returned >= 0);
            return -1;
        }

        aecp::aecpdu_common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

        free(ether_frame);
        return 0;
    }

    int descriptor_base_imp::default_proc_acquire_entity_resp(struct jdksavdecc_aem_command_acquire_entity_response &aem_cmd_acquire_entity_resp,
                                                              void *&notification_id,
                                                              const uint8_t *frame,
                                                              uint16_t frame_len,
                                                              int &status)
    {
        struct jdksavdecc_frame *ether_frame;
        int aem_cmd_acquire_entity_resp_returned;
        uint32_t msg_type;
        bool u_field;

        ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(ether_frame->payload, frame, frame_len);

        aem_cmd_acquire_entity_resp_returned = jdksavdecc_aem_command_acquire_entity_response_read(&aem_cmd_acquire_entity_resp,
                                                                                                   frame,
                                                                                                   aecp::CMD_POS,
                                                                                                   frame_len);

        if(aem_cmd_acquire_entity_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_acquire_entity_resp_read error\n");
            assert(aem_cmd_acquire_entity_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_acquire_entity_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_acquire_entity_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_acquire_entity_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

        free(ether_frame);
        return 0;
    }

    int STDCALL descriptor_base_imp::send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Need to implement send_lock_entity_cmd\n");

        return 0;
    }

    int descriptor_base_imp::proc_lock_entity_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Need to implement proc_lock_entity_resp\n");

        return 0;
    }

    int descriptor_base_imp::default_send_lock_entity_cmd(descriptor_base_imp *descriptor_base_imp_ref, void *notification_id, uint32_t lock_entity_flag)
    {
        struct jdksavdecc_frame *ether_frame;
        struct jdksavdecc_aem_command_lock_entity aem_cmd_lock_entity;
        int aem_cmd_acquire_entity_returned;
        ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /***************************************** AECP Common Data ******************************************/
        aem_cmd_lock_entity.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
        // Fill aem_cmd_lock_entity.sequence_id in AEM Controller State Machine
        aem_cmd_lock_entity.command_type = JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY;

        /****************************** AECP Message Specific Data ****************************/
        aem_cmd_lock_entity.aem_lock_flags = lock_entity_flag;
        jdksavdecc_eui64_init(&aem_cmd_lock_entity.locked_entity_id);
        aem_cmd_lock_entity.descriptor_type = descriptor_base_imp_ref->get_descriptor_type();
        aem_cmd_lock_entity.descriptor_index = descriptor_base_imp_ref->get_descriptor_index();

        /**************************** Fill frame payload with AECP data and send the frame **********************/
        aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
        aem_cmd_acquire_entity_returned = jdksavdecc_aem_command_lock_entity_write(&aem_cmd_lock_entity,
                                                                                   ether_frame->payload,
                                                                                   aecp::CMD_POS,
                                                                                   sizeof(ether_frame->payload));

        if(aem_cmd_acquire_entity_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_lock_entity_write error\n");
            assert(aem_cmd_acquire_entity_returned >= 0);
            return -1;
        }

        aecp::aecpdu_common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

        free(ether_frame);
        return 0;
    }

    int descriptor_base_imp::default_proc_lock_entity_resp(struct jdksavdecc_aem_command_lock_entity_response &aem_cmd_lock_entity_resp,
                                                           void *&notification_id,
                                                           const uint8_t *frame,
                                                           uint16_t frame_len,
                                                           int &status)
    {
        struct jdksavdecc_frame *ether_frame;
        int aem_cmd_lock_entity_resp_returned = 0;
        uint32_t msg_type = 0;
        bool u_field = false;

        ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(ether_frame->payload, frame, frame_len);

        aem_cmd_lock_entity_resp_returned = jdksavdecc_aem_command_lock_entity_response_read(&aem_cmd_lock_entity_resp,
                                                                                             frame,
                                                                                             aecp::CMD_POS,
                                                                                             frame_len);

        if(aem_cmd_lock_entity_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_lock_entity_resp_read error\n");
            assert(aem_cmd_lock_entity_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_lock_entity_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_lock_entity_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_lock_entity_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, ether_frame);

        free(ether_frame);
        return 0;
    }

    int STDCALL descriptor_base_imp::send_set_name_cmd(void *notification_id, uint16_t name_index, uint16_t config_index, char * name)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_NAME command.");

        return 0;
    }

    int descriptor_base_imp::proc_set_name_resp(uint8_t *base_pointer, uint16_t frame_len)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_NAME response.");

        return 0;
    }

    int STDCALL descriptor_base_imp::send_get_name_cmd(void *notification_id, uint16_t name_index, uint16_t config_index)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement GET_NAME command.");

        return 0;
    }

    int descriptor_base_imp::proc_get_name_resp(uint8_t *base_pointer, uint16_t frame_len)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement GET_NAME response.");

        return 0;
    }
}
