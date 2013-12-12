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
 * audio_cluster_descriptor.h
 *
 * Public Audio Cluster descriptor interface class
 * The Audio Cluster descriptor describes groups of audio channels in a stream.
 * An Audio Cluster could represent a stereo IEC 60958 encoded signal, a one or
 * more channel multibit linear audio signal, a MIDI signal or a SMPTE signal.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    class audio_cluster_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The descriptor type for the signal source of the cluster.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_type() = 0;

        /**
         * \return The descriptor index for the signal source of the cluster.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_index() = 0;

        /**
         * \return The index of the output of the signal source of the cluster. For a signal type of
         *	       Signal Splitter or Signal Demultiplexer, this is which output of the object it is
         *	       being source from, for a signal type of Matrix, this is the column the signal is
         *	       from and for any other signal type this is 0.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL signal_output() = 0;

        /**
         * \return The latency in nanoseconds between the timing reference plane and the opposite end
         *	       of the currently selected signal path. This does not include any latency added by a
         *	       delay control. The path latency is used to inform smart Controllers of the extra
         *	       latency to get the samples to the output, so that output across multiple entries
         *	       can be sample aligned.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL path_latency() = 0;

        /**
         * \return The block latency of the Audio Cluster. For an Aduio Cluster attached to a Stream Port Input,
         *	       this is the latency in nanoseconds between the reference plane and the output of the cluster. For
         *	       an Audio Cluster attached to a Stream Port Output, this is the latency in nanoseconds between the
         *	       output of the previous block's output and the reference plane. The previous block is the object
         *	       identified by the signal type and signal index fields.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint32_t STDCALL block_latency() = 0;

        /**
         * \return The number of channels within the cluster.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL channel_count() = 0;

        /**
         * The format for each channel of this cluster, all channels within the cluster have the same format.
         *
         * \return 0x00 (IEC 60958) for IEC 60958 encoded Audio Cluster. \n
         *	       0x40 (MBLA) for Multi-bit Linear Audio. \n
         *	       0x80 (MIDI) for MIDI data. \n
         *	       0x88 (SMPTE) for SMPTE data.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint8_t STDCALL format() = 0;
    };
}
