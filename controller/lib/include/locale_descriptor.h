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
 * locale_descriptor.h
 *
 * Public LOCALE descriptor interface class
 * The LOCALE descriptor describes a localization of the immutable strings within
 * the AVDECC Entity.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class locale_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * The identifier is a UTF-8 string that contains one to three components such as a
         * language code, a region code, or a variant code, separated by the dash character.
         * Examples of valid locale identifiers are en-US for English in the US, en-AU for
         * English in Australia, haw-US for Hawaiian in the US, and fr-CA for French in Canada.
         *
         * \return The identifier of the LOCALE.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL locale_identifier() = 0;

        /**
         * \return The number of Strings descriptor in this locale. This is the same value for
         *	       all locales in an AVDECC Entity.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_strings() = 0;

        /**
         * \return The descriptor index of the first Strings descriptor for this LOCALE.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_strings() = 0;
    };
}

