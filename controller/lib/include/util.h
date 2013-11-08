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
 * util.h
 *
 * Public Utility interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_UTIL_H_
#define _AVDECC_CONTROLLER_LIB_UTIL_H_

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
	class util
	{
	public:
		/**
		 * Call destructor for Controller used for destroying objects
		 */
		AVDECC_CONTROLLER_LIB32_API virtual void STDCALL destroy() = 0;

		/**
		 * Convert command value to its corresponding command name.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL cmd_value_to_name(uint16_t cmd_value) = 0;

		/**
		 * Convert command name to its corresponding command value.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL cmd_name_to_value(const char *cmd_name) = 0;

		/**
		 * Convert descriptor value to its corresponding descriptor name.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL desc_value_to_name(uint16_t desc_value) = 0;

		/**
		 * Convert descriptor name to its corresponding descriptor value.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL desc_name_to_value(const char *desc_name) = 0;

		/**
		 * Convert command status value to its corresponding command status name.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL cmd_status_value_to_name(uint16_t cmd_status_value) = 0;

		/**
		 * Convert notification value to its corresponding notification name.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL notification_value_to_name(uint16_t notification_value) = 0;

		/**
		 * Convert post_log_msg value to its corresponding post_log_msg name.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL logging_level_value_to_name(uint16_t logging_level_value) = 0;

		/**
		 * Convert IEEE1722 format value to name.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL ieee1722_format_value_to_name(uint64_t format) = 0;
	};

	/**
	 * Create a public Utility object used for accessing from outside the library.
	 */
	extern "C" AVDECC_CONTROLLER_LIB32_API util * STDCALL create_util();
}

#endif
