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
 * descriptor_base_imp.h
 *
 * Descriptor base implementation class
 */

#pragma once
#if defined _MSC_VER
#pragma warning(disable : 4250) // Disable warning message C4250: inherits via dominance
#endif

#include <vector>
#include "jdksavdecc_util.h"
#include "jdksavdecc_aem_command.h"
#include "jdksavdecc_aem_descriptor.h"
#include "descriptor_base.h"
#include "descriptor_field_imp.h"

namespace avdecc_lib
{
    class end_station_imp;

    class descriptor_base_imp : public virtual descriptor_base
    {
    protected:
        end_station_imp *base_end_station_imp_ref;
        std::vector<descriptor_field_imp *>m_fields;

    public:
        descriptor_base_imp(end_station_imp *base);
        virtual ~descriptor_base_imp();

        virtual uint16_t STDCALL descriptor_type() const;
        virtual uint16_t STDCALL descriptor_index() const;
        virtual uint8_t * STDCALL object_name();
        virtual uint16_t STDCALL localized_description();

        size_t STDCALL field_count() const
        {
            return m_fields.size();
        };

        descriptor_field * STDCALL field(size_t index) const
        {
            if (index < m_fields.size())
                return m_fields.at(index);
            else
                return nullptr;
        };

        /**
         * Get the flags after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
         */
        virtual uint32_t STDCALL acquire_entity_flags();

        /**
         * Get the owner id after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
         */
        virtual uint64_t STDCALL acquire_entity_owner_entity_id();

        /**
         * Get the flags after sending a LOCK_ENTITY command and receiving a response back for the command.
         */
        virtual uint32_t STDCALL lock_entity_flags();

        /**
         * Get the locked entity id after sending a LOCK_ENTITY command and receiving a response back for the command.
         */
        virtual uint64_t STDCALL lock_entity_locked_entity_id();

        virtual int STDCALL send_acquire_entity_cmd(void *notification_id, uint32_t acquire_entity_flag);
        virtual int proc_acquire_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int default_send_acquire_entity_cmd(descriptor_base_imp *desc_base_imp_ref, void *notification_id, uint32_t acquire_entity_flag);
        int default_proc_acquire_entity_resp(struct jdksavdecc_aem_command_acquire_entity_response &aem_cmd_acquire_entity_resp,
                                             void *&notification_id,
                                             const uint8_t *frame,
                                             size_t frame_len,
                                             int &status);

        virtual int STDCALL send_lock_entity_cmd(void *notification_id, uint32_t lock_entity_flag);
        virtual int proc_lock_entity_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int default_send_lock_entity_cmd(descriptor_base_imp *descriptor_base_imp_ref, void *notification_id, uint32_t lock_entity_flag);
        int default_proc_lock_entity_resp(struct jdksavdecc_aem_command_lock_entity_response &aem_cmd_lock_entity_resp,
                                          void *&notification_id,
                                          const uint8_t *frame,
                                          size_t frame_len,
                                          int &status);

        virtual int STDCALL send_reboot_cmd(void *notification_id);
        virtual int proc_reboot_resp(void *&notification_id, const uint8_t *frame, size_t frame_len, int &status);

        int default_send_reboot_cmd(descriptor_base_imp *descriptor_base_imp_ref, void *notification_id);
        int default_proc_reboot_resp(struct jdksavdecc_aem_command_reboot_response &aem_cmd_reboot_resp,
                                     void *&notification_id,
                                     const uint8_t *frame,
                                     size_t frame_len,
                                     int &status);

        virtual int STDCALL send_set_name_cmd(void *notification_id, uint16_t name_index, uint16_t config_index, char * new_name);
        virtual int proc_set_name_resp(uint8_t *base_pointer, uint16_t frame_len);

        virtual int STDCALL send_get_name_cmd(void *notification_id, uint16_t name_index, uint16_t config_index);
        virtual int proc_get_name_resp(uint8_t *base_pointer, uint16_t frame_len);
    };

    bool operator== (const descriptor_base_imp &n1, const descriptor_base_imp &n2);
    bool operator< (const descriptor_base_imp &n1, const descriptor_base_imp &n2);
}

