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
 * cli_command_format.cpp
 *
 * CLI command format implementation
 */

#include <limits.h>
#include <assert.h>

#include "cli_command_format.h"

cli_command_format::cli_command_format(std::string help, cli_fptr action)
    : m_help(help)
    , m_action(action)
{
}

void cli_command_format::add_argument(cli_argument *arg)
{
    m_args.push_back(arg);
}

cli_argument *cli_command_format::get_arg(size_t index) const
{
    if (index < m_args.size())
        return m_args[index];
    else
        return NULL;
}

bool cli_command_format::run_command(cmd_line *cmd_ptr, std::queue<std::string> args, bool &done)
{
    clear_args();

    bool args_ok = true;
    size_t m_i = 0;
    int match_count = 0;
    int total_matched = 0;
    while (args.size() && (m_i < m_args.size()) && args_ok)
    {
        bool ok = m_args[m_i]->set_value(args.front());

        const int match_max = m_args[m_i]->get_match_max();
        const int match_min = m_args[m_i]->get_match_min();
        if (ok)
        {
            args.pop();
            match_count++;
            total_matched++;

            if ((match_max != UINT_MAX) && (match_count >= match_max))
            {
                // Move to next argument - completed this one
                m_i++;
                match_count = 0;
            }
        }
        else if (match_count >= match_min)
        {
            // Move to next argument - matched enough of this one
            m_i++;
            match_count = 0;
        }
        else
        {
            args_ok = false;
        }
    }

    if (args.size())
    {
        // Unused arguments left
        args_ok = false;
    }

    while (m_i < m_args.size() && args_ok)
    {
        if (m_args[m_i]->get_match_min() > m_args[m_i]->get_all_value_count())
        {
            // Left over arguments that are not optional
            args_ok = false;
        }
        m_i++;
    }

    if (args_ok && m_action)
    {
        done = ((cmd_ptr->*(m_action))(total_matched, m_args) != 0);
        return true;
    }

    return false;
}

void cli_command_format::print_help(std::string prefix) const
{
    printf("\n%s", prefix.c_str());
    for (size_t i = 0; i < m_args.size(); i++)
    {
        printf("[%s] ", m_args[i]->get_name().c_str());
    }

    printf("\n%s\n", m_help.c_str());

    if (m_args.size() > 0)
    {
        printf("Parameters:\n");
        for (size_t i = 0; i < m_args.size(); i++)
        {
            m_args[i]->print_help();
        }
    }
}

void cli_command_format::clear_args()
{
    for (std::vector<cli_argument*>::iterator iter = m_args.begin(); iter != m_args.end(); ++iter)
    {
        (*iter)->clear();
    }
}

