/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2014 AudioScience Inc.
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
 * audio_unit_descriptor_response_imp.cpp
 *
 * AUDIO UNIT descriptor response implementation
 */

#include <cstddef>
#include <vector>
#include "enumeration.h"
#include "audio_unit_descriptor_response_imp.h"

namespace avdecc_lib
{
    audio_unit_descriptor_response_imp::audio_unit_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
        descriptor_response_base_imp(frame, frame_len, pos)
    {
        sampling_rates_init(frame);
    }
    
    audio_unit_descriptor_response_imp::~audio_unit_descriptor_response_imp() {}
    
    void audio_unit_descriptor_response_imp::sampling_rates_init(const uint8_t *frame)
    {
        uint16_t offset = ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN + sampling_rates_offset();
        uint32_t sampling_rate = 0;
        
        for(uint32_t i = 0; i < sampling_rates_count(); i++)
        {
            sampling_rate = jdksavdecc_uint32_get(frame, offset);
            sample_rates_vec.push_back(sampling_rate);
            offset += 0x4;
        }
    }
    
    uint8_t * STDCALL audio_unit_descriptor_response_imp::object_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT_OFFSET_OBJECT_NAME];
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::localized_description()
    {
        return jdksavdecc_descriptor_audio_unit_get_localized_description(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::clock_domain_index()
    {
        return jdksavdecc_descriptor_audio_unit_get_clock_domain_index(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_stream_input_ports()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_stream_input_ports(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_stream_input_port()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_stream_input_port(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_stream_output_ports()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_stream_output_ports(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_stream_output_port()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_stream_output_port(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_external_input_ports()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_external_input_ports(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_external_input_port()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_external_input_port(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_external_output_ports()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_external_output_ports(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_external_output_port()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_external_output_port(buffer, position);    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_internal_input_ports()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_internal_input_ports(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_internal_input_port()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_internal_input_port(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_internal_output_ports()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_internal_output_ports(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_internal_output_port()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_internal_output_port(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_controls()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_controls(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_control()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_control(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_signal_selectors()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_signal_selectors(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_signal_selector()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_signal_selector(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_mixers()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_mixers(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_mixer()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_mixer(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_matrices()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_matrices(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_matrix()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_matrix(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_splitters()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_splitters(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_splitter()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_splitter(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_combiners()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_combiners(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_combiner()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_combiner(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_demultiplexers()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_demultiplexers(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_demultiplexer()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_demultiplexer(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_multiplexers()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_multiplexers(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_multiplexer()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_multiplexer(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_transcoders()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_transcoders(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_transcoder()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_transcoder(buffer, position);

    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::number_of_control_blocks()
    {
        return jdksavdecc_descriptor_audio_unit_get_number_of_control_blocks(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::base_control_block()
    {
        return jdksavdecc_descriptor_audio_unit_get_base_control_block(buffer, position);
    }
    
    uint32_t STDCALL audio_unit_descriptor_response_imp::current_sampling_rate()
    {
        return jdksavdecc_descriptor_audio_unit_get_current_sampling_rate(buffer, position);
    }
    
    uint32_t STDCALL audio_unit_descriptor_response_imp::get_sampling_rate_by_index(size_t sampling_rate_index)
    {
        return sample_rates_vec.at(sampling_rate_index);
    }
    
    uint16_t audio_unit_descriptor_response_imp::sampling_rates_offset()
    {
        return jdksavdecc_descriptor_audio_unit_get_sampling_rates_offset(buffer, position);
    }
    
    uint16_t STDCALL audio_unit_descriptor_response_imp::sampling_rates_count()
    {
        return jdksavdecc_descriptor_audio_unit_get_sampling_rates_count(buffer, position);
    }
}
