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
#include "avdecc-lib_build.h"
#include "descriptor_base.h"
#include "avb_interface_descriptor_response.h"
#include "avb_counters_response.h"

namespace avdecc_lib
{
    class avb_interface_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return the avb_interface descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual avb_interface_descriptor_response * STDCALL get_avb_interface_response() = 0;
        
        /**
         * \return the avb_interface descriptor counters response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual avb_counters_response * STDCALL get_avb_interface_counters_response() = 0;
        
        /**
         * Send a GET_COUNTERS command to get the avb_interface counters of the AVDECC Entity.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_counters_cmd(void *notification_id) = 0;
    };
}
