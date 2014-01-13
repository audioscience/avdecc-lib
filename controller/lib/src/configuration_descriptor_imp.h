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
#include "locale_descriptor_imp.h"
#include "strings_descriptor_imp.h"
#include "stream_port_input_descriptor_imp.h"
#include "stream_port_output_descriptor_imp.h"
#include "audio_cluster_descriptor_imp.h"
#include "audio_map_descriptor_imp.h"
#include "clock_domain_descriptor_imp.h"
#include "configuration_descriptor.h"

namespace avdecc_lib
{
    class configuration_descriptor_imp : public configuration_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_configuration config_desc; // Structure containing the config_desc fields
        ssize_t config_desc_read_returned; // Status of extracting CONFIGURATION descriptor information from a network buffer

        std::vector<uint16_t> desc_type_vec; // Store descriptor types present in the CONFIGURATION descriptor
        std::vector<uint16_t> desc_count_vec; // Store descriptor counts present in the CONFIGURATION descriptor

        std::vector<audio_unit_descriptor_imp *> audio_unit_desc_vec;  // Store a list of AUDIO UNIT descriptor objects
        std::vector<stream_input_descriptor_imp *> stream_input_desc_vec; // Store a list of STREAM INPUT descriptor objects
        std::vector<stream_output_descriptor_imp *> stream_output_desc_vec; // Store a list of STREAM OUTPUT descriptor objects
        std::vector<jack_input_descriptor_imp *> jack_input_desc_vec; // Store a list of JACK INPUT descriptor objects
        std::vector<jack_output_descriptor_imp *> jack_output_desc_vec; // Store a list of JACK OUTPUT descriptor objects
        std::vector<avb_interface_descriptor_imp *> avb_interface_desc_vec; // Store a list of AVB INTERFACE descriptor objects
        std::vector<clock_source_descriptor_imp *> clock_source_desc_vec; // Store a list of CLOCK SOURCE descriptor objects
        std::vector<locale_descriptor_imp *> locale_desc_vec; // Store a list of LOCALE descriptor objects
        std::vector<strings_descriptor_imp *> strings_desc_vec; // Store a list of Strings descriptor objects
        std::vector<stream_port_input_descriptor_imp *> stream_port_input_desc_vec; // Store a list of Stream Port Input descriptor objects
        std::vector<stream_port_output_descriptor_imp *> stream_port_output_desc_vec; // Store a list of Stream Port Output descriptor objects
        std::vector<audio_cluster_descriptor_imp *> audio_cluster_desc_vec; // Store a list of Audio Cluster descriptor objects
        std::vector<audio_map_descriptor_imp *> audio_map_desc_vec; // Store a list of Audio Map descriptor objects
        std::vector<clock_domain_descriptor_imp *> clock_domain_desc_vec; // Store a list of CLOCK DOMAIN descriptor objects

    public:
        /**
         * Constructor for CONFIGURATION descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        configuration_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~configuration_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the name of the Configuration. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL object_name();

        /**
         * Get the localized string reference pointing to the localized Configuration name.
         */
        uint16_t STDCALL localized_description();

        /**
         * Get the number of descriptor counts. The maximum value for this field is 108 for this version of AEM.
         */
        uint16_t STDCALL descriptor_counts_count();

        /**
         * Get the offset to read the counts of the top level descriptor from the start of the descriptor.
         * This field is set to 74 for this version of AEM.
         */
        uint16_t descriptor_counts_offset();

        /**
         * Store AUDIO UNIT descriptor object.
         */
        void store_audio_unit_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store STREAM INPUT descriptor object.
         */
        void store_stream_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store STREAM OUTPUT descriptor object.
         */
        void store_stream_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store JACK INPUT descriptor object.
         */
        void store_jack_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store JACK OUTPUT descriptor object.
         */
        void store_jack_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store AVB INTERFACE descriptor object.
         */
        void store_avb_interface_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store CLOCK SOURCE descriptor object.
         */
        void store_clock_source_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store LOCALE descriptor object.
         */
        void store_locale_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store Strings descriptor object.
         */
        void store_strings_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store Stream Port Input descriptor object.
         */
        void store_stream_port_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store Stream Port Output descriptor object.
         */
        void store_stream_port_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store Audio Cluster descriptor object.
         */
        void store_audio_cluster_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store Audio Map descriptor object.
         */
        void store_audio_map_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Store CLOCK DOMAIN descriptor object.
         */
        void store_clock_domain_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Get the number of AUDIO UNIT descriptors present in the current configuration.
         */
        size_t STDCALL audio_unit_desc_count();

        /**
         * Get the number of STREAM INPUT descriptors present in the current configuration.
         */
        size_t STDCALL stream_input_desc_count();

        /**
         * Get the number of STREAM OUTPUT descriptors present in the current configuration.
         */
        size_t STDCALL stream_output_desc_count();

