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
 * notification_imp.h
 *
 * Notification implementation class
 */

#pragma once

#include "avdecc_lib_os.h"
#include "notification.h"


namespace avdecc_lib
{
    class notification_imp : public virtual notification
    {
    private:
        pthread_t h_thread;
        sem_t notify_waiting;

    public:
        /**
         * An empty constructor for notification_imp
         */
        notification_imp();

        /**
         * Destructor for notification_imp used for destroying objects
         */
        virtual ~notification_imp();

    private:
        /**
         * Create and initialize notification thread, event, and semaphore.
         */
        int notification_thread_init();

        /**
         * Start of the post_notification_msg thread used for generating notification messages.
         */
        static void * dispatch_thread(void * lpParam);

        void * dispatch_callbacks(void);

    public:
        /**
         * Release sempahore so that notification callback function is called.
         */
        void post_notification_event();
    };

    extern notification_imp *notification_imp_ref;
}
