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
 * clock_domain_descriptor_imp.cpp
 *
 * Clock Domain descriptor implementation
 */

#include <vector>
#include "enumeration.h"
#include "log.h"
#include "clock_domain_descriptor_imp.h"

namespace avdecc_lib
{
        clock_domain_descriptor_imp::clock_domain_descriptor_imp() {}

        clock_domain_descriptor_imp::clock_domain_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len) : descriptor_base_imp(base_end_station_imp_ref)
        {
                desc_clock_domain_read_returned = jdksavdecc_descriptor_clock_domain_read(&clock_domain_desc, frame, pos, mem_buf_len);

                if(desc_clock_domain_read_returned < 0)
                {
                        avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "desc_clock_domain_read error");
                        assert(desc_clock_domain_read_returned >= 0);
                }

                store_clock_sources(frame, pos);
        }

        clock_domain_descriptor_imp::~clock_domain_descriptor_imp() {}

        uint16_t STDCALL clock_domain_descriptor_imp::get_descriptor_type()
        {
                assert(clock_domain_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN);
                return clock_domain_desc.descriptor_type;
        }

        uint16_t STDCALL clock_domain_descriptor_imp::get_descriptor_index()
        {
                return clock_domain_desc.descriptor_index;
        }

        uint8_t * STDCALL clock_domain_descriptor_imp::get_object_name()
        {
                return clock_domain_desc.object_name.value;
        }

        uint16_t STDCALL clock_domain_descriptor_imp::get_localized_description()
        {
                return clock_domain_desc.localized_description;
        }

        uint16_t STDCALL clock_domain_descriptor_imp::get_clock_source_index()
        {
                return clock_domain_desc.clock_source_index;
        }

        uint16_t STDCALL clock_domain_descriptor_imp::get_clock_sources_offset()
        {
                assert(clock_domain_desc.clock_sources_offset == 76);
                return clock_domain_desc.clock_sources_offset;
        }

        uint16_t STDCALL clock_domain_descriptor_imp::get_clock_sources_count()
        {
                assert(clock_domain_desc.clock_sources_count <= 249);
                return clock_domain_desc.clock_sources_count;
        }

        void clock_domain_descriptor_imp::store_clock_sources(uint8_t *frame, size_t pos)
        {
                uint16_t offset = 0x0;

                for(uint32_t index_i = 0; index_i < get_clock_sources_count(); index_i++)
                {
                        clock_src_vec.push_back(jdksavdecc_uint16_get(frame, get_clock_sources_offset() + pos + offset));
                        offset += 0x2;
                }
        }

        //void clock_domain_descriptor_imp::print_clock_domain_desc_info()
        //{
        //	std::cout << "\nClock Domain Descriptor";
        //	std::cout << "\ndescriptor_type = 0x" << std::hex << get_descriptor_type();
        //	std::cout << "\ndescriptor_index = 0x" << std::hex << get_descriptor_index();
        //	std::cout << "\nobject_name = " << std::hex << get_object_name().value;
        //	std::cout << "\nlocalized_description = 0x" << std::hex << get_localized_description();
        //	std::cout << "\nclock_source_index = 0x" << std::hex << get_clock_source_index();
        //	std::cout << "\nclock_sources_offset = " << std::dec << get_clock_sources_offset();
        //	std::cout << "\nclock_sources_count = " << std::dec << get_clock_sources_count();
        //	print_clock_sources();
        //}
}