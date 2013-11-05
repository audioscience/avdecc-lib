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
 * end_station_imp.cpp
 *
 * End Station implementation
 */

#include <vector>
#include "enumeration.h"
#include "notification.h"
#include "log.h"
#include "util_imp.h"
#include "adp.h"
#include "aecp.h"
#include "aem_controller_state_machine.h"
#include "system_tx_queue.h"
#include "end_station_imp.h"

namespace avdecc_lib
{
	end_station_imp::end_station_imp() {}

	end_station_imp::end_station_imp(uint8_t *frame, size_t mem_buf_len)
	{
		end_station_connection_status = ' ';
		current_entity_desc = 0;
		current_config_desc = 0;
		desc_type_from_config = 0;
		desc_type_index_from_config = 0;
		desc_count_from_config = 0;
		desc_count_index_from_config = 0;
		read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_IDLE;
		adp_ref = new adp(frame, mem_buf_len);
		end_station_guid = adp_ref->get_entity_entity_id();
		utility->convert_eui48_to_uint64(adp_ref->get_src_addr().value, end_station_mac);
		end_station_init();
	}

	end_station_imp::~end_station_imp()
	{
		delete adp_ref;

		for(uint32_t entity_vec_index = 0; entity_vec_index < entity_desc_vec.size(); entity_vec_index++)
		{
			delete entity_desc_vec.at(entity_vec_index);
		}
	}

	int end_station_imp::end_station_init()
	{
		uint16_t desc_type = JDKSAVDECC_DESCRIPTOR_ENTITY;
		uint16_t desc_index = 0x0;

		read_desc_init(desc_type, desc_index);

		return 0;
	}

	char STDCALL end_station_imp::get_connection_status()
	{
		return end_station_connection_status;
	}

	void end_station_imp::set_connected()
	{
		end_station_connection_status = 'C';
	}

	void end_station_imp::set_disconnected()
	{
		end_station_connection_status = 'D';
	}

	uint64_t STDCALL end_station_imp::get_end_station_guid()
	{
		return end_station_guid;
	}

	uint64_t STDCALL end_station_imp::get_end_station_mac()
	{
		return end_station_mac;
	}

	adp * end_station_imp::get_adp()
	{
		return adp_ref;
	}

	uint32_t STDCALL end_station_imp::get_entity_desc_count()
	{
		return entity_desc_vec.size();
	}

	entity_descriptor * STDCALL end_station_imp::get_entity_desc_by_index(uint32_t entity_desc_index)
	{
		return entity_desc_vec.at(entity_desc_index);
	}

	int end_station_imp::read_desc_init(uint16_t desc_type, uint16_t desc_index)
	{
		return send_read_desc_cmd_with_flag(NULL, CMD_WITHOUT_NOTIFICATION, desc_type, desc_index);
	}

	int STDCALL end_station_imp::send_read_desc_cmd(void *notification_id, uint16_t desc_type, uint16_t desc_index)
	{
		return send_read_desc_cmd_with_flag(notification_id, CMD_WITH_NOTIFICATION, desc_type, desc_index);
	}

	int end_station_imp::send_read_desc_cmd_with_flag(void *notification_id, uint32_t notification_flag, uint16_t desc_type, uint16_t desc_index)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_read_descriptor aem_command_read_desc;
		int aem_command_read_desc_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/***************************** AECP Common Data ****************************/
		aem_command_read_desc.controller_entity_id = adp_ref->get_controller_guid();
		// Fill aem_command_read_desc.sequence_id in AEM Controller State Machine
		aem_command_read_desc.command_type = JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR;

		/******************************************************** AECP Message Specific Data ********************************************************/
		aem_command_read_desc.configuration_index = (desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY || desc_type == JDKSAVDECC_DESCRIPTOR_CONFIGURATION) ?
		                                            0 : entity_desc_vec.at(current_entity_desc)->get_current_configuration();
		aem_command_read_desc.descriptor_type = desc_type;
		aem_command_read_desc.descriptor_index = desc_index;

