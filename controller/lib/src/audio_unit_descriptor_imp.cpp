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
 * audio_unit_descriptor_imp.cpp
 *
 * AUDIO UNIT descriptor implementation
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
#include "audio_unit_descriptor_imp.h"

namespace avdecc_lib
{
    audio_unit_descriptor_imp::audio_unit_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos) {}

    audio_unit_descriptor_imp::~audio_unit_descriptor_imp() {}

    audio_unit_descriptor_response * STDCALL audio_unit_descriptor_imp::get_audio_unit_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return resp = new audio_unit_descriptor_response_imp(resp_ref->get_desc_buffer(),
                                                             resp_ref->get_desc_size(), resp_ref->get_desc_pos());
    }

    audio_unit_get_sampling_rate_response * STDCALL audio_unit_descriptor_imp::get_audio_unit_get_sampling_rate_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return sampling_rate_resp = new audio_unit_get_sampling_rate_response_imp(resp_ref->get_buffer(),
                                                             resp_ref->get_size(), resp_ref->get_pos());
    }

    int STDCALL audio_unit_descriptor_imp::send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_set_sampling_rate aem_cmd_set_sampling_rate;
        ssize_t aem_cmd_set_sampling_rate_returned;
        memset(&aem_cmd_set_sampling_rate, 0, sizeof(aem_cmd_set_sampling_rate));

        /******************************************* AECP Common Data **********************************************/
		aem_cmd_set_sampling_rate.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
		aem_cmd_set_sampling_rate.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE;

        /******************** AECP Message Specific Data *******************/
        aem_cmd_set_sampling_rate.descriptor_type = descriptor_type();
        aem_cmd_set_sampling_rate.descriptor_index = descriptor_index();
        aem_cmd_set_sampling_rate.sampling_rate = new_sampling_rate;

        /******************************** Fill frame payload with AECP data and send the frame ***************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
								ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE_COMMAND_LEN);
        aem_cmd_set_sampling_rate_returned = jdksavdecc_aem_command_set_sampling_rate_write(&aem_cmd_set_sampling_rate,
                                                                                            cmd_frame.payload,
                                                                                            ETHER_HDR_SIZE,
                                                                                            sizeof(cmd_frame.payload));

        if(aem_cmd_set_sampling_rate_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_write error\n");
            assert(aem_cmd_set_sampling_rate_returned >= 0);
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                            &cmd_frame,
                                                            base_end_station_imp_ref->entity_id(),
                                                            JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE_COMMAND_LEN - 
                                                            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

        return 0;

    }

    int audio_unit_descriptor_imp::proc_set_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_set_sampling_rate_response aem_cmd_set_sampling_rate_resp;
        ssize_t aem_cmd_set_sampling_rate_resp_returned;
        uint32_t msg_type;
        bool u_field;
        uint8_t * buffer;

        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&aem_cmd_set_sampling_rate_resp, 0, sizeof(struct jdksavdecc_aem_command_set_sampling_rate_response));

        aem_cmd_set_sampling_rate_resp_returned = jdksavdecc_aem_command_set_sampling_rate_response_read(&aem_cmd_set_sampling_rate_resp,
                                                                                                         frame,
                                                                                                         ETHER_HDR_SIZE,
                                                                                                         frame_len);

        if(aem_cmd_set_sampling_rate_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_resp_read error\n");
            assert(aem_cmd_set_sampling_rate_resp_returned >= 0);
            return -1;
        }
        
        buffer = (uint8_t *)malloc(resp_ref->get_desc_size() * sizeof(uint8_t)); //fetch current desc frame
        memcpy(buffer, resp_ref->get_desc_buffer(), resp_ref->get_desc_size());
        jdksavdecc_descriptor_audio_unit_set_current_sampling_rate(aem_cmd_set_sampling_rate_resp.sampling_rate, buffer, resp_ref->get_desc_pos()); //set clk source

        replace_desc_frame(buffer, resp_ref->get_desc_pos(), resp_ref->get_desc_size()); //replace frame

        msg_type = aem_cmd_set_sampling_rate_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_set_sampling_rate_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_set_sampling_rate_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        
        free(buffer);
        return 0;
    }

    int STDCALL audio_unit_descriptor_imp::send_get_sampling_rate_cmd(void *notification_id)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_sampling_rate aem_cmd_get_sampling_rate;
        ssize_t aem_cmd_get_sampling_rate_returned;
        memset(&aem_cmd_get_sampling_rate,0,sizeof(aem_cmd_get_sampling_rate));

        /******************************************** AECP Common Data *********************************************/
        aem_cmd_get_sampling_rate.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
        aem_cmd_get_sampling_rate.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE;

        /****************** AECP Message Specific Data *****************/
        aem_cmd_get_sampling_rate.descriptor_type = descriptor_type();
        aem_cmd_get_sampling_rate.descriptor_index = descriptor_index();

        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
									ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE_COMMAND_LEN);
        aem_cmd_get_sampling_rate_returned = jdksavdecc_aem_command_get_sampling_rate_write(&aem_cmd_get_sampling_rate,
                                                                                            cmd_frame.payload,
                                                                                            ETHER_HDR_SIZE,
                                                                                            sizeof(cmd_frame.payload));

        if(aem_cmd_get_sampling_rate_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_write error\n");
            assert(aem_cmd_get_sampling_rate_returned >= 0);
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                            &cmd_frame,
                                                            base_end_station_imp_ref->entity_id(),
                                                            JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE_COMMAND_LEN - 
                                                            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

        return 0;
    }

    int audio_unit_descriptor_imp::proc_get_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_sampling_rate_response aem_cmd_get_sampling_rate_resp;
        ssize_t aem_cmd_get_sampling_rate_resp_returned;
        uint32_t msg_type;
        bool u_field;

        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&aem_cmd_get_sampling_rate_resp, 0, sizeof(jdksavdecc_aem_command_get_sampling_rate_response));

        aem_cmd_get_sampling_rate_resp_returned = jdksavdecc_aem_command_get_sampling_rate_response_read(&aem_cmd_get_sampling_rate_resp,
                                                                                                         frame,
                                                                                                         ETHER_HDR_SIZE,
                                                                                                         frame_len);

        if(aem_cmd_get_sampling_rate_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_resp_read error\n");
            assert(aem_cmd_get_sampling_rate_resp_returned >= 0);
            return -1;
        }
        replace_frame(frame, ETHER_HDR_SIZE, frame_len);

        msg_type = aem_cmd_get_sampling_rate_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_get_sampling_rate_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_get_sampling_rate_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

        return 0;
    }
}
