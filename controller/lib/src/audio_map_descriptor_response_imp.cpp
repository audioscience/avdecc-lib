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
 * audio_map_descriptor_response_imp.cpp
 *
 * AUDIO MAP descriptor response implementation
 */

#include <vector>
#include "audio_map_descriptor_response_imp.h"

namespace avdecc_lib
{
    audio_map_descriptor_response_imp::audio_map_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) : descriptor_base_imp(nullptr, frame, frame_len, pos)
    {
        frame_size = frame_len;
        buffer = (uint8_t *)malloc(frame_size * sizeof(uint8_t));
        memcpy(buffer, frame, frame_size);
        position = pos;
        
        ssize_t offset = pos + mappings_offset();
        for (unsigned int i = 0; i < (unsigned int)number_of_mappings(); i++)
        {
            struct audio_map_mapping map;
            
            map.stream_index = jdksavdecc_uint16_get(frame, offset);
            map.stream_channel = jdksavdecc_uint16_get(frame, offset + 2);
            map.cluster_offset = jdksavdecc_uint16_get(frame, offset + 4);
            map.cluster_channel = jdksavdecc_uint16_get(frame, offset + 6);
            maps.push_back(map);
            offset += sizeof(struct audio_map_mapping);
        }
    }
    
    audio_map_descriptor_response_imp::~audio_map_descriptor_response_imp() {}
    
    uint16_t audio_map_descriptor_response_imp::mappings_offset()
    {
        return jdksavdecc_descriptor_audio_map_get_mappings_offset(buffer, position);
    }
    
    uint16_t STDCALL audio_map_descriptor_response_imp::number_of_mappings()
    {
        return jdksavdecc_descriptor_audio_map_get_number_of_mappings(buffer, position);
    }
    
    int STDCALL audio_map_descriptor_response_imp::mapping(size_t index, struct audio_map_mapping &map)
    {
        if (index >= number_of_mappings())
            return -1;
        
        map = maps.at(index);
        return 0;
    }
}
