/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2020 AudioScience Inc.
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
 * control_descriptor_get_control_response_imp.cpp
 *
 * CONTROL get_control response implementation
 */


#include "enumeration.h"
#include "log_imp.h"
#include "control_descriptor_get_jdks_ipv4_control_response_imp.h"
#include "util.h"
#include "jdksavdecc_jdks.h"

namespace avdecc_lib
{
control_descriptor_get_jdks_ipv4_control_response_imp::control_descriptor_get_jdks_ipv4_control_response_imp(uint8_t * frame, size_t frame_len, ssize_t pos)
{
    m_position = pos;
    m_size = frame_len;
    m_frame = (uint8_t *)malloc(m_size * sizeof(uint8_t));
    memcpy(m_frame, frame, m_size);
}

control_descriptor_get_jdks_ipv4_control_response_imp::~control_descriptor_get_jdks_ipv4_control_response_imp()
{
    free(m_frame);
}

uint32_t STDCALL control_descriptor_get_jdks_ipv4_control_response_imp::get_ipv4_address()
{
    return jdksavdecc_aem_command_get_jdks_ipv4_control_response_get_ipv4_address(m_frame, m_position);
}

uint32_t STDCALL control_descriptor_get_jdks_ipv4_control_response_imp::get_ipv4_netmask()
{
    return jdksavdecc_aem_command_get_jdks_ipv4_control_response_get_ipv4_netmask(m_frame, m_position);
}

bool STDCALL control_descriptor_get_jdks_ipv4_control_response_imp::get_ipv4_flags_static_enabled()
{
    uint32_t ipv4_flags = jdksavdecc_aem_command_get_jdks_ipv4_control_response_get_flags(m_frame, m_position);
    
    return ipv4_flags & (1 << JDKSAVDECC_JDKS_IPV4_CONTROL_FLAGS_STATIC_ENABLE_BIT);
}

bool STDCALL control_descriptor_get_jdks_ipv4_control_response_imp::get_ipv4_flags_dhcp_enabled()
{
    uint32_t ipv4_flags = jdksavdecc_aem_command_get_jdks_ipv4_control_response_get_flags(m_frame, m_position);

    return ipv4_flags & (1 << JDKSAVDECC_JDKS_IPV4_CONTROL_FLAGS_DHCP_ENABLE_BIT);
}
}
