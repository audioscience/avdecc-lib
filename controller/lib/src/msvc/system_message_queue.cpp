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
 * system_message_queue.cpp
 *
 * System message queue implementation, which is called by System modules to queue packets.
 */

#include <Windows.h>
#include "system_message_queue.h"

namespace avdecc_lib
{
    system_message_queue::system_message_queue(int count, int size)
    {
        entry_count = count;
        entry_size = size;
        in_pos = 0;
        out_pos = 0;
        buf = (uint8_t *)calloc(entry_count, entry_size);
        space_avail = CreateSemaphore(NULL, entry_count, entry_count, NULL);
        data_avail = CreateSemaphore(NULL, 0, entry_count, NULL);
        InitializeCriticalSection(&critical_section_obj);
    }

    system_message_queue::~system_message_queue()
    {
        if(buf)
        {
            free(buf);
        }
        CloseHandle(space_avail);
        CloseHandle(data_avail);
        DeleteCriticalSection(&critical_section_obj);
    }

    void system_message_queue::queue_push(void *thread_data)
    {
        WaitForSingleObject(space_avail, INFINITE);
        EnterCriticalSection(&critical_section_obj);
        memcpy(&buf[in_pos * entry_size], thread_data, entry_size);
        in_pos = (in_pos + 1) % entry_count;
        LeaveCriticalSection(&critical_section_obj);
        ReleaseSemaphore(data_avail, 1, NULL);
    }

    void system_message_queue::queue_pop_nowait(void *thread_data)
    {
        EnterCriticalSection(&critical_section_obj);
        memcpy(thread_data, &buf[out_pos * entry_size], entry_size);
        out_pos = (out_pos + 1) % entry_count;
        LeaveCriticalSection(&critical_section_obj);
        ReleaseSemaphore(space_avail, 1, NULL);
    }

    void system_message_queue::queue_pop_wait(void *thread_data)
    {
        WaitForSingleObject(data_avail, INFINITE);
        queue_pop_nowait(thread_data);
    }

    HANDLE system_message_queue::queue_data_available_object()
    {
        return data_avail;
    }
}
