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
 * avdecc_cmd_line_main.cpp
 *
 * AVDECC command line main implementation used for testing command line interface.
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>
#include <cinttypes>

#include <stdexcept>
#include "cmd_line.h"
#if defined(__MACH__)
#include <readline/readline.h>
#include <readline/history.h>
#elif defined(__linux__)
#include <readline/readline.h>
#include <readline/history.h>
#endif

#if defined(__MACH__) || defined(__linux__)
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// For TAB-completion
#include "cli_argument.h"
#include <set>
#else
#include "msvc\getopt.h"
#endif

using namespace std;

bool supress_unsolicited_response_notifications = false;

extern "C" void notification_callback(void * user_obj, int32_t notification_type, uint64_t entity_id, uint32_t msg_type, uint16_t cmd_type,
                                      uint16_t desc_type, uint16_t desc_index, uint32_t cmd_status,
                                      void * notification_id)
{
    if (notification_type == avdecc_lib::COMMAND_TIMEOUT || notification_type == avdecc_lib::RESPONSE_RECEIVED)
    {
        const char * cmd_name;
        const char * desc_name;
        const char * cmd_status_name;

        if (cmd_type < avdecc_lib::CMD_LOOKUP)
        {
            cmd_name = avdecc_lib::utility::aem_cmd_value_to_name(cmd_type);
            desc_name = avdecc_lib::utility::aem_desc_value_to_name(desc_type);
            cmd_status_name = avdecc_lib::utility::aem_cmd_status_value_to_name(cmd_status);
        }
        else
        {
            cmd_name = avdecc_lib::utility::acmp_cmd_value_to_name(cmd_type - avdecc_lib::CMD_LOOKUP);
            desc_name = "NULL";
            cmd_status_name = avdecc_lib::utility::acmp_cmd_status_value_to_name(cmd_status);
        }

        printf("\n[NOTIFICATION] (%s, 0x%" PRIx64 ", %d, %s, %s, %d, %s, %p)\n",
               avdecc_lib::utility::notification_value_to_name(notification_type),
               entity_id,
               msg_type,
               cmd_name,
               desc_name,
               desc_index,
               cmd_status_name,
               notification_id);
    }
    else if (supress_unsolicited_response_notifications == false ||
			notification_type != avdecc_lib::UNSOLICITED_RESPONSE_RECEIVED)
    {
        printf("\n[NOTIFICATION] (%s, 0x%" PRIx64 ", %d, %d, %d, %d, %d, %p)\n",
               avdecc_lib::utility::notification_value_to_name(notification_type),
               entity_id,
               msg_type,
               cmd_type,
               desc_type,
               desc_index,
               cmd_status,
               notification_id);
    }
    
    fflush(stdout);
}

extern "C" void acmp_notification_callback(void * user_obj, int32_t notification_type, uint16_t cmd_type,
                                           uint64_t talker_entity_id, uint16_t talker_unique_id,
                                           uint64_t listener_entity_id, uint16_t listener_unique_id,
                                           uint32_t cmd_status, void * notification_id)
{
    if (notification_type == avdecc_lib::ACMP_RESPONSE_RECEIVED || notification_type == avdecc_lib::BROADCAST_ACMP_RESPONSE_RECEIVED)
    {
        const char * cmd_name;
        const char * cmd_status_name;

        if (cmd_type < avdecc_lib::CMD_LOOKUP)
        {
            cmd_name = avdecc_lib::utility::aem_cmd_value_to_name(cmd_type);
            cmd_status_name = avdecc_lib::utility::aem_cmd_status_value_to_name(cmd_status);
        }
        else
        {
            cmd_name = avdecc_lib::utility::acmp_cmd_value_to_name(cmd_type - avdecc_lib::CMD_LOOKUP);
            cmd_status_name = avdecc_lib::utility::acmp_cmd_status_value_to_name(cmd_status);
        }

        printf("\n[NOTIFICATION] (%s, %s, 0x%llx, %d, 0x%llx, %d, %s, %p)\n",
               avdecc_lib::utility::acmp_notification_value_to_name(notification_type),
               cmd_name,
               (long long unsigned int)talker_entity_id,
               talker_unique_id,
               (long long unsigned int)listener_entity_id,
               listener_unique_id,
               cmd_status_name,
               notification_id);
    }
    
    fflush(stdout);
}

