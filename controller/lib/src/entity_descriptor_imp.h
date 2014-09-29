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
 * entity_descriptor_imp.h
 *
 * ENTITY descriptor implementation class
 */

#pragma once

#include "descriptor_base_imp.h"
#include "configuration_descriptor_imp.h"
#include "entity_descriptor.h"
#include "entity_descriptor_response_imp.h"

namespace avdecc_lib
{
    class entity_descriptor_imp : public entity_descriptor, public virtual descriptor_base_imp
    {
    private:
        std::vector<configuration_descriptor_imp *> config_desc_vec; // Store a list of CONFIGURATION descriptor objects
        struct jdksavdecc_aem_command_acquire_entity_response aem_cmd_acquire_entity_resp; // Store the response received after sending a ACQUIRE_ENTITY command.
        struct jdksavdecc_aem_command_lock_entity_response aem_cmd_lock_entity_resp; // Store the response received after sending a LOCK_ENTITY command.
        struct jdksavdecc_aem_command_reboot_response aem_cmd_reboot_resp;
        uint16_t m_type;
        uint16_t m_index;
    public:
        entity_descriptor_imp(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        virtual ~entity_descriptor_imp();
        
        entity_descriptor_response_imp *resp;

        uint16_t STDCALL descriptor_type() const;
        uint16_t STDCALL descriptor_index() const;
        uint16_t STDCALL current_configuration();
        void store_config_desc(end_station_imp *end_station_obj, const uint8_t *frame, ssize_t pos, size_t frame_len);
        size_t STDCALL config_desc_count();
        configuration_descriptor * STDCALL get_config_desc_by_index(uint16_t config_desc_index);
        entity_descriptor_response * STDCALL get_entity_response();
        uint32_t STDCALL acquire_entity_flags();
        uint64_t STDCALL acquire_entity_owner_entity_id();
        uint32_t STDCALL lock_entity_flags();
        uint64_t STDCALL lock_entity_locked_entity_id();

        int STDCALL send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag);
        int proc_acquire_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag);
        int proc_lock_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_reboot_cmd(void *notification_id);
        int proc_reboot_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int STDCALL send_set_config_cmd();
        int proc_set_config_resp();

        int STDCALL send_get_config_cmd();
        int proc_get_config_resp();
    };
}
