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
 * controller_imp.cpp
 *
 * Controller implementation
 */

#include <vector>
#include <cstdint>
#include "version.h"
#include "net_interface_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "util_imp.h"
#include "adp.h"
#include "system_tx_queue.h"
#include "end_station_imp.h"
#include "adp_discovery_state_machine.h"
#include "acmp_controller_state_machine.h"
#include "aem_controller_state_machine.h"
#include "controller_imp.h"

namespace avdecc_lib
{
    net_interface_imp *net_interface_ref;
    controller_imp *controller_imp_ref;

    controller * STDCALL create_controller(net_interface *netif,
                                           void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
                                           void (*log_callback) (void *, int32_t, const char *, int32_t))
    {
        net_interface_ref = dynamic_cast<net_interface_imp *>(netif);
        if(!net_interface_ref)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base net_interface to derived net_interface_imp error");
        }

        controller_imp_ref = new controller_imp(notification_callback, log_callback);

        return controller_imp_ref;
    }

    controller_imp::controller_imp(void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
                                   void (*log_callback) (void *, int32_t, const char *, int32_t))
    {
        notification_imp_ref->set_notification_callback(notification_callback, NULL);
        log_imp_ref->set_log_callback(log_callback, NULL);
    }

    controller_imp::~controller_imp()
    {
        for(uint32_t end_station_index = 0; end_station_index < end_station_vec.size(); end_station_index++)
        {
            delete end_station_vec.at(end_station_index);
        }

        delete adp_discovery_state_machine_ref;
        delete acmp_controller_state_machine_ref;
        delete aem_controller_state_machine_ref;
    }

    void STDCALL controller_imp::destroy()
    {
        delete this;
    }

    const char * STDCALL controller_imp::get_version() const
    {
        return AVDECC_CONTROLLER_VERSION;
    }

    size_t STDCALL controller_imp::get_end_station_count()
    {
        return end_station_vec.size();
    }

    end_station * STDCALL controller_imp::get_end_station_by_index(size_t end_station_index)
    {
        return end_station_vec.at(end_station_index);
    }

    bool controller_imp::is_end_station_found_by_guid(uint64_t entity_guid, uint32_t &end_station_index)
    {
        uint64_t end_station_guid;

        for(uint32_t i = 0; i < end_station_vec.size(); i++)
        {
            end_station_guid = end_station_vec.at(i)->guid();

            if(end_station_guid == entity_guid)
            {
                end_station_index = i;
                return true;
            }
        }

        return false;
    }

    configuration_descriptor * STDCALL controller_imp::get_config_desc_by_index(size_t end_station_index, uint16_t entity_index, uint16_t config_index)
    {
        bool is_valid = ((end_station_index < end_station_vec.size()) &&
                         (entity_index < end_station_vec.at(end_station_index)->entity_desc_count()) &&
                         (config_index < end_station_vec.at(end_station_index)->get_entity_desc_by_index(entity_index)->configurations_count()));

        if(is_valid)
        {
            configuration_descriptor * configuration;
            configuration = end_station_vec.at(end_station_index)->get_entity_desc_by_index(entity_index)->get_config_desc_by_index(config_index);

            return configuration;
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_config_desc_by_index error");
        }

        return NULL;
    }

    configuration_descriptor * controller_imp::get_config_desc_by_guid(uint64_t entity_guid, uint16_t entity_index, uint16_t config_index)
    {
        for(uint32_t i = 0; i < end_station_vec.size(); i++)
        {
            uint64_t end_station_guid = end_station_vec.at(i)->guid();
            if(end_station_guid == entity_guid)
            {
                bool is_valid = ((entity_index < end_station_vec.at(i)->entity_desc_count()) &&
                                 (config_index < end_station_vec.at(i)->get_entity_desc_by_index(entity_index)->configurations_count()));
                if(is_valid)
                {
                    configuration_descriptor * configuration;
                    configuration = end_station_vec.at(i)->get_entity_desc_by_index(entity_index)->get_config_desc_by_index(config_index);

                    return configuration;
                }
                else
                {
                    log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_config_desc_by_guid error");
                }
            }
        }

        return NULL;
    }

    bool controller_imp::is_inflight_cmd_with_notification_id(void *notification_id)
    {
        bool is_inflight_cmd = ((aem_controller_state_machine_ref->is_inflight_cmd_with_notification_id(notification_id)) ||
                                (acmp_controller_state_machine_ref->is_inflight_cmd_with_notification_id(notification_id)));

        return is_inflight_cmd;
    }

    void STDCALL controller_imp::set_logging_level(int32_t new_log_level)
    {
        log_imp_ref->set_log_level(new_log_level);
    }

    uint32_t STDCALL controller_imp::missed_notification_count()
    {
        return notification_imp_ref->missed_notification_event_count();
    }

    uint32_t STDCALL controller_imp::missed_log_count()
    {
        return log_imp_ref->missed_log_event_count();
    }

    void controller_imp::time_tick_event()
    {
        uint64_t end_station_guid;
        uint32_t disconnected_end_station_index;
        aem_controller_state_machine_ref->tick();

        if(adp_discovery_state_machine_ref->tick(end_station_guid) &&
           is_end_station_found_by_guid(end_station_guid, disconnected_end_station_index))
        {
            end_station_vec.at(disconnected_end_station_index)->set_disconnected();
        }
    }

    int controller_imp::find_in_end_station(struct jdksavdecc_eui64 &other_entity_id, const uint8_t *frame)
    {
        struct jdksavdecc_eui64 other_controller_id = jdksavdecc_acmpdu_get_controller_entity_id(frame, ETHER_HDR_SIZE);

        for(uint32_t i = 0; i < end_station_vec.size(); i++)
        {
            struct jdksavdecc_eui64 end_entity_id = end_station_vec.at(i)->get_adp()->get_entity_entity_id();
            struct jdksavdecc_eui64 this_controller_id = end_station_vec.at(i)->get_adp()->get_controller_guid();

            if((jdksavdecc_eui64_compare(&end_entity_id, &other_entity_id) == 0) &&
                ((jdksavdecc_eui64_compare(&other_controller_id, &this_controller_id) == 0) ||
                (jdksavdecc_eui64_compare(&other_controller_id, &end_entity_id) == 0)))
            {
                return i;
            }
        }

        return -1;
    }

    void controller_imp::rx_packet_event(void *&notification_id, bool &is_notification_id_valid, const uint8_t *frame, size_t frame_len, int &status)
    {
        uint64_t dest_mac_addr;
        utility->convert_eui48_to_uint64(frame, dest_mac_addr);

        if((dest_mac_addr == net_interface_ref->mac_addr()) || (dest_mac_addr & UINT64_C(0x010000000000))) // Process if the packet dest is our MAC address or a multicast address
        {
            uint32_t subtype = jdksavdecc_subtype_data_get_subtype(jdksavdecc_uint32_get(frame, ETHER_HDR_SIZE));

            switch(subtype)
            {
                case JDKSAVDECC_SUBTYPE_ADP:
                    {
                        int found_end_station_index = -1;
                        bool found_adp_in_end_station = false;
                        struct jdksavdecc_eui64 other_entity_id;
                        jdksavdecc_eui64_read(&other_entity_id, frame, ETHER_HDR_SIZE + PROTOCOL_HDR_SIZE, frame_len);

                        //log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "ADP packet discovered.");

                        /**
                         * Check if an ADP object is already in the system. If not, create a new End Station object storing the ADPDU information
                         * and add the End Station object to the system.
                         */
                        for(uint32_t i = 0; i < end_station_vec.size(); i++)
                        {
                            struct jdksavdecc_eui64 end_entity_id = end_station_vec.at(i)->get_adp()->get_entity_entity_id();
                            if(jdksavdecc_eui64_compare(&end_entity_id, &other_entity_id) == 0)
                            {
                                found_adp_in_end_station = true;
                                found_end_station_index = i;
                            }
                        }

                        if(jdksavdecc_uint64_get(&other_entity_id, 0) != 0)
                        {
                            if(!found_adp_in_end_station)
                            {
                                adp_discovery_state_machine_ref->state_avail(frame, frame_len);
                                end_station_vec.push_back(new end_station_imp(frame, frame_len));
                                end_station_vec.at(end_station_vec.size() - 1)->set_connected();
                            }
                            else
                            {
                                if(end_station_vec.at(found_end_station_index)->get_connection_status() == 'D')
                                {
                                    end_station_vec.at(found_end_station_index)->set_connected();
                                    adp_discovery_state_machine_ref->state_avail(frame, frame_len);
                                }
                                else
                                {
                                    adp_discovery_state_machine_ref->state_avail(frame, frame_len);
                                }
                            }
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid ADP packet with an entity GUID of 0.");
                        }

                        status = AVDECC_LIB_STATUS_INVALID;
                        is_notification_id_valid = false;
                    }
                    break;

                case JDKSAVDECC_SUBTYPE_AECP:
                    {
                        int found_end_station_index = -1;
                        bool found_aecp_in_end_station = false;
                        uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);
                        struct jdksavdecc_eui64 entity_guid = jdksavdecc_common_control_header_get_stream_id(frame, ETHER_HDR_SIZE);

                        /**
                         * Check if an AECP object is already in the system. If yes, process response for the AECP packet.
                         */
                        if((msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE) && (dest_mac_addr == net_interface_ref->mac_addr()))
                        {
                            found_end_station_index = find_in_end_station(entity_guid, frame);
                            if (found_end_station_index >= 0) found_aecp_in_end_station = true;
                        }

                        if(found_aecp_in_end_station)
                        {
                            uint16_t cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame, ETHER_HDR_SIZE + JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);

                            if(cmd_type == JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE)
                            {
                                proc_controller_avail_resp(notification_id, frame, frame_len, status);
                            }
                            else
                            {
                                end_station_vec.at(found_end_station_index)->proc_rcvd_aem_resp(notification_id, frame, frame_len, status);
                            }

                            is_notification_id_valid = true;
                        }
                        else
                        {
                            status = AVDECC_LIB_STATUS_INVALID;
                        }
                    }
                    break;

                case JDKSAVDECC_SUBTYPE_ACMP:
                    {
                        int found_end_station_index = -1;
                        bool found_acmp_in_end_station = false;
                        struct jdksavdecc_eui64 entity_guid;
                        uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);

                        if((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) || 
                           (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE))
                        {
                            entity_guid = jdksavdecc_acmpdu_get_talker_entity_id(frame, ETHER_HDR_SIZE);
                        }
                        else if((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE) ||
                                (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE) ||
                                (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE))
                        {
                            entity_guid = jdksavdecc_acmpdu_get_listener_entity_id(frame, ETHER_HDR_SIZE);
                        }

                        found_end_station_index = find_in_end_station(entity_guid, frame);
                        if (found_end_station_index >= 0) found_acmp_in_end_station = true;

                        if(found_acmp_in_end_station)
                        {
                            end_station_vec.at(found_end_station_index)->proc_rcvd_acmp_resp(msg_type, notification_id, frame, frame_len, status);
                            is_notification_id_valid = true;
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Wait for correct ACMP response packet.");
                            status = AVDECC_LIB_STATUS_INVALID;
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }

    void controller_imp::tx_packet_event(void *notification_id, uint32_t notification_flag, uint8_t *frame, size_t frame_len)
    {
        uint32_t subtype = jdksavdecc_subtype_data_get_subtype(jdksavdecc_uint32_get(frame, ETHER_HDR_SIZE));
        struct jdksavdecc_frame packet_frame;

        packet_frame.length = frame_len;
        memcpy(packet_frame.payload, frame, frame_len);

        if(subtype == JDKSAVDECC_SUBTYPE_AECP)
        {
            aem_controller_state_machine_ref->state_send_cmd(notification_id, notification_flag, &packet_frame);
            memcpy(frame, packet_frame.payload, frame_len); // Get the updated frame with sequence id
        }
        else if(subtype == JDKSAVDECC_SUBTYPE_ACMP)
        {
            acmp_controller_state_machine_ref->state_command(notification_id, notification_flag, &packet_frame);
            memcpy(frame, packet_frame.payload, frame_len); // Get the updated frame with sequence id
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid Subtype");
        }
    }

    int STDCALL controller_imp::send_controller_avail_cmd(void *notification_id, uint32_t end_station_index)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_controller_available aem_cmd_controller_avail;
        ssize_t aem_cmd_controller_avail_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /*************************************************** AECP Common Data **************************************************/
        aem_cmd_controller_avail.controller_entity_id = end_station_vec.at(end_station_index)->get_adp()->get_controller_guid();
        // Fill aem_cmd_controller_avail.sequence_id in AEM Controller State Machine
        aem_cmd_controller_avail.command_type = JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE;

        /******************************** Fill frame payload with AECP data and send the frame ***************************/
        aem_controller_state_machine_ref->ether_frame_init(end_station_vec.at(end_station_index)->mac(), cmd_frame);
        aem_cmd_controller_avail_returned = jdksavdecc_aem_command_controller_available_write(&aem_cmd_controller_avail,
                                                                                              cmd_frame->payload,
                                                                                              ETHER_HDR_SIZE,
                                                                                              sizeof(cmd_frame->payload));

        if(aem_cmd_controller_avail_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_controller_avail_write error\n");
            assert(aem_cmd_controller_avail_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, end_station_vec.at(end_station_index)->guid());
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;
    }

    int controller_imp::proc_controller_avail_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_controller_available_response aem_cmd_controller_avail_resp;
        ssize_t aem_cmd_controller_avail_resp_returned = 0;
        uint32_t msg_type = 0;
        bool u_field = false;

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);

        aem_cmd_controller_avail_resp_returned = jdksavdecc_aem_command_controller_available_response_read(&aem_cmd_controller_avail_resp,
                                                                                                           frame,
                                                                                                           ETHER_HDR_SIZE,
                                                                                                           frame_len);

        if(aem_cmd_controller_avail_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_controller_avail_resp_read error\n");
            assert(aem_cmd_controller_avail_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_controller_avail_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_controller_avail_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_controller_avail_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

        free(cmd_frame);
        return 0;
    }
}
