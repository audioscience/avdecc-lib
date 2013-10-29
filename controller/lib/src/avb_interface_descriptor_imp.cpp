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
#include "log.h"
#include "avb_interface_descriptor_imp.h"

namespace avdecc_lib
{
	avb_interface_descriptor_imp::avb_interface_descriptor_imp() {}

	avb_interface_descriptor_imp::avb_interface_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len) : descriptor_base_imp(base_end_station_imp_ref)
	{
		desc_avb_interface_read_returned = jdksavdecc_descriptor_avb_interface_read(&avb_interface_desc, frame, pos, mem_buf_len);

		if(desc_avb_interface_read_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "desc_audio_read error");
			assert(desc_avb_interface_read_returned >= 0);
		}
	}

	avb_interface_descriptor_imp::~avb_interface_descriptor_imp() {}

	uint16_t STDCALL avb_interface_descriptor_imp::get_descriptor_type()
	{
		assert(avb_interface_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE);
		return avb_interface_desc.descriptor_type;
	}

	uint16_t STDCALL avb_interface_descriptor_imp::get_descriptor_index()
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

	struct jdksavdecc_eui48 STDCALL avb_interface_descriptor_imp::get_mac_address()
	{
		return avb_interface_desc.mac_address;
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

#ifdef DEBUG_DESCRIPTOR_FIELD_INFORMATION
	void avb_interface_descriptor_imp::print_avb_interface_desc_info()
	{
		uint64_t mac_address = avb_pdu_get_field_mac(avb_interface_desc.mac_address.value);
		std::cout << "\nAVB Interface Descriptor";
		std::cout << "\ndescriptor_type = 0x" << std::hex << get_descriptor_type();
		std::cout << "\ndescriptor_index = 0x" << std::hex << get_descriptor_index();
		std::cout << "\nobject_name = " << std::hex << get_object_name().value;
		std::cout << "\nlocalized_description = 0x" << std::hex << get_localized_description();
		std::cout << "\nmac_address = 0x" << std::hex << mac_address;
		std::cout << "\ninterface_flags = 0x" << std::hex << get_interface_flags();
		std::cout << "\nclock_identity = 0x" << std::hex << get_clock_identity();
		std::cout << "\npriority1 = " << std::dec << get_priority1();
		std::cout << "\nclock_class = " << std::dec << get_clock_class();
		std::cout << "\noffset_scaled_log_variance = " << std::dec << get_offset_scaled_log_variance();
		std::cout << "\nclock_accuracy = " << std::dec << get_clock_accuracy();
		std::cout << "\npriority2 = " << std::dec << get_priority2();
		std::cout << "\ndomain_number = " << std::dec << get_domain_number();
		std::cout << "\nlog_sync_interval = " << std::dec << get_log_sync_interval();
	}
#endif

}