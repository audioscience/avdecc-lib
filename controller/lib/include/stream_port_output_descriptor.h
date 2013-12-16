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
 * stream_port_output_descriptor.h
 *
 * Public Stream Port Output descriptor interface class
 * The Stream Port Output descriptor describes a STREAM OUTPUT Port of the Unit.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class stream_port_output_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The descriptor index of the CLOCK DOMAIN descriptor describing the CLOCK DOMAIN for the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL clock_domain_index() = 0;

        /**
         * The flags describing the capabilities or features of the port.
         *
         * \return 1 (Clock Sync Source) if the port can be used as a clock synchronization source. \n
         *	       2 (Async Sample Rate Conv) if the port has an asynchronous sample rate converter
         *	         to convert sample rates between another CLOCK DOMAIN and the Unit's. \n
         *	       3 (Sync Sample Rate Conv) if the port has a synchronous sample rate converter
         *	         to convert between sample rates in the same CLOCK DOMAIN.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL port_flags() = 0;

        /**
         * \return The number of controls within the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_controls() = 0;

        /**
         * \return The index of the first Control descriptor.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_control() = 0;

        /**
         * \return The number of clusters within the port. This corresponds to the number of Audio Cluster,
         *	       Video Cluster, and Sensor Cluster descriptors which represent these clusters.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_clusters() = 0;

        /**
         * \return The index of the first Audio Cluster, Video Cluster, or Sensor Cluster descriptor
         *	       describing the clusters within the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_cluster() = 0;

        /**
         * \return The number of map descriptors used to define the mapping between the stream and the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_maps() = 0;

        /**
         * \return The index of the first Audio Map, Video Map, or Sensor Map, descriptor which defines
         *	       the mappling between the stream and the port.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL base_map() = 0;
    };
}

