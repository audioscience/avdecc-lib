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
    bool while_loop = true;
    bool is_input_valid = false;
    std::ofstream ofstream_ref;
    std::streambuf *cout_buf = std::cout.rdbuf();
    bool is_output_redirected = false;
    std::string cmd_input_orig;
#if defined(__MACH__) || defined(__linux__)
    char* input, shell_prompt[100];
#endif


    std::cout << "\nEnter help for a list of valid commands." << std::endl;

    while(while_loop)
    {
#if defined(__MACH__) || defined(__linux__)
        snprintf(shell_prompt, sizeof(shell_prompt), "$ ");
        input = readline(shell_prompt);

        if (strlen(input) == 0)
            continue;
        if (!input)
            break;
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

        if(is_output_redirected)
        {
            std::cout << "\n> " << cmd_input_orig << std::endl;
        }

        if(input_argv.at(0) == "help")
        {
            if(input_argv.size() == 1)
            {
                avdecc_cmd_line_ref->cmd_help();
            }
            else
            {
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig.substr(5, cmd_input_orig.length()));
            }
        }
        else if(input_argv.at(0) == "version")
        {
            if(input_argv.size() == 1)
            {
                if(is_output_redirected)
                {
                    std::cout.rdbuf(cout_buf);
                    avdecc_cmd_line_ref->cmd_version();
                    std::cout.rdbuf(ofstream_ref.rdbuf());
                }
                else
                {
                    avdecc_cmd_line_ref->cmd_version();
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "show")
        {
            if(input_argv.size() == 2 && input_argv.at(1) == "connections")
            {
                avdecc_cmd_line_ref->cmd_show_connections();
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "list")
        {
            if(input_argv.size() == 1)
            {
                avdecc_cmd_line_ref->cmd_list();
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "select")
        {
            if(input_argv.size() == 1)
            {
                avdecc_cmd_line_ref->cmd_select();
            }
            else if(input_argv.size() == 4)
            {
                uint32_t new_end_station = 0;
                uint16_t new_entity = 0;
                uint16_t new_config = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(1), new_end_station) &&
                   ((input_argv.at(2) == "0") || (atoi(input_argv.at(2).c_str()) != 0)) &&
                   ((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0)))
                {
                    is_input_valid = true;
                    new_entity = (uint16_t)atoi(input_argv.at(2).c_str());
                    new_config = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_select(new_end_station, new_entity, new_config);
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "log")
        {
            if(input_argv.size() == 2)
            {
                if(input_argv.at(1) == "level")
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
                else
                {
                    std::string file = cmd_line::log_path + input_argv.at(1) + ".txt";
                    try
                    {
                        ofstream_ref.open(file);
                        ofstream_ref.good();
                        std::cout.rdbuf(ofstream_ref.rdbuf());
                        is_output_redirected = true;
                    }
                    catch(std::ofstream::failure e)
                    {
                        std::cerr << "\nofstream exception";
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else if((input_argv.size() == 3) && (input_argv.at(1) == "level"))
            {
                uint32_t new_log_level;

                if((input_argv.at(2) == "0") || (atoi(input_argv.at(2).c_str()) != 0))
                {
                    is_input_valid = true;
                    new_log_level = (uint16_t)atoi(input_argv.at(2).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_log_level(new_log_level);
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "view")
        {
            if((input_argv.size() == 2) && (input_argv.at(1) == "all"))
            {
                try
                {
                    avdecc_cmd_line_ref->cmd_view_all();
                }
                catch(std::out_of_range &e)
                {
                    std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                }
            }
            else if((input_argv.size() == 2) && (input_argv.at(1) == "media_clock"))
            {
                avdecc_cmd_line_ref->cmd_view_media_clock();
            }
            else if((input_argv.size() == 3) && (input_argv.at(1) == "details"))
            {
                uint32_t end_station_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(2), end_station_index))
                {
                    is_input_valid = true;
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_view_details(end_station_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                        return -1;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 4) && input_argv.at(1) == "descriptor")
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_view_descriptor(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                        return -1;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "connect")
        {
            if(input_argv.size() == 1)
            {
                avdecc_cmd_line_ref->cmd_connect();
            }
            else if(input_argv.size() == 3)
            {
                uint32_t instream_end_station_index = 0;
                uint16_t instream_desc_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(1), instream_end_station_index) &&
                   ((input_argv.at(2) == "0") || (atoi(input_argv.at(2).c_str()) != 0)))
                {
                    is_input_valid = true;
                    instream_desc_index = (uint16_t)atoi(input_argv.at(2).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_connect(instream_end_station_index, instream_desc_index);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0));
                }
            }
            else if(input_argv.size() >= 5)
            {
                uint32_t outstream_end_station_index = 0;
                uint16_t outstream_desc_index = 0;
                uint32_t instream_end_station_index = 0;
                uint16_t instream_desc_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(1), outstream_end_station_index) &&
                   ((input_argv.at(2) == "0") || (atoi(input_argv.at(2).c_str()) != 0)) &&
                   avdecc_cmd_line_ref->get_end_station_index(input_argv.at(3), instream_end_station_index) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    outstream_desc_index = (uint16_t)atoi(input_argv.at(2).c_str());
                    instream_desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    std::vector<std::string>::const_iterator start = input_argv.begin() + 5;
                    std::vector<std::string>::const_iterator end = input_argv.end();
                    std::vector<std::string> flags(start, end);
                    avdecc_cmd_line_ref->cmd_connect_rx(instream_end_station_index,
                                                        instream_desc_index,
                                                        outstream_end_station_index,
                                                        outstream_desc_index,
                                                        flags);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "disconnect")
        {
            if(input_argv.size() == 5)
            {
                uint32_t outstream_end_station_index = 0;
                uint16_t outstream_desc_index = 0;
                uint32_t instream_end_station_index = 0;
                uint16_t instream_desc_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(1), outstream_end_station_index) &&
                   ((input_argv.at(2) == "0") || (atoi(input_argv.at(2).c_str()) != 0)) &&
                   avdecc_cmd_line_ref->get_end_station_index(input_argv.at(3), instream_end_station_index) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    outstream_desc_index = (uint16_t)atoi(input_argv.at(2).c_str());
                    instream_desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_disconnect_rx(instream_end_station_index, instream_desc_index, outstream_end_station_index, outstream_desc_index);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "acquire")
        {
            if(input_argv.size() == 5 && input_argv.at(1) == "entity")
            {
                uint16_t desc_index = 0;

                if((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_acquire_entity(input_argv.at(2), input_argv.at(3), desc_index);
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "lock")
        {
            if(input_argv.size() == 5 && input_argv.at(1) == "entity")
            {
                uint16_t desc_index = 0;

                if((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_lock_entity(input_argv.at(2), input_argv.at(3), desc_index);
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);

            }
        }
        else if(input_argv.at(0) == "entity")
        {
            if(input_argv.size() == 2 && input_argv.at(1) == "available")
            {
                avdecc_cmd_line_ref->cmd_entity_avail();
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
            }
        }
        else if(input_argv.at(0) == "controller")
        {
            if(input_argv.size() == 2 && input_argv.at(1) == "available")
            {
                avdecc_cmd_line_ref->cmd_controller_avail();
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
            }
        }
        else if(input_argv.at(0) == "read")
        {
            if(input_argv.size() == 4 && input_argv.at(1) == "descriptor")
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_read_descriptor(input_argv.at(2), desc_index);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if((input_argv.at(0) == "set"))
        {
            if((input_argv.size() == 5) && (input_argv.at(1) == "stream_format"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_set_stream_format(input_argv.at(2), desc_index, input_argv.at(4));
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 6) && (input_argv.at(1) == "stream_info"))
            {
                uint16_t desc_index = 0;

                if(((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0)) &&
                   ((input_argv.at(5) == "0") || (atoi(input_argv.at(5).c_str()) != 0)))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_set_stream_info(input_argv.at(2), desc_index, input_argv.at(4), input_argv.at(5).c_str());
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 6) && (input_argv.at(1) == "name"))
            {
                uint16_t desc_index = 0;
                uint16_t name_index = 0;

                if(((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0)) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                    name_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_set_name(input_argv.at(2), desc_index, name_index, input_argv.at(5));
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 5) && (input_argv.at(1) == "sampling_rate"))
            {
                uint16_t desc_index = 0;
                uint32_t new_sampling_rate = 0;

                if(((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0)) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                    new_sampling_rate = (uint32_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_set_sampling_rate(input_argv.at(2), desc_index, new_sampling_rate);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 5) && (input_argv.at(1) == "clock_source"))
            {
                uint16_t desc_index = 0;
                uint16_t new_clk_src_index = 0;

                if(((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0)) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                    new_clk_src_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_set_clock_source(input_argv.at(2), desc_index, new_clk_src_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if((input_argv.at(0) == "get"))
        {
            if((input_argv.size() == 4) && (input_argv.at(1) == "stream_format"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_get_stream_format(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }             
            else if((input_argv.size() == 4) && (input_argv.at(1) == "stream_info"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_get_stream_info(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 5) && (input_argv.at(1) == "name"))
            {
                uint16_t desc_index = 0;
                uint16_t name_index = 0;

                if(((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0)) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                    name_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_get_name(input_argv.at(2), desc_index, name_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 4) && (input_argv.at(1) == "sampling_rate"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_get_sampling_rate(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 4) && (input_argv.at(1) == "clock_source"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_get_clock_source(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else if((input_argv.size() == 5) && (input_argv.at(1) == "tx") && (input_argv.at(2) == "state"))
            {
                uint32_t outstream_end_station_index = 0;
                uint16_t outstream_desc_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(3), outstream_end_station_index) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    outstream_desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_get_tx_state(outstream_end_station_index, outstream_desc_index);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1) + " " + input_argv.at(2));
                }
            }
            else if((input_argv.size() == 5) && (input_argv.at(1) == "rx") && (input_argv.at(2) == "state"))
            {
                uint32_t instream_end_station_index = 0;
                uint16_t instream_desc_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(3), instream_end_station_index) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    instream_desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                        avdecc_cmd_line_ref->cmd_get_rx_state(instream_end_station_index, instream_desc_index);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1) + " " + input_argv.at(2));
                }
            }
            else if((input_argv.size() == 5) && (input_argv.at(1) == "tx") && (input_argv.at(2) == "connection"))
            {
                uint32_t outstream_end_station_index = 0;
                uint16_t outstream_desc_index = 0;

                if(avdecc_cmd_line_ref->get_end_station_index(input_argv.at(3), outstream_end_station_index) &&
                   ((input_argv.at(4) == "0") || (atoi(input_argv.at(4).c_str()) != 0)))
                {
                    is_input_valid = true;
                    outstream_desc_index = (uint16_t)atoi(input_argv.at(4).c_str());
                }

                if(is_input_valid)
                {
                    avdecc_cmd_line_ref->cmd_get_tx_connection(outstream_end_station_index, outstream_desc_index);
                }
                else
                {
                    std::cout << "Invalid Command" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1) + " " + input_argv.at(2));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
               avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if((input_argv.at(0) == "start"))
        {
            if((input_argv.size() == 4) && (input_argv.at(1) == "streaming"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_start_streaming(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if((input_argv.at(0) == "stop"))
        {
            if((input_argv.size() == 4) && (input_argv.at(1) == "streaming"))
            {
                uint16_t desc_index = 0;

                if((input_argv.at(3) == "0") || (atoi(input_argv.at(3).c_str()) != 0))
                {
                    is_input_valid = true;
                    desc_index = (uint16_t)atoi(input_argv.at(3).c_str());
                }

                if(is_input_valid)
                {
                    try
                    {
                        avdecc_cmd_line_ref->cmd_stop_streaming(input_argv.at(2), desc_index);
                    }
                    catch(std::out_of_range &e)
                    {
                        std::cerr << "Out Of Range Exception " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid Command\n" << std::endl;
                    avdecc_cmd_line_ref->cmd_help_details(input_argv.at(0) + " " + input_argv.at(1));
                }
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "path")
        {
            if(input_argv.size() == 1)
            {
                avdecc_cmd_line_ref->cmd_path();
            }
            else if(input_argv.size() == 2)
            {
                avdecc_cmd_line_ref->cmd_path(input_argv.at(1));
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "unlog")
        {
            if(input_argv.size() == 1)
            {
                std::cout.rdbuf(cout_buf);
                ofstream_ref.close();
                is_output_redirected = false;
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "clr")
        {
            if(input_argv.size() == 1)
            {
#if defined(__MACH__) || defined(__linux__)
                std::system("clear");
#else
                std::system("cls");
#endif
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else if(input_argv.at(0) == "q" || input_argv.at(0) == "quit")
        {
            if(input_argv.size() == 1)
            {
                while_loop = false;
            }
            else
            {
                std::cout << "Invalid Command\n" << std::endl;
                avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
            }
        }
        else
        {
            std::cout << "Invalid Command\n" << std::endl;
            avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig);
        }

        is_input_valid = false;
        input_argv.clear();
#if defined(__MACH__) || defined(__linux__)
        free(input);
#endif
    }

    ofstream_ref.close();
    delete avdecc_cmd_line_ref;
    return 0;
}
