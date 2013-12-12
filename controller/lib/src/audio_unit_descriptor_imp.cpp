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

#include <vector>
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "aem_controller_state_machine.h"
#include "audio_unit_descriptor_imp.h"

namespace avdecc_lib
{
    audio_unit_descriptor_imp::audio_unit_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        desc_audio_read_returned = jdksavdecc_descriptor_audio_read(&audio_unit_desc, frame, pos, frame_len);

        if(desc_audio_read_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, audio_unit_desc_read error", end_station_obj->guid());
            assert(desc_audio_read_returned >= 0);
        }

        sampling_rates_init(frame);

        memset(&aem_cmd_set_sampling_rate_resp, 0, sizeof(struct jdksavdecc_aem_command_set_sampling_rate_response));
        memset(&aem_cmd_get_sampling_rate_resp, 0, sizeof(struct jdksavdecc_aem_command_get_sampling_rate_response));
    }

    audio_unit_descriptor_imp::~audio_unit_descriptor_imp() {}

    uint16_t STDCALL audio_unit_descriptor_imp::descriptor_type() const
    {
        assert(audio_unit_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT);
        return audio_unit_desc.descriptor_type;
    }

    void audio_unit_descriptor_imp::sampling_rates_init(const uint8_t *frame)
    {
        uint16_t offset = 0;
        uint32_t sampling_rate = 0;

        for(uint32_t i = 0; i < sampling_rates_count(); i++)
        {
            sampling_rate = jdksavdecc_uint32_get(frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN + sampling_rates_offset() + offset);
            sample_rates_vec.push_back(sampling_rate);
            offset += 0x4;
        }
    }

    uint16_t STDCALL audio_unit_descriptor_imp::descriptor_index() const
    {
        return audio_unit_desc.descriptor_index;
    }

    uint8_t * STDCALL audio_unit_descriptor_imp::object_name()
    {
        return audio_unit_desc.object_name.value;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::localized_description()
    {
        return audio_unit_desc.localized_description;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::clock_domain_index()
    {
        return audio_unit_desc.clock_domain_index;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_stream_input_ports()
    {
        return audio_unit_desc.number_of_stream_input_ports;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_stream_input_port()
    {
        return audio_unit_desc.base_stream_input_port;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_stream_output_ports()
    {
        return audio_unit_desc.number_of_stream_output_ports;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_stream_output_port()
    {
        return audio_unit_desc.base_stream_output_port;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_external_input_ports()
    {
        return audio_unit_desc.number_of_external_input_ports;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_external_input_port()
    {
        return audio_unit_desc.base_external_input_port;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_external_output_ports()
    {
        return audio_unit_desc.number_of_external_output_ports;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_external_output_port()
    {
        return audio_unit_desc.base_external_output_port;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_internal_input_ports()
    {
        return audio_unit_desc.number_of_internal_input_ports;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_internal_input_port()
    {
        return audio_unit_desc.base_internal_input_port;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_internal_output_ports()
    {
        return audio_unit_desc.number_of_internal_output_ports;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_internal_output_port()
    {
        return audio_unit_desc.base_internal_output_port;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_controls()
    {
        return audio_unit_desc.number_of_controls;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_control()
    {
        return audio_unit_desc.base_control;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_signal_selectors()
    {
        return audio_unit_desc.number_of_signal_selectors;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_signal_selector()
    {
        return audio_unit_desc.base_signal_selector;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_mixers()
    {
        return audio_unit_desc.number_of_mixers;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_mixer()
    {
        return audio_unit_desc.base_mixer;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_matrices()
    {
        return audio_unit_desc.number_of_matrices;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_matrix()
    {
        return audio_unit_desc.base_matrix;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_splitters()
    {
        return audio_unit_desc.number_of_splitters;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_splitter()
    {
        return audio_unit_desc.base_splitter;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_combiners()
    {
        return audio_unit_desc.number_of_combiners;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_combiner()
    {
        return audio_unit_desc.base_combiner;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_demultiplexers()
    {
        return audio_unit_desc.number_of_demultiplexers;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_demultiplexer()
    {
        return audio_unit_desc.base_demultiplexer;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_multiplexers()
    {
        return audio_unit_desc.number_of_multiplexers;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_multiplexer()
    {
        return audio_unit_desc.base_multiplexer;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_transcoders()
    {
        return audio_unit_desc.number_of_transcoders;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_transcoder()
    {
        return audio_unit_desc.base_transcoder;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::number_of_control_blocks()
    {
        return audio_unit_desc.number_of_control_blocks;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::base_control_block()
    {
        return audio_unit_desc.base_control_block;
    }

    uint32_t STDCALL audio_unit_descriptor_imp::current_sampling_rate()
    {
        return audio_unit_desc.current_sampling_rate;
    }

    uint32_t STDCALL audio_unit_descriptor_imp::get_sampling_rate_by_index(size_t sampling_rate_index)
    {
        return sample_rates_vec.at(sampling_rate_index);
    }

    uint16_t audio_unit_descriptor_imp::sampling_rates_offset()
    {
        return audio_unit_desc.sampling_rates_offset;
    }

    uint16_t STDCALL audio_unit_descriptor_imp::sampling_rates_count()
    {
        return audio_unit_desc.sampling_rates_count;
    }

    uint32_t STDCALL audio_unit_descriptor_imp::set_sampling_rate_sampling_rate()
    {
        return aem_cmd_set_sampling_rate_resp.sampling_rate;
    }

    uint32_t STDCALL audio_unit_descriptor_imp::get_sampling_rate_sampling_rate()
    {
        return aem_cmd_get_sampling_rate_resp.sampling_rate;
    }


    int STDCALL audio_unit_descriptor_imp::send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_set_sampling_rate aem_cmd_set_sampling_rate;
        ssize_t aem_cmd_set_sampling_rate_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /******************************************* AECP Common Data **********************************************/
        aem_cmd_set_sampling_rate.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
        aem_cmd_set_sampling_rate.command_type = JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE;

        /******************** AECP Message Specific Data *******************/
        aem_cmd_set_sampling_rate.descriptor_type = descriptor_type();
        aem_cmd_set_sampling_rate.descriptor_index = descriptor_index();
        aem_cmd_set_sampling_rate.sampling_rate = new_sampling_rate;

        /******************************** Fill frame payload with AECP data and send the frame ***************************/
        aem_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), cmd_frame);
        aem_cmd_set_sampling_rate_returned = jdksavdecc_aem_command_set_sampling_rate_write(&aem_cmd_set_sampling_rate,
                                                                                            cmd_frame->payload,
                                                                                            ETHER_HDR_SIZE,
                                                                                            sizeof(cmd_frame->payload));

        if(aem_cmd_set_sampling_rate_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_write error\n");
            assert(aem_cmd_set_sampling_rate_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, base_end_station_imp_ref->guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;

    }

    int audio_unit_descriptor_imp::proc_set_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        ssize_t aem_cmd_set_sampling_rate_resp_returned;
        uint32_t msg_type;
        bool u_field;

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);

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

        msg_type = aem_cmd_set_sampling_rate_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_set_sampling_rate_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_set_sampling_rate_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

        free(cmd_frame);
        return 0;
    }

    int STDCALL audio_unit_descriptor_imp::send_get_sampling_rate_cmd(void *notification_id)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_get_sampling_rate aem_cmd_get_sampling_rate;
        ssize_t aem_cmd_get_sampling_rate_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /******************************************** AECP Common Data *********************************************/
        aem_cmd_get_sampling_rate.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
        aem_cmd_get_sampling_rate.command_type = JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE;

        /****************** AECP Message Specific Data *****************/
        aem_cmd_get_sampling_rate.descriptor_type = descriptor_type();
        aem_cmd_get_sampling_rate.descriptor_index = descriptor_index();

        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aem_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), cmd_frame);
        aem_cmd_get_sampling_rate_returned = jdksavdecc_aem_command_get_sampling_rate_write(&aem_cmd_get_sampling_rate,
                                                                                            cmd_frame->payload,
                                                                                            ETHER_HDR_SIZE,
                                                                                            sizeof(cmd_frame->payload));

        if(aem_cmd_get_sampling_rate_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_write error\n");
            assert(aem_cmd_get_sampling_rate_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, base_end_station_imp_ref->guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;
    }


    int audio_unit_descriptor_imp::proc_get_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        ssize_t aem_cmd_get_sampling_rate_resp_returned;
        uint32_t msg_type;
        bool u_field;

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);

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

        msg_type = aem_cmd_get_sampling_rate_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_get_sampling_rate_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_get_sampling_rate_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

        free(cmd_frame);
        return 0;
    }
}
