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
 * descriptor_base.h
 *
 * Public descriptor base interface class
 */

#pragma once

#ifndef _AVDECC_CONTROLLER_LIB_DESCRIPTOR_BASE_H_
#define _AVDECC_CONTROLLER_LIB_DESCRIPTOR_BASE_H_

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
	class descriptor_base
	{
	public:
		/**
		 * \return The descriptor type of a descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_descriptor_type() = 0;

		/**
		 * \return The descriptor index of a descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_descriptor_index() = 0;

		/**
		 * Send a ACQURE_ENTITY command to obtain exclusive access to an entire Entity or a sub-tree of objects.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag) = 0;

		/**
		 * Send a LOCK ENTITY command to provide short term exclusive access to the AVDECC Entity to perform atomic operations.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag) = 0;

		/**
		 * Send a SET_NAME command to change the value of a name field within a descriptor.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_name_cmd(uint16_t desc_index, uint16_t name_index, uint16_t config_index, char * new_name) = 0;

		/**
		 * Send a GET_NAME command to get the value of a name field within a descriptor.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_name_cmd(uint16_t desc_index, uint16_t name_index, uint16_t config_index) = 0;
	};
}

#endif
