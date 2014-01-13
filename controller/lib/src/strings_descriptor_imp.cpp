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
 * strings_descriptor_imp.cpp
 *
 * Strings descriptor implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "strings_descriptor_imp.h"

namespace avdecc_lib
{
    strings_descriptor_imp::strings_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        desc_strings_read_returned = jdksavdecc_descriptor_strings_read(&strings_desc, frame, pos, frame_len);

        if(desc_strings_read_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, strings_desc_read error", end_station_obj->guid());
            assert(desc_strings_read_returned >= 0);
        }
    }

    strings_descriptor_imp::~strings_descriptor_imp() {}

    uint16_t STDCALL strings_descriptor_imp::descriptor_type() const
    {
        assert(strings_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_STRINGS);
        return strings_desc.descriptor_type;
    }

    uint16_t STDCALL strings_descriptor_imp::descriptor_index() const
    {
        return strings_desc.descriptor_index;
    }

    uint8_t * STDCALL strings_descriptor_imp::get_string_by_index(size_t string_index)
    {
        switch(string_index)
        {
            case 0:
                return strings_desc.string_0.value;
                break;

            case 1:
                return strings_desc.string_1.value;
                break;

            case 2:
                return strings_desc.string_2.value;
                break;

            case 3:
                return strings_desc.string_3.value;
                break;

            case 4:
                return strings_desc.string_4.value;
                break;

            case 5:
                return strings_desc.string_5.value;
                break;

            case 6:
                return strings_desc.string_6.value;
                break;

            default:
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_string_by_index error");
                break;
        }

        return 0;
    }
}
