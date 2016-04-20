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
 * stream_input_get_rx_state_response.h
 *
 * STREAM INPUT rx state response base class
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"

namespace avdecc_lib
{
    class stream_input_get_rx_state_response
    {
    public:
        virtual ~stream_input_get_rx_state_response() {};
        /**
         * \return The stream id field used to identify and transfer the associated stream ID where suitable
         *         after sending a GET_RX_STATE command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_rx_state_stream_id() = 0;

        /**
         * \return The Talker unique ID used to uniquely identify the stream source of the AVDECC Talker
         *         after sending a GET_RX_STATE command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_rx_state_talker_unique_id() = 0;

        /**
         * \return The Listener unique ID used to uniquely identify the stream sink of the AVDECC Listener
         *         after sending a GET_RX_STATE command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_rx_state_listener_unique_id() = 0;

        /**
         * \return The stream destination MAC address used to convey the destination MAC address for a stream
         *         from the AVDECC Talker to the AVDECC Listener, or from either to the AVDECC Controller after
         *         sending a GET_RX_STATE command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_rx_state_stream_dest_mac() = 0;

        /**
         * \return The connection count used by the state commands to return the number of connections an AVDECC Talker
         *         thinks it has on its stream source after sending a GET_RX_STATE command and receiving a response
         *         back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_rx_state_connection_count() = 0;

        /**
         * \return The flags used to indicate attributes of the connection or saved state after sending a GET_RX_STATE
         *         command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_rx_state_flags() = 0;

        /**
         * \return The stream vlan id used to convey the VLAN ID for a stream from the AVDECC Talker to the AVDECC Listener,
         *         or from either to the AVDECC Controller after sending a GET_RX_STATE command and receiving a response
         *         back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_rx_state_stream_vlan_id() = 0;
    };
}
