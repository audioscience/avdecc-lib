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
#include <inttypes.h>
#include <mutex>

#include "version.h"
#include "net_interface_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "notification_acmp_imp.h"
#include "log_imp.h"
#include "util.h"
#include "adp.h"
#include "system_tx_queue.h"
#include "end_station_imp.h"
#include "adp_discovery_state_machine.h"
#include "acmp_controller_state_machine.h"
#include "aecp_controller_state_machine.h"
#include "controller_imp.h"

namespace avdecc_lib
{
net_interface_imp * net_interface_ref;
controller_imp * controller_imp_ref;

/*
* The end_stations class is added here so that in the rare case that an endpoint is added by the background discovery
* thread, causing the vector of end_stations to be re-allocated, a foreground process can still obtain a handle
* to an end-station and send commands. Note that end_stations are never deleted by this library, even if they go
* off-line, so end_station classes and their pointers will remain valid.
*/
class end_stations
{
private:
    size_t m_count;
    std::mutex locker;
    std::vector<end_station_imp *> end_station_vec; // Store a list of End Station objects
public:
    end_stations()
    {
        m_count = 0;
    };
    ~end_stations()
    {
        for (uint32_t i = 0; i < end_station_vec.size(); i++)
            delete end_station_vec.at(i);
    };
    /* use local m_count to avoid mutex operations */
    const size_t size(void)
    {
        return m_count;
    };
    end_station_imp * at(size_t i)
    {
        end_station_imp * ep;
        std::lock_guard<std::mutex> guard(locker);
        ep = end_station_vec.at(i);
        return ep;
    };
    void push_back(end_station_imp * ep)
    {
        locker.lock();
        end_station_vec.push_back(ep);
        m_count++;
        locker.unlock();
    };
};

controller * STDCALL create_controller(net_interface * netif,
                                       void (*notification_callback)(void *, int32_t, uint64_t,
                                                                     uint16_t, uint16_t, uint16_t,
                                                                     uint32_t, void *),
                                       void (*acmp_notification_callback)(void *, int32_t, uint16_t,
                                                                          uint64_t, uint16_t, uint64_t,
                                                                          uint16_t, uint32_t, void *),
                                       void (*log_callback)(void *, int32_t, const char *, int32_t),
                                       int32_t initial_log_level)
{
    log_imp_ref->set_log_level(initial_log_level);

    net_interface_ref = dynamic_cast<net_interface_imp *>(netif);

    controller_imp_ref = new controller_imp(notification_callback, acmp_notification_callback, log_callback);

    //Start up state machines if previously deleted on a restart
    if (!aecp_controller_state_machine_ref)
        aecp_controller_state_machine_ref = new aecp_controller_state_machine();

    if (!acmp_controller_state_machine_ref)
        acmp_controller_state_machine_ref = new acmp_controller_state_machine();

    if (!adp_discovery_state_machine_ref)
        adp_discovery_state_machine_ref = new adp_discovery_state_machine();

    if (!net_interface_ref)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base net_interface to derived net_interface_imp error");
    }

    return controller_imp_ref;
}

controller_imp::controller_imp(void (*notification_callback)(void *, int32_t, uint64_t, uint16_t,
                                                             uint16_t, uint16_t, uint32_t, void *),
                               void (*acmp_notification_callback)(void *, int32_t, uint16_t,
                                                                  uint64_t, uint16_t, uint64_t,
                                                                  uint16_t, uint32_t, void *),
                               void (*log_callback)(void *, int32_t, const char *, int32_t))
{
    notification_imp_ref->set_notification_callback(notification_callback, NULL);
    notification_acmp_imp_ref->set_acmp_notification_callback(acmp_notification_callback, NULL);
    end_station_array = new end_stations();
    log_imp_ref->set_log_callback(log_callback, NULL);

    m_entity_capabilities_flags = 0x00000000;
    m_talker_capabilities_flags = 0x00000000;
    m_listener_capabilities_flags = 0x00000000;
}

