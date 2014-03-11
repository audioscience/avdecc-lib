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
 * cli_argument.h
 *
 * Arguements for commands in the command-line application.
 *
 * Arguments are used to match user input and convert it to the form the command
 * requires. Arguments by default match once. The minimum and maximum number of
 * times they match can be configured.
 *
 */
#pragma once
#include <string>
#include <list>
#include <vector>
#include <set>

#include <stdint.h>

class cmd_line;

class cli_argument
{
public:
    cli_argument(cmd_line *cmd_ptr, const std::string name, const std::string help, const std::string hint,
            size_t match_min, size_t match_max);
    virtual ~cli_argument() {};

    const std::string &get_name() const;

    virtual void clear() = 0;
    virtual bool set_value(std::string value_str) = 0;
    virtual void get_completion_options(std::set<std::string> &options) = 0;

    /**
     * The get_ functions return the first matching value.
     */
    virtual int get_value_int() const;
    virtual uint32_t get_value_uint() const;
    virtual std::string get_value_str() const;

    /**
     * The get_all_ functions return all matching values where an argument has
     * matched more than once
     */
    virtual size_t get_all_value_count() const = 0;
    virtual std::vector<int> get_all_value_int() const;
    virtual std::vector<uint32_t> get_all_value_uint() const;
    virtual std::vector<std::string> get_all_value_str() const;

    void print_help() const;
    bool is_valid() const;
    size_t get_match_min() const;
    size_t get_match_max() const;

protected:
    cmd_line *m_cmd_line_ptr;
    bool m_is_valid;
    size_t m_match_min;
    size_t m_match_max;
    std::string m_name;
    std::string m_help;
    std::string m_hint;
};

class cli_argument_int : public cli_argument
{
public:
    cli_argument_int(cmd_line *cmd_ptr, const std::string name, const std::string help, const std::string hint="",
            size_t match_min=1, size_t match_max=1);
    virtual ~cli_argument_int() {};

    virtual void clear();
    virtual bool set_value(std::string value_str);
    virtual void get_completion_options(std::set<std::string> &options);

    virtual int get_value_int() const;

    virtual size_t get_all_value_count() const;
    virtual std::vector<int> get_all_value_int() const;

private:
    std::vector<int> m_values;
};

class cli_argument_end_station : public cli_argument
{
public:
    cli_argument_end_station(cmd_line *cmd_ptr, const std::string name, const std::string help, const std::string hint="",
            size_t match_min=1, size_t match_max=1);
    virtual ~cli_argument_end_station() {};

    virtual void clear();
    virtual bool set_value(std::string value_str);
    virtual void get_completion_options(std::set<std::string> &options);

    virtual uint32_t get_value_uint() const;

    virtual size_t get_all_value_count() const;
    virtual std::vector<uint32_t> get_all_value_uint() const;

private:
    std::vector<uint32_t> m_values;
};

class cli_argument_string : public cli_argument
{
public:
    cli_argument_string(cmd_line *cmd_ptr, const std::string name, const std::string help, const std::string hint="",
            size_t match_min=1, size_t match_max=1);
    virtual ~cli_argument_string() {};

    virtual void clear();
    virtual bool set_value(std::string value_str);
    virtual void get_completion_options(std::set<std::string> &options);

    virtual std::string get_value_str() const;

    virtual size_t get_all_value_count() const;
    virtual std::vector<std::string> get_all_value_str() const;

private:
    std::vector<std::string> m_values;
};

