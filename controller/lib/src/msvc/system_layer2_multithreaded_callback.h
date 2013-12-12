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
 * system_layer2_multithreaded_callback.h
 *
 * Multithreaded System implementation class
 */

#pragma once

#include "system.h"
#include "timer.h"

namespace avdecc_lib
{
    class system_layer2_multithreaded_callback : public virtual system
    {
    private:
        struct poll_thread_data
        {
            uint8_t *frame;
            size_t frame_len;
            void *notification_id;
            uint32_t notification_flag;
        };

        struct thread_creation
        {
            LPTHREAD_START_ROUTINE thread;
            HANDLE handle;
            DWORD id;
            HANDLE kill_sem;
        };

        struct msg_poll
        {
            struct thread_creation queue_thread;
            system_message_queue *rx_queue;
            system_message_queue *tx_queue;
            HANDLE timeout_event;
        };

        enum wpcap_events
        {
            WPCAP_TIMEOUT,
            WPCAP_RX_PACKET,
            WPCAP_TX_PACKET,
            KILL_ALL,
            NUM_OF_EVENTS
        };

        struct msg_poll poll_rx;
        struct msg_poll poll_tx;
        struct thread_creation poll_thread;
        HANDLE poll_events_array[NUM_OF_EVENTS];
        HANDLE waiting_sem;

        bool is_waiting;
        bool queue_is_waiting;
        void *waiting_notification_id;
        int resp_status_for_cmd;
        timer tick_timer; // A tick timer that is always running

    public:
        /**
         * A constructor for system_layer2_multithreaded_callback used for constructing an object with network interface, notification, and post_log_msg callback functions.
         */
        system_layer2_multithreaded_callback(net_interface *netif, controller *controller_obj);

        virtual ~system_layer2_multithreaded_callback();

        /**
         * Call destructor for Controller used for destroying objects
         */
        void STDCALL destroy();

        /**
         * Store the frame to be sent in a queue.
         */
        int queue_tx_frame(void *notification_id, uint32_t notification_flag, uint8_t *frame, size_t frame_len);

        /**
         * Set a waiting flag for the command sent.
         */
        int STDCALL set_wait_for_next_cmd(void *notification_id);

        /**
         * Wait for the response packet with the corrsponding notification id to be received.
         */
        int STDCALL get_last_resp_status();

        /**
         * Start point of the system process, which calls the thread initialization function.
         */
        int STDCALL process_start();

        /**
         * End point of the system process, which terminates the threads.
         */
        int STDCALL process_close();

    private:
        /**
         * Create and initialize threads, events, and semaphores for wpcap thread.
         */
        int init_wpcap_thread();

        /**
         * Create and initialize threads, events, and semaphores for poll thread.
         */
        int init_poll_thread();

        /**
         * Start of the packet capture thread used for capturing packets.
         */
        static DWORD WINAPI proc_wpcap_thread(LPVOID lpParam);

        /**
         * A member function called to start the wpcap thread processing.
         */
        int proc_wpcap_thread_callback();

        /**
         * Start of the polling thread used for polling events.
         */
        static DWORD WINAPI proc_poll_thread(LPVOID lpParam);

        /**
         * A member function called to start the poll thread processing.
         */
        int proc_poll_thread_callback();

        /**
         * Execute poll events.
         */
        int poll_single();
    };
}
