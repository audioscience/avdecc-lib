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
    stream_port_input_descriptor_imp::stream_port_input_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos) {}

    stream_port_input_descriptor_imp::~stream_port_input_descriptor_imp() {}

    stream_port_input_descriptor_response * STDCALL stream_port_input_descriptor_imp::get_stream_port_input_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return resp = new stream_port_input_descriptor_response_imp(resp_ref->get_buffer(),
                                                             resp_ref->get_size(), resp_ref->get_pos());
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::descriptor_type() const
    {
        assert(jdksavdecc_descriptor_stream_port_get_descriptor_type(resp_ref->get_buffer(), resp_ref->get_pos()) == JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT);
        return jdksavdecc_descriptor_stream_port_get_descriptor_type(resp_ref->get_buffer(), resp_ref->get_pos());
    }

    uint16_t STDCALL stream_port_input_descriptor_imp::descriptor_index() const
    {
        return jdksavdecc_descriptor_stream_port_get_descriptor_index(resp_ref->get_buffer(), resp_ref->get_pos());
    }
}
