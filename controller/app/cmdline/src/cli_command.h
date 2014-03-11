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
 * cli_command.h
 *
 * CLI command. A command is a collection of sub-commands and/or formats.
 * This provides a hierarchical set of commands which can be used to parse
 * the user input and call the correct command.
 *
 */

#pragma once
#include <map>
#include <vector>
#include <list>
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

class cmd_line;
class cli_command_format;
class cli_argument;

class cli_command
{
public:
    cli_command(std::string hint="");
    virtual ~cli_command() {};

    void add_sub_command(std::string name, cli_command *cmd);
    void add_format(cli_command_format *format);

    bool run_command(cmd_line *cmd_ptr, std::queue<std::string> args, bool &done, std::string prefix="");

    void print_help_all(std::string prefix, size_t depth) const;
    void print_help_details(std::string prefix) const;

    const cli_command *get_sub_command(std::queue<std::string> &cmd_path, std::string &prefix) const;
    std::list<std::string> get_sub_command_names() const;

    // There can be multiple arguments at a given index (one per command format)
    void get_args(ssize_t index, std::vector<cli_argument*> &args) const;

private:
    std::string m_hint;

    std::map<std::string, cli_command*> m_sub_commands;
    std::list<std::string> m_sub_command_names;

    std::vector<cli_command_format *> m_formats;
};

