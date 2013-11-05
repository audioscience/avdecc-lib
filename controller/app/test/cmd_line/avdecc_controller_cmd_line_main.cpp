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
#include "avdecc_controller_cmd_line.h"

using namespace std;

extern "C" void notification_callback(void *user_obj, int32_t notification_type, uint64_t guid, uint16_t cmd_type,
                                      uint16_t desc_type, uint16_t desc_index, void *notification_id)
{
	avdecc_lib::util *utility = avdecc_lib::create_util();

	if(notification_type == avdecc_lib::COMMAND_TIMEOUT || notification_type == avdecc_lib::RESPONSE_RECEIVED)
	{
		printf("\n[NOTIFICATION] (%s, 0x%llx, %s, %s, %d, %d)\n",
		       utility->notification_value_to_name(notification_type),
		       guid,
		       utility->cmd_value_to_name(cmd_type),
		       utility->desc_value_to_name(desc_type),
		       desc_index,
		       notification_id);
	}
	else
	{
		printf("\n[NOTIFICATION] (%s, 0x%llx, %d, %d, %d, %d)\n",
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

	std::vector<std::string> cmd_input_vector;
	std::string cmd_input;
	size_t pos = 0;
	bool while_loop = true;
	bool is_input_valid = false;
	std::ofstream ofstream_ref;
	std::streambuf *cout_buf = std::cout.rdbuf();
	bool is_output_redirected = false;

	std::cout << "\nEnter help for a list of valid commands." << std::endl;

	while(while_loop)
	{
		printf("\n>");
		std::getline(std::cin, cmd_input);

		while((pos = cmd_input.find(" ")) != std::string::npos)
		{
			cmd_input_vector.push_back(cmd_input.substr(0, pos));
			cmd_input.erase(0, pos + 1);
		}

		if(cmd_input.compare(" ") != 0)
		{
			cmd_input_vector.push_back(cmd_input);
		}

		switch(cmd_input_vector.size())
		{
			case 1:
				if(cmd_input_vector.at(0).compare("help") == 0)
				{
					if(is_output_redirected)
					{
						std::cout.rdbuf(cout_buf);
						avdecc_cmd_line_ref->cmd_help();
						std::cout.rdbuf(ofstream_ref.rdbuf());
					}
					else
					{
						avdecc_cmd_line_ref->cmd_help();
					}
				}
				else if(cmd_input_vector.at(0).compare("version") == 0)
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
				else if(cmd_input_vector.at(0).compare("list") == 0)
				{
					avdecc_cmd_line_ref->cmd_list();
				}
				else if(cmd_input_vector.at(0).compare("select") == 0)
				{
					avdecc_cmd_line_ref->cmd_select();
				}
				else if(cmd_input_vector.at(0).compare("path") == 0)
				{
					avdecc_cmd_line_ref->cmd_path();
				}
				else if(cmd_input_vector.at(0).compare("unlog") == 0)
				{
					std::cout.rdbuf(cout_buf);
					ofstream_ref.close();
					is_output_redirected = false;
				}
				else if(cmd_input_vector.at(0).compare("clr") == 0)
				{
					std::system("cls");
				}
				else if(cmd_input_vector.at(0).compare("q") == 0 || cmd_input_vector.at(0).compare("quit") == 0)
				{
					while_loop = false;
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
				}

				break;

			case 2:
				if(cmd_input_vector.at(0).compare("view") == 0 && cmd_input_vector.at(1).compare("all") == 0)
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
				else if(cmd_input_vector.at(0).compare("list") == 0 && cmd_input_vector.at(1).compare("clock_sync_source") == 0)
				{
					avdecc_cmd_line_ref->cmd_list_clock_sync_source();
				}
				else if(cmd_input_vector.at(0).compare("entity") == 0 && cmd_input_vector.at(1).compare("available") == 0)
				{
					avdecc_cmd_line_ref->cmd_entity_avail();
				}
				else if(cmd_input_vector.at(0).compare("path") == 0)
				{
					avdecc_cmd_line_ref->cmd_path(cmd_input_vector.at(1));
				}

				else if(cmd_input_vector.at(0).compare("log") == 0)
				{
					std::string file = avdecc_cmd_line::log_path + cmd_input_vector.at(1) + ".txt";
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
				else
				{
					std::cout << "Invalid Command" << std::endl;
				}

				break;

			case 3:
				if(cmd_input_vector.at(0).compare("log") == 0 && cmd_input_vector.at(1).compare("level") == 0)
				{
					uint32_t new_log_level;

					if((cmd_input_vector.at(2).compare("0") == 0) || (atoi(cmd_input_vector.at(1).c_str()) != 0))
					{
						is_input_valid = true;
						new_log_level = (uint16_t)atoi(cmd_input_vector.at(2).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_log_level(new_log_level);
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}

				break;

			case 4:
				if(cmd_input_vector.at(0).compare("select") == 0)
				{
					uint32_t new_end_station = 0x0;
					uint16_t new_entity = 0x0;
					uint16_t new_config = 0x0;

					if(((cmd_input_vector.at(1).compare("0") == 0) || (atoi(cmd_input_vector.at(1).c_str()) != 0)) &&
					   ((cmd_input_vector.at(2).compare("0") == 0) || (atoi(cmd_input_vector.at(2).c_str()) != 0)) &&
					   ((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)))
					{
						is_input_valid = true;
						new_end_station = (uint16_t)atoi(cmd_input_vector.at(1).c_str());
						new_entity = (uint16_t)atoi(cmd_input_vector.at(2).c_str());
						new_config = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_select(new_end_station, new_entity, new_config);
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("view") == 0 && cmd_input_vector.at(1).compare("descriptor") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_view_descriptor(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
							return -1;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("read") == 0 && cmd_input_vector.at(1).compare("descriptor") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_read_descriptor(cmd_input_vector.at(2), desc_index);
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("get") == 0 && cmd_input_vector.at(1).compare("stream_format") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_get_stream_format(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("get") == 0 && cmd_input_vector.at(1).compare("stream_info") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_get_stream_info(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("get") == 0 && cmd_input_vector.at(1).compare("sampling_rate") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_get_sampling_rate(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
				}
				else if(cmd_input_vector.at(0).compare("get") == 0 && cmd_input_vector.at(1).compare("clock_source") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_get_clock_source(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("start") == 0 && cmd_input_vector.at(1).compare("streaming") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_start_streaming(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("stop") == 0 && cmd_input_vector.at(1).compare("streaming") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_stop_streaming(cmd_input_vector.at(2), desc_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
				}

				break;

			case 5:
				if(cmd_input_vector.at(0).compare("acquire") == 0 && cmd_input_vector.at(1).compare("entity") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_acquire_entity(cmd_input_vector.at(2), cmd_input_vector.at(3), desc_index);
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("lock") == 0 && cmd_input_vector.at(1).compare("entity") == 0)
				{
					uint16_t desc_index = 0x0;

					if((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_lock_entity(cmd_input_vector.at(2), cmd_input_vector.at(3), desc_index);
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("set") == 0 && cmd_input_vector.at(1).compare("stream_format") == 0)
				{
					uint16_t desc_index = 0x0;
					uint64_t new_stream_format = 0x0;

					if(((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)) &&
					   ((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0)))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
						new_stream_format = (uint64_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_set_stream_format(cmd_input_vector.at(2), desc_index, new_stream_format);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("get") == 0 && cmd_input_vector.at(1).compare("name") == 0)
				{
					uint16_t desc_index = 0x0;
					uint16_t name_index = 0x0;

					if(((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)) &&
					   ((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0)))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
						name_index = (uint16_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_get_name(cmd_input_vector.at(2), desc_index, name_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("set") == 0 && cmd_input_vector.at(1).compare("sampling_rate") == 0)
				{
					uint16_t desc_index = 0x0;
					uint32_t new_sampling_rate = 0x0;

					if(((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)) &&
					   ((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0)))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
						new_sampling_rate = (uint32_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_set_sampling_rate(cmd_input_vector.at(2), desc_index, new_sampling_rate);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("set") == 0 && cmd_input_vector.at(1).compare("clock_source") == 0)
				{
					uint16_t desc_index = 0x0;
					uint16_t new_clk_src_index = 0x0;

					if(((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)) &&
					   ((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0)))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
						new_clk_src_index = (uint16_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						try
						{
							avdecc_cmd_line_ref->cmd_set_clock_source(cmd_input_vector.at(2), desc_index, new_clk_src_index);
						}
						catch(std::out_of_range &e)
						{
							std::cerr << "Out Of Range Exception " << e.what() << std::endl;
						}
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
				}

				break;

			case 6:
				if(cmd_input_vector.at(0).compare("set") == 0 && cmd_input_vector.at(1).compare("stream_info") == 0)
				{
					uint16_t desc_index = 0;
					uint64_t stream_info_field_value = 0x0;

					if(((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)) &&
					   ((cmd_input_vector.at(5).compare("0") == 0) || (atoi(cmd_input_vector.at(5).c_str()) != 0)))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
						stream_info_field_value = (uint16_t)atoi(cmd_input_vector.at(5).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_set_stream_info(cmd_input_vector.at(2), desc_index, cmd_input_vector.at(4), stream_info_field_value);
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else if(cmd_input_vector.at(0).compare("set") == 0 && cmd_input_vector.at(1).compare("name") == 0)
				{
					uint16_t desc_index = 0x0;
					uint16_t name_index = 0x0;

					if(((cmd_input_vector.at(3).compare("0") == 0) || (atoi(cmd_input_vector.at(3).c_str()) != 0)) &&
					   ((cmd_input_vector.at(4).compare("0") == 0) || (atoi(cmd_input_vector.at(4).c_str()) != 0)))
					{
						is_input_valid = true;
						desc_index = (uint16_t)atoi(cmd_input_vector.at(3).c_str());
						name_index = (uint16_t)atoi(cmd_input_vector.at(4).c_str());
					}

					if(is_input_valid)
					{
						avdecc_cmd_line_ref->cmd_set_name(cmd_input_vector.at(2), desc_index, name_index, cmd_input_vector.at(5));
					}
					else
					{
						std::cout << "Invalid Command" << std::endl;
					}
				}
				else
				{
					std::cout << "Invalid Command" << std::endl;
				}

				break;

			default:
				{
					std::cout << "Invalid Command" << std::endl;
				}
		}

		is_input_valid = false;
		cmd_input_vector.clear();
	}

	ofstream_ref.close();

	delete avdecc_cmd_line_ref;
	return 0;
}
