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
 * notification.cpp
 *
 * Notification base class implementation, which is called by AVDECC LIB modules to generate notification messages.
 */

#include "avdecc_lib_os.h"
#include "enumeration.h"
#include "notification.h"

namespace avdecc_lib
{
    extern "C" void default_notification(void *notification_user_obj, int32_t notification_type, uint64_t entity_id, uint16_t cmd_type,
                                         uint16_t desc_type, uint16_t desc_index, uint32_t status, void *notification_id) {}

    notification::notification()
    {
        notifications = NO_MATCH_FOUND;
        read_index = 0;
        write_index = 0;
        notification_callback = default_notification;
        user_obj = NULL;
        missed_notification_event_cnt = 0;
    }

    notification::~notification() {}

    void notification::post_notification_msg(int32_t notification_type, uint64_t entity_id, uint16_t cmd_type, uint16_t desc_type, uint16_t desc_index, uint32_t cmd_status, void *notification_id)
    {
        uint32_t index;

        if((write_index - read_index) > NOTIFICATION_BUF_COUNT)
        {
            missed_notification_event_cnt++;
            return;
        }

        if(notification_type == NO_MATCH_FOUND || notification_type == END_STATION_CONNECTED ||
           notification_type == END_STATION_DISCONNECTED || notification_type == COMMAND_TIMEOUT ||
           notification_type == RESPONSE_RECEIVED || notification_type == END_STATION_READ_COMPLETED)
        {
            index = InterlockedExchangeAdd(&write_index, 1);
            notification_buf[index % NOTIFICATION_BUF_COUNT].notification_type = notification_type;
            notification_buf[index % NOTIFICATION_BUF_COUNT].entity_id = entity_id;
            notification_buf[index % NOTIFICATION_BUF_COUNT].cmd_type = cmd_type;
            notification_buf[index % NOTIFICATION_BUF_COUNT].desc_type = desc_type;
            notification_buf[index % NOTIFICATION_BUF_COUNT].desc_index = desc_index;
            notification_buf[index % NOTIFICATION_BUF_COUNT].cmd_status = cmd_status;
            notification_buf[index % NOTIFICATION_BUF_COUNT].notification_id = notification_id;

            post_notification_event();
        }
    }

    void notification::set_notification_callback(void (*new_notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *), void *p)
    {
        notification_callback = new_notification_callback;
        user_obj = p;
    }

    uint32_t notification::missed_notification_event_count()
    {
        return missed_notification_event_cnt;
    }
}
