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
 * avdecc_cmd_line.cpp
 *
 * AVDECC command line processing implementation
 */

#include <iostream>
#include <iomanip>
#include <string>
#include "end_station.h"
#include "entity_descriptor.h"
#include "configuration_descriptor.h"
#include "audio_unit_descriptor.h"
#include "stream_input_descriptor.h"
#include "stream_output_descriptor.h"
#include "jack_input_descriptor.h"
#include "jack_output_descriptor.h"
#include "avb_interface_descriptor.h"
#include "clock_source_descriptor.h"
#include "locale_descriptor.h"
#include "strings_descriptor.h"
#include "clock_domain_descriptor.h"
#include "avdecc_controller_cmd_line.h"

uint32_t avdecc_cmd_line::current_end_station = 0;
uint16_t avdecc_cmd_line::current_entity = 0;
uint16_t avdecc_cmd_line::current_config = 0;
uint32_t avdecc_cmd_line::notification_id = 0;
std::string avdecc_cmd_line::log_path = "C:\\..\\..\\avdecc-lib\\controller\\app\\bin\\avdecc_controller_cmd_line_main_debug\\";

avdecc_cmd_line::avdecc_cmd_line() {}

avdecc_cmd_line::avdecc_cmd_line(void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, void *),
                                 void (*log_callback) (void *, int32_t, const char *, int32_t))
{
	netif_ref = avdecc_lib::create_net_interface();
	controller_ref = avdecc_lib::create_controller(netif_ref, notification_callback, log_callback);
	system_ref = avdecc_lib::create_system(avdecc_lib::system::LAYER2_MULTITHREADED_CALLBACK, netif_ref, controller_ref);
	utility = avdecc_lib::create_util();

	std::cout << "avdecc_controller version: " << controller_ref->get_version() << std::endl;
	std::cout << "(c) AudioScience, Inc. 2013\n"<< std::endl;
	print_interfaces_and_select();
	is_setting_valid(current_end_station, current_entity, current_config);
	system_ref->process_start();
}

avdecc_cmd_line::~avdecc_cmd_line()
{
	system_ref->process_close();
	system_ref->destroy();
	controller_ref->destroy();
	utility->destroy();
}

int avdecc_cmd_line::print_interfaces_and_select()
{
	int interface_num;
	char *dev_desc;
	dev_desc = (char *)malloc(256);

	for(uint32_t i = 0; i < netif_ref->devs_count(); i++)
	{
		dev_desc = netif_ref->get_dev_desc_by_index(i);
		printf("%d (%s)\n", i, dev_desc);
	}

	std::cout << "Enter the interface number (1-" << std::dec << netif_ref->devs_count() << "): ";
	std::cin >> interface_num;

	netif_ref->select_interface_by_num(interface_num);

	return 0;
}