extern "C" void log_callback(void * user_obj, int32_t log_level, const char * log_msg, int32_t time_stamp_ms)
{
    printf("\n[LOG] %s (%s)\n", avdecc_lib::utility::logging_level_value_to_name(log_level), log_msg);
    
    fflush(stdout);
}

#if defined(__MACH__) || defined(__linux__)
const cli_command * top_level_command;
const cli_command * current_command;
int complettion_arg_index = 0;

void split(const std::string & s, char delim, std::queue<std::string> & elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push(item);
}

char * command_generator(const char * text, int state)
{
    static std::list<std::string> completion_options;
    static int len;

    if (!current_command)
        return NULL;

    if (!state)
    {
        // New word to complete then set up the options to match

        // Cache the len for efficiency
        len = strlen(text);

        // Try the sub-commands of the current command
        completion_options = current_command->get_sub_command_names();
        if (!completion_options.size())
        {
            // If there are no sub-commands then try the arguments
            std::vector<cli_argument *> args;
            std::set<std::string> arg_options;

            // There can be multiple arguments at a given index as there
            // can be multiple command formats
            current_command->get_args(complettion_arg_index, args);
            for (std::vector<cli_argument *>::iterator iter = args.begin();
                 iter != args.end();
                 ++iter)
            {
                (*iter)->get_completion_options(arg_options);
            }
            completion_options.insert(completion_options.end(),
                                      arg_options.begin(), arg_options.end());
        }
    }

    // Return the next name which matches from the command list
    while (completion_options.size())
    {
        std::string sub_command = completion_options.front();
        completion_options.pop_front();

        if (strncmp(sub_command.c_str(), text, len) == 0)
            return (strdup(sub_command.c_str()));
    }

    // There are no matches
    return NULL;
}

char ** command_completer(const char * text, int start, int end)
{
    if (start == 0)
    {
        // Start of a new command
        current_command = top_level_command;
    }
    else
    {
        // In the middle of a command line, use the rest of the line
        // to find the right command to provide completion options
        std::string cmd_path(rl_line_buffer);
        cmd_path = cmd_path.substr(0, start);
        std::queue<std::string, std::deque<std::string>> cmd_path_queue;
        split(cmd_path, ' ', cmd_path_queue);

        std::string prefix;
        current_command = top_level_command->get_sub_command(cmd_path_queue, prefix);

        // There can be remaining parts of the command which mean that an argument
        // value is being completed instead
        complettion_arg_index = cmd_path_queue.size();
    }

    char ** matches = rl_completion_matches(text, command_generator);
    return matches;
}

char * null_completer(const char * text, int state)
{
    return NULL;
}
#endif

// static function to print the network interfaces
static void print_interfaces()
{
    avdecc_lib::net_interface * netif = avdecc_lib::create_net_interface();
    for (uint32_t i = 1; i < netif->devs_count() + 1; i++)
    {
        size_t dev_index = i - 1;
        char * dev_desc = netif->get_dev_desc_by_index(dev_index);
        printf("%d (%s)", i, dev_desc);
        
        uint64_t dev_mac = netif->get_dev_mac_addr_by_index(dev_index);
        if (dev_mac)
        {
            avdecc_lib::utility::MacAddr mac(dev_mac);
            char mac_str[20];
            mac.tostring(mac_str);
            printf(" (%s)", mac_str);
        }
        
        size_t ip_addr_count = netif->device_ip_address_count(dev_index);
        if (ip_addr_count > 0)
        {
            for(size_t ip_index = 0; ip_index < ip_addr_count; ip_index++)
            {
                const char * dev_ip = netif->get_dev_ip_address_by_index(dev_index, ip_index);
                if (dev_ip)
                    printf(" <%s>", dev_ip);
            }
        }
        printf("\n");
    }
}

