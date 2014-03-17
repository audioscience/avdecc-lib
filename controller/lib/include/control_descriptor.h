/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2014 Renkus-Heinz Inc.
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
 * control_descriptor.h
 *
 * Public CONTROL descriptor interface class
 * The CONTROL descriptor describes a generic Control.
  */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class control_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return This is the latency in nanoseconds between the output of the previous
         *		   block and its output. The previous block is the object identified by the
         *		   signal_type and signal_index fields. For a DELAY Control, the value
         *		   of the delay is not included in this value.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL block_latency() = 0;
        
        /**
         * \return The worst-case time in microseconds from when a Control value
         *		change is received and when the Control has completely switched to
         *		the new value.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL control_latency() = 0;
        
        /**
         * \return The domain that this Control belongs to.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL control_domain() = 0;
        
        /**
         * \return The type of the value contained in the Control as defined in 7.3.5.1
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL control_value_type() = 0;
        
        /**
         * \return The type of the Control.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL control_type() = 0;
        
        /**
         * \return The time period in milliseconds from when a Control is set with the 
         *  SET_CONTROL command until it automatically resets to its default values.
         *  When this is zero (0), automatic resets do not happen.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL reset_time() = 0;
        
        /**
         * \return The offset from the start of the descriptor for the first octet
         *  of the value_details. The field is 104 for this version of AEM.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL values_offset() = 0;
        
        /**
         * \return The number of value settings this Control has.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_values() = 0;
        
        /**
         * \return The descriptor_type for the signal source of the Control.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_type() = 0;
        
        /**
         * \return The descriptor_index for the signal source of the Control.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_index() = 0;
        
        /**
         * \return The index of the output of the signal source of the Control.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_output() = 0;
    };
}
