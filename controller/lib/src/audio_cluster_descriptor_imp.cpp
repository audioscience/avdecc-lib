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
 * audio_cluster_descriptor_imp.cpp
 *
 * Audio Cluster descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "audio_cluster_descriptor_imp.h"

namespace avdecc_lib
{
    audio_cluster_descriptor_imp::audio_cluster_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_audio_cluster_read(&audio_cluster_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("audio_cluster_desc_read error");
        }
    }

    audio_cluster_descriptor_imp::~audio_cluster_descriptor_imp() {}

    uint16_t STDCALL audio_cluster_descriptor_imp::descriptor_type() const
    {
        assert(audio_cluster_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER);
        return audio_cluster_desc.descriptor_type;
    }

    uint16_t STDCALL audio_cluster_descriptor_imp::descriptor_index() const
    {
        return audio_cluster_desc.descriptor_index;
    }

    uint8_t * STDCALL audio_cluster_descriptor_imp::object_name()
    {
        return audio_cluster_desc.object_name.value;
    }

    uint16_t STDCALL audio_cluster_descriptor_imp::localized_description()
    {
        return audio_cluster_desc.localized_description;
    }

    uint16_t STDCALL audio_cluster_descriptor_imp::signal_type()
    {
        return audio_cluster_desc.signal_type;
    }

    uint16_t STDCALL audio_cluster_descriptor_imp::signal_index()
    {
        return audio_cluster_desc.signal_index;
    }

    uint16_t STDCALL audio_cluster_descriptor_imp::signal_output()
    {
        return audio_cluster_desc.signal_output;
    }

    uint32_t STDCALL audio_cluster_descriptor_imp::path_latency()
    {
        return audio_cluster_desc.path_latency;
    }

    uint32_t STDCALL audio_cluster_descriptor_imp::block_latency()
    {
        return audio_cluster_desc.block_latency;
    }

    uint16_t STDCALL audio_cluster_descriptor_imp::channel_count()
    {
        return audio_cluster_desc.channel_count;
    }

    uint8_t STDCALL audio_cluster_descriptor_imp::format()
    {
        return audio_cluster_desc.format;
    }
}
