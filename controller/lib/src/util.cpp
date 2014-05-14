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
 * util_imp.cpp
 *
 * Utility implementation
 */

#include <sstream>
#include <string>
#include <algorithm>
#include <string.h>
#include "enumeration.h"
#include "util.h"

namespace avdecc_lib
{
    namespace utility
    {
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

        const char *aem_cmds_status_names[] =
        {
            "SUCCESS", // AEM_STATUS_SUCCESS
            "NOT_IMPLEMENTED", // AEM_STATUS_NOT_IMPLEMENTED
            "NO_SUCH_DESCRIPTOR", // AEM_STATUS_NO_SUCH_DESCRIPTOR
            "ENTITY_LOCKED", // AEM_STATUS_ENTITY_LOCKED
            "ENTITY_ACQUIRED", // AEM_STATUS_ENTITY_ACQUIRED
            "NOT_AUTHENTICATED", // AEM_STATUS_NOT_AUTHENTICATED
            "AUTHENTICATION_DISABLED ", // AEM_STATUS_AUTHENTICATION_DISABLED
            "BAD_ARGUMENTS", // AEM_STATUS_BAD_ARGUMENTS
            "STATUS_NO_RESOURCES", // STATUS_NO_RESOURCES
            "IN_PROGRESS", // AEM_STATUS_IN_PROGRESS
            "ENTITY_MISBEHAVING", // AEM_STATUS_ENTITY_MISBEHAVING
            "NOT_SUPPORTED", // AEM_STATUS_NOT_SUPPORTED
            "STREAM_IS_RUNNING", // AEM_STATUS_STREAM_IS_RUNNING
        };

        const char *acmp_cmds_names[] =
        {
            "CONNECT_TX_COMMAND",
            "CONNECT_TX_RESPONSE",
            "DISCONNECT_TX_COMMAND",
            "DISCONNECT_TX_RESPONSE",
            "GET_TX_STATE_COMMAND",
            "GET_TX_STATE_RESPONSE",
            "CONNECT_RX_COMMAND",
            "CONNECT_RX_RESPONSE",
            "DISCONNECT_RX_COMMAND",
            "DISCONNECT_RX_RESPONSE",
            "GET_RX_STATE_COMMAND",
            "GET_RX_STATE_RESPONSE",
            "GET_TX_CONNECTION_COMMAND",
            "GET_TX_CONNECTION_RESPONSE"
        };

        const char *acmp_cmds_status_names[] =
        {
            "SUCCESS", // ACMP_STATUS_SUCCESS
            "LISTENER_UNKNOWN_ID", // ACMP_STATUS_LISTENER_UNKNOWN_ID
            "TALKER_UNKNOWN_ID", // ACMP_STATUS_TALKER_UNKNOWN_ID
            "TALKER_DEST_MAC_FAIL", // ACMP_STATUS_TALKER_DEST_MAC_FAIL
            "TALKER_NO_STREAM_INDEX", // ACMP_STATUS_TALKER_NO_STREAM_INDEX
            "TALKER_NO_BANDWIDTH", // ACMP_STATUS_TALKER_NO_BANDWIDTH
            "TALKER_EXCLUSIVE", // ACMP_STATUS_TALKER_EXCLUSIVE
            "LISTENER_TALKER_TIMEOUT", // ACMP_STATUS_LISTENER_TALKER_TIMEOUT
            "LISTENER_EXCLUSIVE", // ACMP_STATUS_LISTENER_EXCLUSIVE
            "STATE_UNAVAILABLE", // ACMP_STATUS_STATE_UNAVAILABLE
            "NOT_CONNECTED", // ACMP_STATUS_NOT_CONNECTED
            "NO_SUCH_CONNECTION", // ACMP_STATUS_NO_SUCH_CONNECTION
            "COULD_NOT_SEND_MESSAGE", // ACMP_STATUS_COULD_NOT_SEND_MESSAGE
            "TALKER_MISBEHAVING", // ACMP_STATUS_TALKER_MISBEHAVING
            "LISTENER_MISBEHAVING", // ACMP_STATUS_LISTENER_MISBEHAVING
            "RESERVED", // ACMP_STATUS_RESERVED
            "CONTROLLER_NOT_AUTHORIZED", // ACMP_STATUS_CONTROLLER_NOT_AUTHORIZED
            "INCOMPATIBLE_REQUEST", // ACMP_STATUS_INCOMPATIBLE_REQUEST
            "LISTENER_INVALID_CONNECTION" // ACMP_STATUS_LISTENER_INVALID_CONNECTION
        };

        const char *notification_names[] =
        {
            "NO_MATCH_FOUND",
            "END_STATION_CONNECTED",
            "END_STATION_DISCONNECTED",
            "COMMAND_TIMEOUT",
            "RESPONSE_RECEIVED",
            "END_STATION_READ_COMPLETED"
        };

