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
 * The Entity descriptor describes the highest level of the AVDECC Entity. It repeats some of the information
 * contained within the ADP advertise for the AVDECC Entity as well as the information required to read the
 * rest of the descriptors from the AVDECC Entity.
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
                 * \return The Entity ID of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_entity_id() = 0;

                /**
                 * \return The vendor id of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_vendor_id() = 0;

                /**
                 * \return The AVDECC Entity model id for the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_entity_model_id() = 0;

                /**
                 * \return The capabilities of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_entity_capabilities() = 0;

                /**
                 * \return The number of Output Streams the AVDECC Entity has. This is also the number
		 *	   of STREAM_OUTPUT descriptors the AVDECC Entity has for Output Streams.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_talker_stream_sources() = 0;

                /**
                 * \return The AVDECC Talker capabilities of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_talker_capabilities() = 0;

                /**
                 * \return The number of Input Streams the AVDECC Entity has. This is also the number
		 *	   of STREAM_INPUT descriptors the AVDECC Entity has for Input Streams.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_listener_stream_sinks() = 0;

                /**
                 * \return The AVDECC Listener capabilities of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_listener_capabilities() = 0;

                /**
                 * \return The AVDECC Controller capabilities of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_controller_capabilities() = 0;

                /**
                 * \return The available index of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_available_index() = 0;

                /**
                 * \return The association ID for the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_association_id() = 0;

                /**
                 * \return The name of the AVDECC Entity. This may be user set through the use of a SET_NAME command.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_entity_name() = 0;

                /**
                 * \return The localized string reference pointing to the localized vendor name.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_vendor_name_string() = 0;

                /**
                 * \return The localized string reference pointing to the localized model name.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_model_name_string() = 0;

                /**
                 * \return The firmware version of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_firmware_version() = 0;

                /**
                 * \return The group name of the AVDECC Entity. This may be user set through the use of a SET_NAME command.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_group_name() = 0;

                /**
                 * \return The serial number of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_serial_number() = 0;

                /**
                 * \return The number of Configurations the device has. A device is required to have at least 1 Configuration.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_configurations_count() = 0;

                /**
                 * \return The index of the currently set Configuration.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_current_configuration() = 0;

                /**
                 * \return The number of Configuration descriptors.
		 * \see get_configurations_count()
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_config_desc_count() = 0;

                /**
                 * \return The corresponding Configuration descriptorby index.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual configuration_descriptor * STDCALL get_config_desc_by_index(uint16_t config_desc_index) = 0;

                /**
                 * Send a SET_CONFIFURATION command to change the current configuration of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_config_cmd() = 0;

                /**
                 * Send a GET_CONFIGURATION command to \return The current configuration of the AVDECC Entity.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_config_cmd() = 0;
        };
}

#endif
