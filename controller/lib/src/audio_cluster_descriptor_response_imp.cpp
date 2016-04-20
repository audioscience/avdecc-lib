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
 * audio_cluster_descriptor_response_imp.cpp
 *
 * AUDIO CLUSTER descriptor response implementation
 */

#include <vector>
#include "audio_cluster_descriptor_response_imp.h"

namespace avdecc_lib
{
audio_cluster_descriptor_response_imp::audio_cluster_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos) :
    descriptor_response_base_imp(frame, frame_len, pos) {}

audio_cluster_descriptor_response_imp::~audio_cluster_descriptor_response_imp() {}

uint8_t * STDCALL audio_cluster_descriptor_response_imp::object_name()
{
    return (uint8_t *)&buffer[position + JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER_OFFSET_OBJECT_NAME];
}

uint16_t STDCALL audio_cluster_descriptor_response_imp::localized_description()
{
    return jdksavdecc_descriptor_audio_cluster_get_localized_description(buffer, position);
}

uint16_t STDCALL audio_cluster_descriptor_response_imp::signal_type()
{
    return jdksavdecc_descriptor_audio_cluster_get_signal_type(buffer, position);
}

uint16_t STDCALL audio_cluster_descriptor_response_imp::signal_index()
{
    return jdksavdecc_descriptor_audio_cluster_get_signal_index(buffer, position);
}

uint16_t STDCALL audio_cluster_descriptor_response_imp::signal_output()
{
    return jdksavdecc_descriptor_audio_cluster_get_signal_output(buffer, position);
}

uint32_t STDCALL audio_cluster_descriptor_response_imp::path_latency()
{
    return jdksavdecc_descriptor_audio_cluster_get_path_latency(buffer, position);
}

uint32_t STDCALL audio_cluster_descriptor_response_imp::block_latency()
{
    return jdksavdecc_descriptor_audio_cluster_get_block_latency(buffer, position);
}

uint16_t STDCALL audio_cluster_descriptor_response_imp::channel_count()
{
    return jdksavdecc_descriptor_audio_cluster_get_channel_count(buffer, position);
}

uint8_t STDCALL audio_cluster_descriptor_response_imp::format()
{
    return jdksavdecc_descriptor_audio_cluster_get_format(buffer, position);
}
}
