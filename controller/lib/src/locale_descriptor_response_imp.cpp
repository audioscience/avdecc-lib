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
 * locale_descriptor_response_imp.cpp
 *
 * LOCALE descriptor response implementation
 */
#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "locale_descriptor_response_imp.h"
#include "end_station_imp.h"

namespace avdecc_lib
{
    locale_descriptor_response_imp::locale_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos)
    {
        frame_size = frame_len;
        buffer = (uint8_t *)malloc(frame_size * sizeof(uint8_t));
        memcpy(buffer, frame, frame_size);
        position = pos;
    }

    locale_descriptor_response_imp::~locale_descriptor_response_imp()
    {
        free(buffer);
    }

    uint8_t * STDCALL locale_descriptor_response_imp::locale_identifier()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_LOCALE_OFFSET_LOCALE_IDENTIFIER];
    }

    uint16_t STDCALL locale_descriptor_response_imp::number_of_strings()
    {
        return jdksavdecc_descriptor_locale_get_number_of_strings(buffer, position);
    }

    uint16_t STDCALL locale_descriptor_response_imp::base_strings()
    {
        return jdksavdecc_descriptor_locale_get_base_strings(buffer, position);
    }
}
