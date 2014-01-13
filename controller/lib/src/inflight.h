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
 * inflight.h
 *
 * A class for storing AVDECC inflight packets.
 */

#pragma once

#include "timer.h"

namespace avdecc_lib
{
    class inflight
    {
    private:
        struct jdksavdecc_frame cmd_frame;
        uint32_t cmd_notification_flag;
        timer cmd_timer;
        uint32_t cmd_timeout_ms;
        uint32_t start_timer_cnt;

    public:
        /* following 2 are public for compare prediate classes */
        uint16_t cmd_seq_id;
        void *cmd_notification_id;

        inflight(struct jdksavdecc_frame *frame,
                 uint16_t seq_id,
                 void *notification_id,
                 uint32_t notification_flag,
                 uint32_t timeout_ms)
                :  cmd_notification_flag(notification_flag), cmd_timeout_ms(timeout_ms), cmd_seq_id(seq_id), cmd_notification_id(notification_id)
        {
            cmd_frame = *frame;
            start_timer_cnt = 0;
        }

        ~inflight() {}

        inline void start_timer()
        {
            start_timer_cnt++;
            cmd_timer.start(cmd_timeout_ms);
        }

        inline struct jdksavdecc_frame frame()
        {
            return cmd_frame;
        }

        inline uint32_t notification_flag()
        {
            return cmd_notification_flag;
        }

        inline bool timeout()
        {
            return cmd_timer.timeout();
        }

        inline bool retried()
        {
            return start_timer_cnt >= 2; // The command can be resent once
        }
    };

    /*
     * Class for use in STL find_if() call to find matching sequence ID.
     */
    class SeqIdComp
    {
    private:
        uint16_t v;

    public:
        SeqIdComp(uint16_t i) : v(i) { }

        inline bool operator()(const inflight & m) const
        {
            return m.cmd_seq_id == v;
        }
    };

    /*
     * Class for use in STL find_if() call to find matching notification ID.
     */
    class NotificationComp
    {
    private:
        void * v;

    public:
        NotificationComp(void * p) : v(p) { }

        inline bool operator()(const inflight & m) const
        {
            return m.cmd_notification_id == v;
        }
    };
}

