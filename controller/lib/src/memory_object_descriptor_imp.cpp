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
 * memory_object_descriptor_imp.cpp
 *
 * MEMORY OBJECT descriptor implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "memory_object_descriptor_imp.h"

namespace avdecc_lib
{
    #define MEMORY_OBJECT_NUM_STRINGS 6
    const char *memory_object_type_str[] =
    {
        "FIRMWARE_IMAGE",
        "VENDOR_SPECIFIC",
        "CRASH_DUMP",
        "LOG_OBJECT",
        "AUTOSTART_SETTINGS",
        "SNAPSHOT_SETTINGS"
    };

    memory_object_descriptor_imp::memory_object_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        desc_memory_object_read_returned = jdksavdecc_descriptor_memory_object_read(&memory_object_desc, frame, pos, frame_len);

        if(desc_memory_object_read_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, memory_object_desc_read error", end_station_obj->guid());
            assert(desc_memory_object_read_returned >= 0);
        }
    }

    memory_object_descriptor_imp::~memory_object_descriptor_imp() {}

    uint16_t STDCALL memory_object_descriptor_imp::descriptor_type() const
    {
        assert(memory_object_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT);
        return memory_object_desc.descriptor_type;
    }

    uint16_t STDCALL memory_object_descriptor_imp::descriptor_index() const
    {
        return memory_object_desc.descriptor_index;
    }

    uint8_t * STDCALL memory_object_descriptor_imp::object_name()
    {
        return memory_object_desc.object_name.value;
    }

    uint16_t STDCALL memory_object_descriptor_imp::localized_description()
    {
        return memory_object_desc.localized_description;
    }

    uint16_t STDCALL memory_object_descriptor_imp::memory_object_type()
    {
        return memory_object_desc.memory_object_type;
    }

    uint16_t STDCALL memory_object_descriptor_imp::target_descriptor_type()
    {
        return memory_object_desc.target_descriptor_type;
    }

    uint16_t STDCALL memory_object_descriptor_imp::target_descriptor_index()
    {
        return memory_object_desc.target_descriptor_index;
    }

    uint64_t STDCALL memory_object_descriptor_imp::start_address()
    {
        return memory_object_desc.start_address;
    }

    uint64_t STDCALL memory_object_descriptor_imp::maximum_length()
    {
        return memory_object_desc.maximum_length;
    }

    uint64_t STDCALL memory_object_descriptor_imp::length()
    {
        return memory_object_desc.length;
    }

    const char * STDCALL memory_object_descriptor_imp::memory_object_type_to_str()
    {
        if(memory_object_desc.memory_object_type < MEMORY_OBJECT_NUM_STRINGS)
        {
            return memory_object_type_str[memory_object_desc.memory_object_type];
        }

        return "UNKNOWN";
    }

}
