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
                struct jdksavdecc_descriptor_clock_domain clock_domain_desc; // Structure containing the clock_domain_desc fields
                int desc_clock_domain_read_returned; // Status of extracting Clock Domain descriptor information from a network buffer
                std::vector<uint16_t> clock_src_vec; // Store clock sources in a vector

        public:
                /**
                 * An empty constructor for clock_domain_descriptor_imp
                 */
                clock_domain_descriptor_imp();

                /**
                 * Constructor for clock_domain_descriptor_imp used for constructing an object with a base End Station, frame, position offest,
                 * and memory buffer length.
                 */
                clock_domain_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

                /**
                 * Destructor for clock_domain_descriptor_imp used for destroying objects
                 */
                virtual ~clock_domain_descriptor_imp();

                /**
                 * Get the descriptor_type of the clock_domain_descriptor object.
                 */
                uint16_t STDCALL get_descriptor_type();

                /**
                 * Get the descriptor_index of the clock_domain_descriptor object.
                 */
                uint16_t STDCALL get_descriptor_index();

                /**
                 * Get the object_name of the clock_domain_descriptor object.
                 */
                uint8_t * STDCALL get_object_name();

                /**
                 * Get the localized_description of the clock_domain_descriptor object.
                 */
                uint16_t STDCALL get_localized_description();

                /**
                 * Get the clock_source_index of the clock_domain_descriptor object.
                 */
                uint16_t STDCALL get_clock_source_index();

                /**
                * Get the clock_sources_offset of the clock_domain_descriptor object.
                */
                uint16_t STDCALL get_clock_sources_offset();

                /**
                 * Get the clock_sources_count of the clock_domain_descriptor object.
                 */
                uint16_t STDCALL get_clock_sources_count();

                /**
                * Store the clock_sources of the clock_domain_descriptor object.
                */
                void store_clock_sources(uint8_t *frame, size_t pos);

                /**
                 * Print out Clock Domain Descriptor fields.
                 */
                //	void print_clock_domain_desc_info();
        };
}

#endif