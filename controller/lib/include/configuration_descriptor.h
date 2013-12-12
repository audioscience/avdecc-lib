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
 * configuration_descriptor.h
 *
 * Public CONFIGURATION descriptor interface class
 * The CONFIGURATION descriptor describes an AVDECC Entity model for a particular setup of the
 * AVDECC Entity. The descriptor tells the AVDECC Controller how many of each of the top level
 * descriptors are present in the Configuration.
 */

#pragma once


#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class audio_unit_descriptor;
    class stream_input_descriptor;
    class stream_output_descriptor;
    class jack_input_descriptor;
    class jack_output_descriptor;
    class avb_interface_descriptor;
    class clock_source_descriptor;
    class locale_descriptor;
    class strings_descriptor;
    class stream_port_input_descriptor;
    class stream_port_output_descriptor;
    class audio_cluster_descriptor;
    class audio_map_descriptor;
    class clock_domain_descriptor;

    class configuration_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The number of descriptor counts. The maximum value for this field is 108 for this version of AEM.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL descriptor_counts_count() = 0;

        /**
         * \return The top level descriptor present in the CONFIGURATION descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_desc_type_from_config_by_index(int desc_index) = 0;

        /**
         * \return The count of the top level descriptor present in the CONFIGURATION descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_desc_count_from_config_by_index(int desc_index) = 0;

        /**
         * \return True if the descriptor type and descriptor count index are valid and present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL are_desc_type_and_index_in_config(int desc_type, int desc_count_index) = 0;

        /**
         * \return The number of AUDIO UNIT descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL audio_unit_desc_count() = 0;

        /**
         * \return The number of STREAM INPUT descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL stream_input_desc_count() = 0;

        /**
         * \return The number of STREAM OUTPUT descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL stream_output_desc_count() = 0;

        /**
         * \return The number of JACK INPUT descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL jack_input_desc_count() = 0;

        /**
         * \return The number of JACK OUTPUT descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL jack_output_desc_count() = 0;

        /**
         * \return The number of AVB INTERFACE descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL avb_interface_desc_count() = 0;

        /**
         * \return The number of CLOCK SOURCE descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL clock_source_desc_count() = 0;

        /**
         * \return The number of LOCALE descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL locale_desc_count() = 0;

        /**
         * \return The number of Strings descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL strings_desc_count() = 0;

        /**
         * \return The number of Stream Port Input descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL stream_port_input_desc_count() = 0;

        /**
         * \return The number of Stream Port Output descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL stream_port_output_desc_count() = 0;

        /**
         * \return The number of Audio Cluster descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL audio_cluster_desc_count() = 0;

        /**
         * \return The number of Audio Map descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL audio_map_desc_count() = 0;

        /**
         * \return The number of CLOCK DOMAIN descriptors present in the current configuration.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL clock_domain_desc_count() = 0;

        /**
         * \return The corresponding AUDIO UNIT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual audio_unit_descriptor * STDCALL get_audio_unit_desc_by_index(size_t audio_unit_desc_index) = 0;

        /**
         * \return The corresponding STREAM INPUT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_input_descriptor * STDCALL get_stream_input_desc_by_index(size_t stream_input_desc_index) = 0;

        /**
         * \return The corresponding STREAM OUTPUT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_output_descriptor * STDCALL get_stream_output_desc_by_index(size_t stream_output_desc_index) = 0;

        /**
         * \return The corresponding JACK INPUT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual jack_input_descriptor * STDCALL get_jack_input_desc_by_index(size_t jack_input_desc_index) = 0;

        /**
         * \return The corresponding JACK OUTPUT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual jack_output_descriptor * STDCALL get_jack_output_desc_by_index(size_t jack_output_desc_index) = 0;

        /**
         * \return The corresponding AVB INTERFACE descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual avb_interface_descriptor * STDCALL get_avb_interface_desc_by_index(size_t avb_interface_desc_index) = 0;

        /**
         * \return The corresponding CLOCK SOURCE descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual clock_source_descriptor * STDCALL get_clock_source_desc_by_index(size_t clock_source_desc_index) = 0;

        /**
         * \return The corresponding LOCALE descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual locale_descriptor * STDCALL get_locale_desc_by_index(size_t locale_desc_index) = 0;

        /**
         * \return The corresponding STRINGS descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual strings_descriptor * STDCALL get_strings_desc_by_index(size_t strings_desc_index) = 0;

        /**
         * \return The corresponding STREAM PORT INPUT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_port_input_descriptor * STDCALL get_stream_port_input_desc_by_index(size_t stream_port_input_desc_index) = 0;

        /**
         * \return The corresponding STREAM PORT OUTPUT descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_port_output_descriptor * STDCALL get_stream_port_output_desc_by_index(size_t stream_port_output_desc_index) = 0;

        /**
         * \return The corresponding AUDIO CLUSTER descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual audio_cluster_descriptor * STDCALL get_audio_cluster_desc_by_index(size_t audio_cluster_desc_index) = 0;

        /**
         * \return The corresponding AUDIO MAP descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual audio_map_descriptor * STDCALL get_audio_map_desc_by_index(size_t audio_map_desc_index) = 0;

        /**
         * \return The corresponding CLOCK DOMAIN descriptor by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual clock_domain_descriptor * STDCALL get_clock_domain_desc_by_index(size_t clock_domain_desc_index) = 0;
    };
}

