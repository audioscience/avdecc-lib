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
 * util.h
 *
 * Utility implementation class
 */

#pragma once

#include "avdecc-lib_build.h"
#include <stdint.h>

namespace avdecc_lib
{
namespace utility
{
    ///
    /// Convert command value to its corresponding AEM command name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL aem_cmd_value_to_name(uint16_t cmd_value);

    ///
    /// Convert AEM command name to its corresponding command value.
    ///
    AVDECC_CONTROLLER_LIB32_API uint16_t STDCALL aem_cmd_name_to_value(const char * cmd_name);

    ///
    /// Convert descriptor value to its corresponding descriptor name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL aem_desc_value_to_name(uint16_t desc_value);

    ///
    /// Convert descriptor name to its corresponding descriptor value.
    ///
    AVDECC_CONTROLLER_LIB32_API uint16_t STDCALL aem_desc_name_to_value(const char * desc_name);

    ///
    /// Convert AEM command status value to its corresponding command status name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL aem_cmd_status_value_to_name(uint32_t aem_cmd_status_value);

    ///
    /// Convert command value to its corresponding ACMP command name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL acmp_cmd_value_to_name(uint32_t cmd_value);

    ///
    /// Convert ACMP command name to its corresponding command value.
    ///
    AVDECC_CONTROLLER_LIB32_API uint16_t STDCALL acmp_cmd_name_to_value(const char * cmd_name);

    ///
    /// Convert ACMP command status value to its corresponding command status name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL acmp_cmd_status_value_to_name(uint32_t acmp_cmd_status_value);

    ///
    /// Convert notification value to its corresponding notification name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL notification_value_to_name(uint16_t notification_value);
    
    ///
    /// Convert ACMP notification value to its corresponding ACMP notification name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL acmp_notification_value_to_name(uint16_t acmp_notification_value);

    ///
    /// Convert post_log_msg value to its corresponding post_log_msg name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL logging_level_value_to_name(uint16_t logging_level_value);

    ///
    /// Get the timeout for the corresponding ACMP command.
    ///
    AVDECC_CONTROLLER_LIB32_API uint32_t STDCALL acmp_cmd_to_timeout(const uint32_t acmp_cmd);

    ///
    /// Convert IEEE1722 format name to value.
    ///
    AVDECC_CONTROLLER_LIB32_API uint64_t STDCALL ieee1722_format_name_to_value(const char * format_name);

    ///
    /// Convert IEEE1722 format name to description.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL ieee1722_format_name_to_description(const char * format_name);

    ///
    ///  Return IEEE1722 format value by index
    ///
    AVDECC_CONTROLLER_LIB32_API uint64_t STDCALL ieee1722_format_index_to_value(unsigned int index);

    ///
    ///  Return IEEE1722 format name by index
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL ieee1722_format_index_to_name(unsigned int index);

    ///
    ///  Return IEEE1722 format description by index
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL ieee1722_format_index_to_description(unsigned int index);

    ///
    /// Convert IEEE1722 format value to name.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL ieee1722_format_value_to_name(uint64_t format_value);

    ///
    /// Get IEEE1722 format table size
    ///
    AVDECC_CONTROLLER_LIB32_API unsigned int STDCALL get_ieee1722_format_table_size();

    ///
    /// Convert End Station MAC address to a short string.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * STDCALL end_station_mac_to_string(uint64_t end_station_mac);

    ///
    /// Convert an uint64_t value to eui48.
    ///
    AVDECC_CONTROLLER_LIB32_API void convert_uint64_to_eui48(const uint64_t value, uint8_t new_value[6]);

    ///
    /// Convert an eui48 value to uint64_t.
    ///
    AVDECC_CONTROLLER_LIB32_API void convert_eui48_to_uint64(const uint8_t value[6], uint64_t & new_value);

    /* 6 byte mac address in network byte order */
    class MacAddr
    {
    public:
        unsigned char byte1;
        unsigned char byte2;
        unsigned char byte3;
        unsigned char byte4;
        unsigned char byte5;
        unsigned char byte6;
        static const int strlen = 18;

        MacAddr(void) : byte1(0), byte2(0), byte3(0), byte4(0), byte5(0), byte6(0) {}
        MacAddr(unsigned char a, unsigned char b,
                unsigned char c, unsigned char d,
                unsigned char e, unsigned char f) : byte1(a), byte2(b), byte3(c), byte4(d), byte5(e), byte6(f) {}
        MacAddr(uint64_t mac_val) : byte1((mac_val >> 40) & 0XFF),
                                    byte2((mac_val >> 32) & 0xFF),
                                    byte3((mac_val >> 24) & 0xFF),
                                    byte4((mac_val >> 16) & 0xFF),
                                    byte5((mac_val >> 8) & 0XFF),
                                    byte6((mac_val & 0XFF)) {}
        MacAddr(const char * p) { fromstring(p); }

        uint64_t tovalue();
        void tostring(char * p, char d = ':') const;
        bool fromstring(const char * p);

        bool operator==(const MacAddr & a) const
        {
            return byte1 == a.byte1 && byte2 == a.byte2 &&
                   byte3 == a.byte3 && byte4 == a.byte4 &&
                   byte5 == a.byte5 && byte6 == a.byte6;
        }
        bool operator!=(const MacAddr & a) const
        {
            return !operator==(a);
        }
        unsigned char & operator[](int a)
        {
            switch (a)
            {
            case 0:
                return byte1;
            case 1:
                return byte2;
            case 2:
                return byte3;
            case 3:
                return byte4;
            case 4:
                return byte5;
            }
            return byte6;
        }
        unsigned char getByte(int a) const
        {
            switch (a)
            {
            case 0:
                return byte1;
            case 1:
                return byte2;
            case 2:
                return byte3;
            case 3:
                return byte4;
            case 4:
                return byte5;
            }
            return byte6;
        }
    };
    inline bool MacAddr::fromstring(const char * p)
    {
        // must be of the form "n:n:n:n:n:n" where 0<=n<=FF in hexidecimal
        int i = 0; // index of byte currently being scanned
        int j = 0; // count of characters scanned for current byte
        int n = 0; // value of current byte;
        bool valid = true;
        while (i < 6)
        {
            if (*p >= '0' && *p <= '9')
            {
                n = 16 * n + *p - '0';
                j++;
            }
            else if (*p >= 'a' && *p <= 'f')
            {
                n = 16 * n + 10 + *p - 'a';
                j++;
            }
            else if (*p >= 'A' && *p <= 'F')
            {
                n = 16 * n + 10 + *p - 'A';
                j++;
            }
            else
            {
                if (!j)
                {
                    // need at least one hex char per byte
                    valid = false;
                }
                if (n >= 0 && n <= 255)
                {
                    operator[](i++) = n;
                    n = 0;
                    j = 0;
                }
                else
                {
                    break;
                }
                if (*p != ':' && *p != '.')
                    break;
            }
            p++;
        };
        if (i < 6)
            valid = false;
        return valid;
    }
    inline uint64_t MacAddr::tovalue()
    {
        uint64_t mac_val = 0;
        for (int i = 0; i < 6; i++)
            mac_val = uint64_t(getByte(i)) << (40 - (8 * i)) | mac_val;
        
        return mac_val;
    }
    inline void MacAddr::tostring(char * p, char d) const
    {
        char digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        int i = 0;
        int n;

        for (i = 0; i < 6; i++)
        {
            n = getByte(i);
            *p = digits[n / 16];
            p++;
            *p = digits[n % 16];
            p++;
            if (i < 5)
                *p = d;
            else
                *p = 0;
            p++;
        }
    }
}
}
