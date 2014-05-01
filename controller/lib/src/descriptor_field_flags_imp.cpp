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
 * descriptor_field_flags_imp.h
 *
 * Implementation of a class to describe a single flag bit.
 */

#include <stdint.h>
#include "build.h"

#include "descriptor_field_flags_imp.h"

namespace avdecc_lib
{
    descriptor_field_flags_imp::descriptor_field_flags_imp(const char * name, uint32_t mask) :
            m_mask(mask), m_name(name)
    {
    }

    descriptor_field_flags_imp::~descriptor_field_flags_imp()
    {
    }

    const char * STDCALL descriptor_field_flags_imp::get_flag_name(void) const
    {
        return m_name;
    }

    uint32_t STDCALL descriptor_field_flags_imp::get_flag_mask(void) const
    {
        return m_mask;
    }
}

