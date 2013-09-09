/**
 * avdecc_string.h
 *
 * Header of the avdecc_string class, which is used as a shared library to access common string functions.
 *
 * (c) AudioScience, Inc. 2013
 */

#pragma once
#ifndef _AVDECC_STRING_H_
#define _AVDECC_STRING_H_

#include <stdint.h>
#include "avdecc_enum.h"
#include "avdecc_build.h"

#define STDCALL __stdcall

namespace asi_avb_avdecc_string
{
	class avdecc_string
	{
	public:
		/**
		 * Convert command value to its corresponding command name.
		 */
		static AVDECC_LIB_API const char * STDCALL aem_cmd_value_to_name(uint16_t cmd_value);

		/**
		 * Convert command name to its corresponding command value.
		 */
		static AVDECC_LIB_API uint16_t STDCALL aem_cmd_name_to_value(const char *cmd_name);

		/**
		 * Convert descriptor value to its corresponding descriptor name.
		 */
		static AVDECC_LIB_API const char * STDCALL aem_desc_value_to_name(uint16_t desc_value);

		/**
		 * Convert descriptor name to its corresponding descriptor value.
		 */
		static AVDECC_LIB_API uint16_t STDCALL aem_desc_name_to_value(const char *desc_name);

		/**
		 * Get the corresponding string notification representation based on the integer value.
		 */
		static AVDECC_LIB_API const char * STDCALL avdecc_lib_notification_value_to_name(int int_value);
	 };
}

#endif