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
 * response_frame.cpp
 *
 * Response Frame class stores the current frame whenever the replace_frame() method is called
 */

#include "response_frame.h"
#include "log_imp.h"
#include "enumeration.h"
#include "build.h"
#include <stdlib.h>
#include <iostream>

namespace avdecc_lib {
    
    response_frame::response_frame(const uint8_t *frame, size_t size, ssize_t pos)
    {
        position = pos;
        frame_size = size;
        buffer = (uint8_t *)malloc(frame_size * sizeof(uint8_t)); //allocate space for the new frame
        memcpy(buffer, frame, frame_size);
    }

    response_frame::~response_frame()
    {
        free(buffer);
    }
    
    int response_frame::replace_frame(const uint8_t *frame, ssize_t pos, size_t size)
    {
        uint8_t * replaced_buffer = NULL;

        if(size <= frame_size)
        {
            assert(size <= frame_size);
            memcpy(buffer, frame, size);
        }
        else
        {
            replaced_buffer = (uint8_t *) realloc (buffer, size * sizeof(uint8_t));
            if(replaced_buffer != NULL)
            {
                buffer = replaced_buffer;
                memcpy(buffer, frame, size);
            }
            else
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Error reallocating memory");
                free(buffer);
                return -1;
            }
        }
        position = pos;
        frame_size = size;
        
        return 0;
    }

    uint8_t * response_frame::get_buffer()
    {
        return buffer;
    }
    
    ssize_t response_frame::get_pos()
    {
        return position;
    }
    
    size_t response_frame::get_size()
    {
        return frame_size;
    }
}
