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
 * locale_descriptor.h
 *
 * Public Locale descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_LOCALE_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_LOCALE_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
	class locale_descriptor : public virtual descriptor_base
	{
	public:
		/**
		 * Get the Locale identifier of the Locale descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_locale_identifier() = 0;

		/**
		 * Get the number of strings of the Locale descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_number_of_strings() = 0;

		/**
		 * Get the base strings of the Locale descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_base_strings() = 0;
	};
}

#endif