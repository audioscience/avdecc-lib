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
 * cmd_wait_mgr.cpp
 *
 * Notification base class implementation, which is called by AVDECC LIB modules to generate notification messages.
 */

#include "cmd_wait_mgr.h"

namespace avdecc_lib
{

    cmd_wait_mgr::cmd_wait_mgr()
    {
        state = wait_idle;
    }

    cmd_wait_mgr::~cmd_wait_mgr() {}

    int cmd_wait_mgr::set_primed_state(void * id)
    {
        if (state != wait_idle)
            return -1;
        else
        {
            notify_id = id;
            state = wait_primed;
            return 0;
        }
    }
    int cmd_wait_mgr::set_active_state(void)
    {
        if (state != wait_primed)
            return -1;
        else
        {
            state = wait_active;
            return 0;
        }
    }
    int cmd_wait_mgr::set_idle_state(void)
    {
        if (state != wait_active)
            return -1;
        else
        {
            state = wait_idle;
            return 0;
        }
    }

    bool cmd_wait_mgr::match_id(void * id)
    {
        return id == notify_id;
    }

    void * cmd_wait_mgr::get_notify_id(void)
    {
        return notify_id;
    }

    bool cmd_wait_mgr::active_state(void)
    {
        return state == wait_active;
    }

    bool cmd_wait_mgr::primed_state(void)
    {
        return state == wait_primed;
    }

    int cmd_wait_mgr::set_completion_status(int status)
    {
        if (state != wait_active)
            return -1;
        else
        {
            completion_status = status;
            return 0;
        }
    }
    int cmd_wait_mgr::get_completion_status(void)
    {
        return completion_status;
    }
}
