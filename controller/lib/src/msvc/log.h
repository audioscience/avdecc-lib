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
 * log.h
 *
 * Log class, which is called by AVDECC LIB modules for logging purposes.
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_LOG_H_
#define _AVDECC_CONTROLLER_LIB_LOG_H_

#include <windows.h>
#include <stdint.h>

namespace avdecc_lib
{
	class log
	{
	private:
		int32_t log_level;
		static uint32_t read_index;
		static uint32_t write_index;
		static void (*callback_func)(void *, int32_t, const char *, int32_t);
		static void *user_obj;
		static uint32_t missed_log_event_cnt;

		enum
		{
		        LOG_BUF_COUNT = 32
		};

		enum events
		{
		        LOG_EVENT,
		        KILL_EVENT
		};

		LPTHREAD_START_ROUTINE thread;
		HANDLE h_thread;
		DWORD thread_id;

		static HANDLE poll_events[2];

		struct log_data
		{
			int32_t level;
			char msg[256];
			int32_t time_stamp_ms;
		};

		struct log_data log_buf[LOG_BUF_COUNT];

	public:
		/**
		 * An empty constructor for log
		 */
		log();

		/**
		 * Destructor for log used for destroying objects
		 */
		virtual ~log();

		/**
		 * Update the base log level for messages to be logged by the logging callback.
		 */
		void set_log_level(int32_t new_log_level);

		/**
		 * Create and initialize logging thread, event, and semaphore.
		 */
		int logging_thread_init();

		/**
		 * Start of the logging thread used for logging purposes.
		 */
		static DWORD WINAPI process_logging_thread(LPVOID lpParam);

		/**
		 * AVDECC LIB modules call this function for logging purposes.
		 */
		void logging(int32_t log_level, const char *fmt,...);

		/**
		 * Change the logging callback function to a new logging callback function.
		 */
		void set_logging_callback(void (*new_log_callback) (void *, int32_t, const char *, int32_t), void *);

		/**
		 * Get the number of missed log that exceeds the log buffer count.
		 */
		uint32_t get_missed_log_event_count();
	};

	extern log *log_ref;
}

#endif
