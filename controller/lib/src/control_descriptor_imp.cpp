/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2014 Renkus-Heinz Inc.
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
 * control_descriptor_imp.cpp
 *
 * CONTROL descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "control_descriptor_imp.h"

namespace avdecc_lib
{
    control_descriptor_imp::control_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_control_read(&control_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("control_desc_read error");
        }
    }


    control_descriptor_imp::~control_descriptor_imp()
    {
    }

    uint16_t STDCALL control_descriptor_imp::descriptor_type() const
    {
        assert(control_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CONTROL);
        return control_desc.descriptor_type;
    }

    uint16_t STDCALL control_descriptor_imp::descriptor_index() const
    {
        return control_desc.descriptor_index;
    }

    uint8_t * STDCALL control_descriptor_imp::object_name()
    {
        return control_desc.object_name.value;
    }

    uint16_t STDCALL control_descriptor_imp::localized_description()
    {
        return control_desc.localized_description;
    }

    uint32_t STDCALL control_descriptor_imp::block_latency()
    {
        return control_desc.block_latency;
    }

    uint32_t STDCALL control_descriptor_imp::control_latency()
    {
        return control_desc.control_latency;
    }

    uint16_t STDCALL control_descriptor_imp::control_domain()
    {
        return control_desc.control_domain;
    }

    uint16_t STDCALL control_descriptor_imp::control_value_type()
    {
        return control_desc.control_value_type;
    }

    uint64_t STDCALL control_descriptor_imp::control_type()
    {
        return jdksavdecc_uint64_get(&control_desc.control_type, 0);
    }

    uint32_t STDCALL control_descriptor_imp::reset_time()
    {
        return control_desc.reset_time;
    }

    uint16_t STDCALL control_descriptor_imp::values_offset()
    {
        return control_desc.values_offset;
    }

    uint16_t STDCALL control_descriptor_imp::number_of_values()
    {
        return control_desc.number_of_values;
    }

    uint16_t STDCALL control_descriptor_imp::signal_type()
    {
        return control_desc.signal_type;
    }

    uint16_t STDCALL control_descriptor_imp::signal_index()
    {
        return control_desc.signal_index;
    }

    uint16_t STDCALL control_descriptor_imp::signal_output()
    {
        return control_desc.signal_output;
    }
}
