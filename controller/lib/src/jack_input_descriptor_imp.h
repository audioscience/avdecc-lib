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
#include "jack_input_descriptor_response_imp.h"

namespace avdecc_lib
{
    class jack_input_descriptor_imp : public jack_input_descriptor, public virtual descriptor_base_imp
    {
    public:
        jack_input_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~jack_input_descriptor_imp();
        
        jack_input_descriptor_response_imp *resp;

        jack_input_descriptor_response * STDCALL get_jack_input_response();
    private:
        /**
         * Store the jack flags componenets of the JACK INPUT descriptor object in a vector.
         */
        void jack_flags_init();
    };
}

