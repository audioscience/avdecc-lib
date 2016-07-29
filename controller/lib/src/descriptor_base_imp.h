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
#include "descriptor_response_base_imp.h"
#include "descriptor_base_get_name_response_imp.h"

namespace avdecc_lib
{
class end_station_imp;
class response_frame;

class descriptor_base_imp : public virtual descriptor_base
{
protected:
    descriptor_response_base_imp * resp_base;
    descriptor_base_get_name_response_imp * get_name_resp;
    end_station_imp * base_end_station_imp_ref;
    std::vector<descriptor_field_imp *> m_fields;
    response_frame * resp_ref;
    uint16_t desc_type;
    uint16_t desc_index;
    
    ///
    /// Store the lock status - the last received lock entity flags.
    /// This is not guaranteed to be the actual status as another
    /// controller may have locked the Entity before this controller
    /// is notified of the change.
    ///
    uint32_t last_rcvd_lock_entity_flags = 0x1; // initialized to unlocked
    
    ///
    /// Store the acquire status - the last received acquire entity flags.
    /// This is not guaranteed to be the actual status as another
    /// controller may have acquired the Entity before this controller
    /// is notified of the change.
    ///
    uint32_t last_rcvd_acquire_entity_flags = 0x80000000; // initialized to released
    
    ///
    /// Store the owning guid - the last received acquire or lock owner.
    /// This is not guaranteed to be the actual owner as another
    /// controller may have locked or acquired the Entity before this controller
    /// is notified of the change.
    ///
    uint64_t owning_guid = 0;
public:
    descriptor_base_imp(end_station_imp * base, const uint8_t * frame, size_t size, ssize_t pos);
    virtual ~descriptor_base_imp();

    uint16_t STDCALL descriptor_type() const;
    uint16_t STDCALL descriptor_index() const;
    virtual uint16_t STDCALL localized_description();
    bool STDCALL get_permission(int flag);
    uint64_t STDCALL get_owning_guid();

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
    ///
    /// Replace the frame for counters/commands.
    ///
    virtual void STDCALL replace_frame(const uint8_t * frame, ssize_t pos, size_t size);

    ///
    /// Replace the frame for descriptors.
    ///
    virtual void STDCALL replace_desc_frame(const uint8_t * frame, ssize_t pos, size_t size);

    ///
    /// Get the flags after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
    ///
    virtual uint32_t STDCALL acquire_entity_flags();

    ///
    /// Get the owner id after sending a ACQUIRE_ENTITY command and receiving a response back for the command.
    ///
    virtual uint64_t STDCALL acquire_entity_owner_entity_id();

    ///
    /// Get the flags after sending a LOCK_ENTITY command and receiving a response back for the command.
    ///
    virtual uint32_t STDCALL lock_entity_flags();

    ///
    /// Get the locked entity id after sending a LOCK_ENTITY command and receiving a response back for the command.
    ///
    virtual uint64_t STDCALL lock_entity_locked_entity_id();

    virtual int STDCALL send_acquire_entity_cmd(void * notification_id, uint32_t acquire_entity_flag);
    virtual int proc_acquire_entity_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int default_send_acquire_entity_cmd(descriptor_base_imp * desc_base_imp_ref, void * notification_id, uint32_t acquire_entity_flag);
    int default_proc_acquire_entity_resp(struct jdksavdecc_aem_command_acquire_entity_response & aem_cmd_acquire_entity_resp,
                                         void *& notification_id,
                                         const uint8_t * frame,
                                         size_t frame_len,
                                         int & status);

    virtual int STDCALL send_lock_entity_cmd(void * notification_id, uint32_t lock_entity_flag);
    virtual int proc_lock_entity_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int default_send_lock_entity_cmd(descriptor_base_imp * descriptor_base_imp_ref, void * notification_id, uint32_t lock_entity_flag);
    int default_proc_lock_entity_resp(struct jdksavdecc_aem_command_lock_entity_response & aem_cmd_lock_entity_resp,
                                      void *& notification_id,
                                      const uint8_t * frame,
                                      size_t frame_len,
                                      int & status);

    virtual int STDCALL send_reboot_cmd(void * notification_id);
    virtual int proc_reboot_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    int default_send_reboot_cmd(descriptor_base_imp * descriptor_base_imp_ref, void * notification_id);
    int default_proc_reboot_resp(struct jdksavdecc_aem_command_reboot_response & aem_cmd_reboot_resp,
                                 void *& notification_id,
                                 const uint8_t * frame,
                                 size_t frame_len,
                                 int & status);

    virtual int STDCALL send_set_name_cmd(void * notification_id,
                                          uint16_t name_index,
                                          uint16_t config_index,
                                          const struct avdecc_lib_name_string64 * new_name);
    virtual int proc_set_name_resp(void *& notification_id,
                                   const uint8_t * frame,
                                   size_t frame_len,
                                   int & status);

    int default_send_set_name_cmd(descriptor_base_imp * desc_base_imp_ref,
                                  void * notification_id,
                                  uint16_t name_index,
                                  uint16_t config_index,
                                  const struct avdecc_lib_name_string64 * name);

    int default_proc_set_name_resp(struct jdksavdecc_aem_command_set_name_response & aem_cmd_set_name_resp,
                                   void *& notification_id,
                                   const uint8_t * frame,
                                   size_t frame_len,
                                   int & status);

    virtual int STDCALL send_get_name_cmd(void * notification_id,
                                          uint16_t name_index,
                                          uint16_t config_index);

    virtual int proc_get_name_resp(void *& notification_id,
                                   const uint8_t * frame,
                                   size_t frame_len,
                                   int & status);

    int default_send_get_name_cmd(descriptor_base_imp * desc_base_imp_ref,
                                  void * notification_id,
                                  uint16_t name_index,
                                  uint16_t config_index);

    int default_proc_get_name_resp(struct jdksavdecc_aem_command_get_name_response & aem_cmd_get_name_resp,
                                   void *& notification_id,
                                   const uint8_t * frame,
                                   size_t frame_len,
                                   int & status);

    descriptor_response_base * STDCALL get_descriptor_response();
    descriptor_base_get_name_response * STDCALL get_name_response();
};

bool operator==(const descriptor_base_imp & n1, const descriptor_base_imp & n2);
bool operator<(const descriptor_base_imp & n1, const descriptor_base_imp & n2);
}
