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
 * stream_output_get_stream_info_response.h
 *
 * STREAM OUTPUT stream info response base class
 */

#pragma once

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
    class stream_output_get_stream_info_response
    {
    public:
        virtual ~stream_output_get_stream_info_response(){};
        /**
         * \return The stream info flags of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_flags() = 0;
        
        /**
         * \return The stream info stream format of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_format() = 0;
        
        /**
         * \return The stream info stream id of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_id() = 0;
        
        /**
         * \return The stream info MSRP accumulated latency of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_msrp_accumulated_latency() = 0;
        
        /**
         * \return The stream info stream destination MAC of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_dest_mac() = 0;
        
        /**
         * \return The stream info MSRP failure code of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_stream_info_msrp_failure_code() = 0;
        
        /**
         * \return The stream info MSRP failure bridge id of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_msrp_failure_bridge_id() = 0;
        
        /**
         * \return The stream info vlan id of a stream after sending a GET_STREAM_INFO command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_stream_info_stream_vlan_id() = 0;
    };
}
