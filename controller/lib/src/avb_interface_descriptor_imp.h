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
 * avb_interface_descriptor_imp.h
 *
 * AVB Interface descriptor implementation class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AVB_INTERFACE_DESCRIPTOR_IMP_H_
#define _AVDECC_CONTROLLER_LIB_AVB_INTERFACE_DESCRIPTOR_IMP_H_

#include "descriptor_base_imp.h"
#include "avb_interface_descriptor.h"

namespace avdecc_lib
{
    class avb_interface_descriptor_imp : public virtual avb_interface_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_avb_interface avb_interface_desc; // Structure containing the avb_interface_desc fields
        int desc_avb_interface_read_returned; // Status of extracting AVB Interface descriptor information from a network buffer

    public:
        /**
         * Constructor for AVB Interface descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        avb_interface_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, size_t pos, size_t frame_len);

        virtual ~avb_interface_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL get_descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL get_descriptor_index() const;

        /**
         * Get the name of the AVB Interface. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL get_object_name();

        /**
         * Get the localized string reference pointing to the localized AVB Interface name.
         */
        uint16_t STDCALL get_localized_description();

        /**
         * Get the MAC address of the interface.
         */
        uint8_t * STDCALL get_mac_addr();

        /**
         * Get the flags describing the features of the interface.
         *
         * \return 1 (GPTP Grandmaster Supported) if the interface supports the grandmaster functionality. \n
         *	   2 (GPTP Supported) if the interface supports the functionality. \n
         *	   4 (SRP Supported) if the interface supports the "Stream Reservation Protocol (SRP)" functionality.
         */
        uint16_t STDCALL get_interface_flags();

        /**
         * Get the clock identity of the interface.
         */
        uint64_t STDCALL get_clock_identity();

        /**
         * Get the priority1 field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
         */
        uint8_t STDCALL get_priority1();

        /**
         * Get the clock class field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
         */
        uint8_t STDCALL get_clock_class();

        /**
         * Get the offset scaled log variance field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
         */
        uint16_t STDCALL get_offset_scaled_log_variance();

        /**
         * Get the clock accuracy field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
         */
        uint8_t STDCALL get_clock_accuracy();

        /**
         * Get the priority2 field of the grandmaster functionality of the AVB Interface if supported, 0xff otherwise.
         */
        uint8_t STDCALL get_priority2();

        /**
         * Get the domain number field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
         */
        uint8_t STDCALL get_domain_number();

        /**
         * Get the current log sync interval field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
         */
        uint8_t STDCALL get_log_sync_interval();

        /**
         * Get the current log announce interval field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
         */
        uint8_t STDCALL get_log_announce_interval();

        /**
         * Get the current log pdelay interval field of the grandmaster functionality of the AVB Interface if supported, 0 otherwise.
         */
        uint8_t STDCALL get_log_pdelay_interval();

        /**
         * Get the port number field as used by the functionality of the AVB Interface if supported, 0 otherwise.
         */
        uint16_t STDCALL get_port_number();
    };
}

#endif
