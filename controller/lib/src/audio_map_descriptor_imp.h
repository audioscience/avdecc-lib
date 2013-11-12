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
 * audio_map_descriptor_imp.h
 *
 * Audio Map descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AUDIO_MAP_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_AUDIO_MAP_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "audio_map_descriptor.h"

namespace avdecc_lib
{
	class audio_map_descriptor_imp : public virtual audio_map_descriptor, public virtual descriptor_base_imp
	{

	private:
		struct jdksavdecc_descriptor_audio_map audio_map_desc; // Structure containing the audio_map_desc fields
		int audio_map_desc_read_returned; // Status of extracting Audio Map descriptor information from a network buffer
	public:
		/**
		 * Constructor for Audio Cluster descriptor object.
		 * 
		 * \param end_station_obj A pointer to the base End Station object.
		 * \param frame The raw memory that contains the descriptor information to read from.
		 * \param pos The position offset to read the descriptor fields from.
		 * \param frame_len The memory buffer length of the descriptor frame.
		 */
		audio_map_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

		virtual ~audio_map_descriptor_imp();

		/**
		 * Get the descriptor type of the Audio Map descriptor object.
		 */
		uint16_t STDCALL get_descriptor_type();

		/**
		 * Get the descriptor index of the Audio Map descriptor object.
		 */
		uint16_t STDCALL get_descriptor_index();

		/**
		 * Get the mappings offset of the Audio Map descriptor object.
		 */
		uint16_t STDCALL get_mappings_offset();

		/**
		 * Get the number of mappings of the Audio Map descriptor object.
		 */
		uint16_t STDCALL get_number_of_mappings();
	};
}

#endif

