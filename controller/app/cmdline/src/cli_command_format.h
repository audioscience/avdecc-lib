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
 * cli_command_format.h
 *
 * CLI command format
 *
 * Each command can have a number of different formats. The format defines the
 * arguments and how they are matched.
 *
 */

#pragma once
#include <vector>
#include <string>
#include <queue>

#if defined _WIN32 || defined _WIN64
#ifdef _MSC_VER
#include <BaseTsd.h>
#ifndef ssize_t
#define ssize_t SSIZE_T
#endif
#endif
#endif

#include "cli_argument.h"
#include "cmd_line.h"

class cli_command;
typedef int (cmd_line::*cli_fptr)(int total_matched, std::vector<cli_argument*> args);

class cli_command_format
{
public:
    cli_command_format(std::string help, cli_fptr action=NULL);
    virtual ~cli_command_format() {};

    void add_argument(cli_argument *arg);
    cli_argument *get_arg(size_t index) const;

    bool run_command(cmd_line *cmd_ptr, std::queue<std::string> args, bool &done);

    void print_help(std::string prefix) const;

private:
    void clear_args();

private:
    std::string m_help;
    std::vector<cli_argument*> m_args;
    cli_fptr m_action;
};

