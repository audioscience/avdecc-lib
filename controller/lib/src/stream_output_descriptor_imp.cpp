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
 * stream_output_descriptor_imp.cpp
 *
 * STREAM OUTPUT descriptor implementation
 */

#include <mutex>
#include <vector>

#include "util.h"
#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "end_station_imp.h"
#include "system_tx_queue.h"
#include "acmp_controller_state_machine.h"
#include "aecp_controller_state_machine.h"
#include "stream_output_descriptor_imp.h"

namespace avdecc_lib
{
stream_output_descriptor_imp::stream_output_descriptor_imp(end_station_imp * end_station_obj, const uint8_t * frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos)
{
    memset(&aem_cmd_set_stream_info_resp, 0, sizeof(struct jdksavdecc_aem_command_set_stream_info_response));
}

stream_output_descriptor_imp::~stream_output_descriptor_imp() {}

stream_output_descriptor_response * STDCALL stream_output_descriptor_imp::get_stream_output_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return resp = new stream_output_descriptor_response_imp(resp_ref->get_desc_buffer(),
                                                            resp_ref->get_desc_size(), resp_ref->get_desc_pos());
}

stream_output_get_stream_format_response * STDCALL stream_output_descriptor_imp::get_stream_output_get_stream_format_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return get_format_resp = new stream_output_get_stream_format_response_imp(resp_ref->get_buffer(),
                                                                              resp_ref->get_size(), resp_ref->get_pos());
}

stream_output_get_stream_info_response * STDCALL stream_output_descriptor_imp::get_stream_output_get_stream_info_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return get_info_resp = new stream_output_get_stream_info_response_imp(resp_ref->get_buffer(),
                                                                          resp_ref->get_size(), resp_ref->get_pos());
}

stream_output_get_tx_state_response * STDCALL stream_output_descriptor_imp::get_stream_output_get_tx_state_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return get_tx_state_resp = new stream_output_get_tx_state_response_imp(resp_ref->get_buffer(),
                                                                           resp_ref->get_size(), resp_ref->get_pos());
}

stream_output_get_tx_connection_response * STDCALL stream_output_descriptor_imp::get_stream_output_get_tx_connection_response()
{
    std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
    return get_tx_connection_resp = new stream_output_get_tx_connection_response_imp(resp_ref->get_buffer(),
                                                                                     resp_ref->get_size(), resp_ref->get_pos());
}

