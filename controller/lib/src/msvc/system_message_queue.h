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

#include <stdint.h>

namespace avdecc_lib
{
    class system_message_queue
    {
    private:
        HANDLE space_avail;
        HANDLE data_avail;
        CRITICAL_SECTION critical_section_obj;
        uint8_t *buf;
        int in_pos;
        int out_pos;
        int entry_count;
        int entry_size;

    public:
        /**
         * Constructor for System Message Queue object with an entry count and size.
         */
        system_message_queue(int count, int size);

        ~system_message_queue();

        void queue_push(void *thread_data);

        void queue_pop_nowait(void *thread_data);

        void queue_pop_wait(void *thread_data);

        HANDLE queue_data_available_object();
    };
}

