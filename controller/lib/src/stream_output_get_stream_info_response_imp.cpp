/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2014 AudioScience Inc.
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
 * stream_output_get_stream_info_response_imp.cpp
 *
 * STREAM OUTPUT get stream info response implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "stream_output_get_stream_info_response_imp.h"
#include "util.h"

namespace avdecc_lib
{
stream_output_get_stream_info_response_imp::stream_output_get_stream_info_response_imp(uint8_t * frame, size_t frame_len, ssize_t pos)
{
    m_position = pos;
    m_size = frame_len;
    m_frame = (uint8_t *)malloc(m_size * sizeof(uint8_t));
    memcpy(m_frame, frame, m_size);
    stream_output_get_info_flags_init();
}

stream_output_get_stream_info_response_imp::~stream_output_get_stream_info_response_imp()
{
    free(m_frame);
}

void stream_output_get_stream_info_response_imp::stream_output_get_info_flags_init()
{
    stream_output_info_flags.class_b = get_stream_info_flags() & 0x01;
    stream_output_info_flags.fast_connect = get_stream_info_flags() >> 1 & 0x01;
    stream_output_info_flags.saved_state = get_stream_info_flags() >> 2 & 0x01;
    stream_output_info_flags.streaming_wait = get_stream_info_flags() >> 3 & 0x01;
    stream_output_info_flags.encrypted_pdu = get_stream_info_flags() >> 4 & 0x01;
    stream_output_info_flags.stream_vlan_id_valid = get_stream_info_flags() >> 25 & 0x01;
    stream_output_info_flags.connected = get_stream_info_flags() >> 26 & 0x01;
    stream_output_info_flags.msrp_failure_valid = get_stream_info_flags() >> 27 & 0x01;
    stream_output_info_flags.stream_dest_mac_valid = get_stream_info_flags() >> 28 & 0x01;
    stream_output_info_flags.msrp_acc_lat_valid = get_stream_info_flags() >> 29 & 0x01;
    stream_output_info_flags.stream_id_valid = get_stream_info_flags() >> 30 & 0x01;
    stream_output_info_flags.stream_format_valid = get_stream_info_flags() >> 31 & 0x01;
}

bool STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags_stream_vlan_id_valid()
{
    return stream_output_info_flags.stream_vlan_id_valid;
}

bool STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags_msrp_failure_valid()
{
    return stream_output_info_flags.msrp_failure_valid;
}

bool STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags_stream_dest_mac_valid()
{
    return stream_output_info_flags.stream_dest_mac_valid;
}

bool STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags_msrp_acc_lat_valid()
{
    return stream_output_info_flags.msrp_acc_lat_valid;
}

bool STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags_stream_id_valid()
{
    return stream_output_info_flags.stream_id_valid;
}

bool STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags_stream_format_valid()
{
    return stream_output_info_flags.stream_format_valid;
}

uint32_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_flags()
{
    return jdksavdecc_aem_command_get_stream_info_response_get_aem_stream_info_flags(m_frame, m_position);
}

uint64_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_stream_format()
{
    jdksavdecc_eui64 stream_format;
    stream_format = jdksavdecc_aem_command_get_stream_info_response_get_stream_format(m_frame, m_position);
    return jdksavdecc_eui64_convert_to_uint64(&stream_format);
}

uint64_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_stream_id()
{
    jdksavdecc_eui64 stream_id;
    stream_id = jdksavdecc_aem_command_get_stream_info_response_get_stream_id(m_frame, m_position);
    return jdksavdecc_eui64_convert_to_uint64(&stream_id);
}

uint32_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_msrp_accumulated_latency()
{
    return jdksavdecc_aem_command_get_stream_info_response_get_msrp_accumulated_latency(m_frame, m_position);
}

uint64_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_stream_dest_mac()
{
    jdksavdecc_eui48 dest_mac;
    dest_mac = jdksavdecc_aem_command_get_stream_info_response_get_stream_dest_mac(m_frame, m_position);
    return jdksavdecc_eui48_convert_to_uint64(&dest_mac);
}

uint8_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_msrp_failure_code()
{
    return jdksavdecc_aem_command_get_stream_info_response_get_msrp_failure_code(m_frame, m_position);
}

uint64_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_msrp_failure_bridge_id()
{
    jdksavdecc_eui64 msrp_failure_bridge_id;
    msrp_failure_bridge_id = jdksavdecc_aem_command_get_stream_info_response_get_msrp_failure_bridge_id(m_frame, m_position);
    return jdksavdecc_eui64_convert_to_uint64(&msrp_failure_bridge_id);
}

uint16_t STDCALL stream_output_get_stream_info_response_imp::get_stream_info_stream_vlan_id()
{
    return jdksavdecc_aem_command_get_stream_info_response_get_stream_vlan_id(m_frame, m_position);
}
}
