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
 * log.h
 *
 * Log base class, which is called by AVDECC LIB modules for logging purposes.
 */

#pragma once

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
    class log
    {
    protected:
        int32_t log_level; // The base log level for messages to be logged
        uint32_t read_index;
        uint32_t write_index;
        void (*callback_func)(void *, int32_t, const char *, int32_t);
        void *user_obj;
        uint32_t missed_log_event_cnt; // The number of missed log that exceeds the log buffer count.

        enum
        {
            LOG_BUF_COUNT = 32
        };

        struct log_data
        {
            int32_t level;
            char msg[256];
            int32_t time_stamp_ms;
        };

        struct log_data log_buf[LOG_BUF_COUNT];

    public:
        log();

        ~log();

        /**
         * Update the base log level for messages to be logged by the logging callback.
         */
        void set_log_level(int32_t new_log_level);

        /**
         * AVDECC LIB modules call this function for logging purposes.
         */
        void post_log_msg(int32_t log_level, const char *fmt,...);

        /**
         * Release sempahore so that log callback function is called.
         */
        virtual void post_log_event() = 0;

        /**
         * Change the logging callback function to a new logging callback function.
         */
        void set_log_callback(void (*new_log_callback) (void *, int32_t, const char *, int32_t), void *);

        /**
         * Get the number of missed logs that exceeds the log buffer count.
         */
        virtual uint32_t missed_log_event_count();
    };
}

