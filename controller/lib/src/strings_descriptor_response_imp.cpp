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
 * strings_descriptor_response_imp.cpp
 *
 * STRINGS descriptor response implementation
 */

#include "strings_descriptor_response_imp.h"

namespace avdecc_lib
{
    strings_descriptor_response_imp::strings_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) : descriptor_base_imp(nullptr, frame, frame_len, pos)
    {
        frame_size = frame_len;
        buffer = (uint8_t *)malloc(frame_size * sizeof(uint8_t));
        memcpy(buffer, frame, frame_size);
        position = pos;
    }
    
    strings_descriptor_response_imp::~strings_descriptor_response_imp() {}
    
    uint8_t * STDCALL strings_descriptor_response_imp::get_string_by_index(size_t string_index)
    {
        switch(string_index)
        {
            case 0:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_0];
                break;
                
            case 1:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_1];
                break;
                
            case 2:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_2];
                break;
                
            case 3:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_3];
                break;
                
            case 4:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_4];
                break;
                
            case 5:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_5];
                break;
                
            case 6:
                return (uint8_t *) &buffer[position + JDKSAVDECC_DESCRIPTOR_STRINGS_OFFSET_STRING_6];
                break;
                
            default:
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_string_by_index error");
                break;
        }
        return 0;
    }
}
