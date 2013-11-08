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
 * cmd_line_help.h
 *
 * AVDECC Controller command line help class
 */

#pragma once
#ifndef _AVDECC_CMD_LINE_HELP_H_
#define _AVDECC_CMD_LINE_HELP_H_

class cmd_line_help
{
public:
	//std::vector<std::string> details;
	std::string command;
	std::string description;
	std::string detail;

	/**
	 * An empty constructor for cmd_line_help
	 */
	cmd_line_help();

	/**
	 * Constructor for cmd_line_help used for constructing an object with a command.
	 */
	cmd_line_help(const std::string cmd, const std::string description, const std::string details);

	/**
	 * Destructor for cmd_line_help used for destroying objects
	 */
	~cmd_line_help();

	const std::string get_command();

	const std::string get_description();

	const std::string get_details();

//vector
//	v->cmd() space v->summary()
//
//
//vector
//	if ( str in v->cmd)
//		print details vector
//
//
//print help filename













};

#endif
