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
 * audio_unit_descriptor.h
 *
 * Public Audio Unit descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AUDIO_UNIT_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_AUDIO_UNIT_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
        class audio_unit_descriptor : public virtual descriptor_base
        {
        public:
                /**
                 * Get the object_name of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

                /**
                 * Get the localized_description of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

                /**
                 * Get the clock_domain_index of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_domain_index() = 0;

                /**
                 * Get the number_of_stream_input_ports of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_stream_input_ports() = 0;

                /**
                 * Get the base_stream_input_port of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_stream_input_port() = 0;

                /**
                 * Get the number_of_stream_output_ports of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_stream_output_ports() = 0;

                /**
                 * Get the base_stream_output_port of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_stream_output_port() = 0;

                /**
                 * Get the number_of_external_input_ports of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_external_input_ports() = 0;

                /**
                 * Get the base_external_input_port of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_external_input_port() = 0;

                /**
                 * Get the number_of_external_output_ports of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_external_output_ports() = 0;

                /**
                 * Get the base_external_output_port of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_external_output_port() = 0;

                /**
                 * Get the number_of_internal_input_ports of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_internal_input_ports() = 0;

                /**
                 * Get the base_internal_input_port of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_internal_input_port() = 0;

                /**
                 * Get the number_of_internal_output_ports of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_internal_output_ports() = 0;

                /**
                 * Get the base_internal_output_port of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_internal_output_port() = 0;

                /**
                 * Get the number_of_controls of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_controls() = 0;

                /**
                 * Get the base_control of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_control() = 0;

                /**
                 * Get the number_of_signal_selectors of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_signal_selectors() = 0;

                /**
                 * Get the base_signal_selector of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_signal_selector() = 0;

                /**
                 * Get the number_of_mixers of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_mixers() = 0;

                /**
                 * Get the base_mixer of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_mixer() = 0;

                /**
                 * Get the number_of_matrices of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_matrices() = 0;

                /**
                 * Get the base_matrix of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_matrix() = 0;

                /**
                 * Get the number_of_splitters of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_splitters() = 0;

                /**
                 * Get the base_splitter of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_splitter() = 0;

                /**
                 * Get the number_of_combiners of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_combiners() = 0;

                /**
                 * Get the base_combiner of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_combiner() = 0;

                /**
                 * Get the number_of_demultiplexers of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_demultiplexers() = 0;

                /**
                 * Get the base_demultiplexer of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_demultiplexer() = 0;

                /**
                 * Get the number_of_multiplexers of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_multiplexers() = 0;

                /**
                 * Get the base_multiplexer of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_multiplexer() = 0;

                /**
                 * Get the number_of_transcoders of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_transcoders() = 0;

                /**
                 * Get the base_transcoder of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_transcoder() = 0;

                /**
                 * Get the number_of_control_blocks of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_control_blocks() = 0;

                /**
                 * Get the base_control_block of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_control_block() = 0;

                /**
                 * Get the current_sampling_rate of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_current_sampling_rate() = 0;

                /**
                 * Get the sampling_rates_offset of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_sampling_rates_offset() = 0;

                /**
                 * Get the sampling_rates_count of the audio_unit_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_sampling_rates_count() = 0;

                /**
                 * Get the corresponding pull_field_multiplier for the pull_field_value.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_pull_field_multiplier(uint8_t pull_field_value) = 0;
        };
}

#endif