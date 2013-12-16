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
 * avdecc_lib_os.h
 *
 * Add handles for some OS specific objects.
 */

#pragma once

#if defined __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#if defined __linux__ || defined __MACH__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <cstdint>

#define vsprintf_s vsnprintf
#define InterlockedExchangeAdd __sync_fetch_and_add

#if defined __linux__
#define OS_PTHREAD_MUTEX_RECURSIVE_TYPE PTHREAD_MUTEX_RECURSIVE_NP
#else
#define OS_PTHREAD_MUTEX_RECURSIVE_TYPE PTHREAD_MUTEX_RECURSIVE
#endif

namespace avdecc_lib_os
{
    typedef uint32_t aTimestamp;
    typedef pthread_t *aThread;
    typedef sem_t *aSemaphore;
    typedef pthread_mutex_t aCriticalSection;
}

#elif defined _WIN32 || defined _WIN64

#include <windows.h>
namespace avdecc_lib_os
{
    typedef LONGLONG aTimestamp;
    typedef HANDLE aThread;
    typedef HANDLE aSemaphore;
    typedef CRITICAL_SECTION aCriticalSection;
}


#endif

