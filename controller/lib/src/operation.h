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
 * operation.h
 *
 * A class for storing AVDECC operations.
 */

#pragma once

#include "timer.h"

namespace avdecc_lib
{
class operation
{
private:
    struct jdksavdecc_frame cmd_frame;
    uint32_t cmd_notification_flag;

public:
    uint16_t operation_id;
    uint16_t operation_type;
    uint16_t percent_complete;
    void *cmd_notification_id;

    operation(struct jdksavdecc_frame *frame,
              uint16_t operation_id,
              uint16_t operation_type,
              void *notification_id,
              uint32_t notification_flag)
        :  cmd_notification_flag(notification_flag), operation_id(operation_id), operation_type(operation_type), cmd_notification_id(notification_id)
    {
        cmd_frame = *frame;
        percent_complete = 0;
    }

    ~operation() {}

    inline struct jdksavdecc_frame frame()
    {
        return cmd_frame;
    }

    inline uint32_t notification_flag()
    {
        return cmd_notification_flag;
    }
};

//
// Class for use in STL find_if() call to find matching operation ID.
//
class operation_id_comp
{
private:
    uint16_t v;

public:
    operation_id_comp(uint16_t i) : v(i) { }

    inline bool operator()(const operation & m) const
    {
        return m.operation_id == v;
    }
};

//
// Class for use in STL find_if() call to find matching notification ID.
//
class notification_comp
{
private:
    void * v;

public:
    notification_comp(void * p) : v(p) { }

    inline bool operator()(const operation & m) const
    {
        return m.cmd_notification_id == v;
    }
};
}

