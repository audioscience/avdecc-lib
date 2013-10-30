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

#define DEBUG_ONLY_NEED_TO_REMOVE

namespace avdecc_lib
{
	net_interface *netif_obj_in_system;
	controller *controller_ref_in_system;
	system_layer2_multithreaded_callback *local_system = NULL;
	struct system_layer2_multithreaded_callback::msg_poll system_layer2_multithreaded_callback::poll_rx;
	struct system_layer2_multithreaded_callback::msg_poll system_layer2_multithreaded_callback::poll_tx;
	struct system_layer2_multithreaded_callback::thread_creation system_layer2_multithreaded_callback::poll_thread;
	HANDLE system_layer2_multithreaded_callback::poll_events_array[NUM_OF_EVENTS];
	HANDLE system_layer2_multithreaded_callback::waiting_sem;
	bool system_layer2_multithreaded_callback::is_waiting = false;
	void *system_layer2_multithreaded_callback::waiting_notification_id = 0;
	int system_layer2_multithreaded_callback::resp_status_for_cmd = STATUS_INVALID_COMMAND;

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
	}

	system_layer2_multithreaded_callback::~system_layer2_multithreaded_callback()
	{
		delete poll_rx.rx_queue;
		delete poll_tx.tx_queue;
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
#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::queue_tx_frame");
#endif

		struct poll_thread_data thread_data;

		thread_data.frame = (uint8_t *)malloc(1600);
		thread_data.mem_buf_len = mem_buf_len;
		memcpy(thread_data.frame, frame, mem_buf_len);
		thread_data.notification_id = notification_id;
		thread_data.notification_flag = notification_flag;
		poll_tx.tx_queue->queue_push(&thread_data);

		/**
		 * If is_waiting is true, wait for the response before returning.
		 */
		if(is_waiting)
		{
#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::WaitForSingleObject");
#endif
			WaitForSingleObject(waiting_sem, INFINITE);

#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::WaitForSingleObject returned");
#endif
		}

		return 0;
	}

	int STDCALL system_layer2_multithreaded_callback::set_wait_for_next_cmd(void *notification_id)
	{
#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::set_wait_for_next_cmd");
#endif

		is_waiting = true;
		resp_status_for_cmd = STATUS_INVALID_COMMAND; // Reset the status

		return 0;
	}

	int STDCALL system_layer2_multithreaded_callback::get_last_resp_status()
	{
#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::get_last_resp_status");
#endif

		is_waiting = false;
		return resp_status_for_cmd;
	}

	DWORD WINAPI system_layer2_multithreaded_callback::proc_wpcap_thread(LPVOID lpParam)
	{
		struct msg_poll *data = (struct msg_poll *)lpParam;
		int status;
		struct poll_thread_data thread_data;
		const uint8_t *frame;
		uint16_t length;

		while(WaitForSingleObject(data->queue_thread.kill_sem, 0))
		{
			status = netif_obj_in_system->capture_frame(&frame, &length);

			if(status > 0)
			{
				thread_data.mem_buf_len = length;
				thread_data.frame = (uint8_t *)malloc(1600);
				memcpy(thread_data.frame, frame, thread_data.mem_buf_len);
				poll_rx.rx_queue->queue_push(&thread_data);
			}
			else
			{
				if(!SetEvent(data->timeout_event))
				{
					avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "SetEvent pkt_event_wpcap_timeout failed");
					exit(EXIT_FAILURE);
				}
			}
		}

		return 0;
	}

	DWORD WINAPI system_layer2_multithreaded_callback::proc_poll_thread(LPVOID lpParam)
	{
		int status;

		while(WaitForSingleObject(poll_thread.kill_sem, 0))
		{
			status = poll_single();

			if(status != 0)
			{
				break;
			}
		}

		return 0;
	}

	int STDCALL system_layer2_multithreaded_callback::process_start()
	{
		if(init_wpcap_thread() < 0 || init_poll_thread() < 0)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "init_polling error");
		}

		return 0;
	}

	int system_layer2_multithreaded_callback::init_wpcap_thread()
	{
		poll_rx.rx_queue = new system_message_queue(256, sizeof(struct poll_thread_data));
		poll_rx.queue_thread.kill_sem = CreateSemaphore(NULL, 0, 32767, NULL);
		poll_rx.timeout_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		poll_events_array[WPCAP_TIMEOUT] = poll_rx.timeout_event;
		poll_events_array[WPCAP_RX_PACKET] = poll_rx.rx_queue->queue_data_available_object();
		poll_rx.queue_thread.handle = CreateThread(NULL, // Default security descriptor
		                                           0, // Default stack size
		                                           proc_wpcap_thread, // Point to the start address of the thread
		                                           &poll_rx, // Data to be passed to the thread
		                                           0, // Flag controlling the creation of the thread
		                                           &poll_rx.queue_thread.id // Thread identifier
		                                          );

		if(poll_rx.queue_thread.handle == NULL)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "Error creating the wpcap thread");
			exit(EXIT_FAILURE);
		}

		poll_tx.tx_queue = new system_message_queue(256, sizeof(struct poll_thread_data));
		poll_tx.queue_thread.kill_sem = CreateSemaphore(NULL, 0, 32767, NULL);
		poll_tx.timeout_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		poll_events_array[WPCAP_TX_PACKET] = poll_tx.tx_queue->queue_data_available_object();

		poll_events_array[KILL_ALL] = CreateEvent(NULL, FALSE, FALSE, NULL);

		waiting_sem = CreateSemaphore(NULL, 0, 32767, NULL);

		return 0;
	}

	int system_layer2_multithreaded_callback::init_poll_thread()
	{
		poll_thread.kill_sem = CreateEvent(NULL, FALSE, FALSE, NULL);
		poll_thread.handle = CreateThread(NULL, // Default security descriptor //poll_thread_handle = CreateThread(NULL, // Default security descriptor
		                                  0, // Default stack size
		                                  proc_poll_thread, // Point to the start address of the thread
		                                  NULL, // Data to be passed to the thread
		                                  0, // Flag controlling the creation of the thread
		                                  &poll_thread.id // Thread identifier
		                                 );

		if(poll_thread.handle == NULL)
		{
			avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_ERROR, "Error creating the poll thread");
			exit(EXIT_FAILURE);
		}

		return 0;
	}

	int system_layer2_multithreaded_callback::poll_single()
	{
		struct poll_thread_data thread_data;
		DWORD dwEvent;
		DWORD poll_count = sizeof(poll_events_array) / sizeof(HANDLE);
		int status = 0;

		//adp_discovery_state_machine_ref->set_do_discover(true); // Send ENTITY_DISCOVER message
		//adp_discovery_state_machine_ref->adp_discovery_state_waiting(NULL);

		dwEvent = WaitForMultipleObjects(poll_count, poll_events_array, FALSE, INFINITE);

		switch (dwEvent)
		{
			case WAIT_OBJECT_0 + WPCAP_TIMEOUT:
				{
					controller_ref_in_system->time_tick_event();

					if(is_waiting && (!controller_ref_in_system->is_inflight_cmd_with_notification_id(waiting_notification_id)))
					{
						ReleaseSemaphore(waiting_sem, 1, NULL);
					}
				}

				break;

			case WAIT_OBJECT_0 + WPCAP_RX_PACKET:
				{
					poll_rx.rx_queue->queue_pop_nowait(&thread_data);

					bool is_notification_id_valid = false;
					int status = -1;

					controller_ref_in_system->rx_packet_event(thread_data.notification_id,
					                                          is_notification_id_valid,
					                                          thread_data.notification_flag,
					                                          thread_data.frame,
					                                          thread_data.mem_buf_len,
					                                          status);

					if(is_waiting && (!controller_ref_in_system->is_inflight_cmd_with_notification_id(waiting_notification_id)) &&
					   is_notification_id_valid && (waiting_notification_id == thread_data.notification_id))
					{
						resp_status_for_cmd = status;
						ReleaseSemaphore(waiting_sem, 1, NULL);

#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::ReleaseSemaphore");
#endif
					}

					free(thread_data.frame);
				}
				break;

			case WAIT_OBJECT_0 + WPCAP_TX_PACKET:
				poll_tx.tx_queue->queue_pop_nowait(&thread_data);

				controller_ref_in_system->tx_packet_event(thread_data.notification_id, thread_data.notification_flag, thread_data.frame, thread_data.mem_buf_len);

				if(thread_data.notification_flag == avdecc_lib::CMD_WITH_NOTIFICATION)
				{
					waiting_notification_id = thread_data.notification_id;

#ifndef DEBUG_ONLY_NEED_TO_REMOVE
		avdecc_lib::log_ref->logging(avdecc_lib::LOGGING_LEVEL_DEBUG, "Called system_layer2_multithreaded_callback::WPCAP_TX_PACKET event");
#endif
				}

				break;

			case WAIT_OBJECT_0 + KILL_ALL: // Exit or kill event
				status = -1;
				break;
		}

		return status;
	}

	int STDCALL system_layer2_multithreaded_callback::process_close()
	{
		LONG previous;

		ReleaseSemaphore(poll_rx.queue_thread.kill_sem, 1, &previous); // Send kill events to threads
		ReleaseSemaphore(poll_thread.kill_sem, 1, &previous); // Send kill events to threads

		while ((WaitForSingleObject(poll_rx.queue_thread.handle, 0) != WAIT_OBJECT_0) ||
		       (WaitForSingleObject(poll_thread.handle, 0) != WAIT_OBJECT_0)) // Wait for thread termination
		{
			Sleep(100);
		}

		return 0;
	}
}