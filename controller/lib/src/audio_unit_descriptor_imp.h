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
        ssize_t desc_audio_read_returned; // Status of extracting AUDIO_UNIT descriptor information from a network buffer

        std::vector<uint32_t> sample_rates_vec; // Store sample rates information
        struct jdksavdecc_aem_command_set_sampling_rate_response aem_cmd_set_sampling_rate_resp; // Store the response received after sending a SET_SAMPLING_RATE command.
        struct jdksavdecc_aem_command_get_sampling_rate_response aem_cmd_get_sampling_rate_resp; // Store the response received after sending a GET_SAMPLING_RATE command.

    public:
        /**
         * Constructor for AUDIO UNIT descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        audio_unit_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~audio_unit_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the name of the Audio Unit. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL object_name();

        /**
         * Get the localized string reference pointing to the localized AUDIO UNIT name.
         */
        uint16_t STDCALL localized_description();

        /**
         * Get the descriptor index of the CLOCK DOMAIN descriptor describing the CLOCK DOMAIN for the Audio Unit.
         */
        uint16_t STDCALL clock_domain_index();

        /**
         * Get the number of Input Stream Ports used by this Audio Unit.
         */
        uint16_t STDCALL number_of_stream_input_ports();

        /**
         * Get the index of the first Stream Port Input descriptor.
         */
        uint16_t STDCALL base_stream_input_port();

        /**
         * Get the number of Output Stream Ports used by this Audio Unit.
         */
        uint16_t STDCALL number_of_stream_output_ports();

        /**
         * Get the index of the first Stream Port Output descriptor.
         */
        uint16_t STDCALL base_stream_output_port();

        /**
         * Get the number of external Input Ports used by this Audio Unit.
         */
        uint16_t STDCALL number_of_external_input_ports();

        /**
         * Get the index of the first External Port Input descriptor.
         */
        uint16_t STDCALL base_external_input_port();

        /**
         * Get the number of external Output Ports used by this Audio Unit.
         */
        uint16_t STDCALL number_of_external_output_ports();

        /**
         * Get the index of the first External Port Output descriptor.
         */
        uint16_t STDCALL base_external_output_port();

        /**
         * Get the number of internal Input Ports used by this Audio Unit.
         */
        uint16_t STDCALL number_of_internal_input_ports();

        /**
         * Get the index of the first input Internal JACK INPUT and Internal Port Input descriptors.
         */
        uint16_t STDCALL base_internal_input_port();

        /**
         * Get the number of internal Output Ports used by this Audio Unit.
         */
        uint16_t STDCALL number_of_internal_output_ports();

        /**
         * Get the index of the first output Internal JACK OUTPUT and Internal Port Output descriptors.
         */
        uint16_t STDCALL base_internal_output_port();

        /**
         * Get the number of controls within this Audio Unit.
         */
        uint16_t STDCALL number_of_controls();

        /**
         * Get the index of the first Control descriptor.
         */
        uint16_t STDCALL base_control();

        /**
         * Get the number of signal selectors within this Audio Unit.
         */
        uint16_t STDCALL number_of_signal_selectors();

        /**
         * Get the index of the first Signal Selector descriptor.
         */
        uint16_t STDCALL base_signal_selector();

        /**
         * Get the number of mixers within this Audio Unit.
         */
        uint16_t STDCALL number_of_mixers();

        /**
         * Get the index of the first Mixer descriptor.
         */
        uint16_t STDCALL base_mixer();

        /**
         * Get the number of matrices within this Audio Unit.
         */
        uint16_t STDCALL number_of_matrices();

        /**
         * Get the index of the first Matrix descriptor.
         */
        uint16_t STDCALL base_matrix();

        /**
         * Get the number of splitters within this Audio Unit.
         */
        uint16_t STDCALL number_of_splitters();

        /**
         * Get the index of the first Signal Splitter descriptor.
         */
        uint16_t STDCALL base_splitter();

        /**
         * Get the number of combiners within this Audio Unit.
         */
        uint16_t STDCALL number_of_combiners();

        /**
         * Get index of the first Signal Combiner descriptor.
         */
        uint16_t STDCALL base_combiner();

        /**
         * Get the number of demultiplexers within this Audio Unit.
         */
        uint16_t STDCALL number_of_demultiplexers();

        /**
         * Get the index of the first Signal Demultiplexer descriptor.
         */
        uint16_t STDCALL base_demultiplexer();

        /**
         * Get the number of multiplexers within this Audio Unit.
         */
        uint16_t STDCALL number_of_multiplexers();

        /**
         * Get the index of the first Multiplexer descriptor.
         */
        uint16_t STDCALL base_multiplexer();

        /**
         * Get the number of transcoders within this Audio Unit.
         */
        uint16_t STDCALL number_of_transcoders();

        /**
         * Get the index of the first Signal Transcoder descriptor.
         */
        uint16_t STDCALL base_transcoder();

        /**
         * Get the number of control blocks within this Audio Unit.
         */
        uint16_t STDCALL number_of_control_blocks();

        /**
         * Get the index of the first Control Block descriptor.
         */
        uint16_t STDCALL base_control_block();

        /**
         * Get the current sampling rate of this Audio Unit.
         */
        uint32_t STDCALL current_sampling_rate();

        /**
         * Get the corresponding sampling rate by index of this Audio Unit.
         */
        uint32_t STDCALL get_sampling_rate_by_index(size_t sampling_rate_index);

        /**
         * Get the offset to the sample rates field from the start of the descriptor.
         * This field is 144 for this version of AEM.
         */
        uint16_t sampling_rates_offset();

        /**
         * Get the number of sample rates. The maximum value is 91 for this version of AEM.
         */
        uint16_t STDCALL sampling_rates_count();

        /**
         * Get the sampling rate of a port or unit after sending a SET_SAMPLING_RATE command and
         * receiving a response back for the command.
         */
        uint32_t STDCALL set_sampling_rate_sampling_rate();

        /**
         * Get the sampling rate of a port or unit after sending a GET_SAMPLING_RATE command and
         * receiving a response back for the command.
         */
        uint32_t STDCALL get_sampling_rate_sampling_rate();

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
        int STDCALL send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate);

        /**
         * Process a SET_SAMPLING_RATE response for the SET_SAMPLING_RATE command. The response always contains the
         * current sampling rate, that is it contains the new sampling rate if the command succeeds or the old
         * sampling rate if it fails.
         */
        int proc_set_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

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
        int STDCALL send_get_sampling_rate_cmd(void *notification_id);

        /**
         * Process a GET_SAMPLING_RATE response for the GET_SAMPLING_RATE command. The sampling rate field is set
         * to the current sampling rate of the port or unit in the response.
         */
        int proc_get_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

    private:
        /**
         * Store the sampling rates of the AUDIO UNIT in a vector.
         */
        void sampling_rates_init(const uint8_t *frame);
    };
}

