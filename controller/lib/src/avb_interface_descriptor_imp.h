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
 * avb_interface_descriptor_imp.h
 *
 * AVB INTERFACE descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "avb_interface_descriptor_response_imp.h"
#include "avb_interface_descriptor.h"
#include "avb_counters_response_imp.h"
#include "avb_interface_get_avb_info_response_imp.h"

namespace avdecc_lib
{
class avb_interface_descriptor_imp : public avb_interface_descriptor, public virtual descriptor_base_imp
{
public:
    avb_interface_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
    virtual ~avb_interface_descriptor_imp();

    avb_interface_descriptor_response_imp *resp;
    avb_counters_response_imp *counters_resp;
    avb_interface_get_avb_info_response_imp *get_avb_info_resp;

    avb_interface_descriptor_response * STDCALL get_avb_interface_response();
    avb_counters_response * STDCALL get_avb_interface_counters_response();
    avb_interface_get_avb_info_response * STDCALL get_avb_interface_get_avb_info_response();
    int STDCALL send_get_counters_cmd(void *notification_id);
    int proc_get_counters_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
    int STDCALL send_get_avb_info_cmd(void *notification_id);
    int proc_get_avb_info_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);
};
}
