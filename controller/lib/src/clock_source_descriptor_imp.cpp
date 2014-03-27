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
 * clock_source_descriptor_imp.cpp
 *
 * Clock Source_ descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "clock_source_descriptor_imp.h"

namespace avdecc_lib
{
    clock_source_descriptor_imp::clock_source_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_clock_source_read(&clock_source_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("clock_source_desc_read error");
        }
    }

    clock_source_descriptor_imp::~clock_source_descriptor_imp() {}

    uint16_t STDCALL clock_source_descriptor_imp::descriptor_type() const
    {
        assert(clock_source_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE);
        return clock_source_desc.descriptor_type;
    }

    uint16_t STDCALL clock_source_descriptor_imp::descriptor_index() const
    {
        return clock_source_desc.descriptor_index;
    }

    uint8_t * STDCALL clock_source_descriptor_imp::object_name()
    {
        return clock_source_desc.object_name.value;
    }

    uint16_t STDCALL clock_source_descriptor_imp::localized_description()
    {
        return clock_source_desc.localized_description;
    }

    uint16_t STDCALL clock_source_descriptor_imp::clock_source_flags()
    {
        return clock_source_desc.clock_source_flags;
    }

    uint16_t STDCALL clock_source_descriptor_imp::clock_source_type()
    {
        return clock_source_desc.clock_source_type;
    }

    uint64_t STDCALL clock_source_descriptor_imp::clock_source_identifier()
    {
        return jdksavdecc_uint64_get(&clock_source_desc.clock_source_identifier, 0);
    }

    uint16_t STDCALL clock_source_descriptor_imp::clock_source_location_type()
    {
        return clock_source_desc.clock_source_location_type;
    }

    uint16_t STDCALL clock_source_descriptor_imp::clock_source_location_index()
    {
        return clock_source_desc.clock_source_location_index;
    }
}