controller_imp::~controller_imp()
{
    delete end_station_array;
    end_station_array = NULL;
    delete adp_discovery_state_machine_ref;
    adp_discovery_state_machine_ref = NULL;
    delete acmp_controller_state_machine_ref;
    acmp_controller_state_machine_ref = NULL;
    delete aecp_controller_state_machine_ref;
    aecp_controller_state_machine_ref = NULL;
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
    return end_station_array->size();
}
    
uint64_t STDCALL controller_imp::get_entity_id()
{
    uint64_t controller_entity_id = ((net_interface_ref->mac_addr() & UINT64_C(0xFFFFFF000000)) << 16) |
                                    UINT64_C(0x000000FFFF000000) |
                                    (net_interface_ref->mac_addr() & UINT64_C(0xFFFFFF));
    
    return controller_entity_id;
}

end_station * STDCALL controller_imp::get_end_station_by_index(size_t end_station_index)
{
    return end_station_array->at(end_station_index);
}

bool STDCALL controller_imp::is_end_station_found_by_entity_id(uint64_t entity_entity_id, uint32_t & end_station_index)
{
    uint64_t end_station_entity_id;

    for (uint32_t i = 0; i < end_station_array->size(); i++)
    {
        end_station_entity_id = end_station_array->at(i)->entity_id();

        if (end_station_entity_id == entity_entity_id)
        {
            end_station_index = i;
            return true;
        }
    }

    return false;
}

bool STDCALL controller_imp::is_end_station_found_by_mac_addr(uint64_t mac_addr, uint32_t & end_station_index)
{
    uint64_t end_station_mac_addr;

    for (uint32_t i = 0; i < end_station_array->size(); i++)
    {
        end_station_mac_addr = end_station_array->at(i)->mac();

        if (end_station_mac_addr == mac_addr)
        {
            end_station_index = i;
            return true;
        }
    }

    return false;
}

configuration_descriptor * STDCALL controller_imp::get_current_config_desc(size_t end_station_index, bool report_error)
{
    uint16_t entity_index = 0;
    uint16_t config_index = 0;
    bool is_valid = false;

    if (end_station_index < end_station_array->size())
    {
        avdecc_lib::end_station * end_station = end_station_array->at(end_station_index);
        entity_index = end_station->get_current_entity_index();
        config_index = end_station->get_current_config_index();

        is_valid = (entity_index < end_station->entity_desc_count()) &&
                   (config_index < end_station->get_entity_desc_by_index(entity_index)->config_desc_count());
    }

    if (is_valid)
    {
        configuration_descriptor * configuration;
        configuration = end_station_array->at(end_station_index)->get_entity_desc_by_index(entity_index)->get_config_desc_by_index(config_index);

        return configuration;
    }
    else if (report_error)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_current_config_desc error");
    }

    return NULL;
}

configuration_descriptor * controller_imp::get_config_desc_by_entity_id(uint64_t entity_entity_id, uint16_t entity_index, uint16_t config_index)
{
    for (uint32_t i = 0; i < end_station_array->size(); i++)
    {
        entity_descriptor_response * entity_resp_ref = end_station_array->at(i)->get_entity_desc_by_index(entity_index)->get_entity_response();
        uint64_t end_station_entity_id = end_station_array->at(i)->entity_id();
        if (end_station_entity_id == entity_entity_id)
        {
            bool is_valid = ((entity_index < end_station_array->at(i)->entity_desc_count()) &&
                             (config_index < entity_resp_ref->configurations_count()));
            if (is_valid)
            {
                configuration_descriptor * configuration;
                configuration = end_station_array->at(i)->get_entity_desc_by_index(entity_index)->get_config_desc_by_index(config_index);

                return configuration;
            }
            else
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_config_desc_by_entity_id error");
            }
        }
        delete entity_resp_ref;
    }
    return NULL;
}

