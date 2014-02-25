#pragma once
#include <string>

class cmd_line;

class cli_argument
{
public:
    cli_argument(const std::string name, const std::string help, const std::string hint,
            int match_min, int match_max);
    virtual ~cli_argument() {};

    const std::string &get_name() const;

    virtual void clear() = 0;
    virtual bool set_value(cmd_line *cmd_ptr, std::string value_str) = 0;

    virtual int get_value_int() const;
    virtual uint32_t get_value_uint() const;
    virtual std::string get_value_str() const;

    virtual int get_all_value_count() const = 0;
    virtual std::vector<int> get_all_value_int() const;
    virtual std::vector<uint32_t> get_all_value_uint() const;
    virtual std::vector<std::string> get_all_value_str() const;

    void print_help() const;
    bool is_valid() const;
    bool get_match_min() const;
    int  get_match_max() const;

protected:
    bool m_is_valid;
    bool m_match_min;
    int  m_match_max;
    std::string m_name;
    std::string m_help;
    std::string m_hint;
};

class cli_argument_int : public cli_argument
{
public:
    cli_argument_int(const std::string name, const std::string help, const std::string hint="",
            int match_min=1, int match_max=1);
    virtual ~cli_argument_int() {};

    virtual void clear();
    virtual bool set_value(cmd_line *cmd_ptr, std::string value_str);
    virtual int get_value_int() const;

    virtual int get_all_value_count() const;
    virtual std::vector<int> get_all_value_int() const;

private:
    std::vector<int> m_values;
};

class cli_argument_end_station : public cli_argument
{
public:
    cli_argument_end_station(const std::string name, const std::string help, const std::string hint="",
            int match_min=1, int match_max=1);
    virtual ~cli_argument_end_station() {};

    virtual void clear();
    virtual bool set_value(cmd_line *cmd_ptr, std::string value_str);
    virtual uint32_t get_value_uint() const;

    virtual int get_all_value_count() const;
    virtual std::vector<uint32_t> get_all_value_uint() const;

private:
    std::vector<uint32_t> m_values;
};

class cli_argument_string : public cli_argument
{
public:
    cli_argument_string(const std::string name, const std::string help, const std::string hint="",
            int match_min=1, int match_max=1);
    virtual ~cli_argument_string() {};

    virtual void clear();
    virtual bool set_value(cmd_line *cmd_ptr, std::string value_str);
    virtual std::string get_value_str() const;

    virtual int get_all_value_count() const;
    virtual std::vector<std::string> get_all_value_str() const;

private:
    std::vector<std::string> m_values;
};

