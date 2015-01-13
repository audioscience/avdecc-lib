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

#include <mutex>
#include <vector>

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "aecp_controller_state_machine.h"
#include "clock_domain_descriptor_imp.h"

namespace avdecc_lib
{
    clock_domain_descriptor_imp::clock_domain_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos)
    {
        memset(&aem_cmd_set_clk_src_resp, 0, sizeof(struct jdksavdecc_aem_command_set_clock_source_response));
    }

    clock_domain_descriptor_imp::~clock_domain_descriptor_imp() {}
    
    clock_domain_descriptor_response * STDCALL clock_domain_descriptor_imp::get_clock_domain_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return resp = new clock_domain_descriptor_response_imp(resp_ref->get_desc_buffer(),
                                                               resp_ref->get_desc_size(), resp_ref->get_desc_pos());
    }

    clock_domain_counters_response * STDCALL clock_domain_descriptor_imp::get_clock_domain_counters_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return counters_resp = new clock_domain_counters_response_imp(resp_ref->get_buffer(),
                                                                      resp_ref->get_size(), resp_ref->get_pos());
    }
    
    clock_domain_get_clock_source_response * STDCALL clock_domain_descriptor_imp::get_clock_domain_get_clock_source_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return clock_source_resp = new clock_domain_get_clock_source_response_imp(resp_ref->get_buffer(),
                                                                                  resp_ref->get_size(), resp_ref->get_pos());
    }

    int STDCALL clock_domain_descriptor_imp::send_set_clock_source_cmd(void *notification_id, uint16_t new_clk_src_index)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_set_clock_source aem_cmd_set_clk_src;
        ssize_t aem_cmd_set_clk_src_returned;
        memset(&aem_cmd_set_clk_src,0,sizeof(aem_cmd_set_clk_src));

        /***************************************** AECP Common Data ******************************************/
        aem_cmd_set_clk_src.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_set_clk_src.sequence_id in AEM Controller State Machine
        aem_cmd_set_clk_src.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE;

        /***************** AECP Message Specific Data ****************/
        aem_cmd_set_clk_src.descriptor_type = descriptor_type();
        aem_cmd_set_clk_src.descriptor_index = descriptor_index();
        aem_cmd_set_clk_src.clock_source_index = new_clk_src_index;

        /*************************** Fill frame payload with AECP data and send the frame ***********************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
						ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE_COMMAND_LEN);
        aem_cmd_set_clk_src_returned = jdksavdecc_aem_command_set_clock_source_write(&aem_cmd_set_clk_src,
                                                                                     cmd_frame.payload,
                                                                                     ETHER_HDR_SIZE,
                                                                                     sizeof(cmd_frame.payload));

        if(aem_cmd_set_clk_src_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_clk_src_write error\n");
            assert(aem_cmd_set_clk_src_returned >= 0);
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                            &cmd_frame,
                                                            base_end_station_imp_ref->entity_id(),
                                                            JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE_COMMAND_LEN - 
                                                            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

        return 0;

    }

    int clock_domain_descriptor_imp::proc_set_clock_source_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        ssize_t aem_cmd_set_clk_src_resp_returned;
        uint32_t msg_type;
        bool u_field;

        memcpy(cmd_frame.payload, frame, frame_len);

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
        u_field = aem_cmd_set_clk_src_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

        return 0;
    }

    int STDCALL clock_domain_descriptor_imp::send_get_clock_source_cmd(void *notification_id)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_clock_source aem_cmd_get_clk_src;
        ssize_t aem_cmd_get_clk_src_returned;
        memset(&aem_cmd_get_clk_src,0,sizeof(aem_cmd_get_clk_src));

        /***************************************** AECP Common Data ******************************************/
        aem_cmd_get_clk_src.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_clk_src.sequence_id in AEM Controller State Machine
        aem_cmd_get_clk_src.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE;

        /****************** AECP Message Specific Data ***************/
        aem_cmd_get_clk_src.descriptor_type = descriptor_type();
        aem_cmd_get_clk_src.descriptor_index = descriptor_index();

        /***************************** Fill frame payload with AECP data and send the frame ***********************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
										ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE_COMMAND_LEN);
        aem_cmd_get_clk_src_returned = jdksavdecc_aem_command_get_clock_source_write(&aem_cmd_get_clk_src,
                                                                                     cmd_frame.payload,
                                                                                     ETHER_HDR_SIZE,
                                                                                     sizeof(cmd_frame.payload));

        if(aem_cmd_get_clk_src_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_clk_src_write error\n");
            assert(aem_cmd_get_clk_src_returned >= 0);
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                            &cmd_frame,
                                                            base_end_station_imp_ref->entity_id(),
                                                            JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE_COMMAND_LEN - 
                                                            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

        return 0;
    }

    int clock_domain_descriptor_imp::proc_get_clock_source_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_clock_source_response aem_cmd_get_clk_src_resp;
        ssize_t aem_cmd_get_clk_src_resp_returned;
        uint32_t msg_type;
        bool u_field;

        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&aem_cmd_get_clk_src_resp, 0, sizeof(jdksavdecc_aem_command_get_clock_source_response));

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

        replace_frame(frame, ETHER_HDR_SIZE, frame_len);
        
        msg_type = aem_cmd_get_clk_src_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_get_clk_src_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_get_clk_src_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

        return 0;
    }
    
    int STDCALL clock_domain_descriptor_imp::send_get_counters_cmd(void *notification_id)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_counters aem_cmd_get_clock_domain_counters;
        memset(&aem_cmd_get_clock_domain_counters,0,sizeof(aem_cmd_get_clock_domain_counters));
        ssize_t aem_cmd_get_clock_domain_counters_returned;
        
        /******************************************** AECP Common Data *********************************************/
        aem_cmd_get_clock_domain_counters.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_counters.sequence_id in AEM Controller State Machine
        aem_cmd_get_clock_domain_counters.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_COUNTERS;
        
        /****************** AECP Message Specific Data *****************/
        aem_cmd_get_clock_domain_counters.descriptor_type = descriptor_type();
        aem_cmd_get_clock_domain_counters.descriptor_index = descriptor_index();
        
        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_COMMAND_LEN);
        aem_cmd_get_clock_domain_counters_returned = jdksavdecc_aem_command_get_counters_write(&aem_cmd_get_clock_domain_counters,
                                                                                         cmd_frame.payload,
                                                                                         ETHER_HDR_SIZE,
                                                                                         sizeof(cmd_frame.payload));
        
        if(aem_cmd_get_clock_domain_counters_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_entity_counters_write error\n");
            assert(aem_cmd_get_clock_domain_counters_returned >= 0);
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
    
    int clock_domain_descriptor_imp::proc_get_counters_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_counters_response clock_domain_counters_resp;
        ssize_t aem_cmd_get_counters_resp_returned;
        uint32_t msg_type;
        bool u_field;
        
        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&clock_domain_counters_resp, 0, sizeof(jdksavdecc_aem_command_get_counters_response));

        aem_cmd_get_counters_resp_returned = jdksavdecc_aem_command_get_counters_response_read(&clock_domain_counters_resp,
                                                                                               frame,
                                                                                               ETHER_HDR_SIZE,
                                                                                               frame_len);
        if(aem_cmd_get_counters_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_clock_domain_counters_resp_read error\n");
            assert(aem_cmd_get_counters_resp_returned >= 0);
            return -1;
        }
        
        replace_frame(frame, ETHER_HDR_SIZE, frame_len);
        
        msg_type = clock_domain_counters_resp.aem_header.aecpdu_header.header.message_type;
        status = clock_domain_counters_resp.aem_header.aecpdu_header.header.status;
        u_field = clock_domain_counters_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
        
        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        
        return 0;
    }
}
