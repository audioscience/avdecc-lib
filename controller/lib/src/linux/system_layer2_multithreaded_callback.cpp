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
 * system_layer2_multithreaded_callback.cpp
 *
 * Multithreaded System implementation
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/timerfd.h>
#include <sys/user.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/un.h>
#include <sys/eventfd.h>

#include <vector>

#include "net_interface_imp.h"
#include "enumeration.h"
#include "notification_imp.h"
#include "log_imp.h"
#include "end_station_imp.h"
#include "controller_imp.h"
#include "system_message_queue.h"
#include "system_tx_queue.h"
#include "system_layer2_multithreaded_callback.h"


namespace avdecc_lib
{

    net_interface_imp *netif_obj_in_system;
    controller_imp *controller_ref_in_system;
    system_layer2_multithreaded_callback *local_system = NULL;

    system_layer2_multithreaded_callback *system_layer2_multithreaded_callback::instance = NULL;


    size_t system_queue_tx(void *notification_id, uint32_t notification_flag, uint8_t *frame, size_t mem_buf_len)
    {
        if(local_system)
        {
            return local_system->queue_tx_frame(notification_id, notification_flag, frame, mem_buf_len);
        }
        else
        {
            return 0;
        }
    }

    system * STDCALL create_system(system::system_type type, net_interface *netif, controller *controller_obj)
    {
        local_system = new system_layer2_multithreaded_callback(netif, controller_obj);

        return local_system;
    }

    system_layer2_multithreaded_callback::system_layer2_multithreaded_callback(net_interface *netif, controller *controller_obj)
    {
        instance = this;
        netif_obj_in_system = dynamic_cast<net_interface_imp *>(netif);
        controller_ref_in_system = dynamic_cast<controller_imp *>(controller_obj);
        pipe(tx_pipe);

        queue_is_waiting = false;

        waiting_sem = (sem_t *)calloc(1, sizeof(*waiting_sem));
        if (waiting_sem)
            sem_init(waiting_sem, 0, 0);

        shutdown_sem = (sem_t *)calloc(1, sizeof(*shutdown_sem));
        if (shutdown_sem)
            sem_init(shutdown_sem, 0, 0);
    }

    system_layer2_multithreaded_callback::~system_layer2_multithreaded_callback()
    {
        free(waiting_sem);
        free(shutdown_sem);
    }

    void STDCALL system_layer2_multithreaded_callback::destroy()
    {
        if (this == local_system)
        {
            local_system = NULL;

            // Wait for controller to have finished
            if (sem_wait(shutdown_sem) != 0)
            {
                perror("sem_wait");
            }
        }

        delete this;
    }

    int system_layer2_multithreaded_callback::queue_tx_frame(
        void *notification_id,
        uint32_t notification_flag,
        uint8_t *frame,
        size_t mem_buf_len)
    {
        struct tx_data t;

        t.frame = new uint8_t[2048];
        if (!t.frame)
        {
            perror("malloc");
            exit(EXIT_FAILURE);

        }
        t.mem_buf_len = mem_buf_len;
        memcpy(t.frame, frame, mem_buf_len);
        t.notification_id = notification_id;
        t.notification_flag = notification_flag;
        write(tx_pipe[PIPE_WR], &t, sizeof(t));

        /**
         * If queue_is_waiting is true, wait for the response before returning.
         */
        if(queue_is_waiting && (notification_flag == CMD_WITH_NOTIFICATION))
        {
            is_waiting = true;
            sem_wait(waiting_sem);
            queue_is_waiting = false;
        }

        return 0;
    }

    int STDCALL system_layer2_multithreaded_callback::set_wait_for_next_cmd()
    {
        queue_is_waiting = true;
        resp_status_for_cmd = AVDECC_LIB_STATUS_INVALID; // Reset the status

        return 0;
    }

    int STDCALL system_layer2_multithreaded_callback::get_last_resp_status()
    {
        return resp_status_for_cmd;
    }


    int system_layer2_multithreaded_callback::timer_start_interval(int timerfd)
    {
        struct itimerspec itimer_new;
        struct itimerspec itimer_old;
        unsigned long ns_per_ms = 1000000;
        unsigned long interval_ms = TIME_PERIOD_25_MILLISECONDS;

        memset(&itimer_new, 0, sizeof(itimer_new));
        memset(&itimer_old, 0, sizeof(itimer_old));

        itimer_new.it_interval.tv_sec = interval_ms / 1000;
        itimer_new.it_interval.tv_nsec = (interval_ms % 1000) * ns_per_ms;
        itimer_new.it_value = itimer_new.it_interval;

        return timerfd_settime(timerfd, 0, &itimer_new, &itimer_old);
    }

    int system_layer2_multithreaded_callback::fn_timer_cb(struct epoll_priv *priv)
    {
        return instance->fn_timer(priv);
    }
    int system_layer2_multithreaded_callback::fn_netif_cb(struct epoll_priv *priv)
    {
        return instance->fn_netif(priv);
    }
    int system_layer2_multithreaded_callback::fn_tx_cb(struct epoll_priv *priv)
    {
        return instance->fn_tx(priv);
    }


