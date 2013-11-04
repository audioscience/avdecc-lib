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

	time_type timer::clk_monotonic(void)
	{
#ifdef WIN32
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);

		return count.QuadPart;
#elif defined __linux__
#endif
	}

	uint32_t timer::clk_convert_to_ms(time_type time_ms)
	{
#ifdef WIN32
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);

		return (uint32_t)((time_ms * 1000/freq.QuadPart) & 0xfffffff);
#elif defined __linux__
#endif
	}

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
			time_type current_time = clk_monotonic();
			elapsed_ms = (uint32_t)clk_convert_to_ms(current_time - start_time);

			if(elapsed_ms > count)
			{
				elapsed = TRUE;
			}
		}

		return elapsed;
	}
}
