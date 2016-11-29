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
 * entity_descriptor_imp.cpp
 *
 * ENTITY descriptor implementation
 */

#include <mutex>
#include <vector>

#include "enumeration.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "entity_descriptor_imp.h"
#include "aecp_controller_state_machine.h"
#include "adp.h"
#include "system_tx_queue.h"

namespace avdecc_lib
{
entity_descriptor_imp::entity_descriptor_imp(end_station_imp * end_station_obj, const uint8_t * frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos) {}

entity_descriptor_imp::~entity_descriptor_imp()
{
    for (uint32_t config_vec_index = 0; config_vec_index < config_desc_vec.size(); config_vec_index++)
    {
        delete config_desc_vec.at(config_vec_index);
    }
}

uint16_t STDCALL entity_descriptor_imp::current_configuration()
{
    return jdksavdecc_descriptor_entity_get_current_configuration(resp_ref->get_buffer(), resp_ref->get_pos());
}

entity_descriptor_response * STDCALL entity_descriptor_imp::get_entity_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return resp = new entity_descriptor_response_imp(resp_ref->get_desc_buffer(),
                                                     resp_ref->get_desc_size(), resp_ref->get_desc_pos());
}
    
entity_descriptor_get_config_response * STDCALL entity_descriptor_imp::get_entity_get_config_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return get_config_resp = new entity_descriptor_get_config_response_imp(resp_ref->get_buffer(),
                                                                           resp_ref->get_size(), resp_ref->get_pos());
}

void entity_descriptor_imp::store_config_desc(end_station_imp * end_station_obj, const uint8_t * frame, ssize_t pos, size_t frame_len)
{
    config_desc_vec.push_back(new configuration_descriptor_imp(end_station_obj, frame, pos, frame_len));
}

size_t STDCALL entity_descriptor_imp::config_desc_count()
{
    return config_desc_vec.size();
}

configuration_descriptor * STDCALL entity_descriptor_imp::get_config_desc_by_index(uint16_t config_desc_index)
{
    bool is_valid = (config_desc_index < config_desc_vec.size());

    if (is_valid)
    {
        return config_desc_vec.at(config_desc_index);
    }
    else
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_config_desc_by_index error");
    }

    return NULL;
}

uint32_t STDCALL entity_descriptor_imp::acquire_entity_flags()
{
    return aem_cmd_acquire_entity_resp.aem_acquire_flags;
}

uint64_t STDCALL entity_descriptor_imp::acquire_entity_owner_entity_id()
{
    return jdksavdecc_uint64_get(&aem_cmd_acquire_entity_resp.owner_entity_id, 0);
}

uint32_t STDCALL entity_descriptor_imp::lock_entity_flags()
{
    return aem_cmd_lock_entity_resp.aem_lock_flags;
}

uint64_t STDCALL entity_descriptor_imp::lock_entity_locked_entity_id()
{
    return jdksavdecc_uint64_get(&aem_cmd_lock_entity_resp.locked_entity_id, 0);
}

int STDCALL entity_descriptor_imp::send_acquire_entity_cmd(void * notification_id, uint32_t acquire_entity_flag)
{
    return default_send_acquire_entity_cmd(this, notification_id, acquire_entity_flag);
}

int entity_descriptor_imp::proc_acquire_entity_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    return default_proc_acquire_entity_resp(aem_cmd_acquire_entity_resp, notification_id, frame, frame_len, status);
}

int STDCALL entity_descriptor_imp::send_lock_entity_cmd(void * notification_id, uint32_t lock_entity_flag)
{
    return default_send_lock_entity_cmd(this, notification_id, lock_entity_flag);
}

int STDCALL entity_descriptor_imp::send_reboot_cmd(void * notification_id)
{
    return default_send_reboot_cmd(this, notification_id);
}

int entity_descriptor_imp::proc_lock_entity_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    return default_proc_lock_entity_resp(aem_cmd_lock_entity_resp, notification_id, frame, frame_len, status);
}

int entity_descriptor_imp::proc_reboot_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    return default_proc_reboot_resp(aem_cmd_reboot_resp, notification_id, frame, frame_len, status);
}