bool controller_imp::is_inflight_cmd_with_notification_id(void * notification_id)
{
    bool is_inflight_cmd = ((aecp_controller_state_machine_ref->is_inflight_cmd_with_notification_id(notification_id)) ||
                            (acmp_controller_state_machine_ref->is_inflight_cmd_with_notification_id(notification_id)));

    return is_inflight_cmd;
}

bool controller_imp::is_active_operation_with_notification_id(void * notification_id)
{
    return aecp_controller_state_machine_ref->is_active_operation_with_notification_id(notification_id);
}

void STDCALL controller_imp::set_logging_level(int32_t new_log_level)
{
    log_imp_ref->set_log_level(new_log_level);
}

void STDCALL controller_imp::apply_end_station_capabilities_filters(uint32_t entity_capabilities_flags,
                                                                    uint32_t talker_capabilities_flags,
                                                                    uint32_t listener_capabilities_flags)
{
    m_entity_capabilities_flags = entity_capabilities_flags;
    m_talker_capabilities_flags = talker_capabilities_flags;
    m_listener_capabilities_flags = listener_capabilities_flags;
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
    uint64_t end_station_entity_id;
    uint32_t disconnected_end_station_index;
    if (aecp_controller_state_machine_ref)
        aecp_controller_state_machine_ref->tick();
    if (acmp_controller_state_machine_ref)
        acmp_controller_state_machine_ref->tick();

    if (adp_discovery_state_machine_ref)
    {
        if (adp_discovery_state_machine_ref->tick(end_station_entity_id) &&
            is_end_station_found_by_entity_id(end_station_entity_id, disconnected_end_station_index))
        {
            end_station_array->at(disconnected_end_station_index)->set_disconnected();
        }
    }

    /* tick updates to background read of descriptors */
    for (uint32_t i = 0; i < end_station_array->size(); i++)
    {
        end_station_array->at(i)->background_read_update_timeouts();
        end_station_array->at(i)->background_read_submit_pending();
    }
}

int controller_imp::find_in_end_station(struct jdksavdecc_eui64 & other_entity_id, bool isUnsolicited, const uint8_t * frame)
{
    struct jdksavdecc_eui64 other_controller_id = jdksavdecc_acmpdu_get_controller_entity_id(frame, ETHER_HDR_SIZE);

    for (uint32_t i = 0; i < end_station_array->size(); i++)
    {
        struct jdksavdecc_eui64 end_entity_id = end_station_array->at(i)->get_adp()->get_entity_entity_id();
        struct jdksavdecc_eui64 this_controller_id = end_station_array->at(i)->get_adp()->get_controller_entity_id();
        bool entity_id_match = false;
        bool controller_id_match = false;

        if (jdksavdecc_eui64_compare(&end_entity_id, &other_entity_id) == 0)
        {
            entity_id_match = true;
        }

        //Do not try to find the controller_id if it is an unsolicited response
        if (isUnsolicited && entity_id_match)
        {
            return i;
        }
        else
        {
            if (jdksavdecc_eui64_compare(&other_controller_id, &this_controller_id) == 0 ||
                jdksavdecc_eui64_compare(&other_controller_id, &end_entity_id) == 0)
            {
                controller_id_match = true;
            }

            if (entity_id_match && controller_id_match)
            {
                return i;
            }
        }
    }

    return -1;
}

