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
 * Public ENTITY descriptor interface class
 * The ENTITY descriptor describes the highest level of the AVDECC Entity. It repeats some of the information
 * contained within the ADP advertise for the AVDECC Entity as well as the information required to read the
 * rest of the descriptors from the AVDECC Entity.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"
#include "entity_descriptor_response.h"

namespace avdecc_lib
{
    class configuration_descriptor;

    class entity_descriptor : public virtual descriptor_base
    {
    public:
        
        /**
         * \return the entity descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual entity_descriptor_response * STDCALL get_entity_response() = 0;
        /**
         * \return The number of Configuration descriptors.
         * \see configurations_count()
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t STDCALL config_desc_count() = 0;

        /**
         * \return The corresponding Configuration descriptor by index.
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

