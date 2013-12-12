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
 * clock_source_descriptor_imp.h
 *
 * CLOCK SOURCE descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "clock_source_descriptor.h"

namespace avdecc_lib
{
    class clock_source_descriptor_imp : public clock_source_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_clock_source clock_source_desc; // Structure containing the clock_source_desc fields
        ssize_t desc_clock_source_read_returned; // Status of extracting CLOCK SOURCE descriptor information from a network buffer

    public:
        /**
         * Constructor for CLOCK SOURCE descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        clock_source_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~clock_source_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the name of the CLOCK SOURCE. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL object_name();

        /**
         * Get the localized string reference pointing to the localized CLOCK SOURCE name.
         */
        uint16_t STDCALL localized_description();

        /**
         * The flags describing the capabilities or features of the CLOCK SOURCE.
         *
         * \return 1 (Stream ID) if the Input Stream CLOCK SOURCE is identified by the stream ID. \n
         *	       2 (Local ID) if the Input Stream CLOCK SOURCE is identified by it's local ID.
         */
        uint16_t STDCALL clock_source_flags();

        /**
         * The type of CLOCK SOURCE.
         *
         * \return 0 (Internal) if the clock is sourced from within the entity such as from a crystal oscillator. \n
         *	       1 (External) if the clock is sourced from an external connection on the entity via a Jack. \n
         *	       2 (Input Stream) if the clock is sourced from the media clock of an Input Stream.
         */
        uint16_t STDCALL clock_source_type();

        /**
         * Get the identifier of the CLOCK SOURCE.
         */
        uint64_t STDCALL clock_source_identifier();

        /**
         * Get the descriptor type of the object that this CLOCK SOURCE is associated with.
         */
        uint16_t STDCALL clock_source_location_type();

        /**
         * Get the descriptor index of the object that this CLOCK SOURCE is associated with.
         */
        uint16_t STDCALL clock_source_location_index();
    };
}