		/******************************* Fill frame payload with AECP data and send the frame ****************************/
		aecp::ether_frame_init(this, ether_frame);
		aem_command_read_desc_returned = jdksavdecc_aem_command_read_descriptor_write(&aem_command_read_desc,
		                                                                              ether_frame->payload,
		                                                                              aecp::CMD_POS,
		                                                                              sizeof(ether_frame->payload));

		if(aem_command_read_desc_returned < 0)
		{
			log_ref->logging(LOGGING_LEVEL_ERROR, "aem_cmd_read_desc_write error");
			assert(aem_command_read_desc_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, end_station_guid);
		system_queue_tx(notification_id, notification_flag, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int end_station_imp::proc_read_desc_resp(void *&notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_read_descriptor_response aem_cmd_read_desc_resp;
		int aem_cmd_read_desc_resp_returned;
		uint32_t msg_type;
		bool u_field;
		uint16_t desc_type;
		uint16_t desc_index;
		configuration_descriptor_imp *config_desc_imp_ref = NULL;

		if(entity_desc_vec.size() >= 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
		{
			config_desc_imp_ref = dynamic_cast<configuration_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc));

			if(!config_desc_imp_ref)
			{
				log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base configuration_descriptor to derived configuration_descriptor_imp error");
			}
		}

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, mem_buf_len);
		aem_cmd_read_desc_resp_returned = jdksavdecc_aem_command_read_descriptor_response_read(&aem_cmd_read_desc_resp,
		                                                                                       frame,
		                                                                                       adp::ETHER_HDR_SIZE,
		                                                                                       mem_buf_len);

		if(aem_cmd_read_desc_resp_returned < 0)
		{
			log_ref->logging(LOGGING_LEVEL_ERROR, "aem_cmd_read_desc_res_read error");
			assert(aem_cmd_read_desc_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_read_desc_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_read_desc_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_read_desc_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
		desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_OFFSET_DESCRIPTOR);
		desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_OFFSET_DESCRIPTOR + 2);

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, notification_flag, msg_type, u_field, ether_frame);

		/******************* Select the corresponding descriptor based on the descriptor type from the response packet received ******************/
		switch(desc_type)
		{
			case JDKSAVDECC_DESCRIPTOR_ENTITY:
				if(entity_desc_vec.size() == 0)
				{
					entity_desc_vec.push_back(new entity_descriptor_imp(this, frame, aecp::READ_DESC_POS, mem_buf_len));
					current_config_desc = entity_desc_vec.at(entity_desc_vec.size() - 1)->get_current_configuration();
					uint16_t desc_type = JDKSAVDECC_DESCRIPTOR_CONFIGURATION;
					uint16_t desc_index = 0x0;
					read_desc_init(desc_type, desc_index);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() == 0)
				{
					entity_desc_vec.at(current_entity_desc)->store_config_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
					read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_STARTING;
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_audio_unit_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_STREAM_INPUT:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_stream_input_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_stream_output_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_JACK_INPUT:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_jack_input_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_JACK_OUTPUT:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_jack_output_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_avb_interface_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_clock_source_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_LOCALE:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_locale_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_STRINGS:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_strings_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			case JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN:
				if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->get_config_desc_count() >= 1)
				{
					config_desc_imp_ref->store_clock_domain_desc(this, frame, aecp::READ_DESC_POS, mem_buf_len);
				}

				break;

			default:
				log_ref->logging(LOGGING_LEVEL_DEBUG, "The descriptor is not implemented.");
				break;
		}

