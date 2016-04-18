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
 * entity_descriptor_response_imp.cpp
 *
 * ENTITY descriptor response implementation
 */

#include <vector>
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "entity_descriptor_response_imp.h"
#include "aecp_controller_state_machine.h"
#include "adp.h"
#include "system_tx_queue.h"
#include "util.h"

namespace avdecc_lib
{
    entity_descriptor_response_imp::entity_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
        descriptor_response_base_imp(frame, frame_len, pos) {}
    
    entity_descriptor_response_imp::~entity_descriptor_response_imp() {}

    uint64_t STDCALL entity_descriptor_response_imp::entity_id()
    {
        uint64_t entity_id;
        return entity_id = jdksavdecc_uint64_get(&buffer[position +
                                                JDKSAVDECC_DESCRIPTOR_ENTITY_OFFSET_ENTITY_ID], 0);
    }
    
    uint64_t STDCALL entity_descriptor_response_imp::entity_model_id()
    {
		jdksavdecc_eui64 eui = jdksavdecc_descriptor_entity_get_entity_model_id(buffer, position);
		return jdksavdecc_eui64_convert_to_uint64(&eui);
    }
    
    uint32_t STDCALL entity_descriptor_response_imp::entity_capabilities()
    {
        return jdksavdecc_descriptor_entity_get_entity_capabilities(buffer, position);
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::talker_stream_sources()
    {
        return jdksavdecc_descriptor_entity_get_talker_stream_sources(buffer, position);
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::talker_capabilities()
    {
        return jdksavdecc_descriptor_entity_get_talker_capabilities(buffer, position);
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::listener_stream_sinks()
    {
        return jdksavdecc_descriptor_entity_get_listener_stream_sinks(buffer, position);
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::listener_capabilities()
    {
        return jdksavdecc_descriptor_entity_get_listener_capabilities(buffer, position);
    }
    
    uint32_t STDCALL entity_descriptor_response_imp::controller_capabilities()
    {
        return jdksavdecc_descriptor_entity_get_controller_capabilities(buffer, position);
    }
    
    uint32_t STDCALL entity_descriptor_response_imp::available_index()
    {
        return jdksavdecc_descriptor_entity_get_available_index(buffer, position);
    }
    
    uint64_t STDCALL entity_descriptor_response_imp::association_id()
    {
        uint64_t association_id;
        
        utility::convert_eui48_to_uint64(&buffer[position +
                                                 JDKSAVDECC_DESCRIPTOR_ENTITY_OFFSET_ASSOCIATION_ID], association_id);
        return association_id;
    }
    
    uint8_t * STDCALL entity_descriptor_response_imp::entity_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_ENTITY_OFFSET_ENTITY_NAME];
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::vendor_name_string()
    {
        return jdksavdecc_descriptor_entity_get_vendor_name_string(buffer, position);
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::model_name_string()
    {
        return jdksavdecc_descriptor_entity_get_model_name_string(buffer, position);
    }
    
    uint8_t * STDCALL entity_descriptor_response_imp::firmware_version()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_ENTITY_OFFSET_FIRMWARE_VERSION];
    }
    
    uint8_t * STDCALL entity_descriptor_response_imp::group_name()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_ENTITY_OFFSET_GROUP_NAME];
    }
    
    uint8_t * STDCALL entity_descriptor_response_imp::serial_number()
    {
        return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_ENTITY_OFFSET_SERIAL_NUMBER];
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::configurations_count()
    {
        uint16_t configurations_count;
        
        configurations_count = jdksavdecc_descriptor_entity_get_configurations_count(buffer, position);
        assert(configurations_count >= 1);
        return configurations_count;
    }
    
    uint16_t STDCALL entity_descriptor_response_imp::current_configuration()
    {
        return jdksavdecc_descriptor_entity_get_current_configuration(buffer, position);
    }
}
