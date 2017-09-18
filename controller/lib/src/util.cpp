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
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <string.h>
#include "enumeration.h"
#include "util.h"
#include "cassert.h"

#define IEEE1722_FORMAT_STR_DELIM             ("_")

// 1722 Stream formats
#define IEEE1722_FORMAT_VERSION_SHIFT         (63)
#define IEEE1722_FORMAT_VERSION_MASK          (0x00000001)
#define IEEE1722_FORMAT_SUBTYPE_SHIFT         (56)
#define IEEE1722_FORMAT_SUBTYPE_MASK          (0x0000007f)

// IEC 61883
#define IEC61883_EXPECTED_TOKEN_COUNT         (5)
#define IEC61883_SF_SHIFT                     (55)
#define IEC61883_SF_MASK                      (0x00000001)
#define IEC61883_TYPE_SHIFT                   (49)
#define IEC61883_TYPE_MASK                    (0x0000003f)
#define IEC61883_6_SFC_SHIFT                  (40)
#define IEC61883_6_SFC_MASK                   (0x00000007)
#define IEC61883_6_BLOCK_COUNT_SHIFT          (32)
#define IEC61883_6_BLOCK_COUNT_MASK           (0x000000ff)
#define IEC61883_6_BLOCKING_SHIFT             (31)
#define IEC61883_6_BLOCKING_MASK              (0x00000001)
#define IEC61883_6_NONBLOCKING_SHIFT          (30)
#define IEC61883_6_NONBLOCKING_MASK           (0x00000001)
#define IEC61883_6_UPTO_SHIFT                 (29)
#define IEC61883_6_UPTO_MASK                  (0x00000001)
#define IEC61883_6_SYNCHRONOUS_SHIFT          (28)
#define IEC61883_6_SYNCHRONOUS_MASK           (0x00000001)
#define IEC61883_6_PACKETIZATION_SHIFT        (43)
#define IEC61883_6_PACKETIZATION_MASK         (0x0000001f)
#define IEC61883_6_AM824_IEC60958_COUNT_SHIFT (16)
#define IEC61883_6_AM824_IEC60958_COUNT_MASK  (0x000000ff)
#define IEC61883_6_AM824_MBLA_COUNT_SHIFT     (8)
#define IEC61883_6_AM824_MBLA_COUNT_MASK      (0x000000ff)
#define IEC61883_6_AM824_MIDI_COUNT_SHIFT     (4)
#define IEC61883_6_AM824_MIDI_COUNT_MASK      (0x0000000f)
#define IEC61883_6_AM824_SMPTE_COUNT_SHIFT    (0)
#define IEC61883_6_AM824_SMPTE_COUNT_MASK     (0x0000000f)

#define IEC61883_6_AM824_TOKEN_TYPE           (0)
#define IEC61883_6_AM824_TOKEN_PACKETIZATION  (1)
#define IEC61883_6_AM824_TOKEN_SFC            (3)
#define IEC61883_6_AM824_TOKEN_BLOCK_COUNT    (4)


// AAF
#define AAF_EXPECTED_TOKEN_COUNT              (6)
#define AAF_UPTO_SHIFT                        (52)
#define AAF_UPTO_MASK                         (0x00000001)
#define AAF_NSR_SHIFT                         (48)
#define AAF_NSR_MASK                          (0x0000000f)
#define AAF_TYPE_SHIFT                        (40)
#define AAF_TYPE_MASK                         (0x000000ff)
#define AAF_PCM_BIT_DEPTH_SHIFT               (32)
#define AAF_PCM_BIT_DEPTH_MASK                (0x000000ff)
#define AAF_PCM_CHANNELS_PER_FRAME_SHIFT      (22)
#define AAF_PCM_CHANNELS_PER_FRAME_MASK       (0x000003ff)
#define AAF_PCM_SAMPLES_PER_FRAME_SHIFT       (12)
#define AAF_PCM_SAMPLES_PER_FRAME_MASK        (0x000003ff)

#define AAF_TOKEN_SUBTYPE                     (0)
#define AAF_TOKEN_NSR                         (1)
#define AAF_TOKEN_PACKETIZATION               (2)
#define AAF_TOKEN_BIT_DEPTH                   (3)
#define AAF_TOKEN_CHANNELS_PER_FRAME          (4)
#define AAF_TOKEN_SAMPLES_PER_FRAME           (5)

// CRF
#define CRF_EXPECTED_TOKEN_COUNT              (6)
#define CRF_TYPE_SHIFT                        (52)
#define CRF_TYPE_MASK                         (0x0000000f)
#define CRF_TIMESTAMP_INTERVAL_SHIFT          (40)
#define CRF_TIMESTAMP_INTERVAL_MASK           (0x00000fff)
#define CRF_TIMESTAMPS_PER_PDU_SHIFT          (32)
#define CRF_TIMESTAMPS_PER_PDU_MASK           (0x000000ff)
#define CRF_PULL_VALUE_SHIFT                  (29)
#define CRF_PULL_VALUE_MASK                   (0x00000007)
#define CRF_BASE_FREQUENCY_SHIFT              (0)
#define CRF_BASE_FREQUENCY_MASK               (0x1fffffff)

#define CRF_TOKEN_SUBTYPE                     (0)
#define CRF_TOKEN_TYPE                        (1)
#define CRF_TOKEN_TIMESTAMP_INTERVAL          (2)
#define CRF_TOKEN_TIMESTAMPS_PER_PDU          (3)
#define CRF_TOKEN_PULL                        (4)
#define CRF_TOKEN_BASE_FREQUENCY              (5)

namespace avdecc_lib
{
namespace utility
{
    const char * aem_cmds_names[] =
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
            "GET_STREAM_BACKUP"};

