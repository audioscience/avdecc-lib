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
 * configuration_descriptor_imp.h
 *
 * CONFIGURATION descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "audio_unit_descriptor_imp.h"
#include "stream_input_descriptor_imp.h"
#include "stream_output_descriptor_imp.h"
#include "jack_input_descriptor_imp.h"
#include "jack_output_descriptor_imp.h"
#include "avb_interface_descriptor_imp.h"
#include "clock_source_descriptor_imp.h"
#include "memory_object_descriptor_imp.h"
#include "locale_descriptor_imp.h"
#include "strings_descriptor_imp.h"
#include "stream_port_input_descriptor_imp.h"
#include "stream_port_output_descriptor_imp.h"
#include "audio_cluster_descriptor_imp.h"
#include "audio_map_descriptor_imp.h"
#include "clock_domain_descriptor_imp.h"
#include "configuration_descriptor.h"
#include "control_descriptor_imp.h"
#include "external_port_input_descriptor_imp.h"
#include "external_port_output_descriptor_imp.h"

namespace avdecc_lib
{
    class configuration_descriptor_imp : public configuration_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_configuration config_desc; // Structure containing the config_desc fields

        std::vector<uint16_t> desc_type_vec; // Store descriptor types present in the CONFIGURATION descriptor
        std::vector<uint16_t> desc_count_vec; // Store descriptor counts present in the CONFIGURATION descriptor

        std::vector<audio_unit_descriptor_imp *> audio_unit_desc_vec;  // Store a list of AUDIO UNIT descriptor objects
        std::vector<stream_input_descriptor_imp *> stream_input_desc_vec; // Store a list of STREAM INPUT descriptor objects
        std::vector<stream_output_descriptor_imp *> stream_output_desc_vec; // Store a list of STREAM OUTPUT descriptor objects
        std::vector<jack_input_descriptor_imp *> jack_input_desc_vec; // Store a list of JACK INPUT descriptor objects
        std::vector<jack_output_descriptor_imp *> jack_output_desc_vec; // Store a list of JACK OUTPUT descriptor objects
        std::vector<avb_interface_descriptor_imp *> avb_interface_desc_vec; // Store a list of AVB INTERFACE descriptor objects
        std::vector<clock_source_descriptor_imp *> clock_source_desc_vec; // Store a list of CLOCK SOURCE descriptor objects
        std::vector<memory_object_descriptor_imp *> memory_object_desc_vec; // Store a list of MEMORY_OBJECT descriptor objects
        std::vector<locale_descriptor_imp *> locale_desc_vec; // Store a list of LOCALE descriptor objects
        std::vector<strings_descriptor_imp *> strings_desc_vec; // Store a list of Strings descriptor objects
        std::vector<stream_port_input_descriptor_imp *> stream_port_input_desc_vec; // Store a list of Stream Port Input descriptor objects
        std::vector<stream_port_output_descriptor_imp *> stream_port_output_desc_vec; // Store a list of Stream Port Output descriptor objects
        std::vector<audio_cluster_descriptor_imp *> audio_cluster_desc_vec; // Store a list of Audio Cluster descriptor objects
        std::vector<audio_map_descriptor_imp *> audio_map_desc_vec; // Store a list of Audio Map descriptor objects
        std::vector<clock_domain_descriptor_imp *> clock_domain_desc_vec; // Store a list of CLOCK DOMAIN descriptor objects
        std::vector<control_descriptor_imp *> control_desc_vec; // Store a list of CONTROL descriptor objects
        std::vector<external_port_input_descriptor_imp *> external_port_input_desc_vec; // Store a list of EXTERNAL_PORT_INPUT descriptor objects
        std::vector<external_port_output_descriptor_imp *> external_port_output_desc_vec; // Store a list of EXTERNAL_PORT_OUTPUT descriptor objects

    public:
        configuration_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~configuration_descriptor_imp();

        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        uint8_t * STDCALL object_name();
        uint16_t STDCALL localized_description();
        uint16_t STDCALL descriptor_counts_count();

