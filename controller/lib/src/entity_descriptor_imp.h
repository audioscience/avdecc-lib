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
 * entity_descriptor_imp.h
 *
 * ENTITY descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "configuration_descriptor_imp.h"
#include "entity_descriptor.h"

namespace avdecc_lib
{
    class entity_descriptor_imp : public entity_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_entity entity_desc; // Structure containing the entity_desc fields
        ssize_t desc_entity_read_returned; // Status of extracting ENTITY descriptor information from a network buffer
        std::vector<configuration_descriptor_imp *> config_desc_vec; // Store a list of CONFIGURATION descriptor objects

        struct jdksavdecc_aem_command_acquire_entity_response aem_cmd_acquire_entity_resp; // Store the response received after sending a ACQUIRE_ENTITY command.
        struct jdksavdecc_aem_command_lock_entity_response aem_cmd_lock_entity_resp; // Store the response received after sending a LOCK_ENTITY command.

    public:
        /**
         * Constructor for ENTITY descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        entity_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~entity_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor. This is always set to 0 as
         * there is only ever one in an AVDECC Entity.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the Entity ID of the AVDECC Entity.
         */
        uint64_t STDCALL entity_id();

        /**
         * Get the vendor id of the AVDECC Entity.
         */
        uint32_t STDCALL vendor_id();

        /**
         * Get the AVDECC Entity model id for the AVDECC Entity.
         */
        uint32_t STDCALL entity_model_id();

        /**
         * Get the capabilities of the AVDECC Entity.
         */
        uint32_t STDCALL entity_capabilities();

        /**
         * Get the number of Output Streams the AVDECC Entity has. This is also the number
         * of STREAM_OUTPUT descriptors the AVDECC Entity has for Output Streams.
         */
        uint16_t STDCALL talker_stream_sources();

        /**
         * Get the AVDECC Talker capabilities of the AVDECC Entity.
         */
        uint16_t STDCALL talker_capabilities();

        /**
         * Get the number of Input Streams the AVDECC Entity has. This is also the number
         * of STREAM_INPUT descriptors the AVDECC Entity has for Input Streams.
         */
        uint16_t STDCALL listener_stream_sinks();

        /**
         * Get the AVDECC Listener capabilities of the AVDECC Entity.
         */
        uint16_t STDCALL listener_capabilities();

        /**
         * Get the AVDECC Controller capabilities of the AVDECC Entity.
         */
        uint32_t STDCALL controller_capabilities();

        /**
         * Get the available index of the AVDECC Entity.
         */
        uint32_t STDCALL available_index();

        /**
         * Get the association ID for the AVDECC Entity.
         */
        uint64_t STDCALL association_id();

        /**
         * Get the name of the AVDECC Entity. This may be user set through the use of a SET_NAME command.
         */
        uint8_t * STDCALL entity_name();

        /**
         * Get the localized string reference pointing to the localized vendor name.
         */
        uint16_t STDCALL vendor_name_string();

        /**
         * Get the localized string reference pointing to the localized model name.
         */
        uint16_t STDCALL model_name_string();

        /**
         * Get the firmware version of the AVDECC Entity.
         */
        uint8_t * STDCALL firmware_version();

        /**
         * Get the group name of the AVDECC Entity. This may be user set through the use of a SET_NAME command.
         */
        uint8_t * STDCALL group_name();

        /**
         * Get the serial number of the AVDECC Entity.
         */
        uint8_t * STDCALL serial_number();

        /**
         * Get the number of Configurations the device has. A device is required to have at least 1 Configuration.
         */
        uint16_t STDCALL configurations_count();

        /**
         * Get the index of the currently set Configuration.
         */
        uint16_t STDCALL current_configuration();

        /**
         * Store CONFIGURATION descriptor object.
         */
        void store_config_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        /**
         * Get the number of Configuration descriptors.
         */
        size_t STDCALL config_desc_count();

        /**
         * Get the corresponding Configuration descriptorby index.
         */
        configuration_descriptor * STDCALL get_config_desc_by_index(uint16_t config_desc_index);

        /**
         * Get the flags after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
         */
        uint32_t STDCALL acquire_entity_flags();

        /**
         * Get the owner id after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
         */
        uint64_t STDCALL acquire_entity_owner_entity_id();

        /**
         * Get the flags after sending a LOCK_ENTITY command and receiving a response back for the command.
         */
        uint32_t STDCALL lock_entity_flags();

        /**
         * Get the locked entity id after sending a LOCK_ENTITY command and receiving a response back for the command.
         */
        uint64_t STDCALL lock_entity_locked_entity_id();

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
         */
        int STDCALL send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag);

        /**
         * Process a ACQURE_ENTITY response for the ACQURE_ENTITY command.
         */
        int proc_acquire_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        /**
         * Send a LOCK_ENTITY command to provide short term exclusive access to the AVDECC Entity to perform atomic operations.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param lock_entity_flag The flag to be set for the command. Valid flags are 0 (LOCK) and 1 (UNLOCK).
         *
         * The flags and locked entity id can be retrieved by calling the following function after successfully
         * receiving a response back for the LOCK_ENTITY command sent.
         *
         * \see lock_entity_flags(), lock_entity_locked_entity_id()
         */
        int STDCALL send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag);

        /**
         * Process a LOCK ENTITY response for the LOCK ENTITY command.
         */
        int proc_lock_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        /**
         * Send a SET_CONFIFURATION command to change the current configuration of the AVDECC Entity.
         */
        int STDCALL send_set_config_cmd();

        /**
         * Process a SET_CONFIFURATION response for the SET_CONFIGURATION command.
         */
        int proc_set_config_resp();

        /**
         * Send a GET_CONFIGURATION command to get the current configuration of the AVDECC Entity.
         */
        int STDCALL send_get_config_cmd();

        /**
         * Process a GET_CONFIGURATION response for the GET_CONFIFURATION command.
         */
        int proc_get_config_resp();
    };
}

