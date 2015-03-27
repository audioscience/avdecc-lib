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
#include "stream_port_output_descriptor_response_imp.h"
#include "stream_port_output_get_audio_map_response_imp.h"

namespace avdecc_lib
{
    class stream_port_output_descriptor_imp : public stream_port_output_descriptor, public virtual descriptor_base_imp
    {
    private:
        std::vector<struct audio_map_mapping> pending_maps; // Store maps in a vector
    public:
        stream_port_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~stream_port_output_descriptor_imp();

        stream_port_output_descriptor_response_imp *resp;
        stream_port_output_get_audio_map_response_imp *audio_map_resp;
        
        stream_port_output_descriptor_response * STDCALL get_stream_port_output_response();
        stream_port_output_get_audio_map_response * STDCALL get_stream_port_output_audio_map_response();
        
        int STDCALL send_get_audio_map_cmd(void *notification_id, uint16_t mapping_index);
        int proc_get_audio_map_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        
        int STDCALL send_add_audio_mappings_cmd(void *notification_id);
        int proc_add_audio_mappings_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        
        int STDCALL send_remove_audio_mappings_cmd(void *notification_id);
        int proc_remove_audio_mappings_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
        
        int store_pending_map(struct audio_map_mapping &map);
        size_t get_number_of_pending_maps();
        int view_pending_maps(size_t index, struct audio_map_mapping &map);
        int clear_pending_maps();
    };
}

