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
 * timer.cpp
 *
 * Timer implementation
 */

#include "timer.h"

namespace avdecc_lib
{
	timer::timer()
	{
		running = 0;
		elapsed = 0;
		count = 0;
		start_time = 0;
	}

	timer::~timer() {}

#ifdef WIN32
	avdecc_lib_os::aTimestamp timer::clk_monotonic(void)
	{
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);

		return count.QuadPart;
	}

#elif defined __linux__
	avdecc_lib_os::aTimestamp timer::clk_monotonic(void)
	{
		struct timespec tp;
		avdecc_lib_os::aTimestamp time;

		if ( clock_getres( CLOCK_MONOTONIC, &tp ) != 0 ) {
			printf("Timer not supported in asios_Clock_monotonic(), asios.c\n");
		}

		clock_gettime( CLOCK_MONOTONIC, &tp );
		time = (avdecc_lib_os::aTimestamp)(tp.tv_sec * 1000) + (avdecc_lib_os::aTimestamp)(tp.tv_nsec/1000000);
		return time;
	}
#endif

#ifdef WIN32
	uint32_t timer::clk_convert_to_ms(avdecc_lib_os::aTimestamp time_stamp)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);

		return (uint32_t)((time_stamp * 1000/freq.QuadPart) & 0xfffffff);
	}
#elif defined __linux__
	uint32_t timer::clk_convert_to_ms(avdecc_lib_os::aTimestamp time_stamp)
	{
		return time_stamp;

	}
#endif

	void timer::start(int duration_ms)
	{
		running = true;
		elapsed = false;
		count = duration_ms;
		start_time = clk_monotonic();
	}

	void timer::stop()
	{
		running = false;
		elapsed = false;
	}

	int timer::timeout()
	{
		if(running && !elapsed)
		{
			uint32_t elapsed_ms;
			avdecc_lib_os::aTimestamp current_time = clk_monotonic();
			elapsed_ms = (uint32_t)clk_convert_to_ms(current_time - start_time);

			if(elapsed_ms > count)
			{
				elapsed = true;
			}
		}

		return elapsed;
	}
}
