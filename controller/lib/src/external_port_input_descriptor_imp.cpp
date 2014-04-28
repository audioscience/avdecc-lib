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
 * external_port_input_descriptor_imp.cpp
 *
 * EXTERNAL_PORT_INPUT descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "external_port_input_descriptor_imp.h"
#include "descriptor_field_imp.h"

namespace avdecc_lib
{
    external_port_input_descriptor_imp::external_port_input_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_external_port_read(&desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("jdksavdecc_descriptor_external_port_read error");
        }

		// fields
		descriptor_field_imp *f;

		m_fields.push_back(new descriptor_field_imp("clock_domain_index", descriptor_field::TYPE_UINT16, &desc.clock_domain_index));

		f = new descriptor_field_imp("port_flags", descriptor_field::TYPE_FLAGS16, &desc.port_flags);
		f->append_field(new descriptor_field_flags_imp("CLOCK_SYNC_SOURCE", 1 << 15));
		f->append_field(new descriptor_field_flags_imp("ASYNC_SAMPLE_RATE_CONVERTER", 1 << 14));
		f->append_field(new descriptor_field_flags_imp("SYNC_SAMPLE_RATE_CONVERTER", 1 << 13));
		m_fields.push_back(f);

		m_fields.push_back(new descriptor_field_imp("number_of_controls", descriptor_field::TYPE_UINT16, &desc.number_of_controls));
		m_fields.push_back(new descriptor_field_imp("base_control", descriptor_field::TYPE_UINT16, &desc.base_control));

		m_fields.push_back(new descriptor_field_imp("signal_type", descriptor_field::TYPE_UINT16, &desc.signal_type));
		m_fields.push_back(new descriptor_field_imp("signal_index", descriptor_field::TYPE_UINT16, &desc.signal_index));
		m_fields.push_back(new descriptor_field_imp("signal_output", descriptor_field::TYPE_UINT16, &desc.signal_output));

		m_fields.push_back(new descriptor_field_imp("block_latency", descriptor_field::TYPE_UINT16, &desc.block_latency));
		m_fields.push_back(new descriptor_field_imp("jack_index", descriptor_field::TYPE_UINT16, &desc.jack_index));
 }

    external_port_input_descriptor_imp::~external_port_input_descriptor_imp() {}

    uint16_t STDCALL external_port_input_descriptor_imp::descriptor_type() const
    {
        assert(desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_INPUT);
        return desc.descriptor_type;
    }

    uint16_t STDCALL external_port_input_descriptor_imp::descriptor_index() const
    {
        return desc.descriptor_index;
    }

    uint16_t STDCALL external_port_input_descriptor_imp::port_flags()
    {
        return desc.port_flags;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::clock_domain_index()
    {
        return desc.clock_domain_index;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::number_of_controls()
    {
        return desc.number_of_controls;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::base_control()
    {
        return desc.base_control;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::signal_type()
    {
        return desc.signal_type;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::signal_index()
    {
        return desc.signal_index;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::signal_output()
    {
        return desc.signal_output;
    }
    uint32_t STDCALL external_port_input_descriptor_imp::block_latency()
    {
        return desc.block_latency;
    }
    uint16_t STDCALL external_port_input_descriptor_imp::jack_index()
    {
        return desc.jack_index;
    }


}
