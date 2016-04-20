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
 * entity_descriptor_response_imp.h
 *
 * ENTITY descriptor repsonse implementation class
 */

#pragma once

#include "configuration_descriptor_imp.h"
#include "entity_descriptor_response.h"
#include "jdksavdecc_aem_descriptor.h"
#include "descriptor_response_base_imp.h"

namespace avdecc_lib
{
class entity_descriptor_response_imp : public entity_descriptor_response, public virtual
    descriptor_response_base_imp
{
public:
    entity_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos);
    virtual ~entity_descriptor_response_imp();

    uint64_t STDCALL entity_id();
    uint64_t STDCALL entity_model_id();
    uint32_t STDCALL entity_capabilities();
    uint16_t STDCALL talker_stream_sources();
    uint16_t STDCALL talker_capabilities();
    uint16_t STDCALL listener_stream_sinks();
    uint16_t STDCALL listener_capabilities();
    uint32_t STDCALL controller_capabilities();
    uint32_t STDCALL available_index();
    uint64_t STDCALL association_id();
    uint8_t * STDCALL entity_name();
    uint16_t STDCALL vendor_name_string();
    uint16_t STDCALL model_name_string();
    uint8_t * STDCALL firmware_version();
    uint8_t * STDCALL group_name();
    uint8_t * STDCALL serial_number();
    uint16_t STDCALL configurations_count();
    uint16_t STDCALL current_configuration();
};
}

