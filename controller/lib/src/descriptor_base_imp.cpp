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
 * descriptor_base.cpp
 *
 * Descriptor base implementation
 */

#include "descriptor_base_imp.h"

namespace avdecc_lib
{
	descriptor_base_imp::descriptor_base_imp() {}

	descriptor_base_imp::descriptor_base_imp(end_station_imp *base)
	{
		base_end_station_imp_ref = base;
	}

	descriptor_base_imp::~descriptor_base_imp() {}

	uint16_t STDCALL descriptor_base_imp::get_descriptor_type()
	{

		return 0;
	}

	uint16_t STDCALL descriptor_base_imp::get_descriptor_index()
	{

		return 0;
	}

	int STDCALL descriptor_base_imp::send_acquire_entity_cmd(uint16_t desc_index, uint32_t acquire_entity_flags)
	{

		return 0;
	}

	int descriptor_base_imp::proc_acquire_entity_resp(uint8_t *base_pointer, uint16_t mem_buf_len)
	{

		return 0;
	}

	int STDCALL descriptor_base_imp::send_lock_entity_cmd(uint16_t desc_index, uint32_t lock_entity_flags)
	{

		return 0;
	}

	int descriptor_base_imp::proc_lock_entity_resp(uint8_t *base_pointer, uint16_t mem_buf_len)
	{

		return 0;
	}

	int STDCALL descriptor_base_imp::send_entity_avail_cmd()
	{

		return 0;
	}

	int descriptor_base_imp::proc_entity_avail_resp(uint8_t *base_pointer, uint16_t mem_buf_len)
	{

		return 0;
	}

	int STDCALL descriptor_base_imp::send_set_name_cmd(uint16_t desc_index, uint16_t name_index, uint16_t config_index, char * name)
	{

		return 0;
	}

	int descriptor_base_imp::proc_set_name_resp(uint8_t *base_pointer, uint16_t mem_buf_len)
	{

		return 0;
	}

	int STDCALL descriptor_base_imp::send_get_name_cmd(uint16_t desc_index, uint16_t name_index, uint16_t config_index)
	{

		return 0;
	}

	int descriptor_base_imp::proc_get_name_resp(uint8_t *base_pointer, uint16_t mem_buf_len)
	{

		return 0;
	}
}
