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
#include <stdint.h>
#include <stdexcept>
#include "cmd_line.h"
#ifdef __MACH__
#include <readline/readline.h>
#include <readline/history.h>
#endif

using namespace std;

extern "C" void notification_callback(void *user_obj, int32_t notification_type, uint64_t guid, uint16_t cmd_type,
                                      uint16_t desc_type, uint16_t desc_index, void *notification_id)
{
	avdecc_lib::util *utility = avdecc_lib::create_util();

	if(notification_type == avdecc_lib::COMMAND_TIMEOUT || notification_type == avdecc_lib::RESPONSE_RECEIVED)
	{
		printf("\n[NOTIFICATION] (%s, 0x%llx, %s, %s, %d, %p)\n",
		       utility->notification_value_to_name(notification_type),
		       guid,
		       utility->cmd_value_to_name(cmd_type),
		       utility->desc_value_to_name(desc_type),
		       desc_index,
		       notification_id);
	}
	else
	{
		printf("\n[NOTIFICATION] (%s, 0x%llx, %d, %d, %d, %p)\n",
		       utility->notification_value_to_name(notification_type),
		       guid,
		       cmd_type,
		       desc_type,
		       desc_index,
		       notification_id);
	}
}

extern "C" void log_callback(void *user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms)
{
	avdecc_lib::util *utility = avdecc_lib::create_util();

	printf("\n[LOG] %s (%s)\n", utility->logging_level_value_to_name(log_level), log_msg);
}

