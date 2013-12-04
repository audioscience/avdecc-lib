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
 * log.cpp
 *
 * Log base class implementation, which is called by AVDECC LIB modules for post_log_msg purposes.
 */

#include "avdecc_lib_os.h"
#include <iostream>
#include "enumeration.h"
#include "log.h"

namespace avdecc_lib
{
    extern "C" void default_log(void *log_user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms) {}

    log::log()
    {
        log_level = LOGGING_LEVEL_ERROR;
        read_index = 0;
        write_index = 0;
        callback_func = default_log;
        user_obj = NULL;
        missed_log_event_cnt = 0;
    }

    log::~log() {}

    void log::set_log_level(int32_t new_log_level)
    {
        log_level = new_log_level;
    }

    void log::post_log_msg(int32_t level, const char *fmt,...)
    {
        if (level <= log_level)
        {
            va_list arglist;
            uint32_t index;

            if ((write_index - read_index) > LOG_BUF_COUNT)
            {
                missed_log_event_cnt++;
                return;
            }
            index = InterlockedExchangeAdd(&write_index, 1);
            va_start(arglist, fmt);
            vsprintf_s(log_buf[index % LOG_BUF_COUNT].msg, sizeof(log_buf[0].msg), fmt, arglist);  // Write to log_buf using write_index
            va_end(arglist);
            log_buf[index % LOG_BUF_COUNT].level = level;
            log_buf[index % LOG_BUF_COUNT].time_stamp_ms = 0;

            post_log_event();
        }
    }

    void log::set_log_callback(void (*new_log_callback) (void *, int32_t, const char *, int32_t), void *p)
    {
        callback_func = new_log_callback;
        user_obj = p;
    }

    uint32_t log::missed_log_event_count()
    {
        return missed_log_event_cnt;
    }
}