    const char * aem_descs_names[] =
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
            "CONTROL_BLOCK"};

    const char * aem_cmds_status_names[] =
        {
            "SUCCESS",                  // AEM_STATUS_SUCCESS
            "NOT_IMPLEMENTED",          // AEM_STATUS_NOT_IMPLEMENTED
            "NO_SUCH_DESCRIPTOR",       // AEM_STATUS_NO_SUCH_DESCRIPTOR
            "ENTITY_LOCKED",            // AEM_STATUS_ENTITY_LOCKED
            "ENTITY_ACQUIRED",          // AEM_STATUS_ENTITY_ACQUIRED
            "NOT_AUTHENTICATED",        // AEM_STATUS_NOT_AUTHENTICATED
            "AUTHENTICATION_DISABLED ", // AEM_STATUS_AUTHENTICATION_DISABLED
            "BAD_ARGUMENTS",            // AEM_STATUS_BAD_ARGUMENTS
            "STATUS_NO_RESOURCES",      // STATUS_NO_RESOURCES
            "IN_PROGRESS",              // AEM_STATUS_IN_PROGRESS
            "ENTITY_MISBEHAVING",       // AEM_STATUS_ENTITY_MISBEHAVING
            "NOT_SUPPORTED",            // AEM_STATUS_NOT_SUPPORTED
            "STREAM_IS_RUNNING",        // AEM_STATUS_STREAM_IS_RUNNING
    };

    const char * acmp_cmds_names[] =
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
            "GET_TX_CONNECTION_RESPONSE"};

    const char * acmp_cmds_status_names[] =
        {
            "SUCCESS",                    // ACMP_STATUS_SUCCESS
            "LISTENER_UNKNOWN_ID",        // ACMP_STATUS_LISTENER_UNKNOWN_ID
            "TALKER_UNKNOWN_ID",          // ACMP_STATUS_TALKER_UNKNOWN_ID
            "TALKER_DEST_MAC_FAIL",       // ACMP_STATUS_TALKER_DEST_MAC_FAIL
            "TALKER_NO_STREAM_INDEX",     // ACMP_STATUS_TALKER_NO_STREAM_INDEX
            "TALKER_NO_BANDWIDTH",        // ACMP_STATUS_TALKER_NO_BANDWIDTH
            "TALKER_EXCLUSIVE",           // ACMP_STATUS_TALKER_EXCLUSIVE
            "LISTENER_TALKER_TIMEOUT",    // ACMP_STATUS_LISTENER_TALKER_TIMEOUT
            "LISTENER_EXCLUSIVE",         // ACMP_STATUS_LISTENER_EXCLUSIVE
            "STATE_UNAVAILABLE",          // ACMP_STATUS_STATE_UNAVAILABLE
            "NOT_CONNECTED",              // ACMP_STATUS_NOT_CONNECTED
            "NO_SUCH_CONNECTION",         // ACMP_STATUS_NO_SUCH_CONNECTION
            "COULD_NOT_SEND_MESSAGE",     // ACMP_STATUS_COULD_NOT_SEND_MESSAGE
            "TALKER_MISBEHAVING",         // ACMP_STATUS_TALKER_MISBEHAVING
            "LISTENER_MISBEHAVING",       // ACMP_STATUS_LISTENER_MISBEHAVING
            "RESERVED",                   // ACMP_STATUS_RESERVED
            "CONTROLLER_NOT_AUTHORIZED",  // ACMP_STATUS_CONTROLLER_NOT_AUTHORIZED
            "INCOMPATIBLE_REQUEST",       // ACMP_STATUS_INCOMPATIBLE_REQUEST
            "LISTENER_INVALID_CONNECTION" // ACMP_STATUS_LISTENER_INVALID_CONNECTION
    };

    const char * notification_names[] =
        {
            "NO_MATCH_FOUND",
            "END_STATION_CONNECTED",
            "END_STATION_DISCONNECTED",
            "COMMAND_TIMEOUT",
            "RESPONSE_RECEIVED",
            "END_STATION_READ_COMPLETED",
            "UNSOLICITED_RESPONSE_RECEIVED"};
    
    const char * acmp_notification_names[] =
    {
        "NULL_ACMP_NOTIFICATION",
        "BROADCAST_ACMP_RESPONSE_RECEIVED",
        "ACMP_RESPONSE_RECEIVED"};

    const char * logging_level_names[] =
        {
            "ERROR",   // LOGGING_LEVEL_ERROR
            "WARNING", // LOGGING_LEVEL_WARNING
            "NOTICE",  // LOGGING_LEVEL_NOTICE
            "INFO",    // LOGGING_LEVEL_INFO
            "DEBUG",   // LOGGING_LEVEL_DEBUG
            "VERBOSE"  // LOGGING_LEVEL_VERBOSE
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
            {avdecc_lib::AEM_ACMP_ERROR, 0xffff}};

    const char * STDCALL aem_cmd_value_to_name(uint16_t cmd_value)
    {
        if (cmd_value < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS)
        {
            return aem_cmds_names[cmd_value];
        }

        return "UNKNOWN";
    }

    uint16_t STDCALL aem_cmd_name_to_value(const char * cmd_name)
    {
        std::string cmd_name_string;
        cmd_name_string = cmd_name;

        std::transform(cmd_name_string.begin(), cmd_name_string.end(), cmd_name_string.begin(), ::toupper);

        for (uint32_t i = 0; i < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS; i++)
        {
            if (cmd_name_string == aem_cmds_names[i])
            {
                return (uint16_t)i;
            }
        }

        return (uint16_t)avdecc_lib::AEM_CMD_ERROR;
    }

    const char * STDCALL aem_desc_value_to_name(uint16_t desc_value)
    {
        if (desc_value < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS)
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

        for (uint32_t i = 0; i < avdecc_lib::TOTAL_NUM_OF_AEM_DESCS; i++)
        {
            if (desc_name_string.compare(aem_descs_names[i]) == 0)
            {
                return (uint16_t)i;
            }
        }

        return (uint16_t)avdecc_lib::AEM_DESC_ERROR;
    }

    const char * STDCALL aem_cmd_status_value_to_name(uint32_t aem_cmd_status_value)
    {
        if (aem_cmd_status_value < avdecc_lib::TOTAL_NUM_OF_AEM_CMDS_STATUS)
        {
            return aem_cmds_status_names[aem_cmd_status_value];
        }
        else if (aem_cmd_status_value == avdecc_lib::AVDECC_LIB_STATUS_INVALID)
        {
            return "AVDECC_LIB_STATUS_INVALID";
        }
        else if (aem_cmd_status_value == avdecc_lib::AVDECC_LIB_STATUS_TICK_TIMEOUT)
        {
            return "AVDECC_LIB_STATUS_TICK_TIMEOUT";
        }

        return "UNKNOWN";
    }

    const char * STDCALL acmp_cmd_value_to_name(uint32_t cmd_value)
    {
        if (cmd_value < avdecc_lib::TOTAL_NUM_OF_ACMP_CMDS)
        {
            return acmp_cmds_names[cmd_value];
        }

        return "UNKNOWN";
    }

    uint16_t STDCALL acmp_cmd_name_to_value(const char * cmd_name)
    {
        std::string cmd_name_string;
        cmd_name_string = cmd_name;

        std::transform(cmd_name_string.begin(), cmd_name_string.end(), cmd_name_string.begin(), ::toupper);

        for (uint32_t i = 0; i < avdecc_lib::TOTAL_NUM_OF_ACMP_CMDS; i++)
        {
            if (cmd_name_string == acmp_cmds_names[i])
            {
                return (uint16_t)i;
            }
        }

        return (uint16_t)avdecc_lib::AEM_CMD_ERROR;
    }

    const char * STDCALL acmp_cmd_status_value_to_name(uint32_t acmp_cmd_status_value)
    {
        if (acmp_cmd_status_value < avdecc_lib::TOTAL_NUM_OF_ACMP_CMDS_STATUS)
        {
            return acmp_cmds_status_names[acmp_cmd_status_value];
        }

        return "UNKNOWN";
    }

    const char * STDCALL notification_value_to_name(uint16_t notification_value)
    {

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

        compile_time_assert(ARRAY_SIZE(notification_names) == TOTAL_NUM_OF_NOTIFICATIONS, assert_notification_names_size);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
        if (notification_value < avdecc_lib::TOTAL_NUM_OF_NOTIFICATIONS)
        {
            return notification_names[notification_value];
        }

        return "UNKNOWN";
    }
    
    const char * STDCALL acmp_notification_value_to_name(uint16_t acmp_notification_value)
    {
        if (acmp_notification_value < avdecc_lib::TOTAL_NUM_OF_ACMP_NOTIFICATIONS)
        {
            return acmp_notification_names[acmp_notification_value];
        }
        
        return "UNKNOWN";
    }

    const char * STDCALL logging_level_value_to_name(uint16_t logging_level_value)
    {
        if (logging_level_value < avdecc_lib::TOTAL_NUM_OF_LOGGING_LEVELS)
        {
            return logging_level_names[logging_level_value];
        }

        return "UNKNOWN";
    }

    uint32_t STDCALL acmp_cmd_to_timeout(const uint32_t acmp_cmd)
    {
        struct acmp_command_and_timeout * p = &acmp_command_and_timeout_table[0];

        while (p->cmd != avdecc_lib::AEM_ACMP_ERROR)
        {
            if (p->cmd == acmp_cmd)
            {
                return p->timeout_ms;
            }

            p++;
        }

        return (uint32_t)0xffff;
    }

    const char * STDCALL end_station_mac_to_string(uint64_t end_station_mac)
    {
        static std::string mac_substring;
        std::stringstream mac_to_string;

        mac_to_string << std::hex << end_station_mac;
        mac_substring = " [" + (mac_to_string.str().substr(mac_to_string.str().length() - 4, 2) + ":" +
                                mac_to_string.str().substr(mac_to_string.str().length() - 2, 2)) +
                        "]";

        return mac_substring.c_str();
    }

    void convert_uint64_to_eui48(const uint64_t value, uint8_t new_value[6])
    {
        for (uint32_t i = 0; i < 6; i++)
        {
            new_value[i] = (uint8_t)(value >> ((5 - i) * 8));
        }
    }

    void convert_eui48_to_uint64(const uint8_t value[6], uint64_t & new_value)
    {
        new_value = 0;

        for (uint32_t i = 0; i < 6; i++)
        {
            new_value |= (uint64_t)value[i] << ((5 - i) * 8);
        }
    }
    
    static std::set<std::string> ieee1722_format_names;
    
    unsigned int ieee1722_format_value_extract_subtype(uint64_t format_value)
    {
        return ieee1722_stream_format(format_value).subtype();
    }

    unsigned int ieee1722_format_value_extract_sample_rate(uint64_t format_value)
    {
        unsigned int sample_rate = 0;
        ieee1722_stream_format sf(format_value);
        switch (sf.subtype())
        {
        case ieee1722_stream_format::IIDC_61883:
            sample_rate = iec_61883_iidc_format(format_value).sample_rate();
            break;
        case ieee1722_stream_format::AAF:
            sample_rate = aaf_format(format_value).sample_rate();
            break;
        }
        
        return sample_rate;
    }

    unsigned int ieee1722_format_value_extract_channel_count(uint64_t format_value)
    {
        unsigned int channel_count = 0;
        ieee1722_stream_format sf(format_value);
        switch (sf.subtype())
        {
        case ieee1722_stream_format::IIDC_61883:
            channel_count = iec_61883_iidc_format(format_value).channel_count();
            break;
        case ieee1722_stream_format::AAF:
            channel_count = aaf_format(format_value).channel_count();
            break;
        }
        
        return channel_count;
    }
    
    unsigned int ieee1722_format_value_extract_ut(uint64_t format_value)
    {
        unsigned int ut = 0;
        ieee1722_stream_format sf(format_value);
        switch (sf.subtype())
        {
            case ieee1722_stream_format::IIDC_61883:
                ut = iec_61883_iidc_format(format_value).upto();
                break;
            case ieee1722_stream_format::AAF:
                ut = aaf_format(format_value).upto();
                break;
        }
        
        return ut;
    }

    unsigned int ieee1722_format_value_extract_packetization(uint64_t format_value)
    {
        unsigned int packetization = 0;
        ieee1722_stream_format sf(format_value);
        switch (sf.subtype())
        {
            case ieee1722_stream_format::IIDC_61883:
                packetization = iec_61883_iidc_format(format_value).packetization_type_value();
                break;
            case ieee1722_stream_format::AAF:
                packetization = aaf_format(format_value).packetization_type();
                break;
        }
        
        return packetization;
    }

    const char * ieee1722_format_value_to_name(uint64_t format_value)
    {
        std::string format_name = "UNKNOWN";
        ieee1722_stream_format sf(format_value);
        switch (sf.subtype())
        {
        case ieee1722_stream_format::IIDC_61883:
            format_name = iec_61883_iidc_format(format_value).name();
            break;
        case ieee1722_stream_format::AAF:
            format_name = aaf_format(format_value).name();
            break;
        case ieee1722_stream_format::CRF:
            format_name = crf_format(format_value).name();
            break;
        }
        
        auto it = ieee1722_format_names.find(format_name);
        if (it == ieee1722_format_names.end())
            return "UNKNOWN";
        else
           return (*it).c_str();
    }
    
    uint64_t ieee1722_format_name_to_value(const char * format_name)
    {
        ieee1722_stream_format sf(format_name);
        switch (sf.subtype())
        {
        case ieee1722_stream_format::IIDC_61883:
            return iec_61883_iidc_format(format_name).value();
        case ieee1722_stream_format::AAF:
            return aaf_format(format_name).value();
        case ieee1722_stream_format::CRF:
            return crf_format(format_name).value();
        }
        
        return 0;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    const std::map<unsigned int, std::string> ieee1722_stream_data_subtypes =
    {
        { ieee1722_stream_format::IIDC_61883, "IIDC-61883" }, // IEC 61883/IIDC format
        { ieee1722_stream_format::MMA_STREAM, "MMA-STREAM" }, // MMA Streams
        { ieee1722_stream_format::AAF, "AAF" },               // AVTP Audio Format
        { ieee1722_stream_format::CVF, "CVF" },               // Compressed Video Format
        { ieee1722_stream_format::CRF, "CRF" }                // Clock Reference Format
    };
    
    ieee1722_stream_format::ieee1722_stream_format(uint64_t format_value) : m_format_value(format_value)
    {
        m_version = (m_format_value >> IEEE1722_FORMAT_VERSION_SHIFT) & IEEE1722_FORMAT_VERSION_MASK;
        m_subtype = (m_format_value >> IEEE1722_FORMAT_SUBTYPE_SHIFT) & IEEE1722_FORMAT_SUBTYPE_MASK;
    }
    
    ieee1722_stream_format::ieee1722_stream_format(const char * format_name) : m_format_name(format_name)
    {
        std::string s = format_name;
        std::string subtype = s.substr(0, s.find(IEEE1722_FORMAT_STR_DELIM));
        
        for (auto it = ieee1722_stream_data_subtypes.begin(); it != ieee1722_stream_data_subtypes.end(); ++it)
            if (it->second == subtype)
                m_subtype = it->first;
    }
    
    bool ieee1722_stream_format::subtype_from_str(std::string subtype)
    {
        bool subtype_found = false;
        for (auto it = ieee1722_stream_data_subtypes.begin(); it != ieee1722_stream_data_subtypes.end(); ++it)
        {
            if (it->second == subtype)
            {
                m_subtype = it->first;
                subtype_found = true;
            }
        }
        
        return subtype_found;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    static const std::map<unsigned int, std::string> crf_types =
    {
        { crf_format::CRF_USER, "USER" },               // User Specified
        { crf_format::CRF_AUDIO_SAMPLE, "AUDIO" },      // Audio sample timestamp
        { crf_format::CRF_VIDEO_FRAME, "VIDEO-FRAME" }, // Video frame sync timestamp
        { crf_format::CRF_VIDEO_LINE, "VIDEO-LINE" },   // Video line sync timestamp
        { crf_format::CRF_MACHINE_CYCLE, "MACHINE" },   // Machine cycle timestamp
    };
    
    static const std::map<unsigned int, std::string> crf_pull_values =
    {
        { crf_format::MULTIPLY_1_0, "1.0-PULL" },
        { crf_format::MULTIPLY_1_1_001, "1/1.001-PULL" },
        { crf_format::MULTIPLY_1_001, "1.001-PULL" },
        { crf_format::MULTIPLY_24_25, "24/25-PULL" },
        { crf_format::MULTIPLY_25_24, "25/24-PULL" },
        { crf_format::MULTIPLY_1_8, "1/8-PULL" }
    };
    
    crf_format::crf_format(uint64_t format_value) : ieee1722_stream_format(format_value)
    {
        m_type = (m_format_value >> CRF_TYPE_SHIFT) & CRF_TYPE_MASK;
        m_timestamp_interval = (m_format_value >> CRF_TIMESTAMP_INTERVAL_SHIFT) & CRF_TIMESTAMP_INTERVAL_MASK;
        m_timestamps_per_pdu = (m_format_value >> CRF_TIMESTAMPS_PER_PDU_SHIFT) & CRF_TIMESTAMPS_PER_PDU_MASK;
        m_pull_value = (m_format_value >> CRF_PULL_VALUE_SHIFT) & CRF_PULL_VALUE_MASK;
        m_base_frequency = (m_format_value >> CRF_BASE_FREQUENCY_SHIFT) & CRF_BASE_FREQUENCY_MASK;
        
        to_string();
    }
    
    void crf_format::to_string()
    {
        std::stringstream ss;
        auto it = ieee1722_stream_data_subtypes.find(CRF);
        if (it != ieee1722_stream_data_subtypes.end())
        {
            ss << it->second << IEEE1722_FORMAT_STR_DELIM;
            it = crf_types.find(m_type);
            if (it != crf_types.end())
            {
                ss << it->second << IEEE1722_FORMAT_STR_DELIM;
                ss << std::to_string(m_timestamp_interval) << "-INTVL" << IEEE1722_FORMAT_STR_DELIM;
                ss << std::to_string(m_timestamps_per_pdu) << "-TS" << IEEE1722_FORMAT_STR_DELIM;
                
                it = crf_pull_values.find(m_pull_value);
                if (it != crf_pull_values.end())
                {
                    ss << it->second<< IEEE1722_FORMAT_STR_DELIM;
                    ss << std::to_string(m_base_frequency) << "HZ";
                    m_format_name = ss.str();
                    ieee1722_format_names.insert(m_format_name);
                }
            }
        }
    }
    
    void crf_format::to_val()
    {
        std::string s(m_format_name);
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(IEEE1722_FORMAT_STR_DELIM)) != std::string::npos) {
            token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + 1);
        }
        tokens.push_back(s);
        
        if (tokens.size() != CRF_EXPECTED_TOKEN_COUNT)
            return;
        
        if (!subtype_from_str(tokens.at(CRF_TOKEN_SUBTYPE)))
            return;
        
        if (m_subtype == CRF)
        {
            if (!crf_type_from_str(tokens.at(CRF_TOKEN_TYPE)) ||
                !crf_timestamp_interval_from_str(tokens.at(CRF_TOKEN_TIMESTAMP_INTERVAL)) ||
                !crf_timestamps_per_pdu_from_str(tokens.at(CRF_TOKEN_TIMESTAMPS_PER_PDU)) ||
                !crf_pull_value_from_str(tokens.at(CRF_TOKEN_PULL)) ||
                !crf_base_frequency_from_str(tokens.at(CRF_TOKEN_BASE_FREQUENCY)))
                return;
            
            uint64_t val = uint64_t( (uint64_t) m_subtype << IEEE1722_FORMAT_SUBTYPE_SHIFT) |
                                     ((uint64_t) m_type << CRF_TYPE_SHIFT) |
                                     ((uint64_t) m_timestamp_interval << CRF_TIMESTAMP_INTERVAL_SHIFT) |
                                     ((uint64_t) m_timestamps_per_pdu << CRF_TIMESTAMPS_PER_PDU_SHIFT) |
                                     ((uint64_t) m_pull_value << CRF_PULL_VALUE_SHIFT) |
                                     ((uint64_t) m_base_frequency << CRF_BASE_FREQUENCY_SHIFT);
                
            m_format_value = val;
        }
    }
    
    bool crf_format::crf_type_from_str(std::string type)
    {
        bool crf_type_found = false;
        for (auto it = crf_types.begin(); it != crf_types.end(); ++it)
        {
            if (it->second == type)
            {
                m_type = it->first;
                crf_type_found = true;
            }
        }
        
        return crf_type_found;
    }
    
    bool crf_format::crf_timestamp_interval_from_str(std::string timestamp_interval)
    {
        std::string timestamp_interval_str = timestamp_interval.substr(0, timestamp_interval.find("-INTVL"));
        if (timestamp_interval_str.empty() || !isdigit(timestamp_interval_str[0]))
            return false;
        
        unsigned int timestamp_interval_val = (unsigned int) std::stoul(timestamp_interval_str, NULL, 0);
        if (!timestamp_interval_val)
            return false;
        
        m_timestamp_interval = timestamp_interval_val;
        return true;
    }
    
    bool crf_format::crf_timestamps_per_pdu_from_str(std::string timestamps_per_pdu)
    {
        std::string timestamps_per_pdu_str = timestamps_per_pdu.substr(0, timestamps_per_pdu.find("-INTVL"));
        if (timestamps_per_pdu_str.empty() || !isdigit(timestamps_per_pdu_str[0]))
            return false;
        
        unsigned int timestamps_per_pdu_val = (unsigned int) std::stoul(timestamps_per_pdu_str, NULL, 0);
        if (!timestamps_per_pdu_val)
            return false;
        
        m_timestamps_per_pdu = timestamps_per_pdu_val;
        return true;
    }
    
    bool crf_format::crf_pull_value_from_str(std::string pull)
    {
        bool pull_value_found = false;
        for (auto it = crf_pull_values.begin(); it != crf_pull_values.end(); ++it)
        {
            if (it->second == pull)
            {
                m_pull_value = it->first;
                pull_value_found = true;
            }
        }
        
        return pull_value_found;
    }
    
    bool crf_format::crf_base_frequency_from_str(std::string base_frequency)
    {
        std::string base_frequency_str = base_frequency.substr(0, base_frequency.find("-HZ"));
        if (base_frequency_str.empty() || !isdigit(base_frequency_str[0]))
            return false;
        
        unsigned int base_frequency_val = (unsigned int) std::stoul(base_frequency_str, NULL, 0);
        if (!base_frequency_val)
            return false;
        
        m_base_frequency = base_frequency_val;
        return true;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    static const std::map<unsigned int, std::string> aaf_nsr_values =
    {
        { aaf_format::NSR_8KHZ, "8KHZ" },
        { aaf_format::NSR_16KHZ, "16KHZ" },
        { aaf_format::NSR_32KHZ, "32KHZ" },
        { aaf_format::NSR_44_1KHZ, "44.1KHZ" },
        { aaf_format::NSR_48KHZ, "48KHZ" },
        { aaf_format::NSR_88_2KHZ, "88.2KHZ" },
        { aaf_format::NSR_96KHZ, "96KHZ" },
        { aaf_format::NSR_176_4KHZ, "176.4KHZ" },
        { aaf_format::NSR_192KHZ, "192KHZ" },
        { aaf_format::NSR_24KHZ, "24KHZ" }
    };
    
    static const std::map<unsigned int, std::string> aaf_packetization_types =
    {
        { aaf_format::FLOAT_32BIT, "FLOAT32" }, // 32-bit floating point
        { aaf_format::INT_32BIT, "INT32" },     // 32-bit integer
        { aaf_format::INT_24BIT, "INT24" },     // 24-bit integer
        { aaf_format::INT_16BIT, "INT16" },     // 16-bit integer
        { aaf_format::AES3_32BIT, "AES32" },    // 32-bit AES3 format
    };

    aaf_format::aaf_format(uint64_t format_value) : ieee1722_stream_format(format_value)
    {
        m_upto = (m_format_value >> AAF_UPTO_SHIFT) & AAF_UPTO_MASK;
        m_nsr_value = (m_format_value >> AAF_NSR_SHIFT) & AAF_NSR_MASK;
        m_packetization_type = (m_format_value >> AAF_TYPE_SHIFT) & AAF_TYPE_MASK;
        switch (m_packetization_type)
        {
        case FLOAT_32BIT:
        case INT_32BIT:
        case INT_24BIT:
        case INT_16BIT:
            decode_aaf_pcm_fields();
            break;
        default:
            break;
        }
        
        to_string();
    }
    
    unsigned int aaf_format::sample_rate()
    {
        switch (m_nsr_value)
        {
        case NSR_8KHZ:
            return 8000;
        case NSR_16KHZ:
            return 16000;
        case NSR_32KHZ:
            return 32000;
        case NSR_44_1KHZ:
            return 44100;
        case NSR_48KHZ:
            return 48000;
        case NSR_88_2KHZ:
            return 88200;
        case NSR_96KHZ:
            return 96000;
        case NSR_176_4KHZ:
            return 176400;
        case NSR_192KHZ:
            return 192000;
        case NSR_24KHZ:
            return 24000;
        }
        
        return 0;
    }

    void aaf_format::to_string()
    {
        std::stringstream ss;
        auto it = ieee1722_stream_data_subtypes.find(AAF);
        if (it != ieee1722_stream_data_subtypes.end())
        {
            ss << it->second << IEEE1722_FORMAT_STR_DELIM;
            it = aaf_nsr_values.find(m_nsr_value);
            if (it != aaf_nsr_values.end())
            {
                ss << it->second << IEEE1722_FORMAT_STR_DELIM;
                it = aaf_packetization_types.find(m_packetization_type);
                if (it != aaf_packetization_types.end())
                {
                    ss << it->second << IEEE1722_FORMAT_STR_DELIM;
                    if (m_packetization_type != AES3_32BIT)
                    {
                        ss << std::to_string(m_bit_depth) << "-BIT" << IEEE1722_FORMAT_STR_DELIM;
                        ss << std::to_string(m_channels_per_frame) << "CH" << IEEE1722_FORMAT_STR_DELIM;
                        ss << std::to_string(m_samples_per_frame) << "-SAMPLES";
                        m_format_name = ss.str();
                        ieee1722_format_names.insert(m_format_name);
                    }
                }
            }
        }
    }
    
    void aaf_format::to_val()
    {
        std::string s(m_format_name);
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(IEEE1722_FORMAT_STR_DELIM)) != std::string::npos) {
            token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + 1);
        }
        tokens.push_back(s);

        if (tokens.size() != AAF_EXPECTED_TOKEN_COUNT)
            return;
        
        if (!subtype_from_str(tokens.at(AAF_TOKEN_SUBTYPE)))
            return;
        
        if (m_subtype == AAF)
        {
            if (!aaf_packetization_type_from_str(tokens.at(AAF_TOKEN_PACKETIZATION)) ||
                !aaf_nsr_from_str(tokens.at(AAF_TOKEN_NSR)))
                return;
            
            if (m_packetization_type != AES3_32BIT)
            {
                if (!aaf_bit_depth_from_str(tokens.at(AAF_TOKEN_BIT_DEPTH)) ||
                    !aaf_channels_per_frame_from_str(tokens.at(AAF_TOKEN_CHANNELS_PER_FRAME)) ||
                    !aaf_samples_per_frame_from_str(tokens.at(AAF_TOKEN_SAMPLES_PER_FRAME)))
                    return;
                
                uint64_t val = uint64_t( (uint64_t) m_subtype << IEEE1722_FORMAT_SUBTYPE_SHIFT) |
                                         ((uint64_t) m_nsr_value << AAF_NSR_SHIFT) |
                                         ((uint64_t) m_packetization_type << AAF_TYPE_SHIFT) |
                                         ((uint64_t) m_bit_depth << AAF_PCM_BIT_DEPTH_SHIFT) |
                                         ((uint64_t) m_channels_per_frame << AAF_PCM_CHANNELS_PER_FRAME_SHIFT) |
                                         ((uint64_t) m_samples_per_frame << AAF_PCM_SAMPLES_PER_FRAME_SHIFT);
                
                m_format_value = val;
            }
        }
    }
    
    void aaf_format::decode_aaf_pcm_fields()
    {
        m_bit_depth = (m_format_value >> AAF_PCM_BIT_DEPTH_SHIFT) & AAF_PCM_BIT_DEPTH_MASK;
        m_channels_per_frame = (m_format_value >> AAF_PCM_CHANNELS_PER_FRAME_SHIFT) & AAF_PCM_CHANNELS_PER_FRAME_MASK;
        m_samples_per_frame = (m_format_value >> AAF_PCM_SAMPLES_PER_FRAME_SHIFT) & AAF_PCM_SAMPLES_PER_FRAME_MASK;
    }
    
    bool aaf_format::aaf_packetization_type_from_str(std::string packetization_type)
    {
        bool packetization_type_found = false;
        for (auto it = aaf_packetization_types.begin(); it != aaf_packetization_types.end(); ++it)
        {
            if (it->second == packetization_type)
            {
                m_packetization_type = it->first;
                packetization_type_found = true;
            }
        }
        
        return packetization_type_found;
    }
    
    bool aaf_format::aaf_nsr_from_str(std::string nsr)
    {
        bool nsr_value_found = false;
        for (auto it = aaf_nsr_values.begin(); it != aaf_nsr_values.end(); ++it)
        {
            if (it->second == nsr)
            {
                m_nsr_value = it->first;
                nsr_value_found = true;
            }
        }
        
        return nsr_value_found;
    }
    
    bool aaf_format::aaf_bit_depth_from_str(std::string bit_depth)
    {
        std::string bit_depth_str = bit_depth.substr(0, bit_depth.find("-BIT"));
        if (bit_depth_str.empty() || !isdigit(bit_depth_str[0]))
            return false;

        unsigned int bit_depth_val = (unsigned int) std::stoul(bit_depth_str, NULL, 0);
        if (!bit_depth_val)
            return false;
        
        m_bit_depth = bit_depth_val;
        return true;
    }
    
    bool aaf_format::aaf_channels_per_frame_from_str(std::string channels)
    {
        std::string channel_count_str = channels.substr(0, channels.find("CH"));
        if (channel_count_str.empty() || !isdigit(channel_count_str[0]))
            return false;
        
        unsigned int channel_count_val = (unsigned int) std::stoul(channel_count_str, NULL, 0);
        if (!channel_count_val)
            return false;
        
        m_channels_per_frame = channel_count_val;
        return true;
    }
    
    bool aaf_format::aaf_samples_per_frame_from_str(std::string samples)
    {
        std::string samples_str = samples.substr(0, samples.find("-SAMPLES"));
        if (samples_str.empty() || !isdigit(samples_str[0]))
            return false;
        
        unsigned int samples_val = (unsigned int) std::stoul(samples_str, NULL, 0);
        if (!samples_val)
            return false;
        
        m_samples_per_frame = samples_val;
        return true;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    static const std::map<unsigned int, std::string> iec61883_types =
    {
        { iec_61883_iidc_format::IEC_61883_4, "IEC61883-4" }, // IEC 61883-4 MPEG2 TS
        { iec_61883_iidc_format::IEC_61883_6, "IEC61883-6" }, // IEC 61883-6 Audio/Music
        { iec_61883_iidc_format::IEC_61883_7, "IEC61883-7" }, // IEC 61883-7 ITU-R BO.1294 System B
        { iec_61883_iidc_format::IEC_61883_8, "IEC61883-8" }, // IEC 61883-8 BT.601 Video
    };
    
    static const std::map<unsigned int, std::string> iec61883_packetization_types =
    {
        { iec_61883_iidc_format::FIXED_32BIT, "FIXED-32BIT" }, // 32-bit fixed-point packetization
        { iec_61883_iidc_format::FLOAT_32BIT, "FLOAT-32BIT" }, // 32-bit floating-point packetization
        { iec_61883_iidc_format::AM824, "AM824" },             // AM824 packetization
    };
    
    static const std::map<unsigned int, std::string> iec61883_fdf_sfc_values =
    {
        { iec_61883_iidc_format::FDF_SFC_44_1KHZ, "44.1KHZ" },
        { iec_61883_iidc_format::FDF_SFC_48KHZ, "48KHZ" },
        { iec_61883_iidc_format::FDF_SFC_88_2KHZ, "88.2KHZ" },
        { iec_61883_iidc_format::FDF_SFC_96KHZ, "96KHZ" },
        { iec_61883_iidc_format::FDF_SFC_176_4KHZ, "176.4KHZ" },
        { iec_61883_iidc_format::FDF_SFC_192KHZ, "192KHZ" }
    };
    
    iec_61883_iidc_format::iec_61883_iidc_format(uint64_t format_value) : ieee1722_stream_format(format_value)
    {
        m_sf = (m_format_value >> IEC61883_SF_SHIFT) & IEC61883_SF_MASK;
        if (m_sf) // IEC 61883-4, IEC 61883-6, IEC 61883-8
            decode_iec_61883_type();
        else                                                          // IIDC
            decode_iidc_format();
        
        to_string();
    }
    
    unsigned int iec_61883_iidc_format::sample_rate()
    {
        switch (m_fdf_sfc_value)
        {
        case FDF_SFC_44_1KHZ:
            return 44100;
        case FDF_SFC_48KHZ:
            return 48000;
        case FDF_SFC_88_2KHZ:
            return 88200;
        case FDF_SFC_96KHZ:
            return 96000;
        case FDF_SFC_176_4KHZ:
            return 176400;
        case FDF_SFC_192KHZ:
            return 192000;
        }
        
        return 0;
    }
    
    void iec_61883_iidc_format::to_string()
    {
        std::stringstream ss;
        if (m_sf) // IEC 61883-4, IEC 61883-6, IEC 61883-8
        {
            auto it = iec61883_types.find(m_iec61883_type);
            if (it != iec61883_types.end())
            {
                ss << it->second << IEEE1722_FORMAT_STR_DELIM;
                it = iec61883_packetization_types.find(m_packetization_type_value);
                if (it != iec61883_packetization_types.end())
                {
                    ss << it->second << IEEE1722_FORMAT_STR_DELIM;
                    if (m_packetization_type_value == AM824)
                    {
                        // FIX ME: Assume all IEC61883-6 AM824 formats
                        // have multi-bit linear audio (MBLA) quadlets.
                        ss << "MBLA" << IEEE1722_FORMAT_STR_DELIM;
                    }

                    it = iec61883_fdf_sfc_values.find(m_fdf_sfc_value);
                    if (it != iec61883_fdf_sfc_values.end())
                    {
                        ss << it->second << IEEE1722_FORMAT_STR_DELIM;
                        ss << std::to_string(m_dbs) << "CH";
                        m_format_name = ss.str();
                        ieee1722_format_names.insert(m_format_name);
                    }
                }
            }
        }
    }
    
    void iec_61883_iidc_format::to_val()
    {
        std::string s(m_format_name);
        std::vector<std::string> tokens;
        
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(IEEE1722_FORMAT_STR_DELIM)) != std::string::npos) {
            token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + 1);
        }
        tokens.push_back(s);
        
        // SFC token may have been split, e.g. 176_4KHZ -> 176 4KHZ
        if (tokens.size() == 6)
        {
            tokens.at(3) += IEEE1722_FORMAT_STR_DELIM + tokens.at(4);
            tokens.erase(tokens.begin() + 4);
        }
        
        if (tokens.size() != IEC61883_EXPECTED_TOKEN_COUNT)
            return;
        
        if (!iec_61883_type_from_str(tokens.at(IEC61883_6_AM824_TOKEN_TYPE)))
            return;
            
        switch (m_iec61883_type)
        {
        case IEC_61883_6:
        {
            m_sf = 1;
            m_nonblocking = 1; // assume non-blocking
            
            if (!iec_61883_packetization_type_from_str(tokens.at(IEC61883_6_AM824_TOKEN_PACKETIZATION)))
                return;
            
            switch (m_packetization_type_value)
            {
            case AM824:
            {
                if (!iec_61883_sfc_from_str(tokens.at(IEC61883_6_AM824_TOKEN_SFC)) ||
                    !iec_61883_dbs_from_str(tokens.at(IEC61883_6_AM824_TOKEN_BLOCK_COUNT)))
                    return;
                
                uint64_t val = uint64_t( (uint64_t) m_subtype << IEEE1722_FORMAT_SUBTYPE_SHIFT) |
                                         ((uint64_t) m_sf << IEC61883_SF_SHIFT) |
                                         ((uint64_t) m_iec61883_type << IEC61883_TYPE_SHIFT) |
                                         ((uint64_t) m_packetization_type_value << IEC61883_6_PACKETIZATION_SHIFT) |
                                         ((uint64_t) m_fdf_sfc_value << IEC61883_6_SFC_SHIFT) |
                                         ((uint64_t) m_dbs << IEC61883_6_BLOCK_COUNT_SHIFT) |
                                         ((uint64_t) m_nonblocking << IEC61883_6_NONBLOCKING_SHIFT) |
                                         ((uint64_t) m_dbs << IEC61883_6_AM824_MBLA_COUNT_SHIFT);

                m_format_value = val;
            }
            default:
                return;
            }
        }
        default:
            return;
        }
    }
    
    void iec_61883_iidc_format::decode_iec_61883_type()
    {
        m_iec61883_type = (m_format_value >> IEC61883_TYPE_SHIFT) & IEC61883_TYPE_MASK;
        switch (m_iec61883_type)
        {
        case IEC_61883_6:
            decode_iec_61883_common();
            decode_iec_61883_packetization_type();
            break;
        default:
            break;
        }
    }

    void iec_61883_iidc_format::decode_iec_61883_common()
    {
        m_fdf_sfc_value = (m_format_value >> IEC61883_6_SFC_SHIFT) & IEC61883_6_SFC_MASK;
        m_dbs = (m_format_value >> IEC61883_6_BLOCK_COUNT_SHIFT) & IEC61883_6_BLOCK_COUNT_MASK;
        m_blocking = (m_format_value >> IEC61883_6_BLOCKING_SHIFT) & IEC61883_6_BLOCKING_MASK;
        m_nonblocking = (m_format_value >> IEC61883_6_NONBLOCKING_SHIFT) & IEC61883_6_NONBLOCKING_MASK;
        m_upto = (m_format_value >> IEC61883_6_UPTO_SHIFT) & IEC61883_6_UPTO_MASK;
        m_synchronous = (m_format_value >> IEC61883_6_SYNCHRONOUS_SHIFT) & IEC61883_6_SYNCHRONOUS_MASK;
    }
    
    void iec_61883_iidc_format::decode_iec_61883_packetization_type()
    {
        m_packetization_type_value = (m_format_value >> IEC61883_6_PACKETIZATION_SHIFT) & IEC61883_6_PACKETIZATION_MASK;
        switch (m_packetization_type_value)
        {
        case AM824:
            decode_iec_61883_am824_fields();
            break;
        default:
            break;
        }
    }
    
    void iec_61883_iidc_format::decode_iec_61883_am824_fields()
    {
        m_iec60958_count = (m_format_value >> IEC61883_6_AM824_IEC60958_COUNT_SHIFT) & IEC61883_6_AM824_IEC60958_COUNT_MASK;
        m_mbla_count = (m_format_value >> IEC61883_6_AM824_MBLA_COUNT_SHIFT) & IEC61883_6_AM824_MBLA_COUNT_MASK;
        m_midi_count = (m_format_value >> IEC61883_6_AM824_MIDI_COUNT_SHIFT) & IEC61883_6_AM824_MIDI_COUNT_MASK;
        m_smpte_count = (m_format_value >> IEC61883_6_AM824_SMPTE_COUNT_SHIFT) & IEC61883_6_AM824_SMPTE_COUNT_MASK;
    }
    
    bool iec_61883_iidc_format::iec_61883_type_from_str(std::string type)
    {
        bool iec61883_type_found = false;
        for (auto it = iec61883_types.begin(); it != iec61883_types.end(); ++it)
        {
            if (it->second == type)
            {
                m_iec61883_type = it->first;
                iec61883_type_found = true;
            }
        }
        
        return iec61883_type_found;
    }
    
    bool iec_61883_iidc_format::iec_61883_packetization_type_from_str(std::string packetization_type)
    {
        bool iec61883_packetization_type_found = false;
        for (auto it = iec61883_packetization_types.begin(); it != iec61883_packetization_types.end(); ++it)
        {
            if (it->second == packetization_type)
            {
                m_packetization_type_value = it->first;
                iec61883_packetization_type_found = true;
            }
        }
        
        return iec61883_packetization_type_found;
    }
    
    bool iec_61883_iidc_format::iec_61883_sfc_from_str(std::string sfc)
    {
        bool iec61883_fdf_sfc_value_found = false;
        for (auto it = iec61883_fdf_sfc_values.begin(); it != iec61883_fdf_sfc_values.end(); ++it)
        {
            if (it->second == sfc)
            {
                m_fdf_sfc_value = it->first;
                iec61883_fdf_sfc_value_found = true;
            }
        }
        
        return iec61883_fdf_sfc_value_found;
    }
    
    bool iec_61883_iidc_format::iec_61883_dbs_from_str(std::string dbs)
    {
        std::string channel_count_str = dbs.substr(0, dbs.find("CH"));
        if (channel_count_str.empty() || !isdigit(channel_count_str[0]))
            return false;

        unsigned int channel_count_val = (unsigned int) std::stoul(channel_count_str, NULL, 0);
        if (!channel_count_val)
            return false;
        
        m_dbs = channel_count_val;
        return true;
    }
}
}
