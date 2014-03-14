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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#include "enumeration.h"
#include "notification_imp.h"

namespace avdecc_lib
{
    notification_imp *notification_imp_ref = new notification_imp();

    notification_imp::notification_imp()
    {
        notification_thread_init(); // Start notification thread
    }

    notification_imp::~notification_imp()
    {
        post_notification_event();
        sem_unlink("/notify_waiting_sem");
    }

    int notification_imp::notification_thread_init()
    {
        int rc;

        sem_unlink("/notify_waiting_sem");

        if ((notify_waiting = sem_open("/notify_waiting_sem", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(-1);
        }

        rc = pthread_create(&h_thread, NULL, &notification_imp::dispatch_thread, (void *)this);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        return 0;
    }

    void * notification_imp::dispatch_thread(void * param)
    {
        return ((notification_imp *)param)->dispatch_callbacks();

    }


    void * notification_imp::dispatch_callbacks(void)
    {
        int status;

        while (true)
        {
            status = sem_wait(notify_waiting);

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
                                     );
                read_index++;
            }
            else
            {
                break;
            }
        }

        return 0;
    }

    void notification_imp::post_notification_event()
    {
        sem_post(notify_waiting);
    }
}