int STDCALL entity_descriptor_imp::send_set_config_cmd(void * notification_id, uint16_t new_configuration_index)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_set_configuration aem_cmd_set_configuration;
    ssize_t aem_cmd_set_configuration_returned;
    memset(&aem_cmd_set_configuration, 0, sizeof(aem_cmd_set_configuration));
    
    /******************************************** AECP Common Data *********************************************/
    aem_cmd_set_configuration.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_set_configuration.sequence_id in AEM Controller State Machine
    aem_cmd_set_configuration.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION;
    
    /******************** AECP Message Specific Data *******************/
    aem_cmd_set_configuration.configuration_index = new_configuration_index;
    
    /******************************* Fill frame payload with AECP data and send the frame **************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION_COMMAND_LEN);
    aem_cmd_set_configuration_returned = jdksavdecc_aem_command_set_configuration_write(&aem_cmd_set_configuration,
                                                                                        cmd_frame.payload,
                                                                                        ETHER_HDR_SIZE,
                                                                                        sizeof(cmd_frame.payload));
    
    if (aem_cmd_set_configuration_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_configuration_write error\n");
        assert(aem_cmd_set_configuration_returned >= 0);
        return -1;
    }
    
    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_SET_CONFIGURATION_COMMAND_LEN -
                                                        JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
    
    return 0;
}

int entity_descriptor_imp::proc_set_config_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_set_configuration_response aem_cmd_set_configuration_resp;
    ssize_t aem_cmd_set_configuration_resp_returned;
    uint32_t msg_type;
    bool u_field;
    
    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_set_configuration_resp, 0, sizeof(struct jdksavdecc_aem_command_set_configuration_response));
    
    aem_cmd_set_configuration_resp_returned = jdksavdecc_aem_command_set_configuration_response_read(&aem_cmd_set_configuration_resp,
                                                                                                     frame,
                                                                                                     ETHER_HDR_SIZE,
                                                                                                     frame_len);
    
    if (aem_cmd_set_configuration_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_configuration_resp_read error\n");
        assert(aem_cmd_set_configuration_resp_returned >= 0);
        return -1;
    }
    
    msg_type = aem_cmd_set_configuration_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_set_configuration_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_set_configuration_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
    
    if (status == AEM_STATUS_SUCCESS)
    {
        uint8_t * buffer = (uint8_t *)malloc(resp_ref->get_desc_size() * sizeof(uint8_t)); // fetch current desc frame
        memcpy(buffer, resp_ref->get_desc_buffer(), resp_ref->get_desc_size());
        jdksavdecc_descriptor_entity_set_current_configuration(aem_cmd_set_configuration_resp.configuration_index, buffer, resp_ref->get_desc_pos());
        
        replace_desc_frame(buffer, resp_ref->get_desc_pos(), resp_ref->get_desc_size()); // replace frame
        free(buffer);
    }
    
    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
    
    return 0;
}

int STDCALL entity_descriptor_imp::send_get_config_cmd(void * notification_id)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_get_configuration aem_cmd_get_configuration;
    ssize_t aem_cmd_get_configuration_returned;
    memset(&aem_cmd_get_configuration, 0, sizeof(aem_cmd_get_configuration));
    
    /******************************************** AECP Common Data *********************************************/
    aem_cmd_get_configuration.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_get_configuration.sequence_id in AEM Controller State Machine
    aem_cmd_get_configuration.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION;
    
    /******************************* Fill frame payload with AECP data and send the frame **************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION_COMMAND_LEN);
    aem_cmd_get_configuration_returned = jdksavdecc_aem_command_get_configuration_write(&aem_cmd_get_configuration,
                                                                                        cmd_frame.payload,
                                                                                        ETHER_HDR_SIZE,
                                                                                        sizeof(cmd_frame.payload));
    
    if (aem_cmd_get_configuration_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_configuration_write error\n");
        assert(aem_cmd_get_configuration_returned >= 0);
        return -1;
    }
    
    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_GET_CONFIGURATION_COMMAND_LEN -
                                                            JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
    
    return 0;
}

int entity_descriptor_imp::proc_get_config_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_get_configuration_response aem_cmd_get_configuration_resp;
    ssize_t aem_cmd_get_configuration_resp_returned;
    uint32_t msg_type;
    bool u_field;
    
    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_get_configuration_resp, 0, sizeof(jdksavdecc_aem_command_get_configuration_response));
    
    aem_cmd_get_configuration_resp_returned = jdksavdecc_aem_command_get_configuration_response_read(&aem_cmd_get_configuration_resp,
                                                                                                     frame,
                                                                                                     ETHER_HDR_SIZE,
                                                                                                     frame_len);

    if (aem_cmd_get_configuration_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_configuration_resp_read error\n");
        assert(aem_cmd_get_configuration_resp_returned >= 0);
        return -1;
    }
    replace_frame(frame, ETHER_HDR_SIZE, frame_len);
    
    msg_type = aem_cmd_get_configuration_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_get_configuration_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_get_configuration_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
    
    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
    
    return 0;
}
}
