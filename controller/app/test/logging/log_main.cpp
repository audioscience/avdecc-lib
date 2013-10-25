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
 * log_main.cpp
 *
 * Log main implementation used for testing AVDECC Lib log callback function.
 *
 * (c) AudioScience, Inc. 2013
 */

#include <windows.h>
#include <stdint.h>
#include <iostream>
#include "aem_string.h"
#include "log.h"
#include "enumeration.h"

using namespace std;

extern "C" void log_callback(void *user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_mst)
{
	printf("\n[LOG] (%s, %s)", avdecc_lib::aem_string::logging_level_value_to_name(log_level), log_msg);
}

int main()
{
	avdecc_lib::log *log_ref = new avdecc_lib::log();

	std::cout << "\n" << std::endl;
	log_ref->set_logging_callback(log_callback, NULL);
	log_ref->logging(0, "LOGGING_LEVEL_ERROR");
	log_ref->logging(1, "LOGGING_LEVEL_WARNING");
	log_ref->logging(2, "LOGGING_LEVEL_NOTICE");
	log_ref->logging(3, "LOGGING_LEVEL_INFO");
	log_ref->logging(4, "LOGGING_LEVEL_DEBUG");
	log_ref->logging(5, "LOGGING_LEVEL_VERBOSE");

	Sleep(50);
	delete log_ref;
	return 0;
}