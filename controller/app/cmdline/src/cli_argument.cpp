#include <assert.h>

#include "cmd_line.h"
#include "cli_argument.h"

cli_argument::cli_argument(const std::string name, const std::string help, const std::string hint,
        int match_min, int match_max)
    : m_is_valid(false)
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
    assert(false);
    return 0;
}

uint32_t cli_argument::get_value_uint() const
{
    assert(false);
    return 0;
}

std::string cli_argument::get_value_str() const
{
    assert(false);
    return std::string("");
}

std::vector<int> cli_argument::get_all_value_int() const
{
    assert(false);
    return std::vector<int>();
}

std::vector<uint32_t> cli_argument::get_all_value_uint() const
{
    assert(false);
    return std::vector<uint32_t>();
}

std::vector<std::string> cli_argument::get_all_value_str() const
{
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

bool cli_argument::get_match_min() const
{
    return m_match_min;
}

int cli_argument::get_match_max() const
{
    return m_match_max;
}

cli_argument_int::cli_argument_int(const std::string name, const std::string help, const std::string hint,
        int match_min, int match_max)
    : cli_argument(name, help + " (type int)", hint, match_min, match_max)
{}

void cli_argument_int::clear()
{
    m_values.clear();
    m_is_valid = false;
}

bool cli_argument_int::set_value(cmd_line *cmd_ptr, std::string value_str)
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

int cli_argument_int::get_value_int() const
{
    assert(m_is_valid && m_values.size() == 1);
    return m_values[0];
}

int cli_argument_int::get_all_value_count() const
{
    return m_values.size();
}

std::vector<int> cli_argument_int::get_all_value_int() const
{
    return m_values;
}

cli_argument_end_station::cli_argument_end_station(const std::string name, const std::string help, const std::string hint,
        int match_min, int match_max)
    : cli_argument(name, help + " (index as int or GUID)", hint, match_min, match_max)
{}

void cli_argument_end_station::clear()
{
    m_values.clear();
    m_is_valid = false;
}

bool cli_argument_end_station::set_value(cmd_line *cmd_ptr, std::string value_str)
{
    if (m_values.size() < m_match_max)
    {
        uint32_t value = 0;
        if (cmd_ptr->get_end_station_index(value_str, value))
        {
            m_values.push_back(value);
            m_is_valid = true;
            return true;
        }
    }
    return false;
}

uint32_t cli_argument_end_station::get_value_uint() const
{
    assert(m_is_valid && m_values.size() == 1);
    return m_values[0];
}

int cli_argument_end_station::get_all_value_count() const
{
    return m_values.size();
}

std::vector<uint32_t> cli_argument_end_station::get_all_value_uint() const
{
    return m_values;
}

cli_argument_string::cli_argument_string(const std::string name, const std::string help, const std::string hint,
        int match_min, int match_max)
    : cli_argument(name, help + " (type string)", hint, match_min, match_max)
{}

void cli_argument_string::clear()
{
    m_values.clear();
    m_is_valid = false;
}

bool cli_argument_string::set_value(cmd_line *cmd_ptr, std::string value_str)
{
    if ((value_str.length() > 0) && (m_values.size() < m_match_max))
    {
        m_values.push_back(value_str);
        m_is_valid = true;
        return true;
    }
    return false;
}

std::string cli_argument_string::get_value_str() const
{
    assert(m_is_valid);
    return m_values[0];
}

int cli_argument_string::get_all_value_count() const
{
    return m_values.size();
}

std::vector<std::string> cli_argument_string::get_all_value_str() const
{
    return m_values;
}

