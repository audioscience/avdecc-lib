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
 * end_station_imp.cpp
 *
 * End Station implementation
 */

#include <vector>
#include <cstring>
#include "avdecc_error.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "util_imp.h"
#include "adp.h"
#include "acmp_controller_state_machine.h"
#include "aecp_controller_state_machine.h"
#include "system_tx_queue.h"
#include "jdksavdecc.h"
#include "end_station_imp.h"

namespace avdecc_lib
{
    end_station_imp::end_station_imp(const uint8_t *frame, size_t frame_len)
    {
        end_station_connection_status = ' ';
        adp_ref = new adp(frame, frame_len);
        struct jdksavdecc_eui64 entity_id;
        entity_id = adp_ref->get_entity_entity_id();
        end_station_entity_id = jdksavdecc_uint64_get(&entity_id, 0);
        utility->convert_eui48_to_uint64(adp_ref->get_src_addr().value, end_station_mac);
        end_station_init();
    }

    end_station_imp::~end_station_imp()
    {
        delete adp_ref;

        for(uint32_t entity_vec_index = 0; entity_vec_index < entity_desc_vec.size(); entity_vec_index++)
        {
            delete entity_desc_vec.at(entity_vec_index);
        }
    }

    int end_station_imp::end_station_init()
    {
        current_entity_desc = 0;
        current_config_desc = 0;
        selected_entity_index = 0;
        selected_config_index = 0;

        read_desc_init(JDKSAVDECC_DESCRIPTOR_ENTITY, 0);

        return 0;
    }

    void end_station_imp::end_station_reenumerate()
    {
        for(uint32_t entity_vec_index = 0; entity_vec_index < entity_desc_vec.size(); entity_vec_index++)
        {
            delete entity_desc_vec.at(entity_vec_index);
        }

        entity_desc_vec.clear();

        end_station_init();
    }

    const char STDCALL end_station_imp::get_connection_status() const
    {
        return end_station_connection_status;
    }

    void end_station_imp::set_connected()
    {
        end_station_connection_status = 'C';
    }

    void end_station_imp::set_disconnected()
    {
        end_station_connection_status = 'D';
    }

    uint64_t STDCALL end_station_imp::entity_id()
    {
        return end_station_entity_id;
    }

    uint64_t STDCALL end_station_imp::mac()
    {
        return end_station_mac;
    }

    adp * end_station_imp::get_adp()
    {
        return adp_ref;
    }

    size_t STDCALL end_station_imp::entity_desc_count()
    {
        return entity_desc_vec.size();
    }

