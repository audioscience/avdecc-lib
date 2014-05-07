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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/filter.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

#include "util.h"
#include "enumeration.h"
#include "log.h"
#include "jdksavdecc_util.h"
#include "net_interface_imp.h"

#include "bpf.h"

namespace avdecc_lib
{



    struct etherII
    {
        uint8_t destmac[6];
        uint8_t srcmac[6];
        uint8_t type[2];
    };
    struct ipheader
    {
        uint8_t ver_len;
        uint8_t service;
        uint8_t len[2];
        uint8_t ident[2];
        uint8_t flags;
        uint8_t frag_offset[2];
        uint8_t ttl;
        uint8_t protocol;
        uint8_t chksum[2];
        uint8_t sourceip[4];
        uint8_t destip[4];
    };
    struct udpheader
    {
        uint8_t srcport[2];
        uint8_t destport[2];
        uint8_t len[2];
        uint8_t chksum[2];
    };

    net_interface_imp::net_interface_imp()
    {
        struct ifaddrs *ifaddr, *ifa;
        int family, s;
        char host[NI_MAXHOST];
        char ifname[256];

        total_devs = 0;

        ip_hdr_store = new ipheader;
        udp_hdr_store = new udpheader;

        if (getifaddrs(&ifaddr) == -1)
        {
            perror("getifaddrs");
            exit(EXIT_FAILURE);
        }

        /* Walk through linked list, maintaining head pointer so we
        can free list later */

        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
                continue;

            family = ifa->ifa_addr->sa_family;

            /* Display interface name and family (including symbolic
            form of the latter for the common families) */
            if (family == AF_INET)
            {
                s = getnameinfo(ifa->ifa_addr,
                                (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                sizeof(struct sockaddr_in6),
                                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);



                snprintf(ifname, sizeof(ifname), "%s, address: <%s>",ifa->ifa_name, host);
                if (s != 0)
                {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }
                ifnames.push_back(ifname);
                total_devs++;
            }
        }

        freeifaddrs(ifaddr);
    }

    net_interface_imp::~net_interface_imp()
    {
        close(rawsock);
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
        return (char *)ifnames[dev_index].c_str();
    }

    int net_interface_imp::get_fd()
    {
        return rawsock;
    }


    int STDCALL net_interface_imp::select_interface_by_num(uint32_t interface_num)
    {
        struct sockaddr_ll sll;
        struct ifreq if_mac;
        const char *ifname;
        char *s;

        /* adjust interface numnber since count starts at 1 */
        interface_num--;

        ifname = ifnames[interface_num].c_str();
        s = (char *)ifname;
        while(*s != ',')
        {
            s++;
        }
        *s = 0;
        rawsock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (rawsock == -1)
        {
            fprintf(stderr, "Socket open failed! %s\nuse sudo?\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        ifindex = getifindex(rawsock, ifname);

        // get the mac address of the eth0 interface
        memset(&if_mac, 0, sizeof(struct ifreq));
        strncpy(if_mac.ifr_name, ifname, IFNAMSIZ-1);
        if (ioctl(rawsock, SIOCGIFHWADDR, &if_mac) < 0)
        {
            perror("SIOCGIFHWADDR");
            exit(EXIT_FAILURE);
        }

        setpromiscuous(rawsock, ifindex);

        memset(&sll,0, sizeof(sll));
        sll.sll_family = AF_PACKET;
        sll.sll_ifindex = ifindex;
        sll.sll_protocol = htons(ETH_P_ALL);
        bind(rawsock, (struct sockaddr *)&sll, sizeof(sll));

        utility::convert_eui48_to_uint64((uint8_t *)if_mac.ifr_hwaddr.sa_data, mac);


        uint16_t etypes[1] = {0x22f0};
        set_capture_ether_type(etypes, 1);

        return 0;
    }

    int net_interface_imp::set_capture_ether_type(uint16_t *ether_type, uint32_t count)
    {
        struct sock_fprog Filter;

        Filter.len = sizeof(BPF_code) / 8;
        Filter.filter = BPF_code;

        for (unsigned int i = 0; i < count; i++)
        {
            if (ether_type[i] != (uint16_t)ethernet_proto_bpf[i])
                fprintf(stderr, "NETIF - packet filter mismatch\n");
        }

        // attach filter to socket
        if(setsockopt(rawsock, SOL_SOCKET, SO_ATTACH_FILTER, &Filter, sizeof(Filter)) == -1)
        {
            fprintf(stderr, "socket attach filter failed! %s\n", strerror(errno));
            close(rawsock);
            exit(EXIT_FAILURE);
        }

        return 0;
    }

    int STDCALL net_interface_imp::capture_frame(const uint8_t **frame, uint16_t *mem_buf_len)
    {
        int len;

        *frame = &rx_buf[0];
        len = read(rawsock, &rx_buf[0], sizeof(rx_buf));
        if (len < 0)
        {
            *mem_buf_len = 0;
        }
        else
        {
            *mem_buf_len = len;
        }
        return len;
    }

    int net_interface_imp::send_frame(uint8_t *frame, uint16_t mem_buf_len)
    {
        int send_result;

        /*target address*/
        struct sockaddr_ll socket_address;

        /*prepare sockaddr_ll*/

        /*RAW communication*/
        socket_address.sll_family   = PF_PACKET;
        socket_address.sll_protocol = htons(ethertype);

        /*index of the network device
        see full code later how to retrieve it*/
        socket_address.sll_ifindex  = ifindex;

        /*ARP hardware identifier is ethernet*/
        socket_address.sll_hatype   = ARPHRD_ETHER;

        /*target is another host*/
        socket_address.sll_pkttype  = PACKET_OTHERHOST;

        /*address length*/
        socket_address.sll_halen    = ETH_ALEN;
        /*MAC - begin*/
        memcpy(&socket_address.sll_addr[0], &frame[0], 6);
        /*MAC - end*/
        socket_address.sll_addr[6]  = 0x00;/*not used*/
        socket_address.sll_addr[7]  = 0x00;/*not used*/

        /*send the packet*/
        send_result = sendto(rawsock, frame, mem_buf_len, 0,
                             (struct sockaddr*)&socket_address, sizeof(socket_address));

        return send_result;
    }

    int net_interface_imp::getifindex(int rawsock, const char *iface)
    {
        struct ifreq ifr;
        int ret;

        memset(&ifr,0,sizeof(ifr));
        strncpy(ifr.ifr_name,iface,sizeof(ifr.ifr_name));

        ret=ioctl(rawsock,SIOCGIFINDEX,&ifr);

        if(ret<0)
        {
            return ret;
        }

        return ifr.ifr_ifindex;
    }

    int net_interface_imp::setpromiscuous(int rawsock, int ifindex)
    {
        struct packet_mreq mr;

        memset(&mr,0,sizeof(mr));
        mr.mr_ifindex=ifindex;
        mr.mr_type=PACKET_MR_ALLMULTI;

        return setsockopt(rawsock, SOL_PACKET,
                          PACKET_ADD_MEMBERSHIP,&mr,sizeof(mr));
    }


    net_interface * create_net_interface()
    {
        return (new net_interface_imp());
    }

}
