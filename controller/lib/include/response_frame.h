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
 * response_frame.h
 *
 * Store current response frame
 */

#pragma once

#include "build.h"
#include <stdint.h>
#include <cstring>
#include <assert.h>

namespace avdecc_lib
{
    class response_frame
    {
    public:
        response_frame(const uint8_t *frame, size_t size, ssize_t pos);
        virtual ~response_frame();
        
        uint8_t * buffer; //buffer to store the current descriptor frame
        size_t frame_size;
        ssize_t position;
        
        void replace_frame(const uint8_t* frame, ssize_t pos, size_t size);
        uint8_t * get_buffer(); //methods to fetch frame info
        ssize_t get_pos();
        size_t get_size();
    };
}
