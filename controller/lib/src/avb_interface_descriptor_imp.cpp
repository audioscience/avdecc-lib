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
 * avb_interface_descriptor_imp.cpp
 *
 * AVB Interface descriptor implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "avb_interface_descriptor_imp.h"

namespace avdecc_lib
{
	avb_interface_descriptor_imp::avb_interface_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
	{
		desc_avb_interface_read_returned = jdksavdecc_descriptor_avb_interface_read(&avb_interface_desc, frame, pos, frame_len);

		if(desc_avb_interface_read_returned < 0)
		{
			log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "desc_audio_read error");
			assert(desc_avb_interface_read_returned >= 0);
		}
	}

	avb_interface_descriptor_imp::~avb_interface_descriptor_imp() {}

	uint16_t STDCALL avb_interface_descriptor_imp::get_descriptor_type()
	{
		assert(avb_interface_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE);
		return avb_interface_desc.descriptor_type;
	}

	uint16_t STDCALL avb_interface_descriptor_imp::get_descriptor_index() const
	{
		return avb_interface_desc.descriptor_index;
	}

	uint8_t * STDCALL avb_interface_descriptor_imp::get_object_name()
	{
		return avb_interface_desc.object_name.value;
	}

	uint16_t STDCALL avb_interface_descriptor_imp::get_localized_description()
	{
		return avb_interface_desc.localized_description;
	}

	uint8_t * STDCALL avb_interface_descriptor_imp::get_mac_addr()
	{
		return avb_interface_desc.mac_address.value;
	}

	uint16_t STDCALL avb_interface_descriptor_imp::get_interface_flags()
	{
		return avb_interface_desc.interface_flags;
	}

	uint64_t STDCALL avb_interface_descriptor_imp::get_clock_identity()
	{
		return jdksavdecc_uint64_get(&avb_interface_desc.clock_identity, 0);
	}

	uint32_t STDCALL avb_interface_descriptor_imp::get_priority1()
	{
		return (uint32_t)avb_interface_desc.priority1;
	}

	uint32_t STDCALL avb_interface_descriptor_imp::get_clock_class()
	{
		return (uint32_t)avb_interface_desc.clock_class;
	}

	uint16_t STDCALL avb_interface_descriptor_imp::get_offset_scaled_log_variance()
	{
		return avb_interface_desc.offset_scaled_log_variance;
	}

	uint32_t STDCALL avb_interface_descriptor_imp::get_clock_accuracy()
	{
		return (uint32_t)avb_interface_desc.clock_accuracy;
	}

	uint32_t STDCALL avb_interface_descriptor_imp::get_priority2()
	{
		return (uint32_t)avb_interface_desc.priority2;
	}

	uint32_t STDCALL avb_interface_descriptor_imp::get_domain_number()
	{
		return (uint32_t)avb_interface_desc.domain_number;
	}

	uint32_t STDCALL avb_interface_descriptor_imp::get_log_sync_interval()
	{
		return (uint32_t)avb_interface_desc.log_sync_interval;
	}
}
