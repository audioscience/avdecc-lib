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
 * descriptor_field.h
 *
 * Public descriptor base interface class
 */

#pragma once

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
    class descriptor_field_flags;
    class descriptor_field
    {
    public:
        enum aem_desc_field_types /// The descriptor field types
        {
            TYPE_INVALID,
            TYPE_CHAR,
            TYPE_UINT16,
            TYPE_UINT32,
            TYPE_FLAGS16,
            TYPE_FLAGS32
        };

        /**
         * \return The name of the descriptor field.
         */
        AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL get_name() const = 0;

        /**
        * \return The type of the descriptor field.
        */
        AVDECC_CONTROLLER_LIB32_API virtual enum aem_desc_field_types STDCALL get_type() const = 0;

        /**
         * \return The value of the descriptor as a char pointer.
         */
        AVDECC_CONTROLLER_LIB32_API virtual char * STDCALL get_char() const = 0;

        /**
        * \return The value of the descriptor as a uint16_t type.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_uint16() const = 0;

        /**
        * \return The value of the descriptor as a uint32_t type.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_uint32() const = 0;

        /**
        * \return The value of the descriptor field of type flags.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_flags() const = 0;

        /**
        * \return The number flags for descriptor field of type flags.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_flags_count() const = 0;

        /**
        * \return Get flag details.
        */
        AVDECC_CONTROLLER_LIB32_API virtual descriptor_field_flags * STDCALL get_flag_by_index(uint32_t index) const = 0;
    };
}

