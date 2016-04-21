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
 * avb_interface_descriptor_response_imp.h
 *
 * avb_interface response implementation class
 */

#pragma once

#include "avb_interface_descriptor_response.h"
#include "jdksavdecc_aem_descriptor.h"
#include "descriptor_response_base_imp.h"

namespace avdecc_lib
{
class avb_interface_descriptor_response_imp : public avb_interface_descriptor_response, public virtual descriptor_response_base_imp
{
public:
    avb_interface_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos);
    virtual ~avb_interface_descriptor_response_imp();

    uint8_t * STDCALL object_name();
    uint16_t STDCALL localized_description();
    uint64_t STDCALL mac_addr();
    uint16_t STDCALL interface_flags();
    uint64_t STDCALL clock_identity();
    uint8_t STDCALL priority1();
    uint8_t STDCALL clock_class();
    uint16_t STDCALL offset_scaled_log_variance();
    uint8_t STDCALL clock_accuracy();
    uint8_t STDCALL priority2();
    uint8_t STDCALL domain_number();
    uint8_t STDCALL log_sync_interval();
    uint8_t STDCALL log_announce_interval();
    uint8_t STDCALL log_pdelay_interval();
    uint16_t STDCALL port_number();
};
}
