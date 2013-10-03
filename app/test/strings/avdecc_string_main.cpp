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
 * avdecc_string_main.cpp
 *
 * AVDECC string main implementation used for testing AVDECC Lib string conversion functions.
 *
 * (c) AudioScience, Inc. 2013
 */

#include <iostream>
#include "aem_string.h"
using namespace std;

int main()
{
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(0) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(1) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(2) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(3) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(4) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(5) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(74) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(75) << std::endl;
	std::cout << avdecc_lib::aem_string::cmd_value_to_name(76) << std::endl;

	std::cout << "\n0x" << std::hex << avdecc_lib::aem_string::cmd_name_to_value("read_descriptor") << std::endl;
	std::cout << "0x" << std::hex << avdecc_lib::aem_string::cmd_name_to_value("acquire_entity") << std::endl;
	std::cout << "0x" << std::hex << avdecc_lib::aem_string::cmd_name_to_value("stream_info") << std::endl;
	std::cout << "0x" << std::hex << avdecc_lib::aem_string::cmd_name_to_value("get_stream_info") << std::endl;

	std::cout << "\n\n" << avdecc_lib::aem_string::desc_value_to_name(0) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(1) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(2) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(3) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(4) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(5) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(37) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(38) << std::endl;
	std::cout << avdecc_lib::aem_string::desc_value_to_name(39) << std::endl;

	std::cout << "\n0x" << std::hex << avdecc_lib::aem_string::desc_name_to_value("entity") << std::endl;
	std::cout << "0x" << std::hex << avdecc_lib::aem_string::desc_name_to_value("configuration") << std::endl;
	std::cout << "0x" << std::hex << avdecc_lib::aem_string::desc_name_to_value("stream_in") << std::endl;
	std::cout << "0x" << std::hex << avdecc_lib::aem_string::desc_name_to_value("stream_INPUT") << std::endl;

	return 0;
}