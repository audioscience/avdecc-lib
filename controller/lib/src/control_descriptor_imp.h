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
 * control_descriptor_imp.h
 *
 * CONTROL descriptor implementation class
 */

#pragma once


#include "descriptor_base_imp.h"
#include "control_descriptor.h"

namespace avdecc_lib
{
    class control_descriptor_imp : public control_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_control control_desc; // Structure containing the control_desc fields

    public:
        control_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~control_descriptor_imp();

        //descriptor_base
        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        uint8_t * STDCALL object_name();
        uint16_t STDCALL localized_description();

        //control-specific
        uint32_t STDCALL block_latency();
        uint32_t STDCALL control_latency();
        uint16_t STDCALL control_domain();
        uint16_t STDCALL control_value_type();
        uint64_t STDCALL control_type();
        uint32_t STDCALL reset_time();
        uint16_t STDCALL values_offset();
        uint16_t STDCALL number_of_values();
        uint16_t STDCALL signal_type();
        uint16_t STDCALL signal_index();
        uint16_t STDCALL signal_output();

    };
}