        /**
         * Get the offset to read the counts of the top level descriptor from the start of the descriptor.
         * This field is set to 74 for this version of AEM.
         */
        uint16_t descriptor_counts_offset();

        void store_audio_unit_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_stream_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_stream_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_jack_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_jack_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_avb_interface_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_clock_source_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_memory_object_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_locale_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_strings_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_stream_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_stream_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_audio_cluster_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_audio_map_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_clock_domain_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_control_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_external_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        void store_external_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        size_t STDCALL audio_unit_desc_count();
        size_t STDCALL stream_input_desc_count();
        size_t STDCALL stream_output_desc_count();
        size_t STDCALL jack_input_desc_count();
        size_t STDCALL jack_output_desc_count();
        size_t STDCALL avb_interface_desc_count();
        size_t STDCALL clock_source_desc_count();
        size_t STDCALL memory_object_desc_count();
        size_t STDCALL locale_desc_count();
        size_t STDCALL strings_desc_count();
        size_t STDCALL stream_port_input_desc_count();
        size_t STDCALL stream_port_output_desc_count();
        size_t STDCALL audio_cluster_desc_count();
        size_t STDCALL audio_map_desc_count();
        size_t STDCALL clock_domain_desc_count();
        size_t STDCALL control_desc_count();
        size_t STDCALL external_port_input_desc_count();
        size_t STDCALL external_port_output_desc_count();

        uint16_t STDCALL get_desc_type_from_config_by_index(int desc_index);
        uint16_t STDCALL get_desc_count_from_config_by_index(int desc_index);
        bool STDCALL are_desc_type_and_index_in_config(int desc_type, int desc_count_index);
        audio_unit_descriptor * STDCALL get_audio_unit_desc_by_index(size_t audio_unit_desc_index);

        stream_input_descriptor * STDCALL get_stream_input_desc_by_index(size_t stream_input_desc_index);
        stream_output_descriptor * STDCALL get_stream_output_desc_by_index(size_t stream_output_desc_index);
        jack_input_descriptor * STDCALL get_jack_input_desc_by_index(size_t jack_input_desc_index);
        jack_output_descriptor * STDCALL get_jack_output_desc_by_index(size_t jack_output_desc_index);
        avb_interface_descriptor * STDCALL get_avb_interface_desc_by_index(size_t avb_interface_desc_index);
        clock_source_descriptor * STDCALL get_clock_source_desc_by_index(size_t clock_source_desc_index);
        memory_object_descriptor * STDCALL get_memory_object_desc_by_index(size_t memory_object_desc_index);
        locale_descriptor * STDCALL get_locale_desc_by_index(size_t locale_desc_index);
        uint8_t * STDCALL get_strings_desc_string_by_reference(size_t reference);
        strings_descriptor * STDCALL get_strings_desc_by_index(size_t strings_desc_index);
        stream_port_input_descriptor * STDCALL get_stream_port_input_desc_by_index(size_t stream_port_input_desc_index);
        stream_port_output_descriptor * STDCALL get_stream_port_output_desc_by_index(size_t stream_port_output_desc_index);
        audio_cluster_descriptor * STDCALL get_audio_cluster_desc_by_index(size_t audio_cluster_desc_index);
        audio_map_descriptor * STDCALL get_audio_map_desc_by_index(size_t audio_map_desc_index);
        clock_domain_descriptor * STDCALL get_clock_domain_desc_by_index(size_t clock_domain_desc_index);
        control_descriptor * STDCALL get_control_desc_by_index(size_t control_desc_index);
        external_port_input_descriptor * STDCALL get_external_port_input_desc_by_index(size_t index);
        external_port_output_descriptor * STDCALL get_external_port_output_desc_by_index(size_t index);

    private:
        /**
         * Initialize the descriptor type vector with descriptor types present in the current configuration.
         */
        void desc_type_vec_init(const uint8_t *frame, size_t pos);

        /**
         * Initialize the descriptor count vector with the number of the corresponding type of descriptor present in the current configuration.
         */
        void desc_count_vec_init(const uint8_t *frame, size_t pos);
    };
}