void controller_imp::rx_packet_event(void *& notification_id,
                                     bool & is_notification_id_valid,
                                     const uint8_t * frame,
                                     size_t frame_len,
                                     int & status,
                                     uint16_t & operation_id,
                                     bool & is_operation_id_valid)
{
    uint64_t dest_mac_addr;
    utility::convert_eui48_to_uint64(frame, dest_mac_addr);
    is_operation_id_valid = false;

    if ((dest_mac_addr == net_interface_ref->mac_addr()) || (dest_mac_addr & UINT64_C(0x010000000000))) // Process if the packet dest is our MAC address or a multicast address
    {
        uint8_t subtype = jdksavdecc_common_control_header_get_subtype(frame, ETHER_HDR_SIZE);

        switch (subtype)
        {
        case JDKSAVDECC_SUBTYPE_ADP:
        {
            end_station_imp * end_station = NULL;
            bool found_adp_in_end_station = false;

            jdksavdecc_adpdu adpdu;
            memset(&adpdu, 0, sizeof(adpdu));
            jdksavdecc_adpdu_read(&adpdu, frame, ETHER_HDR_SIZE, frame_len);

            status = AVDECC_LIB_STATUS_INVALID;
            is_notification_id_valid = false;

            if ((adpdu.entity_capabilities & JDKSAVDECC_ADP_ENTITY_CAPABILITY_GENERAL_CONTROLLER_IGNORE) ||
                (adpdu.entity_capabilities & JDKSAVDECC_ADP_ENTITY_CAPABILITY_ENTITY_NOT_READY))
            {
                // The entity indicates that we should not enumerate it
                break;
            }

            if ((m_entity_capabilities_flags & adpdu.entity_capabilities) != m_entity_capabilities_flags ||
                (m_talker_capabilities_flags & adpdu.talker_capabilities) != m_talker_capabilities_flags ||
                (m_listener_capabilities_flags & adpdu.listener_capabilities) != m_listener_capabilities_flags)
            {
                //The entity has been filtered out by the user set exclusion flags
                break;
            }

            /**
             * Check if an ADP object is already in the system. If not, create a new End Station object storing the ADPDU information
             * and add the End Station object to the system.
             */
            for (uint32_t i = 0; i < end_station_array->size(); i++)
            {
                struct jdksavdecc_eui64 end_entity_id = end_station_array->at(i)->get_adp()->get_entity_entity_id();
                if (jdksavdecc_eui64_compare(&end_entity_id, &adpdu.header.entity_id) == 0)
                {
                    found_adp_in_end_station = true;
                    end_station = end_station_array->at(i);
                }
            }

            if (jdksavdecc_eui64_convert_to_uint64(&adpdu.header.entity_id) != 0)
            {
                if (!found_adp_in_end_station)
                {
                    if (adp_discovery_state_machine_ref)
                        adp_discovery_state_machine_ref->state_avail(frame, frame_len);
                    end_station_array->push_back(new end_station_imp(frame, frame_len));
                    end_station_array->at(end_station_array->size() - 1)->set_connected();
                }
                else
                {
                    if ((adpdu.available_index < end_station->get_adp()->get_available_index()) ||
                        (jdksavdecc_eui64_convert_to_uint64(&adpdu.entity_model_id) != end_station->get_adp()->get_entity_model_id()))
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Re-enumerating end station with entity_id %ull", end_station->entity_id());
                        end_station->end_station_reenumerate();
                    }

                    end_station->get_adp()->proc_adpdu(frame, frame_len);

                    if (end_station->get_connection_status() == 'D')
                    {
                        end_station->set_connected();
                        if (adp_discovery_state_machine_ref)
                            adp_discovery_state_machine_ref->state_avail(frame, frame_len);
                    }
                    else
                    {
                        if (adp_discovery_state_machine_ref)
                            adp_discovery_state_machine_ref->state_avail(frame, frame_len);
                    }
                }
            }
            else if (adpdu.header.message_type != JDKSAVDECC_ADP_MESSAGE_TYPE_ENTITY_DISCOVER)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid ADP packet with an entity ID of 0.");
            }
        }
        break;

        case JDKSAVDECC_SUBTYPE_AECP:
        {
            int found_end_station_index = -1;
            uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);
            struct jdksavdecc_eui64 entity_entity_id = jdksavdecc_common_control_header_get_stream_id(frame, ETHER_HDR_SIZE);
            uint16_t cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame, ETHER_HDR_SIZE);
            bool isUnsolicited = cmd_type >> 15 & 0x01;

            /* check dest mac address is ours */
            if (dest_mac_addr == net_interface_ref->mac_addr())
            {
                if (msg_type == JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND &&
                    cmd_type == JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE)
                {
                    send_controller_avail_response(frame, frame_len);
                }
                else
                {
                    /**
                     * Check if an AECP object is already in the system. If yes, process response for the AECP packet.
                     */
                    found_end_station_index = find_in_end_station(entity_entity_id, isUnsolicited, frame);
                    if (found_end_station_index >= 0)
                    {
                        switch (msg_type)
                        {

                        case JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE:
                        {
                            end_station_array->at(found_end_station_index)->proc_rcvd_aem_resp(notification_id, frame, frame_len, status, operation_id, is_operation_id_valid);

                            is_notification_id_valid = true;
                            break;
                        }
                        case JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE:
                        {
                            end_station_array->at(found_end_station_index)->proc_rcvd_aecp_aa_resp(notification_id, frame, frame_len, status);

                            is_notification_id_valid = true;
                            break;
                        }
                        }
                    }
                    else
                    {
                        status = AVDECC_LIB_STATUS_INVALID;
                    }
                }
            }
        }
        break;

        case JDKSAVDECC_SUBTYPE_ACMP:
        {
            int found_end_station_index = -1;
            bool found_acmp_in_end_station = false;
            struct jdksavdecc_eui64 entity_entity_id;
            uint32_t msg_type = jdksavdecc_common_control_header_get_control_data(frame, ETHER_HDR_SIZE);

            if ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE) ||
                (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE))
            {
                entity_entity_id = jdksavdecc_acmpdu_get_talker_entity_id(frame, ETHER_HDR_SIZE);
            }
            else if ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE) ||
                     (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE) ||
                     (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE))
            {
                entity_entity_id = jdksavdecc_acmpdu_get_listener_entity_id(frame, ETHER_HDR_SIZE);
            }

            if ((msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE) ||
                (msg_type == JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE))
            {
                // check for unsolicited connect/disconnect responses
                found_end_station_index = find_in_end_station(entity_entity_id, true, frame);
            }
            else
            {
                found_end_station_index = find_in_end_station(entity_entity_id, false, frame);
            }

            if (found_end_station_index >= 0)
                found_acmp_in_end_station = true;

            if (found_acmp_in_end_station)
            {
                end_station_array->at(found_end_station_index)->proc_rcvd_acmp_resp(msg_type, notification_id, frame, frame_len, status);
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

void controller_imp::tx_packet_event(void * notification_id, uint32_t notification_flag, uint8_t * frame, size_t frame_len)
{
    uint8_t subtype = jdksavdecc_common_control_header_get_subtype(frame, ETHER_HDR_SIZE);
    struct jdksavdecc_frame packet_frame;

    packet_frame.length = (uint16_t)frame_len;
    assert(frame_len <= sizeof(packet_frame.payload));
    memcpy(packet_frame.payload, frame, frame_len);

    if (subtype == JDKSAVDECC_SUBTYPE_AECP)
    {
        if (aecp_controller_state_machine_ref)
        {
            aecp_controller_state_machine_ref->state_send_cmd(notification_id, notification_flag, &packet_frame);
            memcpy(frame, packet_frame.payload, frame_len); // Get the updated frame with sequence id
        }
    }
    else if (subtype == JDKSAVDECC_SUBTYPE_ACMP)
    {
        if (acmp_controller_state_machine_ref)
        {
            acmp_controller_state_machine_ref->state_command(notification_id, notification_flag, &packet_frame);
            memcpy(frame, packet_frame.payload, frame_len); // Get the updated frame with sequence id
        }
    }
    else
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid Subtype: %x", subtype);
    }
}

