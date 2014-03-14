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
 * notification.h
 *
 * Notification base class, which is called by AVDECC LIB modules to generate notification messages.
 */

#pragma once


#include <stdint.h>

namespace avdecc_lib
{
    class notification
    {
    public:
        notification();

        ~notification();

        /**
         * AVDECC LIB modules call this function to generate a notification message.
         */
        void post_notification_msg(int32_t notification_type, uint64_t entity_id, uint16_t cmd_type, uint16_t desc_type, uint16_t desc_index, uint32_t cmd_status, void *notification_id);

        /**
         * Change the notification callback function to a new post_notification_msg callback function.
         */
        void set_notification_callback(void (*new_notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *), void *);

        /**
         * Get the number of missed notifications that exceeds the notification buffer count.
         */
        uint32_t missed_notification_event_count();

	protected:
        int32_t notifications;
        uint32_t read_index;
        uint32_t write_index;
        void (*notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *);
        void *user_obj;
        uint32_t missed_notification_event_cnt;

        enum
        {
            NOTIFICATION_BUF_COUNT = 32
        };

        struct notification_data
        {
            int32_t notification_type;
            uint64_t entity_id;
            uint16_t cmd_type;
            uint16_t desc_type;
            uint16_t desc_index;
            uint32_t cmd_status;
            void *notification_id;
        };

        struct notification_data notification_buf[NOTIFICATION_BUF_COUNT];

        /**
         * Release sempahore so that notification callback function is called.
         */
        virtual void post_notification_event() = 0;

    };
}
