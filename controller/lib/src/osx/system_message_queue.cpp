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

#include <stdio.h>
#include <string.h>

#include "system_message_queue.h"

system_message_queue::system_message_queue() {}

system_message_queue::system_message_queue(int count, int size)
{
    entry_count = count;
    entry_size = size;
    in_pos = 0;
    out_pos = 0;
    buf = (uint8_t *)calloc(entry_count, entry_size);

    sem_unlink("/space_avail_sem");
    sem_unlink("/data_avail_sem");

    if ((space_avail = sem_open("/space_avail_sem", O_CREAT | O_EXCL, 0644, entry_count)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(-1);
    }

    if ((data_avail = sem_open("/data_avail_sem", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(-1);
    }

    //create mutex attribute variable
    pthread_mutexattr_t mAttr;

    // setup recursive mutex for mutex attribute
    pthread_mutexattr_settype(&mAttr, OS_PTHREAD_MUTEX_RECURSIVE_TYPE);

    // Use the mutex attribute to create the mutex
    pthread_mutex_init(&critical_section_obj, &mAttr);

    // Mutex attribute can be destroy after initializing the mutex variable
    pthread_mutexattr_destroy(&mAttr);
}

system_message_queue::~system_message_queue()
{
    if(buf)
    {
        free(buf);
    }
    sem_unlink("/space_avail_sem");
    sem_unlink("/data_avail_sem");
}

void system_message_queue::queue_push(void *thread_data)
{
    sem_wait(space_avail);
    pthread_mutex_lock(&critical_section_obj);
    memcpy(&buf[in_pos * entry_size], thread_data, entry_size);
    in_pos = (in_pos + 1) % entry_count;
    pthread_mutex_unlock (&critical_section_obj);
    sem_post(data_avail);
}

void system_message_queue::queue_pop_nowait(void *thread_data)
{
    pthread_mutex_lock(&critical_section_obj);
    memcpy(thread_data, &buf[out_pos * entry_size], entry_size);
    out_pos = (out_pos + 1) % entry_count;
    pthread_mutex_unlock(&critical_section_obj);
    sem_post(space_avail);
}

void system_message_queue::queue_pop_wait(void *thread_data)
{
    sem_wait(data_avail);
    queue_pop_nowait(thread_data);
}

avdecc_lib_os::aSemaphore system_message_queue::queue_data_available_object()
{
    return data_avail;
}
