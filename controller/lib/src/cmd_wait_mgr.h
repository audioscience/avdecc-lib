/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2014 AudioScience Inc.
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
 * cmd_wait_mgr.h
 *
 * A helper class for managing commands that are sent from an application that
 * wait for the commands to complete.
 *
 * There are 2 threads that operate on this class. The avdecc-lib "lib" thread
 * that is responsible for all 1722.1 operations happening in a single context and
 * the "app" thread that is responsible for issuing commands to the 1722.1 lib
 * thread and potentially waiting for the completion of those commands.
 * This class clarifies the "contract" between the two threads.
 *
 * An internal state is maintained that is written to by the app thread and read
 * by the lib thread.
 *
 * The only communication from the lib thread to the main thread occurs when
 * a semaphore is posted to release the waiting operation in the app thread.
 * Either a successful command completion or a timeout can cause the lib
 * thread to post to the waiting semaphore in the app thread.
 */

#pragma once

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
    class cmd_wait_mgr
    {
    public:
        cmd_wait_mgr();
        virtual ~cmd_wait_mgr();

        int set_primed_state(void * id);
        int set_active_state(void);
        int set_idle_state(void);

        bool match_id(void * id);
        bool active_state(void);
        bool primed_state(void);
        int set_completion_status(int status);
        int get_completion_status(void);
        void * get_notify_id(void);

    private:

        /**
         * Wait states.
         */
        enum wait_states
        {
            wait_idle,   /// idle state
            wait_primed, /// primed means a call to set_wait_for_next_cmd() has been made
            wait_active  /// active means a command matching the "primed" notify ID has been sent
        };

        void *notify_id;
        int completion_status;
        enum wait_states state;
    };
}

