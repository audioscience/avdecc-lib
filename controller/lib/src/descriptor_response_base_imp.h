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
 * descriptor_response_base_imp.h
 *
 * Descriptor response base implemenation class
 */

#pragma once
#include "descriptor_response_base.h"
#include <cstddef>
#include <string>
#include <cstdio>
#include <cstdlib>

namespace avdecc_lib
{
class descriptor_response_base_imp : public virtual descriptor_response_base
{
public:
    descriptor_response_base_imp(const uint8_t *frame, size_t frame_len, size_t pos);
    virtual ~descriptor_response_base_imp();

    virtual uint8_t * STDCALL object_name();

protected:
    uint8_t * buffer;
    size_t position;
    size_t frame_size;
};
}
