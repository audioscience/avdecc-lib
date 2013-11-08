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
 * Configuration descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CONFIGURATION_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_CONFIGURATION_DESCRIPTOR_IMP_H_

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
#include "clock_domain_descriptor_imp.h"
#include "configuration_descriptor.h"

namespace avdecc_lib
{
	class configuration_descriptor_imp : public virtual configuration_descriptor, public virtual descriptor_base_imp
	{
	private:
		struct jdksavdecc_descriptor_configuration config_desc; // Structure containing the config_desc fields
		int desc_config_read_returned; // Status of extracting CONFIGURATION descriptor information from a network buffer

		std::vector<uint16_t> desc_type_vector; // Store descriptor types present in the CONFIGURATION descriptor
		std::vector<uint16_t> desc_count_vector; // Store descriptor counts present in the CONFIGURATION descriptor

		std::vector<audio_unit_descriptor_imp *> audio_unit_desc_vec;  // Store a list of Audio Unit descriptor class objects
		std::vector<stream_input_descriptor_imp *> stream_input_desc_vec; // Store a list of Stream Input descriptor class objects
		std::vector<stream_output_descriptor_imp *> stream_output_desc_vec; // Store a list of Stream Output descriptor class objects
		std::vector<jack_input_descriptor_imp *> jack_input_desc_vec; // Store a list of Jack Input descriptor class objects
		std::vector<jack_output_descriptor_imp *> jack_output_desc_vec; // Store a list of Jack Output descriptor class objects
		std::vector<avb_interface_descriptor_imp *> avb_interface_desc_vec; // Store a list of AVB Interface descriptor class objects
		std::vector<clock_source_descriptor_imp *> clock_source_desc_vec; // Store a list of Clock Source descriptor class objects
		std::vector<locale_descriptor_imp *> locale_desc_vec; // Store a list of Locale descriptor class objects
		std::vector<strings_descriptor_imp *> strings_desc_vec; // Store a list of Strings descriptor class objects
		std::vector<clock_domain_descriptor_imp *> clock_domain_desc_vec; // Store a list of Clock Domain descriptor class objects

		/**
		 * Initialize the descriptor type vector with descriptor types present in the current Configuration.
		 */
		void desc_type_vector_init(const uint8_t *frame, size_t pos);

		/**
		 * Initialize the descriptor count vector with descriptor counts present in the current Configuration.
		 */
		void desc_count_vector_init(const uint8_t *frame, size_t pos);

	public:
		/**
		 * Constructor for Configuration descriptor object.
		 * 
		 * \param end_station_obj A pointer to the base End Station object.
		 * \param frame The raw memory that contains the descriptor information to read from.
		 * \param pos The position offset to read the descriptor fields from.
		 * \param frame_len The memory buffer length of the descriptor frame.
		 */
		configuration_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		virtual ~configuration_descriptor_imp();

		/**
		 * Get the descriptor type of the Configuration descriptor object.
		 */
		uint16_t STDCALL get_descriptor_type();

		/**
		 * Get the descriptor index of the Configuration descriptor object.
		 */
		uint16_t STDCALL get_descriptor_index();

		/**
		 * Get the name of the Configuration descriptor object.
		 */
		uint8_t * STDCALL get_object_name();

		/**
		 * Get the localized description of the Configuration descriptor object.
		 */
		uint16_t STDCALL get_localized_description();

		/**
		 * Get the descriptor counts count of the Configuration descriptor object.
		 */
		uint16_t STDCALL get_descriptor_counts_count();

		/**
		 * Get the descriptor counts offset of the Configuration descriptor object.
		 */
		uint16_t STDCALL get_descriptor_counts_offset();

