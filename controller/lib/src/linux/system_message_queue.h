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
 * system_message_queue.h
 *
 * System message queue class, which is called by System modules to queue packets.
 */

#pragma once

#include <cstdint>

#include "avdecc_lib_os.h"

class system_message_queue
{
private:
    avdecc_lib_os::aSemaphore space_avail;
    avdecc_lib_os::aSemaphore data_avail;
    avdecc_lib_os::aCriticalSection critical_section_obj;
    uint8_t *buf;
    int in_pos;
    int out_pos;
    int entry_count;
    int entry_size;

public:
    /**
     * An empty constructor for system_message_queue
     */
    system_message_queue();

    /**
     * Constructor for system_message_queue used for constructing an object with count and size.
     */
    system_message_queue(int count, int size);

    /**
     * Destructor for system_message_queue used for destroying objects
     */
    ~system_message_queue();

    void queue_push(void *thread_data);

    void queue_pop_nowait(void *thread_data);

    void queue_pop_wait(void *thread_data);

    avdecc_lib_os::aSemaphore queue_data_available_object();
};
