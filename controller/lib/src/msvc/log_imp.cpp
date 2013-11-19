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
 * log_imp.cpp
 *
 * Log implementation
 */

#include <iostream>
#include "enumeration.h"
#include "log_imp.h"

namespace avdecc_lib
{
        log_imp *log_imp_ref = new log_imp();

        log_imp::log_imp()
        {
                logging_thread_init(); // Start log thread
        }

        log_imp::~log_imp() {}

        int log_imp::logging_thread_init()
        {
                poll_events[LOG_EVENT] = CreateSemaphore(NULL, 0, 32767, NULL);
                poll_events[KILL_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);

                h_thread = CreateThread(NULL, // Default security descriptor
                                        0, // Default stack size
                                        proc_logging_thread, // Point to the start address of the thread
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

        DWORD WINAPI log_imp::proc_logging_thread(LPVOID lpParam)
        {
                return reinterpret_cast<log_imp *>(lpParam)->proc_logging_thread_callback();
        }

        int log_imp::proc_logging_thread_callback()
        {
                DWORD dwEvent;

                while (true)
                {
                        dwEvent = WaitForMultipleObjects(2, poll_events, FALSE, INFINITE);

                        if (dwEvent == (WAIT_OBJECT_0 + LOG_EVENT))
                        {
                                if((write_index - read_index) > 0)
                                {
                                        callback_func(user_obj,
                                                      log_buf[read_index % LOG_BUF_COUNT].level,
                                                      log_buf[read_index % LOG_BUF_COUNT].msg,
                                                      log_buf[read_index % LOG_BUF_COUNT].time_stamp_ms
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

        void log_imp::post_log_event()
        {
                ReleaseSemaphore(poll_events[LOG_EVENT], 1, NULL);
        }
}