    entity_descriptor * STDCALL end_station_imp::get_entity_desc_by_index(size_t entity_desc_index)
    {
        bool is_valid = (entity_desc_index < entity_desc_vec.size());

        if(is_valid)
        {
            return entity_desc_vec.at(entity_desc_index);
        }
        else
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "get_entity_desc_by_index error");
        }

        return NULL;
    }

    int end_station_imp::read_desc_init(uint16_t desc_type, uint16_t desc_index)
    {
        return send_read_desc_cmd_with_flag(NULL, CMD_WITHOUT_NOTIFICATION, desc_type, desc_index);
    }

    int STDCALL end_station_imp::send_read_desc_cmd(void *notification_id, uint16_t desc_type, uint16_t desc_index)
    {
        return send_read_desc_cmd_with_flag(notification_id, CMD_WITH_NOTIFICATION, desc_type, desc_index);
    }

    int end_station_imp::send_read_desc_cmd_with_flag(void *notification_id, uint32_t notification_flag, uint16_t desc_type, uint16_t desc_index)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_read_descriptor aem_command_read_desc;
        memset(&aem_command_read_desc,0,sizeof(aem_command_read_desc));

        /***************************** AECP Common Data ****************************/
        aem_command_read_desc.aem_header.aecpdu_header.controller_entity_id = adp_ref->get_controller_entity_id();
        // Fill aem_command_read_desc.sequence_id in AEM Controller State Machine
        aem_command_read_desc.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR;

        /******************************************************** AECP Message Specific Data ********************************************************/
        aem_command_read_desc.configuration_index = (desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY || desc_type == JDKSAVDECC_DESCRIPTOR_CONFIGURATION) ?
                                                    0 : entity_desc_vec.at(current_entity_desc)->current_configuration();
        aem_command_read_desc.descriptor_type = desc_type;
        aem_command_read_desc.descriptor_index = desc_index;

        /************************** Fill frame payload with AECP data and send the frame *************************/
        aecp_controller_state_machine_ref->ether_frame_init(end_station_mac, &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_COMMAND_LEN);
        ssize_t write_return_val = jdksavdecc_aem_command_read_descriptor_write(&aem_command_read_desc,
                                                                                cmd_frame.payload,
                                                                                ETHER_HDR_SIZE,
                                                                                sizeof(cmd_frame.payload));

        if(write_return_val < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_read_desc_write error");
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           end_station_entity_id,
                                                           JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, notification_flag, cmd_frame.payload, cmd_frame.length);
        return 0;
    }

    int end_station_imp::proc_read_desc_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        const int read_desc_offset = ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN;
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_read_descriptor_response aem_cmd_read_desc_resp;
        ssize_t aem_cmd_read_desc_resp_returned;
        uint32_t msg_type;
        bool u_field;
        uint16_t desc_type;
        configuration_descriptor_imp *config_desc_imp_ref = NULL;
        memset(&aem_cmd_read_desc_resp,0,sizeof(aem_cmd_read_desc_resp));

        if(entity_desc_vec.size() >= 1 && entity_desc_vec.at(current_entity_desc)->config_desc_count() >= 1)
        {
            config_desc_imp_ref = dynamic_cast<configuration_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc));

            if(!config_desc_imp_ref)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base configuration_descriptor to derived configuration_descriptor_imp error");
            }
        }

        memcpy(cmd_frame.payload, frame, frame_len);
        aem_cmd_read_desc_resp_returned = jdksavdecc_aem_command_read_descriptor_response_read(&aem_cmd_read_desc_resp,
                                                                                               frame,
                                                                                               ETHER_HDR_SIZE,
                                                                                               frame_len);

        if(aem_cmd_read_desc_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_read_desc_res_read error");
            return -1;
        }

        msg_type = aem_cmd_read_desc_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_read_desc_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_read_desc_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
        desc_type = jdksavdecc_uint16_get(frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_OFFSET_DESCRIPTOR);

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);

        bool store_descriptor = false;
        if(status == avdecc_lib::AEM_STATUS_SUCCESS)
        {
            switch(desc_type)
            {
                case JDKSAVDECC_DESCRIPTOR_ENTITY:
                    store_descriptor = true;
                    break;

                case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
                    if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->config_desc_count() == 0)
                    {
                        store_descriptor = true;
                    }
                    break;

                default:
                    if(entity_desc_vec.size() == 1 && entity_desc_vec.at(current_entity_desc)->config_desc_count() >= 1)
                    {
                        store_descriptor = true;
                    }
                    break;
            }
        }

        if (store_descriptor)
        {
            try
            {
                switch (desc_type)
                {
                    case JDKSAVDECC_DESCRIPTOR_ENTITY:
                        if (entity_desc_vec.size() == 0)
                        {
                            entity_desc_vec.push_back(new entity_descriptor_imp(this, frame, read_desc_offset, frame_len));
                            current_config_desc = entity_desc_vec.at(current_entity_desc)->current_configuration();
                        }
                        break;

                    case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
                        entity_desc_vec.at(current_entity_desc)->store_config_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT:
                        config_desc_imp_ref->store_audio_unit_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_STREAM_INPUT:
                        config_desc_imp_ref->store_stream_input_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT:
                        config_desc_imp_ref->store_stream_output_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_JACK_INPUT:
                        config_desc_imp_ref->store_jack_input_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_JACK_OUTPUT:
                        config_desc_imp_ref->store_jack_output_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE:
                        config_desc_imp_ref->store_avb_interface_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE:
                        config_desc_imp_ref->store_clock_source_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT:
                        config_desc_imp_ref->store_memory_object_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_LOCALE:
                        config_desc_imp_ref->store_locale_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_STRINGS:
                        config_desc_imp_ref->store_strings_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT:
                        config_desc_imp_ref->store_stream_port_input_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT:
                        config_desc_imp_ref->store_stream_port_output_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER:
                        config_desc_imp_ref->store_audio_cluster_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_AUDIO_MAP:
                        config_desc_imp_ref->store_audio_map_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN:
                        config_desc_imp_ref->store_clock_domain_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_CONTROL:
                        config_desc_imp_ref->store_control_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_INPUT:
                        config_desc_imp_ref->store_external_port_input_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    case JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_OUTPUT:
                        config_desc_imp_ref->store_external_port_output_desc(this, frame, read_desc_offset, frame_len);
                        break;

                    default:
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Descriptor %s is not yet implemented in avdecc-lib.", utility->aem_desc_value_to_name(desc_type));
                        break;
                }
            }
            catch (const avdecc_read_descriptor_error& ia)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "0x%llx, catch %s", entity_id(), ia.what());
            }

            configuration_descriptor *cd = NULL;

            if ((entity_desc_vec.size() >= 1) && (entity_desc_vec.at(current_entity_desc)->config_desc_count() >= 1))
            {
                cd = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc);
            }
            background_read_deduce_next(cd, desc_type, (void *)frame, read_desc_offset);
        }
        background_read_update_inflight(desc_type, (void *)frame, read_desc_offset);
        background_read_submit_pending();

        if ((entity_desc_vec.size() >= 1) && (entity_desc_vec.at(current_entity_desc)->config_desc_count() >= 1))
        {
            if (m_backbround_read_inflight.empty() && m_backbround_read_pending.empty())
            {
                notification_imp_ref->post_notification_msg(END_STATION_READ_COMPLETED, end_station_entity_id, 0, 0, 0, 0, NULL);
            }
        }

        return 0;
    }

    void end_station_imp::background_read_update_timeouts(void)
    {
        std::list<background_read_request *>::iterator ii;
        background_read_request *b;

        ii = m_backbround_read_inflight.begin();
        while (ii != m_backbround_read_inflight.end())
        {
            b = *ii;
            // check inflight timeout
            if (b->m_timer.timeout())
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Background read timeout reading descriptor %s index %d\n", utility->aem_desc_value_to_name(b->m_type), b->m_index);
                ii = m_backbround_read_inflight.erase(ii);
                delete b;
            }
            else
            {
                ++ii;
            }
        }
    }


    void end_station_imp::background_read_update_inflight(uint16_t desc_type, void *frame, ssize_t read_desc_offset)
    {
        std::list<background_read_request *>::iterator ii;
        background_read_request *b;
        uint16_t desc_index;

        bool have_index = desc_index_from_frame(desc_type, frame, read_desc_offset, desc_index);

        ii = m_backbround_read_inflight.begin();
        while (ii != m_backbround_read_inflight.end())
        {
            b = *ii;
            // check inflight has been read
            if (have_index && (b->m_type == desc_type) && (b->m_index == desc_index))
            {
                ii = m_backbround_read_inflight.erase(ii);
                delete b;
            }
            else
            {
                ++ii;
            }
        }
    }

    void end_station_imp::background_read_submit_pending(void)
    {
        // if there are no pending inflight, but the background read list is not
        // empty submit the next set of read operations
        if (m_backbround_read_inflight.empty() && !m_backbround_read_pending.empty())
        {
            background_read_request *b_first = m_backbround_read_pending.front();
            m_backbround_read_pending.pop_front();
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Background read of %s index %d", utility->aem_desc_value_to_name(b_first->m_type), b_first->m_index);
            read_desc_init(b_first->m_type, b_first->m_index);
            b_first->m_timer.start(750);       // 750 ms timeout (1722.1 timeout is 250ms)
            m_backbround_read_inflight.push_back(b_first);

            if (!m_backbround_read_pending.empty())
            {
                background_read_request *b_next = m_backbround_read_pending.front();
                while (b_next->m_type == b_first->m_type)
                {
                    m_backbround_read_pending.pop_front();
                    log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "Background read of %s index %d", utility->aem_desc_value_to_name(b_next->m_type), b_next->m_index);
                    read_desc_init(b_next->m_type, b_next->m_index);
                    b_next->m_timer.start(750);       // 750 ms timeout (1722.1 timeout is 250ms)
                    m_backbround_read_inflight.push_back(b_next);
                    if (m_backbround_read_pending.empty())
                    {
                        break;
                    }
                    else
                    {
                        b_next = m_backbround_read_pending.front();
                    }
                }
            }
        }
    }

    bool end_station_imp::desc_index_from_frame(uint16_t desc_type, void *frame, ssize_t read_desc_offset, uint16_t &desc_index)
    {
        switch (desc_type)
        {
            case JDKSAVDECC_DESCRIPTOR_ENTITY:
                desc_index = 0;
                break;

            case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
                desc_index = jdksavdecc_descriptor_configuration_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT:
                desc_index = jdksavdecc_descriptor_audio_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_STREAM_INPUT:
            case JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT:
                desc_index = jdksavdecc_descriptor_stream_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_JACK_INPUT:
            case JDKSAVDECC_DESCRIPTOR_JACK_OUTPUT:
                desc_index = jdksavdecc_descriptor_jack_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE:
                desc_index = jdksavdecc_descriptor_avb_interface_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE:
                desc_index = jdksavdecc_descriptor_clock_source_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT:
                desc_index = jdksavdecc_descriptor_memory_object_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_LOCALE:
                desc_index = jdksavdecc_descriptor_locale_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_STRINGS:
                desc_index = jdksavdecc_descriptor_strings_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT:
            case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT:
                desc_index = jdksavdecc_descriptor_stream_port_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_INPUT:
            case JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_OUTPUT:
                desc_index = jdksavdecc_descriptor_external_port_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER:
                desc_index = jdksavdecc_descriptor_audio_cluster_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_AUDIO_MAP:
                desc_index = jdksavdecc_descriptor_audio_map_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN:
                desc_index = jdksavdecc_descriptor_clock_domain_get_descriptor_index(frame, read_desc_offset);
                break;

            case JDKSAVDECC_DESCRIPTOR_CONTROL:
                desc_index = jdksavdecc_descriptor_control_get_descriptor_index(frame, read_desc_offset);
                break;

            default:
                return false;
        }
        return true;
    }

    /** This function looks inside rx'd descriptors and deducees which descriptors need to be read in the background next.
     *  There are two lists that are maintained for reading descriptors. The m_backbround_read_pending list where descriptors
     *  are queued before being sent and the m_backbround_read_inflight list the contains read requests that are on "the wire".
     */
    void end_station_imp::background_read_deduce_next(configuration_descriptor *cd, uint16_t desc_type, void *frame, ssize_t read_desc_offset)
    {
        stream_port_input_descriptor *spid;
        stream_port_output_descriptor *spod;
        audio_unit_descriptor *aud;
        uint16_t total_num_of_desc = 0;
        uint16_t desc_index;

        bool have_index = desc_index_from_frame(desc_type, frame, read_desc_offset, desc_index);
        if (!have_index)
            desc_index = 0;

        if ((desc_type != JDKSAVDECC_DESCRIPTOR_ENTITY) && (cd == NULL))
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Invalid configuration_descriptor passed to background_read_deduce_next()\n");
            return;
        }

        switch (desc_type)
        {
            case JDKSAVDECC_DESCRIPTOR_ENTITY:
                queue_background_read_request(JDKSAVDECC_DESCRIPTOR_CONFIGURATION, 0, 1);
                break;

            case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
                total_num_of_desc = cd->descriptor_counts_count();
                for (int j = 0; j < total_num_of_desc; j++)
                {
                    queue_background_read_request(
                        cd->get_desc_type_from_config_by_index(j),
                        0,
                        cd->get_desc_count_from_config_by_index(j));
                }
                break;

            case JDKSAVDECC_DESCRIPTOR_LOCALE:
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_STRINGS,
                    0,
                    cd->get_locale_desc_by_index(0)->number_of_strings());
                break;

            case JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT:
                aud = cd->get_audio_unit_desc_by_index(desc_index);
                // stream port inputs
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT,
                    aud->base_stream_input_port(),
                    aud->number_of_stream_input_ports());
                // stream port outputs
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT,
                    aud->base_stream_output_port(),
                    aud->number_of_stream_output_ports());
                // external inputs
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_INPUT,
                    aud->base_external_input_port(),
                    aud->number_of_external_input_ports());
                // external outputs
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_EXTERNAL_PORT_OUTPUT,
                    aud->base_external_output_port(),
                    aud->number_of_external_output_ports());
                // controls
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_CONTROL,
                    aud->base_control_block(),
                    aud->number_of_control_blocks());
                // TODO: other descriptor types in AUDIO_UNIT
                break;

            case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT:
                spid = cd->get_stream_port_input_desc_by_index(desc_index);
                // controls
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_CONTROL,
                    spid->base_control(),
                    spid->number_of_controls());
                // clusters
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER,
                    spid->base_cluster(),
                    spid->number_of_clusters());
                // maps
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_AUDIO_MAP,
                    spid->base_map(),
                    spid->number_of_maps());
                break;

            case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT:
                spod = cd->get_stream_port_output_desc_by_index(desc_index);
                // controls
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_CONTROL,
                    spod->base_control(),
                    spod->number_of_controls());
                // clusters
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER,
                    spod->base_cluster(),
                    spod->number_of_clusters());
                // maps
                queue_background_read_request(
                    JDKSAVDECC_DESCRIPTOR_AUDIO_MAP,
                    spod->base_map(),
                    spod->number_of_maps());
                break;
        }
    }

    void end_station_imp::queue_background_read_request(uint16_t desc_type, uint16_t desc_base_index, uint16_t desc_count)
    {
        background_read_request *b;

        for (int i = 0; i < desc_count; i++)
        {
            b = new background_read_request(desc_type, desc_base_index + i);
            m_backbround_read_pending.push_back(b);
        }
    }


    int STDCALL end_station_imp::send_entity_avail_cmd(void *notification_id)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_entity_available aem_cmd_entity_avail;
        memset(&aem_cmd_entity_avail,0,sizeof(aem_cmd_entity_avail));

        /**************************** AECP Common Data ****************************/
        aem_cmd_entity_avail.aem_header.aecpdu_header.controller_entity_id = adp_ref->get_controller_entity_id();
        // Fill aem_cmd_entity_avail.sequence_id in AEM Controller State Machine
        aem_cmd_entity_avail.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE;

        /**************************** Fill frame payload with AECP data and send the frame *************************/
        aecp_controller_state_machine_ref->ether_frame_init(end_station_mac, &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE_COMMAND_LEN);
        ssize_t write_return_val = jdksavdecc_aem_command_entity_available_write(&aem_cmd_entity_avail,
                                                                                 cmd_frame.payload,
                                                                                 ETHER_HDR_SIZE,
                                                                                 sizeof(cmd_frame.payload));

        if(write_return_val < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_entity_avail_write error\n");
            return -1;
        }

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           end_station_entity_id,
                                                           JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE_COMMAND_LEN -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        return 0;
    }

    int end_station_imp::proc_entity_avail_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_entity_available_response aem_cmd_entity_avail_resp;
        ssize_t aem_cmd_entity_avail_resp_returned = 0;
        uint32_t msg_type = 0;
        bool u_field = false;
        memset(&aem_cmd_entity_avail_resp,0,sizeof(aem_cmd_entity_avail_resp));

        memcpy(cmd_frame.payload, frame, frame_len);

        aem_cmd_entity_avail_resp_returned = jdksavdecc_aem_command_entity_available_response_read(&aem_cmd_entity_avail_resp,
                                                                                                   frame,
                                                                                                   ETHER_HDR_SIZE,
                                                                                                   frame_len);

        if(aem_cmd_entity_avail_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_entity_avail_resp_read error\n");
            return -1;
        }

        msg_type = aem_cmd_entity_avail_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_entity_avail_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_entity_avail_resp.aem_header.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, &cmd_frame);
        return 0;
    }

    int end_station_imp::proc_rcvd_aem_resp(void *&notification_id,
                                            const uint8_t *frame,
                                            size_t frame_len,
                                            int &status,
                                            uint16_t &operation_id,
                                            bool &is_operation_id_valid)
    {
        uint16_t cmd_type;
        uint16_t desc_type;
        uint16_t desc_index;
        cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame, ETHER_HDR_SIZE);
        cmd_type &= 0x7FFF;

        switch(cmd_type)
        {
            case JDKSAVDECC_AEM_COMMAND_ACQUIRE_ENTITY:
                {
                    desc_type = jdksavdecc_aem_command_acquire_entity_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_acquire_entity_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY)
                    {
                        entity_descriptor_imp *entity_desc_imp_ref =
                            dynamic_cast<entity_descriptor_imp *>(entity_desc_vec.at(current_entity_desc));

                        if(entity_desc_imp_ref)
                        {
                            entity_desc_imp_ref->proc_acquire_entity_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base entity_descriptor to derived entity_descriptor_imp error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                    {
                        stream_input_descriptor_imp *stream_input_desc_imp_ref =
                            dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                        if(stream_input_desc_imp_ref)
                        {
                            stream_input_desc_imp_ref->proc_acquire_entity_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                    {
                        stream_output_descriptor_imp *stream_output_desc_imp_ref =
                            dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                        if(stream_output_desc_imp_ref)
                        {
                            stream_output_desc_imp_ref->proc_acquire_entity_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
                        }
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Descriptor type %d is not implemented.", desc_type);
                    }
                }

                break;

            case JDKSAVDECC_AEM_COMMAND_LOCK_ENTITY:
                {
                    desc_type = jdksavdecc_aem_command_lock_entity_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_lock_entity_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY)
                    {
                        entity_descriptor_imp *entity_desc_imp_ref =
                            dynamic_cast<entity_descriptor_imp *>(entity_desc_vec.at(current_entity_desc));

                        if(entity_desc_imp_ref)
                        {
                            entity_desc_imp_ref->proc_lock_entity_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base entity_descriptor to derived entity_descriptor_imp error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                    {
                        stream_input_descriptor_imp *stream_input_desc_imp_ref =
                            dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                        if(stream_input_desc_imp_ref)
                        {
                            stream_input_desc_imp_ref->proc_lock_entity_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                    {
                        stream_output_descriptor_imp *stream_output_desc_imp_ref =
                            dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                        if(stream_output_desc_imp_ref)
                        {
                            stream_output_desc_imp_ref->proc_lock_entity_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
                        }
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Descriptor type %d is not implemented.", desc_type);
                    }
                }

                break;

            case JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE:
                proc_entity_avail_resp(notification_id, frame, frame_len, status);
                break;

            case JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR:
                proc_read_desc_resp(notification_id, frame, frame_len, status);
                break;

            case JDKSAVDECC_AEM_COMMAND_SET_STREAM_FORMAT:
                {
                    desc_type = jdksavdecc_aem_command_set_stream_format_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_set_stream_format_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                    {
                        stream_input_descriptor_imp *stream_input_desc_imp_ref =
                            dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                        if(stream_input_desc_imp_ref)
                        {
                            stream_input_desc_imp_ref->proc_set_stream_format_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                    {
                        stream_output_descriptor_imp *stream_output_desc_imp_ref =
                            dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                        if(stream_output_desc_imp_ref)
                        {
                            stream_output_desc_imp_ref->proc_set_stream_format_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
                        }
                    }
                }

                break;

            case JDKSAVDECC_AEM_COMMAND_GET_STREAM_FORMAT:
                {
                    desc_type = jdksavdecc_aem_command_get_stream_format_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_get_stream_format_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                    {
                        stream_input_descriptor_imp *stream_input_desc_imp_ref =
                            dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                        if(stream_input_desc_imp_ref)
                        {
                            stream_input_desc_imp_ref->proc_get_stream_format_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                    {
                        stream_output_descriptor_imp *stream_output_desc_imp_ref =
                            dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                        if(stream_output_desc_imp_ref)
                        {
                            stream_output_desc_imp_ref->proc_get_stream_format_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
                        }
                    }
                }

                break;

            case JDKSAVDECC_AEM_COMMAND_SET_STREAM_INFO:
                desc_type = jdksavdecc_aem_command_set_stream_info_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                desc_index = jdksavdecc_aem_command_set_stream_info_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
                if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                {
                    stream_input_descriptor_imp *stream_input_desc_imp_ref =
                        dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                    if(stream_input_desc_imp_ref)
                    {
                        stream_input_desc_imp_ref->proc_set_stream_info_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_input_descriptor to derived stream_input_descriptor_imp error");
                    }
                }
                else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                {
                    stream_output_descriptor_imp *stream_output_desc_imp_ref =
                        dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                    if(stream_output_desc_imp_ref)
                    {
                        stream_output_desc_imp_ref->proc_set_stream_info_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base stream_output_descriptor_imp to derived stream_output_descriptor_imp error");
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_GET_STREAM_INFO:
                desc_type = jdksavdecc_aem_command_get_stream_info_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                desc_index = jdksavdecc_aem_command_get_stream_info_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                {
                    stream_input_descriptor_imp *stream_input_desc_imp_ref =
                        dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                    if(stream_input_desc_imp_ref)
                    {
                        stream_input_desc_imp_ref->proc_get_stream_info_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                    }
                }
                else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                {
                    stream_output_descriptor_imp *stream_output_desc_imp_ref =
                        dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                    if(stream_output_desc_imp_ref)
                    {
                        stream_output_desc_imp_ref->proc_get_stream_info_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_output_descriptor_imp to base stream_output_descriptor error");
                    }
                }

                break;

            case JDKSAVDECC_AEM_COMMAND_SET_NAME:
                desc_type = jdksavdecc_aem_command_set_name_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                desc_index = jdksavdecc_aem_command_set_name_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_NAME command.");

                break;

            case JDKSAVDECC_AEM_COMMAND_GET_NAME:
                desc_type = jdksavdecc_aem_command_get_name_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                desc_index = jdksavdecc_aem_command_get_name_response_get_descriptor_index(frame, ETHER_HDR_SIZE);
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement GET_NAME command.");

                break;

            case JDKSAVDECC_AEM_COMMAND_SET_SAMPLING_RATE:
                {
                    desc_type = jdksavdecc_aem_command_set_sampling_rate_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_set_sampling_rate_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT)
                    {
                        audio_unit_descriptor_imp *audio_unit_desc_imp_ref =
                            dynamic_cast<audio_unit_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_audio_unit_desc_by_index(desc_index));

                        if(audio_unit_desc_imp_ref)
                        {
                            audio_unit_desc_imp_ref->proc_set_sampling_rate_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base audio_unit_descriptor to derived audio_unit_descriptor_imp error");
                        }
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_GET_SAMPLING_RATE:
                {
                    desc_type = jdksavdecc_aem_command_get_sampling_rate_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_get_sampling_rate_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT)
                    {
                        audio_unit_descriptor_imp *audio_unit_desc_imp_ref =
                            dynamic_cast<audio_unit_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_audio_unit_desc_by_index(desc_index));

                        if(audio_unit_desc_imp_ref)
                        {
                            audio_unit_desc_imp_ref->proc_get_sampling_rate_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base audio_unit_descriptor to derived audio_unit_descriptor_imp error");
                        }
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_SET_CLOCK_SOURCE:
                {
                    desc_type = jdksavdecc_aem_command_set_clock_source_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_set_clock_source_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    clock_domain_descriptor_imp *clock_domain_desc_imp_ref =
                        dynamic_cast<clock_domain_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_clock_domain_desc_by_index(desc_index));

                    if(clock_domain_desc_imp_ref)
                    {
                        clock_domain_desc_imp_ref->proc_set_clock_source_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base clock_domain_descriptor to derived clock_domain_descriptor_imp error");
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_GET_CLOCK_SOURCE:
                {
                    desc_type = jdksavdecc_aem_command_get_clock_source_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_get_clock_source_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    clock_domain_descriptor_imp *clock_domain_desc_imp_ref =
                        dynamic_cast<clock_domain_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_clock_domain_desc_by_index(desc_index));

                    if(clock_domain_desc_imp_ref)
                    {
                        clock_domain_desc_imp_ref->proc_get_clock_source_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base clock_domain_descriptor to derived clock_domain_descriptor_imp error");
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_START_STREAMING:
                {
                    desc_type = jdksavdecc_aem_command_start_streaming_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_start_streaming_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                    {
                        stream_input_descriptor_imp *stream_input_desc_imp_ref =
                            dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                        if(stream_input_desc_imp_ref)
                        {
                            stream_input_desc_imp_ref->proc_start_streaming_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                    {
                        stream_output_descriptor_imp *stream_output_desc_imp_ref =
                            dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                        if(stream_output_desc_imp_ref)
                        {
                            stream_output_desc_imp_ref->proc_start_streaming_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_output_descriptor_imp to base stream_output_descriptor error");
                        }
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_STOP_STREAMING:
                {
                    desc_type = jdksavdecc_aem_command_stop_streaming_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_stop_streaming_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_INPUT)
                    {
                        stream_input_descriptor_imp *stream_input_desc_imp_ref =
                            dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                        if(stream_input_desc_imp_ref)
                        {
                            stream_input_desc_imp_ref->proc_stop_streaming_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                        }
                    }
                    else if(desc_type == JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT)
                    {
                        stream_output_descriptor_imp *stream_output_desc_imp_ref =
                            dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                        if(stream_output_desc_imp_ref)
                        {
                            stream_output_desc_imp_ref->proc_stop_streaming_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_output_descriptor_imp to base stream_output_descriptor error");
                        }
                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_REBOOT:
                {
                    desc_type = jdksavdecc_aem_command_reboot_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_reboot_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY)
                    {
                        entity_descriptor_imp *entity_desc_imp_ref =
                            dynamic_cast<entity_descriptor_imp *>(entity_desc_vec.at(current_entity_desc));

                        if(entity_desc_imp_ref)
                        {
                            entity_desc_imp_ref->proc_reboot_resp(notification_id, frame, frame_len, status);
                        }
                        else
                        {
                            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base entity_descriptor to derived entity_descriptor_imp error");
                        }
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Descriptor type %d is not valid.", desc_type);
                    }
                }

                break;

            case JDKSAVDECC_AEM_COMMAND_START_OPERATION:
                {
                    desc_type = jdksavdecc_aem_command_start_operation_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_start_operation_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT)
                    {
                        memory_object_descriptor *mo = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_memory_object_desc_by_index(desc_index);

                        if (mo)
                        {
                            memory_object_descriptor_imp *memory_object_desc_imp_ref =
                                dynamic_cast<memory_object_descriptor_imp *>(mo);

                            if(memory_object_desc_imp_ref)
                            {
                                uint16_t operation_type;
                                memory_object_desc_imp_ref->proc_start_operation_resp(notification_id, frame, frame_len, status, operation_id, operation_type);
                                if (status == AEM_STATUS_SUCCESS && operation_id)
                                {
                                    aecp_controller_state_machine_ref->start_operation(notification_id, operation_id, operation_type, frame, frame_len);
                                    is_operation_id_valid = true;
                                }
                            }
                            else
                            {
                                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived memory_object_descriptor_imp to base memory_object_descriptor error");
                            }
                        }

                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_OPERATION_STATUS:
                {
                    desc_type = jdksavdecc_aem_command_operation_status_response_get_descriptor_type(frame, ETHER_HDR_SIZE);
                    desc_index = jdksavdecc_aem_command_operation_status_response_get_descriptor_index(frame, ETHER_HDR_SIZE);

                    if(desc_type == JDKSAVDECC_DESCRIPTOR_MEMORY_OBJECT)
                    {
                        memory_object_descriptor *mo = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_memory_object_desc_by_index(desc_index);

                        if (mo)
                        {
                            memory_object_descriptor_imp *memory_object_desc_imp_ref =
                                dynamic_cast<memory_object_descriptor_imp *>(mo);

                            if(memory_object_desc_imp_ref)
                            {
                                memory_object_desc_imp_ref->proc_operation_status_resp(notification_id, frame, frame_len, status, operation_id, is_operation_id_valid);
                            }
                            else
                            {
                                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived memory_object_descriptor_imp to base memory_object_descriptor error");
                            }
                        }

                    }
                }
                break;

            case JDKSAVDECC_AEM_COMMAND_SET_CONTROL:
                proc_set_control_resp(notification_id, frame, frame_len, status);
                break;

            default:
                notification_imp_ref->post_notification_msg(NO_MATCH_FOUND, 0, cmd_type, 0, 0, 0, 0);
                break;
        }

        return 0;
    }


    int STDCALL end_station_imp::send_aecp_address_access_cmd(void *notification_id,
                                                              unsigned mode,
                                                              unsigned length,
                                                              uint64_t address,
                                                              uint8_t memory_data[])
    {
        struct jdksavdecc_aecp_aa aecp_cmd_aa_header;
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aecp_aa_tlv aa_tlv;
        memset(&aecp_cmd_aa_header,0,sizeof(aecp_cmd_aa_header));


        aecp_cmd_aa_header.controller_entity_id = adp_ref->get_controller_entity_id();
        aecp_cmd_aa_header.sequence_id = 0;
        aecp_cmd_aa_header.tlv_count = 1;

        aecp_controller_state_machine_ref->ether_frame_init(end_station_mac, &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AECPDU_AA_LEN + JDKSAVDECC_AECPDU_AA_TLV_LEN + length);

        ssize_t write_return_val = jdksavdecc_aecp_aa_write(&aecp_cmd_aa_header,
                                                            cmd_frame.payload,
                                                            ETHER_HDR_SIZE,
                                                            sizeof(cmd_frame.payload));

        if(write_return_val < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "jdksavdecc_aecp_aa_write error");
            return -1;
        }

        aa_tlv.mode_length = (mode << 12) | (length & 0xFFF);
        aa_tlv.address_upper = address >> 32;
        aa_tlv.address_lower = address & 0xFFFFFFFF;

        write_return_val = jdksavdecc_aecp_aa_tlv_write(&aa_tlv,
                                                        cmd_frame.payload,
                                                        ETHER_HDR_SIZE + JDKSAVDECC_AECPDU_AA_LEN,
                                                        sizeof(cmd_frame.payload));

        if(write_return_val < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "jdksavdecc_aecp_aa_tlv_write error");
            return -1;
        }

        memcpy(&cmd_frame.payload[ETHER_HDR_SIZE + JDKSAVDECC_AECPDU_AA_LEN + JDKSAVDECC_AECPDU_AA_TLV_LEN], memory_data, length);

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_COMMAND,
                                                           &cmd_frame,
                                                           end_station_entity_id,
                                                           JDKSAVDECC_AECPDU_AA_LEN + JDKSAVDECC_AECPDU_AA_TLV_LEN + length -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);

        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);

        return 0;
    }

    int STDCALL end_station_imp::send_identify(void *notification_id, bool turn_on)
    {
        struct jdksavdecc_frame cmd_frame;
        struct jdksavdecc_aem_command_set_control aem_command_set_control;
        memset(&aem_command_set_control,0,sizeof(aem_command_set_control));

        /***************************** AECP Common Data ****************************/
        aem_command_set_control.aem_header.aecpdu_header.controller_entity_id = adp_ref->get_controller_entity_id();
        // Fill aem_command_read_desc.sequence_id in AEM Controller State Machine
        aem_command_set_control.aem_header.command_type = JDKSAVDECC_AEM_COMMAND_SET_CONTROL;

        /******************************************************** AECP Message Specific Data ********************************************************/
        aem_command_set_control.descriptor_type = JDKSAVDECC_DESCRIPTOR_CONTROL;
        aem_command_set_control.descriptor_index = get_adp()->get_identify_control_index();

        /************************** Fill frame payload with AECP data and send the frame *************************/
        aecp_controller_state_machine_ref->ether_frame_init(end_station_mac, &cmd_frame,
                                                            ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN + 1);
        ssize_t write_return_val = jdksavdecc_aem_command_set_control_write(&aem_command_set_control,
                                                                            cmd_frame.payload,
                                                                            ETHER_HDR_SIZE,
                                                                            sizeof(cmd_frame.payload));

        if(write_return_val < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_command_set_control_write error");
            return -1;
        }

        unsigned char data[1];
        if (turn_on)
            data[0] = 255;
        else
            data[0] = 0;
        memcpy(&cmd_frame.payload[ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN], data, 1);

        aecp_controller_state_machine_ref->common_hdr_init(JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_COMMAND,
                                                           &cmd_frame,
                                                           end_station_entity_id,
                                                           JDKSAVDECC_AEM_COMMAND_SET_CONTROL_COMMAND_LEN + 1 -
                                                           JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame.payload, cmd_frame.length);
        return 0;
    }

    int end_station_imp::proc_set_control_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        memcpy(cmd_frame.payload, frame, frame_len);
        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, JDKSAVDECC_AECP_MESSAGE_TYPE_AEM_RESPONSE, false, &cmd_frame);
        return 0;
    }

    int end_station_imp::proc_rcvd_aecp_aa_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame cmd_frame;
        memcpy(cmd_frame.payload, frame, frame_len);

        status = jdksavdecc_common_control_header_get_status(frame, ETHER_HDR_SIZE);

        //uint16_t sequence_id = jdksavdecc_aecp_aa_get_sequence_id(frame, ETHER_HDR_SIZE);
        uint16_t tlv_count = jdksavdecc_aecp_aa_get_tlv_count(frame, ETHER_HDR_SIZE);

        if (tlv_count != 1)
        {
            // Do not currently support TLV counts > 1
            notification_imp_ref->post_notification_msg(NO_MATCH_FOUND, 0, 0, 0, 0, 0, 0);
        }

        const int tlv_data_offset = ETHER_HDR_SIZE + JDKSAVDECC_AECPDU_AA_LEN;

        uint16_t mode_length = jdksavdecc_aecp_aa_tlv_get_mode_length(frame, tlv_data_offset);

        unsigned mode = (mode_length >> 12) & 0xF;
        //unsigned length = mode_length & 0xFFF;

        switch (mode)
        {
            case JDKSAVDECC_AECP_AA_MODE_READ:
                break;
            case JDKSAVDECC_AECP_AA_MODE_WRITE:
                break;
            case JDKSAVDECC_AECP_AA_MODE_EXECUTE:
                break;
        }

        //uint32_t address_upper = jdksavdecc_aecp_aa_tlv_get_address_upper(frame, tlv_data_offset);
        //uint32_t address_lower = jdksavdecc_aecp_aa_tlv_get_address_lower(frame, tlv_data_offset);

        aecp_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, JDKSAVDECC_AECP_MESSAGE_TYPE_ADDRESS_ACCESS_RESPONSE, false, &cmd_frame);

        return 0;
    }

    int end_station_imp::proc_rcvd_acmp_resp(uint32_t msg_type, void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        uint16_t desc_index = 0;

        switch(msg_type)
        {
            case JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_STATE_RESPONSE:
                {
                    desc_index = jdksavdecc_acmpdu_get_talker_unique_id(frame, ETHER_HDR_SIZE);
                    stream_output_descriptor_imp *stream_output_desc_imp_ref;
                    stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                    if(stream_output_desc_imp_ref)
                    {
                        stream_output_desc_imp_ref->proc_get_tx_state_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                    }
                }

                break;

            case JDKSAVDECC_ACMP_MESSAGE_TYPE_CONNECT_RX_RESPONSE:
                {
                    desc_index = jdksavdecc_acmpdu_get_listener_unique_id(frame, ETHER_HDR_SIZE);
                    stream_input_descriptor_imp *stream_input_desc_imp_ref;
                    stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                    if(stream_input_desc_imp_ref)
                    {
                        stream_input_desc_imp_ref->proc_connect_rx_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                    }
                }

                break;

            case JDKSAVDECC_ACMP_MESSAGE_TYPE_DISCONNECT_RX_RESPONSE:
                {
                    desc_index = jdksavdecc_acmpdu_get_listener_unique_id(frame, ETHER_HDR_SIZE);
                    stream_input_descriptor_imp *stream_input_desc_imp_ref;
                    stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                    if(stream_input_desc_imp_ref)
                    {
                        stream_input_desc_imp_ref->proc_disconnect_rx_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                    }
                }

                break;

            case JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_RX_STATE_RESPONSE:
                {
                    desc_index = jdksavdecc_acmpdu_get_listener_unique_id(frame, ETHER_HDR_SIZE);
                    stream_input_descriptor_imp *stream_input_desc_imp_ref;
                    stream_input_desc_imp_ref = dynamic_cast<stream_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_input_desc_by_index(desc_index));

                    if(stream_input_desc_imp_ref)
                    {
                        stream_input_desc_imp_ref->proc_get_rx_state_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                    }
                }

                break;

            case JDKSAVDECC_ACMP_MESSAGE_TYPE_GET_TX_CONNECTION_RESPONSE:
                {
                    desc_index = jdksavdecc_acmpdu_get_talker_unique_id(frame, ETHER_HDR_SIZE);
                    stream_output_descriptor_imp *stream_output_desc_imp_ref;
                    stream_output_desc_imp_ref = dynamic_cast<stream_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_output_desc_by_index(desc_index));

                    if(stream_output_desc_imp_ref)
                    {
                        stream_output_desc_imp_ref->proc_get_tx_connection_resp(notification_id, frame, frame_len, status);
                    }
                    else
                    {
                        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from derived stream_input_descriptor_imp to base stream_input_descriptor error");
                    }
                }

                break;

            default:
                notification_imp_ref->post_notification_msg(NO_MATCH_FOUND, 0, msg_type, 0, 0, 0, 0);
                break;
        }

        return 0;
    }

    void STDCALL end_station_imp::set_current_entity_index(uint16_t entity_index)
    {
        selected_entity_index = entity_index;
    }

    uint16_t STDCALL end_station_imp::get_current_entity_index() const
    {
        return selected_entity_index;
    }

    void STDCALL end_station_imp::set_current_config_index(uint16_t config_index)
    {
        selected_config_index = config_index;
    }

    uint16_t STDCALL end_station_imp::get_current_config_index() const
    {
        return selected_config_index;
    }
}
