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
                 * Get the  name of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

                /**
                 * Get the localized description of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

                /**
                 * Get the clock domain index of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_domain_index() = 0;

                /**
                 * Get the number of stream input ports of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_stream_input_ports() = 0;

                /**
                 * Get the base stream input port of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_stream_input_port() = 0;

                /**
                 * Get the number of stream output ports of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_stream_output_ports() = 0;

                /**
                 * Get the base stream output port of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_stream_output_port() = 0;

                /**
                 * Get the number of external input ports of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_external_input_ports() = 0;

                /**
                 * Get the base external input port of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_external_input_port() = 0;

                /**
                 * Get the number of external output_ports of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_external_output_ports() = 0;

                /**
                 * Get the base external output port of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_external_output_port() = 0;

                /**
                 * Get the number of internal input ports of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_internal_input_ports() = 0;

                /**
                 * Get the base internal input port of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_internal_input_port() = 0;

                /**
                 * Get the number of internal output ports of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_internal_output_ports() = 0;

                /**
                 * Get the base internal output port of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_internal_output_port() = 0;

                /**
                 * Get the number of controls of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_controls() = 0;

                /**
                 * Get the base control of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_control() = 0;

                /**
                 * Get the number of signal selectors of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_signal_selectors() = 0;

                /**
                 * Get the base signal selector of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_signal_selector() = 0;

                /**
                 * Get the number of mixers of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_mixers() = 0;

                /**
                 * Get the base mixer of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_mixer() = 0;

                /**
                 * Get the number of matrices of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_matrices() = 0;

                /**
                 * Get the base matrix of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_matrix() = 0;

                /**
                 * Get the number of splitters of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_splitters() = 0;

                /**
                 * Get the base splitter of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_splitter() = 0;

                /**
                 * Get the number of combiners of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_combiners() = 0;

                /**
                 * Get the base combiner of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_combiner() = 0;

                /**
                 * Get the number of demultiplexers of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_demultiplexers() = 0;

                /**
                 * Get the base demultiplexer of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_demultiplexer() = 0;

                /**
                 * Get the number of multiplexers of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_multiplexers() = 0;

                /**
                 * Get the base multiplexer of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_multiplexer() = 0;

                /**
                 * Get the number of transcoders of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_transcoders() = 0;

                /**
                 * Get the base transcoder of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_transcoder() = 0;

                /**
                 * Get the number of control blocks of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_control_blocks() = 0;

                /**
                 * Get the base control block of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_control_block() = 0;

                /**
                 * Get the current sampling rate of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_current_sampling_rate() = 0;

                /**
                 * Get the sampling rates offset of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_sampling_rates_offset() = 0;

                /**
                 * Get the sampling rates count of the Audio Unit descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_sampling_rates_count() = 0;

                /**
                 * Get the corresponding pull field multiplier for the pull field value.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_pull_field_multiplier(uint8_t pull_field_value) = 0;
        };
}

#endif