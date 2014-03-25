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
 * audio_unit_descriptor_imp.h
 *
 * AUDIO UNIT descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "audio_unit_descriptor.h"

namespace avdecc_lib
{
    class audio_unit_descriptor_imp : public audio_unit_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_audio audio_unit_desc; // Structure containing the audio_unit_desc fields

        std::vector<uint32_t> sample_rates_vec; // Store sample rates information
        struct jdksavdecc_aem_command_set_sampling_rate_response aem_cmd_set_sampling_rate_resp; // Store the response received after sending a SET_SAMPLING_RATE command.
        struct jdksavdecc_aem_command_get_sampling_rate_response aem_cmd_get_sampling_rate_resp; // Store the response received after sending a GET_SAMPLING_RATE command.

    public:
        audio_unit_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~audio_unit_descriptor_imp();

        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        uint8_t * STDCALL object_name();
        uint16_t STDCALL localized_description();
        uint16_t STDCALL clock_domain_index();
        uint16_t STDCALL number_of_stream_input_ports();
        uint16_t STDCALL base_stream_input_port();
        uint16_t STDCALL number_of_stream_output_ports();
        uint16_t STDCALL base_stream_output_port();
        uint16_t STDCALL number_of_external_input_ports();
        uint16_t STDCALL base_external_input_port();
        uint16_t STDCALL number_of_external_output_ports();
        uint16_t STDCALL base_external_output_port();
        uint16_t STDCALL number_of_internal_input_ports();
        uint16_t STDCALL base_internal_input_port();
        uint16_t STDCALL number_of_internal_output_ports();
        uint16_t STDCALL base_internal_output_port();
        uint16_t STDCALL number_of_controls();
        uint16_t STDCALL base_control();
        uint16_t STDCALL number_of_signal_selectors();
        uint16_t STDCALL base_signal_selector();
        uint16_t STDCALL number_of_mixers();
        uint16_t STDCALL base_mixer();
        uint16_t STDCALL number_of_matrices();
        uint16_t STDCALL base_matrix();
        uint16_t STDCALL number_of_splitters();
        uint16_t STDCALL base_splitter();
        uint16_t STDCALL number_of_combiners();
        uint16_t STDCALL base_combiner();
        uint16_t STDCALL number_of_demultiplexers();
        uint16_t STDCALL base_demultiplexer();
        uint16_t STDCALL number_of_multiplexers();
        uint16_t STDCALL base_multiplexer();
        uint16_t STDCALL number_of_transcoders();
        uint16_t STDCALL base_transcoder();
        uint16_t STDCALL number_of_control_blocks();
        uint16_t STDCALL base_control_block();
        uint32_t STDCALL current_sampling_rate();
        uint32_t STDCALL get_sampling_rate_by_index(size_t sampling_rate_index);
        uint16_t sampling_rates_offset();
        uint16_t STDCALL sampling_rates_count();
        uint32_t STDCALL set_sampling_rate_sampling_rate();
        uint32_t STDCALL get_sampling_rate_sampling_rate();
        int STDCALL send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate);
        int proc_set_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        int STDCALL send_get_sampling_rate_cmd(void *notification_id);
        int proc_get_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

    private:
        /**
         * Store the sampling rates of the AUDIO UNIT in a vector.
         */
        void sampling_rates_init(const uint8_t *frame);

        /**
         * Update the internal AUDIO UNIT's sampling rate field.
         */
        void update_sampling_rate(uint32_t sampling_rate);
    };
}

