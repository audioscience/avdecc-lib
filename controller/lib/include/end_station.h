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
 * end_station.h
 *
 * Public End Station interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_END_STATION_H_
#define _AVDECC_CONTROLLER_LIB_END_STATION_H_

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
	class entity_descriptor;
	class descriptor_base;

	class end_station
	{
	public:
		/**
		 * Get the status of the End Station connection.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual char STDCALL get_end_station_connection_status() = 0;

		/**
		 * Get the GUID of the End Station.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_end_station_guid() = 0;

		/**
		 * Get the source MAC address of the End Station.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_end_station_mac() = 0;

		/**
		 * Send a READ_DESCRIPTOR command with notification id to read a descriptor from an AVDECC Entity.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_read_desc_cmd(void *notification_id, uint16_t desc_type, uint16_t desc_index) = 0;

		/**
		 * Get the number of Entity descriptors for this End Station.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL get_entity_desc_count() = 0;

		/**
		 * Get the corresponding Entity descriptor by index.
		 */
		AVDECC_CONTROLLER_LIB32_API virtual entity_descriptor * STDCALL get_entity_desc_by_index(uint32_t entity_desc_index) = 0;

		/**
		 * Get the corresponding descriptor object by type and index.
		 */
		//AVDECC_CONTROLLER_LIB32_API virtual template<class T> * STDCALL get_desc_by_type_and_index(uint32_t desc_type, uint32_t desc_index) = 0;
	};
}

#endif