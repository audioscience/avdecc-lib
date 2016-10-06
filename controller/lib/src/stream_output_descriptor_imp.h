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
 * stream_output_descriptor_imp.h
 *
 * STREAM OUTPUT descriptor implementation class
 */

#pragma once

#include <iostream>
#include <map>
#include <string>
using namespace std;

#include "jdksavdecc_acmp.h"
#include "descriptor_base_imp.h"
#include "stream_output_descriptor.h"
#include "stream_output_descriptor_response_imp.h"
#include "stream_output_get_stream_format_response_imp.h"
#include "stream_output_get_stream_info_response_imp.h"
#include "stream_output_get_tx_state_response_imp.h"
#include "stream_output_get_tx_connection_response_imp.h"

namespace avdecc_lib
{
class stream_output_descriptor_imp : public stream_output_descriptor, public virtual descriptor_base_imp
{
private:
    struct jdksavdecc_aem_command_set_stream_info_response aem_cmd_set_stream_info_resp; // Store the response received after sending a SET_STREAM_INFO command.
public:
    stream_output_descriptor_imp(end_station_imp * end_station_obj, const uint8_t * frame, ssize_t pos, size_t frame_len);
    virtual ~stream_output_descriptor_imp();

    stream_output_descriptor_response_imp * resp;
    stream_output_get_stream_format_response_imp * get_format_resp;
    stream_output_get_stream_info_response_imp * get_info_resp;
    stream_output_get_tx_state_response_imp * get_tx_state_resp;
    stream_output_get_tx_connection_response_imp * get_tx_connection_resp;

    stream_output_descriptor_response * STDCALL get_stream_output_response();
    stream_output_get_stream_format_response * STDCALL get_stream_output_get_stream_format_response();
    stream_output_get_stream_info_response * STDCALL get_stream_output_get_stream_info_response();
    stream_output_get_tx_state_response * STDCALL get_stream_output_get_tx_state_response();
    stream_output_get_tx_connection_response * STDCALL get_stream_output_get_tx_connection_response();

    int STDCALL send_set_stream_format_cmd(void * notification_id, uint64_t new_stream_format);
    int proc_set_stream_format_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_get_stream_format_cmd(void * notification_id);
    int proc_get_stream_format_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_set_stream_info_vlan_id_cmd(void * notification_id, uint16_t vlan_id);
    int STDCALL send_set_stream_info_msrp_accumulated_latency_cmd(void * notification_id, uint32_t msrp_accumulated_latency);
    int proc_set_stream_info_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_get_stream_info_cmd(void * notification_id);
    int proc_get_stream_info_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_start_streaming_cmd(void * notification_id);
    int proc_start_streaming_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_stop_streaming_cmd(void * notification_id);
    int proc_stop_streaming_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_get_tx_state_cmd(void * notification_id);
    int proc_get_tx_state_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int STDCALL send_get_tx_connection_cmd(void * notification_id, uint64_t listener_entity_id, uint16_t listener_unique_id);
    int proc_get_tx_connection_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);
};
}
