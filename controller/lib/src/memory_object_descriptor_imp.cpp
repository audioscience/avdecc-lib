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
 * memory_object_descriptor_imp.cpp
 *
 * MEMORY OBJECT descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "acmp_controller_state_machine.h"
#include "aecp_controller_state_machine.h"
#include "memory_object_descriptor_imp.h"

namespace avdecc_lib
{
    #define MEMORY_OBJECT_NUM_STRINGS 6
    const char *memory_object_type_str[] =
    {
        "FIRMWARE_IMAGE",
        "VENDOR_SPECIFIC",
        "CRASH_DUMP",
        "LOG_OBJECT",
        "AUTOSTART_SETTINGS",
        "SNAPSHOT_SETTINGS"
    };

    memory_object_descriptor_imp::memory_object_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_memory_object_read(&memory_object_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("memory_object_desc_read error");
        }
    }

    memory_object_descriptor_imp::~memory_object_descriptor_imp() {}

    uint16_t STDCALL memory_object_descriptor_imp::descriptor_type() const
    {
        assert(memory_object_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT);
        return memory_object_desc.descriptor_type;
    }

    uint16_t STDCALL memory_object_descriptor_imp::descriptor_index() const
    {
        return memory_object_desc.descriptor_index;
    }

    uint8_t * STDCALL memory_object_descriptor_imp::object_name()
    {
        return memory_object_desc.object_name.value;
    }

    uint16_t STDCALL memory_object_descriptor_imp::localized_description()
    {
        return memory_object_desc.localized_description;
    }

    uint16_t STDCALL memory_object_descriptor_imp::memory_object_type()
    {
        return memory_object_desc.memory_object_type;
    }

    uint16_t STDCALL memory_object_descriptor_imp::target_descriptor_type()
    {
        return memory_object_desc.target_descriptor_type;
    }

    uint16_t STDCALL memory_object_descriptor_imp::target_descriptor_index()
    {
        return memory_object_desc.target_descriptor_index;
    }

    uint64_t STDCALL memory_object_descriptor_imp::start_address()
    {
        return memory_object_desc.start_address;
    }

    uint64_t STDCALL memory_object_descriptor_imp::maximum_length()
    {
        return memory_object_desc.maximum_length;
    }

    uint64_t STDCALL memory_object_descriptor_imp::length()
    {
        return memory_object_desc.length;
    }

    const char * STDCALL memory_object_descriptor_imp::memory_object_type_to_str()
    {
        if(memory_object_desc.memory_object_type < MEMORY_OBJECT_NUM_STRINGS)
        {
            return memory_object_type_str[memory_object_desc.memory_object_type];
        }

        return "UNKNOWN";
    }

    int STDCALL memory_object_descriptor_imp::start_operation_cmd(void *notification_id, uint16_t operation_type)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_start_operation aem_cmd_start_operation;
        memset(&aem_cmd_start_operation,0,sizeof(aem_cmd_start_operation));

        if (operation_type > JDKSAVDECC_MEMORY_OBJECT_OPERATION_UPLOAD)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, " Invalid operation type %x on memory object\n", operation_type);
            return -1;
        }

        aem_cmd_start_operation.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        aem_cmd_start_operation.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_START_OPERATION;

        aem_cmd_start_operation.descriptor_type = descriptor_type();
        aem_cmd_start_operation.descriptor_index = descriptor_index();
        aem_cmd_start_operation.operation_id = 0;
        aem_cmd_start_operation.operation_type = operation_type;

        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_START_OPERATION_COMMAND_LEN);
        ssize_t aem_cmd_start_operation_returned = jdksavdecc_aem_command_start_operation_write(&aem_cmd_start_operation,
                                                                                                cmd_frame.payload,
                                                                                                ETHER_HDR_SIZE,
                                                                                                sizeof(cmd_frame.payload));

        if(aem_cmd_start_operation_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_command_start_operation_write error\n");
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                            &cmd_frame,
                                                            base_end_station_imp_ref->entity_id(),
                                                            JDKSAVDECC_AEM_COMMAND_START_OPERATION_COMMAND_LEN - 
                                                            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

        return 0;
    }

    int memory_object_descriptor_imp::proc_start_operation_resp(void *&notification_id,
                                                                const uint8_t *frame,
                                                                size_t frame_len,
                                                                int &status,
                                                                uint16_t &operation_id,
                                                                uint16_t &operation_type)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_start_operation_response aem_cmd_start_operation_resp;
        memset(&aem_cmd_start_operation_resp,0,sizeof(aem_cmd_start_operation_resp));

        memcpy(cmd_frame.payload, frame, frame_len);

        ssize_t aem_cmd_start_operation_resp_returned = jdksavdecc_aem_command_start_operation_response_read(&aem_cmd_start_operation_resp,
                                                                                                             frame,
                                                                                                             ETHER_HDR_SIZE,
                                                                                                             frame_len);

        if(aem_cmd_start_operation_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_command_start_operation_response_read error");
            return -1;
        }

        uint32_t msg_type = aem_cmd_start_operation_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_start_operation_resp.aem_header.aecpdu_header.header.status;
        bool u_field = aem_cmd_start_operation_resp.aem_header.command_type >> 15 & 0x01;
        operation_id = aem_cmd_start_operation_resp.operation_id;
        operation_type = aem_cmd_start_operation_resp.operation_type;

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

        return 0;
    }


    int memory_object_descriptor_imp::proc_operation_status_resp(void *&notification_id,
                                                                const uint8_t *frame,
                                                                size_t frame_len,
                                                                int &status,
                                                                uint16_t &operation_id,
                                                                bool &is_operation_id_valid)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_operation_status_response aem_operation_status_resp;
        memset(&aem_operation_status_resp,0,sizeof(aem_operation_status_resp));
        memcpy(cmd_frame.payload, frame, frame_len);

        ssize_t aem_operation_status_resp_returned = jdksavdecc_aem_command_operation_status_response_read(&aem_operation_status_resp,
                                                                                                             frame,
                                                                                                             ETHER_HDR_SIZE,
                                                                                                             frame_len);

        if(aem_operation_status_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_command_operation_status_response_read error");
            return -1;
        }

        uint32_t msg_type = aem_operation_status_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_operation_status_resp.aem_header.aecpdu_header.header.status;
        bool u_field = aem_operation_status_resp.aem_header.command_type >> 15 & 0x01;
        operation_id = aem_operation_status_resp.operation_id;
        uint16_t percent_complete = aem_operation_status_resp.percent_complete;

        if (operation_id) is_operation_id_valid = true;

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        aecp_controller_state_machine_ref->update_operation_for_rcvd_resp(notification_id, operation_id, percent_complete, &cmd_frame);

        return 0;
    }

}
