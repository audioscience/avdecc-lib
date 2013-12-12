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
 * stream_port_output_descriptor_imp.h
 *
 * Stream Port Output descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "stream_port_output_descriptor.h"

namespace avdecc_lib
{
    class stream_port_output_descriptor_imp : public stream_port_output_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_stream_port stream_port_output_desc; // Structure containing the stream_port_output_desc fields
        ssize_t stream_port_output_desc_read_returned; // Status of extracting Stream Port Output descriptor information from a network buffer

    public:
        /**
         * Constructor for Stream Port Output descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        stream_port_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~stream_port_output_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the descriptor index of the CLOCK DOMAIN descriptor describing the CLOCK DOMAIN for the port.
         */
        uint16_t STDCALL clock_domain_index();

        /**
         * The flags describing the capabilities or features of the port.
         *
         * \return 1 (Clock Sync Source) if the port can be used as a clock synchronization source. \n
         *	       2 (Async Sample Rate Conv) if the port has an asynchronous sample rate converter
         *	         to convert sample rates between another CLOCK DOMAIN and the Unit's. \n
         *	       3 (Sync Sample Rate Conv) if the port has a synchronous sample rate converter
         *	         to convert between sample rates in the same CLOCK DOMAIN.
         */
        uint16_t STDCALL port_flags();

        /**
         * Get the number of controls within the port.
         */
        uint16_t STDCALL number_of_controls();

        /**
         * Get the index of the first Control descriptor.
         */
        uint16_t STDCALL base_control();

        /**
         * Get the number of clusters within the port. This corresponds to the number of Audio Cluster,
         * Video Cluster, and Sensor Cluster descriptors which represent these clusters.
         */
        uint16_t STDCALL number_of_clusters();

        /**
         * Get the index of the first Audio Cluster, Video Cluster, or Sensor Cluster descriptor
         * describing the clusters within the port.
         */
        uint16_t STDCALL base_cluster();

        /**
         * Get the number of map descriptors used to define the mapping between the stream and the port.
         */
        uint16_t STDCALL number_of_maps();

        /**
         * Get the index of the first Audio Map, Video Map, or Sensor Map, descriptor which defines
         * the mappling between the stream and the port.
         */
        uint16_t STDCALL base_map();
    };
}

