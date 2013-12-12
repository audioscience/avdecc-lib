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
 * net_interface.h
 *
 * Public network interface class
 */

#pragma once

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
    class net_interface
    {
    public:
        /**
         * Call destructor for Network Interface used for destroying objects
         */
        AVDECC_CONTROLLER_LIB32_API virtual void STDCALL destroy() = 0;

        /**
         * \return The number of devices.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL devs_count() = 0;

        /**
         * \return The corresponding network interface description by index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual char * STDCALL get_dev_desc_by_index(size_t dev_index) = 0;

        /**
         * Select the corresponding interface by number.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL select_interface_by_num(uint32_t interface_num) = 0;

        /**
         * Capture a network packet.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL capture_frame(const uint8_t **frame, uint16_t *frame_len) = 0;
    };

    /**
     * Create a public network interface object used for accessing from outside the library.
     */
    extern "C" AVDECC_CONTROLLER_LIB32_API net_interface * STDCALL create_net_interface();
}

