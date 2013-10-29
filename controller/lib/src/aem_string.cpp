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
 * aem_string.cpp
 *
 * AEM String conversion implementation
 */

#include <string>
#include <algorithm>
#include <stdexcept>
#include "enumeration.h"
#include "aem_string.h"

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

const char *aem_desc_error = "AEM_DESC_ERROR";

const char *aem_cmds_status_names[] =
{
	"STATUS_SUCCESS",
	"STATUS_NOT_IMPLEMENTED",
	"STATUS_NO_SUCH_DESCRIPTOR",
	"STATUS_ENTITY_LOCKED",
	"STATUS_ENTITY_ACQUIRED",
	"STATUS_NOT_AUTHENTICATED",
	"STATUS_AUTHENTICATION_DISABLED ",
	"STATUS_BAD_ARGUMENTS",
	"STATUS_NO_RESOURCES",
	"STATUS_IN_PROGRESS",
	"STATUS_ENTITY_MISBEHAVING",
	"STATUS_NOT_SUPPORTED",
	"STATUS_STREAM_IS_RUNNING",
	"STATUS_INVALID_COMMAND",
};

const char *aem_cmd_status_error = "AEM_CMD_STATUS_ERROR";

const char *notification_names[] =
{
	"NO_MATCH_FOUND",
	"END_STATION_DISCOVERED",
	"END_STATION_CONNECTED",
	"END_STATION_RECONNECTED",
	"END_STATION_DISCONNECTED",
	"COMMAND_SENT",
	"COMMAND_TIMEOUT",
	"COMMAND_RESENT",
	"COMMAND_SUCCESS",
	"RESPONSE_RECEIVED"
};

const char *notification_error = "NOTIFICATION_ERROR";

const char *logging_level_names[] =
{
	"LOGGING_LEVEL_ERROR",
	"LOGGING_LEVEL_WARNING",
	"LOGGING_LEVEL_NOTICE",
	"LOGGING_LEVEL_INFO",
	"LOGGING_LEVEL_DEBUG",
	"LOGGING_LEVEL_VERBOSE"
};

const char *logging_level_error = "LOGGING_LEVEL_ERROR";

namespace avdecc_lib
{
	const char * STDCALL aem_string::cmd_value_to_name(uint16_t cmd_value)
	{
		if(cmd_value < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS)
		{
			return aem_cmds_names[cmd_value];
		}

		return aem_cmd_error;
	}

	uint16_t STDCALL aem_string::cmd_name_to_value(const char *cmd_name)
	{
		std::string cmd_name_string;
		cmd_name_string = cmd_name;

		std::transform(cmd_name_string.begin(), cmd_name_string.end(), cmd_name_string.begin(), ::toupper);

		for(uint32_t index_i = 0; index_i < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS; index_i++)
		{
			if(cmd_name_string.compare(aem_cmds_names[index_i]) == 0)
			{
				return index_i;
			}
		}

		return (uint16_t)avdecc_lib::AEM_CMD_ERROR;
	}

	const char * STDCALL aem_string::desc_value_to_name(uint16_t desc_value)
	{
		if(desc_value < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS)
		{
			return aem_descs_names[desc_value];
		}

		return aem_desc_error;
	}

	uint16_t STDCALL aem_string::desc_name_to_value(const char * desc_name)
	{
		std::string desc_name_string;
		desc_name_string = desc_name;

		std::transform(desc_name_string.begin(), desc_name_string.end(), desc_name_string.begin(), ::toupper);

		for(uint32_t index_i = 0; index_i < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS; index_i++)
		{
			if(desc_name_string.compare(aem_descs_names[index_i]) == 0)
			{
				return index_i;
			}
		}

		return (uint16_t)avdecc_lib::AEM_DESC_ERROR;
	}

	const char * STDCALL aem_string::cmd_status_value_to_name(uint16_t cmd_status_value)
	{
		if(cmd_status_value < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS_STATUS)
		{
			return aem_cmds_status_names[cmd_status_value];
		}

		return aem_cmd_status_error;
	}

	const char * STDCALL aem_string::notification_value_to_name(uint16_t notification_value)
	{
		if(notification_value < avdecc_lib::TOTAL_NUM_OF_NOTIFICATIONS)
		{
			return notification_names[notification_value];
		}

		return notification_error;
	}

	const char * STDCALL aem_string::logging_level_value_to_name(uint16_t logging_level_value)
	{
		if(logging_level_value < avdecc_lib::TOTAL_NUM_OF_LOGGING_LEVELS)
		{
			return logging_level_names[logging_level_value];
		}

		return logging_level_error;
	}
}
