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
 * net_interface_imp.cpp
 *
 * Network interface implementation class
 */

#include <winsock2.h>
#include <iphlpapi.h>
#include "util.h"
#include "enumeration.h"
#include "log_imp.h"
#include "jdksavdecc_pdu.h"
#include "net_interface_imp.h"

namespace avdecc_lib
{
    net_interface * STDCALL create_net_interface()
    {
        return (new net_interface_imp());
    }

    net_interface_imp::net_interface_imp()
    {
        if(pcap_findalldevs(&all_devs, err_buf) == -1) // Retrieve the device list on the local machine.
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "pcap_findalldevs error %s", err_buf);
            exit(EXIT_FAILURE);
        }

        for(dev = all_devs, total_devs = 0; dev; dev = dev->next)
        {
            total_devs++;
        }

        if(total_devs == 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "No interfaces found! Make sure WinPcap is installed.");
            exit(EXIT_FAILURE);
        }
    }

    net_interface_imp::~net_interface_imp()
    {
        pcap_freealldevs(all_devs); // Free the device list
        pcap_close(pcap_interface);
    }

    void STDCALL net_interface_imp::destroy()
    {
        delete this;
    }

    uint32_t STDCALL net_interface_imp::devs_count()
    {
        return total_devs;
    }

    uint64_t net_interface_imp::mac_addr()
    {
        return mac;
    }

    char * STDCALL net_interface_imp::get_dev_desc_by_index(size_t dev_index)
    {
        uint32_t i;

        for(dev = all_devs, i = 0; (i < dev_index) && (dev_index < total_devs); dev = dev->next, i++); // Get the selected interface

        if(!dev->description)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Interface description is blank.");
        }

        return dev->description;
    }


    int STDCALL net_interface_imp::select_interface_by_num(uint32_t interface_num)
    {
        uint32_t index;
        IP_ADAPTER_INFO *AdapterInfo;
        IP_ADAPTER_INFO *Current;
        ULONG AIS;
        DWORD status;
        int timeout_ms = NETIF_READ_TIMEOUT_MS;

        if(interface_num < 1 || interface_num > total_devs)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Interface number out of range.");
            pcap_freealldevs(all_devs); // Free the device list
            exit(EXIT_FAILURE);
        }

        for(dev = all_devs, index = 0; index < interface_num - 1; dev = dev->next, index++); // Jump to the selected adapter

        /************************************************************** Open the device ****************************************************************/
        if((pcap_interface = pcap_open_live(dev->name,		       // Name of the device
                                            65536,		       // Portion of the packet to capture
                                            // 65536 guarantees that the whole packet will be captured on all the link layers
                                            PCAP_OPENFLAG_PROMISCUOUS, // In promiscuous mode, all packets including packets of other hosts are captured
                                            timeout_ms,		       // Read timeout in ms
                                            err_buf		       // Error buffer
                                           )) == NULL)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Unable to open the adapter. %s is not supported by WinPcap.", dev->name);
            pcap_freealldevs(all_devs); // Free the device list
            exit(EXIT_FAILURE);
        }

        /****************************** Lookup IP address ***************************/
        AdapterInfo = (IP_ADAPTER_INFO *)calloc(total_devs, sizeof(IP_ADAPTER_INFO));
        AIS = sizeof(IP_ADAPTER_INFO) * total_devs;

        if(GetAdaptersInfo(AdapterInfo, &AIS) == ERROR_BUFFER_OVERFLOW)
        {
            free(AdapterInfo);
            AdapterInfo = (IP_ADAPTER_INFO *)calloc(1, AIS);

            if(AdapterInfo == NULL)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Allocating memory needed to call GetAdaptersinfo.", dev->name);
                exit(EXIT_FAILURE);
            }

            status = GetAdaptersInfo(AdapterInfo, &AIS);

            if(status != ERROR_SUCCESS)
            {
                log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "GetAdaptersInfo call in netif_win32_pcap.c failed.", dev->name);
                free(AdapterInfo);
                exit(EXIT_FAILURE);
            }
        }

        for(Current = AdapterInfo; Current != NULL; Current = Current->Next)
        {
            if(strstr(dev->name, Current->AdapterName) != 0)
            {
                uint32_t my_ip;
                ULONG len;
                uint8_t tmp[16];

                my_ip = inet_addr(Current->IpAddressList.IpAddress.String);
                len = sizeof(tmp);
                SendARP(my_ip ,INADDR_ANY, tmp, &len);
                utility::convert_eui48_to_uint64(&tmp[0], mac);
            }
        }

        uint16_t ether_type[1];
        ether_type[0] = JDKSAVDECC_AVTP_ETHERTYPE;
        set_capture_ether_type(ether_type, 0); // Set the filter

        free(AdapterInfo);
        return 0;
    }

    int net_interface_imp::set_capture_ether_type(uint16_t *ether_type, uint32_t count)
    {
        struct bpf_program fcode;
        char ether_type_string[512];
        char ether_type_single[64];
        const unsigned char *ether_packet = NULL;
        struct pcap_pkthdr pcap_header;

        ether_type_string[0] = 0;

        for(uint32_t i = 0; i < count; i++)
        {
            sprintf(ether_type_single, "ether proto 0x%04x", ether_type[i]);
            strcat(ether_type_string, ether_type_single);

            if((i + 1) < count)
            {
                strcat(ether_type_string, " or ");
            }
        }

        /************************************** Compile a filter **************************************/
        if(pcap_compile(pcap_interface, &fcode, ether_type_string, 1, 0) < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Unable to compile the packet filter.");
            pcap_freealldevs(all_devs); // Free the device list
            return -1;
        }

        /********************************** Set the filter *********************************/
        if(pcap_setfilter(pcap_interface, &fcode) < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Error setting the filter.");
            pcap_freealldevs(all_devs);  // Free the device list
            return -1;
        }

        /*********** Flush any packets that might be present **********/
        for(uint32_t index_j = 0; index_j < 1; index_j++)
        {
            ether_packet = pcap_next(pcap_interface, &pcap_header);
        }

        return 0;
    }

    int STDCALL net_interface_imp::capture_frame(const uint8_t **frame, uint16_t *frame_len)
    {
        struct pcap_pkthdr *header;
        int error = 0;

        *frame_len = 0;
        error = pcap_next_ex(pcap_interface, &header, frame);

        if(error > 0 )
        {
            cmd_frame = *frame;
            *frame_len = (uint16_t)header->len;
            return 1;
        }

        return -2; // Timeout
    }

    int net_interface_imp::send_frame(uint8_t *frame, size_t frame_len)
    {
        if(pcap_sendpacket(pcap_interface, frame, (int)frame_len) != 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "pcap_sendpacket error %s", pcap_geterr(pcap_interface));
            return -1;
        }

        return 0;
    }
}
