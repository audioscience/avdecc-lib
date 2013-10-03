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
 * aem_string.h
 *
 * AEM String conversion class
 */

#pragma once
#ifndef _AEM_STRING_H_
#define _AEM_STRING_H_

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
        class aem_string
        {
        public:
                /**
                 * Convert command value to its corresponding command name.
                 */
                static AVDECC_LIB_API const char * STDCALL cmd_value_to_name(uint16_t cmd_value);

                /**
                 * Convert command name to its corresponding command value.
                 */
                static AVDECC_LIB_API uint16_t STDCALL cmd_name_to_value(const char *cmd_name);

                /**
                 * Convert descriptor value to its corresponding descriptor name.
                 */
                static AVDECC_LIB_API const char * STDCALL desc_value_to_name(uint16_t desc_value);

                /**
                 * Convert descriptor name to its corresponding descriptor value.
                 */
                static AVDECC_LIB_API uint16_t STDCALL desc_name_to_value(const char *desc_name);

                /**
                 * Convert command status value to its corresponding command status name.
                 */
                static AVDECC_LIB_API const char * STDCALL cmd_status_value_to_name(uint16_t cmd_status_value);

                /**
                 * Convert notifying value to its corresponding notifying name.
                 */
                static AVDECC_LIB_API const char * STDCALL notification_value_to_name(uint16_t notification_value);

                /**
                 * Convert logging value to its corresponding logging name.
                 */
                static AVDECC_LIB_API const char * STDCALL logging_level_value_to_name(uint16_t logging_level_value);
        };
}

#endif