int STDCALL controller_imp::send_controller_avail_cmd(void * notification_id, uint32_t end_station_index)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_controller_available aem_cmd_controller_avail;
    ssize_t aem_cmd_controller_avail_returned;
    memset(&aem_cmd_controller_avail, 0, sizeof(aem_cmd_controller_avail));

    /*************************************************** AECP Common Data **************************************************/
    aem_cmd_controller_avail.aem_header.aecpdu_header.controller_entity_id = end_station_array->at(end_station_index)->get_adp()->get_controller_entity_id();
    // Fill aem_cmd_controller_avail.sequence_id in AEM Controller State Machine
    aem_cmd_controller_avail.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE;

    /******************************** Fill frame payload with AECP data and send the frame ***************************/
    aecp_controller_state_machine_ref->ether_frame_init(end_station_array->at(end_station_index)->mac(), &cmd_frame,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE);
    aem_cmd_controller_avail_returned = jdksavdecc_aem_command_controller_available_write(&aem_cmd_controller_avail,
                                                                                          cmd_frame.payload,
                                                                                          ETHER_HDR_SIZE,
                                                                                          sizeof(cmd_frame.payload));

    if (aem_cmd_controller_avail_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_controller_avail_write error\n");
        assert(aem_cmd_controller_avail_returned >= 0);
        return -1;
    }

    aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                       &cmd_frame,
                                                       end_station_array->at(end_station_index)->entity_id(),
                                                       JDKSAVDECC_AEM_COMMAND_CONTROLLER_AVAILABLE_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
    system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

    return 0;
}