        const char *logging_level_names[] =
        {
            "ERROR", // LOGGING_LEVEL_ERROR
            "WARNING", // LOGGING_LEVEL_WARNING
            "NOTICE", // LOGGING_LEVEL_NOTICE
            "INFO", // LOGGING_LEVEL_INFO
            "DEBUG", // LOGGING_LEVEL_DEBUG
            "VERBOSE" // LOGGING_LEVEL_VERBOSE
        };

        struct acmp_command_and_timeout
        {
            uint32_t cmd;
            uint32_t timeout_ms;
        };

        struct acmp_command_and_timeout acmp_command_and_timeout_table[] =
        {
            {avdecc_lib::CONNECT_TX_COMMAND, avdecc_lib::ACMP_CONNECT_TX_COMMAND_TIMEOUT_MS},
            {avdecc_lib::DISCONNECT_TX_COMMAND, avdecc_lib::ACMP_DISCONNECT_TX_COMMAND_TIMEOUT_MS},
            {avdecc_lib::GET_TX_STATE_COMMAND, avdecc_lib::ACMP_GET_TX_STATE_COMMAND_TIMEOUT_MS},
            {avdecc_lib::CONNECT_RX_COMMAND, avdecc_lib::ACMP_CONNECT_RX_COMMAND_TIMEOUT_MS},
            {avdecc_lib::DISCONNECT_RX_COMMAND, avdecc_lib::ACMP_DISCONNECT_RX_COMMAND_TIMEOUT_MS},
            {avdecc_lib::GET_RX_STATE_COMMAND, avdecc_lib::ACMP_GET_RX_STATE_COMMAND_TIMEOUT_MS},
            {avdecc_lib::GET_TX_CONNECTION_COMMAND, avdecc_lib::ACMP_GET_TX_CONNECTION_COMMAND_TIMEOUT_MS},
            {avdecc_lib::AEM_ACMP_ERROR, 0xffff}
        };

        struct ieee1722_format
        {
            uint64_t fmt;
            const char *str;
        };

        struct ieee1722_format ieee1722_format_table[] =
        {
            {UINT64_C(0x00a0020140000100), "IEC...48KHZ_1CH"}, // IEC61883_AM824_MBLA_48KHZ_1CH
            {UINT64_C(0x00a0020240000200), "IEC...48KHZ_2CH"}, // IEC61883_AM824_MBLA_48KHZ_2CH
            {UINT64_C(0x00a0020440000400), "IEC...48KHZ_4CH"}, // IEC61883_AM824_MBLA_48KHZ_4CH
            {UINT64_C(0x00a0020840000800), "IEC...48KHZ_8CH"}, // IEC61883_AM824_MBLA_48KHZ_8CH
            {UINT64_C(0x00a0040140000100), "IEC...96KHZ_1CH"}, // IEC61883_AM824_MBLA_96KHZ_1CH
            {UINT64_C(0x00a0040240000200), "IEC...96KHZ_2CH"}, // IEC61883_AM824_MBLA_96KHZ_2CH
            {UINT64_C(0x00a0040440000400), "IEC...96KHZ_4CH"}, // IEC61883_AM824_MBLA_96KHZ_4CH
            {UINT64_C(0x00a0040840000800), "IEC...96KHZ_8CH"}, // IEC61883_AM824_MBLA_96KHZ_8CH
            {UINT64_C(0x0000000000000000), "UNKNOWN"}
        };

        const char * STDCALL aem_cmd_value_to_name(uint16_t cmd_value)
        {
            if(cmd_value < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS)
            {
                return aem_cmds_names[cmd_value];
            }

            return "UNKNOWN";
        }

        uint16_t STDCALL aem_cmd_name_to_value(const char *cmd_name)
        {
            std::string cmd_name_string;
            cmd_name_string = cmd_name;

            std::transform(cmd_name_string.begin(), cmd_name_string.end(), cmd_name_string.begin(), ::toupper);

            for(uint32_t i = 0; i < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS; i++)
            {
                if(cmd_name_string == aem_cmds_names[i])
                {
                    return (uint16_t)i;
                }
            }

            return (uint16_t)avdecc_lib::AEM_CMD_ERROR;
        }

        const char * STDCALL aem_desc_value_to_name(uint16_t desc_value)
        {
            if(desc_value < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS)
            {
                return aem_descs_names[desc_value];
            }

            return "UNKNOWN";
        }

        uint16_t STDCALL aem_desc_name_to_value(const char * desc_name)
        {
            std::string desc_name_string;
            desc_name_string = desc_name;

            std::transform(desc_name_string.begin(), desc_name_string.end(), desc_name_string.begin(), ::toupper);

            for(uint32_t i = 0; i < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS; i++)
            {
                if(desc_name_string.compare(aem_descs_names[i]) == 0)
                {
                    return (uint16_t)i;
                }
            }

            return (uint16_t)avdecc_lib::AEM_DESC_ERROR;
        }

