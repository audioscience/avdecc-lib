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
 * notification_imp.cpp
 *
 * Notification implementation
 */

#include "enumeration.h"
#include "notification_imp.h"

namespace avdecc_lib
{
    notification_imp *notification_imp_ref = new notification_imp();

    notification_imp::notification_imp()
    {
        notification_thread_init(); // Start notification thread
    }

    notification_imp::~notification_imp() {}

    int notification_imp::notification_thread_init()
    {
        poll_events[NOTIFICATION_EVENT] = CreateSemaphore(NULL, 0, 32767, NULL);
        poll_events[KILL_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);

        h_thread = CreateThread(NULL, // Default security descriptor
                                0, // Default stack size
                                proc_notification_thread, // Point to the start address of the thread
                                this, // Data to be passed to the thread
                                0, // Flag controlling the creation of the thread
                                &thread_id // Thread identifier
                               );

        if (h_thread == NULL)
        {
            exit(EXIT_FAILURE);
        }

        return 0;
    }

    DWORD WINAPI notification_imp::proc_notification_thread(LPVOID lpParam)
    {
        return reinterpret_cast<notification_imp *>(lpParam)->proc_notification_thread_callback();
    }

    int notification_imp::proc_notification_thread_callback()
    {
        DWORD dwEvent;

        while (true)
        {
            dwEvent = WaitForMultipleObjects(2, poll_events, FALSE, INFINITE);

            if(dwEvent == (WAIT_OBJECT_0 + NOTIFICATION_EVENT))
            {
                if((write_index - read_index) > 0)
                {
                    notification_callback(user_obj,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].notification_type,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].entity_id,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].cmd_type,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].desc_type,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].desc_index,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].cmd_status,
                                          notification_buf[read_index % NOTIFICATION_BUF_COUNT].notification_id
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

    void notification_imp::post_notification_event()
    {
        ReleaseSemaphore(poll_events[NOTIFICATION_EVENT], 1, NULL);
    }
}
