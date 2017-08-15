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
#include <string>

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

    ///
    /// Convert IEEE1722 stream format name to value.
    ///
    /// \return 0 if the name is not recognized.
    ///
    AVDECC_CONTROLLER_LIB32_API uint64_t ieee1722_format_name_to_value(const char * format_name);
    
    ///
    /// Convert IEEE1722 stream format value to name.
    ///
    /// \return 'UNKNOWN' if the value cannot be decoded.
    ///
    AVDECC_CONTROLLER_LIB32_API const char * ieee1722_format_value_to_name(uint64_t format_value);

    ///
    /// Convert IEEE1722 stream format value to subtype.
    ///
    /// \return 0 if the value is not recognized.
    ///
    AVDECC_CONTROLLER_LIB32_API unsigned int ieee1722_format_value_to_subtype(uint64_t format_value);

    ///
    /// Convert IEEE1722 stream format value to sample rate.
    ///
    /// \return 0 if the value is not recognized or if sample rate is not an applicable field of the format.
    ///
    AVDECC_CONTROLLER_LIB32_API unsigned int ieee1722_format_value_to_sample_rate(uint64_t format_value);

    ///
    /// Convert IEEE1722 stream format value to channel count.
    ///
    /// \return 0 if the value is not recognized or if channel count is not an applicable field of the format.
    ///
    AVDECC_CONTROLLER_LIB32_API unsigned int ieee1722_format_value_to_channel_count(uint64_t format_value);
    
    class ieee1722_stream_format
    {
    public:
        AVDECC_CONTROLLER_LIB32_API ieee1722_stream_format(uint64_t format_value);
        AVDECC_CONTROLLER_LIB32_API ieee1722_stream_format(const char * format_str);
        
        std::string name() { return m_format_name; }
        uint64_t value() { return m_format_value; }
        
        unsigned int version() { return m_version; }
        unsigned int subtype() { return m_subtype; }
        
        enum ieee1722_stream_subtypes
        {
            IIDC_61883 = 0,
            MMA_STREAM = 1,
            AAF = 2,
            CVF = 3,
            CRF = 4
        };
        
    protected:
        uint64_t m_format_value = 0;
        std::string m_format_name = "UNKNOWN";
        
        // 1722 Stream format common
        unsigned int m_version = 0;
        unsigned int m_subtype = 0;
        
        bool subtype_from_str(std::string subtype);
    };
    
    class crf_format : public ieee1722_stream_format
    {
    public:
        AVDECC_CONTROLLER_LIB32_API crf_format(uint64_t format_value);
        AVDECC_CONTROLLER_LIB32_API crf_format(const char * format_name) : ieee1722_stream_format(format_name) { to_val(); }
        
        unsigned int type() { return m_type; }
        unsigned int timestamp_interval() { return m_timestamp_interval; }
        unsigned int timestamps_per_pdu() { return m_timestamps_per_pdu; }
        unsigned int pull_value() { return m_pull_value; }
        unsigned int base_frequency() { return m_base_frequency; }
        
        enum Crf_types
        {
            CRF_USER = 0,
            CRF_AUDIO_SAMPLE = 1,
            CRF_VIDEO_FRAME = 2,
            CRF_VIDEO_LINE = 3,
            CRF_MACHINE_CYCLE = 4
        };
        
        enum Crf_pull_values
        {
            MULTIPLY_1_0 = 0,
            MULTIPLY_1_1_001 = 1,
            MULTIPLY_1_001 = 2,
            MULTIPLY_24_25 = 3,
            MULTIPLY_25_24 = 4,
            MULTIPLY_1_8 = 5
        };

    private:
        // CRF common
        unsigned int m_type = 0;
        unsigned int m_timestamp_interval = 0;
        unsigned int m_timestamps_per_pdu = 0;
        unsigned int m_pull_value = 0;
        unsigned int m_base_frequency = 0;
        
        void to_string();
        void to_val();
        
        bool crf_type_from_str(std::string type);
        bool crf_timestamp_interval_from_str(std::string timestamp_interval);
        bool crf_timestamps_per_pdu_from_str(std::string timestamps_per_pdu);
        bool crf_pull_value_from_str(std::string pull);
        bool crf_base_frequency_from_str(std::string base_frequency);
    };
    
    class aaf_format : public ieee1722_stream_format
    {
    public:
        AVDECC_CONTROLLER_LIB32_API aaf_format(uint64_t format_value);
        AVDECC_CONTROLLER_LIB32_API aaf_format(const char * format_name) : ieee1722_stream_format(format_name) { to_val(); }
        
        AVDECC_CONTROLLER_LIB32_API unsigned int sample_rate();
        unsigned int channel_count() { return m_channels_per_frame; }
        unsigned int upto() { return m_upto; }
        unsigned int nsr_value() { return m_nsr_value; }
        unsigned int packetization_type() { return m_packetization_type; }
        unsigned int bit_depth() { return m_bit_depth; }
        unsigned int channels_per_frame() { return m_channels_per_frame; }
        unsigned int samples_per_frame() { return m_samples_per_frame; }

        enum Aaf_nsr_values
        {
            NSR_8KHZ = 0x01,
            NSR_16KHZ = 0x02,
            NSR_32KHZ = 0x03,
            NSR_44_1KHZ = 0x04,
            NSR_48KHZ = 0x05,
            NSR_88_2KHZ = 0x06,
            NSR_96KHZ = 0x07,
            NSR_176_4KHZ = 0x08,
            NSR_192KHZ = 0x09,
            NSR_24KHZ = 0x0a
        };
        
        enum Aaf_packetization_types
        {
            FLOAT_32BIT = 1,
            INT_32BIT = 2,
            INT_24BIT = 3,
            INT_16BIT = 4,
            AES3_32BIT = 5
        };

    private:
        // AAF common
        unsigned int m_upto = 0;
        unsigned int m_nsr_value = 0;
        unsigned int m_packetization_type = 0;

        // AAF PCM
        unsigned int m_bit_depth = 0;
        unsigned int m_channels_per_frame = 0;
        unsigned int m_samples_per_frame = 0;
        
        void to_string();
        void to_val();
        
        void decode_aaf_pcm_fields();
        
        bool aaf_packetization_type_from_str(std::string packetization_type);
        bool aaf_nsr_from_str(std::string nsr);
        bool aaf_bit_depth_from_str(std::string bit_depth);
        bool aaf_channels_per_frame_from_str(std::string channels);
        bool aaf_samples_per_frame_from_str(std::string samples);
    };

    class iec_61883_iidc_format : public ieee1722_stream_format
    {
    public:
        AVDECC_CONTROLLER_LIB32_API iec_61883_iidc_format(uint64_t format_value);
        AVDECC_CONTROLLER_LIB32_API iec_61883_iidc_format(const char * format_name) : ieee1722_stream_format(format_name) { to_val(); }

        AVDECC_CONTROLLER_LIB32_API unsigned int sample_rate();
        unsigned int channel_count() { return m_dbs; }
        unsigned int sf() { return m_sf; }
        unsigned int iec61883_type() { return m_iec61883_type; }
        unsigned int packetization_type_value() { return m_packetization_type_value; }
        unsigned int fdf_sfc_value() { return m_fdf_sfc_value; }
        unsigned int dbs() { return m_dbs; }
        unsigned int blocking() { return m_blocking; }
        unsigned int nonblocking() { return m_nonblocking; }
        unsigned int upto() { return m_upto; }
        unsigned int synchronous() { return m_synchronous; }
        unsigned int iec60958_count() { return m_iec60958_count; }
        unsigned int mbla_count() { return m_mbla_count; }
        unsigned int midi_count() { return m_midi_count; }
        unsigned int smpte_count() { return m_smpte_count; }
        
        enum Iec61883_types
        {
            IEC_61883_4 = 0x20,
            IEC_61883_6 = 0x10,
            IEC_61883_7 = 0x21,
            IEC_61883_8 = 0x01
        };
        
        enum Iec61883_packetization_types
        {
            FIXED_32BIT = 6,
            FLOAT_32BIT = 4,
            AM824 = 0
        };
        
        enum Iec61883_fdf_sfc_values
        {
            FDF_SFC_44_1KHZ = 1,
            FDF_SFC_48KHZ = 2,
            FDF_SFC_88_2KHZ = 3,
            FDF_SFC_96KHZ = 4,
            FDF_SFC_176_4KHZ = 5,
            FDF_SFC_192KHZ = 6
        };
        
    private:
        // IEC 61883-6 common
        unsigned int m_sf = 0;
        unsigned int m_iec61883_type = 0;
        unsigned int m_packetization_type_value = 0;
        unsigned int m_fdf_sfc_value = 0;
        unsigned int m_dbs = 0;
        unsigned int m_blocking = 0;
        unsigned int m_nonblocking = 0;
        unsigned int m_upto = 0;
        unsigned int m_synchronous = 0;
        
        // IEC 61883-6 AM824
        unsigned int m_iec60958_count = 0; // count of iec60958 quadlets
        unsigned int m_mbla_count = 0;     // count of multi-bit linear audio quadlets
        unsigned int m_midi_count = 0;     // count of MIDI quadlets
        unsigned int m_smpte_count = 0;    // count of SMPTE quadlets

        void to_string();
        void to_val();
        
        void decode_iec_61883_type();
        void decode_iidc_format() { return; }
        void decode_iec_61883_common();
        void decode_iec_61883_packetization_type();
        void decode_iec_61883_am824_fields();
        
        bool iec_61883_type_from_str(std::string type);
        bool iec_61883_packetization_type_from_str(std::string packetization_type);
        bool iec_61883_sfc_from_str(std::string sfc);
        bool iec_61883_dbs_from_str(std::string dbs);
    };
    
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
