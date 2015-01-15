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
 * stream_input_get_rx_state_response_imp.h
 *
 * STREAM INPUT rx state response implementation class
 */

#pragma once

#include "stream_input_get_rx_state_response.h"
#include "jdksavdecc_acmp.h"

namespace avdecc_lib
{
    class stream_input_get_rx_state_response_imp : public stream_input_get_rx_state_response
    {
    private:
        uint8_t * m_frame;
        size_t m_size;
        ssize_t m_position;
    public:
        stream_input_get_rx_state_response_imp(uint8_t *frame, size_t frame_len, ssize_t pos);
        virtual ~stream_input_get_rx_state_response_imp();
        
        uint64_t STDCALL get_rx_state_stream_id();
        uint16_t STDCALL get_rx_state_talker_unique_id();
        uint16_t STDCALL get_rx_state_listener_unique_id();
        uint64_t STDCALL get_rx_state_stream_dest_mac();
        uint16_t STDCALL get_rx_state_connection_count();
        uint16_t STDCALL get_rx_state_flags();
        uint16_t STDCALL get_rx_state_stream_vlan_id();
    };
}