int main()
{
	avdecc_cmd_line *avdecc_cmd_line_ref = new avdecc_cmd_line(notification_callback, log_callback);

	std::vector<std::string> argv;
	size_t pos = 0;
	bool while_loop = true;
	bool is_input_valid = false;
	std::ofstream ofstream_ref;
	std::streambuf *cout_buf = std::cout.rdbuf();
	bool is_output_redirected = false;
	std::string cmd_input_orig;
#ifdef __MACH__
	char* input, shell_prompt[100];
#endif

	std::cout << "\nEnter help for a list of valid commands." << std::endl;

	while(while_loop)
	{
#ifdef __MACH__
		snprintf(shell_prompt, sizeof(shell_prompt), "$ ");
		input = readline(shell_prompt);
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
			argv.push_back(cmd_input.substr(0, pos));
			cmd_input.erase(0, pos + 1);
		}

		if(cmd_input.compare(" ") != 0)
		{
			argv.push_back(cmd_input);
		}

		if(argv.at(0) == "help")
		{
			if(argv.size() == 1)
			{
				avdecc_cmd_line_ref->cmd_help();
			}
			else
			{
				avdecc_cmd_line_ref->cmd_help_details(cmd_input_orig.substr(5, cmd_input_orig.length()));
			}
		}
		else if(argv.at(0) == "version")
		{
			if(argv.size() == 1)
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
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "list")
		{
			if(argv.size() == 1)
			{
				avdecc_cmd_line_ref->cmd_list();
			}
			else if((argv.at(1) == "clock_sync_source") &&
			        (argv.size() == 2))
			{
				avdecc_cmd_line_ref->cmd_list_clock_sync_source();
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}

		}
		else if(argv.at(0) == "select")
		{
			if(argv.size() == 1)
			{
				avdecc_cmd_line_ref->cmd_select();
			}
			else if(argv.size() == 4)
			{
				uint32_t new_end_station = 0x0;
				uint16_t new_entity = 0x0;
				uint16_t new_config = 0x0;

				if(((argv.at(1) == "0") || (atoi(argv.at(1).c_str()) != 0)) &&
				   ((argv.at(2) == "0") || (atoi(argv.at(2).c_str()) != 0)) &&
				   ((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)))
				{
					is_input_valid = true;
					new_end_station = (uint16_t)atoi(argv.at(1).c_str());
					new_entity = (uint16_t)atoi(argv.at(2).c_str());
					new_config = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_select(new_end_station, new_entity, new_config);
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "log")
		{
			if(argv.size() == 2)
			{
				std::string file = avdecc_cmd_line::log_path + argv.at(1) + ".txt";
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
			else if((argv.size() == 3) && (argv.at(1) == "level"))
			{
				uint32_t new_log_level;

				if((argv.at(2) == "0") || (atoi(argv.at(1).c_str()) != 0))
				{
					is_input_valid = true;
					new_log_level = (uint16_t)atoi(argv.at(2).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_log_level(new_log_level);
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "view")
		{
			if((argv.size() == 2) && (argv.at(1) == "all"))
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
			else if((argv.size() == 4) && argv.at(1) == "descriptor")
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_view_descriptor(argv.at(2), desc_index);
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
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "connect")
		{
			if(argv.size() == 1)
			{
				avdecc_cmd_line_ref->cmd_connect();
			}
			else if(argv.size() == 3)
			{
				uint32_t dest_end_station_index = 0x0;
				uint16_t dest_desc_index = 0x0;

				if(((argv.at(1) == "0") || (atoi(argv.at(1).c_str()) != 0)) &&
				   ((argv.at(2) == "0") || (atoi(argv.at(2).c_str()) != 0)))
				{
					is_input_valid = true;
					dest_end_station_index = (uint16_t)atoi(argv.at(1).c_str());
					dest_desc_index = (uint16_t)atoi(argv.at(2).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_connect(dest_end_station_index, dest_desc_index);
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
			}
			else if(argv.size() == 5)
			{
				std::cout << "Not Implemented" << std::endl;
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}

		}
		else if(argv.at(0) == "disconnect")
		{
			if(argv.size() == 1)
			{
				avdecc_cmd_line_ref->cmd_disconnect();
			}
			else if(argv.size() == 3)
			{
				uint32_t dest_end_station_index = 0x0;
				uint16_t dest_desc_index = 0x0;

				if(((argv.at(1) == "0") || (atoi(argv.at(1).c_str()) != 0)) &&
				   ((argv.at(2) == "0") || (atoi(argv.at(2).c_str()) != 0)))
				{
					is_input_valid = true;
					dest_end_station_index = (uint16_t)atoi(argv.at(1).c_str());
					dest_desc_index = (uint16_t)atoi(argv.at(2).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_disconnect(dest_end_station_index, dest_desc_index);
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "acquire")
		{
			if(argv.size() == 5 && argv.at(1) == "entity")
			{
				uint16_t desc_index = 0x0;

				if((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_acquire_entity(argv.at(2), argv.at(3), desc_index);
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "lock")
		{
			if(argv.size() == 5 && argv.at(1) == "entity")
			{
				uint16_t desc_index = 0x0;

				if((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_lock_entity(argv.at(2), argv.at(3), desc_index);
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "entity")
		{
			if(argv.size() == 2 && argv.at(1) == "available")
			{
				avdecc_cmd_line_ref->cmd_entity_avail();
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "controller")
		{
			if(argv.size() == 2 && argv.at(1) == "available")
			{
				avdecc_cmd_line_ref->cmd_controller_avail();
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "read")
		{
			if(argv.size() == 4 && argv.at(1) == "descriptor")
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_read_descriptor(argv.at(2), desc_index);
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if((argv.at(0) == "set"))
		{
			if((argv.size() == 5) && (argv.at(1) == "stream_format"))
			{
				uint16_t desc_index = 0x0;
				uint64_t new_stream_format = 0x0;

				if(((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)) &&
				   ((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0)))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
					new_stream_format = (uint64_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_set_stream_format(argv.at(2), desc_index, new_stream_format);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 6) && (argv.at(1) == "stream_info"))
			{
				uint16_t desc_index = 0;
				uint64_t stream_info_field_value = 0x0;

				if(((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)) &&
				   ((argv.at(5) == "0") || (atoi(argv.at(5).c_str()) != 0)))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
					stream_info_field_value = (uint16_t)atoi(argv.at(5).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_set_stream_info(argv.at(2), desc_index, argv.at(4), stream_info_field_value);
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 6) && (argv.at(1) == "name"))
			{
				uint16_t desc_index = 0x0;
				uint16_t name_index = 0x0;

				if(((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)) &&
				   ((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0)))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
					name_index = (uint16_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					avdecc_cmd_line_ref->cmd_set_name(argv.at(2), desc_index, name_index, argv.at(5));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 5) && (argv.at(1) == "sampling_rate"))
			{
				uint16_t desc_index = 0x0;
				uint32_t new_sampling_rate = 0x0;

				if(((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)) &&
				   ((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0)))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
					new_sampling_rate = (uint32_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_set_sampling_rate(argv.at(2), desc_index, new_sampling_rate);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 5) && (argv.at(1) == "clock_source"))
			{
				uint16_t desc_index = 0x0;
				uint16_t new_clk_src_index = 0x0;

				if(((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)) &&
				   ((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0)))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
					new_clk_src_index = (uint16_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_set_clock_source(argv.at(2), desc_index, new_clk_src_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if((argv.at(0) == "get"))
		{
			if((argv.size() == 4) && (argv.at(1) == "stream_format"))
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_get_stream_format(argv.at(2), desc_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 4) && (argv.at(1) == "stream_info"))
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_get_stream_info(argv.at(2), desc_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 5) && (argv.at(1) == "name"))
			{
				uint16_t desc_index = 0x0;
				uint16_t name_index = 0x0;

				if(((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0)) &&
				   ((argv.at(4) == "0") || (atoi(argv.at(4).c_str()) != 0)))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
					name_index = (uint16_t)atoi(argv.at(4).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_get_name(argv.at(2), desc_index, name_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 4) && (argv.at(1) == "sampling_rate"))
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_get_sampling_rate(argv.at(2), desc_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else if((argv.size() == 4) && (argv.at(1) == "clock_source"))
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_get_clock_source(argv.at(2), desc_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if((argv.at(0) == "start"))
		{
			if((argv.size() == 4) && (argv.at(1) == "streaming"))
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_start_streaming(argv.at(2), desc_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}


			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if((argv.at(0) == "stop"))
		{
			if((argv.size() == 4) && (argv.at(1) == "streaming"))
			{
				uint16_t desc_index = 0x0;

				if((argv.at(3) == "0") || (atoi(argv.at(3).c_str()) != 0))
				{
					is_input_valid = true;
					desc_index = (uint16_t)atoi(argv.at(3).c_str());
				}

				if(is_input_valid)
				{
					try
					{
						avdecc_cmd_line_ref->cmd_stop_streaming(argv.at(2), desc_index);
					}
					catch(std::out_of_range &e)
					{
						std::cerr << "Out Of Range Exception " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "path")
		{
			if(argv.size() == 1)
			{
				avdecc_cmd_line_ref->cmd_path();
			}
			else if(argv.size() == 2)
			{
				avdecc_cmd_line_ref->cmd_path(argv.at(1));
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "unlog")
		{
			if(argv.size() == 1)
			{
				std::cout.rdbuf(cout_buf);
				ofstream_ref.close();
				is_output_redirected = false;
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "clr")
		{
			if(argv.size() == 1)
			{
				std::system("cls");
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else if(argv.at(0) == "q" || argv.at(0) == "quit")
		{
			if(argv.size() == 1)
			{
				while_loop = false;
			}
			else
			{
				if(argv.size() == 1)
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
				}
				else
				{
					std::cout << "Invalid Command\n" << std::endl;
					avdecc_cmd_line_ref->cmd_help_details(argv.at(0) + " " + argv.at(1));
				}
			}
		}
		else
		{
			std::cout << "Invalid Command\n" << std::endl;
			avdecc_cmd_line_ref->cmd_help_details(argv.at(0));
		}

		is_input_valid = false;
		argv.clear();
#if __MACH__
		free(input);
#endif
	}

	ofstream_ref.close();

	delete avdecc_cmd_line_ref;
	return 0;
}
