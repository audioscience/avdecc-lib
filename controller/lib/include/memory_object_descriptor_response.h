/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2014 AudioScience Inc.
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
 * memory_object_descriptor_response.h
 *
 * Public MEMORY_OBJECT descriptor response interface class
 * The MEMORY_OBJECT descriptor describes a Memory Object.
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"
#include "descriptor_response_base.h"

namespace avdecc_lib
{
    class memory_object_descriptor_response : public virtual descriptor_response_base
    {
    public:
        virtual ~memory_object_descriptor_response() {};

        /**
         * \return The localized string reference pointing to the localized descriptor name.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL localized_description() = 0;

        /**
         * \return The type of the Memory Object
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL memory_object_type() = 0;

        /**
         * \return The descriptor_type of the object that is the target of the memory region.
         *         This is the object to which the settings, log file, or firmware applies.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL target_descriptor_type() = 0;

        /**
         * \return The descriptor_index of the object that is the target of the memory region.
         *         This is the object to which the settings, log file, or firmware applies.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL target_descriptor_index() = 0;

        /**
         * \return The 64-bit start address used for reading or writing the object’s data
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL start_address() = 0;

        /**
         * \return The 64-bit maximum length of the Memory Object.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL maximum_length() = 0;

        /**
         * \return The 64-bit actual length of the Memory Object.
         *         This value will change and will reflect the actual size of the data contained
         *         in the memory region described by this Memory Object.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL length() = 0;

        /**
         * \return String representation of the memory_object_type
         */
        AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL memory_object_type_to_str() = 0;
    };
}

