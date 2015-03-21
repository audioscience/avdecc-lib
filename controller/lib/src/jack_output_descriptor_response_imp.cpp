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
 * jack_output_descriptor_response_imp.cpp
 *
 * JACK OUTPUT descriptor response implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "jack_output_descriptor_response_imp.h"

namespace avdecc_lib
{
    jack_output_descriptor_response_imp::jack_output_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
        descriptor_response_base_imp(frame, frame_len, pos)
    {
        jack_flags_init();
    }
    
    jack_output_descriptor_response_imp::~jack_output_descriptor_response_imp() {}
    
    void jack_output_descriptor_response_imp::jack_flags_init()
    {
        jack_output_flags.clock_sync_source = jack_flags() >> 1 & 0x01;
        jack_output_flags.captive = jack_flags() >> 2 & 0x01;
    }
    
    uint8_t * STDCALL jack_output_descriptor_response_imp::object_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_JACK_OFFSET_OBJECT_NAME];
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::localized_description()
    {
        return jdksavdecc_descriptor_jack_get_localized_description(buffer, position);
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::jack_flags()
    {
        return jdksavdecc_descriptor_jack_get_jack_flags(buffer, position);
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::jack_flag_clock_sync_source()
    {
        return jack_output_flags.clock_sync_source;
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::jack_flag_captive()
    {
        return jack_output_flags.captive;
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::jack_type()
    {
        return jdksavdecc_descriptor_jack_get_jack_type(buffer, position);
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::number_of_controls()
    {
        return jdksavdecc_descriptor_jack_get_number_of_controls(buffer, position);
    }
    
    uint16_t STDCALL jack_output_descriptor_response_imp::base_control()
    {
        return jdksavdecc_descriptor_jack_get_base_control(buffer, position);
    }
}