		/*
		 * A state machine that iterates through the top level descriptors present in the Configuration descriptor.
		 */
		switch(read_top_level_desc_in_config_state)
		{
			case READ_TOP_LEVEL_DESC_IN_CONFIG_IDLE:
				break;

			case READ_TOP_LEVEL_DESC_IN_CONFIG_STARTING:
				{
					uint16_t desc_count = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_desc_count_from_config_by_index(desc_type_index_from_config);
					desc_type_from_config = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_desc_type_from_config_by_index(desc_type_index_from_config);

					if(desc_count_index_from_config < desc_count)
					{
						read_desc_init(desc_type_from_config, desc_count_index_from_config);
						read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_RUNNING;
					}
				}

				break;

			case READ_TOP_LEVEL_DESC_IN_CONFIG_RUNNING:
				{
					uint16_t desc_count = entity_desc_vec.at(entity_desc_vec.size() - 1)->get_config_desc_by_index(current_config_desc)->get_desc_count_from_config_by_index(desc_type_index_from_config);
					desc_count_index_from_config++;

					if(desc_count_index_from_config >= desc_count)
					{
						uint16_t total_num_of_desc = entity_desc_vec.at(entity_desc_vec.size() - 1)->get_config_desc_by_index(current_config_desc)->get_descriptor_counts_count();
						desc_type_index_from_config++;
						desc_count_index_from_config = 0;

						if(desc_type_index_from_config >= total_num_of_desc)
						{
							read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_DONE;
						}
						else
						{
							desc_count = entity_desc_vec.at(entity_desc_vec.size() - 1)->get_config_desc_by_index(current_config_desc)->get_desc_count_from_config_by_index(desc_type_index_from_config);

							if(desc_count_index_from_config < desc_count)
							{
								desc_type_from_config = entity_desc_vec.at(entity_desc_vec.size() - 1)->get_config_desc_by_index(current_config_desc)->get_desc_type_from_config_by_index(desc_type_index_from_config);
								read_desc_init(desc_type_from_config, desc_count_index_from_config);
							}
						}
					}
					else
					{
						read_desc_init(desc_type_from_config, desc_count_index_from_config);
					}
				}

				break;

			case READ_TOP_LEVEL_DESC_IN_CONFIG_DONE:
				break;
		}

