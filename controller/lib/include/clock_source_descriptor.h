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
 * clock_source_descriptor.h
 *
 * Public Clock Source descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CLOCK_SOURCE_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_CLOCK_SOURCE_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
	class clock_source_descriptor : public virtual descriptor_base
	{
	public:
		/**
		 * Get the name of the Clock Source descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

		/**
		 * Get the localized description of the Clock Source descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description () = 0;

		/**
		 * Get the flags of the Clock Source descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_flags() = 0;

		/**
		* Get the type of the Clock Source descriptor object.
		*/
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_type() = 0;

		/**
		* Get the identifier of the Clock Source descriptor object.
		*/
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_clock_source_identifier() = 0;

		/**
		 * Get the location type of the Clock Source descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_location_type() = 0;

		/**
		 * Get the location index of the Clock Source descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_location_index() = 0;
	};
}

#endif