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
 * jack_output_descriptor_imp.h
 *
 * JACK OUTPUT descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "jack_output_descriptor.h"

namespace avdecc_lib
{
    class jack_output_descriptor_imp : public jack_output_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_jack jack_output_desc; // Structure containing the jack_output_desc fields
        ssize_t desc_jack_read_returned; // Status of extracting JACK INPUT Descriptor or JACK OUTPUT descriptor information from a network buffer

        struct jack_input_desc_jack_flags
        {
            bool clock_sync_source;
            bool captive;
        };

        struct jack_input_desc_jack_flags jack_output_flags;

    public:
        /**
         * Constructor for JACK OUTPUT descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        jack_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~jack_output_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the name of the JACK OUTPUT. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL object_name();

        /**
         * Get the localized string reference pointing to the localized JACK OUTPUT name.
         */
        uint16_t STDCALL localized_description();

        /**
         * Get the flags describing the capabilities or features of the Jack.
         */
        uint16_t STDCALL jack_flags();

        /**
         * Check if the jack can be used as a clock synchronization source.
         */
        uint16_t STDCALL jack_flag_clock_sync_source();

        /**
         * Check if the jack connection is hardwired, cannot be disconnected and
         * may be physically within the device's structure.
         */
        uint16_t STDCALL jack_flag_captive();

        /**
         * Get the type of the jack.
         */
        uint16_t STDCALL jack_type();

        /**
         * Get the number of controls within this jack.
         */
        uint16_t STDCALL number_of_controls();

        /**
         * Get the index of the first Control descriptor.
         */
        uint16_t STDCALL base_control();

    private:
        /**
         * Store the jack flags componenets of the JACK INPUT descriptor object in a vector.
         */
        void jack_flags_init();
    };
}

