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
 * audio_unit_descriptor.h
 *
 * Public AUDIO UNIT descriptor interface class
 * The AUDIO UNIT descriptor describes an AUDIO UNIT within the AVDECC Entity. An Audio Unit
 * represents a single audio clock domain.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"
#include "audio_unit_descriptor_response.h"
#include "audio_unit_get_sampling_rate_response.h"

namespace avdecc_lib
{
    class audio_unit_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return the avb_interface descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual audio_unit_descriptor_response * STDCALL get_audio_unit_response() = 0;
        
        /**
         * \return the avb_interface get_sampling_rate response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual audio_unit_get_sampling_rate_response * STDCALL get_audio_unit_get_sampling_rate_response() = 0;
        
        /**
         * Send a SET_SAMPLING_RATE command to change the sampling rate of a port or unit.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         * \param new_sampling_rate The sampling rate field is set to the new sampling rate.
         *
         * The new sampling rates can be retrieved by calling the following function after successfully
         * receiving a response back for the SET_SAMPLING_RATE command sent.
         *
         * \see set_sampling_rate_sampling_rate()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate) = 0;

        /**
         * Send a GET_SAMPLING_RATE command to get the current sampling rate of a port or unit.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * The sampling rates can be retrieved by calling the following function after successfully
         * receiving a response back for the GET_SAMPLING_RATE command sent.
         *
         * \see get_sampling_rate_sampling_rate()
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_sampling_rate_cmd(void *notification_id) = 0;
    };
}

