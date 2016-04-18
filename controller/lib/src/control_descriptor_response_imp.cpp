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
 * control_descriptor_response_imp.cpp
 *
 * control descriptor response implementation
 */

#include <vector>
#include "control_descriptor_response_imp.h"
#include "util.h"

namespace avdecc_lib
{
    control_descriptor_response_imp::control_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
        descriptor_response_base_imp(frame, frame_len, pos) {}
    
    control_descriptor_response_imp::~control_descriptor_response_imp() {}
    
    uint8_t * STDCALL control_descriptor_response_imp::object_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_CONTROL_OFFSET_OBJECT_NAME];
    }
    
    uint16_t STDCALL control_descriptor_response_imp::localized_description()
    {
        return jdksavdecc_descriptor_control_get_localized_description(buffer, position);
    }
    
    uint32_t STDCALL control_descriptor_response_imp::block_latency()
    {
        return jdksavdecc_descriptor_control_get_block_latency(buffer, position);
    }
    
    uint32_t STDCALL control_descriptor_response_imp::control_latency()
    {
        return jdksavdecc_descriptor_control_get_control_latency(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::control_domain()
    {
        return jdksavdecc_descriptor_control_get_control_domain(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::control_value_type()
    {
        return jdksavdecc_descriptor_control_get_control_value_type(buffer, position);
    }
    
    uint64_t STDCALL control_descriptor_response_imp::control_type()
    {
        uint64_t control_type;
        
        utility::convert_eui48_to_uint64(&buffer[position +
                                                 JDKSAVDECC_DESCRIPTOR_CONTROL_OFFSET_CONTROL_TYPE], control_type);
        return control_type;
    }
    
    uint32_t STDCALL control_descriptor_response_imp::reset_time()
    {
        return jdksavdecc_descriptor_control_get_reset_time(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::values_offset()
    {
        return jdksavdecc_descriptor_control_get_values_offset(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::number_of_values()
    {
        return jdksavdecc_descriptor_control_get_number_of_values(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::signal_type()
    {
        return jdksavdecc_descriptor_control_get_signal_type(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::signal_index()
    {
        return jdksavdecc_descriptor_control_get_signal_index(buffer, position);
    }
    
    uint16_t STDCALL control_descriptor_response_imp::signal_output()
    {
        return jdksavdecc_descriptor_control_get_signal_output(buffer, position);
    }
}
