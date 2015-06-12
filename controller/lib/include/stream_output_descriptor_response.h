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
 * stream_output_descriptor_response.h
 *
 * Public STREAM OUTPUT descriptor response interface class
 * The STREAM OUTPUT descriptor describes a sourced or sinked stream.
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"
#include "descriptor_response_base.h"

namespace avdecc_lib
{
    class stream_output_descriptor_response : public virtual descriptor_response_base
    {
    public:
        virtual ~stream_output_descriptor_response(){};
        
        /**
         * \return The localized string reference pointing to the localized descriptor name.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL localized_description() = 0;

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
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL stream_flags_tertiary_backup_valid() = 0;
        
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
         * Tests state of a flag returned by last GET_STREAM_INFO command.
         *
         * \param flag The flag to check. Valid values are:
         * CLASS_B, FAST_CONNECT, SAVED_STATE, STREAMING_WAIT, ENCRYPTED_PDU, STREAM_VLAN_ID_VALID
         * CONNECTED, MSRP_FAILURE_VALID, STREAM_DEST_MAC_VALID, MSRP_ACC_LAT_VALID, STREAM_ID_VALID,
         * STREAM_FORMAT_VALID.
         *
         * \see get_stream_info_flags(), get_stream_info_stream_format(), get_stream_info_stream_id(),
         *      get_stream_info_msrp_accumulated_latency(), get_stream_info_stream_dest_mac(),
         *      get_stream_info_msrp_failure_code(), get_stream_info_msrp_failure_bridge_id()
         */
        
        AVDECC_CONTROLLER_LIB32_API virtual bool STDCALL get_stream_info_flag(const char *flag) = 0;
    };
}

