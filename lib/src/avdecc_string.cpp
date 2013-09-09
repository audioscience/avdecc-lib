/**
 * avdecc_string.cpp
 *
 * Implementation of the avdecc_string class, which is used as a shared library to access common string functions.
 *
 * (c) AudioScience, Inc. 2013
 */

#include <string>
#include <algorithm>
#include <stdexcept>
#include "avdecc_string.h"

const char *aem_cmds_names[] = 
{ 
	"ACQUIRE_ENTITY",
	"LOCK_ENTITY",
	"ENTITY_AVAILABLE",
	"CONTROLLER_AVAILABLE",
	"READ_DESCRIPTOR",
	"WRITE_DESCRIPTOR",
	"SET_CONFIGURATION",
	"GET_CONFIGURATION",
	"SET_STREAM_FORMAT",
	"GET_STREAM_FORMAT",
	"SET_VIDEO_FORMAT",
	"GET_VIDEO_FORMAT",
	"SET_SENSOR_FORMAT",
	"GET_SENSOR_FORMAT",
	"SET_STREAM_INFO",
	"GET_STREAM_INFO",
	"SET_NAME",
	"GET_NAME",
	"SET_ASSOCIATION_ID",
	"GET_ASSOCIATION_ID",
	"SET_SAMPLING_RATE",
	"GET_SAMPLING_RATE",
	"SET_CLOCK_SOURCE",
	"GET_CLOCK_SOURCE",
	"SET_CONTROL",
	"GET_CONTROL",
	"INCREMENT_CONTROL",
	"DECREMENT_CONTROL",
	"SET_SIGNAL_SELECTOR",
	"GET_SIGNAL_SELECTOR",
	"SET_MIXER",
	"GET_MIXER",
	"SET_MATRIX",
	"GET_MATRIX",
	"START_STREAMING",
	"STOP_STREAMING",
	"REGISTER_UNSOLICITED_NOTIFICATION",
	"DEREGISTER_UNSOLICITED_NOTIFICATION",
	"IDENTIFY_NOTIFICATION",
	"GET_AVB_INFO",
	"GET_AS_PATH",
	"GET_COUNTERS",
	"REBOOT",
	"GET_AUDIO_MAP",
	"ADD_AUDIO_MAPPINGS",
	"REMOVE_AUDIO_MAPPINGS",
	"GET_VIDEO_MAP",
	"ADD_VIDEO_MAPPINGS",
	"REMOVE_VIDEO_MAPPINGS",
	"GET_SENSOR_MAP",
	"ADD_SENSOR_MAPPINGS",
	"REMOVE_SENSOR_MAPPINGS",
	"START_OPERATION",
	"ABORT_OPERATION",
	"OPERATION_STATUS",
	"AUTH_ADD_KEY",
	"AUTH_DELETE_KEY",
	"AUTH_GET_KEY_LIST",
	"AUTH_GET_KEY",
	"AUTH_ADD_KEY_TO_CHAIN",
	"AUTH_DELETE_KEY_FROM_CHAIN",
	"AUTH_GET_KEYCHAIN_LIST",
	"AUTH_GET_IDENTITY",
	"AUTH_ADD_TOKEN",
	"AUTH_DELETE_TOKEN",
	"AUTHENTICATE",
	"DEAUTHENTICATE",
	"ENABLE_TRANSPORT_SECURITY",
	"DISABLE_TRANSPORT_SECURITY",
	"ENABLE_STREAM_ENCRYPTION",
	"DISABLE_STREAM_ENCRYPTION",
	"SET_MEMORY_OBJECT_LENGTH",
	"GET_MEMORY_OBJECT_LENGTH",
	"SET_STREAM_BACKUP",
	"GET_STREAM_BACKUP"
	};

const char *aem_cmd_error = "AEM_CMD_ERROR";