int STDCALL controller_imp::send_controller_avail_response(const uint8_t * frame, size_t frame_len)
{
    struct jdksavdecc_eui48 dest_address;
    struct jdksavdecc_eui48 src_address;
    struct jdksavdecc_eui48 temp_address;
    int send_frame_returned = 0;
    size_t pos = 0;
    uint8_t * tx_frame = new uint8_t[frame_len];

    if (tx_frame == NULL)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "unable to allocate response frame");
        return -1;
    }

    memcpy(tx_frame, frame, frame_len);

    dest_address = jdksavdecc_eui48_get(tx_frame, pos + 0);
    src_address = jdksavdecc_eui48_get(tx_frame, pos + 6);

    // swap MAC addresses
    temp_address = src_address;
    src_address = dest_address;
    dest_address = temp_address;

    jdksavdecc_eui48_set(dest_address, tx_frame, pos + 0);
    jdksavdecc_eui48_set(src_address, tx_frame, pos + 6);

    //set message type
    jdksavdecc_common_control_header_set_control_data(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE,
                                                      tx_frame,
                                                      pos + ETHER_HDR_SIZE);

    //send packet
    send_frame_returned = net_interface_ref->send_frame(tx_frame, frame_len);

    if (send_frame_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "netif_send_frame error");
        assert(send_frame_returned >= 0);
    }

    delete[] tx_frame;
    return 0;
}

int controller_imp::proc_controller_avail_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status)
{
    struct jdksavdecc_frame cmd_frame;
    struct jdksavdecc_aem_command_controller_available_response aem_cmd_controller_avail_resp;
    ssize_t aem_cmd_controller_avail_resp_returned = 0;
    uint32_t msg_type = 0;
    bool u_field = false;

    memcpy(cmd_frame.payload, frame, frame_len);
    memset(&aem_cmd_controller_avail_resp, 0, sizeof(aem_cmd_controller_avail_resp));

    aem_cmd_controller_avail_resp_returned = jdksavdecc_aem_command_controller_available_response_read(&aem_cmd_controller_avail_resp,
                                                                                                       frame,
                                                                                                       ETHER_HDR_SIZE,
                                                                                                       frame_len);

    if (aem_cmd_controller_avail_resp_returned < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_controller_avail_resp_read error\n");
        assert(aem_cmd_controller_avail_resp_returned >= 0);
        return -1;
    }

    msg_type = aem_cmd_controller_avail_resp.aem_header.aecpdu_header.header.message_type;
    status = aem_cmd_controller_avail_resp.aem_header.aecpdu_header.header.status;
    u_field = aem_cmd_controller_avail_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

    aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

    return 0;
}
}
