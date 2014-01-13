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
 * jack_input_descriptor.h
 *
 * Public JACK INPUT descriptor interface class
 * The JACK INPUT descriptor describes an Input Jack.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class jack_input_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The flags describing the capabilities or features of the Jack.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL jack_flags() = 0;

        /**
         * Check if the jack can be used as a clock synchronization source.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL jack_flag_clock_sync_source() = 0;

        /**
         * Check if the jack connection is hardwired, cannot be disconnected and
         * may be physically within the device's structure.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL jack_flag_captive() = 0;

        /**
         * \return The type of the jack.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL jack_type() = 0;

        /**
         * \return The number of controls within this jack.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_controls() = 0;

        /**
         * \return The index of the first Control descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_control() = 0;
    };
}

