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
 * stream_output_descriptor_response_imp.cpp
 *
 * STREAM OUTPUT descriptor response implementation
 */

#include <vector>
#include "util.h"
#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "acmp_controller_state_machine.h"
#include "aecp_controller_state_machine.h"
#include "stream_output_descriptor_response_imp.h"

namespace avdecc_lib
{
    stream_output_descriptor_response_imp::stream_output_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
        descriptor_response_base_imp(frame, frame_len, pos)
    {
        memset(&stream_output_flags, 0, sizeof(struct stream_output_desc_stream_flags));
        stream_flags_init();
        store_supported_stream_fmts();

        stream_info_flags["CLASS_B"]                  = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_CLASS_B;
        stream_info_flags["FAST_CONNECT"]             = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_FAST_CONNECT;
        stream_info_flags["SAVED_STATE"]              = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_SAVED_STATE;
        stream_info_flags["STREAMING_WAIT"]           = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_STREAMING_WAIT;
        stream_info_flags["ENCRYPTED_PDU"]            = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_ENCRYPTED_PDU;
        stream_info_flags["STREAM_VLAN_ID_VALID"]     = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_STREAM_VLAN_ID_VALID;
        stream_info_flags["CONNECTED"]                = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_CONNECTED;
        stream_info_flags["MSRP_FAILURE_VALID"]       = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_MSRP_FAILURE_VALID;
        stream_info_flags["STREAM_DEST_MAC_VALID"]    = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_STREAM_DEST_MAC_VALID;
        stream_info_flags["MSRP_ACC_LAT_VALID"]       = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_MSRP_ACC_LAT_VALID;
        stream_info_flags["STREAM_ID_VALID"]          = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_STREAM_ID_VALID;
        stream_info_flags["STREAM_FORMAT_VALID"]      = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_STREAM_FORMAT_VALID;
    }
    
    stream_output_descriptor_response_imp::~stream_output_descriptor_response_imp() {}
    
    void stream_output_descriptor_response_imp::stream_flags_init()
    {
        stream_output_flags.clock_sync_source = stream_flags() >> 0 & 0x01;;
        stream_output_flags.class_a = stream_flags() >> 1 & 0x01;
        stream_output_flags.class_b = stream_flags() >> 2 & 0x01;
        stream_output_flags.supports_encrypted = stream_flags() >> 3 & 0x01;
        stream_output_flags.primary_backup_supported = stream_flags() >> 4 & 0x01;
        stream_output_flags.primary_backup_valid = stream_flags() >> 5 & 0x01;
        stream_output_flags.secondary_backup_supported = stream_flags() >> 6 & 0x01;
        stream_output_flags.secondary_backup_valid = stream_flags() >> 7 & 0x01;
        stream_output_flags.tertiary_backup_supported = stream_flags() >> 8 & 0x01;
        stream_output_flags.tertiary_backup_valid = stream_flags() >> 9 & 0x01;
    }

