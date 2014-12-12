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
 * external_port_output_descriptor_response_imp.cpp
 *
 * EXTERNAL_PORT_OUTPUT descriptor response implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "external_port_output_descriptor_response_imp.h"

namespace avdecc_lib
{
    external_port_output_descriptor_response_imp::external_port_output_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos)
    {
        frame_size = frame_len;
        buffer = (uint8_t *)malloc(frame_size * sizeof(uint8_t));
        memcpy(buffer, frame, frame_size);
        position = pos;
    }
    
    external_port_output_descriptor_response_imp::~external_port_output_descriptor_response_imp()
    {
        free(buffer);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::port_flags()
    {
        return jdksavdecc_descriptor_external_port_get_port_flags(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::clock_domain_index()
    {
        return jdksavdecc_descriptor_external_port_get_clock_domain_index(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::number_of_controls()
    {
        return jdksavdecc_descriptor_external_port_get_number_of_controls(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::base_control()
    {
        return jdksavdecc_descriptor_external_port_get_base_control(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::signal_type()
    {
        return jdksavdecc_descriptor_external_port_get_signal_type(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::signal_index()
    {
        return jdksavdecc_descriptor_external_port_get_signal_index(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::signal_output()
    {
        return jdksavdecc_descriptor_external_port_get_signal_output(buffer, position);
    }
    
    uint32_t STDCALL external_port_output_descriptor_response_imp::block_latency()
    {
        return jdksavdecc_descriptor_external_port_get_block_latency(buffer, position);
    }
    
    uint16_t STDCALL external_port_output_descriptor_response_imp::jack_index()
    {
        return jdksavdecc_descriptor_external_port_get_jack_index(buffer, position);
    }
}
