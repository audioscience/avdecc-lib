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
 * avb_interface_descriptor_imp.cpp
 *
 * AVB INTERFACE descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "util.h"
#include "end_station_imp.h"
#include "avb_interface_descriptor_imp.h"

namespace avdecc_lib
{
    avb_interface_descriptor_imp::avb_interface_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_avb_interface_read(&avb_interface_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("avb_interface_desc_read error");
        }
    }

    avb_interface_descriptor_imp::~avb_interface_descriptor_imp() {}

    uint16_t STDCALL avb_interface_descriptor_imp::descriptor_type() const
    {
        //assert(avb_interface_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE);
        return avb_interface_desc.descriptor_type;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::descriptor_index() const
    {
        return avb_interface_desc.descriptor_index;
    }

    uint8_t * STDCALL avb_interface_descriptor_imp::object_name()
    {
        return avb_interface_desc.object_name.value;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::localized_description()
    {
        return avb_interface_desc.localized_description;
    }

    uint64_t STDCALL avb_interface_descriptor_imp::mac_addr()
    {
        uint64_t mac_addr;
        utility::convert_eui48_to_uint64(avb_interface_desc.mac_address.value, mac_addr);

        return mac_addr;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::interface_flags()
    {
        return avb_interface_desc.interface_flags;
    }

    uint64_t STDCALL avb_interface_descriptor_imp::clock_identity()
    {
        return jdksavdecc_uint64_get(&avb_interface_desc.clock_identity, 0);
    }

    uint8_t STDCALL avb_interface_descriptor_imp::priority1()
    {
        return avb_interface_desc.priority1;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::clock_class()
    {
        return avb_interface_desc.clock_class;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::offset_scaled_log_variance()
    {
        return avb_interface_desc.offset_scaled_log_variance;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::clock_accuracy()
    {
        return avb_interface_desc.clock_accuracy;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::priority2()
    {
        return avb_interface_desc.priority2;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::domain_number()
    {
        return avb_interface_desc.domain_number;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::log_sync_interval()
    {
        return avb_interface_desc.log_sync_interval;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::log_announce_interval()
    {
        return avb_interface_desc.log_announce_interval;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::log_pdelay_interval()
    {
        return avb_interface_desc.log_pdelay_interval;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::port_number()
    {
        return avb_interface_desc.port_number;
    }
}
