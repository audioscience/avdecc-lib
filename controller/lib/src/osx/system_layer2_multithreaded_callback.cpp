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

		queue_is_waiting = false;

		if (waiting_sem)
			waiting_sem = sem_open("/waiting_sem", O_CREAT, 0644, 0);
	}

	system_layer2_multithreaded_callback::~system_layer2_multithreaded_callback()
	{
		free(waiting_sem);
		delete netif_obj_in_system;
		delete controller_ref_in_system;
		delete local_system;
	}

	void STDCALL system_layer2_multithreaded_callback::destroy()
	{
		delete this;
	}

	int system_layer2_multithreaded_callback::queue_tx_frame(
			void *notification_id,
			uint32_t notification_flag,
			uint8_t *frame,
			size_t mem_buf_len)
	{
		struct tx_data *t;

		t = new struct tx_data;
		if (!t)
		{
				perror("malloc");
				exit(EXIT_FAILURE);

		}

		t->frame = new uint8_t[2048];
		if (!t->frame)
		{
				perror("malloc");
				exit(EXIT_FAILURE);

		}
		t->mem_buf_len = mem_buf_len;
		memcpy(t->frame, frame, mem_buf_len);
		t->notification_id = notification_id;
		t->notification_flag = notification_flag;
		write(tx_pipe[PIPE_WR], t, sizeof(*t));

		/**
		 * If queue_is_waiting is true, wait for the response before returning.
		 */
		if(queue_is_waiting)
		{
			is_waiting = true;
			sem_wait(waiting_sem);
			queue_is_waiting = false;
		}

		return 0;
	}

	int STDCALL system_layer2_multithreaded_callback::set_wait_for_next_cmd(void *notification_id)
	{
		queue_is_waiting = true;
		resp_status_for_cmd = AVDECC_LIB_STATUS_INVALID; // Reset the status

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

	int system_layer2_multithreaded_callback::fn_tx(struct kevent *priv)
	{
		struct tx_data *t;

		t = new struct tx_data;
		if (!t)
		{
				perror("malloc");
				exit(EXIT_FAILURE);

		}

		int result = read(tx_pipe[PIPE_RD], t, sizeof(*t));

		if (result > 0) {
			controller_ref_in_system->tx_packet_event(
				t->notification_id,
				t->notification_flag,
				t->frame,
				t->mem_buf_len);

			if(t->notification_flag == CMD_WITH_NOTIFICATION)
			{
				waiting_notification_id = t->notification_id;
			}

			delete[] t->frame;
		}

		delete t;

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

	int system_layer2_multithreaded_callback::proc_poll_loop()
	{
		// POLL_COUNT
		struct kevent chlist[POLL_COUNT];   /* events we want to monitor */
		struct kevent evlist[POLL_COUNT];   /* events that were triggered */
		int nev, i, kq;

		kq = kqueue();
		if (kq == -1) {
			perror("kqueue");
		}

		EV_SET(&chlist[0], netif_obj_in_system->get_fd(), EVFILT_READ, EV_ADD | EV_ENABLE,
			0, 0, (void *)&system_layer2_multithreaded_callback::fn_netif_cb);

		EV_SET(&chlist[1], tx_pipe[PIPE_RD], EVFILT_READ, EV_ADD | EV_ENABLE,
			0, 0, (void *)&system_layer2_multithreaded_callback::fn_tx_cb);

		EV_SET(&chlist[2], 0, EVFILT_TIMER, EV_ADD | EV_ENABLE,
			0, TIME_PERIOD_25_MILLISECONDS, (void *)&system_layer2_multithreaded_callback::fn_timer_cb);		


		do {
			nev = kevent(kq, chlist, POLL_COUNT, evlist, POLL_COUNT, NULL);

			if (nev == -1) {
				perror("kevent()");
				exit(EXIT_FAILURE);
			}
			else if (nev > 0) {
				for (i = 0; i < nev; i++) {
					int (*fcn)(struct kevent *) = (int(*)(struct kevent *))evlist[i].udata;
					int rv = fcn(&evlist[i]);
					if (rv < 0) {
						return -1;
					}
				}
			}
			
		} while (1);

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
		if (rc){
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
