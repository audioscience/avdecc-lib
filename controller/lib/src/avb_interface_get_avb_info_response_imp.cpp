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
 * avb_interface_get_avb_info_response_imp.cpp
 *
 * AVB INTERFACE get avb info response implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "avb_interface_get_avb_info_response_imp.h"
#include "util.h"

namespace avdecc_lib
{
avb_interface_get_avb_info_response_imp::avb_interface_get_avb_info_response_imp(uint8_t *frame, size_t frame_len, ssize_t pos)
{
    m_position = pos;
    m_size = frame_len;
    m_frame = (uint8_t *)malloc(m_size * sizeof(uint8_t));
    memcpy(m_frame, frame, m_size);
    avb_interface_get_info_flags_init();
}

avb_interface_get_avb_info_response_imp::~avb_interface_get_avb_info_response_imp()
{
    free(m_frame);
}

void avb_interface_get_avb_info_response_imp::avb_interface_get_info_flags_init()
{
    avb_interface_info_flags.as_capable= get_avb_info_flags() & 0x01;
    avb_interface_info_flags.gptp_enabled = get_avb_info_flags() >> 1 & 0x01;
    avb_interface_info_flags.srp_enabled = get_avb_info_flags() >> 2 & 0x01;
}

bool STDCALL avb_interface_get_avb_info_response_imp::get_avb_info_flags_as_capable()
{
    return avb_interface_info_flags.as_capable;
}

bool STDCALL avb_interface_get_avb_info_response_imp::get_avb_info_flags_gptp_enabled()
{
    return avb_interface_info_flags.gptp_enabled;
}

bool STDCALL avb_interface_get_avb_info_response_imp::get_avb_info_flags_srp_enabled()
{
    return avb_interface_info_flags.srp_enabled;
}

uint8_t STDCALL avb_interface_get_avb_info_response_imp::get_avb_info_flags()
{
    return jdksavdecc_aem_command_get_avb_info_response_get_flags(m_frame, m_position);
}

uint64_t STDCALL avb_interface_get_avb_info_response_imp::get_gptp_grandmaster_id()
{
    jdksavdecc_eui64 gptp_grandmaster_id;
    gptp_grandmaster_id = jdksavdecc_aem_command_get_avb_info_response_get_gptp_grandmaster_id(m_frame, m_position);
    return jdksavdecc_eui64_convert_to_uint64(&gptp_grandmaster_id);
}

uint32_t STDCALL avb_interface_get_avb_info_response_imp::get_propagation_delay()
{
    return jdksavdecc_aem_command_get_avb_info_response_get_propagation_delay(m_frame, m_position);
}

uint8_t STDCALL avb_interface_get_avb_info_response_imp::get_gptp_domain_number()
{
    return jdksavdecc_aem_command_get_avb_info_response_get_gptp_domain_number(m_frame, m_position);
}

uint16_t STDCALL avb_interface_get_avb_info_response_imp::get_msrp_mappings_count()
{
    return jdksavdecc_aem_command_get_avb_info_response_get_msrp_mappings_count(m_frame, m_position);
}
}
