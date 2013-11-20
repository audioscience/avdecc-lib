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
 * stream_output_descriptor_imp.h
 *
 * Stream Output descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_STREAM_OUTPUT_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_STREAM_OUTPUT_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "stream_output_descriptor.h"

namespace avdecc_lib
{
    class stream_output_descriptor_imp : public stream_output_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_stream stream_output_desc; // Structure containing the stream_output_desc fields
        int stream_output_desc_read_returned; // Status of extracting Stream Output descriptor information from a network buffer

        struct stream_output_desc_stream_flags
        {
            bool clock_sync_source;
            bool class_a;
            bool class_b;
            bool supports_encrypted;
            bool primary_backup_supported;
            bool primary_backup_valid;
            bool secondary_backup_supported;
            bool secondary_backup_valid;
            bool tertiary_backup_supported;
            bool tertiary_back_up_valid;
        };

        struct stream_output_desc_stream_flags stream_flags;
        struct jdksavdecc_aem_command_set_stream_format_response aem_cmd_set_stream_format_resp; // Store the response received after sending a SET_STREAM_FORMAT command.
        struct jdksavdecc_aem_command_get_stream_format_response aem_cmd_get_stream_format_resp; // Store the response received after sending a GET_STREAM_FORMAT command.
        struct jdksavdecc_aem_command_set_stream_info_response aem_cmd_set_stream_info_resp; // Store the response received after sending a SET_STREAM_INFO command.
        struct jdksavdecc_aem_command_get_stream_info_response aem_cmd_get_stream_info_resp; // Store the response received after sending a GET_STREAM_INFO command.

    public:
        /**
         * Constructor for Stream Output descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        stream_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

        virtual ~stream_output_descriptor_imp();

    private:
        /**
         * Store the stream flags components of the Stream Output descriptor object in a vector.
         */
        void stream_flags_init();

    public:
        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL get_descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL get_descriptor_index() const;

        /**
         * Get the name of the Stream Output. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL get_object_name();

        /**
         * Get the localized string reference pointing to the localized Stream Output name.
         */
        uint16_t STDCALL get_localized_description();

        /**
         * Get the descriptor index of the Clock Domain descriptor providing the media clock for the stream.
         */
        uint16_t STDCALL get_clock_domain_index();

        /**
         * Get the flags describing the capabilities or features of the stream.
         */
        uint16_t STDCALL get_stream_flags();

        /**
         * Check if the stream can be used as a clock synchronization source.
         */
        bool STDCALL get_stream_flags_clock_sync_source();

        /**
         * Check if the stream supports streaming at Class A.
         */
        bool STDCALL get_stream_flags_class_a();

        /**
         * Check if the stream supports streaming at Class B.
         */
        bool STDCALL get_stream_flags_class_b();

        /**
         * Check if the stream supports streaming with encrypted PDUs.
         */
        bool STDCALL get_stream_flags_supports_encrypted();

        /**
         * Check if the primary backup AVDECC Talker's Entity ID and primary backup AVDECC Talker's Unique ID are supported.
         */
        bool STDCALL get_stream_flags_primary_backup_supported();

        /**
         * Check if the primary backup AVDECC Talker's Entity ID and primary backup AVDECC Talker's Unique ID are valid.
         */
        bool STDCALL get_stream_flags_primary_backup_valid();

        /**
         * Check if the secondary backup AVDECC Talker's Entity ID and secondary backup AVDECC Talker's Unique ID are supported.
         */
        bool STDCALL get_stream_flags_secondary_backup_supported();

        /**
         * Check if the secondary backup AVDECC Talker's Entity ID and secondary backup AVDECC Talker's Unique ID are valid.
         */
        bool STDCALL get_stream_flags_secondary_backup_valid();

        /**
         * Check if the tertiary backup AVDECC Talker's Entity ID and tertiary backup AVDECC Talker's Unique ID are supported.
         */
        bool STDCALL get_stream_flags_tertiary_backup_supported();

        /**
         * Check if the tertiary backup AVDECC Talker's Entity ID and tertiary backup AVDECC Talker's Unique ID are valid.
         */
        bool STDCALL get_stream_flags_tertiary_back_up_valid();

        /**
         * Get the current format of the stream.
         */
        const char * STDCALL get_current_format();

        /**
         * Get the offset from the start of the descriptor for the first octet of the formats.
         * This field is 132 for this version of AEM.
         */
        uint16_t STDCALL get_formats_offset();

        /**
         * Get the number of formats supported by this audio stream. The maximum value
         * for this field is 47 for this version of AEM.
         */
        uint16_t STDCALL get_number_of_formats();

        /**
         * Get the primary backup AVDECC Talker's Entity ID.
         */
        uint64_t STDCALL get_backup_talker_entity_id_0();

        /**
         * Get the primary backup AVDECC Talker's Unique ID.
         */
        uint16_t STDCALL get_backup_talker_unique_0();

