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
 * strings_descriptor_imp.h
 *
 * Strings descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AVDECC_CONTROLLER_LIB_STRINGS_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_STRINGS_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "strings_descriptor.h"

namespace avdecc_lib
{
        class strings_descriptor_imp : public virtual strings_descriptor, public virtual descriptor_base_imp
        {
        private:
                struct jdksavdecc_descriptor_strings strings_desc; // Structure containing the strings_desc fields
                int desc_strings_read_returned; // Status of extracting Strings descriptor information from a network buffer

        public:
                /**
                 * An empty constructor for strings_descriptor_imp
                 */
                strings_descriptor_imp();

                /**
                 * Constructor for strings_descriptor_imp used for constructing an object with a base End Station, frame, position offest,
                 * and memory buffer length.
                 */
                strings_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len);

                /**
                 * Destructor for strings_descriptor_imp used for destroying objects
                 */
                virtual ~strings_descriptor_imp();

                /**
                 * Get the descriptor_type of the strings_descriptor object.
                 */
                uint16_t STDCALL get_descriptor_type();

                /**
                 * Get the descriptor_index of the strings_descriptor object.
                 */
                uint16_t STDCALL get_descriptor_index();

                /**
                 * Get the corresponding string of the strings_descriptor object.
                 */
                uint8_t * STDCALL get_string_by_index(uint32_t string_index);

                /**
                 * Print out Strings Descriptor fields.
                 */
                //	void print_strings_desc_info();
        };
}

#endif