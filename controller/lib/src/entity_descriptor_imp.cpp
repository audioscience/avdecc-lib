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
    entity_descriptor_imp::entity_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len) : descriptor_base_imp(end_station_obj, frame, frame_len, pos) {}

    entity_descriptor_imp::~entity_descriptor_imp()
    {
        for(uint32_t config_vec_index = 0; config_vec_index < config_desc_vec.size(); config_vec_index++)
        {
            delete config_desc_vec.at(config_vec_index);
        }
    }

    uint16_t STDCALL entity_descriptor_imp::descriptor_type() const
    {
        assert(jdksavdecc_descriptor_entity_get_descriptor_type(resp_ref->get_buffer(), resp_ref->get_pos()) == JDKSAVDECC_DESCRIPTOR_ENTITY);
        return jdksavdecc_descriptor_entity_get_descriptor_type(resp_ref->get_buffer(), resp_ref->get_pos());
    }

    uint16_t STDCALL entity_descriptor_imp::descriptor_index() const
    {
        assert(jdksavdecc_descriptor_entity_get_descriptor_index(resp_ref->get_buffer(), resp_ref->get_pos()) == 0);
        return jdksavdecc_descriptor_entity_get_descriptor_index(resp_ref->get_buffer(), resp_ref->get_pos());
    }
    
    uint16_t STDCALL entity_descriptor_imp::current_configuration()
    {
        return jdksavdecc_descriptor_entity_get_current_configuration(resp_ref->get_buffer(), resp_ref->get_pos());
    }

    entity_descriptor_response * STDCALL entity_descriptor_imp::get_entity_response()
    {
        std::lock_guard<std::mutex> guard(base_end_station_imp_ref->locker); //mutex lock end station
        return resp = new entity_descriptor_response_imp(resp_ref->get_buffer(),
                                                         resp_ref->get_size(), resp_ref->get_pos());
    }

    void entity_descriptor_imp::store_config_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len)
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

        if(is_valid)
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

    int STDCALL entity_descriptor_imp::send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag)
    {
        return default_send_acquire_entity_cmd(this, notification_id, acquire_entity_flag);
    }

    int entity_descriptor_imp::proc_acquire_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        return default_proc_acquire_entity_resp(aem_cmd_acquire_entity_resp, notification_id, frame, frame_len, status);
    }

    int STDCALL entity_descriptor_imp::send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag)
    {
        return default_send_lock_entity_cmd(this, notification_id, lock_entity_flag);
    }

    int STDCALL entity_descriptor_imp::send_reboot_cmd(void *notification_id)
    {
        return default_send_reboot_cmd(this, notification_id);
    }

    int entity_descriptor_imp::proc_lock_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        return default_proc_lock_entity_resp(aem_cmd_lock_entity_resp, notification_id, frame, frame_len, status);
    }

    int entity_descriptor_imp::proc_reboot_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status)
    {
        return default_proc_reboot_resp(aem_cmd_reboot_resp, notification_id, frame, frame_len, status);
    }

    int STDCALL entity_descriptor_imp::send_set_config_cmd()
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_CONFIGURATION command.");

        return 0;
    }

    int entity_descriptor_imp::proc_set_config_resp()
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement SET_CONFIGURATION response.");

        return 0;
    }

    int STDCALL entity_descriptor_imp::send_get_config_cmd()
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement GET_CONFIGURATION command.");

        return 0;
    }

    int entity_descriptor_imp::proc_get_config_resp()
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Need to implement GET_CONFIGURATION response.");

        return 0;
    }
}
