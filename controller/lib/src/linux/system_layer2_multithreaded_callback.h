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

#include <sys/epoll.h>

#include "avdecc_lib_os.h"
#include "system.h"


namespace avdecc_lib
{
    struct epoll_priv;


    class system_layer2_multithreaded_callback : public virtual system
    {
    public:
        /**
         * A constructor for system_layer2_multithreaded_callback used for constructing an object with network interface, notification, and logging callback functions.
         */
        system_layer2_multithreaded_callback(net_interface *netif, controller *controller_obj);

        /**
         * Destructor for system_layer2_multithreaded_callback used for destroying objects
         */
        virtual ~system_layer2_multithreaded_callback();

        /**
         * Deallocate memory
         */
        void STDCALL destroy();

        /**
         * Store the frame to be sent in a queue.
         */
        int queue_tx_frame(void *notification_id, uint32_t notification_flag, uint8_t *frame, size_t mem_buf_len);

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
        static system_layer2_multithreaded_callback *instance;
        struct epoll_priv;
        typedef int (* handler_fn) (struct epoll_priv * priv);

        struct epoll_priv
        {
            int fd;
            handler_fn fn;
        };

        struct tx_data
        {
            uint8_t *frame;
            size_t mem_buf_len;
            void *notification_id;
            uint32_t notification_flag;
        };

        enum useful_enums
        {
            PIPE_RD = 0,
            PIPE_WR = 1,
            POLL_COUNT = 3,
            TIME_PERIOD_25_MILLISECONDS = 25
        };

        pthread_t h_thread;

        //int network_fd;
        int tx_pipe[2];
        //int tick_timer;

        sem_t *waiting_sem;

        /*
        Events to process:
        Rx packet - from socket
        Tx packet - from FIFO
        Timer tick - from timer
        */

        bool is_waiting;
        bool queue_is_waiting;
        void *waiting_notification_id;
        int resp_status_for_cmd;
        int prep_evt_desc(int fd, handler_fn fn, struct epoll_priv *priv, struct epoll_event *ev);
        static int fn_timer_cb(struct epoll_priv *priv);
        static int fn_netif_cb(struct epoll_priv *priv);
        static int fn_tx_cb(struct epoll_priv *priv);
        int fn_timer(struct epoll_priv *priv);
        int fn_netif(struct epoll_priv *priv);
        int fn_tx(struct epoll_priv *priv);
        int timer_start_interval(int timerfd);

        void * proc_poll_thread(void * p);
        int proc_poll_loop();
        static void * thread_fn(void *param);

        int poll_single(void);

    };
}
