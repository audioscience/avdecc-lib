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
 * Public Configuration descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CONFIGURATION_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_CONFIGURATION_DESCRIPTOR_H_

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
		 * \return The name of the Configuration descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

		/**
		 * \return The localized description of the Configuration descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

		/**
		 * \return The descriptor counts count of the Configuration descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_descriptor_counts_count() = 0;

		/**
		 * \return The descriptor counts offset of the Configuration descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_descriptor_counts_offset() = 0;

		/**
		 * \return The top level descriptor present in the Configuration descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_desc_type_from_config_by_index(int desc_index) = 0;

		/**
		 * \return The count of the top level descriptor present in the Configuration descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_desc_count_from_config_by_index(int desc_index) = 0;

		/**
		 * \return True if the descriptor type and descriptor count index are valid and present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL are_desc_type_and_index_in_config(int desc_type, int desc_count_index) = 0;

		/**
		 * \return The number of Audio Unit descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_audio_unit_desc_count() = 0;

		/**
		 * \return The number of Stream Input descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_input_desc_count() = 0;

		/**
		 * \return The number of Stream Output descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_output_desc_count() = 0;

		/**
		 * \return The number of Jack Input descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_jack_input_desc_count() = 0;

		/**
		 * \return The number of Jack Output descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_jack_output_desc_count() = 0;

		/**
		 * \return The number of AVB Interface descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_avb_interface_desc_count() = 0;

		/**
		 * \return The number of Clock Source descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_clock_source_desc_count() = 0;

		/**
		 * \return The number of Locale descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_locale_desc_count() = 0;

		/**
		 * \return The number of Strings descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_strings_desc_count() = 0;

		/**
		 * \return The number of Stream Port Input descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_port_input_desc_count() = 0;

		/**
		 * \return The number of Stream Port Output descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_port_output_desc_count() = 0;

		/**
		 * \return The number of Audio Cluster descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_audio_cluster_desc_count() = 0;

		/**
		 * \return The number of Audio Map descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_audio_map_desc_count() = 0;

		/**
		 * \return The number of Clock Domain descriptors present in the current Configuration.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_clock_domain_desc_count() = 0;

		/**
		 * \return The corresponding AUDIO UNIT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual audio_unit_descriptor * STDCALL get_audio_unit_desc_by_index(uint32_t audio_unit_desc_index) = 0;

		/**
		 * \return The corresponding STREAM INPUT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual stream_input_descriptor * STDCALL get_stream_input_desc_by_index(uint32_t stream_input_desc_index) = 0;

		/**
		 * \return The corresponding STREAM OUTPUT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual stream_output_descriptor * STDCALL get_stream_output_desc_by_index(uint32_t stream_output_desc_index) = 0;

		/**
		 * \return The corresponding JACK INPUT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual jack_input_descriptor * STDCALL get_jack_input_desc_by_index(uint32_t jack_input_desc_index) = 0;

		/**
		 * \return The corresponding JACK OUTPUT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual jack_output_descriptor * STDCALL get_jack_output_desc_by_index(uint32_t jack_output_desc_index) = 0;

		/**
		 * \return The corresponding AVB INTERFACE descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual avb_interface_descriptor * STDCALL get_avb_interface_desc_by_index(uint32_t avb_interface_desc_index) = 0;

		/**
		 * \return The corresponding CLOCK SOURCE descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual clock_source_descriptor * STDCALL get_clock_source_desc_by_index(uint32_t clock_source_desc_index) = 0;

		/**
		 * \return The corresponding LOCALE descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual locale_descriptor * STDCALL get_locale_desc_by_index(uint32_t locale_desc_index) = 0;

		/**
		 * \return The corresponding STRINGS descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual strings_descriptor * STDCALL get_strings_desc_by_index(uint32_t strings_desc_index) = 0;

		/**
		 * \return The corresponding STREAM PORT INPUT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual stream_port_input_descriptor * STDCALL get_stream_port_input_desc_by_index(uint32_t stream_port_input_desc_index) = 0;

		/**
		 * \return The corresponding STREAM PORT OUTPUT descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual stream_port_output_descriptor * STDCALL get_stream_port_output_desc_by_index(uint32_t stream_port_output_desc_index) = 0;

		/**
		 * \return The corresponding AUDIO CLUSTER descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual audio_cluster_descriptor * STDCALL get_audio_cluster_desc_by_index(uint32_t clock_domain_desc_index) = 0;

		/**
		 * \return The corresponding AUDIO MAP descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual audio_map_descriptor * STDCALL get_audio_map_desc_by_index(uint32_t clock_domain_desc_index) = 0;

		/**
		 * \return The corresponding CLOCK DOMAIN descriptor object by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual clock_domain_descriptor * STDCALL get_clock_domain_desc_by_index(uint32_t clock_domain_desc_index) = 0;
	};
}

#endif
