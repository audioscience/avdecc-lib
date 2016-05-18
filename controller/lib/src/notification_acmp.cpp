/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2016 AudioScience Inc.
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
 * notification_acmp.cpp
 *
 * ACMP Notification base class implementation, which is called by AVDECC LIB modules to generate notification messages.
 */

#include "avdecc_lib_os.h"
#include "enumeration.h"
#include "notification_acmp.h"

namespace avdecc_lib
{
extern "C" void default_acmp_notification(void * notification_user_obj, int32_t notification_type, uint16_t cmd_type,
                                          uint64_t talker_entity_id, uint16_t talker_unique_id, uint64_t listener_entity_id,
                                          uint16_t listener_unique_id, uint32_t status, void * notification_id)
{
    (void)notification_user_obj; //unused
    (void)notification_type;
    (void)cmd_type;
    (void)talker_entity_id;
    (void)talker_unique_id;
    (void)listener_entity_id;
    (void)listener_unique_id;
    (void)status;
    (void)notification_id;
}

notification_acmp::notification_acmp()
{
    notifications = NO_MATCH_FOUND;
    read_index = 0;
    write_index = 0;
    acmp_notification_callback = default_acmp_notification;
    user_obj = NULL;
    missed_notification_event_cnt = 0;
}

notification_acmp::~notification_acmp() {}

void notification_acmp::post_acmp_notification_msg(int32_t notification_type, uint16_t cmd_type, uint64_t talker_entity_id,
                                                   uint16_t talker_unique_id, uint64_t listener_entity_id,
                                                   uint16_t listener_unique_id, uint32_t cmd_status, void * notification_id)
{
    uint32_t index;

    if ((write_index - read_index) > NOTIFICATION_BUF_COUNT)
    {
        missed_notification_event_cnt++;
        return;
    }

    if (notification_type == BROADCAST_RESPONSE_RECEIVED)
    {
        index = InterlockedExchangeAdd(&write_index, 1);
        notification_buf[index % NOTIFICATION_BUF_COUNT].notification_type = notification_type;
        notification_buf[index % NOTIFICATION_BUF_COUNT].cmd_type = cmd_type;
        notification_buf[index % NOTIFICATION_BUF_COUNT].talker_entity_id = talker_entity_id;
        notification_buf[index % NOTIFICATION_BUF_COUNT].talker_unique_id = talker_unique_id;
        notification_buf[index % NOTIFICATION_BUF_COUNT].listener_entity_id = listener_entity_id;
        notification_buf[index % NOTIFICATION_BUF_COUNT].listener_unique_id = listener_unique_id;
        notification_buf[index % NOTIFICATION_BUF_COUNT].cmd_status = cmd_status;
        notification_buf[index % NOTIFICATION_BUF_COUNT].notification_id = notification_id;

        post_acmp_notification_event();
    }
}

void notification_acmp::set_acmp_notification_callback(void (*new_acmp_notification_callback)(void *, int32_t, uint16_t,
                                                                                              uint64_t, uint16_t, uint64_t,
                                                                                              uint16_t, uint32_t, void *),
                                                       void * p)
{
    acmp_notification_callback = new_acmp_notification_callback;
    user_obj = p;
}

uint32_t notification_acmp::missed_notification_event_count()
{
    return missed_notification_event_cnt;
}
}
