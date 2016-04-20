/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2015 AudioScience Inc.
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
 * stream_port_output_get_audio_map_response_imp.h
 *
 * STREAM PORT OUTPUT get audio map response implementation class
 */

#pragma once

#include "stream_port_output_get_audio_map_response.h"
#include <vector>
#include "jdksavdecc_aem_command.h"

namespace avdecc_lib
{
class stream_port_output_get_audio_map_response_imp : public stream_port_output_get_audio_map_response
{
private:
    std::vector<struct stream_port_output_audio_mapping> maps; // Store maps in a vector

    uint8_t * m_frame;
    size_t m_size;
    ssize_t m_position;

    size_t offset;
public:
    stream_port_output_get_audio_map_response_imp(uint8_t *frame, size_t frame_len, ssize_t pos);
    virtual ~stream_port_output_get_audio_map_response_imp();

    uint16_t map_index();
    uint16_t number_of_mappings();
    int STDCALL mapping(size_t index, struct stream_port_output_audio_mapping &map);
};
}
