#pragma once
#include <map>
#include <vector>
#include <list>
#include <string>
#include <queue>

class cmd_line;
class cli_command_format;

class cli_command
{
public:
    cli_command(std::string hint="");
    virtual ~cli_command() {};

    void add_sub_command(std::string name, cli_command *cmd);
    void add_format(cli_command_format *format);

    bool run_command(cmd_line *cmd_ptr, std::queue<std::string> args, bool &done, std::string prefix="");

    void print_help_all(std::string prefix, int depth) const;
    void print_help_one(std::queue<std::string> cmd_path, std::string prefix="") const;
    void print_help_details(std::string prefix) const;

private:
    std::string m_hint;

    std::map<std::string, cli_command*> m_sub_commands;
    std::list<std::string> m_sub_command_names;

    std::vector<cli_command_format *> m_formats;
};