        /**
         * Get the number of JACK INPUT descriptors present in the current configuration.
         */
        size_t STDCALL jack_input_desc_count();

        /**
         * Get the number of JACK OUTPUT descriptors present in the current configuration.
         */
        size_t STDCALL jack_output_desc_count();

        /**
         * Get the number of AVB INTERFACE descriptors present in the current configuration.
         */
        size_t STDCALL avb_interface_desc_count();

        /**
         * Get the number of CLOCK SOURCE descriptors present in the current configuration.
         */
        size_t STDCALL clock_source_desc_count();

        /**
         * Get the number of LOCALE descriptors present in the current configuration.
         */
        size_t STDCALL locale_desc_count();

        /**
         * Get the number of Strings descriptors present in the current configuration.
         */
        size_t STDCALL strings_desc_count();

        /**
         * Get the number of Stream Port Input descriptors present in the current configuration.
         */
        size_t STDCALL stream_port_input_desc_count();

        /**
         * Get the number of Stream Port Output descriptors present in the current configuration.
         */
        size_t STDCALL stream_port_output_desc_count();

        /**
         * Get the number of Audio Cluster descriptors present in the current configuration.
         */
        size_t STDCALL audio_cluster_desc_count();

        /**
         * Get the number of Audio Map descriptors present in the current configuration.
         */
        size_t STDCALL audio_map_desc_count();

        /**
         * Get the number of CLOCK DOMAIN descriptors present in the current configuration.
         */
        size_t STDCALL clock_domain_desc_count();

        /**
         * Get the top level descriptor present in the CONFIGURATION descriptor.
         */
        uint16_t STDCALL get_desc_type_from_config_by_index(int desc_index);

        /**
         * Get the count of the top level descriptor present in the CONFIGURATION descriptor.
         */
        uint16_t STDCALL get_desc_count_from_config_by_index(int desc_index);

        /**
         * Check if the descriptor type and descriptor count index are valid and present in the current configuration.
         */
        bool STDCALL are_desc_type_and_index_in_config(int desc_type, int desc_count_index);

        /**
         * Get the corresponding AUDIO UNIT descriptor by index.
         */
        audio_unit_descriptor * STDCALL get_audio_unit_desc_by_index(size_t audio_unit_desc_index);

        /**
         * Get the corresponding STREAM INPUT descriptor by index.
         */
        stream_input_descriptor * STDCALL get_stream_input_desc_by_index(size_t stream_input_desc_index);

        /**
         * Get the corresponding STREAM OUTPUT descriptor by index.
         */
        stream_output_descriptor * STDCALL get_stream_output_desc_by_index(size_t stream_output_desc_index);

        /**
         * Get the corresponding JACK INPUT descriptor by index.
         */
        jack_input_descriptor * STDCALL get_jack_input_desc_by_index(size_t jack_input_desc_index);

        /**
         * Get the corresponding JACK OUTPUT descriptor by index.
         */
        jack_output_descriptor * STDCALL get_jack_output_desc_by_index(size_t jack_output_desc_index);

        /**
         * Get the corresponding AVB INTERFACE descriptor by index.
         */
        avb_interface_descriptor * STDCALL get_avb_interface_desc_by_index(size_t avb_interface_desc_index);

        /**
         * Get the corresponding CLOCK SOURCE descriptor by index.
         */
        clock_source_descriptor * STDCALL get_clock_source_desc_by_index(size_t clock_source_desc_index);

        /**
         * Get the corresponding LOCALE descriptor by index.
         */
        locale_descriptor * STDCALL get_locale_desc_by_index(size_t locale_desc_index);

        /**
         * Get the corresponding STRINGS descriptor by index.
         */
        strings_descriptor * STDCALL get_strings_desc_by_index(size_t strings_desc_index);

        /**
         * \return The corresponding STREAM PORT INPUT descriptor by index.
         */
        stream_port_input_descriptor * STDCALL get_stream_port_input_desc_by_index(size_t stream_port_input_desc_index);

        /**
         * \return The corresponding STREAM PORT OUTPUT descriptor by index.
         */
        stream_port_output_descriptor * STDCALL get_stream_port_output_desc_by_index(size_t stream_port_output_desc_index);

        /**
         * Get the corresponding AUDIO CLUSTER descriptor by index.
         */
        audio_cluster_descriptor * STDCALL get_audio_cluster_desc_by_index(size_t audio_cluster_desc_index);

        /**
         * Get the corresponding AUDIO MAP descriptor by index.
         */
        audio_map_descriptor * STDCALL get_audio_map_desc_by_index(size_t audio_map_desc_index);

        /**
         * Get the corresponding CLOCK DOMAIN descriptor by index.
         */
        clock_domain_descriptor * STDCALL get_clock_domain_desc_by_index(size_t clock_domain_desc_index);

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

