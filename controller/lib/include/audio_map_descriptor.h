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
 * audio_map_descriptor.h
 *
 * Public Audio Map descriptor interface class
 * The Audio Map descriptor describes a static mapping between an audio stream's channels
 * and an audio cluster's channels for streams and stream ports which are located in the
 * same clock domain. An AVDECC Entity which supports dynamic mappings, that is mappings
 * which can be changed at runtime by an AVDECC Controller, does not use Audio Map
 * descriptors, but instead provides their mappings via the GET_AUDIO_MAP command.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"

namespace avdecc_lib
{
    struct audio_map_mapping {
        uint16_t stream_index;
        uint16_t stream_channel;
        uint16_t cluster_offset;
        uint16_t cluster_channel;
    };

    class audio_map_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return The number of channel mappings within the Audio Map. The maximum value
         *	       of this field is 62 for this version of AEM.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t STDCALL number_of_mappings() = 0;
        /**
         * \param index The index of the mapping to return.
         * \param mapping The mapping structure that is filled in by this funtion.
         * \return Returns 0 on success.
        */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL mapping(size_t index, struct audio_map_mapping &mapping) = 0;
    };
}


