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
 * entity_descriptor_imp.h
 *
 * Entity descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_ENTITY_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_ENTITY_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "configuration_descriptor_imp.h"
#include "entity_descriptor.h"

namespace avdecc_lib
{
	class entity_descriptor_imp : public virtual entity_descriptor, public virtual descriptor_base_imp
	{
	private:
		struct jdksavdecc_descriptor_entity entity_desc; // Structure containing the entity_desc fields
		int desc_entity_read_returned; // Status of extracting ENTITY descriptor information from a network buffer
		std::vector<configuration_descriptor_imp *> config_desc_vec; // Store a list of Configuration descriptor class objects

	public:
		/**
		 * An empty constructor for entity_descriptor_imp
		 */
		entity_descriptor_imp();

		/**
		 * Constructor for entity_descriptor_imp used for constructing an object with a base End Station, frame, position offest,
		 * and memory buffer length.
		 */
		entity_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

		/**
		 * Destructor for entity_descriptor_imp used for destroying objects
		 */
		virtual ~entity_descriptor_imp();

		/**
		 * Get the descriptor type of the Entity descriptor object.
		 */
		uint16_t STDCALL get_descriptor_type();

		/**
		 * Get the descriptor index of the Entity descriptor object.
		 */
		uint16_t STDCALL get_descriptor_index();

		/**
		 * Get the id of the Entity descriptor object.
		 */
		uint64_t STDCALL get_entity_id();

		/**
		 * Get the vendor id of the Entity descriptor object.
		 */
		uint32_t STDCALL get_vendor_id();

		/**
		 * Get the model id of the Entity descriptor object.
		 */
		uint32_t STDCALL get_entity_model_id();

		/**
		 * Get the capabilities of the Entity descriptor object.
		 */
		uint32_t STDCALL get_entity_capabilities();

		/**
		 * Get the Talker Stream Sources of the Entity descriptor object.
		 */
		uint16_t STDCALL get_talker_stream_sources();

		/**
		 * Get the Talker capabilities of the Entity descriptor object.
		 */
		uint16_t STDCALL get_talker_capabilities();

		/**
		 * Get the Listener Stream Sinks of the Entity descriptor object.
		 */
		uint16_t STDCALL get_listener_stream_sinks();

		/**
		 * Get the Listener capabilities of the Entity descriptor object.
		 */
		uint16_t STDCALL get_listener_capabilities();

		/**
		 * Get the Controller capabilities of the Entity descriptor object.
		 */
		uint32_t STDCALL get_controller_capabilities();

		/**
		 * Get the available index of the Entity descriptor object.
		 */
		uint32_t STDCALL get_available_index();

		/**
		 * Get the association id of the Entity descriptor object.
		 */
		uint64_t STDCALL get_association_id();

		/**
		 * Get the name of the Entity descriptor object.
		 */
		uint8_t * STDCALL get_entity_name();

		/**
		 * Get the vendor name string of the Entity descriptor object.
		 */
		uint16_t STDCALL get_vendor_name_string();

		/**
		 * Get the model name string of the Entity descriptor object.
		 */
		uint16_t STDCALL get_model_name_string();

		/**
		 * Get the firmware version of the Entity descriptor object.
		 */
		uint8_t * STDCALL get_firmware_version();

		/**
		 * Get the group name of the Entity descriptor object.
		 */
		uint8_t * STDCALL get_group_name();

		/**
		 * Get the serial number of the Entity descriptor object.
		 */
		uint8_t * STDCALL get_serial_number();

		/**
		 * Get the configurations count of the Entity descriptor object.
		 */
		uint16_t STDCALL get_configurations_count();

		/**
		 * Get the current Configuration of the Entity descriptor object.
		 */
		uint16_t STDCALL get_current_configuration();

		/**
		 * Store Configuration descriptor object.
		 */
		void store_config_desc(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

		/**
		 * Get the number of Configuration descriptors.
		 */
		uint32_t STDCALL get_config_desc_count();

		/**
		 * Get the corresponding Configuration descriptor object by index.
		 */
		configuration_descriptor * STDCALL get_config_desc_by_index(uint16_t config_desc_index);

		/**
		 * Send a SET_CONFIFURATION command to change the current configuration of the AVDECC Entity.
		 */
		int STDCALL send_set_config_cmd();

		/**
		 * Process a SET_CONFIFURATION response for the SET_CONFIGURATION command.
		 */
		int proc_set_config_resp();

		/**
		 * Send a GET_CONFIGURATION command to get the current configuration of the AVDECC Entity.
		 */
		int STDCALL send_get_config_cmd();

		/**
		 * Process a GET_CONFIGURATION response for the GET_CONFIFURATION command.
		 */
		int proc_get_config_resp();
	};
}

#endif