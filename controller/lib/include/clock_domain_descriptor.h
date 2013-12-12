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
 * Public CLOCK DOMAIN descriptor interface class
 * The CLOCK DOMAIN descriptor describes a source of a common clock signal within an
 * AVDECC Entity. This could be the output from a PLL, which can be locked to a
 * number of sources or a clock signal generator. The CLOCK DOMAIN allows for the
 * selection of the CLOCK SOURCE of the domain and determines what the valid
 * sources are for the domain.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class clock_domain_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The descriptor index of the CLOCK SOURCE descriptor describing the current CLOCK SOURCE
         *	       for the CLOCK DOMAIN.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL clock_source_index() = 0;

        /**
         * \return The number of CLOCK SOURCE indexes in the clock sources field. The maximum value for this field
         *	       is 249 for this version of AEM.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL clock_sources_count() = 0;

        /**
         * \return The corresponding Clock Sources by index present in the CLOCK DOMAIN.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_by_index(size_t clk_src_index) = 0;

        /**
         * \return The CLOCK SOURCE index of the requested CLOCK DOMAIN after sending a
         *	       SET_CLOCK_SOURCE command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL set_clock_source_clock_source_index() = 0;

        /**
         * \return The CLOCK SOURCE index of the requested CLOCK DOMAIN after sending a
         *	       GET_CLOCK_SOURCE command and receiving a response back for the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL get_clock_source_clock_source_index() = 0;

        /**
         * Send a SET_CLOCK_SOURCE command to change the CLOCK SOURCE of a CLOCK DOMAIN.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_clk_src_index The CLOCK SOURCE index field is set to the new CLOCK SOURCE index.
         *
         * The new CLOCK SOURCE index can be retrieved by calling the following function after successfully
         * receiving a response back for the SET_CLOCK_SOURCE command sent.
         *
         * \see set_clock_source_clock_source_index()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_clock_source_cmd(void *notification_id, uint16_t new_clk_src_index) = 0;

        /**
         * Send a GET_CLOCK_SOURCE command to get the current CLOCK SOURCE of a CLOCK DOMAIN.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * The CLOCK SOURCE index can be retrieved by calling the following function after successfully
         * receiving a response back for the GET_CLOCK_SOURCE command sent.
         *
         * \see get_clock_source_clock_source_index()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_clock_source_cmd(void *notification_id) = 0;
    };
}

