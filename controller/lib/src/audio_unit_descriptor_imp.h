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
 * audio_unit_descriptor_imp.h
 *
 * AUDIO UNIT descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "audio_unit_descriptor.h"
#include "audio_unit_descriptor_response_imp.h"
#include "audio_unit_get_sampling_rate_response_imp.h"

namespace avdecc_lib
{
    class audio_unit_descriptor_imp : public audio_unit_descriptor, public virtual descriptor_base_imp
    {
    private:
        struct jdksavdecc_aem_command_set_sampling_rate_response aem_cmd_set_sampling_rate_resp; // Store the response received after sending a SET_SAMPLING_RATE command.
    public:
        audio_unit_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~audio_unit_descriptor_imp();
        
        audio_unit_descriptor_response_imp *resp;
        audio_unit_get_sampling_rate_response_imp *sampling_rate_resp;
        
        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        audio_unit_descriptor_response * STDCALL get_audio_unit_response();
        audio_unit_get_sampling_rate_response * STDCALL get_audio_unit_get_sampling_rate_response();
        int STDCALL send_set_sampling_rate_cmd(void *notification_id, uint32_t new_sampling_rate);
        int proc_set_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_get_sampling_rate_cmd(void *notification_id);
        int proc_get_sampling_rate_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
    };
}
