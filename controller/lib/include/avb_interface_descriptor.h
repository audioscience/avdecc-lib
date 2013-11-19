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
 * The AVB Interface descriptor describes an interface implementing AVB functionality.
 * This may be a wired jack, wireless interface, or other interface providing AVB
 * services.
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
                 * \return The MAC address of the interface.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint8_t * STDCALL get_mac_addr() = 0;

                /**
                 * The flags describing the features of the interface.
		 * 
		 * \return 1 if the interface supports the grandmaster functionality.
		 *	   2 if the interface supports the functionality.
		 *	   3 if the interface supports the "Stream Reservation Protocol (SRP)" functionality.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_interface_flags() = 0;

                /**
                 * \return The clock identity of the interface.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_clock_identity() = 0;

                /**
                 * \return The priority1 field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_priority1() = 0;

                /**
                 * \return The clock class field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_clock_class() = 0;

                /**
                 * \return The offset scaled log variance field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_offset_scaled_log_variance() = 0;

                /**
                 * \return The clock accuracy field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_clock_accuracy() = 0;

                /**
                 * \return The priority2 field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_priority2() = 0;

                /**
                 * \return The domain number field of the grandmaster functionality of the AVB Interface if supporte, 0 otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_domain_number() = 0;

                /**
                 * \return The log sync interval field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_log_sync_interval() = 0;
        };
}

#endif
