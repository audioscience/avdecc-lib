#include <assert.h>

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

void cli_command::print_help_all(std::string prefix, int depth) const
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
            cmd_iter->second->print_help_all(sub_prefix, depth - 1);
        }
    }
    else
    {
        printf("%s\n", prefix.c_str());
    }
}

void cli_command::print_help_one(std::queue<std::string> cmd_path, std::string prefix) const
{
    if (cmd_path.size() == 0)
    {
        print_help_details(prefix);
    }
    else
    {
        std::map<std::string, cli_command*>::const_iterator iter = m_sub_commands.find(cmd_path.front());
        if (iter != m_sub_commands.end())
        {
            prefix += cmd_path.front() + " ";
            cmd_path.pop();
            iter->second->print_help_one(cmd_path, prefix);
        }
        else
        {
            printf("Could not find command '%s' to print help for\n", cmd_path.front().c_str());
        }
    }
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

