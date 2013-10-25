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
 * notification_main.cpp
 *
 * Notification main implementation used for testing AVDECC Lib notification callback function.
 *
 * (c) AudioScience, Inc. 2013
 */

#include <windows.h>
#include <stdint.h>
#include <iostream>
#include "aem_string.h"
#include "notification.h"
#include "enumeration.h"

using namespace std;

extern "C" void notification_callback(void *user_obj, int32_t notification, uint64_t guid, uint16_t cmd_type,
				      uint16_t desc_type, uint16_t desc_index, uint32_t user_cmd_id)
{

	printf("\n[NOTIFICATION] (%s, 0x%llx, %d, %d, %d, %d)\n",
	       avdecc_lib::aem_string::notification_value_to_name(notification),
	       guid,
	       cmd_type,
	       desc_type,
	       desc_index,
	       user_cmd_id);
}

int main()
{
	avdecc_lib::notification *notification_ref = new avdecc_lib::notification();

	notification_ref->set_notification_callback(notification_callback, NULL);
	notification_ref->notifying(-1, 0, 0, 0, 0, 0);
	notification_ref->notifying(0, 0, 0, 0, 0, 0);
	notification_ref->notifying(1, 0, 0, 0, 0, 0);
	notification_ref->notifying(2, 0, 0, 0, 0, 0);
	notification_ref->notifying(3, 0, 0, 0, 0, 0);
	notification_ref->notifying(10, 0, 0, 0, 0, 0);
	notification_ref->notifying(11, 0, 0, 0, 0, 0);
	notification_ref->notifying(12, 0, 0, 0, 0, 0);
	notification_ref->notifying(13, 0, 0, 0, 0, 0);
	notification_ref->notifying(21, 0, 0, 0, 0, 0);
	notification_ref->notifying(23, 0, 0, 0, 0, 0);

	Sleep(50);

	delete notification_ref;
	return 0;
}