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
#include "avdecc-lib_build.h"
#include "descriptor_base.h"
#include "audio_cluster_descriptor_response.h"

namespace avdecc_lib
{
    class audio_cluster_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return the audio_cluster descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual audio_cluster_descriptor_response * STDCALL get_audio_cluster_response() = 0;
    };
}
