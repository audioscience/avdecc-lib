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
 * log.cpp
 *
 * Log class implementation
 */

#include <iostream>
#include "enumeration.h"
#include "log.h"

extern "C" void default_log(void *log_user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms)
{
	printf("avdecc_default_log (%d, %s)\n", log_level, log_msg);
}

namespace avdecc_lib
{
	uint32_t log::read_index = 0;
	uint32_t log::write_index = 0;
	void (*log::callback_func) (void *, int32_t, const char *, int32_t);
	void *log::user_obj;
	HANDLE log::poll_events[2];

	log *log_ref = new log();

	log::log()
	{
		callback_func = default_log;
		user_obj = NULL;
		log_level = LOGGING_LEVEL_ERROR;
		missed_log_event_cnt = 0;

		logging_thread_init(); // Start logging thread
	}

	log::~log() {}

	void log::set_log_level(int32_t new_log_level)
	{
		log_level = new_log_level;
	}

	int log::logging_thread_init()
	{
		poll_events[LOG_EVENT] = CreateSemaphore(NULL, 0, 32767, NULL);
		poll_events[KILL_EVENT] = CreateEvent(NULL, FALSE, FALSE, NULL);

		h_thread = CreateThread(NULL, // Default security descriptor
		                        0, // Default stack size
		                        process_logging_thread, // Point to the start address of the thread
		                        &log_buf, // Data to be passed to the thread
		                        0, // Flag controlling the creation of the thread
		                        &thread_id // Thread identifier
		                       );

		if (h_thread == NULL)
		{
			exit(EXIT_FAILURE);
		}

		return 0;
	}

	DWORD WINAPI log::process_logging_thread(LPVOID lpParam)
	{
		DWORD dwEvent;
		struct log_data *data = (struct log_data *)lpParam;

		while (true)
		{
			dwEvent = WaitForMultipleObjects(2, poll_events, FALSE, INFINITE);

			if (dwEvent == (WAIT_OBJECT_0 + LOG_EVENT))
			{
				if ((write_index - read_index) > 0)
				{
					callback_func(user_obj,
					              data[read_index % LOG_BUF_COUNT].level,
					              data[read_index % LOG_BUF_COUNT].msg,
					              data[read_index % LOG_BUF_COUNT].time_stamp_ms
					             ); // Call callback function

					read_index++;
				}
			}

			else
			{
				SetEvent(poll_events[KILL_EVENT]);
				break;
			}
		}

		return 0;
	}

	void log::logging(int32_t level, const char *fmt,...)
	{
		if (level >= log_level)
		{
			va_list arglist;

			if ((write_index - read_index) > LOG_BUF_COUNT)
			{
				missed_log_event_cnt++;
				return;
			}

			va_start(arglist, fmt);
			vsprintf_s(log_buf[write_index % LOG_BUF_COUNT].msg, 256, fmt, arglist);  // Write to log_buf using write_index
			va_end(arglist);
			log_buf[write_index % LOG_BUF_COUNT].level = level;
			log_buf[write_index % LOG_BUF_COUNT].time_stamp_ms = 0;
			write_index++;

			ReleaseSemaphore(poll_events[LOG_EVENT], 1, NULL);
		}
	}

	void log::set_logging_callback(void (*new_log_callback) (void *, int32_t, const char *, int32_t), void *p)
	{
		callback_func = new_log_callback;
		user_obj = p;
	}

	uint32_t log::get_missed_log_event_count()
	{
		return missed_log_event_cnt;
	}
}
