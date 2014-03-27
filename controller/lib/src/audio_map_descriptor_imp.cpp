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
 * audio_map_descriptor_imp.cpp
 *
 * Audio Map descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "audio_map_descriptor_imp.h"

namespace avdecc_lib
{
    audio_map_descriptor_imp::audio_map_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_audio_map_read(&audio_map_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("audio_map_desc_read error");
        }

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

    audio_map_descriptor_imp::~audio_map_descriptor_imp() {}

    uint16_t STDCALL audio_map_descriptor_imp::descriptor_type() const
    {
        assert(audio_map_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AUDIO_MAP);
        return audio_map_desc.descriptor_type;
    }

    uint16_t STDCALL audio_map_descriptor_imp::descriptor_index() const
    {
        return audio_map_desc.descriptor_index;
    }

    uint16_t audio_map_descriptor_imp::mappings_offset()
    {
        assert(audio_map_desc.mappings_offset == 8);
        return audio_map_desc.mappings_offset;
    }

    uint16_t STDCALL audio_map_descriptor_imp::number_of_mappings()
    {
        return audio_map_desc.number_of_mappings;
    }
    
    int STDCALL audio_map_descriptor_imp::mapping(size_t index, struct audio_map_mapping &map)
    {
        if (index >= audio_map_desc.number_of_mappings)
            return -1;

        map = maps.at(index);
        return 0;
    }


}
