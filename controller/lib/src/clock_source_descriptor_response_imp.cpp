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
 * clock_source_descriptor_response_imp.cpp
 *
 * CLOCK source descriptor response implementation
 */

#include <vector>
#include "clock_source_descriptor_response_imp.h"
#include "util.h"

namespace avdecc_lib
{
clock_source_descriptor_response_imp::clock_source_descriptor_response_imp(const uint8_t * frame, size_t frame_len, ssize_t pos) : descriptor_response_base_imp(frame, frame_len, pos) {}

clock_source_descriptor_response_imp::~clock_source_descriptor_response_imp() {}

uint8_t * STDCALL clock_source_descriptor_response_imp::object_name()
{
    return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE_OFFSET_OBJECT_NAME];
}

uint16_t STDCALL clock_source_descriptor_response_imp::localized_description()
{
    return jdksavdecc_descriptor_clock_source_get_localized_description(buffer, position);
}

uint16_t STDCALL clock_source_descriptor_response_imp::clock_source_flags()
{
    return jdksavdecc_descriptor_clock_source_get_clock_source_flags(buffer, position);
}

uint16_t STDCALL clock_source_descriptor_response_imp::clock_source_type()
{
    return jdksavdecc_descriptor_clock_source_get_clock_source_type(buffer, position);
}

uint64_t STDCALL clock_source_descriptor_response_imp::clock_source_identifier()
{
    uint64_t clock_source_identifier;

    utility::convert_eui48_to_uint64(&buffer[position +
                                             JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE_OFFSET_CLOCK_SOURCE_IDENTIFIER],
                                     clock_source_identifier);
    return clock_source_identifier;
}

uint16_t STDCALL clock_source_descriptor_response_imp::clock_source_location_type()
{
    return jdksavdecc_descriptor_clock_source_get_clock_source_location_type(buffer, position);
}

uint16_t STDCALL clock_source_descriptor_response_imp::clock_source_location_index()
{
    return jdksavdecc_descriptor_clock_source_get_clock_source_location_index(buffer, position);
}
}
