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
 * avb_interface_descriptor_response_imp.cpp
 *
 * AVB INTERFACE descriptor response implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "avb_interface_descriptor_response_imp.h"
#include "end_station_imp.h"
#include "util.h"

namespace avdecc_lib
{
    avb_interface_descriptor_response_imp::avb_interface_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
        descriptor_response_base_imp(frame, frame_len, pos) {}
    
    avb_interface_descriptor_response_imp::~avb_interface_descriptor_response_imp() {}
    
    uint8_t * STDCALL avb_interface_descriptor_response_imp::object_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE_OFFSET_OBJECT_NAME];
    }
    uint16_t STDCALL avb_interface_descriptor_response_imp::localized_description()
    {
        return jdksavdecc_descriptor_avb_interface_get_localized_description(buffer, position);
    }
    uint64_t STDCALL avb_interface_descriptor_response_imp::mac_addr()
    {
        uint64_t mac_addr;
        utility::convert_eui48_to_uint64(&buffer[position +
                                JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE_OFFSET_MAC_ADDRESS], mac_addr);
        return mac_addr;
    }
    uint16_t STDCALL avb_interface_descriptor_response_imp::interface_flags()
    {
        return jdksavdecc_descriptor_avb_interface_get_interface_flags(buffer, position);
    }
    uint64_t STDCALL avb_interface_descriptor_response_imp::clock_identity()
    {
        return jdksavdecc_uint64_get(buffer, position +
                                     JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE_OFFSET_CLOCK_IDENTITY);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::priority1()
    {
        return jdksavdecc_descriptor_avb_interface_get_priority1(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::clock_class()
    {
        return jdksavdecc_descriptor_avb_interface_get_clock_class(buffer, position);
    }
    uint16_t STDCALL avb_interface_descriptor_response_imp::offset_scaled_log_variance()
    {
        return jdksavdecc_descriptor_avb_interface_get_offset_scaled_log_variance(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::clock_accuracy()
    {
        return jdksavdecc_descriptor_avb_interface_get_clock_accuracy(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::priority2()
    {
        return jdksavdecc_descriptor_avb_interface_get_priority2(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::domain_number()
    {
        return jdksavdecc_descriptor_avb_interface_get_domain_number(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::log_sync_interval()
    {
        return jdksavdecc_descriptor_avb_interface_get_log_sync_interval(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::log_announce_interval()
    {
        return jdksavdecc_descriptor_avb_interface_get_log_announce_interval(buffer, position);
    }
    uint8_t STDCALL avb_interface_descriptor_response_imp::log_pdelay_interval()
    {
        return jdksavdecc_descriptor_avb_interface_get_log_pdelay_interval(buffer, position);
    }
    uint16_t STDCALL avb_interface_descriptor_response_imp::port_number()
    {
        return jdksavdecc_descriptor_avb_interface_get_port_number(buffer, position);
    }
}
