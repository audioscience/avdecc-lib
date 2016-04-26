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
 * memory_object_descriptor_response_imp.cpp
 *
 * MEMORY OBJECT descriptor response implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "acmp_controller_state_machine.h"
#include "aecp_controller_state_machine.h"
#include "memory_object_descriptor_response_imp.h"

namespace avdecc_lib
{
#define MEMORY_OBJECT_NUM_STRINGS 6
const char * memory_object_type_str[] =
    {
        "FIRMWARE_IMAGE",
        "VENDOR_SPECIFIC",
        "CRASH_DUMP",
        "LOG_OBJECT",
        "AUTOSTART_SETTINGS",
        "SNAPSHOT_SETTINGS"};

memory_object_descriptor_response_imp::memory_object_descriptor_response_imp(const uint8_t * frame, size_t frame_len, ssize_t pos) : descriptor_response_base_imp(frame, frame_len, pos) {}

memory_object_descriptor_response_imp::~memory_object_descriptor_response_imp() {}

uint8_t * STDCALL memory_object_descriptor_response_imp::object_name()
{
    return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT_OFFSET_OBJECT_NAME];
}

uint16_t STDCALL memory_object_descriptor_response_imp::localized_description()
{
    return jdksavdecc_descriptor_memory_object_get_localized_description(buffer, position);
}

uint16_t STDCALL memory_object_descriptor_response_imp::memory_object_type()
{
    return jdksavdecc_descriptor_memory_object_get_memory_object_type(buffer, position);
}

uint16_t STDCALL memory_object_descriptor_response_imp::target_descriptor_type()
{
    return jdksavdecc_descriptor_memory_object_get_target_descriptor_type(buffer, position);
}

uint16_t STDCALL memory_object_descriptor_response_imp::target_descriptor_index()
{
    return jdksavdecc_descriptor_memory_object_get_target_descriptor_index(buffer, position);
}

uint64_t STDCALL memory_object_descriptor_response_imp::start_address()
{
    return jdksavdecc_descriptor_memory_object_get_start_address(buffer, position);
}

uint64_t STDCALL memory_object_descriptor_response_imp::maximum_length()
{
    return jdksavdecc_descriptor_memory_object_get_maximum_length(buffer, position);
}

uint64_t STDCALL memory_object_descriptor_response_imp::length()
{
    return jdksavdecc_descriptor_memory_object_get_length(buffer, position);
}

const char * STDCALL memory_object_descriptor_response_imp::memory_object_type_to_str()
{
    if (memory_object_type() < MEMORY_OBJECT_NUM_STRINGS)
    {
        return memory_object_type_str[memory_object_type()];
    }

    return "UNKNOWN";
}
}
