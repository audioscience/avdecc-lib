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
 * Public AVB INTERFACE descriptor interface class
 * The AVB INTERFACE descriptor describes an interface implementing AVB functionality.
 * This may be a wired jack, wireless interface, or other interface providing AVB
 * services.
 */

#pragma once


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
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL mac_addr() = 0;

        /**
         * The flags describing the features of the interface.
         *
         * \return 1 (GPTP Grandmaster Supported) if the interface supports the grandmaster functionality.
         * \return 2 (GPTP Supported) if the interface supports the functionality.
         * \return 4 (SRP Supported) if the interface supports the "Stream Reservation Protocol (SRP)" functionality.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL interface_flags() = 0;

        /**
         * \return The clock identity of the interface.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL clock_identity() = 0;

        /**
         * \return The priority1 field of the grandmaster functionality of the AVB INTERFACE if supported, 0xff otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL priority1() = 0;

        /**
         * \return The clock class field of the grandmaster functionality of the AVB INTERFACE if supported, 0xff otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL clock_class() = 0;

        /**
         * \return The offset scaled log variance field of the grandmaster functionality of the AVB INTERFACE if supported, 0 otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL offset_scaled_log_variance() = 0;

        /**
         * \return The clock accuracy field of the grandmaster functionality of the AVB INTERFACE if supported, 0xff otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL clock_accuracy() = 0;

        /**
         * \return The priority2 field of the grandmaster functionality of the AVB INTERFACE if supported, 0xff otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL priority2() = 0;

        /**
         * \return The domain number field of the grandmaster functionality of the AVB INTERFACE if supported, 0 otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL domain_number() = 0;

        /**
         * \return The current log sync interval field of the grandmaster functionality of the AVB INTERFACE if supported, 0 otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL log_sync_interval() = 0;

        /**
         * \return The current log announce interval field of the grandmaster functionality of the AVB INTERFACE if supported, 0 otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL log_announce_interval() = 0;

        /**
         * \return The current log pdelay interval field of the grandmaster functionality of the AVB INTERFACE if supported, 0 otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL log_pdelay_interval() = 0;

        /**
         * \return The port number field as used by the functionality of the AVB INTERFACE if supported, 0 otherwise.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL port_number() = 0;
    };
}