		free(ether_frame);
		return 0;
	}

	int STDCALL end_station_imp::send_entity_avail_cmd(void *notification_id)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_entity_available aem_cmd_entity_avail;
		int aem_cmd_entity_avail_returned;
		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

		/**************************** AECP Common Data ****************************/
		aem_cmd_entity_avail.controller_entity_id = adp_ref->get_controller_guid();
		// Fill aem_cmd_entity_avail.sequence_id in AEM Controller State Machine
		aem_cmd_entity_avail.command_type = JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE;

		/**************************** Fill frame payload with AECP data and send the frame *************************/
		aecp::ether_frame_init(this, ether_frame);
		aem_cmd_entity_avail_returned = jdksavdecc_aem_command_entity_available_write(&aem_cmd_entity_avail,
											      ether_frame->payload,
											      aecp::CMD_POS,
											      sizeof(ether_frame->payload));

		if(aem_cmd_entity_avail_returned < 0)
		{
			log_ref->logging(LOGGING_LEVEL_ERROR, "aem_cmd_entity_avail_write error\n");
			assert(aem_cmd_entity_avail_returned >= 0);
			return -1;
		}

		aecp::common_hdr_init(ether_frame, end_station_guid);
		system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, ether_frame->payload, ether_frame->length);

		free(ether_frame);
		return 0;
	}

	int end_station_imp::proc_entity_avail_resp(void *&notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len, int &status)
	{
		struct jdksavdecc_frame *ether_frame;
		struct jdksavdecc_aem_command_entity_available_response aem_cmd_entity_avail_resp;
		int aem_cmd_entity_avail_resp_returned = 0;
		uint32_t msg_type = 0;
		bool u_field = false;

		ether_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
		memcpy(ether_frame->payload, frame, mem_buf_len);

		aem_cmd_entity_avail_resp_returned = jdksavdecc_aem_command_entity_available_response_read(&aem_cmd_entity_avail_resp,
													   frame,
													   aecp::CMD_POS,
													    mem_buf_len);

		if(aem_cmd_entity_avail_resp_returned < 0)
		{
			log_ref->logging(LOGGING_LEVEL_ERROR, "aem_cmd_entity_avail_resp_read error\n");
			assert(aem_cmd_entity_avail_resp_returned >= 0);
			return -1;
		}

		msg_type = aem_cmd_entity_avail_resp.aem_header.aecpdu_header.header.message_type;
		status = aem_cmd_entity_avail_resp.aem_header.aecpdu_header.header.status;
		u_field = aem_cmd_entity_avail_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

		aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, notification_flag, msg_type, u_field, ether_frame);

		free(ether_frame);
		return 0;
	}

	int end_station_imp::proc_rcvd_resp(void *&notification_id, uint32_t &notification_flag, uint8_t *frame, uint16_t mem_buf_len, int &status)
	{
		uint16_t cmd_type;
		uint16_t desc_type;
		uint16_t desc_index;
		cmd_type = jdksavdecc_uint16_get(frame, aecp::CMD_TYPE_POS);

		switch(cmd_type)
		{
			case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY)
					{
						entity_descriptor_imp *entity_desc_imp_ref;
						entity_desc_imp_ref = dynamic_cast<entity_descriptor_imp *>(entity_desc_vec.at(current_entity_desc));

						if(entity_desc_imp_ref)
						{
							entity_desc_imp_ref->proc_acquire_entity_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
						}
					}
					else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
					{
						stream_input_descriptor_imp *stream_input_desc_imp_ref;
						stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

						if(stream_input_desc_imp_ref)
						{
							stream_input_desc_imp_ref->proc_acquire_entity_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
						}
					}
					else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
					{
						stream_output_descriptor_imp *stream_output_desc_imp_ref;
						stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

						if(stream_output_desc_imp_ref)
						{
							stream_output_desc_imp_ref->proc_acquire_entity_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
						}
					}
				}

				break;

			case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:

				log_ref->logging(LOGGING_LEVEL_ERROR, "Need to implement LOCK_ENTITY command.");

				break;

			case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
				proc_entity_avail_resp(notification_id, notification_flag, frame, mem_buf_len, status);
				break;

			case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
				proc_read_desc_resp(notification_id, notification_flag, frame, mem_buf_len, status);
				break;

			case JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
					{
						stream_input_descriptor_imp *stream_input_desc_imp_ref;
						stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

						if(stream_input_desc_imp_ref)
						{
							stream_input_desc_imp_ref->proc_set_stream_format_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
						}
					}
					else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
					{
						stream_output_descriptor_imp *stream_output_desc_imp_ref;
						stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

						if(stream_output_desc_imp_ref)
						{
							stream_output_desc_imp_ref->proc_set_stream_format_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
						}
					}
				}

				break;

			case JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
					{
						stream_input_descriptor_imp *stream_input_desc_imp_ref;
						stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

						if(stream_input_desc_imp_ref)
						{
							stream_input_desc_imp_ref->proc_get_stream_format_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
						}
					}
					else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
					{
						stream_output_descriptor_imp *stream_output_desc_imp_ref;
						stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

						if(stream_output_desc_imp_ref)
						{
							stream_output_desc_imp_ref->proc_get_stream_format_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
						}
					}
				}

				break;

			case JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO:

				log_ref->logging(LOGGING_LEVEL_ERROR, "Need to implement SET_STREAM_INFO command.");

				break;

			case JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO:
				desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
				desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

				if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
				{
					stream_input_descriptor_imp *stream_input_desc_imp_ref;
					stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

					if(stream_input_desc_imp_ref)
					{
						stream_input_desc_imp_ref->proc_get_stream_info_resp(notification_id, notification_flag, frame, mem_buf_len, status);
					}
					else
					{
						log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
					}
				}
				else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
				{
					stream_output_descriptor_imp *stream_output_desc_imp_ref;
					stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

					if(stream_output_desc_imp_ref)
					{
						stream_output_desc_imp_ref->proc_get_stream_info_resp(notification_id, notification_flag, frame, mem_buf_len, status);
					}
					else
					{
						log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_output_descriptor_imp to base stream_output_descriptor error");
					}
				}

				break;

			case JDKSAVDECC_AEM_COMMAND_SET_NAME:

				log_ref->logging(LOGGING_LEVEL_ERROR, "Need to implement SET_NAME command.");

				break;

			case JDKSAVDECC_AEM_COMMAND_GET_NAME:

				log_ref->logging(LOGGING_LEVEL_ERROR, "Need to implement GET_NAME command.");

				break;

			case JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT)
					{
						audio_unit_descriptor_imp *audio_unit_desc_imp_ref;
						audio_unit_desc_imp_ref = dynamic_cast<audio_unit_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_audio_unit_desc_by_index(desc_index));

						if(audio_unit_desc_imp_ref)
						{
							audio_unit_desc_imp_ref->proc_set_sampling_rate_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base audio_unit_descriptor to derived audio_unit_descriptor_imp error");
						}

					}
				}
				break;

			case JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT)
					{
						audio_unit_descriptor_imp *audio_unit_desc_imp_ref;
						audio_unit_desc_imp_ref = dynamic_cast<audio_unit_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_audio_unit_desc_by_index(desc_index));

						if(audio_unit_desc_imp_ref)
						{
							audio_unit_desc_imp_ref->proc_get_sampling_rate_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base audio_unit_descriptor to derived audio_unit_descriptor_imp error");
						}

					}
				}
				break;

			case JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					clock_domain_descriptor_imp *clock_domain_desc_imp_ref;
					clock_domain_desc_imp_ref = dynamic_cast<clock_domain_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_clock_domain_desc_by_index(desc_index));

					if(clock_domain_desc_imp_ref)
					{
						clock_domain_desc_imp_ref->proc_set_clock_source_resp(notification_id, notification_flag, frame, mem_buf_len, status);
					}
					else
					{
						log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base clock_domain_descriptor to derived clock_domain_descriptor_imp error");
					}
				}
				break;

			case JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					clock_domain_descriptor_imp *clock_domain_desc_imp_ref;
					clock_domain_desc_imp_ref = dynamic_cast<clock_domain_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_clock_domain_desc_by_index(desc_index));

					if(clock_domain_desc_imp_ref)
					{
						clock_domain_desc_imp_ref->proc_get_clock_source_resp(notification_id, notification_flag, frame, mem_buf_len, status);
					}
					else
					{
						log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from base clock_domain_descriptor to derived clock_domain_descriptor_imp error");
					}
				}
				break;

			case JDKSAVDECC_AEM_COMMAND_START_STREAMING:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_START_STREAMING_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_START_STREAMING_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
					{
						stream_input_descriptor_imp *stream_input_desc_imp_ref;
						stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

						if(stream_input_desc_imp_ref)
						{
							stream_input_desc_imp_ref->proc_start_streaming_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
						}
					}
					else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
					{
						stream_output_descriptor_imp *stream_output_desc_imp_ref;
						stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

						if(stream_output_desc_imp_ref)
						{
							stream_output_desc_imp_ref->proc_start_streaming_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_output_descriptor_imp to base stream_output_descriptor error");
						}
					}
				}
				break;

			case JDKSAVDECC_AEM_COMMAND_STOP_STREAMING:
				{
					desc_type = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_STOP_STREAMING_RESPONSE_OFFSET_DESCRIPTOR_TYPE);
					desc_index = jdksavdecc_uint16_get(frame, adp::ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_STOP_STREAMING_RESPONSE_OFFSET_DESCRIPTOR_INDEX);

					if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
					{
						stream_input_descriptor_imp *stream_input_desc_imp_ref;
						stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

						if(stream_input_desc_imp_ref)
						{
							stream_input_desc_imp_ref->proc_stop_streaming_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
						}
					}
					else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
					{
						stream_output_descriptor_imp *stream_output_desc_imp_ref;
						stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

						if(stream_output_desc_imp_ref)
						{
							stream_output_desc_imp_ref->proc_stop_streaming_resp(notification_id, notification_flag, frame, mem_buf_len, status);
						}
						else
						{
							log_ref->logging(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_output_descriptor_imp to base stream_output_descriptor error");
						}
					}
				}
				break;

			default:
				notification_ref->notifying(NO_MATCH_FOUND, 0, cmd_type, 0, 0, 0);
				break;
		}

		return 0;
	}
}