    int system_layer2_multithreaded_callback::fn_timer(struct epoll_priv *priv)
    {
        uint64_t timer_exp_count;
        read(priv->fd, &timer_exp_count, sizeof(timer_exp_count));

        controller_ref_in_system->time_tick_event();

        bool is_waiting_completed = is_waiting &&
                                    (!controller_ref_in_system->is_inflight_cmd_with_notification_id(waiting_notification_id));
        if(is_waiting_completed)
        {
            is_waiting = false;
            resp_status_for_cmd = AVDECC_LIB_STATUS_TICK_TIMEOUT;
            sem_post(waiting_sem);

        }
        return 0;
    }

    int system_layer2_multithreaded_callback::fn_tx(struct epoll_priv *priv)
    {
        struct tx_data t;
        int result = read(tx_pipe[PIPE_RD], &t, sizeof(t));

        if (result > 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_DEBUG, "fn_tx");
            controller_ref_in_system->tx_packet_event(
                t.notification_id,
                t.notification_flag,
                t.frame,
                t.mem_buf_len);

            if(t.notification_flag == CMD_WITH_NOTIFICATION)
            {
                waiting_notification_id = t.notification_id;
            }

            delete[] t.frame;
        }

        return 0;
    }


    int system_layer2_multithreaded_callback::fn_netif(struct epoll_priv *priv)
    {
        uint16_t length = 0;
        const uint8_t *rx_frame;
        int status = 0;

        status = netif_obj_in_system->capture_frame(&rx_frame, &length);

        if(status > 0)
        {

            bool is_notification_id_valid = false;
            int status = -1;
            bool is_waiting_completed = false;
            void *notification_id = NULL;

            controller_ref_in_system->rx_packet_event(notification_id,
                                                      is_notification_id_valid,
                                                      rx_frame,
                                                      length,
                                                      status);

            is_waiting_completed =
                is_waiting &&
                !controller_ref_in_system->is_inflight_cmd_with_notification_id(waiting_notification_id) &&
                is_notification_id_valid &&
                (waiting_notification_id == notification_id);

            if(is_waiting_completed)
            {
                resp_status_for_cmd = status;
                is_waiting = false;
                sem_post(waiting_sem);
            }
        }
        return 0;
    }


    int system_layer2_multithreaded_callback::prep_evt_desc(
        int fd,
        handler_fn fn,
        struct epoll_priv *priv,
        struct epoll_event *ev)
    {
        priv->fd = fd;
        priv->fn = fn;
        ev->events = EPOLLIN;
        ev->data.ptr = priv;
        return 0;
    }

    int system_layer2_multithreaded_callback::proc_poll_loop()
    {

        int epollfd;
        struct epoll_event ev, epoll_evt[POLL_COUNT];
        struct epoll_priv fd_fns[POLL_COUNT];

        epollfd = epoll_create(POLL_COUNT);

        prep_evt_desc(timerfd_create(CLOCK_MONOTONIC, 0), &system_layer2_multithreaded_callback::fn_timer_cb, &fd_fns[0],  &ev);
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_fns[0].fd, &ev);

        prep_evt_desc(netif_obj_in_system->get_fd(), &system_layer2_multithreaded_callback::fn_netif_cb, &fd_fns[1], &ev);
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_fns[1].fd, &ev);

        prep_evt_desc(tx_pipe[PIPE_RD], &system_layer2_multithreaded_callback::fn_tx_cb, &fd_fns[2],
                      &ev);
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_fns[2].fd, &ev);

        fcntl(fd_fns[0].fd, F_SETFL, O_NONBLOCK);
        timer_start_interval(fd_fns[0].fd);

        do
        {
            int i, res;
            struct epoll_priv *priv;
            res = epoll_wait(epollfd, epoll_evt, POLL_COUNT, -1);

            if (local_system == NULL)
            {
                // System has been shut down
                sem_post(shutdown_sem);
                return 0;
            }

            /* exit on error */
            if (-1 == res)
                return -errno;

            for (i = 0; i < res; i++)
            {
                priv = (struct epoll_priv *)epoll_evt[i].data.ptr;
                if (priv->fn(priv) < 0)
                    return -1;
            }
        }
        while (1);
        return 0;
    }

    void * system_layer2_multithreaded_callback::thread_fn(void *param)
    {
        ((system_layer2_multithreaded_callback *)param)->proc_poll_loop();

        return 0;
    }

    int STDCALL system_layer2_multithreaded_callback::process_start()
    {
        int rc;

        rc = pthread_create(&h_thread, NULL, &system_layer2_multithreaded_callback::thread_fn, (void *)this);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        return 0;
    }

    int STDCALL system_layer2_multithreaded_callback::process_close()
    {

        return 0;
    }
}