static void usage(char * argv[])
{
    std::cerr << "Usage: " << argv[0] << " [<options>] [-i interface][-l log_level]" << std::endl;
    std::cerr << "  -p           :  Print interfaces" << std::endl;
    std::cerr << "  -t           :  Sets test mode which disables checks" << std::endl;
#if !(defined(__MACH__) || defined(__linux__))
    std::cerr << "  -s           :  Show command separator" << std::endl;
#endif
    std::cerr << "  -q           :  Supress notification of unsolicited responses." << std::endl;
    std::cerr << "  -i interface :  Sets the network interface to use.\n \
                    Valid options are IP Address and MAC Address (must be in the form 'n:n:n:n:n:n', where 0<=n<=FF in hexidecimal" << std::endl;
    std::cerr << "  -l log_level :  Sets the log level to use." << std::endl;
    std::cerr << log_level_help << std::endl;
    exit(1);
}

int main(int argc, char * argv[])
{
#if !(defined(__MACH__) || defined(__linux__))
    bool show_cmd_separator = false;
#endif
    bool test_mode = false;
    int error = 0;
    char * interface = NULL;
    int c = 0;
    int32_t log_level = avdecc_lib::LOGGING_LEVEL_ERROR;

    while ((c = getopt(argc, argv, "sptqi:l:")) != -1)
    {
        switch (c)
        {
        case 'p':
            // print the network interfaces and exit
            print_interfaces();
            exit(EXIT_SUCCESS);
#if !(defined(__MACH__) || defined(__linux__))
        case 's':
            show_cmd_separator = true;
            break;
#endif
        case 't':
            test_mode = true;
            break;
		case 'q':
			supress_unsolicited_response_notifications = true;
			break;
        case 'i':
            interface = optarg;
            break;
        case 'l':
            log_level = atoi(optarg);
            break;
        case ':':
            fprintf(stderr, "Option -%c requires an operand\n", optopt);
            error++;
            break;
        case '?':
            fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
            error++;
            break;
        }
    }

    for (; optind < argc; optind++)
    {
        error++; // Unused arguments
    }

    if (error)
    {
        usage(argv);
    }

    if (test_mode)
    {
        // Ensure that stdout is not buffered
        setvbuf(stdout, NULL, _IOLBF, 0);
    }

    cmd_line avdecc_cmd_line_ref(notification_callback, acmp_notification_callback, log_callback,
                                 test_mode, interface, log_level);

    std::vector<std::string> input_argv;
    size_t pos = 0;
    bool done = false;
    //bool is_input_valid = false;
    std::string cmd_input_orig;
#if defined(__MACH__) || defined(__linux__)
    char * input;

    // Set up the state for command-line completion
    top_level_command = avdecc_cmd_line_ref.get_commands();
    rl_attempted_completion_function = command_completer;

#endif
// Override to prevent filename completion
#if defined(__MACH__)

// This macro can be set via cmake command line argument -DUSE_GNU_READLINE=ON
#ifdef USE_GNU_READLINE
    // GNU Readline library
    rl_completion_entry_function = (rl_compentry_func_t *)null_completer;
#else
    // OSX default libedit library
    rl_completion_entry_function = (Function *)null_completer;
#endif
#elif defined(__linux__)
    rl_completion_entry_function = null_completer;
#endif

    std::cout << "\nEnter \"help\" for a list of valid commands, or \"help -a\" for an expanded list of subcommands." << std::endl;

    while (!done)
    {
#if defined(__MACH__) || defined(__linux__)
        input = readline("$ ");

        if (!input)
            break;
        if (strlen(input) == 0)
            continue;
        std::string cmd_input(input);
        cmd_input_orig = cmd_input;
        add_history(input);
#else
        std::string cmd_input;
        if (show_cmd_separator)
            printf("\n_\n");
        printf("\n>");
        std::getline(std::cin, cmd_input);
        cmd_input_orig = cmd_input;
#endif

        while ((pos = cmd_input.find(" ")) != std::string::npos)
        {
            if (pos)
                input_argv.push_back(cmd_input.substr(0, pos));

            cmd_input.erase(0, pos + 1);
        }

        if (cmd_input.length() && cmd_input != " ")
        {
            input_argv.push_back(cmd_input);
        }

        if (avdecc_cmd_line_ref.is_output_redirected())
        {
            std::cout << "\n> " << cmd_input_orig << std::endl;
        }

        done = avdecc_cmd_line_ref.handle(input_argv);

        //is_input_valid = false;
        input_argv.clear();
#if defined(__MACH__) || defined(__linux__)
        free(input);
#endif
    }

    return 0;
}
