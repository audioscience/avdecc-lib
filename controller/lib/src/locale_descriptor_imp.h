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
 * locale_descriptor_imp.h
 *
 * LOCALE descriptor implementation class
 */

#pragma once


#include "descriptor_base_imp.h"
#include "locale_descriptor.h"

namespace avdecc_lib
{
    class locale_descriptor_imp : public locale_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_locale locale_desc; // Structure containing the locale_desc fields
        ssize_t desc_locale_read_returned; // Status of extracting LOCALE descriptor information from a network buffer

    public:
        /**
         * Constructor for LOCALE descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        locale_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~locale_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the identifier of the LOCALE. The identifier is a UTF-8 string that
         * contains one to three components such as a language code, a region code,
         * or a variant code, separated by the dash character. Examples of valid
         * LOCALE identifiers are en-US for English in the US, en-AU for English in
         * Australia, and fr-CA for French in Canada.
         */
        uint8_t * STDCALL locale_identifier();

        /**
         * Get the number of Strings descriptor in this LOCALE. This is the same value for
         * all locales in an AVDECC Entity.
         */
        uint16_t STDCALL number_of_strings();

        /**
         * Get the descriptor index of the first Strings descriptor for this LOCALE.
         */
        uint16_t STDCALL base_strings();
    };
}

