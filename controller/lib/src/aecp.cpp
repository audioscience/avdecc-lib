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
 * aecp.cpp
 *
 * AVDECC Enumeration and Control Protocol implementation
 */

#include "net_interface_imp.h"
#include "enumeration.h"
#include "log.h"
#include "util.h"
#include "adp.h"
#include "aecp.h"

namespace avdecc_lib
{
	aecp::aecp() {}

	aecp::aecp(uint8_t *frame, size_t pos, size_t mem_buf_len)
	{
		aecp_frame = (uint8_t *)malloc(mem_buf_len * sizeof(uint8_t));
		memcpy(aecp_frame, frame, mem_buf_len);
		aecpdu_aem_read_returned = jdksavdecc_aecpdu_aem_read(&aecpdu, aecp_frame, pos, mem_buf_len);

		if(aecpdu_aem_read_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "aecpdu_aem_read error");
			assert(aecpdu_aem_read_returned >= 0);
		}
	}

	aecp::~aecp()
	{
		free(aecp_frame);
	}

	int aecp::ether_frame_init(end_station *end_station, struct jdksavdecc_frame *ether_frame)
	{
		/*** Offset to write the field to ***/
		size_t ether_frame_pos = 0x0;
		jdksavdecc_frame_init(ether_frame);

		/***************************** Ethernet Frame ****************************/
		ether_frame->ethertype = JDKSAVDECC_AVTP_ETHERTYPE;
		convert_uint64_to_eui48(net_interface_ref->get_mac(), ether_frame->src_address.value);
		convert_uint64_to_eui48(end_station->get_end_station_mac(), ether_frame->dest_address.value);
		ether_frame->length = AECP_FRAME_LEN; // Length of AECP packet is 64 bytes

		/*********************** Fill frame payload with Ethernet frame information ********************/
		jdksavdecc_frame_write(ether_frame, ether_frame->payload, ether_frame_pos, adp::ETHER_HDR_SIZE);

		return 0;
	}

	void aecp::common_hdr_init(struct jdksavdecc_frame *ether_frame, uint64_t target_guid)
	{
		struct jdksavdecc_aecpdu_common_control_header aecpdu_common_ctrl_hdr;
		int aecpdu_common_ctrl_hdr_returned;
		size_t aecpdu_common_pos;

		/***** Offset to write the field to ****/
		aecpdu_common_pos = adp::ETHER_HDR_SIZE;

		/************************************** 1722 Protocol Header **************************************/
		aecpdu_common_ctrl_hdr.cd = 1;
		aecpdu_common_ctrl_hdr.subtype = JDKSAVDECC_SUBTYPE_AECP;
		aecpdu_common_ctrl_hdr.sv = 0;
		aecpdu_common_ctrl_hdr.version = 0;
		aecpdu_common_ctrl_hdr.message_type = JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND;
		aecpdu_common_ctrl_hdr.status = JDKSAVDECC_AEM_STATUS_SUCCESS;
		aecpdu_common_ctrl_hdr.control_data_length = 20;
		jdksavdecc_uint64_write(target_guid, &aecpdu_common_ctrl_hdr.target_entity_id, 0, sizeof(uint64_t));

		/*********************** Fill frame payload with AECP Common Control Header information **********************/
		aecpdu_common_ctrl_hdr_returned = jdksavdecc_aecpdu_common_control_header_write(&aecpdu_common_ctrl_hdr,
		                                                                                ether_frame->payload,
		                                                                                aecpdu_common_pos,
		                                                                                sizeof(ether_frame->payload));

		if(aecpdu_common_ctrl_hdr_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "adpdu_common_ctrl_hdr_write error");
			assert(aecpdu_common_ctrl_hdr_returned >= 0);
		}
	}

#ifdef DEBUG_DESCRIPTOR_FIELD_INFORMATION
	void aecp::print_aecpdu_information()
	{
		std::cout << "\nAECPDU";
		std::cout << "\naecpdu_header status = 0x" << std::hex << get_aecpdu_header().header.status;
		std::cout << "\naecpdu_header message_type = 0x" << std::hex << get_aecpdu_header().header.message_type;
		std::cout << "\naecpdu_header subtype = 0x" << std::hex << get_aecpdu_header().header.subtype;
		std::cout << "\naecpdu_header version = 0x" << std::hex << get_aecpdu_header().header.version;
		std::cout << "\naecpdu_header control_data_length = 0x" << std::hex << get_aecpdu_header().header.control_data_length;
		std::cout << "\ncontroller_entity_id = 0x" << std::hex << get_controller_entity_id();
		std::cout << "\nsequence_id = 0x" << std::hex << get_sequence_id();
		std::cout << "\ncommand_type = 0x" << std::hex << get_command_type();
	}
#endif

}