        const char * STDCALL aem_cmd_status_value_to_name(uint32_t aem_cmd_status_value)
        {
            if(aem_cmd_status_value < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS_STATUS)
            {
                return aem_cmds_status_names[aem_cmd_status_value];
            }
            else if(aem_cmd_status_value == avdecc_lib::AVDECC_LIB_STATUS_INVALID)
            {
                return "AVDECC_LIB_STATUS_INVALID";
            }
            else if(aem_cmd_status_value == avdecc_lib::AVDECC_LIB_STATUS_TICK_TIMEOUT)
            {
                return "AVDECC_LIB_STATUS_TICK_TIMEOUT";
            }

            return "UNKNOWN";
        }

        const char * STDCALL acmp_cmd_value_to_name(uint32_t cmd_value)
        {
            if(cmd_value < avdecc_lib::TOTAL_NUM_OF_ACMP_CMDS)
            {
                return acmp_cmds_names[cmd_value];
            }

            return "UNKNOWN";
        }

        uint16_t STDCALL acmp_cmd_name_to_value(const char *cmd_name)
        {
            std::string cmd_name_string;
            cmd_name_string = cmd_name;

            std::transform(cmd_name_string.begin(), cmd_name_string.end(), cmd_name_string.begin(), ::toupper);

            for(uint32_t i = 0; i < avdecc_lib::TOTAL_NUM_OF_ACMP_CMDS; i++)
            {
                if(cmd_name_string == acmp_cmds_names[i])
                {
                    return (uint16_t)i;
                }
            }

            return (uint16_t)avdecc_lib::AEM_CMD_ERROR;
        }

        const char * STDCALL acmp_cmd_status_value_to_name(uint32_t acmp_cmd_status_value)
        {
            if(acmp_cmd_status_value < avdecc_lib::TOTAL_NUM_OF_ACMP_CMDS_STATUS)
            {
                return acmp_cmds_status_names[acmp_cmd_status_value];
            }

            return "UNKNOWN";
        }

        const char * STDCALL notification_value_to_name(uint16_t notification_value)
        {
            if(notification_value < avdecc_lib::TOTAL_NUM_OF_NOTIFICATIONS)
            {
                return notification_names[notification_value];
            }

            return "UNKNOWN";
        }

        const char * STDCALL logging_level_value_to_name(uint16_t logging_level_value)
        {
            if(logging_level_value < avdecc_lib::TOTAL_NUM_OF_LOGGING_LEVELS)
            {
                return logging_level_names[logging_level_value];
            }

            return "UNKNOWN";
        }

        uint32_t STDCALL acmp_cmd_to_timeout(const uint32_t acmp_cmd)
        {
            struct acmp_command_and_timeout *p = &acmp_command_and_timeout_table[0];

            while(p->cmd != avdecc_lib::AEM_ACMP_ERROR)
            {
                if(p->cmd == acmp_cmd)
                {
                    return p->timeout_ms;
                }

                p++;
            }

            return (uint32_t)0xffff;
        }

        uint64_t STDCALL ieee1722_format_name_to_value(const char *format_name)
        {
            struct ieee1722_format *p = &ieee1722_format_table[0];

            while(p->fmt != UINT64_C(0x0000000000000000))
            {
                if(strcmp(p->str, format_name) == 0)
                {
                    return p->fmt;
                }

                p++;
            }

            return (uint64_t)0xffff;
        }

        const char * STDCALL ieee1722_format_value_to_name(uint64_t format)
        {
            struct ieee1722_format *p = &ieee1722_format_table[0];

            while(p->fmt != 0)
            {
                if(p->fmt == format)
                {
                    return p->str;
                }

                p++;
            }

            return "UNKNOWN";
        }

        const char * STDCALL end_station_mac_to_string(uint64_t end_station_mac)
        {
            static std::string mac_substring;
            std::stringstream mac_to_string;

            mac_to_string << std::hex << end_station_mac;
            mac_substring = " [" + (mac_to_string.str().substr(mac_to_string.str().length()-4, 2) + ":" +
                            mac_to_string.str().substr(mac_to_string.str().length()-2, 2)) + "]";

            return mac_substring.c_str(); 
        }

        void convert_uint64_to_eui48(const uint64_t value, uint8_t new_value[6])
        {
            for(uint32_t i = 0; i < 6; i++)
            {
                new_value[i] = (uint8_t) (value >> ((5 - i) * 8));
            }
        }

        void convert_eui48_to_uint64(const uint8_t value[6], uint64_t &new_value)
        {
            new_value = 0;

            for(uint32_t i = 0; i < 6; i++)
            {
                new_value |= (uint64_t) value[i] << ((5 - i) * 8);
            }
        }

    }
}