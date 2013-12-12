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
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "util_imp.h"
#include "adp.h"
#include "acmp_controller_state_machine.h"
#include "aem_controller_state_machine.h"
#include "system_tx_queue.h"
#include "end_station_imp.h"

namespace avdecc_lib
{
    end_station_imp::end_station_imp(const uint8_t *frame, size_t frame_len)
    {
        end_station_connection_status = ' ';
        current_entity_desc = 0;
        current_config_desc = 0;
        desc_type_from_config = 0;
        desc_type_index_from_config = 0;
        desc_count_from_config = 0;
        desc_count_index_from_config = 0;
        read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_IDLE;
        read_desc_in_locale_state = READ_DESC_IN_LOCALE_IDLE;
        read_desc_in_audio_unit_state = READ_DESC_IN_AUDIO_UNIT_IDLE;
        read_desc_in_stream_port_input_state = READ_DESC_IN_STREAM_PORT_INPUT_IDLE;
        read_desc_in_stream_port_output_state = READ_DESC_IN_STREAM_PORT_OUTPUT_IDLE;
        read_desc_count = 0;
        read_top_level_desc_done = false;
        read_desc_in_locale_done = false;
        read_desc_in_audio_unit_done = false;
        read_desc_in_stream_port_input_done = false;
        read_desc_in_stream_port_output_done = false;
        desc_type_index_from_audio_unit = JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT;
        desc_type_index_from_stream_port_input = JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER;
        desc_type_index_from_stream_port_output = JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER;

        adp_ref = new adp(frame, frame_len);
        struct jdksavdecc_eui64 guid;
        guid = adp_ref->get_entity_entity_id();
        end_station_guid = jdksavdecc_uint64_get(&guid, 0);
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
        uint16_t desc_type = JDKSAVDECC_DESCRIPTOR_ENTITY;
        uint16_t desc_index = 0;

        read_desc_init(desc_type, desc_index);

        return 0;
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

    uint64_t STDCALL end_station_imp::guid()
    {
        return end_station_guid;
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
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_read_descriptor aem_command_read_desc;
        ssize_t aem_command_read_desc_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /***************************** AECP Common Data ****************************/
        aem_command_read_desc.controller_entity_id = adp_ref->get_controller_guid();
        // Fill aem_command_read_desc.sequence_id in AEM Controller State Machine
        aem_command_read_desc.command_type = JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR;

        /******************************************************** AECP Message Specific Data ********************************************************/
        aem_command_read_desc.configuration_index = (desc_type == JDKSAVDECC_DESCRIPTOR_ENTITY || desc_type == JDKSAVDECC_DESCRIPTOR_CONFIGURATION) ?
                                                    0 : entity_desc_vec.at(current_entity_desc)->current_configuration();
        aem_command_read_desc.descriptor_type = desc_type;
        aem_command_read_desc.descriptor_index = desc_index;

        /************************** Fill frame payload with AECP data and send the frame *************************/
        aem_controller_state_machine_ref->ether_frame_init(end_station_mac, cmd_frame);
        aem_command_read_desc_returned = jdksavdecc_aem_command_read_descriptor_write(&aem_command_read_desc,
                                                                                      cmd_frame->payload,
                                                                                      ETHER_HDR_SIZE,
                                                                                      sizeof(cmd_frame->payload));

        if(aem_command_read_desc_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_read_desc_write error");
            assert(aem_command_read_desc_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, end_station_guid);
        system_queue_tx(notification_id, notification_flag, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;
    }

    int end_station_imp::proc_read_desc_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_read_descriptor_response aem_cmd_read_desc_resp;
        ssize_t aem_cmd_read_desc_resp_returned;
        uint32_t msg_type;
        bool u_field;
        uint16_t desc_type;
        configuration_descriptor_imp *config_desc_imp_ref = NULL;

        if(entity_desc_vec.size() >= 1 && entity_desc_vec.at(current_entity_desc)->config_desc_count() >= 1)
        {
            config_desc_imp_ref = dynamic_cast<configuration_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc));

            if(!config_desc_imp_ref)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Dynamic cast from base configuration_descriptor to derived configuration_descriptor_imp error");
            }
        }

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);
        aem_cmd_read_desc_resp_returned = jdksavdecc_aem_command_read_descriptor_response_read(&aem_cmd_read_desc_resp,
                                                                                               frame,
                                                                                               ETHER_HDR_SIZE,
                                                                                               frame_len);

        if(aem_cmd_read_desc_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_read_desc_res_read error");
            assert(aem_cmd_read_desc_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_read_desc_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_read_desc_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_read_desc_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type
        desc_type = jdksavdecc_uint16_get(frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_OFFSET_DESCRIPTOR);

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

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
                        read_desc_count--;
                    }
                    break;
            }
        }
        else
        {
            read_desc_count--;
        }

        if(store_descriptor)
        {
            switch(desc_type)
            {
                case JDKSAVDECC_DESCRIPTOR_ENTITY:
                    if(entity_desc_vec.size() == 0)
                    {
                        entity_desc_vec.push_back(new entity_descriptor_imp(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len));
                        current_config_desc = entity_desc_vec.at(current_entity_desc)->current_configuration();
                        read_desc_init(JDKSAVDECC_DESCRIPTOR_CONFIGURATION, 0); // Send READ_DESCRIPTOR command for CONFIGURATION descriptor to get the setup for the AVDECC Entity
                    }
                    break;

                case JDKSAVDECC_DESCRIPTOR_CONFIGURATION:
                    entity_desc_vec.at(current_entity_desc)->store_config_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_RUNNING;
                    break;

                case JDKSAVDECC_DESCRIPTOR_AUDIO_UNIT:
                    config_desc_imp_ref->store_audio_unit_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    read_desc_in_audio_unit_state = READ_DESC_IN_AUDIO_UNIT_RUNNING;
                    break;

                case JDKSAVDECC_DESCRIPTOR_STREAM_INPUT:
                    config_desc_imp_ref->store_stream_input_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_STREAM_OUTPUT:
                    config_desc_imp_ref->store_stream_output_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_JACK_INPUT:
                    config_desc_imp_ref->store_jack_input_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_JACK_OUTPUT:
                    config_desc_imp_ref->store_jack_output_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_AVB_INTERFACE:
                    config_desc_imp_ref->store_avb_interface_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_CLOCK_SOURCE:
                    config_desc_imp_ref->store_clock_source_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_LOCALE:
                    config_desc_imp_ref->store_locale_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_STRINGS:
                    config_desc_imp_ref->store_strings_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT:
                    config_desc_imp_ref->store_stream_port_input_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    read_desc_in_stream_port_input_state = READ_DESC_IN_STREAM_PORT_INPUT_RUNNING;
                    break;

                case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT:
                    config_desc_imp_ref->store_stream_port_output_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    read_desc_in_stream_port_output_state = READ_DESC_IN_STREAM_PORT_OUTPUT_RUNNING;
                    break;

                case JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER:
                    config_desc_imp_ref->store_audio_cluster_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_AUDIO_MAP:
                    config_desc_imp_ref->store_audio_map_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                case JDKSAVDECC_DESCRIPTOR_CLOCK_DOMAIN:
                    config_desc_imp_ref->store_clock_domain_desc(this, frame, ETHER_HDR_SIZE + JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR_RESPONSE_LEN, frame_len);
                    break;

                default:
                    log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "The descriptor is not implemented.");
                    break;
            }
        }

        /*
         * A state machine that iterates through the top level descriptors present in the CONFIGURATION descriptor.
         */
        switch(read_top_level_desc_in_config_state)
        {
            case READ_TOP_LEVEL_DESC_IN_CONFIG_IDLE:
                break;

            case READ_TOP_LEVEL_DESC_IN_CONFIG_RUNNING:
                {
                    if(read_desc_count == 0)
                    {
                        uint16_t total_num_of_desc = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->descriptor_counts_count();
                        uint16_t desc_count = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_desc_count_from_config_by_index(desc_type_index_from_config);
                        desc_type_from_config = entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_desc_type_from_config_by_index(desc_type_index_from_config);

                        for(int desc_count_index = 0; desc_count_index < desc_count; desc_count_index++)
                        {
                            read_desc_init(desc_type_from_config, desc_count_index);
                            read_desc_count++;
                        }

                        desc_type_index_from_config++;
    
                        if(desc_type_index_from_config >= total_num_of_desc)
                        {
                            read_top_level_desc_in_config_state = READ_TOP_LEVEL_DESC_IN_CONFIG_DONE;
                        }
                    }
                }
                break;

            case READ_TOP_LEVEL_DESC_IN_CONFIG_DONE:
                if(!read_top_level_desc_done)
                {
                    read_desc_in_locale_state = READ_DESC_IN_LOCALE_RUNNING;
                    read_top_level_desc_done = true;
                }
                break;        
        }

        if(read_top_level_desc_done)
        {
            switch(read_desc_in_locale_state)
            {
                case READ_DESC_IN_LOCALE_IDLE:
                    break;
            
                case READ_DESC_IN_LOCALE_RUNNING:
                    {
                        if(read_desc_count == 0)
                        {
                            if(config_desc_imp_ref->get_locale_desc_by_index(0)) // Check if LOCALE descriptor is present in the top level descriptor
                            {
                                uint16_t num_of_string_desc = config_desc_imp_ref->get_locale_desc_by_index(0)->number_of_strings();
                                for(int i = 0; i < num_of_string_desc; i++)
                                {
                                    read_desc_init(JDKSAVDECC_DESCRIPTOR_STRINGS, i); // Send a READ_DESCRIPTOR command for the STRINGS descriptor as part of the End Station initialization
                                    read_desc_count++;
                                }

                                read_desc_in_locale_state = READ_DESC_IN_LOCALE_DONE;
                            }
                        }
                    }
                    break;

                case READ_DESC_IN_LOCALE_DONE:
                    read_desc_in_locale_done = true;
                    break;        
            }
        }

        if(read_desc_in_locale_done)
        {
            switch(read_desc_in_audio_unit_state)
            {
                case READ_DESC_IN_AUDIO_UNIT_IDLE:
                    break;

                case READ_DESC_IN_AUDIO_UNIT_STARTING:
                    read_desc_in_audio_unit_state = READ_DESC_IN_AUDIO_UNIT_RUNNING;
                    break;

                case READ_DESC_IN_AUDIO_UNIT_RUNNING:
                {
                    audio_unit_descriptor_imp *audio_unit_desc_imp_ref = 
                                dynamic_cast<audio_unit_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_audio_unit_desc_by_index(0));
                    // TODO: Handle multiple audio units

                    if (!audio_unit_desc_imp_ref) assert(audio_unit_desc_imp_ref == NULL);

                    switch(desc_type_index_from_audio_unit)
                    {
                        case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT:
                        {
                            if(read_desc_count == 0)
                            {
                                uint16_t num_desc = audio_unit_desc_imp_ref->number_of_stream_input_ports();
                                uint16_t base_desc = audio_unit_desc_imp_ref->base_stream_input_port();
                                if (num_desc)
                                {
                                    for(int i = base_desc; i < base_desc+num_desc; i++)
                                    {
                                        read_desc_init(JDKSAVDECC_DESCRIPTOR_STREAM_PORT_INPUT, i);
                                        read_desc_count++;
                                    }
                                }

                                desc_type_index_from_audio_unit = JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT;
                            }
                        }
                        break;
                        
                        case JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT:
                        {
                            if(read_desc_count == 0)
                            {
                                uint16_t num_desc = audio_unit_desc_imp_ref->number_of_stream_output_ports();
                                uint16_t base_desc = audio_unit_desc_imp_ref->base_stream_output_port();
                                if (num_desc)
                                {
                                    for(int i = base_desc; i < base_desc+num_desc; i++)
                                    {
                                        read_desc_init(JDKSAVDECC_DESCRIPTOR_STREAM_PORT_OUTPUT, i);
                                        read_desc_count++;
                                    }
                                }
                                read_desc_in_audio_unit_state = READ_DESC_IN_AUDIO_UNIT_DONE;
                            }
                        }
                        break;
                    }
                }
                break;

                case READ_DESC_IN_AUDIO_UNIT_DONE:
                    read_desc_in_audio_unit_done = true;
                    break;
            }
        }

        if(read_desc_in_audio_unit_done)
        {
            switch(read_desc_in_stream_port_input_state)
            {
                case READ_DESC_IN_STREAM_PORT_INPUT_IDLE:
                    break;

                case READ_DESC_IN_STREAM_PORT_INPUT_RUNNING:
                    {
                        stream_port_input_descriptor_imp *stream_port_input_desc_imp_ref = 
                            dynamic_cast<stream_port_input_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_port_input_desc_by_index(0));

                        if(!stream_port_input_desc_imp_ref)
                        {
                            assert(stream_port_input_desc_imp_ref == NULL);
                        }

                        switch(desc_type_index_from_stream_port_input)
                        {
                            case JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER:
                                {
                                    if(read_desc_count == 0)
                                    {
                                        uint16_t num_desc = stream_port_input_desc_imp_ref->number_of_clusters();
                                        uint16_t base_desc = stream_port_input_desc_imp_ref->base_cluster();

                                        if(num_desc)
                                        {
                                            for(int i = 0; i < base_desc + num_desc; i++)
                                            {
                                                read_desc_init(JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER, i);
                                                read_desc_count++;
                                            }
                                        }

                                        desc_type_index_from_stream_port_input = JDKSAVDECC_DESCRIPTOR_AUDIO_MAP;
                                    }
                                }
                                break;

                            case JDKSAVDECC_DESCRIPTOR_AUDIO_MAP:
                                {
                                    if(read_desc_count == 0)
                                    {
                                        uint16_t num_desc = stream_port_input_desc_imp_ref->number_of_maps();
                                        uint16_t base_desc = stream_port_input_desc_imp_ref->base_map();

                                        if(num_desc)
                                        {
                                            for(int i = 0; i < base_desc + num_desc; i++)
                                            {
                                                read_desc_init(JDKSAVDECC_DESCRIPTOR_AUDIO_MAP, i);
                                                read_desc_count++;
                                            }
                                        }

                                        desc_type_index_from_stream_port_input = JDKSAVDECC_DESCRIPTOR_CONTROL;
                                    }
                                }
                            break;

                            case JDKSAVDECC_DESCRIPTOR_CONTROL:
                                {
                                    if(read_desc_count == 0)
                                    {
                                        uint16_t num_desc = stream_port_input_desc_imp_ref->number_of_controls();
                                        uint16_t base_desc = stream_port_input_desc_imp_ref->base_control();

                                        if(num_desc)
                                        {
                                            for(int i = 0; i < base_desc + num_desc; i++)
                                            {
                                                read_desc_init(JDKSAVDECC_DESCRIPTOR_CONTROL, i);
                                                read_desc_count++;
                                            }
                                        }

                                        read_desc_in_stream_port_input_state = READ_DESC_IN_STREAM_PORT_INPUT_DONE;
                                    }
                                }
                            break;
                        }
                    }
                    break;

                case READ_DESC_IN_STREAM_PORT_INPUT_DONE:
                    read_desc_in_stream_port_input_done = true;
                    break;        
            }
        }

        if(read_desc_in_stream_port_input_done)
        {
            switch(read_desc_in_stream_port_output_state)
            {
                case READ_DESC_IN_STREAM_PORT_OUTPUT_IDLE:
                    break;

                case READ_DESC_IN_STREAM_PORT_OUTPUT_RUNNING:
                    {
                        stream_port_output_descriptor_imp *stream_port_output_desc_imp_ref = 
                            dynamic_cast<stream_port_output_descriptor_imp *>(entity_desc_vec.at(current_entity_desc)->get_config_desc_by_index(current_config_desc)->get_stream_port_output_desc_by_index(0));

                        if(!stream_port_output_desc_imp_ref)
                        {
                            assert(stream_port_output_desc_imp_ref == NULL);
                        }

                        switch(desc_type_index_from_stream_port_output)
                        {
                            case JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER:
                                {
                                    if(read_desc_count == 0)
                                    {
                                        uint16_t num_desc = stream_port_output_desc_imp_ref->number_of_clusters();
                                        uint16_t base_desc = stream_port_output_desc_imp_ref->base_cluster();

                                        if(num_desc)
                                        {
                                            for(int i = 0; i < base_desc + num_desc; i++)
                                            {
                                                read_desc_init(JDKSAVDECC_DESCRIPTOR_AUDIO_CLUSTER, i);
                                                read_desc_count++;
                                            }
                                        }

                                        desc_type_index_from_stream_port_output = JDKSAVDECC_DESCRIPTOR_AUDIO_MAP;
                                    }
                                }
                                break;

                            case JDKSAVDECC_DESCRIPTOR_AUDIO_MAP:
                                {
                                    if(read_desc_count == 0)
                                    {
                                        uint16_t num_desc = stream_port_output_desc_imp_ref->number_of_maps();
                                        uint16_t base_desc = stream_port_output_desc_imp_ref->base_map();

                                        if(num_desc)
                                        {
                                            for(int i = 0; i < base_desc + num_desc; i++)
                                            {
                                                read_desc_init(JDKSAVDECC_DESCRIPTOR_AUDIO_MAP, i);
                                                read_desc_count++;
                                            }
                                        }

                                        desc_type_index_from_stream_port_output = JDKSAVDECC_DESCRIPTOR_CONTROL;
                                    }
                                }
                            break;

                            case JDKSAVDECC_DESCRIPTOR_CONTROL:
                                {
                                    if(read_desc_count == 0)
                                    {
                                        uint16_t num_desc = stream_port_output_desc_imp_ref->number_of_controls();
                                        uint16_t base_desc = stream_port_output_desc_imp_ref->base_control();

                                        if(num_desc)
                                        {
                                            for(int i = 0; i < base_desc + num_desc; i++)
                                            {
                                                read_desc_init(JDKSAVDECC_DESCRIPTOR_CONTROL, i);
                                                read_desc_count++;
                                            }
                                        }

                                        read_desc_in_stream_port_output_state = READ_DESC_IN_STREAM_PORT_OUTPUT_DONE;
                                        notification_imp_ref->post_notification_msg(END_STATION_READ_COMPLETED, end_station_guid, 0, 0, 0, 0, NULL);
                                    }
                                }
                            break;
                        }
                    }
                    break;

                case READ_DESC_IN_STREAM_PORT_OUTPUT_DONE:
                    read_desc_in_stream_port_output_done = true;
                    break;        
            }
        }

        free(cmd_frame);
        return 0;
    }

    int STDCALL end_station_imp::send_entity_avail_cmd(void *notification_id)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_entity_available aem_cmd_entity_avail;
        ssize_t aem_cmd_entity_avail_returned;
        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));

        /**************************** AECP Common Data ****************************/
        aem_cmd_entity_avail.controller_entity_id = adp_ref->get_controller_guid();
        // Fill aem_cmd_entity_avail.sequence_id in AEM Controller State Machine
        aem_cmd_entity_avail.command_type = JDKSAVDECC_AEM_COMMAND_ENTITY_AVAILABLE;

        /**************************** Fill frame payload with AECP data and send the frame *************************/
        aem_controller_state_machine_ref->ether_frame_init(end_station_mac, cmd_frame);
        aem_cmd_entity_avail_returned = jdksavdecc_aem_command_entity_available_write(&aem_cmd_entity_avail,
                                                                                      cmd_frame->payload,
                                                                                      ETHER_HDR_SIZE,
                                                                                      sizeof(cmd_frame->payload));

        if(aem_cmd_entity_avail_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_entity_avail_write error\n");
            assert(aem_cmd_entity_avail_returned >= 0);
            return -1;
        }

        aem_controller_state_machine_ref->common_hdr_init(cmd_frame, end_station_guid);
        system_queue_tx(notification_id, CMD_WITH_NOTIFICATION, cmd_frame->payload, cmd_frame->length);

        free(cmd_frame);
        return 0;
    }

    int end_station_imp::proc_entity_avail_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        struct jdksavdecc_frame *cmd_frame;
        struct jdksavdecc_aem_command_entity_available_response aem_cmd_entity_avail_resp;
        ssize_t aem_cmd_entity_avail_resp_returned = 0;
        uint32_t msg_type = 0;
        bool u_field = false;

        cmd_frame = (struct jdksavdecc_frame *)malloc(sizeof(struct jdksavdecc_frame));
        memcpy(cmd_frame->payload, frame, frame_len);

        aem_cmd_entity_avail_resp_returned = jdksavdecc_aem_command_entity_available_response_read(&aem_cmd_entity_avail_resp,
                                                                                                   frame,
                                                                                                   ETHER_HDR_SIZE,
                                                                                                   frame_len);

        if(aem_cmd_entity_avail_resp_returned < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "aem_cmd_entity_avail_resp_read error\n");
            assert(aem_cmd_entity_avail_resp_returned >= 0);
            return -1;
        }

        msg_type = aem_cmd_entity_avail_resp.aem_header.aecpdu_header.header.message_type;
        status = aem_cmd_entity_avail_resp.aem_header.aecpdu_header.header.status;
        u_field = aem_cmd_entity_avail_resp.command_type >> 15 & 0x01; // u_field = the msb of the uint16_t command_type

        aem_controller_state_machine_ref->update_inflight_for_rcvd_resp(notification_id, msg_type, u_field, cmd_frame);

        free(cmd_frame);
        return 0;
    }

    int end_station_imp::proc_rcvd_aem_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        uint16_t cmd_type;
        uint16_t desc_type;
        uint16_t desc_index;
        cmd_type = jdksavdecc_aecpdu_aem_get_command_type(frame, ETHER_HDR_SIZE + JDKSAVDECC_COMMON_CONTROL_HEADER_LEN);

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
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_STREAM_INFO command.");

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

            default:
                notification_imp_ref->post_notification_msg(NO_MATCH_FOUND, 0, cmd_type, 0, 0, 0, 0);
                break;
        }

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
}
