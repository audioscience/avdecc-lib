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
 * descriptor_field_imp.cpp
 *
 * Descriptor field class implementation
 */

#include <algorithm>
#include <vector>
#include <assert.h>
#include <stdint.h>
#include "build.h"

#include "descriptor_field_flags_imp.h"
#include "descriptor_field_imp.h"

static void delete_bitfield(avdecc_lib::descriptor_field_flags_imp *f)
{
    delete f;
}

namespace avdecc_lib
{

    descriptor_field_imp::descriptor_field_imp(const char * name, enum aem_desc_field_types the_type, void * v) :
            m_name(name), m_value(v) , m_type(the_type)
    {
    }

    descriptor_field_imp::~descriptor_field_imp()
    {
        std::for_each(m_fields.begin(), m_fields.end(), delete_bitfield);
        m_fields.clear();
    }

    enum descriptor_field::aem_desc_field_types STDCALL descriptor_field_imp::get_type() const
    {
        return m_type;
    }

    void descriptor_field_imp::append_field(descriptor_field_flags_imp *bit_field)
    {
        m_fields.push_back(bit_field);
    }

    const char * STDCALL descriptor_field_imp::get_name() const
    {
        return m_name;
    }

    char * STDCALL descriptor_field_imp::get_char() const
    {
        assert(m_type == TYPE_CHAR);
        return (char *)m_value;
    }

    uint16_t STDCALL descriptor_field_imp::get_uint16() const
    {
        assert(m_type == TYPE_UINT16);
        return *(uint16_t *)m_value;
    }

    uint32_t STDCALL descriptor_field_imp::get_uint32() const
    {
        assert(m_type == TYPE_UINT32);
        return *(uint32_t *)m_value;
    }

    uint32_t STDCALL descriptor_field_imp::get_flags() const
    {
        uint32_t flag;
        assert((m_type == TYPE_FLAGS16) || (m_type == TYPE_FLAGS16));
        if (m_type == TYPE_FLAGS16)
        {
            flag = (uint32_t)*(uint16_t *)m_value;
        }
        else
        {
            flag = *(uint32_t *)m_value;

        }
        return flag;
    }

    uint32_t STDCALL descriptor_field_imp::get_flags_count() const
    {
        assert((m_type == TYPE_FLAGS16) || (m_type == TYPE_FLAGS16));
        return m_fields.size();
    }

    descriptor_field_flags * STDCALL descriptor_field_imp::get_flag_by_index(uint32_t index) const
    {
        assert((m_type == TYPE_FLAGS16) || (m_type == TYPE_FLAGS16));
        return m_fields[index];
    }
}

