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
#ifndef _AVDECC_CONTROLLER_LIB_INFLIGHT_H_
#define _AVDECC_CONTROLLER_LIB_INFLIGHT_H_

#include "jdksavdecc_util.h"
#include "jdksavdecc_pdu.h"

#include "timer.h"

namespace avdecc_lib
{
        class inflight
        {
        private:
                unsigned int start_timer_count;
                struct jdksavdecc_frame inflight_cmd_frame;
                bool notification_flag;
                unsigned int inflight_timeout;
                timer flight_timer;

        public:
                /* following 2 are public for compare prediate classes */
                uint16_t seq_id;
                void *notification_id;

                inflight(
                        struct jdksavdecc_frame *frame,
                        uint16_t sequence,
                        unsigned int timeout_ms,
                        void *notify_id,
                        bool notify_flag)
                        : seq_id(sequence), notification_id(notify_id), notification_flag(notify_flag),
                          inflight_timeout(timeout_ms), start_timer_count(0)
                {
                        inflight_cmd_frame = *frame;
                };

                ~inflight() {};

                void start_timer()
                {
                        flight_timer.start(inflight_timeout);
                };
                void *notify_id()
                {
                        return notification_id;
                };
                bool timeout()
                {
                        return flight_timer.timeout();
                };
                bool notify_flag()
                {
                        return notification_flag;
                }
                bool retried()
                {
                        return start_timer_count >= 2;
                };
        };

        /*
         * Class for use in STL find_if() call to find matching sequence ID.
         */
        class SeqIdComp
        {
        public:
                SeqIdComp(uint16_t i) : v(i) { }
                inline bool operator()(const inflight & m) const
                {
                        return m.seq_id == v;
                }
        private:
                uint16_t v;
        };

        /*
         * Class for use in STL find_if() call to find matching notification ID.
         */
        class NotificationComp
        {
        public:
                NotificationComp(void * p) : v(p) { }
                inline bool operator()(const inflight & m) const
                {
                        return m.notification_id == v;
                }
        private:
                void * v;
        };
}

#endif
