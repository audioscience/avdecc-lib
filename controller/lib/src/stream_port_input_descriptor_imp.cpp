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
 * stream_port_input_descriptor_imp.cpp
 *
 * Stream Port Input descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "stream_port_input_descriptor_imp.h"

namespace avdecc_lib
{
    stream_port_input_descriptor_imp::stream_port_input_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_stream_port_read(&stream_port_input_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("stream_port_input_desc_read error");
        }
    }

    stream_port_input_descriptor_imp::~stream_port_input_descriptor_imp() {}

    uint16_t STDCALL stream_port_input_descriptor_imp::descriptor_type() const
    {
        assert(stream_port_input_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT);
        return stream_port_input_desc.descriptor_type;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::descriptor_index() const
    {
        //assert(stream_port_input_desc.descriptor_index == 0);
        return stream_port_input_desc.descriptor_index;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::clock_domain_index()
    {
        return stream_port_input_desc.clock_domain_index;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::port_flags()
    {
        return stream_port_input_desc.port_flags;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::number_of_controls()
    {
        return stream_port_input_desc.number_of_controls;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::base_control()
    {
        return stream_port_input_desc.base_control;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::number_of_clusters()
    {
        return stream_port_input_desc.number_of_clusters;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::base_cluster()
    {
        return stream_port_input_desc.base_cluster;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::number_of_maps()
    {
        return stream_port_input_desc.number_of_maps;
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::base_map()
    {
        return stream_port_input_desc.base_map;
    }
}
