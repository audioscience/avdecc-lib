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
 * clock_domain_descriptor_imp.h
 *
 * Clock Domain descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CLOCK_DOMAIN_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_CLOCK_DOMAIN_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "clock_domain_descriptor.h"

namespace avdecc_lib
{
        class clock_domain_descriptor_imp : public virtual clock_domain_descriptor, public virtual descriptor_base_imp
        {
        private:
                struct jdksavdecc_descriptor_clock_domain clock_domain_desc; // Store the Clock Domain Descriptor fields
                int desc_clock_domain_read_returned; // Status of extracting Clock Domain descriptor information from a network buffer
                std::vector<uint16_t> clk_src_vec; // Store clock sources in a vector

                struct jdksavdecc_aem_command_set_clock_source_response aem_cmd_set_clk_src_resp; // Store the response received after sending a SET_CLOCK_SOURCE command
                struct jdksavdecc_aem_command_get_clock_source_response aem_cmd_get_clk_src_resp; // Store the response received after sending a GET_CLOCK_SOURCE command

                /**
                * Store the Clock Sources of the Clock Domain descriptor object.
                */
                void store_clock_sources(const uint8_t *frame, size_t pos);

        public:
                /**
                 * Constructor for Clock Domain descriptor object.
                 *
                 * \param end_station_obj A pointer to the base End Station object.
                 * \param frame The raw memory that contains the descriptor information to read from.
                 * \param pos The position offset to read the descriptor fields from.
                 * \param frame_len The memory buffer length of the descriptor frame.
                 */
                clock_domain_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

                virtual ~clock_domain_descriptor_imp();

                /**
                 * Get the type of the descriptor.
                 */
                uint16_t STDCALL get_descriptor_type() const;

                /**
                 * Get the index of the descriptor.
                 */
                uint16_t STDCALL get_descriptor_index() const;

                /**
                 * Get the name of the Clock Domain. This may be user set through the use of a SET_NAME command.
		 * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
		 * defined value being provided in a localized form via the localized descripton field. By leaving
		 * this field blank an AVDECC Controller can determine if the user has overridden the name and can
		 * use this name rather than the localized name.
                 */
                uint8_t * STDCALL get_object_name();

                /**
                 * Get the localized string reference pointing to the localized Clock Domain name.
                 */
                uint16_t STDCALL get_localized_description();

                /**
                 * Get the descriptor index of the Clock Source descriptor describing the current Clock Source
		 * for the Clock Domain.
                 */
                uint16_t STDCALL get_clock_source_index();

                /**
		 * Get the offset to the clock sources field from the start of the descriptor.
		 * This is 76 for this version of AEM.
                 */
                uint16_t STDCALL get_clock_sources_offset();

                /**
                 * Get the number of clock source indexes in the clock sources field. The maximum value for this field
		 * is 249 for this version of AEM.
                 */
                uint16_t STDCALL get_clock_sources_count();

                /**
                 * Get the corresponding Clock Source by index present in the Clock Domain.
                 */
                uint16_t STDCALL get_clock_source_by_index(uint32_t clk_src_index);

                /**
                 * Get the clock source index of the requested Clock Domain after sending a
                 * SET_CLOCK_SOURCE command and receiving a response back for the command.
                 */
                uint16_t STDCALL set_clock_source_clock_source_index();

                /**
                 * Get the clock source index of the requested Clock Domain after sending a
                 * GET_CLOCK_SOURCE command and receiving a response back for the command.
                 */
                uint16_t STDCALL get_clock_source_clock_source_index();

                /**
                 * Send a SET_CLOCK_SOURCE command to change the clock source of a clock domain.
                 */
                int STDCALL send_set_clock_source_cmd(void *notification_id, uint16_t new_clk_src_index);

                /**
                 * Process a SET_CLOCK_SOURCE response for the SET_CLOCK_SOURCE command.
                 */
                int proc_set_clock_source_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);

                /**
                 * Send a GET_CLOCK_SOURCE command to get the current clock source of a clock domain.
                 */
                int STDCALL send_get_clock_source_cmd(void *notification_id);

                /**
                 * Process a GET_CLOCK_SOURCE response for the GET_CLOCK_SOURCE command.
                 */
                int proc_get_clock_source_resp(void *&notification_id, const uint8_t *frame, uint16_t frame_len, int &status);
        };
}

#endif