int STDCALL stream_output_descriptor_imp::send_set_stream_format_cmd(void * notification_id, uint64_t new_stream_format)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_set_stream_format aem_cmd_set_stream_format;
    ssize_t aem_cmd_set_stream_format_returned;
    memset(&aem_cmd_set_stream_format, 0, sizeof(aem_cmd_set_stream_format));

    /******************************************** AECP Common Data *********************************************/
    aem_cmd_set_stream_format.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_set_stream_format.sequence_id in AEM Controller State Machine
    aem_cmd_set_stream_format.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT;

    /**************************************** AECP Message Specific Data *************************************/
    aem_cmd_set_stream_format.descriptor_type = descriptor_type();
    aem_cmd_set_stream_format.descriptor_index = descriptor_index();
    jdksavdecc_uint64_write(new_stream_format, &aem_cmd_set_stream_format.stream_format, 0, sizeof(uint64_t));

    /******************************** Fill frame payload with AECP data and send the frame ***************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT_COMMAND_LEN);
    aem_cmd_set_stream_format_returned = jdksavdecc_aem_command_set_stream_format_write(&aem_cmd_set_stream_format,
                                                                                        cmd_frame.payload,
                                                                                        ETHER_HDR_SIZE,
                                                                                        sizeof(cmd_frame.payload));

    if (aem_cmd_set_stream_format_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_stream_format_write error\n");
        assert(aem_cmd_set_stream_format_returned >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}

int stream_output_descriptor_imp::proc_set_stream_format_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_set_stream_format_response aem_cmd_set_stream_format_resp;
    ssize_t aem_cmd_set_stream_format_resp_returned;
    uint32_t msg_type;
    bool u_field;

    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_set_stream_format_resp, 0, sizeof(struct jdksavdecc_aem_command_set_stream_format_response));

    aem_cmd_set_stream_format_resp_returned = jdksavdecc_aem_command_set_stream_format_response_read(&aem_cmd_set_stream_format_resp,
                                                                                                     frame,
                                                                                                     ETHER_HDR_SIZE,
                                                                                                     frame_len);

    if (aem_cmd_set_stream_format_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_stream_format_resp_read error\n");
        assert(aem_cmd_set_stream_format_resp_returned >= 0);
        return -1;
    }

    msg_type = aem_cmd_set_stream_format_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_set_stream_format_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_set_stream_format_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    if (status == AEM_STATUS_SUCCESS)
    {
        uint8_t * buffer = (uint8_t *)malloc(resp_ref->get_desc_size() * sizeof(uint8_t)); //fetch current desc frame
        memcpy(buffer, resp_ref->get_desc_buffer(), resp_ref->get_desc_size());
        jdksavdecc_descriptor_stream_set_current_format(aem_cmd_set_stream_format_resp.stream_format,
                                                        buffer, resp_ref->get_desc_pos()); //set stream format

        replace_desc_frame(buffer, resp_ref->get_desc_pos(), resp_ref->get_desc_size()); //replace frame
        free(buffer);
    }
    
    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_get_stream_format_cmd(void * notification_id)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_get_stream_format aem_cmd_get_stream_format;
    ssize_t aem_cmd_get_stream_format_returned;
    memset(&aem_cmd_get_stream_format, 0, sizeof(aem_cmd_get_stream_format));

    /******************************************** AECP Common Data *********************************************/
    aem_cmd_get_stream_format.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_get_stream_format.sequence_id in AEM Controller State Machine
    aem_cmd_get_stream_format.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT;

    /****************** AECP Message Specific Data *****************/
    aem_cmd_get_stream_format.descriptor_type = descriptor_type();
    aem_cmd_get_stream_format.descriptor_index = descriptor_index();

    /****************************** Fill frame payload with AECP data and send the frame **************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_COMMAND_LEN);
    aem_cmd_get_stream_format_returned = jdksavdecc_aem_command_get_stream_format_write(&aem_cmd_get_stream_format,
                                                                                        cmd_frame.payload,
                                                                                        ETHER_HDR_SIZE,
                                                                                        sizeof(cmd_frame.payload));

    if (aem_cmd_get_stream_format_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_format_write error\n");
        assert(aem_cmd_get_stream_format_returned >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}

int stream_output_descriptor_imp::proc_get_stream_format_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_get_stream_format_response aem_cmd_get_stream_format_resp;
    ssize_t aem_cmd_get_stream_format_resp_returned;
    uint32_t msg_type;
    bool u_field;

    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_get_stream_format_resp, 0, sizeof(jdksavdecc_aem_command_get_stream_format_response));

    aem_cmd_get_stream_format_resp_returned = jdksavdecc_aem_command_get_stream_format_response_read(&aem_cmd_get_stream_format_resp,
                                                                                                     frame,
                                                                                                     ETHER_HDR_SIZE,
                                                                                                     frame_len);

    if (aem_cmd_get_stream_format_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_format_resp_read error\n");
        assert(aem_cmd_get_stream_format_resp_returned >= 0);
        return -1;
    }

    replace_frame(frame, ETHER_HDR_SIZE, frame_len);

    msg_type = aem_cmd_get_stream_format_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_get_stream_format_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_get_stream_format_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_set_stream_info_vlan_id_cmd(void * notification_id, uint16_t vlan_id)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_set_stream_info cmd;
    ssize_t write_return;

    memset(&cmd, 0, sizeof(cmd));

    /******************************************** AECP Common Data *******************************************/
    cmd.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_start_streaming.sequence_id in AEM Controller State Machine
    cmd.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO;

    /****************** AECP Message Specific Data ***************/
    cmd.descriptor_type = descriptor_type();
    cmd.descriptor_index = descriptor_index();
    cmd.aem_stream_info_flags = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_STREAM_VLAN_ID_VALID;
    cmd.stream_vlan_id = vlan_id;

    /************************** Fill frame payload with AECP data and send the frame ***************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_COMMAND_LEN);
    write_return = jdksavdecc_aem_command_set_stream_info_write(&cmd,
                                                                cmd_frame.payload,
                                                                ETHER_HDR_SIZE,
                                                                sizeof(cmd_frame.payload));

    if (write_return < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_start_streaming_write error\n");
        assert(write_return >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}
    
int STDCALL stream_output_descriptor_imp::send_set_stream_info_msrp_accumulated_latency_cmd(void * notification_id, uint32_t msrp_accumulated_latency)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_set_stream_info cmd;
    ssize_t write_return;
    
    memset(&cmd, 0, sizeof(cmd));
    
    /******************************************** AECP Common Data *******************************************/
    cmd.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_start_streaming.sequence_id in AEM Controller State Machine
    cmd.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO;
    
    /****************** AECP Message Specific Data ***************/
    cmd.descriptor_type = descriptor_type();
    cmd.descriptor_index = descriptor_index();
    cmd.aem_stream_info_flags = JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_FLAG_MSRP_ACC_LAT_VALID;
    cmd.msrp_accumulated_latency = msrp_accumulated_latency;
    
    /************************** Fill frame payload with AECP data and send the frame ***************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_COMMAND_LEN);
    write_return = jdksavdecc_aem_command_set_stream_info_write(&cmd,
                                                                cmd_frame.payload,
                                                                ETHER_HDR_SIZE,
                                                                sizeof(cmd_frame.payload));
    
    if (write_return < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_start_streaming_write error\n");
        assert(write_return >= 0);
        return -1;
    }
    
    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO_COMMAND_LEN -
                                                       JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
    
    return 0;
}

int stream_output_descriptor_imp::proc_set_stream_info_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    ssize_t read_status;
    uint32_t msg_type;
    bool u_field;

    memcpy(cmd_frame.payload, frame, frame_len);
    read_status = jdksavdecc_aem_command_set_stream_info_response_read(&aem_cmd_set_stream_info_resp,
                                                                       frame,
                                                                       ETHER_HDR_SIZE,
                                                                       frame_len);

    if (read_status < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_set_stream_info_resp_read error");
        assert(read_status >= 0);
        return -1;
    }

    replace_frame(frame, ETHER_HDR_SIZE, frame_len);

    msg_type = aem_cmd_set_stream_info_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_set_stream_info_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_set_stream_info_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_get_stream_info_cmd(void * notification_id)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_get_stream_info aem_cmd_get_stream_info;
    ssize_t aem_cmd_get_stream_info_returned;

    memset(&aem_cmd_get_stream_info, 0, sizeof(aem_cmd_get_stream_info));

    /******************************************** AECP Common Data *******************************************/
    aem_cmd_get_stream_info.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_get_stream_info.sequence_id in AEM Controller State Machine
    aem_cmd_get_stream_info.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO;

    /****************** AECP Message Specific Data ***************/
    aem_cmd_get_stream_info.descriptor_type = descriptor_type();
    aem_cmd_get_stream_info.descriptor_index = descriptor_index();

    /************************* Fill frame payload with AECP data and send the frame ***************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_COMMAND_LEN);
    aem_cmd_get_stream_info_returned = jdksavdecc_aem_command_get_stream_info_write(&aem_cmd_get_stream_info,
                                                                                    cmd_frame.payload,
                                                                                    ETHER_HDR_SIZE,
                                                                                    sizeof(cmd_frame.payload));

    if (aem_cmd_get_stream_info_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_info_write error\n");
        assert(aem_cmd_get_stream_info_returned >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}

int stream_output_descriptor_imp::proc_get_stream_info_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_get_stream_info_response aem_cmd_get_stream_info_resp;
    ssize_t aem_cmd_get_stream_info_resp_returned;
    uint32_t msg_type;
    bool u_field;

    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_get_stream_info_resp, 0, sizeof(jdksavdecc_aem_command_get_stream_info_response));

    aem_cmd_get_stream_info_resp_returned = jdksavdecc_aem_command_get_stream_info_response_read(&aem_cmd_get_stream_info_resp,
                                                                                                 frame,
                                                                                                 ETHER_HDR_SIZE,
                                                                                                 frame_len);

    if (aem_cmd_get_stream_info_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_stream_info_resp_read error");
        assert(aem_cmd_get_stream_info_resp_returned >= 0);
        return -1;
    }

    replace_frame(frame, ETHER_HDR_SIZE, frame_len);

    msg_type = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_get_stream_info_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_get_stream_info_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_start_streaming_cmd(void * notification_id)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_start_streaming aem_cmd_start_streaming;
    ssize_t aem_cmd_start_streaming_returned;

    memset(&aem_cmd_start_streaming, 0, sizeof(aem_cmd_start_streaming));
    /******************************************** AECP Common Data *******************************************/
    aem_cmd_start_streaming.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_start_streaming.sequence_id in AEM Controller State Machine
    aem_cmd_start_streaming.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_START_STREAMING;

    /****************** AECP Message Specific Data ***************/
    aem_cmd_start_streaming.descriptor_type = descriptor_type();
    aem_cmd_start_streaming.descriptor_index = descriptor_index();

    /************************** Fill frame payload with AECP data and send the frame ***************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_START_STREAMING_COMMAND_LEN);
    aem_cmd_start_streaming_returned = jdksavdecc_aem_command_start_streaming_write(&aem_cmd_start_streaming,
                                                                                    cmd_frame.payload,
                                                                                    ETHER_HDR_SIZE,
                                                                                    sizeof(cmd_frame.payload));

    if (aem_cmd_start_streaming_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_start_streaming_write error\n");
        assert(aem_cmd_start_streaming_returned >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_START_STREAMING_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}

int stream_output_descriptor_imp::proc_start_streaming_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_start_streaming_response aem_cmd_start_streaming_resp;
    ssize_t aem_cmd_start_streaming_resp_returned;
    uint32_t msg_type;
    bool u_field;

    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_start_streaming_resp, 0, sizeof(aem_cmd_start_streaming_resp));

    aem_cmd_start_streaming_resp_returned = jdksavdecc_aem_command_start_streaming_response_read(&aem_cmd_start_streaming_resp,
                                                                                                 frame,
                                                                                                 ETHER_HDR_SIZE,
                                                                                                 frame_len);

    if (aem_cmd_start_streaming_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_start_streaming_resp_read error");
        assert(aem_cmd_start_streaming_resp_returned >= 0);
        return -1;
    }

    msg_type = aem_cmd_start_streaming_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_start_streaming_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_start_streaming_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_stop_streaming_cmd(void * notification_id)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_stop_streaming aem_cmd_stop_streaming;
    ssize_t aem_cmd_stop_streaming_returned;

    memset(&aem_cmd_stop_streaming, 0, sizeof(aem_cmd_stop_streaming));
    /******************************************* AECP Common Data *******************************************/
    aem_cmd_stop_streaming.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_stop_streaming.sequence_id in AEM Controller State Machine
    aem_cmd_stop_streaming.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_STOP_STREAMING;

    /***************** AECP Message Specific Data ***************/
    aem_cmd_stop_streaming.descriptor_type = descriptor_type();
    aem_cmd_stop_streaming.descriptor_index = descriptor_index();

    /************************** Fill frame payload with AECP data and send the frame *************************/
    aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_STOP_STREAMING_COMMAND_LEN);
    aem_cmd_stop_streaming_returned = jdksavdecc_aem_command_stop_streaming_write(&aem_cmd_stop_streaming,
                                                                                  cmd_frame.payload,
                                                                                  ETHER_HDR_SIZE,
                                                                                  sizeof(cmd_frame.payload));

    if (aem_cmd_stop_streaming_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_stop_streaming_write error\n");
        assert(aem_cmd_stop_streaming_returned >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       base_end_station_imp_ref->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_STOP_STREAMING_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}

int stream_output_descriptor_imp::proc_stop_streaming_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_stop_streaming_response aem_cmd_stop_streaming_resp;
    ssize_t aem_cmd_stop_streaming_resp_returned;
    uint32_t msg_type;
    bool u_field;

    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_stop_streaming_resp, 0, sizeof(aem_cmd_stop_streaming_resp));

    aem_cmd_stop_streaming_resp_returned = jdksavdecc_aem_command_stop_streaming_response_read(&aem_cmd_stop_streaming_resp,
                                                                                               frame,
                                                                                               ETHER_HDR_SIZE,
                                                                                               frame_len);

    if (aem_cmd_stop_streaming_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_stop_streaming_resp_read error");
        assert(aem_cmd_stop_streaming_resp_returned >= 0);
        return -1;
    }

    msg_type = aem_cmd_stop_streaming_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_stop_streaming_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_stop_streaming_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_get_tx_state_cmd(void * notification_id)
{
    entity_descriptor_response * entity_resp_ref = base_end_station_imp_ref->get_entity_desc_by_index(0)->get_entity_response();
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_acmpdu acmp_cmd_get_tx_state;
    ssize_t acmp_cmd_get_tx_state_returned;
    uint64_t talker_entity_id = entity_resp_ref->entity_id();

    /******************************************* ACMP Common Data ******************************************/
    acmp_cmd_get_tx_state.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    jdksavdecc_uint64_write(talker_entity_id, &acmp_cmd_get_tx_state.talker_entity_id, 0, sizeof(uint64_t));
    jdksavdecc_eui64_init(&acmp_cmd_get_tx_state.listener_entity_id);
    acmp_cmd_get_tx_state.talker_unique_id = descriptor_index();
    acmp_cmd_get_tx_state.listener_unique_id = 0;
    jdksavdecc_eui48_init(&acmp_cmd_get_tx_state.stream_dest_mac);
    acmp_cmd_get_tx_state.connection_count = 0;
    // Fill acmp_cmd_get_tx_state.sequence_id in AEM Controller State Machine
    acmp_cmd_get_tx_state.flags = 0;
    acmp_cmd_get_tx_state.stream_vlan_id = 0;

    /*************** Fill frame payload with AECP data and send the frame ****************/
    acmp_controller_state_machine_ref->ether_frame_init(&cmd_frame);
    acmp_cmd_get_tx_state_returned = jdksavdecc_acmpdu_write(&acmp_cmd_get_tx_state,
                                                             cmd_frame.payload,
                                                             ETHER_HDR_SIZE,
                                                             sizeof(cmd_frame.payload));

    if (acmp_cmd_get_tx_state_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "cmd_get_tx_state_write error\n");
        assert(acmp_cmd_get_tx_state_returned >= 0);
        return -1;
    }

    acmp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_COMMAND, &cmd_frame);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    delete entity_resp_ref;
    return 0;
}