int avdecc_cmd_line::cmd_help()
{
	std::cout << "Command" << std::setw(28) << "" << "Description" << std::endl;
	std::cout << "------------------------------------------------------------------------------" << std::endl;
	std::cout << "help" << std::setw(31) << "" << "Display a list of valid commands." << std::endl;
	std::cout << "\nversion" << std::setw(28) << "" << "Display the current AVDECC Controller\n" <<
	          std::setw(35) << "" << "build release version." << std::endl;
	std::cout << "\nlist" << std::setw(31) << "" << "Display a table with information about each\n" <<
	          std::setw(35) << "" << "end station." << std::endl;
	std::cout << "\nlist clock_sync_source" << std::setw(13) << "" << "Display a list of descriptors that has the\n" <<
	          std::setw(35) << "" << "Clock Sync Source flag set." << std::endl;
	std::cout << "\nselect" << std::setw(29) << "" << "Display the current end station, entity,\n" <<
	          std::setw(35) << "" << "and configuration setting." << std::endl;
	std::cout << "\nselect [es] [et] [cf]" << std::setw(14) << "" << "Change the setting of end station, entity,\n" <<
	          std::setw(35) << "" << "and configuration, where es stands for\n" <<
	          std::setw(35) << "" << "end station, et stands for entity, and cf\n" <<
	          std::setw(35) << "" << "stands for configuration and should be\n" <<
	          std::setw(35) << "" << "integers.\n" <<
	          std::setw(35) << "" << "To see a list of valid end stations, enter\n" <<
	          std::setw(35) << "" << "\"list\" command." << std::endl;
	std::cout << "\nlog level [nll]" << std::setw(20) << "" << "Update the base log level for messages to be\n" <<
	          std::setw(35) << "" << "logged by the logging callback." << std::endl;
	std::cout << "\nview all" << std::setw(27) << "" << "Display all the descriptors present in each\n" <<
	          std::setw(35) << "" << "end station." << std::endl;
	std::cout << "\nview descriptor [dt] [di]" << std::setw(10) << "" << "Display information for the specified\n" <<
	          std::setw(35) << "" << "descriptor using the current setting, where\n" <<
	          std::setw(35) << "" << "dt stands for descriptor type and should be\n" <<
	          std::setw(35) << "" << "a string, and di stands for descriptor index\n" <<
	          std::setw(35) << "" << "and should be an integer.\n" <<
	          std::setw(35) << "" << "To see a list of valid descriptor types and\n" <<
	          std::setw(35) << "" << "corresponding indexes, enter \"view all\"\n" <<
	          std::setw(35) << "" << "command." << std::endl;
	std::cout << "\nacquire entity [aef] [dt] [di]" << std::setw(5) << "" << "Send a ACQUIRE_ENTITY command to obtain\n" <<
	          std::setw(35) << "" << "exclusive access to an entire Entity or a\n" <<
	          std::setw(35) << "" << "sub-tree of objects using the current\n" <<
	          std::setw(35) << "" << "setting, where aef stands for Acquire Entity\n" <<
	          std::setw(35) << "" << "Flags and should be a string, dt stands for\n" <<
	          std::setw(35) << "" << "descriptor type and should be a string, and\n" <<
	          std::setw(35) << "" << "di stands for descriptor index and should\n" <<
	          std::setw(35) << "" << "be an integer.\n" <<
	          std::setw(35) << "" << "Valid Acquire Entity Flags are 0, persistent\n" <<
	          std::setw(35) << "" << ", and release.\n" <<
	          std::setw(35) << "" << "To see a list of valid descriptor types and\n" <<
	          std::setw(35) << "" << "corresponding indexes, enter \"view all\"\n" <<
	          std::setw(35) << "" << "command." << std::endl;
	std::cout << "\nlock entity [lef] [dt] [di]" << std::setw(8) << "" << "Send a LOCK_ENTITY command to provide short\n" <<
	          std::setw(35) << "" << "term exclusive access to the AVDECC Entity\n" <<
	          std::setw(35) << "" << "to perform atomic operations using the\n" <<
	          std::setw(35) << "" << "current setting, where lef stands for Lock\n" <<
	          std::setw(35) << "" << "Entity Flags and should be a string, dt\n" <<
	          std::setw(35) << "" << "stands for descriptor type and should be a\n" <<
	          std::setw(35) << "" << "string, and di stands for descriptor index\n" <<
	          std::setw(35) << "" << "and should be an integer.\n" <<
	          std::setw(35) << "" << "Valid Lock Entity Flags are 0 and unlock.\n" <<
	          std::setw(35) << "" << "To see a list of valid descriptor types and\n" <<
	          std::setw(35) << "" << "corresponding indexes, enter \"view all\"\n" <<
	          std::setw(35) << "" << "command." << std::endl;
	std::cout << "\nentity available" << std::setw(19) << "" << "Send a ENTITY_AVAILABLE command to determine\n" <<
	          std::setw(35) << "" << "if another AVDECC Entity is still alive and\n" <<
	          std::setw(35) << "" << "responding to commands." << std::endl;
	std::cout << "\ncontroller available" << std::setw(15) << "" << "Send a CONTROLLER_AVAILABLE command to\n" <<
	          std::setw(35) << "" << "determine if an AVDECC Controller is still\n" <<
		  std::setw(35) << "" << "alive." << std::endl;
	std::cout << "\nread descriptor [dt] [di]" << std::setw(10) << "" << "Send a READ_DESCRIPTOR command to get the\n" <<
	          std::setw(35) << "" << "localized strings from an AVDECC Entity\n" <<
	          std::setw(35) << "" << "using the current setting, where dt stands\n" <<
	          std::setw(35) << "" << "for descriptor type and should be a string,\n" <<
	          std::setw(35) << "" << "and di stands for descriptor index and\n" <<
	          std::setw(35) << "" << "should be an integer." << std::endl;
	std::cout << "\nset stream_format [dt] [di] [sf]" << std::setw(3) << "" << "Send a SET_STREAM_FORMAT command to change\n" <<
	          std::setw(35) << "" << "the format of a stream using the current\n" <<
	          std::setw(35) << "" << "setting, where dt stands for descriptor type\n" <<
	          std::setw(35) << "" << "and should be a string, di stands for\n" <<
	          std::setw(35) << "" << "descriptor index and should be an integer,\n" <<
	          std::setw(35) << "" << "sf stands for stream format and should be\n" <<
		  std::setw(35) << "" << " an integer.\n" <<
	          std::setw(35) << "" << "Valid descriptor types are STREAM_INPUT and\n" <<
	          std::setw(35) << "" << "STREAM_OUTPUT.\n" << std::endl;
	std::cout << "\nget stream_format [dt] [di]" << std::setw(8) << "" << "Send a GET_STREAM_FORMAT command to display\n" <<
	          std::setw(35) << "" << "the current format of a stream using\n" <<
	          std::setw(35) << "" << "the current setting, where dt stands for\n" <<
	          std::setw(35) << "" << "descriptor type and should be a string, and\n" <<
	          std::setw(35) << "" << "di stands for descriptor index and should be\n" <<
	          std::setw(35) << "" << "an integer.\n" <<
	          std::setw(35) << "" << "Valid descriptor types are STREAM_INPUT\n" <<
	          std::setw(35) << "" << "and STREAM_OUTPUT.\n" << std::endl;
	std::cout << "\nset stream_info [dt] [di] [f] [fv]" << std::setw(1) << "" << "Send a SET_STREAM_INFO command to change\n" <<
	          std::setw(35) << "" << "a stream info field value to a new\n" <<
	          std::setw(35) << "" << "value using the current setting, where dt\n" <<
	          std::setw(35) << "" << "stands for descriptor type and should be a\n" <<
	          std::setw(35) << "" << "string, di stands for descriptor index and\n" <<
	          std::setw(35) << "" << "should be an integer, f stands for field\n" <<
	          std::setw(35) << "" << "and should be a string, and fv stands for\n" <<
	          std::setw(35) << "" << "field value and should be an integer.\n" <<
	          std::setw(35) << "" << "Valid descriptor types are STREAM_INPUT\n" <<
	          std::setw(35) << "" << "and STREAM_OUTPUT.\n" <<
	          std::setw(35) << "" << "Valid fields are msrp_accumulated_latency\n" <<
	          std::setw(35) << "" << "and stream_dest_mac.\n" <<
	          std::setw(35) << "" << "This command should be used after sending a\n" <<
	          std::setw(35) << "" << "GET_STREAM_INFO command and receiving a\n" <<
	          std::setw(35) << "" << "GET_STREAM_INFO response." << std::endl;
	std::cout << "\nget stream_info [dt] [di] [f]" << std::setw(6) << "" << "Display the GET_STREAM_INFO Stream ID\n" <<
	          std::setw(35) << "" << "or the MSRP Accumulated Latency field\n" <<
	          std::setw(35) << "" << "using the current setting, where dt stands\n" <<
	          std::setw(35) << "" << "for descriptor type and should be a string,\n" <<
	          std::setw(35) << "" << "di stands for descriptor index and should be\n" <<
	          std::setw(35) << "" << "an integer, f stands for field and should\n" <<
	          std::setw(35) << "" << "be a string.\n" <<
	          std::setw(35) << "" << "Valid descriptor types are STREAM_INPUT\n" <<
	          std::setw(35) << "" << "and STREAM_OUTPUT.\n" <<
	          std::setw(35) << "" << "Valid fields are stream_id,\n" <<
	          std::setw(35) << "" << "msrp_accumulated_latency, and\n" <<
	          std::setw(35) << "" << "stream_dest_mac.\n" <<
	          std::setw(35) << "" << "This command should be used after sending a\n" <<
	          std::setw(35) << "" << "GET_STREAM_INFO command and receiving a\n" <<
	          std::setw(35) << "" << "GET_STREAM_INFO response." << std::endl;
	std::cout << "\nget stream_info [dt] [di]" << std::setw(10) << "" << "Send a GET_STREAM_INFO command to\n" <<
	          std::setw(35) << "" << "fetch the current information of a stream\n" <<
	          std::setw(35) << "" << "using the current setting, where dt stands\n" <<
	          std::setw(35) << "" << "for descriptor type and should be a string,\n" <<
	          std::setw(35) << "" << "and di stands for descriptor index and\n" <<
	          std::setw(35) << "" << "should be an integer.\n" <<
	          std::setw(35) << "" << "Valid descriptor types are STREAM_INPUT\n" <<
	          std::setw(35) << "" << "and STREAM_OUTPUT." << std::endl;
	std::cout << "\nset sampling_rate [dt] [di]" << std::setw(8) << "" << "Send a SET_SAMPLING_RATE command to change\n" <<
		  std::setw(35) << "" << "the sampling rate of a port or unit." << std::endl;
	std::cout << "\nget sampling_rate [dt] [di]" << std::setw(8) << "" << "Send a GET_SAMPLING_RATE command to get the \n" <<
		  std::setw(35) << "" << "current sampling rate of a port or unit." << std::endl;
	std::cout << "\nset clock_source [dt] [di]" << std::setw(9) << "" << "Send a SET_CLOCK_SOURCE command to change\n" <<
		 std::setw(35) << "" << "the clock source of a clock domain." << std::endl;
	std::cout << "\nget clock_source [dt] [di]" << std::setw(9) << "" << "Send a GET_CLOCK_SOURCE command to get the\n" <<
		 std::setw(35) << "" << "current clock source of a clock domain." << std::endl;
	std::cout << "\nstart streaming [dt] [di]" << std::setw(10) << "" << "Send a START_STREAMING command to start\n" <<
		std::setw(35) << "" << "streaming on a previously connected stream\n" <<
		std::setw(35) << "" << "that was connected via ACMP or has\n" <<
		std::setw(35) << "" << "previously been stopped with the\n" <<
		std::setw(35) << "" << "STOP_STREAMING command." << std::endl;
	std::cout << "\nstop streaming [dt] [di]" << std::setw(11) << "" << "Send a STOP_STREAMING command to stop a\n" <<
		std::setw(35) << "" << "connected stream for streaming media." << std::endl;
	std::cout << "\nset name [dt] [di] [ni] [n]" << std::setw(8) << "" << "Send a GET_NAME command to change the\n" <<
	          std::setw(35) << "" << "value of a name field within a\n" <<
	          std::setw(35) << "" << "descriptor using the current setting, where\n" <<
	          std::setw(35) << "" << "dt stands for descriptor type and should be\n" <<
	          std::setw(35) << "" << "a string, di stands for descriptor index and\n" <<
	          std::setw(35) << "" << "should be an integer, ni stands for name\n" <<
	          std::setw(35) << "" << "index and should be an integer, and n stands\n" <<
	          std::setw(35) << "" << "for name and should be a string." << std::endl;
	std::cout << "\nget name [dt] [di] [ni]" << std::setw(12) << "" << "Send a GET_NAME command to fetch the\n" <<
	          std::setw(35) << "" << "value of a name field within a\n" <<
	          std::setw(35) << "" << "descriptor using the current end station and\n" <<
	          std::setw(35) << "" << "configuration setting, where dt stands for\n" <<
	          std::setw(35) << "" << "descriptor type and should be a string, di\n" <<
	          std::setw(35) << "" << "stands for descriptor index and should be an\n" <<
	          std::setw(35) << "" << "integer, and ni stands for name index and\n" <<
	          std::setw(35) << "" << "should be an integer." << std::endl;
	std::cout << "\npath" << std::setw(31) << "" << "Display the location of the redirected\n" <<
	          std::setw(35) << "" << "output file." << std::endl;
	std::cout << "\npath [pathname]" << std::setw(20) << "" << "Change the location of the redirected output\n" <<
	          std::setw(35) << "" << "file." << std::endl;
	std::cout << "\nlog [filename]" << std::setw(21) << "" << "Redirect output to a specified file." << std::endl;
	std::cout << "\nunlog" << std::setw(30) << "" << "Set output scheme back to console screen." << std::endl;
	std::cout << "\nclr" << std::setw(32) << "" << "Clear the console screen." << std::endl;
	std::cout << "\nquit or q" << std::setw(26) << "" << "Exit out of the thread execution." << std::endl;

	return 0;
}

