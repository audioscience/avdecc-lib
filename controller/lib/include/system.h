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
 * system.h
 *
 * Public System interface class
 */

#pragma once
#ifndef _AVDECC_CONTROLLER_LIB_SYSTEM_H_
#define _AVDECC_CONTROLLER_LIB_SYSTEM_H_

#include <stdint.h>
#include "build.h"

namespace avdecc_lib
{
        class net_interface;
        class controller;

        class system
        {
        public:
                /**
                 * Deallocate memory
                 */
                AVDECC_CONTROLLER_LIB32_API virtual void STDCALL destroy() = 0;

                /**
                 * Start point of the system process, which calls the thread initialization function.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual int STDCALL process_start() = 0;

                /**
                 * End point of the system process, which terminates the threads.
                 */
                AVDECC_CONTROLLER_LIB32_API virtual int STDCALL process_close() = 0;
        };

        /**
         * Create a public AVDECC System object with notification and logging callback functions used for accessing from outside the library.
         */
        extern "C" AVDECC_CONTROLLER_LIB32_API system * STDCALL create_system(net_interface *netif, controller *controller_ref);
}

#endif