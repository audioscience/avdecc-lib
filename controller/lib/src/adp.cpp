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

#include "net_interface_imp.h"
#include "enumeration.h"
#include "log_imp.h"
#include "util_imp.h"
#include "adp.h"

namespace avdecc_lib
{
        adp::adp(const uint8_t *frame, size_t frame_len)
        {
                adp_frame = (uint8_t *)malloc(frame_len * sizeof(uint8_t));
                memcpy(adp_frame, frame, frame_len);

                frame_read_returned = jdksavdecc_frame_read(&ether_frame, adp_frame, 0x0, frame_len);

                if(frame_read_returned < 0)
                {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "frame_read error");
                        assert(frame_read_returned >= 0);
                }

                adpdu_read_returned = jdksavdecc_adpdu_read(&adpdu, adp_frame, ETHER_HDR_SIZE, frame_len);

                if(adpdu_read_returned < 0)
                {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "adpdu_read error");
                        assert(adpdu_read_returned >= 0);
                }
        }

        adp::~adp()
        {
                free(adp_frame); // Free allocated memory for frame from the heap
        }

        struct jdksavdecc_eui64 adp::get_controller_guid()
        {
                uint64_t mac_guid;
                mac_guid = ((net_interface_ref->get_mac() & UINT64_C(0xFFFFFF000000)) << 16) |
                           UINT64_C(0x000000FFFF000000) |
                           (net_interface_ref->get_mac() & UINT64_C(0xFFFFFF));

                return jdksavdecc_eui64_get(&mac_guid, 0);
        }

        int adp::ether_frame_init(struct jdksavdecc_frame *ether_frame)
        {
                /*** Offset to write the field to ***/
                size_t ether_frame_pos = 0x0;
                jdksavdecc_frame_init(ether_frame);

                /*************************************** Ethernet Frame ***************************************/
                ether_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
                utility->convert_uint64_to_eui48(net_interface_ref->get_mac(), ether_frame->src_address.value);
                //		ether_frame->src_address = src_mac_addr;
                //		ether_frame->dest_address = get_dest_addr();
                ether_frame->length = ADP_FRAME_LEN; // Length of ADP packet is 82 bytes

                /********************* Fill frame payload with Ethernet frame information *****************/
                jdksavdecc_frame_write(ether_frame, ether_frame->payload, ether_frame_pos, ETHER_HDR_SIZE);

                return 0;
        }

        void adp::common_hdr_init(struct jdksavdecc_frame *ether_frame, uint64_t target_guid)
        {
                struct jdksavdecc_adpdu_common_control_header adpdu_common_ctrl_hdr;
                int adpdu_common_ctrl_hdr_returned;

                /********************************** 1722 Protocol Header ***********************************/
                adpdu_common_ctrl_hdr.cd = 1;
                adpdu_common_ctrl_hdr.subtype = JDKSAVDECC_SUBTYPE_ADP;
                adpdu_common_ctrl_hdr.sv = 0;
                adpdu_common_ctrl_hdr.version = 0;
                adpdu_common_ctrl_hdr.message_type = 2;
                adpdu_common_ctrl_hdr.valid_time = 0;
                adpdu_common_ctrl_hdr.control_data_length = 56;
                jdksavdecc_uint64_write(target_guid, &adpdu_common_ctrl_hdr.entity_id, 0, sizeof(uint64_t));

                /********************* Fill frame payload with AECP Common Control Header information **********************/
                adpdu_common_ctrl_hdr_returned = jdksavdecc_adpdu_common_control_header_write(&adpdu_common_ctrl_hdr,
                                                                                              ether_frame->payload,
                                                                                              ETHER_HDR_SIZE,
                                                                                              sizeof(ether_frame->payload));

                if(adpdu_common_ctrl_hdr_returned < 0)
                {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "adpdu_common_ctrl_hdr_write error");
                        assert(adpdu_common_ctrl_hdr_returned >= 0);
                }
        }
}
