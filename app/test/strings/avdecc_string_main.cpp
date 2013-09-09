/**
 * avdecc_string_main.cpp
 *
 * Implementation of the main function for the AVDECC string conversions.
 *
 * (c) AudioScience, Inc. 2013
 */

#include <iostream>
#include "avdecc_string.h"

using namespace std;

int main()
{
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(0) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(1) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(2) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(3) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(4) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(5) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(74) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(75) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_cmd_value_to_name(76) << std::endl;

	std::cout << "\n0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_cmd_name_to_value("read_descriptor") << std::endl;
	std::cout << "0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_cmd_name_to_value("acquire_entity") << std::endl;
	std::cout << "0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_cmd_name_to_value("stream_info") << std::endl;
	std::cout << "0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_cmd_name_to_value("get_stream_info") << std::endl;

	std::cout << "\n\n" << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(0) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(1) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(2) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(3) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(4) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(5) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(37) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(38) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::aem_desc_value_to_name(39) << std::endl;

	std::cout << "\n0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_desc_name_to_value("entity") << std::endl;
	std::cout << "0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_desc_name_to_value("configuration") << std::endl;
	std::cout << "0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_desc_name_to_value("stream_in") << std::endl;
	std::cout << "0x" << std::hex << asi_avb_avdecc_string::avdecc_string::aem_desc_name_to_value("stream_INPUT") << std::endl;

	std::cout << "\n\n" << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(-1) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(0) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(1) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(2) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(10) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(11) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(12) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(13) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(21) << std::endl;
	std::cout << asi_avb_avdecc_string::avdecc_string::avdecc_lib_notification_value_to_name(23) << std::endl;

	return 0;
}