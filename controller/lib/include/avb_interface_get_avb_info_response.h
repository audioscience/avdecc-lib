/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2015 AudioScience Inc.
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
 * avb_interface_get_avb_info_response.h
 *
 * AVB INTERFACE get_avb_info response base class
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"

namespace avdecc_lib
{
    class avb_interface_get_avb_info_response
    {
    public:
        virtual ~avb_interface_get_avb_info_response() {};

        ///
        /// \return the ClockIdentity of the current IEEE Std 802.1AS-2011 grandmaster as elected on this AVB Interface.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_gptp_grandmaster_id() = 0;

        ///
        /// \return the propagation delay in nanoseconds as reported by the IEEE Std 802.1AS-2011 pDelay mechanism.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_propagation_delay() = 0;

        ///
        /// \return the domainNumber of the current IEEE Std 802.1AS-2011 grandmaster as elected on this AVB Interface.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_gptp_domain_number() = 0;

        ///
        /// \return the number of mappings present in the msrp_mappings field.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_msrp_mappings_count() = 0;

        ///
        /// \return True if the IEEE Std 802.1AS-2011 variable asCapable is set on this interface.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_avb_info_flags_as_capable() = 0;

        ///
        /// \return True if the interface has the IEEE Std 802.1AS- 2011 functionality enabled.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_avb_info_flags_gptp_enabled() = 0;

        ///
        /// \return True if the interface has the IEEE Std 802.1Q- 2011 Clause 35,
        ///  “Stream Reservation Protocol (SRP)” functionality enabled.
        ///
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_avb_info_flags_srp_enabled() = 0;
    };
}
