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
 * adp.cpp
 *
 * AVDECC Discovery Protocol implementation
 */

#include <cstdint>
#include <inttypes.h>

#include "net_interface_imp.h"
#include "enumeration.h"
#include "log_imp.h"
#include "util.h"
#include "adp.h"

namespace avdecc_lib
{
    adp::adp(const uint8_t *frame, size_t frame_len)
    {
        adp_frame = (uint8_t *)malloc(frame_len * sizeof(uint8_t));
        memcpy(adp_frame, frame, frame_len);

        assert(proc_adpdu(frame, frame_len) == 0);
    }

    adp::~adp()
    {
        free(adp_frame); // Free allocated memory for frame from the heap
    }

    int adp::proc_adpdu(const uint8_t *frame, size_t frame_len)
    {
        frame_read_returned = jdksavdecc_frame_read(&cmd_frame, frame, 0, frame_len);

        if(frame_read_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "frame_read error");
            return -1;
        }

        adpdu_read_returned = jdksavdecc_adpdu_read(&adpdu, frame, ETHER_HDR_SIZE, frame_len);

        if(adpdu_read_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "adpdu_read error");
            return -1;
        }

        return 0;
    }

    struct jdksavdecc_eui64 adp::get_controller_entity_id()
    {
        uint64_t mac_entity_id = ((net_interface_ref->mac_addr() & UINT64_C(0xFFFFFF000000)) << 16) |
                   UINT64_C(0x000000FFFF000000) |
                   (net_interface_ref->mac_addr() & UINT64_C(0xFFFFFF));
        struct jdksavdecc_eui64 entity_id;
        jdksavdecc_eui64_init_from_uint64(&entity_id, mac_entity_id);

        return entity_id;
    }
}