    uint8_t * STDCALL stream_output_descriptor_response_imp::object_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_STREAM_OFFSET_OBJECT_NAME];
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::localized_description()
    {
        return jdksavdecc_descriptor_stream_get_localized_description(buffer, position);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::clock_domain_index()
    {
        return jdksavdecc_descriptor_stream_get_clock_domain_index(buffer, position);
    }
    
    uint16_t stream_output_descriptor_response_imp::stream_flags()
    {
        return jdksavdecc_descriptor_stream_get_stream_flags(buffer, position);
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_clock_sync_source()
    {
        return stream_output_flags.clock_sync_source;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_class_a()
    {
        return stream_output_flags.class_a;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_class_b()
    {
        return stream_output_flags.class_b;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_supports_encrypted()
    {
        return stream_output_flags.supports_encrypted;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_primary_backup_supported()
    {
        return stream_output_flags.primary_backup_supported;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_primary_backup_valid()
    {
        return stream_output_flags.primary_backup_valid;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_secondary_backup_supported()
    {
        return stream_output_flags.secondary_backup_supported;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_secondary_backup_valid()
    {
        return stream_output_flags.secondary_backup_valid;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_tertiary_backup_supported()
    {
        return stream_output_flags.tertiary_backup_supported;
    }
    
    bool STDCALL stream_output_descriptor_response_imp::stream_flags_tertiary_backup_valid()
    {
        return stream_output_flags.tertiary_backup_valid;
    }
    
    const char * STDCALL stream_output_descriptor_response_imp::current_format()
    {
        uint64_t current_format;
        current_format = jdksavdecc_uint64_get(&buffer[position +
                                                       JDKSAVDECC_DESCRIPTOR_STREAM_OFFSET_CURRENT_FORMAT], 0);
        
        return utility::ieee1722_format_value_to_name(current_format);
    }
    
    uint16_t stream_output_descriptor_response_imp::formats_offset()
    {
        assert(jdksavdecc_descriptor_stream_get_formats_offset(buffer, position) == 132);
        return jdksavdecc_descriptor_stream_get_formats_offset(buffer, position);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::number_of_formats()
    {
        assert(jdksavdecc_descriptor_stream_get_number_of_formats(buffer, position));
        return jdksavdecc_descriptor_stream_get_number_of_formats(buffer, position);
    }
    
    uint64_t STDCALL stream_output_descriptor_response_imp::backup_talker_entity_id_0()
    {
        uint64_t backup_talker_entity_id_0;
        return backup_talker_entity_id_0 = jdksavdecc_uint64_get(&buffer[position +
                                                            JDKSAVDECC_DESCRIPTOR_STREAM_OFFSET_BACKUP_TALKER_ENTITY_ID_0], 0);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::backup_talker_unique_0()
    {
        return jdksavdecc_descriptor_stream_get_backup_talker_unique_id_0(buffer, position);
    }
    
    uint64_t STDCALL stream_output_descriptor_response_imp::backup_talker_entity_id_1()
    {
        uint64_t backup_talker_entity_id_1;
        return backup_talker_entity_id_1 = jdksavdecc_uint64_get(&buffer[position +
                                                                         JDKSAVDECC_DESCRIPTOR_STREAM_OFFSET_BACKUP_TALKER_ENTITY_ID_1], 0);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::backup_talker_unique_1()
    {
        return jdksavdecc_descriptor_stream_get_backup_talker_unique_id_1(buffer, position);
    }
    
    uint64_t STDCALL stream_output_descriptor_response_imp::backup_talker_entity_id_2()
    {
        uint64_t backup_talker_entity_id_2;
        return backup_talker_entity_id_2 = jdksavdecc_uint64_get(&buffer[position +
                                                                         JDKSAVDECC_DESCRIPTOR_STREAM_OFFSET_BACKUP_TALKER_ENTITY_ID_2], 0);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::backup_talker_unique_2()
    {
        return jdksavdecc_descriptor_stream_get_backup_talker_unique_id_2(buffer, position);
    }
    
    uint64_t STDCALL stream_output_descriptor_response_imp::backedup_talker_entity_id()
    {
        uint64_t backedup_talker_entity_id;
        return backedup_talker_entity_id = jdksavdecc_uint64_get(&buffer[position +
                                                                         JDKSAVDECC_DESCRIPTOR_STREAM_OFFSET_BACKEDUP_TALKER_ENTITY_ID], 0);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::backedup_talker_unique()
    {
        return jdksavdecc_descriptor_stream_get_backedup_talker_unique(buffer, position);
    }
    
    uint16_t STDCALL stream_output_descriptor_response_imp::avb_interface_index()
    {
        return jdksavdecc_descriptor_stream_get_avb_interface_index(buffer, position);
    }
    
    uint32_t STDCALL stream_output_descriptor_response_imp::buffer_length()
    {
        return jdksavdecc_descriptor_stream_get_buffer_length(buffer, position);
    }
    
    bool stream_output_descriptor_response_imp::get_stream_info_flag(const char *flag)
    {
        std::map<string, int>::iterator it;
        it = stream_info_flags.find(flag);
        assert(it != stream_info_flags.end());
        return it->second;
    }

    void stream_output_descriptor_response_imp::store_supported_stream_fmts()
    {
        uint64_t offset = 0;
        for(int i = 0; i < number_of_formats(); i++)
        {
            stream_fmts_vec.push_back(jdksavdecc_uint64_get(&buffer[position + formats_offset() + offset], 0));
            offset += 0x8;
        }
    }

    uint64_t STDCALL stream_output_descriptor_response_imp::get_supported_stream_fmt_by_index(size_t supported_stream_fmt_index)
    {
        return stream_fmts_vec.at(supported_stream_fmt_index);
    }
}
