#pragma once
#include <vector>
#include <string>
#include <queue>

#include "cli_argument.h"

class cmd_line;

class cli_command;
typedef int (cmd_line::*cli_fptr)(int total_matched, std::vector<cli_argument*> args);

class cli_command_format
{
public:
    cli_command_format(std::string help, cli_fptr action=NULL);
    virtual ~cli_command_format() {};

    void add_argument(cli_argument *arg);

    bool run_command(cmd_line *cmd_ptr, std::queue<std::string> args, bool &done);

    void print_help(std::string prefix) const;

private:
    void clear_args();

private:
    std::string m_help;
    std::vector<cli_argument*> m_args;
    cli_fptr m_action;
};

