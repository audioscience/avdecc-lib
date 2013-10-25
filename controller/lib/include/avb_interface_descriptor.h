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
 * avb_interface_descriptor.h
 *
 * Public AVB Interface descriptor interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AVB_INTERFACE_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_AVB_INTERFACE_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
        class avb_interface_descriptor : public virtual descriptor_base
        {
        public:
                /**
                 * Get the object_name of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_object_name() = 0;

                /**
                 * Get the localized_description of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_localized_description() = 0;

                /**
                 * Get the mac_address of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual struct jdksavdecc_eui48 STDCALL get_mac_address() = 0;

                /**
                 * Get the interface_flags of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_interface_flags() = 0;

                /**
                 * Get the clock_identity of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_clock_identity() = 0;

                /**
                 * Get the priority1 of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_priority1() = 0;

                /**
                 * Get the clock_class of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_clock_class() = 0;

                /**
                 * Get the offset_scaled_log_variance of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_offset_scaled_log_variance() = 0;

                /**
                 * Get the clock_accuracy of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_clock_accuracy() = 0;

                /**
                 * Get the priority2 of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_priority2() = 0;

                /**
                 * Get the domain_number of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_domain_number() = 0;

                /**
                 * Get the log_sync_interval of the avb_interface_descriptor object.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_log_sync_interval() = 0;
        };
}

#endif