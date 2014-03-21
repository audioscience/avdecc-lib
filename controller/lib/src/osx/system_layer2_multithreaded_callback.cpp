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
#include <sys/user.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/event.h>

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

        wait_mgr = new cmd_wait_mgr();

        sem_unlink("/waiting_sem");
        sem_unlink("/shutdown_sem");

        if (((waiting_sem = sem_open("/waiting_sem", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) ||
            ((shutdown_sem = sem_open("/shutdown_sem", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED))
        {
            perror("sem_open");
            exit(-1);
        }
    }

    system_layer2_multithreaded_callback::~system_layer2_multithreaded_callback()
    {
        sem_unlink("/waiting_sem");
        sem_unlink("/shutdown_sem");
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
         * Check for conditions that cause wait for completion.
         */
        if ( wait_mgr->primed_state() &&
             wait_mgr->match_id(notification_id) &&
             (notification_flag == CMD_WITH_NOTIFICATION))
        {
            int status = 0;

            status = wait_mgr->set_active_state();
            assert(status == 0);
            if (sem_wait(waiting_sem) != 0)
            {
                perror("sem_wait");
            }
            resp_status_for_cmd = wait_mgr->get_completion_status();
            status = wait_mgr->set_idle_state();
            assert(status == 0);
        }

        return 0;
    }

    int STDCALL system_layer2_multithreaded_callback::set_wait_for_next_cmd(void * id)
    {
        wait_mgr->set_primed_state(id);
        return 0;
    }

    int STDCALL system_layer2_multithreaded_callback::get_last_resp_status()
    {
        return resp_status_for_cmd;
    }

    int system_layer2_multithreaded_callback::fn_timer_cb(struct kevent *priv)
    {
        return instance->fn_timer(priv);
    }
    int system_layer2_multithreaded_callback::fn_netif_cb(struct kevent *priv)
    {
        return instance->fn_netif(priv);
    }
    int system_layer2_multithreaded_callback::fn_tx_cb(struct kevent *priv)
    {
        return instance->fn_tx(priv);
    }


    int system_layer2_multithreaded_callback::fn_timer(struct kevent *priv)
    {
        bool notification_id_incomplete = false;

        if (wait_mgr->active_state())
        {
            if (controller_ref_in_system->is_inflight_cmd_with_notification_id(wait_mgr->get_notify_id()) ||
                controller_ref_in_system->is_active_operation_with_notification_id(wait_mgr->get_notify_id()))
                notification_id_incomplete = true;
        }

        // If waiting on a command to complete and the command was "inflight" prior to calling the
        // timer tick update (ie notification_id_incomplete == true) AND after calling the timer tick
        // update the command is no longer "inflight", timeout processing has removed it, so signal the
        // waiting app thread.

        controller_ref_in_system->time_tick_event();

        bool is_timeout_for_waiting_notify_id = wait_mgr->active_state() && notification_id_incomplete &&
                                                !controller_ref_in_system->is_inflight_cmd_with_notification_id(wait_mgr->get_notify_id()) &&
                                                !controller_ref_in_system->is_active_operation_with_notification_id(wait_mgr->get_notify_id());
        if (is_timeout_for_waiting_notify_id)
        {
            int status = wait_mgr->set_completion_status(AVDECC_LIB_STATUS_TICK_TIMEOUT);
            assert(status == 0);
            sem_post(waiting_sem);
        }

        return 0;
    }

    int system_layer2_multithreaded_callback::fn_tx(struct kevent *priv)
    {
        struct tx_data t;
        int result = read(tx_pipe[PIPE_RD], &t, sizeof(t));

        if (result > 0)
        {
            controller_ref_in_system->tx_packet_event(
                t.notification_id,
                t.notification_flag,
                t.frame,
                t.mem_buf_len);

            delete[] t.frame;
        }

        return 0;
    }


    int system_layer2_multithreaded_callback::fn_netif(struct kevent *priv)
    {
        uint16_t length = 0;
        const uint8_t *rx_frame;
        int status = 0;

        status = netif_obj_in_system->capture_frame(&rx_frame, &length);

        if(status > 0)
        {

            bool is_notification_id_valid = false;
            int rx_status = -1;
            void *notification_id = NULL;
            uint16_t operation_id = 0;
            bool is_operation_id_valid = false;

            controller_ref_in_system->rx_packet_event(notification_id,
                                                      is_notification_id_valid,
                                                      rx_frame,
                                                      length,
                                                      rx_status,
                                                      operation_id,
                                                      is_operation_id_valid);

            if (
                wait_mgr->active_state() &&
                is_notification_id_valid &&
                wait_mgr->match_id(notification_id) &&
                !controller_ref_in_system->is_inflight_cmd_with_notification_id(wait_mgr->get_notify_id()) &&
                !controller_ref_in_system->is_active_operation_with_notification_id(wait_mgr->get_notify_id())
            )
            {
                int status = wait_mgr->set_completion_status(rx_status);
                assert(status == 0);
                sem_post(waiting_sem);
            }
        }
        return 0;
    }

    int system_layer2_multithreaded_callback::proc_poll_loop()
    {
        // POLL_COUNT
        struct kevent chlist[POLL_COUNT];   /* events we want to monitor */
        struct kevent evlist[POLL_COUNT];   /* events that were triggered */
        int nev, i, kq;

        kq = kqueue();
        if (kq == -1)
        {
            perror("kqueue");
        }

        EV_SET(&chlist[0], netif_obj_in_system->get_fd(), EVFILT_READ, EV_ADD | EV_ENABLE,
               0, 0, (void *)&system_layer2_multithreaded_callback::fn_netif_cb);

        EV_SET(&chlist[1], tx_pipe[PIPE_RD], EVFILT_READ, EV_ADD | EV_ENABLE,
               0, 0, (void *)&system_layer2_multithreaded_callback::fn_tx_cb);

        EV_SET(&chlist[2], 0, EVFILT_TIMER, EV_ADD | EV_ENABLE,
               0, TIME_PERIOD_25_MILLISECONDS, (void *)&system_layer2_multithreaded_callback::fn_timer_cb);


        do
        {
            nev = kevent(kq, chlist, POLL_COUNT, evlist, POLL_COUNT, NULL);

            if (local_system == NULL)
            {
                // System has been shut down
                sem_post(shutdown_sem);
                return 0;
            }

            if (nev == -1)
            {
                perror("kevent()");
                exit(EXIT_FAILURE);
            }
            else if (nev > 0)
            {
                for (i = 0; i < nev; i++)
                {
                    int (*fcn)(struct kevent *) = (int(*)(struct kevent *))evlist[i].udata;
                    int rv = fcn(&evlist[i]);
                    if (rv < 0)
                    {
                        return -1;
                    }
                }
            }

        }
        while (1);

        return 0;
    }

    void * system_layer2_multithreaded_callback::thread_fn(void *param)
    {
        int rc;

        rc = ((system_layer2_multithreaded_callback *)param)->proc_poll_loop();

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
