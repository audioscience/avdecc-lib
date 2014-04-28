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
 * external_port_input_descriptor.h
 *
 * Public EXTERNAL PORT INPUT descriptor interface class
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class external_port_input_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The flags describing the capabilities or features of the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL port_flags() = 0;

        /**
         * The index of the CLOCK_DOMAIN descriptor that describes the clock domain for the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL clock_domain_index() = 0;

        /**
        * \return The number of controls within this jack.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_controls() = 0;

        /**
        * \return The index of the first Control descriptor.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_control() = 0;

        /**
         * \return The signal type.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_type() = 0;

        /**
        * \return The signal index.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_index() = 0;

        /**
        * \return The signal output.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_output() = 0;

        /**
        * \return Latency in nanoseconds.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL block_latency() = 0;

        /**
        * \return The index of the jack connected to the port.
        */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL jack_index() = 0;
    };
}

