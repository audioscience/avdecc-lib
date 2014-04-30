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

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
    class descriptor_field;

    class descriptor_base
    {
    public:
        /**
         * \return The type of the descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL descriptor_type() const = 0;

        /**
         * \return The index of the descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL descriptor_index() const = 0;

        /**
        * \return The number of fields in the descriptor.
        */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL field_count() const = 0;

        /**
        * \return The indicated field in the descriptor.
        */
        AVDECC_CONTROLLER_LIB32_API virtual descriptor_field * STDCALL field(size_t index) const = 0;

        /**
         * \return The name of the descriptor object. This may be user set through the use of a SET_NAME command.
         *	   The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         *	   defined value being provided in a localized form via the localized descripton field. By leaving
         *	   this field blank an AVDECC Controller can determine if the user has overridden the name and can
         *	   use this name rather than the localized name.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL object_name() = 0;

        /**
         * \return The localized string reference pointing to the localized descriptor name.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL localized_description() = 0;

        /**
         * \return The flags after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL acquire_entity_flags() = 0;

        /**
         * \return The owner id after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL acquire_entity_owner_entity_id() = 0;

        /**
         * \return The flags after sending a LOCK_ENTITY command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL lock_entity_flags() = 0;

        /**
         * \return The locked entity id after sending a LOCK_ENTITY command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL lock_entity_locked_entity_id() = 0;

        /**
         * Send a ACQURE_ENTITY command to obtain exclusive access to an entire Entity or a sub-tree of objects.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param acquire_entity_flag The flag to be set for the command. Valid flags are 0, 1 (PERSISTENT), and 0x80000000 (RELEASE).
         *
         * The flags and owner id can be retrieved by calling the following function after successfully
         * receiving a response back for the ACQURE_ENTITY command sent.
         *
         * \see acquire_entity_flags(), acquire_entity_owner_entity_id()
         *
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag) = 0;

        /**
         * Send a LOCK_ENTITY command to provide short term exclusive access to the AVDECC Entity to perform atomic operations.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param lock_entity_flag The flag to be set for the command. Valid flags are 0 and 1 (UNLOCK).
         *
         * The flags and locked entity id can be retrieved by calling the following function after successfully
         * receiving a response back for the LOCK_ENTITY command sent.
         *
         * \see lock_entity_flags(), lock_entity_locked_entity_id()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag) = 0;

        /**
         * Send a REBOOT command to the AVDECC Entity
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param lock_entity_flag The flag to be set for the command. Valid flags are 0 and 1 (UNLOCK).
         *
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_reboot_cmd(void *notification_id) = 0;

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
    };
}

