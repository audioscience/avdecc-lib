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

#include "net_interface.h"
#include "enumeration.h"
#include "notification.h"
#include "log.h"
#include "end_station.h"
#include "controller.h"
#include "system_message_queue.h"
#include "system_tx_queue.h"
#include "system_layer2_multithreaded_callback.h"

namespace avdecc_lib
{
	net_interface *netif_obj_in_system;
	controller *controller_ref_in_system;
	system_layer2_multithreaded_callback *local_system = NULL;

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

	system_layer2_multithreaded_callback::system_layer2_multithreaded_callback() {}

	system_layer2_multithreaded_callback::system_layer2_multithreaded_callback(net_interface *netif, controller *controller_obj)
	{
		netif_obj_in_system = netif;
		controller_ref_in_system = controller_obj;
		queue_is_waiting = false;
	}

	system_layer2_multithreaded_callback::~system_layer2_multithreaded_callback()
	{
		delete netif_obj_in_system;
		delete controller_ref_in_system;
		delete local_system;
	}

	void STDCALL system_layer2_multithreaded_callback::destroy()
	{
		delete this;
	}

	int system_layer2_multithreaded_callback::queue_tx_frame(void *notification_id, uint32_t notification_flag, uint8_t *frame, size_t mem_buf_len)
	{
#if 0
		struct poll_thread_data thread_data;

		thread_data.frame = (uint8_t *)malloc(1600);
		thread_data.mem_buf_len = mem_buf_len;
		memcpy(thread_data.frame, frame, mem_buf_len);
		thread_data.notification_id = notification_id;
		thread_data.notification_flag = notification_flag;
		poll_tx.tx_queue->queue_push(&thread_data);

		/**
		 * If queue_is_waiting is true, wait for the response before returning.
		 */
		if(queue_is_waiting)
		{
			is_waiting = true;
			WaitForSingleObject(waiting_sem, INFINITE);
			queue_is_waiting = false;
		}
#endif
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



	void * system_layer2_multithreaded_callback::proc_poll_thread(void * p)
	{

		return 0;
	}

	int STDCALL system_layer2_multithreaded_callback::process_start()
	{
		struct itimerspec itimerspec_new;
		struct itimerspec itimerspec_old;
		const unsigned long ns_per_ms = 1000000;
		unsigned int interval_ms = 10;
		int rc;

		/* create and init the timer */
		tick_timer = timerfd_create(CLOCK_MONOTONIC, 0);
		if (-1 != tick_timer)
		{
			fcntl(tick_timer, F_SETFL, O_NONBLOCK);	
			memset(&itimerspec_new, 0, sizeof(itimerspec_new));
			memset(&itimerspec_old, 0, sizeof(itimerspec_old));
			if (interval_ms)
			{
				itimerspec_new.it_interval.tv_sec = interval_ms / 1000;
				itimerspec_new.it_interval.tv_nsec =
				    (interval_ms % 1000) * ns_per_ms;
			}
			itimerspec_new.it_value.tv_sec = interval_ms / 1000;
			itimerspec_new.it_value.tv_nsec = (interval_ms % 1000) * ns_per_ms;
			rc = timerfd_settime(tick_timer, 0, &itimerspec_new, &itimerspec_old);
			if (rc < 0)
			{
	        		perror("timerset");
        			exit(EXIT_FAILURE);
			}

		}
		else
		{
        		perror("timer");
        		exit(EXIT_FAILURE);
        	}

		/* create the tx pipe for sending tx packets */
		if (pipe(tx_pipe) == -1) {
        		perror("pipe");
        		exit(EXIT_FAILURE);
		}
	
		//network_fd = netif_obj_in_system->



		return 0;
	}


	int system_layer2_multithreaded_callback::poll_single(void)
	{
		return 0;
	}

	int STDCALL system_layer2_multithreaded_callback::process_close()
	{

		return 0;
	}
}
