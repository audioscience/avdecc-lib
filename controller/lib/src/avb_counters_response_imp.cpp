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
 * avb_counters_response_imp.cpp
 *
 * AVB INTERFACE counters response implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "avb_counters_response_imp.h"

namespace avdecc_lib
{
avb_counters_response_imp::avb_counters_response_imp(const uint8_t *frame, size_t frame_len, ssize_t pos)
{
    m_position = pos;
    m_size = frame_len;
    m_frame = (uint8_t *)malloc(m_size * sizeof(uint8_t));
    memcpy(m_frame, frame, m_size);

    m_counters_valid = jdksavdecc_uint32_get(m_frame, ETHER_HDR_SIZE
                       + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_RESPONSE_OFFSET_COUNTERS_VALID);

    for(int i = 0; i<31; i++) {
        int r = jdksavdecc_uint32_read(&m_counters_block[i], frame, ETHER_HDR_SIZE
                                       + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_RESPONSE_OFFSET_COUNTERS_BLOCK + 4 * i,
                                       frame_len);
        if (r < 0)
            break;
    }
}

avb_counters_response_imp::~avb_counters_response_imp()
{
    free(m_frame);
}

uint32_t STDCALL avb_counters_response_imp::get_counter_valid(int name)
{
    switch(name)
    {
    case AVB_INTERFACE_LINK_UP:
        return m_counters_valid & 0x01;
    case AVB_INTERFACE_LINK_DOWN:
        return m_counters_valid >> 1 & 0x01;
    case AVB_INTERFACE_FRAMES_TX:
        return m_counters_valid >> 2 & 0x01;
    case AVB_INTERFACE_FRAMES_RX:
        return m_counters_valid >> 3 & 0x01;
    case AVB_INTERFACE_RX_CRC_ERROR:
        return m_counters_valid >> 4 & 0x01;
    case AVB_GPTP_GM_CHANGED:
        return m_counters_valid >> 5 & 0x01;
    default:
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "counter name not found\n");
    }
    return 0;
}

uint32_t STDCALL avb_counters_response_imp::get_counter_by_name(int name)
{
    switch(name)
    {
    case AVB_INTERFACE_LINK_UP:
        return m_counters_block[0];
    case AVB_INTERFACE_LINK_DOWN:
        return m_counters_block[1];
    case AVB_INTERFACE_FRAMES_TX:
        return m_counters_block[2];
    case AVB_INTERFACE_FRAMES_RX:
        return m_counters_block[3];
    case AVB_INTERFACE_RX_CRC_ERROR:
        return m_counters_block[4];
    case AVB_GPTP_GM_CHANGED:
        return m_counters_block[5];
    default:
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "counter name not found\n");
    }
    return 0;
}
}
