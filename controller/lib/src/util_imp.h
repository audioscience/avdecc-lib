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
 * util_imp.h
 *
 * Utility implementation class
 */

#pragma once

#include "util.h"

namespace avdecc_lib
{
    class util_imp : public virtual util
    {
    public:
        util_imp();

        virtual ~util_imp();

        /**
         * Call destructor for Controller used for destroying objects
         */
        void STDCALL destroy();

        /**
         * Convert command value to its corresponding AEM command name.
         */
        const char * STDCALL aem_cmd_value_to_name(uint16_t cmd_value);

        /**
         * Convert AEM command name to its corresponding command value.
         */
        uint16_t STDCALL aem_cmd_name_to_value(const char *cmd_name);

        /**
         * Convert descriptor value to its corresponding descriptor name.
         */
        const char * STDCALL aem_desc_value_to_name(uint16_t desc_value);

        /**
         * Convert descriptor name to its corresponding descriptor value.
         */
        uint16_t STDCALL aem_desc_name_to_value(const char *desc_name);

        /**
         * Convert AEM command status value to its corresponding command status name.
         */
        const char * STDCALL aem_cmd_status_value_to_name(uint32_t aem_cmd_status_value);

        /**
         * Convert command value to its corresponding ACMP command name.
         */
        const char * STDCALL acmp_cmd_value_to_name(uint16_t cmd_value);

        /**
         * Convert ACMP command name to its corresponding command value.
         */
        uint16_t STDCALL acmp_cmd_name_to_value(const char *cmd_name);

        /**
         * Convert ACMP command status value to its corresponding command status name.
         */
        const char * STDCALL acmp_cmd_status_value_to_name(uint32_t acmp_cmd_status_value);

        /**
         * Convert notification value to its corresponding notification name.
         */
        const char * STDCALL notification_value_to_name(uint16_t notification_value);

        /**
         * Convert post_log_msg value to its corresponding post_log_msg name.
         */
        const char * STDCALL logging_level_value_to_name(uint16_t logging_level_value);

        /**
         * Get the timeout for the corresponding ACMP command.
         */
        uint32_t STDCALL acmp_cmd_to_timeout(const uint32_t acmp_cmd);

        /**
          * Convert IEEE1722 format name to value.
          */
        uint64_t STDCALL ieee1722_format_name_to_value(const char *format_name);

        /**
         * Convert IEEE1722 format value to name.
         */
        const char * STDCALL ieee1722_format_value_to_name(uint64_t format_value);

        /**
         * Convert End Station MAC address to a short string.
         */
        const char * STDCALL end_station_mac_to_string(uint64_t end_station_mac);

        /**
         * Convert an uint64_t value to eui48.
         */
        void convert_uint64_to_eui48(const uint64_t value, uint8_t new_value[6]);

        /**
         * Convert an eui48 value to uint64_t.
         */
        void convert_eui48_to_uint64(const uint8_t value[6], uint64_t &new_value);
    };

    extern util_imp *utility;
}

