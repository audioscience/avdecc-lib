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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <net/bpf.h>

#include <algorithm>

#include "util.h"
#include "enumeration.h"
#include "log_imp.h"
#include "jdksavdecc_pdu.h"
#include "net_interface_imp.h"
#include "mac_native_interface_bridge.h"

namespace avdecc_lib
{
net_interface * STDCALL create_net_interface()
{
    return (new net_interface_imp());
}

net_interface_imp::net_interface_imp()
{
    if (pcap_findalldevs(&all_devs, err_buf) == -1) // Retrieve the device list on the local machine.
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "pcap_findalldevs error %s", err_buf);
    }

    for (dev = all_devs, total_devs = 0; dev; dev = dev->next)
    {
        // store the valid IPv4 addresses associated with the device
        std::vector<std::string> device_ip_addresses;
        pcap_addr_t * dev_addr;
        for (dev_addr = dev->addresses; dev_addr != NULL; dev_addr = dev_addr->next)
        {
            if (dev_addr->addr->sa_family == AF_INET && dev_addr->addr)
            {
                char ip_str[INET_ADDRSTRLEN] = {0};
                inet_ntop(AF_INET, &((struct sockaddr_in *)dev_addr->addr)->sin_addr, ip_str, INET_ADDRSTRLEN);
                device_ip_addresses.push_back(std::string(ip_str));
            }
        }
        
        all_ip_addresses.push_back(device_ip_addresses);
        
        // store the MAC addr associated with device
        find_and_store_device_mac_addr(dev->name);
        
        total_devs++;
    }

    if (total_devs == 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "No interfaces found! Make sure WinPcap is installed.");
    }
}

net_interface_imp::~net_interface_imp()
{
    if (is_netif_pcap)
    {
        pcap_freealldevs(all_devs); // Free the device list
        pcap_close(pcap_interface);
    }
}
    
void net_interface_imp::find_and_store_device_mac_addr(char * dev_name)
{
    uint64_t mac;
    int mib[6];
    size_t len;
    char * buf;
    unsigned char * ptr;
    struct if_msghdr * ifm;
    struct sockaddr_dl * sdl;
    
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_LINK;
    mib[4] = NET_RT_IFLIST;
    
    if ((mib[5] = if_nametoindex(dev_name)) == 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "if_nametoindex error");
        return;
    }
    
    if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "sysctl 1 error");
        return;
    }
    
    if ((buf = (char *)malloc(len)) == NULL)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "malloc error");
        return;
    }
    
    if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "sysctl 2 error");
        return;
    }
    
    ifm = (struct if_msghdr *)buf;
    sdl = (struct sockaddr_dl *)(ifm + 1);
    ptr = (unsigned char *)LLADDR(sdl);
    utility::convert_eui48_to_uint64(ptr, mac);

    all_mac_addresses.push_back(mac);
    free(buf);
}

void STDCALL net_interface_imp::destroy()
{
    if (!is_netif_pcap)
        mac_native_interface_bridge::close();

    delete this;
}

uint32_t STDCALL net_interface_imp::devs_count()
{
    return total_devs;
}
    
size_t STDCALL net_interface_imp::device_ip_address_count(size_t dev_index)
{
    if (dev_index < all_ip_addresses.size())
        return all_ip_addresses.at(dev_index).size();

    return -1;
}

uint64_t net_interface_imp::mac_addr()
{
    return selected_dev_mac;
}

uint64_t net_interface_imp::get_dev_eui()
{
    return selected_dev_eui;
}
    
void net_interface_imp::set_dev_eui(uint64_t dev_eui)
{
    selected_dev_eui = dev_eui;
}

char * STDCALL net_interface_imp::get_dev_desc_by_index(size_t dev_index)
{
    uint32_t index_i;

    // Get the selected interface
    for (dev = all_devs, index_i = 0; (index_i < dev_index) && (dev_index < total_devs); dev = dev->next, index_i++)
        ;

    if (!dev->name)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Interface name is blank.");
    }

    return dev->name;
}
    
const char * STDCALL net_interface_imp::get_dev_ip_address_by_index(size_t dev_index, size_t ip_index)
{
    if (dev_index < all_ip_addresses.size())
    {
        if (ip_index < all_ip_addresses.at(dev_index).size())
            return all_ip_addresses.at(dev_index).at(ip_index).c_str();
    }
    
    return NULL;
}
    
bool STDCALL net_interface_imp::does_interface_have_ip_address(size_t dev_index, char * ip_addr_str)
{
    if (dev_index < all_ip_addresses.size())
    {
        if (std::find(all_ip_addresses.at(dev_index).begin(), all_ip_addresses.at(dev_index).end(),
                      std::string(ip_addr_str)) != all_ip_addresses.at(dev_index).end())
            return true;
    }
    
    return false;
}
    
bool STDCALL net_interface_imp::does_interface_have_mac_address(size_t dev_index, uint64_t mac_addr)
{
    if (dev_index < all_mac_addresses.size())
    {
        if (all_mac_addresses.at(dev_index) == mac_addr)
            return true;
    }
    
    return false;
}
    
char * STDCALL net_interface_imp::get_dev_name_by_index(size_t dev_index)
{
    return get_dev_desc_by_index(dev_index);
}

int net_interface_imp::get_fd()
{
    if (pcap_interface)
        return pcap_get_selectable_fd(pcap_interface);

    return 0;
}

