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
 * audio_cluster_descriptor_imp.h
 *
 * Audio Cluster descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "audio_cluster_descriptor.h"

namespace avdecc_lib
{
    class audio_cluster_descriptor_imp : public audio_cluster_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_descriptor_audio_cluster audio_cluster_desc; // Structure containing the audio_cluster_desc fields
        ssize_t audio_cluster_desc_read_returned; // Status of extracting Audio Cluster descriptor information from a network buffer

    public:
        /**
         * Constructor for Audio Cluster descriptor object.
         *
         * \param end_station_obj A pointer to the base End Station object.
         * \param frame The raw memory that contains the descriptor information to read from.
         * \param pos The position offset to read the descriptor fields from.
         * \param frame_len The memory buffer length of the descriptor frame.
         */
        audio_cluster_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);

        virtual ~audio_cluster_descriptor_imp();

        /**
         * Get the type of the descriptor.
         */
        uint16_t STDCALL descriptor_type() const;

        /**
         * Get the index of the descriptor.
         */
        uint16_t STDCALL descriptor_index() const;

        /**
         * Get the name of the Audio Cluster. This may be user set through the use of a SET_NAME command.
         * The object name should be left blank (all zeros) by the manufacturer, with the manufacturer
         * defined value being provided in a localized form via the localized descripton field. By leaving
         * this field blank an AVDECC Controller can determine if the user has overridden the name and can
         * use this name rather than the localized name.
         */
        uint8_t * STDCALL object_name();

        /**
         * Get the localized string reference pointing to the localized Audio Cluster name.
         */
        uint16_t STDCALL localized_description();

        /**
         * Get the descriptor type for the signal source of the cluster.
         */
        uint16_t STDCALL signal_type();

        /**
         * Get the descriptor index for the signal source of the cluster.
         */
        uint16_t STDCALL signal_index();

        /**
         * Get the index of the output of the signal source of the cluster. For a signal type of
         * Signal Splitter or Signal Demultiplexer, this is which output of the object it is
         * being source from, for a signal type of Matrix, this is the column the signal is
         * from, and for any other signal type this is 0.
         */
        uint16_t STDCALL signal_output();

        /**
         * Get the latency in nanoseconds between the timing reference plane and the opposite end
         * of the currently selected signal path. This does not include any latency added by a
         * delay control. The path latency is used to inform smart Controllers of the extra
         * latency to get the samples to the output, so that output across multiple entries
         * can be sample aligned.
         */
        uint32_t STDCALL path_latency();

        /**
         * Get the block latency of the Audio Cluster. For an Aduio Cluster attached to a Stream Port Input,
         * this is the latency in nanoseconds between the reference plane and the output of the cluster. For
         * an Audio Cluster attached to a Stream Port Output, this is the latency in nanoseconds between the
         * output of the previous block's output and the reference plane. The previous block is the object
         * identified by the signal type and signal index fields.
         */
        uint32_t STDCALL block_latency();

        /**
         * Get the number of channels within the cluster.
         */
        uint16_t STDCALL channel_count();

        /**
         * The format for each channel of this cluster, all channels within the cluster have the same format.
         *
         * \return 0x00 (IEC 60958) for IEC 60958 encoded Audio Cluster. \n
         *	       0x40 (MBLA) for Multi-bit Linear Audio. \n
         *	       0x80 (MIDI) for MIDI data. \n
         *	       0x88 (SMPTE) for SMPTE data.
         */
        uint8_t STDCALL format();
    };
}

