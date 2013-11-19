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
 * clock_source_descriptor.h
 *
 * Public Clock Source descriptor interface class
 * The Clock Source descriptor describes a clock source. A clock source may be an
 * internal oscillator, an external clock source such as a word clock or SPDIF
 * jack or an Input Stream.
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_CLOCK_SOURCE_DESCRIPTOR_H_
#define _AVDECC_CONTROLLER_LIB_CLOCK_SOURCE_DESCRIPTOR_H_

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
        class clock_source_descriptor : public virtual descriptor_base
        {
        public:
                /**
                 * The flags describing the capabilities or features of the clock source.
		 *
		 * \return 1 (Stream ID) if the Input Stream clock source is identified by the stream ID.
		 *	   2 (Local ID) if the Input Stream clock source is identified by it's local ID.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_flags() = 0;

                /**
                 * The type of clock source.
		 *
		 * \return 0 (Internal) if the clock is sourced from within the entity such as from a crystal oscillator.
		 *	   1 (External) if the clock is sourced from an external connection on the entity via a Jack.
		 *	   2 (Input Stream) if the clock is sourced from the media clock of an Input Stream.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_type() = 0;

                /**
                 * \return The identifier of the Clock Source.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint64_t STDCALL get_clock_source_identifier() = 0;

                /**
                 * \return The descriptor type of the object that this Clock Source is associated with.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_location_type() = 0;

                /**
                 * \return The descriptor index of the object that this Clock Source is associated with.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_location_index() = 0;
        };
}

#endif
