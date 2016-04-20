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
 * audio_map_descriptor_response_imp.h
 *
 * Audio Map descriptor response implementation class
 */

#pragma once

#include "audio_map_descriptor_response.h"
#include "jdksavdecc_aem_descriptor.h"
#include "descriptor_response_base_imp.h"

namespace avdecc_lib
{
class audio_map_descriptor_response_imp : public audio_map_descriptor_response, public virtual descriptor_response_base_imp
{
private:
    std::vector<struct audio_map_mapping> maps; // Store maps in a vector
public:
    audio_map_descriptor_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos);
    virtual ~audio_map_descriptor_response_imp();

    uint8_t * STDCALL object_name();
    uint16_t mappings_offset();
    uint16_t STDCALL number_of_mappings();
    int const STDCALL mapping(size_t index, struct audio_map_mapping &map);
};
}
