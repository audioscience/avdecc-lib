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
 * clock_domain_descriptor.h
 *
 * Public Clock Domain descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CLOCK_DOMAIN_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_CLOCK_DOMAIN_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
	class clock_domain_descriptor : public virtual descriptor_base
	{
	public:
		/**
		 * Get the name of the Clock Domain descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

		/**
		 * Get the localized description of the Clock Domain descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

		/**
		 * Get the Clock Source index of the Clock Domain descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_index() = 0;

		/**
		* Get the Clock Sources offset of the Clock Domain descriptor object.
		*/
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_sources_offset() = 0;

		/**
		 * Get the Clock Sources count of the Clock Domain descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_sources_count() = 0;

		/**
		 * Get the corresponding Clock Sources by index present in the Clock Domain descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_by_index(uint32_t clk_src_index) = 0;

		/**
		 * Get the clock source index of the requested Clock Domain descriptor after sending a
		 * GET_CLOCK_SOURCE command and receiving a response back for the command.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_clock_source_index() = 0;

		/**
		 * Send a SET_CLOCK_SOURCE command to change the clock source of a clock domain.
		 */
		int STDCALL send_set_clock_source_cmd(void *notification_id, uint16_t desc_index, uint16_t new_clk_src_index);

		/**
		 * Send a GET_CLOCK_SOURCE command to get the current clock source of a clock domain.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_clock_source_cmd(void *notification_id, uint16_t desc_index) = 0;
	};
}

#endif