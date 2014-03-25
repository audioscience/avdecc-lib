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
 * jack_input_descriptor_imp.h
 *
 * JACK INPUT descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "jack_input_descriptor.h"

namespace avdecc_lib
{
    class jack_input_descriptor_imp : public jack_input_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_jack jack_input_desc; // Structure containing the jack_input_desc fields

        struct jack_input_desc_jack_flags
        {
            bool clock_sync_source;
            bool captive;
        };

        struct jack_input_desc_jack_flags jack_input_flags;

    public:
        jack_input_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~jack_input_descriptor_imp();

        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        uint8_t * STDCALL object_name();
        uint16_t STDCALL localized_description();

		uint16_t STDCALL jack_flags();
        uint16_t STDCALL jack_flag_clock_sync_source();
        uint16_t STDCALL jack_flag_captive();
        uint16_t STDCALL jack_type();
        uint16_t STDCALL number_of_controls();
        uint16_t STDCALL base_control();
    
    private:
        /**
         * Store the jack flags componenets of the JACK INPUT descriptor object in a vector.
         */
        void jack_flags_init();
    };
}

