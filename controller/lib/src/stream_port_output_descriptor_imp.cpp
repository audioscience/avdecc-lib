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
 * stream_port_output_descriptor_imp.cpp
 *
 * Stream Port Output descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "adp.h"
#include "system_tx_queue.h"
#include "aecp_controller_state_machine.h"
#include "end_station_imp.h"
#include "stream_port_output_descriptor_imp.h"

namespace avdecc_lib
{
    stream_port_output_descriptor_imp::stream_port_output_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos) {}

    stream_port_output_descriptor_imp::~stream_port_output_descriptor_imp() {}

    stream_port_output_descriptor_response * STDCALL stream_port_output_descriptor_imp::get_stream_port_output_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return resp = new stream_port_output_descriptor_response_imp(resp_ref->get_desc_buffer(),
                                                                     resp_ref->get_desc_size(), resp_ref->get_desc_pos());
    }
    
    stream_port_output_get_audio_map_response * STDCALL stream_port_output_descriptor_imp::get_stream_port_output_audio_map_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return audio_map_resp = new stream_port_output_get_audio_map_response_imp(resp_ref->get_buffer(),
                                                                                 resp_ref->get_size(), resp_ref->get_pos());
    }
    
    int stream_port_output_descriptor_imp::store_pending_map(struct audio_map_mapping &map)
    {
        if (pending_maps.size() >= MAX_MAPPINGS_PER_FRAME) //max number of pending maps for a frame
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Too many pending audio maps.\n");
        }
        else
        {
            pending_maps.push_back(map);
            std::cout << "new map pending, total maps pending: " << pending_maps.size() << std::endl;
        }
        
        return 0;
    }

    size_t stream_port_output_descriptor_imp::get_number_of_pending_maps()
    {
        return pending_maps.size();
    }
    
    int stream_port_output_descriptor_imp::get_pending_maps(size_t index, struct audio_map_mapping &map)
    {
        if (index >= pending_maps.size())
            return -1;
        
        map = pending_maps.at(index);
        return 0;
    }
    
    int stream_port_output_descriptor_imp::clear_pending_maps()
    {
        if(pending_maps.size() == 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "No pending audio mappings.\n");
        }
        else
        {
            pending_maps.clear();
        }
        return 0;
    }

    int STDCALL stream_port_output_descriptor_imp::send_get_audio_map_cmd(void *notification_id, uint16_t mapping_index)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_audio_map aem_cmd_get_audio_map;
        ssize_t aem_cmd_get_audio_map_returned;
        memset(&aem_cmd_get_audio_map,0,sizeof(aem_cmd_get_audio_map));
        
        /******************************************** AECP Common Data *********************************************/
        aem_cmd_get_audio_map.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
        aem_cmd_get_audio_map.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_AUDIO_MAP;
        
        /****************** AECP Message Specific Data *****************/
        aem_cmd_get_audio_map.descriptor_type = descriptor_type();
        aem_cmd_get_audio_map.descriptor_index = descriptor_index();
        aem_cmd_get_audio_map.map_index = mapping_index;
        
        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_AUDIO_MAP_COMMAND_LEN);
        aem_cmd_get_audio_map_returned = jdksavdecc_aem_command_get_audio_map_write(&aem_cmd_get_audio_map,
                                                                                    cmd_frame.payload,
                                                                                    ETHER_HDR_SIZE,
                                                                                    sizeof(cmd_frame.payload));
        if(aem_cmd_get_audio_map_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_audio_map_write error\n");
            assert(aem_cmd_get_audio_map_returned >= 0);
            return -1;
        }
        
        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           base_end_station_imp_ref->entity_id(),
                                                           JDKSAVDECC_AEM_COMMAND_GET_AUDIO_MAP_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        
        return 0;
    }
    
    int stream_port_output_descriptor_imp::proc_get_audio_map_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_audio_map_response aem_cmd_get_audio_map_resp;
        ssize_t aem_cmd_get_audio_map_resp_returned;
        uint32_t msg_type;
        bool u_field;
        
        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&aem_cmd_get_audio_map_resp, 0, sizeof(jdksavdecc_aem_command_get_audio_map_response));
        
        aem_cmd_get_audio_map_resp_returned = jdksavdecc_aem_command_get_audio_map_response_read(&aem_cmd_get_audio_map_resp,
                                                                                                 frame,
                                                                                                 ETHER_HDR_SIZE,
                                                                                                 frame_len);
        
        if(aem_cmd_get_audio_map_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_audio_map_resp_read error\n");
            assert(aem_cmd_get_audio_map_resp_returned >= 0);
            return -1;
        }
        
        replace_frame(frame, ETHER_HDR_SIZE, frame_len);
        
        msg_type = aem_cmd_get_audio_map_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_get_audio_map_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_get_audio_map_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
        
        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        
        return 0;
    }
    
    int STDCALL stream_port_output_descriptor_imp::send_add_audio_mappings_cmd(void *notification_id)
    {
        int i = 0;
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_add_audio_mappings aem_cmd_add_audio_mappings;
        ssize_t aem_cmd_add_audio_mappings_returned;
        memset(&aem_cmd_add_audio_mappings,0,sizeof(aem_cmd_add_audio_mappings));
        
        /******************************************** AECP Common Data *********************************************/
        aem_cmd_add_audio_mappings.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
        aem_cmd_add_audio_mappings.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS;
        
        /****************** AECP Message Specific Data *****************/
        aem_cmd_add_audio_mappings.descriptor_type = descriptor_type();
        aem_cmd_add_audio_mappings.descriptor_index = descriptor_index();
        aem_cmd_add_audio_mappings.number_of_mappings = pending_maps.size();
        
        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS_COMMAND_LEN +
                                                            pending_maps.size() * JDKSAVDECC_AUDIO_MAPPING_LEN);
        aem_cmd_add_audio_mappings_returned = jdksavdecc_aem_command_add_audio_mappings_write(&aem_cmd_add_audio_mappings,
                                                                                              cmd_frame.payload,
                                                                                              ETHER_HDR_SIZE,
                                                                                              sizeof(cmd_frame.payload));
        if(aem_cmd_add_audio_mappings_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_add_audio_mappings_write error\n");
            assert(aem_cmd_add_audio_mappings_returned >= 0);
            return -1;
        }
        
        size_t num_pending_maps = pending_maps.size();
        std::vector<struct audio_map_mapping>::iterator it = pending_maps.begin();
        while(it != pending_maps.end())
        {
            if(i >= num_pending_maps)
                break;
            
            struct avdecc_lib::audio_map_mapping map = *it;
            
            jdksavdecc_uint16_set(map.stream_index, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS + JDKSAVDECC_AUDIO_MAPPING_LEN * i +
                                  JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_STREAM_INDEX);
            jdksavdecc_uint16_set(map.stream_channel, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS +
                                  (JDKSAVDECC_AUDIO_MAPPING_LEN * i + JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_STREAM_CHANNEL));
            jdksavdecc_uint16_set(map.cluster_offset, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS +
                                  (JDKSAVDECC_AUDIO_MAPPING_LEN * i + JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_CLUSTER_OFFSET));
            jdksavdecc_uint16_set(map.cluster_channel, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS +
                                  (JDKSAVDECC_AUDIO_MAPPING_LEN * i + JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_CLUSTER_CHANNEL));
            
            pending_maps.erase(it);
            i++;
        }
        
        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           base_end_station_imp_ref->entity_id(),
                                                           JDKSAVDECC_AEM_COMMAND_ADD_AUDIO_MAPPINGS_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        
        if(num_pending_maps > AEM_MAX_MAPS)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    int stream_port_output_descriptor_imp::proc_add_audio_mappings_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_add_audio_mappings_response aem_cmd_add_audio_mappings_resp;
        ssize_t aem_cmd_add_audio_mappings_resp_returned;
        uint32_t msg_type;
        bool u_field;
        
        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&aem_cmd_add_audio_mappings_resp, 0, sizeof(jdksavdecc_aem_command_add_audio_mappings_response));
        
        aem_cmd_add_audio_mappings_resp_returned = jdksavdecc_aem_command_add_audio_mappings_response_read(&aem_cmd_add_audio_mappings_resp,
                                                                                                           frame,
                                                                                                           ETHER_HDR_SIZE,
                                                                                                           frame_len);
        
        if(aem_cmd_add_audio_mappings_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_add_audio_mappings_resp_read error\n");
            assert(aem_cmd_add_audio_mappings_resp_returned >= 0);
            return -1;
        }
        
        msg_type = aem_cmd_add_audio_mappings_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_add_audio_mappings_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_add_audio_mappings_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        
        return 0;
    }
    
    int STDCALL stream_port_output_descriptor_imp::send_remove_audio_mappings_cmd(void *notification_id)
    {
        int i = 0;
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_remove_audio_mappings aem_cmd_remove_audio_mappings;
        ssize_t aem_cmd_remove_audio_mappings_returned;
        memset(&aem_cmd_remove_audio_mappings,0,sizeof(aem_cmd_remove_audio_mappings));
        
        /******************************************** AECP Common Data *********************************************/
        aem_cmd_remove_audio_mappings.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_sampling_rate.sequence_id in AEM Controller State Machine
        aem_cmd_remove_audio_mappings.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS;
        
        /****************** AECP Message Specific Data *****************/
        aem_cmd_remove_audio_mappings.descriptor_type = descriptor_type();
        aem_cmd_remove_audio_mappings.descriptor_index = descriptor_index();
        aem_cmd_remove_audio_mappings.number_of_mappings = pending_maps.size();
        
        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS_COMMAND_LEN +
                                                            pending_maps.size() * JDKSAVDECC_AUDIO_MAPPING_LEN);
        aem_cmd_remove_audio_mappings_returned = jdksavdecc_aem_command_remove_audio_mappings_write(&aem_cmd_remove_audio_mappings,
                                                                                                    cmd_frame.payload,
                                                                                                    ETHER_HDR_SIZE,
                                                                                                    sizeof(cmd_frame.payload));
        if(aem_cmd_remove_audio_mappings_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_remove_audio_mappings_write error\n");
            assert(aem_cmd_remove_audio_mappings_returned >= 0);
            return -1;
        }
        
        size_t num_pending_maps = pending_maps.size();
        std::vector<struct audio_map_mapping>::iterator it = pending_maps.begin();
        while(it != pending_maps.end())
        {
            if(i >= num_pending_maps)
                break;
            
            struct avdecc_lib::audio_map_mapping map = *it;
            
            jdksavdecc_uint16_set(map.stream_index, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS + JDKSAVDECC_AUDIO_MAPPING_LEN * i +
                                  JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_STREAM_INDEX);
            jdksavdecc_uint16_set(map.stream_channel, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS +
                                  (JDKSAVDECC_AUDIO_MAPPING_LEN * i + JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_STREAM_CHANNEL));
            jdksavdecc_uint16_set(map.cluster_offset, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS +
                                  (JDKSAVDECC_AUDIO_MAPPING_LEN * i + JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_CLUSTER_OFFSET));
            jdksavdecc_uint16_set(map.cluster_channel, cmd_frame.payload, ETHER_HDR_SIZE +
                                  JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS_COMMAND_OFFSET_MAPPINGS +
                                  (JDKSAVDECC_AUDIO_MAPPING_LEN * i + JDKSAVDECC_AUDIO_MAPPING_OFFSET_MAPPING_CLUSTER_CHANNEL));
            
            pending_maps.erase(it);
            i++;
        }
        
        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           base_end_station_imp_ref->entity_id(),
                                                           JDKSAVDECC_AEM_COMMAND_REMOVE_AUDIO_MAPPINGS_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        
        if(num_pending_maps > AEM_MAX_MAPS)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    int stream_port_output_descriptor_imp::proc_remove_audio_mappings_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_remove_audio_mappings_response aem_cmd_remove_audio_mappings_resp;
        ssize_t aem_cmd_remove_audio_mappings_resp_returned;
        uint32_t msg_type;
        bool u_field;
        
        memcpy(cmd_frame.payload, frame, frame_len);
        memset(&aem_cmd_remove_audio_mappings_resp, 0, sizeof(jdksavdecc_aem_command_remove_audio_mappings_response));
        
        aem_cmd_remove_audio_mappings_resp_returned = jdksavdecc_aem_command_remove_audio_mappings_response_read(&aem_cmd_remove_audio_mappings_resp,
                                                                                                                 frame,
                                                                                                                 ETHER_HDR_SIZE,
                                                                                                                 frame_len);
        
        if(aem_cmd_remove_audio_mappings_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_remove_audio_mappings_resp_read error\n");
            assert(aem_cmd_remove_audio_mappings_resp_returned >= 0);
            return -1;
        }
        
        msg_type = aem_cmd_remove_audio_mappings_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_remove_audio_mappings_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_remove_audio_mappings_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

        return 0;
    }
}
