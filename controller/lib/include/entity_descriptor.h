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
 * entity_descriptor.h
 *
 * Public Entity descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_ENTITY_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_ENTITY_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
        class configuration_descriptor;

        class entity_descriptor : public virtual descriptor_base
        {
        public:
                /**
                 * Get the id of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_entity_id() = 0;

                /**
                 * Get the vendor id of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_vendor_id() = 0;

                /**
                 * Get the model id of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_entity_model_id() = 0;

                /**
                 * Get the capabilities of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_entity_capabilities() = 0;

                /**
                 * Get the Talker Stream Sources of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_talker_stream_sources() = 0;

                /**
                 * Get the Talker capabilities of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_talker_capabilities() = 0;

                /**
                 * Get the Listener Stream Sinks of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_listener_stream_sinks() = 0;

                /**
                 * Get the Listener capabilities of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_listener_capabilities() = 0;

                /**
                 * Get the Controller capabilities of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_controller_capabilities() = 0;

                /**
                 * Get the available index of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_available_index() = 0;

                /**
                 * Get the association id of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_association_id() = 0;

                /**
                 * Get the name of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_entity_name() = 0;

                /**
                 * Get the vendor name string of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_vendor_name_string() = 0;

                /**
                 * Get the model name string of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_model_name_string() = 0;

                /**
                 * Get the firmware version of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_firmware_version() = 0;

                /**
                 * Get the group name of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_group_name() = 0;

                /**
                 * Get the serial number of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_serial_number() = 0;

                /**
                 * Get the configurations count of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_configurations_count() = 0;

                /**
                 * Get the current configuration of the Entity descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_current_configuration() = 0;

                /**
                 * Get the number of Configuration descriptors.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_config_desc_count() = 0;

                /**
                 * Get the corresponding CONFIGURATION descriptor object by index.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual configuration_descriptor * STDCALL get_config_desc_by_index(uint16_t config_desc_index) = 0;

                /**
                 * Send a SET_CONFIFURATION command to change the current configuration of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_config_cmd() = 0;

                /**
                 * Send a GET_CONFIGURATION command to get the current configuration of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_config_cmd() = 0;
        };
}

#endif