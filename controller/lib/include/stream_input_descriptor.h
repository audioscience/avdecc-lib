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
 * stream_input_descriptor.h
 *
 * Public STREAM INPUT descriptor interface
 * The STREAM INPUT descriptor describes a sourced or sinked stream.
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"
#include "descriptor_base.h"
#include "stream_input_counters_response.h"
#include "stream_input_descriptor_response.h"
#include "stream_input_get_stream_format_response.h"
#include "stream_input_get_stream_info_response.h"
#include "stream_input_get_rx_state_response.h"

namespace avdecc_lib
{
    class stream_input_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return the stream_input descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_input_descriptor_response * STDCALL get_stream_input_response() = 0;

        /**
         * \return the stream_input descriptor counters response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_input_counters_response * STDCALL get_stream_input_counters_response() = 0;

        /**
         * \return the stream_input get_stream_format response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_input_get_stream_format_response * STDCALL get_stream_input_get_stream_format_response() = 0;

        /**
         * \return the stream_input get_stream_format response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_input_get_stream_info_response * STDCALL get_stream_input_get_stream_info_response() = 0;

        /**
         * \return the stream_input get_rx_state response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_input_get_rx_state_response * STDCALL get_stream_input_get_rx_state_response() = 0;

        /**
         * Send a SET_STREAM_FORMAT command with a notification id to change the format of a stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_stream_format The stream format field is set to the new stream format.
         *
         * The new stream format can be retrieved by calling the set_stream_format_stream_format function after successfully
         * receiving a response back for the SET_STREAM_FORMAT command sent.
         *
         * \see set_stream_format_stream_format()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_stream_format_cmd(void *notification_id, uint64_t new_stream_format) = 0;

        /**
         * Send a GET_STREAM_FORMAT command with a notification id to fetch the current format of a stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * The stream format can be retrieved by calling the get_stream_format_stream_format function after successfully
         * receiving a response back for the GET_STREAM_FORMAT command sent.
         *
         * \see get_stream_format_stream_format()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_stream_format_cmd(void *notification_id) = 0;

        /**
         * Send a SET_STREAM_INFO command with a notification id to change the current values of the dynamic information of
         * the stream, such as the msrp_accumulated_latency, stream ID, and destination MAC.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_stream_info_field The new field information to be set to for a stream.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_stream_info_cmd(void *notification_id, void *new_stream_info_field) = 0;

        /**
         * Send a GET_STREAM_INFO command with a notification id to fetch the current information for a stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * The stream information can be retrieved by calling the following functions after successfully
         * receiving a response back for the GET_STREAM_INFO command sent.
         *
         * \see get_stream_info_flags(), get_stream_info_stream_format(), get_stream_info_stream_id(),
         *      get_stream_info_msrp_accumulated_latency(), get_stream_info_stream_dest_mac(),
         *      get_stream_info_msrp_failure_code(), get_stream_info_msrp_failure_bridge_id()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_stream_info_cmd(void *notification_id) = 0;

        /**
         * Send a START_STREAMING command with a notification id to start streaming on a previously connected stream that was connected
         * via ACMP or has previously been stopped with the STOP_STREAMING command.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_start_streaming_cmd(void *notification_id) = 0;

        /**
         * Send a STOP_STREAMING command with a notification id to stop a connected stream for streaming media.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_stop_streaming_cmd(void *notification_id) = 0;

        /**
         * Send a CONNECT_RX command with a notification id to connect Listener sink stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param talker_entity_id The Talker Entity ID used to identify the AVDECC Talker being targed by
         *                    the command. In the case of Talker commands, this is the AVDECC Entity
         *                    receiving the command. In the case of Listener commands, this is the
         *                    AVDECC Entity that any Talker command is to be sent to. This field is
         *                    either the Entity ID of the AVDECC Entity being targets to or 0.
         * \param talker_unique_id The Talker Unique ID is used to uniquely identify the stream source
         *                         of the AVDECC Talker. For entities using the AVDECC Entity Mondel,
         *                         this corresponds to the id of the STREAM OUTPUT descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_connect_rx_cmd(void *notification_id, uint64_t talker_entity_id, uint16_t talker_unique_id, uint16_t flags) = 0;

        /**
         * Send a DISCONNECT_RX command with a notification id to disconnect Listener sink stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param talker_entity_id The Talker Entity ID used to identify the AVDECC Talker being targed by
         *                    the command. In the case of Talker commands, this is the AVDECC Entity
         *                    receiving the command. In the case of Listener commands, this is the
         *                    AVDECC Entity that any Talker command is to be sent to. This field is
         *                    either the Entity ID of the AVDECC Entity being targets to or 0.
         * \param talker_unique_id The Talker Unique ID is used to uniquely identify the stream source
         *                         of the AVDECC Talker. For entities using the AVDECC Entity Mondel,
         *                         this corresponds to the id of the STREAM OUTPUT descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_disconnect_rx_cmd(void *notification_id, uint64_t talker_entity_id, uint16_t talker_unique_id) = 0;

        /**
         * Send a GET_RX_STATE command with a notification id to get Listener sink stream connection state.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param talker_entity_id The Talker Entity ID used to identify the AVDECC Talker being targed by
         *                    the command. In the case of Talker commands, this is the AVDECC Entity
         *                    receiving the command. In the case of Listener commands, this is the
         *                    AVDECC Entity that any Talker command is to be sent to. This field is
         *                    either the Entity ID of the AVDECC Entity being targets to or 0.
         * \param talker_unique_id The Talker Unique ID is used to uniquely identify the stream source
         *                         of the AVDECC Talker. For entities using the AVDECC Entity Mondel,
         *                         this corresponds to the id of the STREAM OUTPUT descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_rx_state_cmd(void *notification_id) = 0;

        /**
         * Send a GET_COUNTERS command to get the stream_input counters of the AVDECC Entity.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_counters_cmd(void *notification_id) = 0;
    };
}

