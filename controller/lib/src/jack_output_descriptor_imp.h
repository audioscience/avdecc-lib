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
 * jack_output_descriptor_imp.h
 *
 * Jack Output descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_JACK_OUTPUT_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_JACK_OUTPUT_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "jack_output_descriptor.h"

namespace avdecc_lib
{
	class jack_output_descriptor_imp : public virtual jack_output_descriptor, public virtual descriptor_base_imp
	{
	private:
		struct jdksavdecc_descriptor_jack jack_output_desc; // Structure containing the jack_output_desc fields
		int desc_jack_read_returned; // Status of extracting Jack Input Descriptor or Jack Output descriptor information from a network buffer

	public:
		/**
		 * An empty constructor for jack_output_descriptor_imp
		 */
		jack_output_descriptor_imp();

		/**
		 * Constructor for jack_output_descriptor_imp used for constructing an object with a base End Station, frame, position offest,
		 * and memory buffer length.
		 */
		jack_output_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

		/**
		 * Destructor for jack_output_descriptor_imp used for destroying objects
		 */
		virtual ~jack_output_descriptor_imp();

		/**
		 * Get the descriptor type of the Jack Output descriptor object.
		 */
		uint16_t STDCALL get_descriptor_type();

		/**
		 * Get the descriptor index of the Jack Output descriptor object.
		 */
		uint16_t STDCALL get_descriptor_index();

		/**
		 * Get the name of the Jack Output descriptor object.
		 */
		uint8_t * STDCALL get_object_name();

		/**
		 * Get the localized description of the Jack Output descriptor object.
		 */
		uint16_t STDCALL get_localized_description();

		/**
		 * Get the flags of the Jack Output descriptor object.
		 */
		uint16_t STDCALL get_jack_flags();

		/**
		* Get the type of the Jack Output descriptor object.
		*/
		uint16_t STDCALL get_jack_type();

		/**
		 * Get the number of controls of the Jack Output descriptor object.
		 */
		uint16_t STDCALL get_number_of_controls();

		/**
		 * Get the base control of the Jack Output descriptor object.
		 */
		uint16_t STDCALL get_base_control();
	};
}

#endif