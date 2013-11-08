/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2013 AudioScience Inc.
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
 * jack_input_descriptor_imp.cpp
 *
 * Jack Input descriptor implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "jack_input_descriptor_imp.h"

namespace avdecc_lib
{
	jack_input_descriptor_imp::jack_input_descriptor_imp() {}

	jack_input_descriptor_imp::jack_input_descriptor_imp(end_station_imp *base_end_station_imp_ref, const uint8_t *frame, size_t pos, size_t mem_buf_len) : descriptor_base_imp(base_end_station_imp_ref)
	{
		desc_jack_input_read_returned = jdksavdecc_descriptor_jack_read(&jack_input_desc, frame, pos, mem_buf_len);

		if(desc_jack_input_read_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "desc_jack_input_read error");
			assert(desc_jack_input_read_returned >= 0);
		}

		jack_flags_init();
	}

	jack_input_descriptor_imp::~jack_input_descriptor_imp() {}

	void jack_input_descriptor_imp::jack_flags_init()
	{
		jack_flags.clock_sync_source = jack_input_desc.jack_flags >> 1 & 0x01;
		jack_flags.captive = jack_input_desc.jack_flags >> 2 & 0x01;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_descriptor_type()
	{
		assert(jack_input_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_JACK_INPUT);
		return jack_input_desc.descriptor_type;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_descriptor_index()
	{
		return jack_input_desc.descriptor_index;
	}

	uint8_t * STDCALL jack_input_descriptor_imp::get_object_name()
	{
		return jack_input_desc.object_name.value;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_localized_description()
	{
		return jack_input_desc.localized_description;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_jack_flags()
	{
		return jack_input_desc.jack_flags;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_jack_flag_clock_sync_source()
	{
		return jack_flags.clock_sync_source;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_captive()
	{
		return jack_flags.captive;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_jack_type()
	{
		return jack_input_desc.jack_type;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_number_of_controls()
	{
		return jack_input_desc.number_of_controls;
	}

	uint16_t STDCALL jack_input_descriptor_imp::get_base_control()
	{
		return jack_input_desc.base_control;
	}
}