const char *aem_descs_names[] =
{ 
	"ENTITY",
	"CONFIGURATION",
	"AUDIO_UNIT",
	"VIDEO_UNIT",
	"SENSOR_UNIT",
	"STREAM_INPUT",
	"STREAM_OUTPUT",
	"JACK_INPUT",
	"JACK_OUTPUT",
	"AVB_INTERFACE",
	"CLOCK_SOURCE",
	"MEMORY_OBJECT",
	"LOCALE",
	"STRINGS",
	"STREAM_PORT_INPUT",
	"STREAM_PORT_OUTPUT",
	"EXTERNAL_PORT_INPUT",
	"EXTERNAL_PORT_OUTPUT",
	"INTERNAL_PORT_INPUT",
	"INTERNAL_PORT_OUTPUT",
	"AUDIO_CLUSTER",
	"VIDEO_CLUSTER",
	"SENSOR_CLUSTER",
	"AUDIO_MAP",
	"VIDEO_MAP",
	"SENSOR_MAP",
	"CONTROL",
	"SIGNAL_SELECTOR",
	"MIXER",
	"MATRIX",
	"MATRIX_SIGNAL",
	"SIGNAL_SPLITTER",
	"SIGNAL_COMBINER",
	"SIGNAL_DEMULTIPLEXER",
	"SIGNAL_MULTIPLEXER",
	"SIGNAL_TRANSCODER",
	"CLOCK_DOMAIN",
	"CONTROL_BLOCK"
};	

const char *aem_decs_error = "AEM_DESC_ERROR";


namespace asi_avb_avdecc_string
{
	const char * STDCALL avdecc_string::aem_cmd_value_to_name(uint16_t cmd_value)
	{
		if(cmd_value >= 0x0 && cmd_value < asi_avb_avdecc_enum::TOTAL_NUM_OF_CMDS)
		{
			return aem_cmds_names[cmd_value];
		}

		return aem_cmd_error;
	}

	uint16_t STDCALL avdecc_string::aem_cmd_name_to_value(const char *cmd_name)
	{
		std::string cmd_name_string;
		cmd_name_string = cmd_name;

		std::transform(cmd_name_string.begin(), cmd_name_string.end(), cmd_name_string.begin(), ::toupper);

		for(uint32_t index_i = 0; index_i < asi_avb_avdecc_enum::TOTAL_NUM_OF_CMDS; index_i++)
		{
			if(cmd_name_string.compare(aem_cmds_names[index_i]) == 0)
			{
				return index_i;
			}
		}

		return (uint16_t)asi_avb_avdecc_enum::AEM_CMD_ERROR;
	}

	const char * STDCALL avdecc_string::aem_desc_value_to_name(uint16_t desc_value)
	{
		if(desc_value >= 0x0 && desc_value < asi_avb_avdecc_enum::TOTAL_NUM_OF_DESCS)
		{
			return aem_descs_names[desc_value];
		}

		return aem_decs_error;
	}

	uint16_t STDCALL avdecc_string::aem_desc_name_to_value(const char * desc_name)
	{
		std::string desc_name_string;
		desc_name_string = desc_name;

		std::transform(desc_name_string.begin(), desc_name_string.end(), desc_name_string.begin(), ::toupper);

		for(uint32_t index_i = 0; index_i < asi_avb_avdecc_enum::TOTAL_NUM_OF_DESCS; index_i++)
		{
			if(desc_name_string.compare(aem_descs_names[index_i]) == 0)
			{
				return index_i;
			}
		}

		return (uint16_t)asi_avb_avdecc_enum::AEM_DESC_ERROR;
	}

	const char * STDCALL avdecc_string::avdecc_lib_notification_value_to_name(int int_value)
	{
		switch(int_value)
		{
		case asi_avb_avdecc_enum::NO_MATCH_FOUND:
			return "NO_MATCH_FOUND";
			break;

		case asi_avb_avdecc_enum::ENDPOINT_DISCOVERED:
			return "ENDPOINT_DISCOVERED";
			break;

		case asi_avb_avdecc_enum::ENDPOINT_CONNECTED:
			return "ENDPOINT_CONNECTED";
			break;

		case asi_avb_avdecc_enum::ENDPOINT_DISCONNECTED:
			return "ENDPOINT_DISCONNECTED";
			break;

		case asi_avb_avdecc_enum::INVALID_COMMAND:
			return "INVALID_COMMAND";
			break;

		case asi_avb_avdecc_enum::COMMAND_TIMEOUT:
			return "COMMAND_TIMEOUT";
			break;

		case asi_avb_avdecc_enum::COMMAND_RESENT:
			return "COMMAND_RESENT";
			break;

		case asi_avb_avdecc_enum::COMMAND_SUCCESS:
			return "COMMAND_SUCCESS"; 
			break;

		default:
			return "NOTIFICATION NOT FOUND";
			break;
		}
	}
}
