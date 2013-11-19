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
 * aecp.h
 *
 * AVDECC Enumeration and Control Protocol class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_AECP_H_
#define _AVDECC_CONTROLLER_LIB_AECP_H_

#include "jdksavdecc_aecp_aem.h"
#include "jdksavdecc_aem_command.h"

namespace avdecc_lib
{
        class end_station;

        class aecp
        {
        private:
                struct jdksavdecc_aecpdu_aem aecpdu; // Structure containing the AECPDU fields
                uint8_t *aecp_frame; // Point to a raw memory buffer to read from
                int aecpdu_aem_read_returned; // Status of extracting AECPDU information from a network buffer

        public:
                enum aem_fields_offsets
                {
                        CMD_POS = adp::ETHER_HDR_SIZE,
                        MSG_TYPE_POS = adp::ETHER_HDR_SIZE + 1,
                        TARGET_GUID_POS = adp::ETHER_HDR_SIZE + adp::PROTOCOL_HDR_SIZE,
                        SEQ_ID_POS = adp::ETHER_HDR_SIZE + JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + JDKSAVDECC_AECPDU_AEM_OFFSET_SEQUENCE_ID,
                        CMD_TYPE_POS = adp::ETHER_HDR_SIZE + JDKSAVDECC_COMMON_CONTROL_HEADER_LEN + JDKSAVDECC_AECPDU_AEM_OFFSET_COMMAND_TYPE,
                        READ_DESC_POS = adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN
                };

                /**
                 * Constructor for aecp used for constructing an object with a base pointer, position offest,
                 * and memory buffer length.
                 */
                aecp(uint8_t *frame, size_t pos, size_t frame_len);

                ~aecp();

                /**
                 * Initialize and fill Ethernet frame payload with Ethernet frame information for AEM commands.
                 */
                static int ether_frame_init(end_station *end_station, struct jdksavdecc_frame *ether_frame);

                /**
                 * Initialize and fill Ethernet frame payload with 1722 AECP Header information.
                 */
                static void common_hdr_init(struct jdksavdecc_frame *ether_frame, uint64_t target_guid);

                /**
                 * Get the header field of the AECP object.
                 */
                inline struct jdksavdecc_aecpdu_common get_aecpdu_header()
                {
                        return aecpdu.aecpdu_header;
                }

                /**
                 * Get the Controller GUID field of the AECP object.
                 */
                inline uint64_t get_controller_entity_id()
                {
                        return jdksavdecc_uint64_get(&aecpdu.controller_entity_id, 0);
                }

                /**
                 * Get the sequence id field of the AECP object.
                 */
                inline uint16_t get_sequence_id()
                {
                        return aecpdu.sequence_id;
                }

                /**
                 * Get the command type field of the AECP object.
                 */
                inline uint16_t get_command_type()
                {
                        return aecpdu.command_type;
                }
        };
}

#endif