int avdecc_cmd_line::cmd_version()
{
	std::cout << "avdecc_controller version: " << controller_ref->get_version() << std::endl;
	return 0;
}

int avdecc_cmd_line::cmd_list()
{
	uint32_t end_station;
	uint8_t *end_station_name;
	uint64_t end_station_guid;
	uint64_t end_station_mac;

	std::cout << "\n" << "End Station" << "  |  " << "Name" << std::setw(21)  << "  |  " <<  "Entity GUID" << std::setw(10) << "  |  " << "MAC" << std::endl;
	std::cout << "------------------------------------------------------------------------------" << std::endl;

	for(uint32_t index_i = 0; index_i < controller_ref->get_end_station_count(); index_i++)
	{
		end_station = index_i;
		end_station_guid = controller_ref->get_end_station_by_index(index_i)->get_end_station_guid();
		end_station_name = controller_ref->get_end_station_by_index(index_i)->get_entity_desc_by_index(current_entity)->get_entity_name();
		end_station_mac = controller_ref->get_end_station_by_index(index_i)->get_end_station_mac();
		std::cout << controller_ref->get_end_station_by_index(index_i)->get_connection_status()
		          << std::setw(10) << std::dec << end_station << "  |  "
		          << std::setw(20) << std::hex << end_station_name << "  |  0x"
		          << std::setw(14) << std::hex << end_station_guid << "  |  0x"
		          << std::hex << end_station_mac << std::endl;
	}

	std::cout << "\nC - End Station Connected." << std::endl;
	std::cout << "D - End Station Disconnected." << std::endl;

	return 0;
}

int avdecc_cmd_line::cmd_list_clock_sync_source()
{
	uint8_t *desc_obj_name;
	uint16_t desc_type_value = 0;
	uint16_t desc_index = 0;
	uint32_t stream_input_desc_count = 0;
	uint32_t stream_output_desc_count = 0;
	bool is_clock_sync_source_set;

	std::cout << "\n" << std::setw(8) << "Endpoint" << "   " << "Descriptor Name" << std::setw(8) << "" <<
	          std::setw(20) << "Descriptor Type" << "   " << std::setw(16) << "Descriptor Index" << std::endl;
	std::cout << "------------------------------------------------------------------------------" << std::endl;

	for(uint32_t index_i = 0; index_i < controller_ref->get_end_station_count(); index_i++)
	{
		stream_input_desc_count = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_count();
		for(uint32_t index_j = 0; index_j < stream_input_desc_count; index_j++)
		{
			is_clock_sync_source_set = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(index_j)->get_stream_flags_clock_sync_source();
			if(is_clock_sync_source_set)
			{
				desc_obj_name = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(index_j)->get_object_name();
				desc_type_value = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(index_j)->get_descriptor_type();
				desc_index = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(index_j)->get_descriptor_index();
				
				std::cout << std::setw(8) << index_i << "   " << std::setw(20) << desc_obj_name <<
					     "   " << std::setw(20) << std::hex << utility->desc_value_to_name(desc_type_value) <<
				             "   " << std::setw(16) << std::hex << desc_index << std::endl;
			}
		}

		stream_output_desc_count = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_count();
		for(uint32_t index_k = 0; index_k < stream_output_desc_count; index_k++)
		{
			is_clock_sync_source_set = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(index_k)->get_stream_flags_clock_sync_source();
			if(is_clock_sync_source_set)
			{
				desc_obj_name = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(index_k)->get_object_name();
				desc_type_value = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(index_k)->get_descriptor_type();
				desc_index = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(index_k)->get_descriptor_index();
				
				std::cout << std::setw(8) << index_i << "   " << std::setw(20) << desc_obj_name <<
					     "   " << std::setw(20) << std::hex << utility->desc_value_to_name(desc_type_value) <<
				             "   " << std::setw(16) << std::hex << desc_index << std::endl;
			}
		}
	}

	return 0;
}

void avdecc_cmd_line::cmd_select()
{
	std::cout << "Current setting" << std::endl;
	std::cout << "\tEnd Station: " << std::dec << current_end_station << " (" << controller_ref->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity)->get_entity_name() << ")" << std::endl;
	std::cout << "\tEntity: " << std::dec << current_entity << std::endl;
	std::cout << "\tConfiguration: " << std::dec << current_config << std::endl;
}

int avdecc_cmd_line::cmd_select(uint32_t new_end_station, uint16_t new_entity, uint16_t new_config)
{
	uint8_t *end_station_name = controller_ref->get_end_station_by_index(new_end_station)->get_entity_desc_by_index(current_entity)->get_entity_name();

	if(is_setting_valid(new_end_station, new_entity, new_config)) // Check if the new setting is valid
	{
		if((current_end_station == new_end_station) && (current_entity == new_entity) && (current_config == new_config))
		{
			std::cout << "Same setting" << std::endl;
			std::cout << "\tEnd Station: " << std::dec << current_end_station << " (" << end_station_name << ")" << std::endl;
			std::cout << "\tEntity: " << std::dec << current_entity << std::endl;
			std::cout << "\tConfiguration: " << std::dec << current_config << std::endl;
		}
		else
		{
			current_end_station = new_end_station;
			current_entity = new_entity;
			current_config = new_config;
			std::cout << "New setting" << std::endl;
			std::cout << "\tEnd Station: " << std::dec << current_end_station << " (" << end_station_name << ")" << std::endl;
			std::cout << "\tEntity: " << std::dec << current_entity << std::endl;
			std::cout << "\tConfiguration: " << std::dec << current_config << std::endl;
		}
	}
	else
	{
		std::cout << "Invalid new setting" << std::endl;
	}

	return 0;
}

int avdecc_cmd_line::cmd_log_level(uint32_t new_log_level)
{
	if(new_log_level < avdecc_lib::TOTAL_NUM_OF_LOGGING_LEVELS)
	{
		controller_ref->set_logging_level(new_log_level);
	}
	else
	{
		std::cout << "Invalid new log level" << std::endl;
	}

	return 0;
}

