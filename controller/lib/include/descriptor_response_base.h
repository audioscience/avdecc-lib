/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2015 AudioScience Inc.
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
 * descriptor_response_base.h
 *
 * Public descriptor response base interface class
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"

namespace avdecc_lib
{
    struct avdecc_lib_name_string64
    {
        uint8_t value[64];
    };

    class descriptor_response_base
    {
    public:
        virtual ~descriptor_response_base() {};
        /**
         * \return The name of the descriptor object. This may be user set through the use of a SET_NAME command.
         *	   The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         *	   defined value being provided in a localized form via the localized descripton field. By leaving
         *	   this field blank an AVDECC Controller can determine if the user has overridden the name and can
         *	   use this name rather than the localized name.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL object_name() = 0;
    };
}