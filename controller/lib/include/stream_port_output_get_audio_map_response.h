/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2015 AudioScience Inc.
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
 * stream_port_output_get_audio_map_response.h
 *
 * STREAM PORT OUTPUT get audio mapping response base class
 */

#pragma once

#include <stdint.h>
#include "avdecc-lib_build.h"

namespace avdecc_lib
{
    struct stream_port_output_audio_mapping
    {
        uint16_t stream_index;
        uint16_t stream_channel;
        uint16_t cluster_offset;
        uint16_t cluster_channel;
    };

    class stream_port_output_get_audio_map_response
    {
    public:
        virtual ~stream_port_output_get_audio_map_response() {};
        /**
         * \return The GET_AUDIO_MAP response map_index.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t map_index() = 0;

        /**
         * \return The number of mappings contained in a GET_AUDIO_MAP response.
         */
        AVDECC_CONTROLLER_LIB32_API virtual uint16_t number_of_mappings() = 0;
        /**
         * \return The audio mapping by index after sending a GET_AUDIO_MAP command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL mapping(size_t index, struct stream_port_output_audio_mapping &map) = 0;
    };
}
