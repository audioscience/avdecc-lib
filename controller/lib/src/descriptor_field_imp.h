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
 * descriptor_field.h
 *
 * Public descriptor base interface class
 */

#pragma once

#include <vector>
#include <stdint.h>
#include "build.h"

#include "descriptor_field_flags_imp.h"
#include "descriptor_field.h"

namespace avdecc_lib
{
    class descriptor_field_imp : public descriptor_field
    {
    public:
        descriptor_field_imp(const char * name, enum aem_desc_field_types the_type, void * v);
        virtual ~descriptor_field_imp();

        void append_field(descriptor_field_flags_imp *bit_field);

        const char * STDCALL get_name() const;
        enum descriptor_field::aem_desc_field_types STDCALL get_type() const;
        char * STDCALL get_char() const;
        uint16_t STDCALL get_uint16() const;
        uint32_t STDCALL get_uint32() const;
        uint32_t STDCALL get_flags() const;
        uint32_t STDCALL get_flags_count() const;
        descriptor_field_flags * STDCALL get_flag_by_index(uint32_t index) const;
    private:
        const char * m_name;
        void * m_value;
        enum aem_desc_field_types m_type;
        std::vector<descriptor_field_flags_imp *> m_fields;
    };
}

