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
 * memory_object_descriptor_imp.h
 *
 * MEMORY_OBJECT descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "memory_object_descriptor.h"
#include "memory_object_descriptor_response_imp.h"

namespace avdecc_lib
{
    class memory_object_descriptor_imp : public memory_object_descriptor, public virtual descriptor_base_imp
    {
    public:
        memory_object_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~memory_object_descriptor_imp();
        
        memory_object_descriptor_response_imp *resp;

        memory_object_descriptor_response * STDCALL get_memory_object_response();

        int STDCALL start_operation_cmd(void *notification_id, uint16_t operation_type);
        int proc_start_operation_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status, uint16_t &operation_id, uint16_t &operation_type);
        int proc_operation_status_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status, uint16_t &operation_id, bool &is_operation_id_valid);
    };
}
