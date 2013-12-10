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
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class stream_input_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The descriptor index of the CLOCK DOMAIN descriptor providing the media clock for the stream.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL clock_domain_index() = 0;

        /**
         * \return The flags describing the capabilities or features of the stream.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL stream_flags() = 0;

        /**
         * \return True if the stream can be used as a clock synchronization source.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_clock_sync_source() = 0;

        /**
         * \return True if the stream supports streaming at Class A.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_class_a() = 0;

        /**
         * \return True if the stream supports streaming at Class B.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_class_b() = 0;

        /**
         * \return True if the stream supports streaming with encrypted PDUs.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_supports_encrypted() = 0;

        /**
         * \return True if the primary backup AVDECC Talker's Entity ID and primary backup AVDECC Talker's Unique ID are supported.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_primary_backup_supported() = 0;

        /**
         * \return True if the primary backup AVDECC Talker's Entity ID and primary backup AVDECC Talker's Unique ID are valid.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_primary_backup_valid() = 0;

        /**
         * \return True if the secondary backup AVDECC Talker's Entity ID and secondary backup AVDECC Talker's Unique ID are supported.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_secondary_backup_supported() = 0;

        /**
         * \return True if the secondary backup AVDECC Talker's Entity ID and secondary backup AVDECC Talker's Unique ID are valid.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_secondary_backup_valid() = 0;

        /**
         * \return True if the tertiary backup AVDECC Talker's Entity ID and tertiary backup AVDECC Talker's Unique ID are supported.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_tertiary_backup_supported() = 0;

        /**
         * \return True if the tertiary backup AVDECC Talker's Entity ID and tertiary backup AVDECC Talker's Unique ID are valid.
         */
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_tertiary_back_up_valid() = 0;

        /**
         * \return The current format of the stream.
         */
        AVDECC_CONTROLLER_LIB32_API virtual const char * STDCALL current_format() = 0;

        /**
         * \return The number of formats supported by this audio stream. The maximum value
         *	       for this field is 47 for this version of AEM.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_formats() = 0;

        /**
         * \return The primary backup AVDECC Talker's Entity ID.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL backup_talker_entity_id_0() = 0;

        /**
         * \return The primary backup AVDECC Talker's Unique ID.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL backup_talker_unique_0() = 0;

        /**
         * \return The secondary backup AVDECC Talker's Entity ID.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL backup_talker_entity_id_1() = 0;

        /**
         * \return The secondary backup AVDECC Talker's Unique ID.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL backup_talker_unique_1() = 0;

        /**
         * \return The tertiary backup AVDECC Talker's Entity ID.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL backup_talker_entity_id_2() = 0;

        /**
         * \return The tertiary backup AVDECC Talker's Unique ID.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL backup_talker_unique_2() = 0;

        /**
         * \return The Entity ID of the AVDECC Talker that this stream is backing up.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL backedup_talker_entity_id() = 0;

        /**
         * \return The Unique ID of the AVDECC Talker that this stream is backing up.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL backedup_talker_unique() = 0;

        /**
         * \return The descriptor index of the AVB INTERFACE descriptor from which this stream
         *	       is sourced or to which it is sinked.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL avb_interface_index() = 0;

        /**
         * \return The length in nanoseconds of the MAC's ingress buffer size.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL buffer_length() = 0;

        /**
         * \return The stream format of a stream after sending a SET_STREAM_FORMAT command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL set_stream_format_stream_format() = 0;

        /**
         * \return The stream format of a stream after sending a GET_STREAM_FORMAT command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_format_stream_format() = 0;

        /**
         * \return The stream info flags of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_flags() = 0;

        /**
         * \return The stream info stream format of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_format() = 0;

        /**
         * \return The stream info stream id of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_id() = 0;

        /**
         * \return The stream info MSRP accumulated latency of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_stream_info_msrp_accumulated_latency() = 0;

        /**
         * \return The stream info stream destination MAC of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_stream_dest_mac() = 0;

        /**
         * \return The stream info MSRP failure code of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL get_stream_info_msrp_failure_code() = 0;

        /**
         * \return The stream info MSRP failure bridge id of a stream after sending a GET_STREAM_info command and
         *	       receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_stream_info_msrp_failure_bridge_id() = 0;

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
         * \param talker_guid The Talker Entity ID used to identify the AVDECC Talker being targed by
         *                    the command. In the case of Talker commands, this is the AVDECC Entity
         *                    receiving the command. In the case of Listener commands, this is the
         *                    AVDECC Entity that any Talker command is to be sent to. This field is
         *                    either the Entity ID of the AVDECC Entity being targets to or 0.
         * \param talker_unique_id The Talker Unique ID is used to uniquely identify the stream source
         *                         of the AVDECC Talker. For entities using the AVDECC Entity Mondel,
         *                         this corresponds to the id of the STREAM OUTPUT descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_connect_rx_cmd(void *notification_id, uint64_t talker_guid, uint16_t talker_unique_id, uint16_t flags) = 0;

        /**
         * Send a DISCONNECT_RX command with a notification id to disconnect Listener sink stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param talker_guid The Talker Entity ID used to identify the AVDECC Talker being targed by
         *                    the command. In the case of Talker commands, this is the AVDECC Entity
         *                    receiving the command. In the case of Listener commands, this is the
         *                    AVDECC Entity that any Talker command is to be sent to. This field is
         *                    either the Entity ID of the AVDECC Entity being targets to or 0.
         * \param talker_unique_id The Talker Unique ID is used to uniquely identify the stream source
         *                         of the AVDECC Talker. For entities using the AVDECC Entity Mondel,
         *                         this corresponds to the id of the STREAM OUTPUT descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_disconnect_rx_cmd(void *notification_id, uint64_t talker_guid, uint16_t talker_unique_id) = 0;

        /**
         * Send a GET_RX_STATE command with a notification id to get Listener sink stream connection state.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param talker_guid The Talker Entity ID used to identify the AVDECC Talker being targed by
         *                    the command. In the case of Talker commands, this is the AVDECC Entity
         *                    receiving the command. In the case of Listener commands, this is the
         *                    AVDECC Entity that any Talker command is to be sent to. This field is
         *                    either the Entity ID of the AVDECC Entity being targets to or 0.
         * \param talker_unique_id The Talker Unique ID is used to uniquely identify the stream source
         *                         of the AVDECC Talker. For entities using the AVDECC Entity Mondel,
         *                         this corresponds to the id of the STREAM OUTPUT descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_rx_state_cmd(void *notification_id) = 0;
    };
}

