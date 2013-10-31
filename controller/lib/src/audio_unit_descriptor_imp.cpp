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
 * audio_unit_descriptor_imp.cpp
 *
 * Audio Unit descriptor implementation
 */

#include <vector>
#include "enumeration.h"
#include "log.h"
#include "adp.h"
#include "aecp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "aem_controller_state_machine.h"
#include "audio_unit_descriptor_imp.h"

namespace avdecc_lib
{
	audio_unit_descriptor_imp::audio_unit_descriptor_imp() {}

	audio_unit_descriptor_imp::audio_unit_descriptor_imp(end_station_imp *base_end_station_imp_ref, uint8_t *frame, size_t pos, size_t mem_buf_len) : descriptor_base_imp(base_end_station_imp_ref)
	{
		desc_audio_read_returned = jdksavdecc_descriptor_audio_read(&audio_unit_desc, frame, pos, mem_buf_len);

		if(desc_audio_read_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "desc_audio_read error");
			assert(desc_audio_read_returned >= 0);
		}
	}

	audio_unit_descriptor_imp::~audio_unit_descriptor_imp() {}

	uint16_t STDCALL audio_unit_descriptor_imp::get_descriptor_type()
	{
		assert(audio_unit_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT);
		return audio_unit_desc.descriptor_type;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_descriptor_index()
	{
		return audio_unit_desc.descriptor_index;
	}

	uint8_t * STDCALL audio_unit_descriptor_imp::get_object_name()
	{
		return audio_unit_desc.object_name.value;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_localized_description()
	{
		return audio_unit_desc.localized_description;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_clock_domain_index()
	{
		return audio_unit_desc.clock_domain_index;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_stream_input_ports()
	{
		return audio_unit_desc.number_of_stream_input_ports;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_stream_input_port()
	{
		return audio_unit_desc.base_stream_input_port;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_stream_output_ports()
	{
		return audio_unit_desc.number_of_stream_output_ports;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_stream_output_port()
	{
		return audio_unit_desc.base_stream_output_port;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_external_input_ports()
	{
		return audio_unit_desc.number_of_external_input_ports;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_external_input_port()
	{
		return audio_unit_desc.base_external_input_port;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_external_output_ports()
	{
		return audio_unit_desc.number_of_external_output_ports;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_external_output_port()
	{
		return audio_unit_desc.base_external_output_port;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_internal_input_ports()
	{
		return audio_unit_desc.number_of_internal_input_ports;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_internal_input_port()
	{
		return audio_unit_desc.base_internal_input_port;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_internal_output_ports()
	{
		return audio_unit_desc.number_of_internal_output_ports;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_internal_output_port()
	{
		return audio_unit_desc.base_internal_output_port;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_controls()
	{
		return audio_unit_desc.number_of_controls;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_control()
	{
		return audio_unit_desc.base_control;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_signal_selectors()
	{
		return audio_unit_desc.number_of_signal_selectors;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_signal_selector()
	{
		return audio_unit_desc.base_signal_selector;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_mixers()
	{
		return audio_unit_desc.number_of_mixers;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_mixer()
	{
		return audio_unit_desc.base_mixer;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_matrices()
	{
		return audio_unit_desc.number_of_matrices;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_matrix()
	{
		return audio_unit_desc.base_matrix;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_splitters()
	{
		return audio_unit_desc.number_of_splitters;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_splitter()
	{
		return audio_unit_desc.base_splitter;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_combiners()
	{
		return audio_unit_desc.number_of_combiners;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_combiner()
	{
		return audio_unit_desc.base_combiner;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_demultiplexers()
	{
		return audio_unit_desc.number_of_demultiplexers;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_demultiplexer()
	{
		return audio_unit_desc.base_demultiplexer;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_multiplexers()
	{
		return audio_unit_desc.number_of_multiplexers;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_multiplexer()
	{
		return audio_unit_desc.base_multiplexer;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_transcoders()
	{
		return audio_unit_desc.number_of_transcoders;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_transcoder()
	{
		return audio_unit_desc.base_transcoder;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_number_of_control_blocks()
	{
		return audio_unit_desc.number_of_control_blocks;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_base_control_block()
	{
		return audio_unit_desc.base_control_block;
	}

	uint32_t STDCALL audio_unit_descriptor_imp::get_current_sampling_rate()
	{
		uint16_t offset = 0x0;

		for(uint32_t index_i = 0; index_i < get_sampling_rates_count(); index_i++)
		{
			sample_rates.pull_field = get_pull_field_multiplier(audio_unit_desc.current_sampling_rate >> 29);
			sample_rates.base_freq = (audio_unit_desc.current_sampling_rate << 3) >> 3;
			sample_rates.freq = sample_rates.pull_field * sample_rates.base_freq;
			offset += 0x4;
		}

		return audio_unit_desc.current_sampling_rate;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_sampling_rates_offset()
	{
		return audio_unit_desc.sampling_rates_offset;
	}

	uint16_t STDCALL audio_unit_descriptor_imp::get_sampling_rates_count()
	{
		return audio_unit_desc.sampling_rates_count;
	}

	uint8_t STDCALL audio_unit_descriptor_imp::get_pull_field_multiplier(uint8_t pull_field_value)
	{
		switch(pull_field_value)
		{
			case 0:
				return (uint8_t)1.0;
				break;

			case 1:
				return (uint8_t)(1/1.001);
				break;

			case 2:
				return (uint8_t)1.001;
				break;

			case 3:
				return (uint8_t)(24/25);
				break;

			case 4:
				return (uint8_t)(25/24);
				break;

			default:
				avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "pull_field_value is not found.");
				break;
		}

		return 0;
	}

	uint32_t STDCALL audio_unit_descriptor_imp::get_sampling_rate_sampling_rates()
	{
		return aem_cmd_get_sampling_rate_resp.sampling_rate;

	}

	int STDCALL audio_unit_descriptor_imp::send_get_sampling_rate_cmd(void *notification_id, uint16_t desc_index)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_get_sampling_rate aem_cmd_get_sampling_rate;
		int aem_cmd_get_sampling_rate_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/***************************************** AECP Common Data ********************************************/
		aem_cmd_get_sampling_rate.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_guid();
		// Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
		aem_cmd_get_sampling_rate.sequence_id = aem_controller_state_machine::aecp_seq_id;
		aem_cmd_get_sampling_rate.command_type = JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE;

		/************************** AECP Message Specific Data ************************/
		aem_cmd_get_sampling_rate.descriptor_type = JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT;
		aem_cmd_get_sampling_rate.descriptor_index = desc_index;

		/******************************** Fill frame payload with AECP data and send the frame ***************************/
		aecp::ether_frame_init(base_end_station_imp_ref, ether_frame);
		aem_cmd_get_sampling_rate_returned = jdksavdecc_aem_command_get_sampling_rate_write(&aem_cmd_get_sampling_rate,
		                                                                                    ether_frame->payload,
		                                                                                    aecp::CMD_POS,
		                                                                                    sizeof(ether_frame->payload));

		if(aem_cmd_get_sampling_rate_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_write error\n");
			assert(aem_cmd_get_sampling_rate_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, base_end_station_imp_ref->get_end_station_guid());
		system_queue_tx(notification_id, avdecc_lib::CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}


	int audio_unit_descriptor_imp::proc_get_sampling_rate_resp(void *&notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		int aem_cmd_get_sampling_rate_resp_returned;
		uint32_t msg_type;
		bool u_field;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, mem_buf_len);

		aem_cmd_get_sampling_rate_resp_returned = jdksavdecc_aem_command_get_sampling_rate_response_read(&aem_cmd_get_sampling_rate_resp,
		                                                                                                 frame,
		                                                                                                 aecp::CMD_POS,
		                                                                                                 mem_buf_len);

		if(aem_cmd_get_sampling_rate_resp_returned < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "aem_cmd_get_sampling_rate_resp_read error\n");
			assert(aem_cmd_get_sampling_rate_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_get_sampling_rate_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_get_sampling_rate_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_get_sampling_rate_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, notification_flag, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}


#ifdef DEBUG_DESCRIPTOR_FIELD_INFORMATION
	void audio_unit_descriptor_imp::print_audio_unit_desc_info()
	{
		std::cout << "\nAudio Unit Descriptor";
		std::cout << "\ndescriptor_type = 0x" << std::hex << get_descriptor_type();
		std::cout << "\ndescriptor_index = 0x" << std::hex << get_descriptor_index();
		std::cout << "\nobject_name = " << std::hex << get_object_name().value;
		std::cout << "\nlocalized_description = 0x" << std::hex << get_localized_description();
		std::cout << "\nclock_domain_index = " << std::dec << get_clock_domain_index();
		std::cout << "\nnumber_of_stream_input_ports = " << std::dec << get_number_of_stream_input_ports();
		std::cout << "\nbase_stream_input_port = " << std::dec << get_base_stream_input_port();
		std::cout << "\nnumber_of_stream_output_ports = " << std::dec << get_number_of_stream_output_ports();
		std::cout << "\nbase_stream_output_port = " << std::dec << get_base_stream_output_port();
		std::cout << "\nnumber_of_external_input_ports = " << std::dec << get_number_of_external_input_ports();
		std::cout << "\nbase_external_input_port = " << std::dec << get_base_external_input_port();
		std::cout << "\nnumber_of_external_output_ports = " << std::dec << get_number_of_external_output_ports();
		std::cout << "\nbase_external_output_port = " << std::dec << get_base_external_output_port();
		std::cout << "\nnumber_of_internal_input_ports = " << std::dec << get_number_of_internal_input_ports();
		std::cout << "\nbase_internal_input_port = " << std::dec << get_base_internal_input_port();
		std::cout << "\nnumber_of_internal_output_ports = " << std::dec << get_number_of_internal_output_ports();
		std::cout << "\nbase_internal_output_port = " << std::dec << get_base_internal_output_port();
		std::cout << "\nnumber_of_controls = " << std::dec << get_number_of_controls();
		std::cout << "\nbase_control = " << std::dec << get_base_control();
		std::cout << "\nnumber_of_signal_selectors = " << std::dec << get_number_of_signal_selectors();
		std::cout << "\nbase_signal_selector = " << std::dec << get_base_signal_selector();
		std::cout << "\nnumber_of_mixers = " << std::dec << get_number_of_mixers();
		std::cout << "\nbase_mixer = " << std::dec << get_base_mixer();
		std::cout << "\nnumber_of_matrices = " << std::dec << get_number_of_matrices();
		std::cout << "\nbase_matrix = " << std::dec << get_base_matrix();
		std::cout << "\nnumber_of_splitters = " << std::dec << get_number_of_splitters();
		std::cout << "\nbase_splitter = " << std::dec << get_base_splitter();
		std::cout << "\nnumber_of_combiners = " << std::dec << get_number_of_combiners();
		std::cout << "\nbase_combiner = " << std::dec << get_base_combiner();
		std::cout << "\nnumber_of_demultiplexers = " << std::dec << get_number_of_demultiplexers();
		std::cout << "\nbase_demultiplexer = " << std::dec << get_base_demultiplexer();
		std::cout << "\nnumber_of_multiplexers = " << std::dec << get_number_of_multiplexers();
		std::cout << "\nbase_multiplexer = " << std::dec << get_base_multiplexer();
		std::cout << "\nnumber_of_transcoders = " << std::dec << get_number_of_transcoders();
		std::cout << "\nbase_transcoder = " << std::dec << get_base_transcoder();
		std::cout << "\nnumber_of_control_blocks = " << std::dec << get_number_of_control_blocks();
		std::cout << "\nbase_control_block = " << std::dec << get_base_control_block();
		std::cout << "\ncurrent_sampling_rate = " << std::dec << get_current_sampling_rate();
		std::cout << "\nsampling_rates_offset = " << std::dec << get_sampling_rates_offset();
		std::cout << "\nsampling_rates_count = " << std::dec << get_sampling_rates_count();
	}
#endif

}