		/**
		 * Store Audio Unit descriptor object.
		 */
		void store_audio_unit_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Stream Input descriptor object.
		 */
		void store_stream_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Stream Output descriptor object.
		 */
		void store_stream_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Jack Input descriptor object.
		 */
		void store_jack_input_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Jack Output descriptor object.
		 */
		void store_jack_output_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store AVB Interface descriptor object.
		 */
		void store_avb_interface_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Clock Source descriptor object.
		 */
		void store_clock_source_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Locale descriptor object.
		 */
		void store_locale_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Strings descriptor object.
		 */
		void store_strings_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Store Clock Domain descriptor object.
		 */
		void store_clock_domain_desc(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		/**
		 * Get the number of Audio Unit descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_audio_unit_desc_count();

		/**
		 * Get the number of Stream Input descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_stream_input_desc_count();

		/**
		 * Get the number of Stream Output descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_stream_output_desc_count();

		/**
		 * Get the number of Jack Input descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_jack_input_desc_count();

		/**
		 * Get the number of Jack Output descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_jack_output_desc_count();

		/**
		 * Get the number of AVB Interface descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_avb_interface_desc_count();

		/**
		 * Get the number of Clock Source descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_clock_source_desc_count();

		/**
		 * Get the number of Locale descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_locale_desc_count();

		/**
		 * Get the number of Strings descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_strings_desc_count();

		/**
		 * Get the number of Clock Domain descriptors present in the current Configuration.
		 */
		uint32_t STDCALL get_clock_domain_desc_count();

		/**
		 * Get the top level descriptor present in the Configuration descriptor object.
		 */
		uint16_t STDCALL get_desc_type_from_config_by_index(int desc_index);

		/**
		 * Get the count of the top level descriptor present in the Configuration descriptor object.
		 */
		uint16_t STDCALL get_desc_count_from_config_by_index(int desc_index);

		/**
		 * Check if the descriptor type and descriptor count index are valid and present in the current Configuration.
		 */
		bool STDCALL are_desc_type_and_index_in_config(int desc_type, int desc_count_index);

		/**
		 * Get the corresponding AUDIO UNIT descriptor object by index.
		 */
		audio_unit_descriptor * STDCALL get_audio_unit_desc_by_index(uint32_t audio_unit_desc_index);

		/**
		 * Get the corresponding STREAM INPUT descriptor object by index.
		 */
		stream_input_descriptor * STDCALL get_stream_input_desc_by_index(uint32_t stream_input_desc_index);

		/**
		 * Get the corresponding STREAM OUTPUT descriptor object by index.
		 */
		stream_output_descriptor * STDCALL get_stream_output_desc_by_index(uint32_t stream_output_desc_index);

		/**
		 * Get the corresponding JACK INPUT descriptor object by index.
		 */
		jack_input_descriptor * STDCALL get_jack_input_desc_by_index(uint32_t jack_input_desc_index);

		/**
		 * Get the corresponding JACK OUTPUT descriptor object by index.
		 */
		jack_output_descriptor * STDCALL get_jack_output_desc_by_index(uint32_t jack_output_desc_index);

		/**
		 * Get the corresponding AVB INTERFACE descriptor object by index.
		 */
		avb_interface_descriptor * STDCALL get_avb_interface_desc_by_index(uint32_t avb_interface_desc_index);

		/**
		 * Get the corresponding CLOCK SOURCE descriptor object by index.
		 */
		clock_source_descriptor * STDCALL get_clock_source_desc_by_index(uint32_t clock_source_desc_index);

		/**
		 * Get the corresponding LOCALE descriptor object by index.
		 */
		locale_descriptor * STDCALL get_locale_desc_by_index(uint32_t locale_desc_index);

		/**
		 * Get the corresponding STRINGS descriptor object by index.
		 */
		strings_descriptor * STDCALL get_strings_desc_by_index(uint32_t strings_desc_index);

		/**
		 * Get the corresponding CLOCK DOMAIN descriptor object by index.
		 */
		clock_domain_descriptor * STDCALL get_clock_domain_desc_by_index(uint32_t clock_domain_desc_index);
	};
}

#endif