int stream_output_descriptor_imp::proc_get_tx_state_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_acmpdu acmp_cmd_get_tx_state_resp;
    ssize_t acmp_cmd_get_tx_state_resp_returned;

    memcpy(cmd_frame.payload, frame, frame_len);
    acmp_cmd_get_tx_state_resp_returned = jdksavdecc_acmpdu_read(&acmp_cmd_get_tx_state_resp,
                                                                 frame,
                                                                 ETHER_HDR_SIZE,
                                                                 frame_len);

    if (acmp_cmd_get_tx_state_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "acmp_cmd_get_tx_state_read error");
        assert(acmp_cmd_get_tx_state_resp_returned >= 0);
        return -1;
    }

    replace_frame(frame, ETHER_HDR_SIZE, frame_len);

    status = acmp_cmd_get_tx_state_resp.header.status;

    acmp_controller_state_machine_ref->state_resp(notification_id, &cmd_frame);

    return 0;
}

int STDCALL stream_output_descriptor_imp::send_get_tx_connection_cmd(void * notification_id, uint64_t listener_entity_id, uint16_t listener_unique_id)
{
    entity_descriptor_response * entity_resp_ref = base_end_station_imp_ref->get_entity_desc_by_index(0)->get_entity_response();
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_acmpdu acmp_cmd_get_tx_connection;
    ssize_t acmp_cmd_get_tx_connection_returned;
    uint64_t talker_entity_id = entity_resp_ref->entity_id();

    /********************************************* ACMP Common Data *********************************************/
    acmp_cmd_get_tx_connection.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
    jdksavdecc_uint64_write(talker_entity_id, &acmp_cmd_get_tx_connection.talker_entity_id, 0, sizeof(uint64_t));
    jdksavdecc_uint64_write(listener_entity_id, &acmp_cmd_get_tx_connection.listener_entity_id, 0, sizeof(uint64_t));
    acmp_cmd_get_tx_connection.talker_unique_id = descriptor_index();
    acmp_cmd_get_tx_connection.listener_unique_id = listener_unique_id;
    jdksavdecc_eui48_init(&acmp_cmd_get_tx_connection.stream_dest_mac);
    acmp_cmd_get_tx_connection.connection_count = 0;
    // Fill acmp_cmd_get_tx_connection.sequence_id in AEM Controller State Machine
    acmp_cmd_get_tx_connection.flags = 0;
    acmp_cmd_get_tx_connection.stream_vlan_id = 0;

    /****************** Fill frame payload with AECP data and send the frame *****************/
    acmp_controller_state_machine_ref->ether_frame_init(&cmd_frame);
    acmp_cmd_get_tx_connection_returned = jdksavdecc_acmpdu_write(&acmp_cmd_get_tx_connection,
                                                                  cmd_frame.payload,
                                                                  ETHER_HDR_SIZE,
                                                                  sizeof(cmd_frame.payload));

    if (acmp_cmd_get_tx_connection_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "cmd_get_tx_connection_write error\n");
        assert(acmp_cmd_get_tx_connection_returned >= 0);
        return -1;
    }

    acmp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_COMMAND, &cmd_frame);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    delete entity_resp_ref;
    return 0;
}

int stream_output_descriptor_imp::proc_get_tx_connection_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_acmpdu acmp_cmd_get_tx_connection_resp;
    ssize_t acmp_cmd_get_tx_connection_resp_returned;

    memcpy(cmd_frame.payload, frame, frame_len);
    acmp_cmd_get_tx_connection_resp_returned = jdksavdecc_acmpdu_read(&acmp_cmd_get_tx_connection_resp,
                                                                      frame,
                                                                      ETHER_HDR_SIZE,
                                                                      frame_len);

    if (acmp_cmd_get_tx_connection_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "acmp_cmd_get_tx_connection_read error");
        assert(acmp_cmd_get_tx_connection_resp_returned >= 0);
        return -1;
    }

    replace_frame(frame, ETHER_HDR_SIZE, frame_len);

    status = acmp_cmd_get_tx_connection_resp.header.status;

    acmp_controller_state_machine_ref->state_resp(notification_id, &cmd_frame);

    return 0;
}
}
