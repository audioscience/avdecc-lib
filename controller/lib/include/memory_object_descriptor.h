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
 * memory_object_descriptor.h
 *
 * Public MEMORY_OBJECT descriptor interface class
 * The MEMORY_OBJECT descriptor describes a Memory Object.
 */

#pragma once

#include <stdint.h>
#include "build.h"
#include "descriptor_base.h"
#include "memory_object_descriptor_response.h"

namespace avdecc_lib
{
    class memory_object_descriptor : public virtual descriptor_base
    {
    public:
        /**
         * \return the memory object descriptor response class.
         */
        AVDECC_CONTROLLER_LIB32_API virtual STDCALL memory_object_descriptor_response * get_memory_object_response() = 0;

        /**
         * Send a START_OPERATION command with a notification id to begin an operation on the memory object
         *
         * \param notification_id   A void pointer to the unique identifier associated with the command.
         * \param operation_tyoe    An integer representation the operation type to perform on the object
         */
        AVDECC_CONTROLLER_LIB32_API virtual int STDCALL start_operation_cmd(void *notification_id, uint16_t operation_type) = 0;
    };
}

