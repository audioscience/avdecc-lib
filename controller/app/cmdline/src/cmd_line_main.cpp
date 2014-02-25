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

#include <stdexcept>
#include "cmd_line.h"
#if defined(__MACH__)
#include <readline/readline.h>
#include <readline/history.h>
#elif defined(__linux__)
#include <editline/readline.h>
#include <inttypes.h>
#endif

#if defined(__MACH__) || defined(__linux__)
#include <unistd.h>
#else
#include "getopt.h"
#endif

using namespace std;

extern "C" void notification_callback(void *user_obj, int32_t notification_type, uint64_t guid, uint16_t cmd_type,
                                      uint16_t desc_type, uint16_t desc_index, uint32_t cmd_status,
                                      void *notification_id)
{
    if(notification_type == avdecc_lib::COMMAND_TIMEOUT || notification_type == avdecc_lib::RESPONSE_RECEIVED)
    {
        const char *cmd_name;
        const char *desc_name;
        const char *cmd_status_name;

        if(cmd_type < avdecc_lib::CMD_LOOKUP)
        {
            cmd_name = cmd_line::utility->aem_cmd_value_to_name(cmd_type);
            desc_name = cmd_line::utility->aem_desc_value_to_name(desc_type);
            cmd_status_name = cmd_line::utility->aem_cmd_status_value_to_name(cmd_status);
        }
        else
        {
            cmd_name = cmd_line::utility->acmp_cmd_value_to_name(cmd_type - avdecc_lib::CMD_LOOKUP);
            desc_name = "NULL";
            cmd_status_name = cmd_line::utility->acmp_cmd_status_value_to_name(cmd_status);
        }

        printf("\n[NOTIFICATION] (%s, 0x%llx, %s, %s, %d, %s, %p)\n",
               cmd_line::utility->notification_value_to_name(notification_type),
               guid,
               cmd_name,
               desc_name,
               desc_index,
               cmd_status_name,
               notification_id);
    }
    else
    {
        printf("\n[NOTIFICATION] (%s, 0x%llx, %d, %d, %d, %d, %p)\n",
               cmd_line::utility->notification_value_to_name(notification_type),
               guid,
               cmd_type,
               desc_type,
               desc_index,
               cmd_status,
               notification_id);
    }
}

extern "C" void log_callback(void *user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms)
{
    printf("\n[LOG] %s (%s)\n", cmd_line::utility->logging_level_value_to_name(log_level), log_msg);
}

static void usage(char *argv[])
{
    std::cerr << "Usage: " << argv[0] << " [-d] [-i interface]" << std::endl;
    std::cerr << "  -t           :  Sets test mode which disables checks" << std::endl;
    std::cerr << "  -i interface :  Sets the name of the interface to use" << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    bool test_mode = false;
    int error = 0;
    char *interface = NULL;
    int c = 0;
    while ((c = getopt(argc, argv, "ti:")) != -1) {
        switch (c) {
            case 't':
                test_mode = true;
                break;
            case 'i':
                interface = optarg;
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

    for ( ; optind < argc; optind++) {
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

    cmd_line *avdecc_cmd_line_ref = new cmd_line(notification_callback, log_callback, test_mode, interface);

    std::vector<std::string> input_argv;
    size_t pos = 0;
    bool done = false;
    bool is_input_valid = false;
    std::string cmd_input_orig;
#if defined(__MACH__) || defined(__linux__)
    char* input, shell_prompt[100];
#endif


    std::cout << "\nEnter \"help\" for a list of valid commands." << std::endl;

    while(!done)
    {
#if defined(__MACH__) || defined(__linux__)
        snprintf(shell_prompt, sizeof(shell_prompt), "$ ");
        input = readline(shell_prompt);

        if (!input)
            break;
        if (strlen(input) == 0)
            continue;
        std::string cmd_input(input);
        cmd_input_orig = cmd_input;
        add_history(input);
#else 
        std::string cmd_input;
        printf("\n>");
        std::getline(std::cin, cmd_input);
        cmd_input_orig = cmd_input;
#endif

        while((pos = cmd_input.find(" ")) != std::string::npos)
        {
            if (pos)
              input_argv.push_back(cmd_input.substr(0, pos));

            cmd_input.erase(0, pos + 1);
        }

        if(cmd_input != " ")
        {
            input_argv.push_back(cmd_input);
        }

        if(avdecc_cmd_line_ref->is_output_redirected())
        {
            std::cout << "\n> " << cmd_input_orig << std::endl;
        }

        done = avdecc_cmd_line_ref->handle(input_argv);

        is_input_valid = false;
        input_argv.clear();
#if defined(__MACH__) || defined(__linux__)
        free(input);
#endif
    }

    delete avdecc_cmd_line_ref;
    return 0;
}