uint64_t net_interface_imp::get_dev_mac_addr_by_index(size_t dev_index)
{
    if (dev_index < all_mac_addresses.size())
        return all_mac_addresses.at(dev_index);
    
    return 0;
}

int STDCALL net_interface_imp::select_interface_by_num(uint32_t interface_num)
{
    uint32_t index;
    int timeout_ms = 1;

    if (interface_num < 1 || interface_num > total_devs)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Interface number out of range.");
        pcap_freealldevs(all_devs); // Free the device list
        return -1;
    }

    for (dev = all_devs, index = 0; index < interface_num - 1; dev = dev->next, index++)
        ; // Jump to the selected adapter
    
    selected_dev_mac = all_mac_addresses.at(index);
    selected_dev_eui = ((selected_dev_mac & UINT64_C(0xFFFFFF000000)) << 16) |
                        UINT64_C(0x000000FFFF000000) |
                        (selected_dev_mac & UINT64_C(0xFFFFFF));

    // use the Mac Native interface on the selected device, if supported
    if (mac_native_interface_bridge::is_avb_enabled((const char *)dev->name))
    {
        /**** Open the device with the Mac native interface ****/
        mac_native_interface_bridge::select((const char *)dev->name, selected_dev_eui, selected_dev_mac);
        is_netif_pcap = false;
    }
    else
    {
        /************* Open the device with pcap **************/
        if ((pcap_interface = pcap_open_live(dev->name, // Name of the device
                                             65536,     // Portion of the packet to capture
                                             // 65536 guarantees that the whole packet will be captured on all the link layers
                                             1,          // In promiscuous mode, all packets including packets of other hosts are captured
                                             timeout_ms, // Read timeout in ms
                                             err_buf     // Error buffer
                                             )) == NULL)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Unable to open the adapter. %s is not supported by pcap.", dev->name);
            pcap_freealldevs(all_devs); // Free the device list
            return -1;
        }

        if (pcap_setnonblock(pcap_interface, 1, err_buf) < 0)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "pcap_setnonblock");
            return -1;
        }

        int fd = pcap_fileno(pcap_interface);
        if (fd == -1)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Can't get file descriptor for pcap_t");
            return -1;
        }

        int on = 1;
        if (ioctl(fd, BIOCIMMEDIATE, &on) == -1)
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "BIOCIMMEDIATE error");
            return -1;
        }

        if (index >= all_mac_addresses.size())
        {
            log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Cannot find selected interface MAC address");
            return -1;
        }

        uint16_t ether_type[1];
        ether_type[0] = JDKSAVDECC_AVTP_ETHERTYPE;
        if (set_capture_ether_type(ether_type, 1) < 0) // Set the filter
            return -1;
    }

    return 0;
}

int net_interface_imp::set_capture_ether_type(uint16_t * ether_type, uint32_t count)
{
    struct bpf_program fcode;
    char ether_type_string[512];
    char ether_type_single[64];
    const unsigned char * ether_packet = NULL;
    struct pcap_pkthdr pcap_header;

    ether_type_string[0] = 0;

    for (uint32_t index_i = 0; index_i < count; index_i++)
    {
        sprintf(ether_type_single, "ether proto 0x%04x", ether_type[index_i]);
        strcat(ether_type_string, ether_type_single);

        if ((index_i + 1) < count)
        {
            strcat(ether_type_string, " or ");
        }
    }

    /******************************************************* Compile a filter ************************************************/
    if (pcap_compile(pcap_interface, &fcode, ether_type_string, 1, 0) < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Unable to compile the packet filter.");
        pcap_freealldevs(all_devs); // Free the device list
        return -1;
    }

    /*************************************************** Set the filter *******************************************/
    if (pcap_setfilter(pcap_interface, &fcode) < 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "Error setting the filter.");
        pcap_freealldevs(all_devs); // Free the device list
        return -1;
    }

    /*********** Flush any packets that might be present **********/
    for (uint32_t index_j = 0; index_j < 1; index_j++)
    {
        ether_packet = pcap_next(pcap_interface, &pcap_header);
        if (!ether_packet)
        {
            break;
        }
    }

    return 0;
}

int STDCALL net_interface_imp::capture_frame(const uint8_t ** frame, uint16_t * mem_buf_len)
{
    struct pcap_pkthdr * header;
    int error = 0;

    *mem_buf_len = 0;
    error = pcap_next_ex(pcap_interface, &header, frame);

    if (error > 0)
    {
        ether_frame = *frame;
        *mem_buf_len = (uint16_t)header->len;

        return 1;
    }

    return error;
}

int net_interface_imp::send_frame(uint8_t * frame, uint16_t mem_buf_len)
{
    if (!is_netif_pcap)
    {
        mac_native_interface_bridge::send_frame(frame, mem_buf_len);
        return 0;
    }

    if (pcap_sendpacket(pcap_interface, frame, mem_buf_len) != 0)
    {
        log_imp_ref->post_log_msg(LOGGING_LEVEL_ERROR, "pcap_sendpacket error %s", pcap_geterr(pcap_interface));
        return -1;
    }

    return 0;
}
    
void net_interface_imp::open_selected_bridge_interface()
{
    if (!is_netif_pcap)
        return mac_native_interface_bridge::open();
}

bool net_interface_imp::is_Mac_Native_end_station_connected(uint64_t entity_id)
{
    if (!is_netif_pcap)
        return mac_native_interface_bridge::is_end_station_connected(entity_id);
    
    return false;
}
}
