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
 * clock_domain_descriptor.h
 *
 * Public Clock Domain descriptor interface class
 */

#pragma once
#ifndef _CLOCK_DOMAIN_DESCRIPTOR_H_
#define _CLOCK_DOMAIN_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
        class clock_domain_descriptor : public virtual descriptor_base
        {
        public:
                /**
                 * Get the object_name of the clock_domain_descriptor object.
                 */
                AVDECC_LIB_API virtual uint8_t * STDCALL get_object_name() = 0;

                /**
                 * Get the localized_description of the clock_domain_descriptor object.
                 */
                AVDECC_LIB_API virtual uint16_t STDCALL get_localized_description() = 0;

                /**
                 * Get the clock_source_index of the clock_domain_descriptor object.
                 */
                AVDECC_LIB_API virtual uint16_t STDCALL get_clock_source_index() = 0;

                /**
                * Get the clock_sources_offset of the clock_domain_descriptor object.
                */
                AVDECC_LIB_API virtual uint16_t STDCALL get_clock_sources_offset() = 0;

                /**
                 * Get the clock_sources_count of the clock_domain_descriptor object.
                 */
                AVDECC_LIB_API virtual uint16_t STDCALL get_clock_sources_count() = 0;
        };
}

#endif