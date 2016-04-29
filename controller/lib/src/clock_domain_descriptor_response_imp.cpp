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
 * clock_domain_descriptor_response_imp.cpp
 *
 * CLOCK DOMAIN descriptor response implementation
 */

#include <cstddef>
#include <vector>
#include "clock_domain_descriptor_response_imp.h"

namespace avdecc_lib
{
clock_domain_descriptor_response_imp::clock_domain_descriptor_response_imp(const uint8_t * frame, size_t frame_len, ssize_t pos) : descriptor_response_base_imp(frame, frame_len, pos)
{
    store_clock_sources();
}

clock_domain_descriptor_response_imp::~clock_domain_descriptor_response_imp() {}

uint8_t * STDCALL clock_domain_descriptor_response_imp::object_name()
{
    return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN_OFFSET_OBJECT_NAME];
}

uint16_t STDCALL clock_domain_descriptor_response_imp::localized_description()
{
    return jdksavdecc_descriptor_clock_domain_get_localized_description(buffer, position);
}

uint16_t STDCALL clock_domain_descriptor_response_imp::clock_source_index()
{
    return jdksavdecc_descriptor_clock_domain_get_clock_source_index(buffer, position);
}

uint16_t clock_domain_descriptor_response_imp::clock_sources_offset()
{
    assert(jdksavdecc_descriptor_clock_domain_get_clock_sources_offset(buffer, position) == 76);
    return jdksavdecc_descriptor_clock_domain_get_clock_sources_offset(buffer, position);
}

uint16_t STDCALL clock_domain_descriptor_response_imp::clock_sources_count()
{
    assert(jdksavdecc_descriptor_clock_domain_get_clock_sources_count(buffer, position) <= 249);
    return jdksavdecc_descriptor_clock_domain_get_clock_sources_count(buffer, position);
}

void clock_domain_descriptor_response_imp::store_clock_sources()
{
    uint16_t offset = 0;

    for (uint32_t i = 0; i < clock_sources_count(); i++)
    {
        clk_src_vec.push_back(jdksavdecc_uint16_get(buffer, clock_sources_offset() + position + offset));
        offset += 0x2;
    }
}

uint16_t STDCALL clock_domain_descriptor_response_imp::get_clock_source_by_index(size_t clk_src_index)
{
    return clk_src_vec.at(clk_src_index);
}
}
