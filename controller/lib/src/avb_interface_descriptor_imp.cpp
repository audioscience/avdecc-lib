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
 * avb_interface_descriptor_imp.cpp
 *
 * AVB INTERFACE descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "util.h"
#include "end_station_imp.h"
#include "avb_interface_descriptor_imp.h"
#include "aecp_controller_state_machine.h"
#include "adp.h"
#include "system_tx_queue.h"

namespace avdecc_lib
{
    avb_interface_descriptor_imp::avb_interface_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos) {}
    
    avb_interface_descriptor_imp::~avb_interface_descriptor_imp(){}

    avb_interface_descriptor_response * STDCALL avb_interface_descriptor_imp::get_avb_interface_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return resp = new avb_interface_descriptor_response_imp(resp_ref->get_buffer(),
                                                                resp_ref->get_size(), resp_ref->get_pos());
    }

    avb_counters_response * STDCALL avb_interface_descriptor_imp::get_avb_interface_counters_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return counters_resp = new avb_counters_response_imp(resp_ref->get_buffer(),
                                                             resp_ref->get_size(), resp_ref->get_pos());
    }

    uint16_t STDCALL avb_interface_descriptor_imp::descriptor_type() const
    {
        assert(jdksavdecc_descriptor_avb_interface_get_descriptor_type(resp_ref->get_buffer(), resp_ref->get_pos()) == JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE);
        return jdksavdecc_descriptor_avb_interface_get_descriptor_type(resp_ref->get_buffer(), resp_ref->get_pos());
    }

    uint16_t STDCALL avb_interface_descriptor_imp::descriptor_index() const
    {
        return jdksavdecc_descriptor_avb_interface_get_descriptor_index(resp_ref->get_buffer(), resp_ref->get_pos());
    }
    
    int STDCALL avb_interface_descriptor_imp::send_get_counters_cmd(void *notification_id)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_counters aem_cmd_get_counters;
        memset(&aem_cmd_get_counters,0,sizeof(aem_cmd_get_counters));
        ssize_t aem_cmd_get_counters_returned;
        /******************************************** AECP Common Data *********************************************/
        aem_cmd_get_counters.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_counters.sequence_id in AEM Controller State Machine
        aem_cmd_get_counters.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_COUNTERS;
        
        /****************** AECP Message Specific Data *****************/
        aem_cmd_get_counters.descriptor_type = descriptor_type();
        aem_cmd_get_counters.descriptor_index = descriptor_index();
        
        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_COMMAND_LEN);
        aem_cmd_get_counters_returned = jdksavdecc_aem_command_get_counters_write(&aem_cmd_get_counters,
                                                                                         cmd_frame.payload,
                                                                                         ETHER_HDR_SIZE,
                                                                                         sizeof(cmd_frame.payload));
        
        if(aem_cmd_get_counters_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_avb_counters_write error\n");
            assert(aem_cmd_get_counters_returned >= 0);
            return -1;
        }
        
        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           base_end_station_imp_ref->entity_id(),
                                                           JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        
        return 0;
    }

    int avb_interface_descriptor_imp::proc_get_counters_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_counters_response avb_interface_counters_resp;
        ssize_t aem_cmd_get_counters_resp_returned;
        uint32_t msg_type;
        bool u_field;
        
        memcpy(cmd_frame.payload, frame, frame_len);

        aem_cmd_get_counters_resp_returned = jdksavdecc_aem_command_get_counters_response_read(&avb_interface_counters_resp,
                                                                                               frame,
                                                                                               ETHER_HDR_SIZE,
                                                                                               frame_len);
        
        if(aem_cmd_get_counters_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_avb_counters_resp_read error\n");
            assert(aem_cmd_get_counters_resp_returned >= 0);
            return -1;
        }
        
        replace_frame(frame, ETHER_HDR_SIZE, frame_len);
        
        msg_type = avb_interface_counters_resp.aem_header.aecpdu_header.header.message_type;
        status = avb_interface_counters_resp.aem_header.aecpdu_header.header.status;
        u_field = avb_interface_counters_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
        
        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        
        return 0;
    }
}
