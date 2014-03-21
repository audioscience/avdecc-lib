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
 * cli_argument.cpp
 *
 * CLI argument implementation
 *
 */

#include <assert.h>
#include <cinttypes>

#include "controller.h"
#include "end_station.h"

#include "cmd_line.h"
#include "cli_argument.h"

cli_argument::cli_argument(cmd_line *cmd_line_ptr, const std::string name,
        const std::string help, const std::string hint,
        size_t match_min, size_t match_max)
    : m_cmd_line_ptr(cmd_line_ptr)
    , m_is_valid(false)
    , m_match_min(match_min)
    , m_match_max(match_max)
    , m_name(name)
    , m_help(help)
    , m_hint(hint)
{
    // Format the indentation of the hint strings
    std::string::size_type pos = 0;
    while ((pos = m_hint.find("\n",pos)) != std::string::npos)
    {
        m_hint.replace(pos, 1, "\n               ");
        pos += 1;
    }
}

const std::string &cli_argument::get_name() const
{
    return m_name;
}

int cli_argument::get_value_int() const
{
    // Base class method. Should never be called unless there is a disconnect between
    // what the CLI (cmd_line_commands_init()) and the implementation method expect.
    assert(false);
    return 0;
}

uint32_t cli_argument::get_value_uint() const
{
    // Base class method. Should never be called unless there is a disconnect between
    // what the CLI (cmd_line_commands_init()) and the implementation method expect.
    assert(false);
    return 0;
}

std::string cli_argument::get_value_str() const
{
    // Base class method. Should never be called unless there is a disconnect between
    // what the CLI (cmd_line_commands_init()) and the implementation method expect.
    assert(false);
    return std::string("");
}

std::vector<int> cli_argument::get_all_value_int() const
{
    // Base class method. Should never be called unless there is a disconnect between
    // what the CLI (cmd_line_commands_init()) and the implementation method expect.
    assert(false);
    return std::vector<int>();
}

std::vector<uint32_t> cli_argument::get_all_value_uint() const
{
    // Base class method. Should never be called unless there is a disconnect between
    // what the CLI (cmd_line_commands_init()) and the implementation method expect.
    assert(false);
    return std::vector<uint32_t>();
}

std::vector<std::string> cli_argument::get_all_value_str() const
{
    // Base class method. Should never be called unless there is a disconnect between
    // what the CLI (cmd_line_commands_init()) and the implementation method expect.
    assert(false);
    return std::vector<std::string>();
}

void cli_argument::print_help() const
{
    printf("  %-8s : %s\n", m_name.c_str(), m_help.c_str());
    if (m_hint.length())
    {
        printf("               %s\n", m_hint.c_str());
    }
}

bool cli_argument::is_valid() const
{
    return m_is_valid;
}

size_t cli_argument::get_match_min() const
{
    return m_match_min;
}

size_t cli_argument::get_match_max() const
{
    return m_match_max;
}

cli_argument_int::cli_argument_int(cmd_line *cmd_line_ptr, const std::string name,
        const std::string help, const std::string hint,
        size_t match_min, size_t match_max)
    : cli_argument(cmd_line_ptr, name, help + " (type int)", hint, match_min, match_max)
{}

void cli_argument_int::clear()
{
    m_values.clear();
    m_is_valid = false;
}

bool cli_argument_int::set_value(std::string value_str)
{
    if (m_values.size() < m_match_max)
    {
        int value = atoi(value_str.c_str());
        if (value_str == "0" || value != 0)
        {
            m_values.push_back(value);
            m_is_valid = true;
            return true;
        }
    }

    return false;
}

void cli_argument_int::get_completion_options(std::set<std::string> &options)
{
    // Do nothing
}

int cli_argument_int::get_value_int() const
{
    assert(m_is_valid && m_values.size() == 1);
    return m_values[0];
}

size_t cli_argument_int::get_all_value_count() const
{
    return m_values.size();
}

std::vector<int> cli_argument_int::get_all_value_int() const
{
    return m_values;
}

cli_argument_end_station::cli_argument_end_station(cmd_line *cmd_line_ptr, const std::string name,
        const std::string help, const std::string hint,
        size_t match_min, size_t match_max)
    : cli_argument(cmd_line_ptr, name, help + " (index as int or Entity ID)", hint, match_min, match_max)
{}

void cli_argument_end_station::clear()
{
    m_values.clear();
    m_is_valid = false;
}

bool cli_argument_end_station::set_value(std::string value_str)
{
    if (m_values.size() < m_match_max)
    {
        uint32_t value = 0;
        if (m_cmd_line_ptr->get_end_station_index(value_str, value))
        {
            m_values.push_back(value);
            m_is_valid = true;
            return true;
        }
    }
    return false;
}

void cli_argument_end_station::get_completion_options(std::set<std::string> &options)
{
    avdecc_lib::controller *controller = m_cmd_line_ptr->get_controller();

    for (size_t i = 0; i < controller->get_end_station_count(); i++)
    {
        char entity_id_str[20];
        avdecc_lib::end_station *end_station = controller->get_end_station_by_index(i);
        sprintf(entity_id_str, "0x%"  PRIx64, end_station->entity_id());
        options.insert(std::string(entity_id_str));
    }
}

uint32_t cli_argument_end_station::get_value_uint() const
{
    assert(m_is_valid && m_values.size() == 1);
    return m_values[0];
}

size_t cli_argument_end_station::get_all_value_count() const
{
    return m_values.size();
}

std::vector<uint32_t> cli_argument_end_station::get_all_value_uint() const
{
    return m_values;
}

cli_argument_string::cli_argument_string(cmd_line *cmd_line_ptr, const std::string name,
        const std::string help, const std::string hint,
        size_t match_min, size_t match_max)
    : cli_argument(cmd_line_ptr, name, help + " (type string)", hint, match_min, match_max)
{}

void cli_argument_string::clear()
{
    m_values.clear();
    m_is_valid = false;
}

bool cli_argument_string::set_value(std::string value_str)
{
    if ((value_str.length() > 0) && (m_values.size() < m_match_max))
    {
        m_values.push_back(value_str);
        m_is_valid = true;
        return true;
    }
    return false;
}

void cli_argument_string::get_completion_options(std::set<std::string> &options)
{
    // Do nothing
}

std::string cli_argument_string::get_value_str() const
{
    assert(m_is_valid && m_values.size() == 1);
    return m_values[0];
}

size_t cli_argument_string::get_all_value_count() const
{
    return m_values.size();
}

std::vector<std::string> cli_argument_string::get_all_value_str() const
{
    return m_values;
}