int avdecc_cmd_line::cmd_view_all()
{
	uint8_t *obj_name = NULL;

	for(uint32_t index_i = 0; index_i < controller_ref->get_end_station_count(); index_i++)
	{
		std::cout << "\nEnd Station: " << index_i << " (" << controller_ref->get_end_station_by_index(index_i)->get_entity_desc_by_index(current_entity)->get_entity_name() << ")" << std::endl;
		std::cout << std::setw(20) << "Descriptor Type" << "   " << std::setw(16)  <<  "Descriptor Index" << "   " << std::setw(20) << "Descriptor Name" << std::endl;
		std::cout << "------------------------------------------------------------------------------" << std::endl;
		
		avdecc_lib::entity_descriptor *entity_desc_ref = controller_ref->get_end_station_by_index(index_i)->get_entity_desc_by_index(current_entity);
		avdecc_lib::configuration_descriptor *config_desc_ref = entity_desc_ref->get_config_desc_by_index(current_config);
		avdecc_lib::locale_descriptor *locale_desc_ref = NULL;
		avdecc_lib::strings_descriptor *strings_desc_ref = NULL;

		if(config_desc_ref->get_locale_desc_count() >= 1)
		{
			locale_desc_ref = config_desc_ref->get_locale_desc_by_index(0);
		}

		if(config_desc_ref->get_strings_desc_count() >= 1)
		{
			strings_desc_ref = config_desc_ref->get_strings_desc_by_index(0);
		}

		switch(0x0)
		{
			case avdecc_lib::AEM_DESC_ENTITY:
				{
					std::cout << std::setw(20) << std::hex << utility->desc_value_to_name(entity_desc_ref->get_descriptor_type());
					std::cout << "   " << std::setw(16) << std::hex << entity_desc_ref->get_descriptor_index();
					std::cout << "   " << std::setw(20) << std::hex << entity_desc_ref->get_entity_name() << std::endl;
				}

			case avdecc_lib::AEM_DESC_CONFIGURATION:
				{
					std::cout << std::setw(20) << utility->desc_value_to_name(config_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << config_desc_ref->get_descriptor_index();
					std::cout << "   " << std::setw(20) << std::hex << config_desc_ref->get_object_name() << std::endl;
					std::cout << "\nTop Level Descriptors" << std::endl;
				}

			case avdecc_lib::AEM_DESC_AUDIO_UNIT:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_audio_unit_desc_count(); index_j++)
				{
					avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = config_desc_ref->get_audio_unit_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(audio_unit_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << audio_unit_desc_ref->get_descriptor_index();
					
					uint8_t localized_desc_index = (audio_unit_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()) && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << audio_unit_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_STREAM_INPUT:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_stream_input_desc_count(); index_j++)
				{
					avdecc_lib::stream_input_descriptor *stream_input_desc_ref = config_desc_ref->get_stream_input_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(stream_input_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << stream_input_desc_ref->get_descriptor_index();
					
					uint8_t localized_desc_index = (stream_input_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << stream_input_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_stream_output_desc_count(); index_j++)
				{
					avdecc_lib::stream_output_descriptor *stream_output_desc_ref = config_desc_ref->get_stream_output_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(stream_output_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << stream_output_desc_ref->get_descriptor_index();

					uint8_t localized_desc_index = (stream_output_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << stream_output_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_JACK_INPUT:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_jack_input_desc_count(); index_j++)
				{
					avdecc_lib::jack_input_descriptor *jack_input_desc_ref = config_desc_ref->get_jack_input_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(jack_input_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << jack_input_desc_ref->get_descriptor_index();
				
					uint8_t localized_desc_index = (jack_input_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << jack_input_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_JACK_OUTPUT:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_jack_output_desc_count(); index_j++)
				{
					avdecc_lib::jack_output_descriptor *jack_output_desc_ref = config_desc_ref->get_jack_output_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(jack_output_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << jack_output_desc_ref->get_descriptor_index();
								
					uint8_t localized_desc_index = (jack_output_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << jack_output_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_AVB_INTERFACE:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_avb_interface_desc_count(); index_j++)
				{
					avdecc_lib::avb_interface_descriptor *avb_interface_desc_ref = config_desc_ref->get_avb_interface_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(avb_interface_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << avb_interface_desc_ref->get_descriptor_index();
												
					uint8_t localized_desc_index = (avb_interface_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << avb_interface_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_clock_source_desc_count(); index_j++)
				{
					avdecc_lib::clock_source_descriptor *clk_src_desc_ref = config_desc_ref->get_clock_source_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(clk_src_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << clk_src_desc_ref->get_descriptor_index();
																
					uint8_t localized_desc_index = (clk_src_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << clk_src_desc_ref->get_object_name() << std::endl;
					}
				}

			case avdecc_lib::AEM_DESC_LOCALE:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_locale_desc_count(); index_j++)
				{
					avdecc_lib::locale_descriptor *locale_def_ref = config_desc_ref->get_locale_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(locale_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << locale_desc_ref->get_descriptor_index();
					std::cout << "   " << std::setw(20) << std::hex << locale_desc_ref->get_locale_identifier() << std::endl;
				}

			case avdecc_lib::AEM_DESC_STRINGS:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_strings_desc_count(); index_j++)
				{
					avdecc_lib::strings_descriptor *strings_desc_ref = config_desc_ref->get_strings_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(strings_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << strings_desc_ref->get_descriptor_index() << std::endl;
				}

			case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
				for(uint32_t index_j = 0; index_j < config_desc_ref->get_clock_domain_desc_count(); index_j++)
				{
					avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = config_desc_ref->get_clock_domain_desc_by_index(index_j);
					std::cout << std::setw(20) << utility->desc_value_to_name(clk_domain_desc_ref->get_descriptor_type());
					std::cout << "   "<<  std::setw(16) << std::hex << clk_domain_desc_ref->get_descriptor_index();
																
					uint8_t localized_desc_index = (clk_domain_desc_ref->get_localized_description()) & 0x3;
					if(strings_desc_ref && locale_desc_ref && (localized_desc_index < locale_desc_ref->get_number_of_strings()))
					{
						std::cout << "   " << std::setw(20) << std::hex << strings_desc_ref->get_string_by_index(localized_desc_index) << std::endl;
					}
					else
					{
						std::cout << "   " << std::setw(20) << std::hex << clk_domain_desc_ref->get_object_name() << std::endl;
					}
				}

				break;
		}
	}

	return 0;
}

int avdecc_cmd_line::cmd_view_descriptor(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());

	std::cout << "\ndescriptor_type: " << utility->desc_value_to_name(desc_type_value);
	std::cout << "\ndescriptor_index: 0x" << std::hex << desc_index;

	switch(desc_type_value)
	{
		case avdecc_lib::AEM_DESC_ENTITY:
			{
				avdecc_lib::entity_descriptor *entity_desc_ref = controller_ref->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity);
				std::cout << "\nentity_id = 0x" << std::hex << entity_desc_ref->get_entity_id();
				std::cout << "\nvendor_id = " << std::dec << entity_desc_ref->get_vendor_id();
				std::cout << "\nentity_model_id = 0x" << std::hex << entity_desc_ref->get_entity_model_id();
				std::cout << "\nentity_capabilities = 0x" << std::hex << entity_desc_ref->get_entity_capabilities();
				std::cout << "\ntalker_stream_sources = 0x" << std::hex << entity_desc_ref->get_talker_stream_sources();
				std::cout << "\ntalker_capabilities = 0x" << std::hex << entity_desc_ref->get_talker_capabilities();
				std::cout << "\nlistener_stream_sinks = 0x" << std::hex << entity_desc_ref->get_listener_stream_sinks();
				std::cout << "\nlistener_capabilities = 0x" << std::hex << entity_desc_ref->get_listener_capabilities();
				std::cout << "\ncontroller_capabilities = 0x" << std::hex << entity_desc_ref->get_controller_capabilities();
				std::cout << "\navailable_index = 0x" << std::hex << entity_desc_ref->get_available_index();
				std::cout << "\nassociation_id = 0x" << std::hex << entity_desc_ref->get_association_id();
				std::cout << "\nentity_name = " << std::dec << entity_desc_ref->get_entity_name();
				std::cout << "\nvendor_name_string = " << std::dec << entity_desc_ref->get_vendor_name_string();
				std::cout << "\nmodel_name_string = " << std::dec << entity_desc_ref->get_model_name_string();
				std::cout << "\nfirmware_version = " << std::dec << entity_desc_ref->get_firmware_version();
				std::cout << "\ngroup_name = " << std::dec << entity_desc_ref->get_group_name();
				std::cout << "\nserial_number = " << std::dec << entity_desc_ref->get_serial_number();
				std::cout << "\nconfigurations_count = " << std::dec << entity_desc_ref->get_configurations_count();
				std::cout << "\ncurrent_configuration = " << std::dec << entity_desc_ref->get_current_configuration();
			}
			break;

		case avdecc_lib::AEM_DESC_CONFIGURATION:
			{
				avdecc_lib::configuration_descriptor *config_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config);
				std::cout << "\nobject_name = " << std::dec << config_desc_ref->get_object_name();
				std::cout << "\nlocalized_description = " << std::dec << config_desc_ref->get_localized_description();
				std::cout << "\ndescriptor_counts_count = " << std::dec << config_desc_ref->get_descriptor_counts_count();
				std::cout << "\ndescriptor_counts_offset = " << std::dec << config_desc_ref->get_descriptor_counts_offset();

				uint16_t desc_counts_count = config_desc_ref->get_descriptor_counts_count();
				uint16_t desc_type_from_config = 0;
				uint16_t desc_count_from_config = 0;
				uint16_t offset = 0x0;

				if(desc_counts_count > 0)
				{
					std::cout << "\nTop level descriptors" << std::endl;

					for(uint32_t index_i = 0; index_i < desc_counts_count; index_i++)
					{
						desc_type_from_config = config_desc_ref->get_desc_type_from_config_by_index(index_i);
						desc_count_from_config = config_desc_ref->get_desc_count_from_config_by_index(index_i);

						std::cout << "\tdesc_type = 0x" << std::hex << desc_type_from_config << " (" <<
						          utility->desc_value_to_name(desc_type_from_config) << ")" << std::endl;
						std::cout << "\tdesc_count = " << std::dec << desc_count_from_config << std::endl;
					}
				}
			}
			break;

		case avdecc_lib::AEM_DESC_AUDIO_UNIT:
			{
				avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_unit_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << audio_unit_desc_ref->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << audio_unit_desc_ref->get_localized_description();
				std::cout << "\nclock_domain_index = " << std::dec << audio_unit_desc_ref->get_clock_domain_index();
				std::cout << "\nnumber_of_stream_input_ports = " << std::dec << audio_unit_desc_ref->get_number_of_stream_input_ports();
				std::cout << "\nbase_stream_input_port = " << std::dec << audio_unit_desc_ref->get_base_stream_input_port();
				std::cout << "\nnumber_of_stream_output_ports = " << std::dec << audio_unit_desc_ref->get_number_of_stream_output_ports();
				std::cout << "\nbase_stream_output_port = " << std::dec << audio_unit_desc_ref->get_base_stream_output_port();
				std::cout << "\nnumber_of_external_input_ports = " << std::dec << audio_unit_desc_ref->get_number_of_external_input_ports();
				std::cout << "\nbase_external_input_port = " << std::dec << audio_unit_desc_ref->get_base_external_input_port();
				std::cout << "\nnumber_of_external_output_ports = " << std::dec << audio_unit_desc_ref->get_number_of_external_output_ports();
				std::cout << "\nbase_external_output_port = " << std::dec << audio_unit_desc_ref->get_base_external_output_port();
				std::cout << "\nnumber_of_internal_input_ports = " << std::dec << audio_unit_desc_ref->get_number_of_internal_input_ports();
				std::cout << "\nbase_internal_input_port = " << std::dec << audio_unit_desc_ref->get_base_internal_input_port();
				std::cout << "\nnumber_of_internal_output_ports = " << std::dec << audio_unit_desc_ref->get_number_of_internal_output_ports();
				std::cout << "\nbase_internal_output_port = " << std::dec << audio_unit_desc_ref->get_base_internal_output_port();
				std::cout << "\nnumber_of_controls = " << std::dec << audio_unit_desc_ref->get_number_of_controls();
				std::cout << "\nbase_control = " << std::dec << audio_unit_desc_ref->get_base_control();
				std::cout << "\nnumber_of_signal_selectors = " << std::dec << audio_unit_desc_ref->get_number_of_signal_selectors();
				std::cout << "\nbase_signal_selector = " << std::dec << audio_unit_desc_ref->get_base_signal_selector();
				std::cout << "\nnumber_of_mixers = " << std::dec << audio_unit_desc_ref->get_number_of_mixers();
				std::cout << "\nbase_mixer = " << std::dec << audio_unit_desc_ref->get_base_mixer();
				std::cout << "\nnumber_of_matrices = " << std::dec << audio_unit_desc_ref->get_number_of_matrices();
				std::cout << "\nbase_matrix = " << std::dec << audio_unit_desc_ref->get_base_matrix();
				std::cout << "\nnumber_of_splitters = " << std::dec << audio_unit_desc_ref->get_number_of_splitters();
				std::cout << "\nbase_splitter = " << std::dec << audio_unit_desc_ref->get_base_splitter();
				std::cout << "\nnumber_of_combiners = " << std::dec << audio_unit_desc_ref->get_number_of_combiners();
				std::cout << "\nbase_combiner = " << std::dec << audio_unit_desc_ref->get_base_combiner();
				std::cout << "\nnumber_of_demultiplexers = " << std::dec << audio_unit_desc_ref->get_number_of_demultiplexers();
				std::cout << "\nbase_demultiplexer = " << std::dec << audio_unit_desc_ref->get_base_demultiplexer();
				std::cout << "\nnumber_of_multiplexers = " << std::dec << audio_unit_desc_ref->get_number_of_multiplexers();
				std::cout << "\nbase_multiplexer = " << std::dec << audio_unit_desc_ref->get_base_multiplexer();
				std::cout << "\nnumber_of_transcoders = " << std::dec << audio_unit_desc_ref->get_number_of_transcoders();
				std::cout << "\nbase_transcoder = " << std::dec << audio_unit_desc_ref->get_base_transcoder();
				std::cout << "\nnumber_of_control_blocks = " << std::dec << audio_unit_desc_ref->get_number_of_control_blocks();
				std::cout << "\nbase_control_block = " << std::dec << audio_unit_desc_ref->get_base_control_block();
				std::cout << "\ncurrent_sampling_rate = " << std::dec << audio_unit_desc_ref->get_current_sampling_rate();
				std::cout << "\nsampling_rates_offset = " << std::dec << audio_unit_desc_ref->get_sampling_rates_offset();
				std::cout << "\nsampling_rates_count = " << std::dec << audio_unit_desc_ref->get_sampling_rates_count();

				for(uint32_t index_i = 0; index_i < audio_unit_desc_ref->get_sampling_rates_count(); index_i++)
				{
					std::cout << "\nsampling_rate_" << index_i << " = " << std::dec << audio_unit_desc_ref->get_sampling_rate_by_index(index_i);
				}
			}
			break;

		case avdecc_lib::AEM_DESC_STREAM_INPUT:
			{
				avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << stream_input_desc_ref->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << stream_input_desc_ref->get_localized_description();
				std::cout << "\nclock_domain_index = 0x" << std::hex << stream_input_desc_ref->get_clock_domain_index();
				std::cout << "\nstream_flags = 0x" << std::hex << stream_input_desc_ref->get_stream_flags();
				std::cout << "\n\tstream_flags.clock_sync_source = " << std::dec << stream_input_desc_ref->get_stream_flags_clock_sync_source();
				std::cout << "\n\tstream_flags.class_a = " << std::dec << stream_input_desc_ref->get_stream_flags_class_a();
				std::cout << "\n\tstream_flags.class_b = " << std::dec << stream_input_desc_ref->get_stream_flags_class_b();
				std::cout << "\n\tstream_flags.supports_encrypted = " << std::dec << stream_input_desc_ref->get_stream_flags_supports_encrypted();
				std::cout << "\n\tstream_flags.primary_backup_valid = " << std::dec << stream_input_desc_ref->get_stream_flags_primary_backup_valid();
				std::cout << "\n\tstream_flags.primary_backup_valid = " << std::dec << stream_input_desc_ref->get_stream_flags_primary_backup_valid();
				std::cout << "\n\tstream_flags.secondary_backup_supported = " << std::dec << stream_input_desc_ref->get_stream_flags_secondary_backup_supported();
				std::cout << "\n\tstream_flags.secondary_backup_valid = " << std::dec << stream_input_desc_ref->get_stream_flags_secondary_backup_valid();
				std::cout << "\n\tstream_flags.tertiary_backup_supported = " << std::dec << stream_input_desc_ref->get_stream_flags_tertiary_backup_supported();
				std::cout << "\n\tstream_flags.tertiary_back_up_valid = " << std::dec << stream_input_desc_ref->get_stream_flags_tertiary_back_up_valid();
				std::cout << "\ncurrent_format = " << std::hex << stream_input_desc_ref->get_current_format();
				std::cout << "\nformats_offset = " << std::dec << stream_input_desc_ref->get_formats_offset();
				std::cout << "\nnumber_of_formats = " << std::dec << stream_input_desc_ref->get_number_of_formats();
				std::cout << "\nbackup_talker_entity_id_0 = 0x" << std::hex << stream_input_desc_ref->get_backup_talker_entity_id_0();
				std::cout << "\nbackup_talker_unique_0 = 0x" << std::hex << stream_input_desc_ref->get_backup_talker_unique_0();
				std::cout << "\nbackup_talker_entity_id_1 = 0x" << std::hex << stream_input_desc_ref->get_backup_talker_entity_id_1();
				std::cout << "\nbackup_talker_unique_1 = 0x" << std::hex << stream_input_desc_ref->get_backup_talker_unique_1();
				std::cout << "\nbackup_talker_entity_id_2 = 0x" << std::hex << stream_input_desc_ref->get_backup_talker_entity_id_2();
				std::cout << "\nbackup_talker_unique_2 = 0x" << std::hex << stream_input_desc_ref->get_backup_talker_unique_2();
				std::cout << "\nbackedup_talker_entity_id = 0x" << std::hex << stream_input_desc_ref->get_backedup_talker_entity_id();
				std::cout << "\nbackedup_talker_unique = 0x" << std::hex << stream_input_desc_ref->get_backedup_talker_unique();
				std::cout << "\navb_interface_index = 0x" << std::hex << stream_input_desc_ref->get_avb_interface_index();
				std::cout << "\nbuffer_length = " << std::dec << stream_input_desc_ref->get_buffer_length();
			}
			break;

		case avdecc_lib::AEM_DESC_STREAM_OUTPUT:
			{
				avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << stream_output_desc_ref->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << stream_output_desc_ref->get_localized_description();
				std::cout << "\nclock_domain_index = 0x" << std::hex << stream_output_desc_ref->get_clock_domain_index();
				std::cout << "\nstream_flags = 0x" << std::hex << stream_output_desc_ref->get_stream_flags();
				std::cout << "\n\tstream_flags.clock_sync_source = " << std::dec << stream_output_desc_ref->get_stream_flags_clock_sync_source();
				std::cout << "\n\tstream_flags.class_a = " << std::dec << stream_output_desc_ref->get_stream_flags_class_a();
				std::cout << "\n\tstream_flags.class_b = " << std::dec << stream_output_desc_ref->get_stream_flags_class_b();
				std::cout << "\n\tstream_flags.supports_encrypted = " << std::dec << stream_output_desc_ref->get_stream_flags_supports_encrypted();
				std::cout << "\n\tstream_flags.primary_backup_valid = " << std::dec << stream_output_desc_ref->get_stream_flags_primary_backup_valid();
				std::cout << "\n\tstream_flags.primary_backup_valid = " << std::dec << stream_output_desc_ref->get_stream_flags_primary_backup_valid();
				std::cout << "\n\tstream_flags.secondary_backup_supported = " << std::dec << stream_output_desc_ref->get_stream_flags_secondary_backup_supported();
				std::cout << "\n\tstream_flags.secondary_backup_valid = " << std::dec << stream_output_desc_ref->get_stream_flags_secondary_backup_valid();
				std::cout << "\n\tstream_flags.tertiary_backup_supported = " << std::dec << stream_output_desc_ref->get_stream_flags_tertiary_backup_supported();
				std::cout << "\n\tstream_flags.tertiary_back_up_valid = " << std::dec << stream_output_desc_ref->get_stream_flags_tertiary_back_up_valid();
				std::cout << "\ncurrent_format = " << std::hex << stream_output_desc_ref->get_current_format();
				std::cout << "\nformats_offset = " << std::dec << stream_output_desc_ref->get_formats_offset();
				std::cout << "\nnumber_of_formats = " << std::dec << stream_output_desc_ref->get_number_of_formats();
				std::cout << "\nbackup_talker_entity_id_0 = 0x" << std::hex << stream_output_desc_ref->get_backup_talker_entity_id_0();
				std::cout << "\nbackup_talker_unique_0 = 0x" << std::hex << stream_output_desc_ref->get_backup_talker_unique_0();
				std::cout << "\nbackup_talker_entity_id_1 = 0x" << std::hex << stream_output_desc_ref->get_backup_talker_entity_id_1();
				std::cout << "\nbackup_talker_unique_1 = 0x" << std::hex << stream_output_desc_ref->get_backup_talker_unique_1();
				std::cout << "\nbackup_talker_entity_id_2 = 0x" << std::hex << stream_output_desc_ref->get_backup_talker_entity_id_2();
				std::cout << "\nbackup_talker_unique_2 = 0x" << std::hex << stream_output_desc_ref->get_backup_talker_unique_2();
				std::cout << "\nbackedup_talker_entity_id = 0x" << std::hex << stream_output_desc_ref->get_backedup_talker_entity_id();
				std::cout << "\nbackedup_talker_unique = 0x" << std::hex << stream_output_desc_ref->get_backedup_talker_unique();
				std::cout << "\navb_interface_index = 0x" << std::hex << stream_output_desc_ref->get_avb_interface_index();
				std::cout << "\nbuffer_length = " << std::dec << stream_output_desc_ref->get_buffer_length();
			}
			break;

		case avdecc_lib::AEM_DESC_JACK_INPUT:
			{
				avdecc_lib::jack_input_descriptor *jack_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_jack_input_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << jack_input_desc_ref->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << jack_input_desc_ref->get_localized_description();
				std::cout << "\njack_flags = 0x" << std::hex << jack_input_desc_ref->get_jack_flags();
				std::cout << "\n\tclock_sync_source_flag = 0x" << std::hex << jack_input_desc_ref->get_jack_flag_clock_sync_source();
				std::cout << "\n\tcaptive_flag = 0x" << std::hex << jack_input_desc_ref->get_captive();
				std::cout << "\nnumber_of_controls = " << std::dec << jack_input_desc_ref->get_number_of_controls();
				std::cout << "\nbase_control = " << std::dec << jack_input_desc_ref->get_base_control();
			}
			break;

		case avdecc_lib::AEM_DESC_JACK_OUTPUT:
			{
				avdecc_lib::jack_output_descriptor *jack_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_jack_output_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << jack_output_desc_ref->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << jack_output_desc_ref->get_localized_description();
				std::cout << "\njack_flags = 0x" << std::hex << jack_output_desc_ref->get_jack_flags();
				std::cout << "\tclock_sync_source_flag = 0x" << std::hex << jack_output_desc_ref->get_jack_flag_clock_sync_source();
				std::cout << "\tcaptive_flag = 0x" << std::hex << jack_output_desc_ref->get_captive();
				std::cout << "\njack_type = 0x" << std::hex << jack_output_desc_ref->get_jack_type();
				std::cout << "\nnumber_of_controls = " << std::dec << jack_output_desc_ref->get_number_of_controls();
				std::cout << "\nbase_control = " << std::dec << jack_output_desc_ref->get_base_control();
			}
			break;

		case avdecc_lib::AEM_DESC_AVB_INTERFACE:
			{
				avdecc_lib::avb_interface_descriptor *avb_interface_desc = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_avb_interface_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << avb_interface_desc->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << avb_interface_desc->get_localized_description();
				std::cout << "\nmac_address = 0x" << std::hex << avb_interface_desc->get_mac_addr();
				std::cout << "\ninterface_flags = 0x" << std::hex << avb_interface_desc->get_interface_flags();
				std::cout << "\nclock_identity = 0x" << std::hex << avb_interface_desc->get_clock_identity();
				std::cout << "\npriority1 = " << std::dec << avb_interface_desc->get_priority1();
				std::cout << "\nclock_class = " << std::dec << avb_interface_desc->get_clock_class();
				std::cout << "\noffset_scaled_log_variance = " << std::dec << avb_interface_desc->get_offset_scaled_log_variance();
				std::cout << "\nclock_accuracy = " << std::dec << avb_interface_desc->get_clock_accuracy();
				std::cout << "\npriority2 = " << std::dec << avb_interface_desc->get_priority2();
				std::cout << "\ndomain_number = " << std::dec << avb_interface_desc->get_domain_number();
				std::cout << "\nlog_sync_interval = " << std::dec << avb_interface_desc->get_log_sync_interval();
			}
			break;

		case avdecc_lib::AEM_DESC_CLOCK_SOURCE:
			{
				avdecc_lib::clock_source_descriptor *clk_src_desc = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_source_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << clk_src_desc->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << clk_src_desc->get_localized_description();
				std::cout << "\nclock_source_flags = 0x" << std::hex << clk_src_desc->get_clock_source_flags();
				std::cout << "\nclock_source_type = 0x" << std::hex << clk_src_desc->get_clock_source_type();
				std::cout << "\nclock_source_identifier = 0x" << std::hex << clk_src_desc->get_clock_source_identifier();
				std::cout << "\nclock_source_location_type = 0x" << std::hex << clk_src_desc->get_clock_source_location_type();
				std::cout << "\nclock_source_location_index = 0x" << std::hex << clk_src_desc->get_clock_source_location_index();
			}
			break;

		case avdecc_lib::AEM_DESC_LOCALE:
			{
				avdecc_lib::locale_descriptor *locale_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_locale_desc_by_index(desc_index);
				std::cout << "\nlocale_identifier = " << std::hex << locale_desc_ref->get_locale_identifier();
				std::cout << "\nnumber_of_strings = " << std::hex << locale_desc_ref->get_number_of_strings();
				std::cout << "\nbase_strings = " << std::hex << locale_desc_ref->get_base_strings();
			}
			break;

		case avdecc_lib::AEM_DESC_STRINGS:
			{
				avdecc_lib::strings_descriptor *strings_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_strings_desc_by_index(desc_index);
				std::cout << "\nstring_0 = " << std::hex << strings_desc_ref->get_string_by_index(0);
				std::cout << "\nstring_1 = " << std::hex << strings_desc_ref->get_string_by_index(1);
				std::cout << "\nstring_2 = " << std::hex << strings_desc_ref->get_string_by_index(2);
				std::cout << "\nstring_3 = " << std::hex << strings_desc_ref->get_string_by_index(3);
				std::cout << "\nstring_4 = " << std::hex << strings_desc_ref->get_string_by_index(4);
				std::cout << "\nstring_5 = " << std::hex << strings_desc_ref->get_string_by_index(5);
				std::cout << "\nstring_6 = " << std::hex << strings_desc_ref->get_string_by_index(6);
			}
			break;

		case avdecc_lib::AEM_DESC_CLOCK_DOMAIN:
			{
				avdecc_lib::clock_domain_descriptor *clk_domain_desc = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_domain_desc_by_index(desc_index);
				std::cout << "\nobject_name = " << std::hex << clk_domain_desc->get_object_name();
				std::cout << "\nlocalized_description = 0x" << std::hex << clk_domain_desc->get_localized_description();
				std::cout << "\nclock_source_index = 0x" << std::hex << clk_domain_desc->get_clock_source_index();
				std::cout << "\nclock_sources_offset = " << std::dec << clk_domain_desc->get_clock_sources_offset();
				std::cout << "\nclock_sources_count = " << std::dec << clk_domain_desc->get_clock_sources_count();

				for(uint32_t index_i = 0; index_i < clk_domain_desc->get_clock_sources_count(); index_i++)
				{
					std::cout << "\n\tclock_sources = " << std::dec << clk_domain_desc->get_clock_source_by_index(index_i);
				}
			}
			break;

		default:
			std::cout << "Descriptor type is not found." << std::endl;
			break;
	}

	return 0;
}

int avdecc_cmd_line::cmd_read_descriptor(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	uint32_t cmd_notification_id = get_next_notification_id();

	if(desc_type_value == avdecc_lib::AEM_DESC_STRINGS && desc_index == 0)
	{
		controller_ref->get_end_station_by_index(current_end_station)->send_read_desc_cmd((void *)cmd_notification_id, desc_type_value, desc_index);
	}
	else
	{
		std::cout << "cmd_read_descriptor error" << std::endl;
		return -1;
	}

	return 0;
}

int avdecc_cmd_line::cmd_acquire_entity(std::string flag_name, std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	uint32_t flag_id = 0;
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(flag_name.compare("0") == 0)
	{
		flag_id = 0x0;
	}
	else if(flag_name.compare("persistent") == 0)
	{
		flag_id = 0x1;
	}
	else if(flag_name.compare("release") == 0)
	{
		flag_id = 0x80000000;
	}
	else
	{
		std::cout << "\nInvalid flag" << std::endl;
		return -1;
	}

	if(desc_type_value == avdecc_lib::AEM_DESC_ENTITY)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::entity_descriptor *entity_desc_ref = controller_ref->get_end_station_by_index(current_end_station)->get_entity_desc_by_index(current_entity);
		entity_desc_ref->send_acquire_entity_cmd((void *)cmd_notification_id, flag_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
		stream_input_desc_ref->send_acquire_entity_cmd((void *)notification_id, flag_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
		stream_output_desc_ref->send_get_stream_format_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else
	{
		std::cout << "cmd_acquire_entity error" << std::endl;
		return -1;
	}

}

int avdecc_cmd_line::cmd_lock_entity(std::string flag_name, std::string desc_name, uint16_t desc_index)
{
	uint32_t flag_id;
	uint16_t desc_type_value;

	desc_type_value = utility->desc_name_to_value(desc_name.c_str());

	if(flag_name.compare("0") == 0)
	{
		flag_id = 0x0;
	}
	else if(flag_name.compare("unlock") == 0)
	{
		flag_id = 0x1;
	}
	else
	{
		std::cout << "\nInvalid flag" << std::endl;
		return -1;
	}

	std::cout << "Need to implement cmd_lock_entity" << std::endl;

	return 0;
}


int avdecc_cmd_line::cmd_entity_avail()
{
	int status = -1;
	uint32_t cmd_notification_id = get_next_notification_id();
	
	system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
	controller_ref->get_end_station_by_index(current_end_station)->send_entity_avail_cmd((void *)cmd_notification_id);
	status = system_ref->get_last_resp_status();

	std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

	return 0;
}

int avdecc_cmd_line::cmd_controller_avail()
{
	int status = -1;
	uint32_t cmd_notification_id = get_next_notification_id();
	
	system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
	controller_ref->send_controller_avail_cmd((void *)cmd_notification_id, current_end_station);
	status = system_ref->get_last_resp_status();

	std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

	return 0;
}

int avdecc_cmd_line::cmd_set_stream_format(std::string desc_name, uint16_t desc_index, uint64_t new_stream_format)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
		stream_input_desc_ref->send_set_stream_format_cmd((void *)cmd_notification_id, new_stream_format);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;
		std::cout << "Stream format: " << std::hex << stream_input_desc_ref->set_stream_format_stream_format() << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
		stream_output_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else
	{
		std::cout << "cmd_get_stream_format error" << std::endl;
		return -1;
	}
}

int avdecc_cmd_line::cmd_get_stream_format(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
		stream_input_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;
		std::cout << "Stream format: " << std::hex << stream_input_desc_ref->get_stream_format_stream_format() << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
		stream_output_desc_ref->send_get_stream_format_cmd((void *)cmd_notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else
	{
		std::cout << "cmd_get_stream_format error" << std::endl;
		return -1;
	}
}


int avdecc_cmd_line::cmd_set_stream_info(std::string desc_name, uint16_t desc_index, std::string stream_info_field,
                                         uint64_t new_stream_info_field_value)
{
	std::cout << "Need to implement cmd_set_stream_info" << std::endl;

	return 0;
}


int avdecc_cmd_line::cmd_get_stream_info(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
		stream_input_desc_ref->send_get_stream_info_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)notification_id);
		avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
		stream_output_desc_ref->send_get_stream_info_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else
	{
		std::cout << "cmd_get_stream_info error" << std::endl;
		return -1;
	}
}

int avdecc_cmd_line::cmd_set_name(std::string desc_name, uint16_t desc_index, uint16_t name_index, std::string new_name)
{
	
	std::cout << "Need to implement cmd_set_name" << std::endl;

	return 0;
}

int avdecc_cmd_line::cmd_get_name(std::string desc_name, uint16_t desc_index, uint16_t name_index)
{
	std::cout << "Need to implement cmd_get_name" << std::endl;

	return 0;
}

int avdecc_cmd_line::cmd_set_sampling_rate(std::string desc_name, uint16_t desc_index, uint32_t new_sampling_rate)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_AUDIO_UNIT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_unit_desc_by_index(desc_index);
		audio_unit_desc_ref->send_set_sampling_rate_cmd((void *)notification_id, new_sampling_rate);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;
		std::cout << "Sampling rate: " << std::dec << audio_unit_desc_ref->set_sampling_rate_sampling_rate();

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_VIDEO_CLUSTER)
	{
		std::cout << "\nVideo Cluster descriptor is not implemented." << std::endl;
		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_SENSOR_CLUSTER)
	{
		std::cout << "\nSensor Cluster descriptor is not implemented." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "cmd_get_sampling_rate error" << std::endl;
		return -1;
	}

	return 0;
}

int avdecc_cmd_line::cmd_get_sampling_rate(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_AUDIO_UNIT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)notification_id);
		avdecc_lib::audio_unit_descriptor *audio_unit_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_audio_unit_desc_by_index(desc_index);
		audio_unit_desc_ref->send_get_sampling_rate_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;
		std::cout << "Sampling rate: " << std::dec << audio_unit_desc_ref->get_sampling_rate_sampling_rate();

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_VIDEO_CLUSTER)
	{
		std::cout << "\nVideo Cluster descriptor is not implemented." << std::endl;
		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_SENSOR_CLUSTER)
	{
		std::cout << "\nSensor Cluster descriptor is not implemented." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "cmd_get_sampling_rate error" << std::endl;
		return -1;
	}

	return 0;
}

int avdecc_cmd_line::cmd_set_clock_source(std::string desc_name, uint16_t desc_index, uint16_t new_clk_src_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = get_next_notification_id();

	system_ref->set_wait_for_next_cmd((void *)notification_id);
	avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_domain_desc_by_index(desc_index);
	clk_domain_desc_ref->send_set_clock_source_cmd((void *)notification_id, new_clk_src_index);
	status = system_ref->get_last_resp_status();

	std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;
	std::cout << "Sampling rate: 0x" << std::dec << clk_domain_desc_ref->set_clock_source_clock_source_index();

	return 0;
}

uint32_t avdecc_cmd_line::get_next_notification_id()
{
	return notification_id++;
}

int avdecc_cmd_line::cmd_get_clock_source(std::string desc_name, uint16_t desc_index)
{
	uint32_t cmd_notification_id = get_next_notification_id();
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	uint16_t clk_src_index = 0;
	int status = -1;

	system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
	avdecc_lib::clock_domain_descriptor *clk_domain_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_clock_domain_desc_by_index(desc_index);
	clk_domain_desc_ref->send_get_clock_source_cmd((void *)notification_id);
	status = system_ref->get_last_resp_status();
	clk_src_index = clk_domain_desc_ref->get_clock_source_clock_source_index();

	std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;
	std::cout << "Clock source : 0x" << std::dec << clk_domain_desc_ref->get_clock_source_by_index(clk_src_index);

	return 0;
}

int avdecc_cmd_line::cmd_start_streaming(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
		stream_input_desc_ref->send_start_streaming_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)notification_id);
		avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
		stream_output_desc_ref->send_start_streaming_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else
	{
		std::cout << "cmd_get_stream_info error" << std::endl;
		return -1;
	}
}

int avdecc_cmd_line::cmd_stop_streaming(std::string desc_name, uint16_t desc_index)
{
	uint16_t desc_type_value = utility->desc_name_to_value(desc_name.c_str());
	int status = -1;
	uint32_t cmd_notification_id = 0;

	if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_INPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)cmd_notification_id);
		avdecc_lib::stream_input_descriptor *stream_input_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_input_desc_by_index(desc_index);
		stream_input_desc_ref->send_stop_streaming_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else if(desc_type_value == avdecc_lib::AEM_DESC_STREAM_OUTPUT)
	{
		cmd_notification_id = get_next_notification_id();
		system_ref->set_wait_for_next_cmd((void *)notification_id);
		avdecc_lib::stream_output_descriptor *stream_output_desc_ref = controller_ref->get_config_desc_by_index(current_end_station, current_entity, current_config)->get_stream_output_desc_by_index(desc_index);
		stream_output_desc_ref->send_stop_streaming_cmd((void *)notification_id);
		status = system_ref->get_last_resp_status();

		std::cout << "\nStatus: " << utility->cmd_status_value_to_name(status) << std::endl;

		return 1;
	}
	else
	{
		std::cout << "cmd_get_stream_info error" << std::endl;
		return -1;
	}
}

void avdecc_cmd_line::cmd_path()
{
	std::cout << "Log path: " << log_path << std::endl;
}

int avdecc_cmd_line::cmd_path(std::string new_log_path)
{
	log_path = new_log_path;
	return 0;
}

bool avdecc_cmd_line::is_setting_valid(uint32_t end_station, uint16_t entity, uint16_t config)
{
	bool is_setting_valid = (end_station < controller_ref->get_end_station_count()) &&
	                        (entity < controller_ref->get_end_station_by_index(end_station)->get_entity_desc_count()) &&
	                        (config == controller_ref->get_end_station_by_index(end_station)->get_entity_desc_by_index(entity)->get_current_configuration());

	return is_setting_valid;
}
