/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2017 AudioScience Inc.
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
 * entity_counters_response_imp.cpp
 *
 * entity counters response implementation
 */

#include "enumeration.h"
#include "log_imp.h"
#include "entity_counters_response_imp.h"

namespace avdecc_lib
{
entity_counters_response_imp::entity_counters_response_imp(const uint8_t * frame, size_t frame_len, ssize_t pos)
{
    m_position = pos;
    m_size = frame_len;
    m_frame = (uint8_t *)malloc(m_size * sizeof(uint8_t));
    memcpy(m_frame, frame, m_size);
    
    m_counters_valid = jdksavdecc_uint32_get(m_frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_RESPONSE_OFFSET_COUNTERS_VALID);
    
    for (int i = 0; i < 32; i++)
    {
        int r = jdksavdecc_uint32_read(&m_counters_block[i], frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_RESPONSE_OFFSET_COUNTERS_BLOCK + 4 * i,
                                       frame_len);
        if (r < 0)
            break;
    }
}

entity_counters_response_imp::~entity_counters_response_imp()
{
    free(m_frame);
}

uint32_t STDCALL entity_counters_response_imp::get_counter_valid(int name)
{
    switch (name)
    {
    case ENTITY_SPECIFIC_8:
        return m_counters_valid >> 24 & 0x01;
    case ENTITY_SPECIFIC_7:
        return m_counters_valid >> 25 & 0x01;
    case ENTITY_SPECIFIC_6:
        return m_counters_valid >> 26 & 0x01;
    case ENTITY_SPECIFIC_5:
        return m_counters_valid >> 27 & 0x01;
    case ENTITY_SPECIFIC_4:
        return m_counters_valid >> 28 & 0x01;
    case ENTITY_SPECIFIC_3:
        return m_counters_valid >> 29 & 0x01;
    case ENTITY_SPECIFIC_2:
        return m_counters_valid >> 30 & 0x01;
    case ENTITY_SPECIFIC_1:
        return m_counters_valid >> 31 & 0x01;
    default:
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "counter name not found");
    }
    return 0;
}

uint32_t STDCALL entity_counters_response_imp::get_counter_by_name(int name)
{
    switch (name)
    {
    case ENTITY_SPECIFIC_8:
        return m_counters_block[24];
    case ENTITY_SPECIFIC_7:
        return m_counters_block[25];
    case ENTITY_SPECIFIC_6:
        return m_counters_block[26];
    case ENTITY_SPECIFIC_5:
        return m_counters_block[27];
    case ENTITY_SPECIFIC_4:
        return m_counters_block[28];
    case ENTITY_SPECIFIC_3:
        return m_counters_block[29];
    case ENTITY_SPECIFIC_2:
        return m_counters_block[30];
    case ENTITY_SPECIFIC_1:
        return m_counters_block[31];
    default:
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "counter name not found");
    }
    return 0;
}
}
