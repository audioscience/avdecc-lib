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
 * avb_interface_descriptor_imp.cpp
 *
 * AVB INTERFACE descriptor implementation
 */

#include "avdecc_error.h"
#include "enumeration.h"
#include "log_imp.h"
#include "util.h"
#include "end_station_imp.h"
#include "avb_interface_descriptor_imp.h"
#include "aecp_controller_state_machine.h"
#include "adp.h"
#include "system_tx_queue.h"

namespace avdecc_lib
{
    avb_interface_descriptor_imp::avb_interface_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj)
    {
        ssize_t ret = jdksavdecc_descriptor_avb_interface_read(&avb_interface_desc, frame, pos, frame_len);

        if (ret < 0)
        {
            throw avdecc_read_descriptor_error("avb_interface_desc_read error");
        }
        
        memset(&aem_cmd_get_counters_resp, 0, sizeof(struct jdksavdecc_aem_command_get_counters_response));
        memset(&counters_response, 0, sizeof(struct get_counters_response_with_block));
    }

    avb_interface_descriptor_imp::~avb_interface_descriptor_imp() {}

    uint16_t STDCALL avb_interface_descriptor_imp::descriptor_type() const
    {
        //assert(avb_interface_desc.descriptor_type == JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE);
        return avb_interface_desc.descriptor_type;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::descriptor_index() const
    {
        return avb_interface_desc.descriptor_index;
    }

    uint8_t * STDCALL avb_interface_descriptor_imp::object_name()
    {
        return avb_interface_desc.object_name.value;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::localized_description()
    {
        return avb_interface_desc.localized_description;
    }

    uint64_t STDCALL avb_interface_descriptor_imp::mac_addr()
    {
        uint64_t mac_addr;
        utility::convert_eui48_to_uint64(avb_interface_desc.mac_address.value, mac_addr);

        return mac_addr;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::interface_flags()
    {
        return avb_interface_desc.interface_flags;
    }

    uint64_t STDCALL avb_interface_descriptor_imp::clock_identity()
    {
        return jdksavdecc_uint64_get(&avb_interface_desc.clock_identity, 0);
    }

    uint8_t STDCALL avb_interface_descriptor_imp::priority1()
    {
        return avb_interface_desc.priority1;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::clock_class()
    {
        return avb_interface_desc.clock_class;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::offset_scaled_log_variance()
    {
        return avb_interface_desc.offset_scaled_log_variance;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::clock_accuracy()
    {
        return avb_interface_desc.clock_accuracy;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::priority2()
    {
        return avb_interface_desc.priority2;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::domain_number()
    {
        return avb_interface_desc.domain_number;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::log_sync_interval()
    {
        return avb_interface_desc.log_sync_interval;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::log_announce_interval()
    {
        return avb_interface_desc.log_announce_interval;
    }

    uint8_t STDCALL avb_interface_descriptor_imp::log_pdelay_interval()
    {
        return avb_interface_desc.log_pdelay_interval;
    }

    uint16_t STDCALL avb_interface_descriptor_imp::port_number()
    {
        return avb_interface_desc.port_number;
    }
    
    int STDCALL avb_interface_descriptor_imp::send_get_counters_cmd(void *notification_id)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_get_counters aem_cmd_get_counters;
        memset(&aem_cmd_get_counters,0,sizeof(aem_cmd_get_counters));
        ssize_t aem_cmd_get_counters_returned;
        
        /******************************************** AECP Common Data *********************************************/
        aem_cmd_get_counters.aem_header.aecpdu_header.controller_entity_id = base_end_station_imp_ref->get_adp()->get_controller_entity_id();
        // Fill aem_cmd_get_counters.sequence_id in AEM Controller State Machine
        aem_cmd_get_counters.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_GET_COUNTERS;
        
        /****************** AECP Message Specific Data *****************/
        aem_cmd_get_counters.descriptor_type = descriptor_type();
        aem_cmd_get_counters.descriptor_index = descriptor_index();
        
        /******************************* Fill frame payload with AECP data and send the frame **************************/
        aecp_controller_state_machine_ref->ether_frame_init(base_end_station_imp_ref->mac(), &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_COMMAND_LEN);
        aem_cmd_get_counters_returned = jdksavdecc_aem_command_get_counters_write(&aem_cmd_get_counters,
                                                                                         cmd_frame.payload,
                                                                                         ETHER_HDR_SIZE,
                                                                                         sizeof(cmd_frame.payload));
        
        if(aem_cmd_get_counters_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_avb_counters_write error\n");
            assert(aem_cmd_get_counters_returned >= 0);
            return -1;
        }
        
        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           base_end_station_imp_ref->entity_id(),
                                                           JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        
        return 0;
    }
    
    int avb_interface_descriptor_imp::proc_get_counters_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        ssize_t aem_cmd_get_counters_resp_returned;
        uint32_t msg_type;
        bool u_field;
        int r = 0;
        int i = 0;
        
        memcpy(cmd_frame.payload, frame, frame_len);
        
        aem_cmd_get_counters_resp_returned = jdksavdecc_aem_command_get_counters_response_read(&aem_cmd_get_counters_resp,
                                                                                                      frame,
                                                                                                      ETHER_HDR_SIZE,
                                                                                                      frame_len);
        
        
        for(i = 0; i<31; i++){
            r = jdksavdecc_uint32_read(&counters_response.counters_block[i], frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_GET_COUNTERS_RESPONSE_OFFSET_COUNTERS_BLOCK + 4 * i,
                frame_len);
            
            if (r < 0)
                break;
        }
        
        
        if(aem_cmd_get_counters_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_get_avb_counters_resp_read error\n");
            assert(aem_cmd_get_counters_resp_returned >= 0);
            return -1;
        }
        
        msg_type = aem_cmd_get_counters_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_get_counters_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_get_counters_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
        
        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        
        return 0;
    }
    
    uint32_t STDCALL avb_interface_descriptor_imp::get_counter_by_name(int name)
    {
        switch(name)
        {
            case AVB_INTERFACE_LINK_UP:
                return counters_response.counters_block[0];
            case AVB_INTERFACE_LINK_DOWN:
                return counters_response.counters_block[1];
            case AVB_INTERFACE_FRAMES_TX:
                return counters_response.counters_block[2];
            case AVB_INTERFACE_FRAMES_RX:
                return counters_response.counters_block[3];
            case AVB_INTERFACE_RX_CRC_ERROR:
                return counters_response.counters_block[4];
            case AVB_GPTP_GM_CHANGED:
                return counters_response.counters_block[5];
            default:
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "counter name not found\n");
        }
        return 0;
    }
    
    uint32_t STDCALL avb_interface_descriptor_imp::get_counter_valid(int name)
    {
        switch(name)
        {
            case AVB_INTERFACE_LINK_UP:
                return aem_cmd_get_counters_resp.counters_valid & 0x01;
            case AVB_INTERFACE_LINK_DOWN:
                return aem_cmd_get_counters_resp.counters_valid >> 1 & 0x01;
            case AVB_INTERFACE_FRAMES_TX:
                return aem_cmd_get_counters_resp.counters_valid >> 2 & 0x01;
            case AVB_INTERFACE_FRAMES_RX:
                return aem_cmd_get_counters_resp.counters_valid >> 3 & 0x01;
            case AVB_INTERFACE_RX_CRC_ERROR:
                return aem_cmd_get_counters_resp.counters_valid >> 4 & 0x01;
            case AVB_GPTP_GM_CHANGED:
                return aem_cmd_get_counters_resp.counters_valid >> 5 & 0x01;
            default:
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "counter name not found\n");
        }
        return 0;
    }
}
