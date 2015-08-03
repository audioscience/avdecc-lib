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
#include "avdecc-lib_build.h"
#include "descriptor_base.h"
#include "stream_port_output_descriptor_response.h"
#include "stream_port_output_get_audio_map_response.h"

namespace avdecc_lib
{
    class stream_port_output_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return the stream port output descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_port_output_descriptor_response * STDCALL get_stream_port_output_response() = 0;

        /**
         * \return the audio_map get_audio_map response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual stream_port_output_get_audio_map_response * STDCALL get_stream_port_output_audio_map_response() = 0;
        
        /**
         * \param map The added audio_map pending for adding/removal.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int store_pending_map(struct audio_map_mapping &map) = 0;
        
        /**
         *  Get pending audio mappings.
         *
         * \param index The index of the queued audio mapping.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int get_pending_maps(size_t index, struct audio_map_mapping &map) = 0;
        
        /**
         *  Clear pending audio mappings.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int clear_pending_maps() = 0;
        
        /**
         * \return the number of pending audio mappings.
         */
        AVDECC_CONTROLLER_LIB32_API virtual size_t get_number_of_pending_maps() = 0;

        /**
         * Send a GET_AUDIO_MAP command to fetch the dynamic mapping between the Audio Clusters and
         * the input or output streams.
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_get_audio_map_cmd(void *notification_id, uint16_t mapping_index) = 0;
        
        /**
         * Send an ADD_AUDIO_MAPPINGS command to add mapping entries to the dynamic mappings between the Audio
         * Clusters and the Input or Output Streams.
         *
         * The mappings to be added are stored in a local queue.
         * \see store_pending_map().
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * \return 0 if the mappings to add fit one cmd frame. \n
         *	       1 if there were more pending mappings than one frame could fit. \see AEM_MAX_MAPS
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_add_audio_mappings_cmd(void *notification_id) = 0;
        
        /**
         * Send a REMOVE_AUDIO_MAPPINGS command to remove mapping entries from the dynamic mappings
         * between the Audio Clusters and the input or output Streams.
         *
         * The mappings to be removed are stored in a local queue.
         * \see store_pending_map().
         *
         * \param notification_id A void pointer to the unique identifier associated with the command.
         *
         * \return 0 if the mappings to remove fit one cmd frame. \n
         *	       1 if there were more pending mappings than one frame could fit. \see AEM_MAX_MAPS
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL send_remove_audio_mappings_cmd(void *notification_id) = 0;
    };
}
