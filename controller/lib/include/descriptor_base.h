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
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_descriptor_index() const = 0;

		/**
		 * Send a ACQURE_ENTITY command to obtain exclusive access to an entire Entity or a sub-tree of objects.
		 *
		 * \param notification_id A void pointer to the unique identifier associated with the command.
		 * \param acquire_entity_flag The flag to be set for the command. Valid flags are 0, 1 (PERSISTENT), and 0x80000000 (RELEASE).
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag) = 0;

		/**
		 * Send a LOCK ENTITY command to provide short term exclusive access to the AVDECC Entity to perform atomic operations.
		 *
		 * \param notification_id A void pointer to the unique identifier associated with the command.
		 * \param lock_entity_flag The flag to be set for the command. Valid flags are 0 and 1 (UNLOCK).
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag) = 0;

		/**
		 * Send a SET_NAME command to change the value of a name field within a descriptor. For descriptors with multiple names, this
		 * sets only one specified name per command.
		 *
		 * \param notification_id A void pointer to the unique identifier associated with the command.
		 * \param name_index The index of the name within the descriptor, with the first name being index 0 and so on.
		 * \param config_index The descriptor index of the configuration, which contains the descriptor whose name is being set.
		 *		       If the descriptor type field is either ENTITY or CONFIGURATION, then this field is set to 0.
		 * \param new_name The new name to be set. The name does not contain a trailing NULL, but if the name is less than 64 bytes
		 *		   in length, then it is zero padded. 
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_name_cmd(void *notification_id, uint16_t name_index, uint16_t config_index, char * new_name) = 0;

		/**
		 * Send a GET_NAME command to get the value of a name field within a descriptor. For descriptors with multiple names, this
		 * sets only one specified name.
		 *
		 * \param notification_id A void pointer to the unique identifier associated with the command.
		 * \param name_index The index of the name within the descriptor, with the first name being index 0 and so on.
		 * \param config_index The descriptor index of the configuration, which contains the descriptor whose name is being set.
		 *		       If the descriptor type field is either ENTITY or CONFIGURATION, then this field is set to 0.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_name_cmd(void *notification_id, uint16_t name_index, uint16_t config_index) = 0;

		/**
		 * \return The localized description of the descriptor
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

		/**
		 * \return The name of the descriptor object.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

	};
}

#endif
