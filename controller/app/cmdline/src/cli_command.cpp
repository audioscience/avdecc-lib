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
 * cli_command.cpp
 *
 * CLI command implementation.
 */

#include <assert.h>
#include <limits.h>
#include "cli_command_format.h"
#include "cli_command.h"

cli_command::cli_command(std::string hint)
    : m_hint(hint)
{
}

void cli_command::add_sub_command(std::string name, cli_command *cmd)
{
    std::pair<std::map<std::string, cli_command*>::iterator,bool> ret;
    ret = m_sub_commands.insert(std::pair<std::string, cli_command*>(name, cmd));

    // Ensure that the element was correctly inserted - otherwise trying to add a duplicate command
    assert(ret.second);

    // Keep an ordered list so that the help list is printed in the order commands are added
    m_sub_command_names.push_back(name);
    m_sub_command_names.sort();
}

void cli_command::add_format(cli_command_format *format)
{
    m_formats.push_back(format);
}

bool cli_command::run_command(cmd_line *cmd_ptr, std::queue<std::string> args, bool &done, std::string prefix)
{
    if (args.size() > 0)
    {
        std::map<std::string, cli_command*>::iterator iter = m_sub_commands.find(args.front());
        if (iter != m_sub_commands.end())
        {
            prefix += args.front() + " ";

            args.pop();
            if (iter->second->run_command(cmd_ptr, args, done, prefix))
            {
                return true;
            }
        }
    }

    for (std::vector<cli_command_format*>::iterator iter = m_formats.begin(); iter != m_formats.end(); ++iter)
    {
        if ((*iter)->run_command(cmd_ptr, args, done))
        {
            return true;
        }
    }

    if (m_formats.size())
    {
        // Only print help at leaf nodes
        print_help_details(prefix);

        // Prevent printing of full list of commands
        return true;
    }

    return false;
}

void cli_command::print_help_all(std::string prefix, size_t depth) const
{
    if (depth && m_sub_commands.size())
    {
        for (std::list<std::string>::const_iterator iter = m_sub_command_names.begin();
             iter != m_sub_command_names.end();
             ++iter)
        {
            std::string sub_prefix(prefix);
            if (prefix.length())
                 sub_prefix += " ";
            sub_prefix += *iter;

            std::map<std::string, cli_command*>::const_iterator cmd_iter = m_sub_commands.find(*iter);
            assert(cmd_iter != m_sub_commands.end());
            cmd_iter->second->print_help_all(sub_prefix, (depth == UINT_MAX) ? depth : depth - 1);
        }
    }
    else
    {
        printf("%s\n", prefix.c_str());
    }
}

const cli_command *cli_command::get_sub_command(std::queue<std::string> &cmd_path, std::string &prefix) const
{
    while (cmd_path.size() && cmd_path.front().length() == 0)
    {
        // Ignore any blanks in the path
        cmd_path.pop();
    }

    if ((cmd_path.size() == 0) || (m_sub_commands.size() == 0))
    {
        return this;
    }
    else
    {
        std::map<std::string, cli_command*>::const_iterator iter = m_sub_commands.find(cmd_path.front());
        if (iter != m_sub_commands.end())
        {
            prefix += cmd_path.front() + " ";
            cmd_path.pop();
            return iter->second->get_sub_command(cmd_path, prefix);
        }
    }
    return (cli_command*)NULL;
}

void cli_command::print_help_details(std::string prefix) const
{
    if (m_sub_command_names.size())
    {
        printf("Available sub-commands for \"%s\":\n", prefix.c_str());
        for (std::list<std::string>::const_iterator iter = m_sub_command_names.begin();
             iter != m_sub_command_names.end();
             ++iter)
        {
            printf("  %s\n", (*iter).c_str());
        }
    }

    for (std::vector<cli_command_format*>::const_iterator iter = m_formats.begin(); iter != m_formats.end(); ++iter)
    {
        (*iter)->print_help(prefix);
    }

    if (m_hint.length())
    {
        printf("\n%s\n", m_hint.c_str());
    }
    printf("\n");
}

std::list<std::string> cli_command::get_sub_command_names() const
{
    return m_sub_command_names;
}

void cli_command::get_args(ssize_t index, std::vector<cli_argument*> &args) const
{
    for (std::vector<cli_command_format*>::const_iterator iter = m_formats.begin(); iter != m_formats.end(); ++iter)
    {
        cli_argument *arg = (*iter)->get_arg(index);
        if (arg)
            args.push_back(arg);
    }
}

