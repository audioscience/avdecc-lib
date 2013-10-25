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
 * Notification implementation, which is called by AVDECC LIB modules to generate notification messages.
 */

#include <iostream>
#include "enumeration.h"
#include "notification.h"

extern "C" void default_notification(void *user_obj, int32_t notification, uint64_t guid, uint16_t cmd_type,
                                     uint16_t desc_type, uint16_t desc_index, void *notification_id)
{
        printf("default_notification (%d, %llx, %d, %d, %d, %d)\n", notification, guid, cmd_type, desc_type, desc_index, notification_id);
}

namespace avdecc_lib
{
        uint32_t notification::read_index = 0;
        uint32_t notification::write_index = 0;
        void (*notification::notification_callback) (void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, void *);
        void *notification::user_obj;
        HANDLE notification::poll_events[2];

        notification *notification_ref = new notification();

        notification::notification()
        {
                notifications = avdecc_lib::NO_MATCH_FOUND;
                notification_callback = default_notification;
                user_obj = NULL;

                notification_thread_init(); // Start notifying thread
        }

        notification::~notification() {}

        int notification::notification_thread_init()
        {
                poll_events[NOTIFICATION_EVENT] = CreateSemaphore(NULL, 0, 32767, NULL);
                poll_events[KILL_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);

                h_thread = CreateThread(NULL, // Default security descriptor
                                        0, // Default stack size
                                        process_notification_thread, // Point to the start address of the thread
                                        &notification_buf, // Data to be passed to the thread
                                        0, // Flag controlling the creation of the thread
                                        &thread_id // Thread identifier
                                       );

                if (h_thread == NULL)
                {
                        exit(EXIT_FAILURE);
                }

                return 0;
        }

        DWORD WINAPI notification::process_notification_thread(LPVOID lpParam)
        {
                DWORD dwEvent;
                struct notification_data *data = (struct notification_data *)lpParam;

                while (true)
                {
                        dwEvent = WaitForMultipleObjects(2, poll_events, FALSE, INFINITE);

                        if(dwEvent == (WAIT_OBJECT_0 + NOTIFICATION_EVENT))
                        {
                                if((write_index - read_index) > 0)
                                {
                                        notification_callback(user_obj,
                                                              data[read_index % NOTIFICATION_BUF_COUNT].notifying,
                                                              data[read_index % NOTIFICATION_BUF_COUNT].guid,
                                                              data[read_index % NOTIFICATION_BUF_COUNT].cmd_type,
                                                              data[read_index % NOTIFICATION_BUF_COUNT].desc_type,
                                                              data[read_index % NOTIFICATION_BUF_COUNT].desc_index,
                                                              data[read_index % NOTIFICATION_BUF_COUNT].notification_id
                                                             ); // Call callback function
                                        read_index++;
                                }
                        }

                        else
                        {
                                SetEvent(poll_events[KILL_EVENT]);
                                break;
                        }
                }

                return 0;
        }

        void notification::notifying(int32_t notifying, uint64_t guid, uint16_t cmd_type, uint16_t desc_type, uint16_t desc_index, void *notification_id)
        {
                if((write_index - read_index) > NOTIFICATION_BUF_COUNT)
                {
                        return;
                }

                if(notifying == avdecc_lib::NO_MATCH_FOUND || notifying == avdecc_lib::END_STATION_DISCOVERED ||
                   notifying == avdecc_lib::END_STATION_CONNECTED || notifying == avdecc_lib::END_STATION_RECONNECTED ||
                   notifying == avdecc_lib::END_STATION_DISCONNECTED || notifying == avdecc_lib::COMMAND_SENT ||
                   notifying == avdecc_lib::COMMAND_TIMEOUT || notifying == avdecc_lib::COMMAND_RESENT ||
                   notifying == avdecc_lib::RESPONSE_RECEIVED || notifying == avdecc_lib::COMMAND_SUCCESS)
                {
                        notification_buf[write_index % NOTIFICATION_BUF_COUNT].notifying = notifying;
                        notification_buf[write_index % NOTIFICATION_BUF_COUNT].guid = guid;
                        notification_buf[write_index % NOTIFICATION_BUF_COUNT].cmd_type = cmd_type;
                        notification_buf[write_index % NOTIFICATION_BUF_COUNT].desc_type = desc_type;
                        notification_buf[write_index % NOTIFICATION_BUF_COUNT].desc_index = desc_index;
                        notification_buf[write_index % NOTIFICATION_BUF_COUNT].notification_id = notification_id;

                        write_index++;
                        ReleaseSemaphore(poll_events[NOTIFICATION_EVENT], 1, NULL);
                }
        }

        void notification::set_notification_callback(void (*new_notification_callback) (void *, int32_t, uint64_t, uint16_t,
                                                                                        uint16_t, uint16_t, void *), void *p)
        {
                notification_callback = new_notification_callback;
                user_obj = p;
        }
}