        /**
         * Get the secondary backup AVDECC Talker's Entity ID.
         */
        uint64_t STDCALL get_backup_talker_entity_id_1();

        /**
         * Get the secondary backup AVDECC Talker's Unique ID.
         */
        uint16_t STDCALL get_backup_talker_unique_1();

        /**
         * Get the tertiary backup AVDECC Talker's Entity ID.
         */
        uint64_t STDCALL get_backup_talker_entity_id_2();

        /**
         * Get the tertiary backup AVDECC Talker's Unique ID.
         */
        uint16_t STDCALL get_backup_talker_unique_2();

        /**
         * Get the Entity ID of the AVDECC Talker that this stream is backing up.
         */
        uint64_t STDCALL get_backedup_talker_entity_id();

        /**
         * Get the Unique ID of the AVDECC Talker that this stream is backing up.
         */
        uint16_t STDCALL get_backedup_talker_unique();

        /**
         * Get the descriptor index of the AVB Interface descriptor from which this stream
         * is sourced or to which it is sinked.
         */
        uint16_t STDCALL get_avb_interface_index();

        /**
         * Get the length in nanoseconds of the MAC's egress buffer size.
         */
        uint32_t STDCALL get_buffer_length();

        /**
         * Get the stream format of a stream after sending a SET_STREAM_FORMAT command and
         * receiving a response back for the command.
         */
        uint64_t STDCALL set_stream_format_stream_format();

        /**
         * Get the stream format of a stream after sending a GET_STREAM_FORMAT command and
         * receiving a response back for the command.
         */
        uint64_t STDCALL get_stream_format_stream_format();

        /**
         * Get the stream info flags of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint32_t STDCALL get_stream_info_flags();

        /**
         * Get the stream info stream format of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint64_t STDCALL get_stream_info_stream_format();

        /**
         * Get the stream info stream id of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint64_t STDCALL get_stream_info_stream_id();

        /**
         * Get the stream info MSRP accumulated latency of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint32_t STDCALL get_stream_info_msrp_accumulated_latency();

        /**
         * Get the stream dest mac of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint8_t * STDCALL get_stream_info_stream_dest_mac();

        /**
         * Get the stream info MSRP failure code of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint8_t STDCALL get_stream_info_msrp_failure_code();

        /**
         * Get the stream info MSRP failure bridge id of a stream after sending a GET_STREAM_info command and
         * receiving a response back for the command.
         */
        uint64_t STDCALL get_stream_info_msrp_failure_bridge_id();

        /**
         * Check if the Clock Sync Source flag is set.
         */
        bool is_clock_sync_source_set();

        /**
         * Send a SET_STREAM_FORMAT command with a notification id to change the format of a stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_stream_format The stream format field is set to the new stream format.
         */
        int STDCALL send_set_stream_format_cmd(void *notification_id, uint64_t new_stream_format);

        /**
         * Process a SET_STREAM_FORMAT response for the SET_STREAM_FORMAT command. The reponse always contains
         * the current value, that is it contains the new stream format if the command succeeds or the old stream
         * format if it fails.
         */
        int proc_set_stream_format_resp(void *notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Send a GET_STREAM_FORMAT command with a notification id to fetch the current format of a stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        int STDCALL send_get_stream_format_cmd(void *notification_id);

        /**
         * Process a GET_STREAM_FORMAT response for the GET_STREAM_FORMAT command. The stream format field is set to
         * the current stream format, which is equivalent to the current format field in the addressed STREAM_OUTPUT
         * descriptor object.
         */
        int proc_get_stream_format_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Send a SET_STREAM_INFO command with a notification id to change the current values of the dynamic information of
         * the stream, such as the msrp_accumulated_latency, stream ID, and destination MAC.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_stream_info_field The new field information to be set to for a stream.
         */
        int STDCALL send_set_stream_info_cmd(void *notification_id, void *new_stream_info_field);

        /**
         * Process a SET_STREAM_INFO response for the SET_STREAM_INFO command.
         */
        int proc_set_stream_info_resp(void *notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Send a GET_STREAM_INFO command with a notification id to fetch the current information for a stream.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        int STDCALL send_get_stream_info_cmd(void *notification_id);

        /**
         * Process a GET_STREAM_INFO response for the GET_STREAM_INFO command.
         */
        int proc_get_stream_info_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Send a START_STREAMING command with a notification id to start streaming on a previously connected stream that was connected
         * via ACMP or has previously been stopped with the STOP_STREAMING command.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        int STDCALL send_start_streaming_cmd(void *notification_id);

        /**
         * Process a START_STREAMING response for the START_STREAMING command.
         */
        int proc_start_streaming_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

        /**
         * Send a STOP_STREAMING command with a notification id to stop a connected stream for streaming media.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        int STDCALL send_stop_streaming_cmd(void *notification_id);

        /**
         * Process a START_STREAMING response for the START_STREAMING command.
         */
        int proc_stop_streaming_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);
    };
}

#endif
