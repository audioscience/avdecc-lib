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
#include <errno.h>

namespace avdecc_lib
{
log_imp * log_imp_ref = new log_imp();

log_imp::log_imp()
{
    logging_thread_init(); // Start log thread
}

log_imp::~log_imp()
{
    // posting to sem without data causes the thread to terminate
    post_log_event();
    sem_unlink("/log_waiting_sem");
}

int log_imp::logging_thread_init()
{
    int rc;

    sem_unlink("/log_waiting_sem");

    if ((log_waiting = sem_open("/log_waiting_sem", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(-1);
    }

    rc = pthread_create(&h_thread, NULL, &log_imp::dispatch_thread, (void *)this);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    return 0;
}

void * log_imp::dispatch_thread(void * param)
{
    return ((log_imp *)param)->dispatch_callbacks();
}

void * log_imp::dispatch_callbacks(void)
{
    int status;

    while (true)
    {
        status = sem_wait(log_waiting);

        if (status < 0)
        {
            perror("sem_wait");
        }

        if ((write_index - read_index) > 0)
        {
            callback_func(user_obj,
                          log_buf[read_index % LOG_BUF_COUNT].level,
                          log_buf[read_index % LOG_BUF_COUNT].msg,
                          log_buf[read_index % LOG_BUF_COUNT].time_stamp_ms);
            read_index++;
        }
        else
        {
            break;
        }
    }

    return 0;
}

void log_imp::post_log_event()
{
    sem_post(log_waiting);
}
}
