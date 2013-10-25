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
 * Audio Unit descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AUDIO_UNIT_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_AUDIO_UNIT_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "audio_unit_descriptor.h"

namespace avdecc_lib
{
        class audio_unit_descriptor_imp : public virtual audio_unit_descriptor, public virtual descriptor_base_imp
        {
        private:
                struct jdksavdecc_descriptor_audio audio_unit_desc; // Structure containing the audio_unit_desc fields
                int desc_audio_read_returned; // Status of extracting AUDIO_UNIT descriptor information from a network buffer

                struct current_sample_rates
                {
                        uint8_t pull_field;
                        uint32_t base_freq;
                        uint32_t freq;
                };

                std::vector<struct current_sample_rates> sample_rates_vector; // Store sample rates pull field, base frequency, and frequency information
                struct current_sample_rates sample_rates;

        public:
                /**
                 * An empty constructor for audio_unit_descriptor_imp
                 */
                audio_unit_descriptor_imp();

                /**
                 * Constructor for audio_unit_descriptor_imp used for constructing an object with a base End Station, frame, position offest,
                 * and memory buffer length.
                 */
                audio_unit_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

                /**
                 * Destructor for audio_unit_descriptor_imp used for destroying objects
                 */
                virtual ~audio_unit_descriptor_imp();

                /**
                 * Get the descriptor_type of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_descriptor_type();

                /**
                 * Get the descriptor_index of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_descriptor_index();

                /**
                 * Get the object_name of the audio_unit_descriptor object.
                 */
                uint8_t * STDCALL get_object_name();

                /**
                 * Get the localized_description of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_localized_description();

                /**
                 * Get the clock_domain_index of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_clock_domain_index();

                /**
                 * Get the number_of_stream_input_ports of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_stream_input_ports();

                /**
                 * Get the base_stream_input_port of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_stream_input_port();

                /**
                 * Get the number_of_stream_output_ports of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_stream_output_ports();

                /**
                 * Get the base_stream_output_port of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_stream_output_port();

                /**
                 * Get the number_of_external_input_ports of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_external_input_ports();

                /**
                 * Get the base_external_input_port of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_external_input_port();

                /**
                 * Get the number_of_external_output_ports of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_external_output_ports();

                /**
                 * Get the base_external_output_port of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_external_output_port();

                /**
                 * Get the number_of_internal_input_ports of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_internal_input_ports();

                /**
                 * Get the base_internal_input_port of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_internal_input_port();

                /**
                 * Get the number_of_internal_output_ports of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_internal_output_ports();

                /**
                 * Get the base_internal_output_port of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_internal_output_port();

                /**
                 * Get the number_of_controls of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_controls();

                /**
                 * Get the base_control of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_control();

                /**
                 * Get the number_of_signal_selectors of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_signal_selectors();

                /**
                 * Get the base_signal_selector of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_signal_selector();

                /**
                 * Get the number_of_mixers of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_mixers();

                /**
                 * Get the base_mixer of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_mixer();

                /**
                 * Get the number_of_matrices of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_matrices();

                /**
                 * Get the base_matrix of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_matrix();

                /**
                 * Get the number_of_splitters of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_splitters();

                /**
                 * Get the base_splitter of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_splitter();

                /**
                 * Get the number_of_combiners of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_combiners();

                /**
                 * Get the base_combiner of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_combiner();

                /**
                 * Get the number_of_demultiplexers of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_demultiplexers();

                /**
                 * Get the base_demultiplexer of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_demultiplexer();

                /**
                 * Get the number_of_multiplexers of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_multiplexers();

                /**
                 * Get the base_multiplexer of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_multiplexer();

                /**
                 * Get the number_of_transcoders of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_transcoders();

                /**
                 * Get the base_transcoder of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_transcoder();

                /**
                 * Get the number_of_control_blocks of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_number_of_control_blocks();

                /**
                 * Get the base_control_block of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_base_control_block();

                /**
                 * Get the current_sampling_rate of the audio_unit_descriptor object.
                 */
                uint32_t STDCALL get_current_sampling_rate();

                /**
                 * Get the sampling_rates_offset of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_sampling_rates_offset();

                /**
                 * Get the sampling_rates_count of the audio_unit_descriptor object.
                 */
                uint16_t STDCALL get_sampling_rates_count();

                /**
                 * Get the corresponding pull_field_multiplier for the pull_field_value.
                 */
                uint8_t STDCALL get_pull_field_multiplier(uint8_t pull_field_value);

                /**
                 * Print out AUDIO_UNIT descriptor fields.
                 */
                //	void print_audio_unit_desc_info();
        };
}

#endif