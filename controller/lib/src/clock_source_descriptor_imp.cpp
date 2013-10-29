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
 * clock_source_descriptor_imp.cpp
 *
 * Clock Source_ descriptor implementation
 */

#include "enumeration.h"
#include "log.h"
#include "clock_source_descriptor_imp.h"

namespace avdecc_lib
{
	clock_source_descriptor_imp::clock_source_descriptor_imp() {}

	clock_source_descriptor_imp::clock_source_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len) : descriptor_base_imp(base_end_station_imp_ref)
	{
		desc_clock_source_read_returned = jdksavdecc_descriptor_clock_source_read(&clock_source_desc, frame, pos, mem_buf_len);

		if(desc_clock_source_read_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "desc_clock_source_read error");
			assert(desc_clock_source_read_returned >= 0);
		}
	}

	clock_source_descriptor_imp::~clock_source_descriptor_imp() {}

	uint16_t STDCALL clock_source_descriptor_imp::get_descriptor_type()
	{
		assert(clock_source_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE);
		return clock_source_desc.descriptor_type;
	}

	uint16_t STDCALL clock_source_descriptor_imp::get_descriptor_index()
	{
		return clock_source_desc.descriptor_index;
	}

	uint8_t * STDCALL clock_source_descriptor_imp::get_object_name()
	{
		return clock_source_desc.object_name.value;
	}

	uint16_t STDCALL clock_source_descriptor_imp::get_localized_description ()
	{
		return clock_source_desc.localized_description;
	}

	uint16_t STDCALL clock_source_descriptor_imp::get_clock_source_flags()
	{
		return clock_source_desc.clock_source_flags;
	}

	uint16_t STDCALL clock_source_descriptor_imp::get_clock_source_type()
	{
		return clock_source_desc.clock_source_type;
	}

	uint64_t STDCALL clock_source_descriptor_imp::get_clock_source_identifier()
	{
		return jdksavdecc_uint64_get(&clock_source_desc.clock_source_identifier, 0);
	}

	uint16_t STDCALL clock_source_descriptor_imp::get_clock_source_location_type()
	{
		return clock_source_desc.clock_source_location_type;
	}

	uint16_t STDCALL clock_source_descriptor_imp::get_clock_source_location_index()
	{
		return clock_source_desc.clock_source_location_index;
	}

#ifdef DEBUG_DESCRIPTOR_FIELD_INFORMATION
	void clock_source_descriptor_imp::print_clock_source_desc_info()
	{
		std::cout << "\nClock Source Descriptor";
		std::cout << "\ndescriptor_type = 0x" << std::hex << get_descriptor_type();
		std::cout << "\ndescriptor_index = 0x" << std::hex << get_descriptor_index();
		std::cout << "\nobject_name = " << std::hex << get_object_name().value;
		std::cout << "\nlocalized_description = 0x" << std::hex << get_localized_description();
		std::cout << "\nclock_source_flags = 0x" << std::hex << get_clock_source_flags();
		std::cout << "\nclock_source_type = 0x" << std::hex << get_clock_source_type();
		std::cout << "\nclock_source_identifier = 0x" << std::hex << get_clock_source_identifier();
		std::cout << "\nclock_source_location_type = 0x" << std::hex << get_clock_source_location_type();
		std::cout << "\nclock_source_location_index = 0x" << std::hex << get_clock_source_location_index();
	}
#endif

}