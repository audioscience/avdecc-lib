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
 * Public AUDIO UNIT descriptor interface class
 * The AUDIO UNIT descriptor describes an AUDIO UNIT within the AVDECC Entity. An Audio Unit
 * represents a single audio clock domain.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class audio_unit_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The descriptor index of the Clock Domain descriptor describing the clock domain for the Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL clock_domain_index() = 0;

        /**
         * \return The number of Input Stream Ports used by this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_stream_input_ports() = 0;

        /**
         * \return The index of the first Stream Port Input descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_stream_input_port() = 0;

        /**
         * \return The number of Output Stream Ports used by this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_stream_output_ports() = 0;

        /**
         * \return The index of the first Stream Port Output descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_stream_output_port() = 0;

        /**
         * \return The number of external Input Ports used by this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_external_input_ports() = 0;

        /**
         * \return The index of the first External Port Input descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_external_input_port() = 0;

        /**
         * \return The number of external Output Ports used by this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_external_output_ports() = 0;

        /**
         * \return The index of the first External Port Output descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_external_output_port() = 0;

        /**
         * \return The number of internal Input Ports used by this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_internal_input_ports() = 0;

        /**
         * \return The index of the first input Internal JACK INPUT and Internal Port Input descriptors.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_internal_input_port() = 0;

        /**
         * \return The number of internal Output Ports used by this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_internal_output_ports() = 0;

        /**
         * \return The index of the first output Internal JACK OUTPUT and Internal Port Output descriptors.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_internal_output_port() = 0;

        /**
         * \return The number of controls within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_controls() = 0;

        /**
         * \return The index of the first Control descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_control() = 0;

        /**
         * \return The number of signal selectors within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_signal_selectors() = 0;

        /**
         * \return The index of the first Signal Selector descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_signal_selector() = 0;

        /**
         * \return The number of mixers within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_mixers() = 0;

        /**
         * \return The index of the first Mixer descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_mixer() = 0;

        /**
         * \return The number of matrices within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_matrices() = 0;

        /**
         * \return The index of the first Matrix descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_matrix() = 0;

        /**
         * \return The number of splitters within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_splitters() = 0;

        /**
         * \return The index of the first Signal Splitter descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_splitter() = 0;

        /**
         * \return The number of combiners within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_combiners() = 0;

        /**
         * \return index of the first Signal Combiner descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_combiner() = 0;

        /**
         * \return The number of demultiplexers within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_demultiplexers() = 0;

        /**
         * \return The index of the first Signal Demultiplexer descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_demultiplexer() = 0;

        /**
         * \return The number of multiplexers within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_multiplexers() = 0;

        /**
         * \return The index of the first Multiplexer descriptor..
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_multiplexer() = 0;

        /**
         * \return The number of transcoders within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_transcoders() = 0;

        /**
         * \return The index of the first Signal Transcoder descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_transcoder() = 0;

        /**
         * \return The number of control blocks within this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_control_blocks() = 0;

        /**
         * \return The index of the first Control Block descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_control_block() = 0;

        /**
         * \return The current sampling rate of this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL current_sampling_rate() = 0;

        /**
         * \return The corresponding sampling rate by index of this Audio Unit.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_sampling_rate_by_index(size_t sampling_rate_index) = 0;

        /**
         * \return The number of sample rates. The maximum value is 91 for this version of AEM.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL sampling_rates_count() = 0;

        /**
         * \return The sampling rate of a port or unit after sending a SET_SAMPLING_RATE command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL set_sampling_rate_sampling_rate() = 0;

        /**
         * \return The sampling rate of a port or unit after sending a GET_SAMPLING_RATE command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_sampling_rate_sampling_rate() = 0;

        /**
         * Send a SET_SAMPLING_RATE command to change the sampling rate of a port or unit.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_sampling_rate The sampling rate field is set to the new sampling rate.
         *
         * The new sampling rates can be retrieved by calling the following function after successfully
         * receiving a response back for the SET_SAMPLING_RATE command sent.
         *
         * \see set_sampling_rate_sampling_rate()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate) = 0;

        /**
         * Send a GET_SAMPLING_RATE command to get the current sampling rate of a port or unit.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * The sampling rates can be retrieved by calling the following function after successfully
         * receiving a response back for the GET_SAMPLING_RATE command sent.
         *
         * \see get_sampling_rate_sampling_rate()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_sampling_rate_cmd(void *notification_id) = 0;
    };
}

