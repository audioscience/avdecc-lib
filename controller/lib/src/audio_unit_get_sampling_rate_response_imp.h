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
 * audio_unit_get_sampling_rate_response_imp.h
 *
 * Audio Unit get sampling rate response implementation class
 */

#pragma once

#include "audio_unit_get_sampling_rate_response.h"
#include "jdksavdecc_aem_command.h"

namespace avdecc_lib
{
class audio_unit_get_sampling_rate_response_imp : public audio_unit_get_sampling_rate_response
{
private:
    uint8_t * m_frame;
    size_t m_size;
    ssize_t m_position;

public:
    audio_unit_get_sampling_rate_response_imp(uint8_t * frame, size_t frame_len, ssize_t pos);
    virtual ~audio_unit_get_sampling_rate_response_imp();

    uint32_t STDCALL get_sampling_rate_sampling_rate();
};
}
