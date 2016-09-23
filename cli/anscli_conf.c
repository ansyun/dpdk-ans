/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 ANS Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of ANS Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <termios.h>
#ifndef __linux__
  #ifdef __FreeBSD__
    #include <sys/socket.h>
  #else
    #include <net/socket.h>
  #endif
#endif

#include <rte_common.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_tailq.h>
#include <rte_per_lcore.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>


#include <cmdline_rdline.h>
#include <cmdline_parse.h>
#include <cmdline_parse_ipaddr.h>
#include <cmdline_parse_num.h>
#include <cmdline_parse_string.h>
#include <cmdline.h>


#include "anscli_conf.h"
#include "anscli_ring.h"
#include "ans_errno.h"
#include "ans_conf.h"


uint32_t netmask_len2int(int mask_len)
{
    int i;
    uint32_t i_mask = 0;

    if(mask_len == 0)
        return 0;

    for (i = 1, i_mask = 1; i < mask_len; i++)
    {
        i_mask = (i_mask << 1) | 1;
    }

    i_mask = htonl(i_mask << (32 - mask_len));

    return i_mask;
}


int netmask_int2len(unsigned int mask)
{
    int netmask = 0;

    while (mask & 0x80000000)
    {
        netmask++;
        mask = (mask << 1);
    }

    return netmask;
}


cmdline_parse_token_string_t anscli_name =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, name, "ip");
cmdline_parse_token_string_t anscli_action_add =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, action, "add");
cmdline_parse_token_string_t anscli_action_del =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, action, "del");
cmdline_parse_token_string_t anscli_action_show =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, action, "show");

cmdline_parse_token_string_t anscli_addr_type =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, type, "addr");
cmdline_parse_token_ipaddr_t anscli_addr_ip =
  TOKEN_IPV4NET_INITIALIZER(struct anscli_iproute_result, ipaddr);
cmdline_parse_token_string_t anscli_addr_dev =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, dev, "dev");
cmdline_parse_token_string_t anscli_addr_iface=
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, iface, NULL);

cmdline_parse_token_string_t anscli_route_type =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, type, "route");
cmdline_parse_token_ipaddr_t anscli_route_destip =
  TOKEN_IPV4NET_INITIALIZER(struct anscli_iproute_result, destip);
cmdline_parse_token_string_t anscli_route_via =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, via, "via");
cmdline_parse_token_ipaddr_t anscli_route_nexthop =
  TOKEN_IPV4_INITIALIZER(struct anscli_iproute_result, gateway);

cmdline_parse_token_string_t anscli_arp_type =
  TOKEN_STRING_INITIALIZER(struct anscli_iproute_result, type, "arp");


/*********************************************************
*
*    IP address add
*
*
**********************************************************/
static void anscli_ip_add_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t conf_ack;
    struct anscli_iproute_result *res = parsed_result;

    if(strlen(res->name) > ANS_IFNAME_LEN_MAX -1)
    {
        cmdline_printf(cl, "Too long device name, max length is %d \n", ANS_IFNAME_LEN_MAX -1);
        return;
    }

    memset(&conf_req, 0, sizeof(conf_req));

    conf_req.msg_type = ANS_MSG_TYPE_IPADDR;
    conf_req.msg_action= ANS_MSG_ACTION_ADD;

    strcpy(conf_req.msg_data.ipaddr_conf.ifname, res->iface);

    conf_req.msg_data.ipaddr_conf.ip.ip_addr = res->ipaddr.addr.ipv4.s_addr;
    conf_req.msg_data.ipaddr_conf.ip.netmask = netmask_len2int(res->ipaddr.prefixlen);

    ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = anscli_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != ANSCLI_RECV_MSG)
    {
        cmdline_printf(cl, "No reply\n");
        return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Add IP address failed,  error code %d \n", conf_ack.status);
    }
    else
    {
         cmdline_printf(cl, "Add IP address successfully \n");
    }
    return;
}


cmdline_parse_inst_t anscli_ip_add = {
  .f = anscli_ip_add_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip addr add 2.2.2.2/24 dev eth2",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_addr_type,
    (void *)&anscli_action_add,
    (void *)&anscli_addr_ip,
    (void *)&anscli_addr_dev,
    (void *)&anscli_addr_iface,
    NULL,
  },
};

/*********************************************************
*
*    IP address del
*
*
**********************************************************/
static void anscli_ip_del_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t conf_ack;
    struct anscli_iproute_result *res = parsed_result;


    if(strlen(res->name) > ANS_IFNAME_LEN_MAX -1)
    {
        cmdline_printf(cl, "Too long device name, max length is %d \n", ANS_IFNAME_LEN_MAX -1);
        return;
    }


    conf_req.msg_type = ANS_MSG_TYPE_IPADDR;
    conf_req.msg_action= ANS_MSG_ACTION_DEL;

    strcpy(conf_req.msg_data.ipaddr_conf.ifname, res->iface);

    conf_req.msg_data.ipaddr_conf.ip.ip_addr = res->ipaddr.addr.ipv4.s_addr;
    conf_req.msg_data.ipaddr_conf.ip.netmask = netmask_len2int(res->ipaddr.prefixlen);

    ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = anscli_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != ANSCLI_RECV_MSG)
    {
        cmdline_printf(cl, "No reply\n");
        return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Show IP address failed,  error code %d \n", conf_ack.status);
         return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Del IP address failed,  error code %d \n", conf_ack.status);
    }
    else
    {
         cmdline_printf(cl, "Del IP address successfully \n");
    }

    return;
}

cmdline_parse_inst_t anscli_ip_del = {
  .f = anscli_ip_del_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip addr del 2.2.2.2/24 dev eth2",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_addr_type,
    (void *)&anscli_action_del,
    (void *)&anscli_addr_ip,
    (void *)&anscli_addr_dev,
    (void *)&anscli_addr_iface,
    NULL,
  },
};

/*********************************************************
*
*    IP address show
*
*
**********************************************************/
static void anscli_ip_show_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int data_len = 0;
    int ret = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t *conf_ack;
    char msg_buf[ANS_RING_MSG_SIZE];
    int msg_len = ANS_RING_MSG_SIZE;
    struct anscli_iproute_result *res = parsed_result;
    struct in_addr ipaddr;
    int mask_len;
    ans_ipaddr_show_t   *ipaddr_show;
    char ifname[ANS_IFNAME_LEN_MAX];

    conf_req.msg_type = ANS_MSG_TYPE_IPADDR;
    conf_req.msg_action= ANS_MSG_ACTION_SHOW;
    memset(ifname, 0, sizeof(ifname));

     ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    while(1)
    {

        memset(msg_buf, 0, sizeof(msg_buf));

         ret = anscli_ring_recv((void *) msg_buf, msg_len);

        if(ret != ANSCLI_RECV_MSG)
        {
           // cmdline_printf(cl, "No reply\n");
            return;
        }

        conf_ack = (ans_conf_ack_t *)msg_buf;

        /* last message */
        if(conf_ack->status  == ANS_ESPIPE)
        {
            return;
        }

        if((conf_ack->status != 0) || (conf_ack->msg_action != ANS_MSG_ACTION_SHOW) || (conf_ack->data_len == 0))
        {
             cmdline_printf(cl, "Show IP address failed,  error code %d \n", conf_ack->status);
             return;
        }

        data_len = 0;
        while(data_len < conf_ack->data_len)
        {
            ipaddr_show = (ans_ipaddr_show_t *)((void *)&(conf_ack->msg_data.ipaddr_show) + data_len);
            if(0 != strcmp(ifname, ipaddr_show->ifname))
            {
                strcpy(ifname, ipaddr_show->ifname);
                cmdline_printf(cl, "\n%s\t", ipaddr_show->ifname);
                cmdline_printf(cl, "HWaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
                    ipaddr_show->ifaddr[0], ipaddr_show->ifaddr[1], ipaddr_show->ifaddr[2],
                    ipaddr_show->ifaddr[3], ipaddr_show->ifaddr[4], ipaddr_show->ifaddr[5]);
            }

            data_len += sizeof(ans_ipaddr_show_t);

            if(ipaddr_show->ip.ip_addr == 0)
                continue;

            memset(&ipaddr, 0, sizeof(ipaddr));
            ipaddr.s_addr =  ipaddr_show->ip.ip_addr;
            mask_len =  netmask_int2len(ntohl(ipaddr_show->ip.netmask));
            cmdline_printf(cl, "\tinet addr:" NIPQUAD_FMT "/%d\n", NIPQUAD(ipaddr), mask_len);

        }

    }

    return;
 }

cmdline_parse_inst_t anscli_ip_show = {
  .f = anscli_ip_show_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip addr show",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_addr_type,
    (void *)&anscli_action_show,
    NULL,
  },
};



/*********************************************************
*
*    route add
*
*
**********************************************************/
static void anscli_route_add_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t conf_ack;
    struct anscli_iproute_result *res = parsed_result;

    memset(&conf_req, 0, sizeof(conf_req));

    conf_req.msg_type = ANS_MSG_TYPE_ROUTE;
    conf_req.msg_action= ANS_MSG_ACTION_ADD;

    conf_req.msg_data.route_conf.dest.ip_addr = res->destip.addr.ipv4.s_addr;
    conf_req.msg_data.route_conf.dest.netmask = netmask_len2int(res->destip.prefixlen);
    conf_req.msg_data.route_conf.gateway = res->gateway.addr.ipv4.s_addr;

    ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = anscli_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != ANSCLI_RECV_MSG)
    {
        cmdline_printf(cl, "No reply\n");
        return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Add routing failed,  error code %d \n", conf_ack.status);
    }
    else
    {
         cmdline_printf(cl, "Add routing successfully \n");
    }
    return;

}

cmdline_parse_inst_t anscli_route_add = {
  .f = anscli_route_add_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip route add 2.2.2.2/24  via 3.3.3.3",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_route_type,
    (void *)&anscli_action_add,
    (void *)&anscli_route_destip,
    (void *)&anscli_route_via,
    (void *)&anscli_route_nexthop,
    NULL,
  },
};

/*********************************************************
*
*    route del
*
*
**********************************************************/
static void anscli_route_del_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t conf_ack;
    struct anscli_iproute_result *res = parsed_result;

    memset(&conf_req, 0, sizeof(conf_req));

    conf_req.msg_type = ANS_MSG_TYPE_ROUTE;
    conf_req.msg_action= ANS_MSG_ACTION_DEL;

    conf_req.msg_data.route_conf.dest.ip_addr = res->destip.addr.ipv4.s_addr;
    conf_req.msg_data.route_conf.dest.netmask = netmask_len2int(res->destip.prefixlen);

    ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = anscli_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != ANSCLI_RECV_MSG)
    {
        cmdline_printf(cl, "No reply\n");
        return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Del routing failed,  error code %d \n", conf_ack.status);
    }
    else
    {
         cmdline_printf(cl, "Del routing successfully \n");
    }
    return;
}

cmdline_parse_inst_t anscli_route_del = {
  .f = anscli_route_del_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip route del 2.2.2.2/24 ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_route_type,
    (void *)&anscli_action_del,
    (void *)&anscli_route_destip,
    NULL,
  },
};


/*********************************************************
*
*    route show
*
*
**********************************************************/
static void anscli_route_show_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int i = 0;
    int ret = 0;
    int flag = 0;
    int data_len = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t *conf_ack;
    ans_route_show_t *route_show;
    char msg_buf[ANS_RING_MSG_SIZE];
    int msg_len = ANS_RING_MSG_SIZE;
    struct anscli_iproute_result *res = parsed_result;
    struct in_addr ipaddr;
    int mask_len;
    char ip_str[32];

    conf_req.msg_type = ANS_MSG_TYPE_ROUTE;
    conf_req.msg_action= ANS_MSG_ACTION_SHOW;

     ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    while(1)
    {

        memset(msg_buf, 0, sizeof(msg_buf));

         ret = anscli_ring_recv((void *) msg_buf, sizeof(msg_buf));

        if(ret != ANSCLI_RECV_MSG)
        {
           // cmdline_printf(cl, "No reply\n");
            return;
        }

        conf_ack = (ans_conf_ack_t *)msg_buf;

        /* last message */
        if(conf_ack->status  == ANS_ESPIPE)
        {
            return;
        }

        if((conf_ack->status != 0) || (conf_ack->msg_action != ANS_MSG_ACTION_SHOW) || ( conf_ack->data_len == 0))
        {
             cmdline_printf(cl, "Show route failed,  error code %d \n", conf_ack->status);
             return;
        }

        if(flag == 0)
        {
             cmdline_printf(cl, "\nANS IP routing table\n");
             cmdline_printf(cl, "%-17s", "Destination");
             cmdline_printf(cl, "%-17s", "Gateway");
             cmdline_printf(cl, "%-17s", "Netmask");
             cmdline_printf(cl, "%-8s", "Flags");
             cmdline_printf(cl, "%-16s\n", "Iface");
             flag = 1;
        }

        data_len = 0;
        while(data_len < conf_ack->data_len)
        {

            route_show = (ans_route_show_t *)((void *)&(conf_ack->msg_data.route_show) + data_len);
            ipaddr.s_addr =  route_show->dest.ip_addr;
            sprintf(ip_str, NIPQUAD_FMT, NIPQUAD(ipaddr));
            cmdline_printf(cl, "%-17s", ip_str );

            if(route_show->gateway == 0)
            {
                cmdline_printf(cl, "%-17s", "*" );
            }
            else
            {
                ipaddr.s_addr = route_show->gateway;
                sprintf(ip_str, NIPQUAD_FMT, NIPQUAD(ipaddr));
                cmdline_printf(cl, "%-17s", ip_str );
            }

            ipaddr.s_addr =  route_show->dest.netmask;
            sprintf(ip_str, NIPQUAD_FMT, NIPQUAD(ipaddr));
            cmdline_printf(cl, "%-17s", ip_str );

            cmdline_printf(cl,  "%-8s", route_show->rt_flags);

            cmdline_printf(cl,  "%-16s\n", route_show->ifname);

            data_len += sizeof(ans_route_show_t);

        }
    }
    return;

}

cmdline_parse_inst_t anscli_route_show = {
  .f = anscli_route_show_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip route show ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_route_type,
    (void *)&anscli_action_show,
    NULL,
  },
};

/*********************************************************
*
*    arp show
*
*
**********************************************************/
static void anscli_arp_show_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int i = 0;
    int ret = 0;
    int flag = 0;
    int data_len = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t *conf_ack;
    ans_arp_show_t *arp_show;
    struct anscli_iproute_result *res = parsed_result;
    struct in_addr ipaddr;
    char msg_buf[ANS_RING_MSG_SIZE];
    int msg_len = ANS_RING_MSG_SIZE;
    int mask_len;
    char str[32];
    uint8_t ans_enet_zeroaddr[ANS_IFADDR_LEN] = {0x0,0x0,0x0,0x0,0x0,0x0};

    conf_req.msg_type = ANS_MSG_TYPE_ARP;
    conf_req.msg_action= ANS_MSG_ACTION_SHOW;

     ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    while(1)
    {

        memset(msg_buf, 0, sizeof(msg_buf));

         ret = anscli_ring_recv((void *) msg_buf, sizeof(msg_buf));

        if(ret != ANSCLI_RECV_MSG)
        {
           // cmdline_printf(cl, "No reply\n");
            return;
        }

        conf_ack = (ans_conf_ack_t *)msg_buf;
        /* last message */
        if(conf_ack->status  == ANS_ESPIPE)
        {
            return;
        }

        if((conf_ack->status != 0) || (conf_ack->msg_action != ANS_MSG_ACTION_SHOW) ||( conf_ack->data_len == 0))
        {
             cmdline_printf(cl, "Show arp failed,  error code %d \n", conf_ack->status);
             return;
        }

        if(flag == 0)
        {
             cmdline_printf(cl, "\nANS ARP table\n");
             cmdline_printf(cl, "%-17s", "Address");
             cmdline_printf(cl, "%-10s", "HWtype");
             cmdline_printf(cl, "%-20s", "HWaddress");
             cmdline_printf(cl, "%-16s\n", "Iface");
             flag = 1;
        }

        data_len = 0;
        while(data_len < conf_ack->data_len)
        {
            arp_show = (ans_arp_show_t *)((void *)&(conf_ack->msg_data.arp_show) + data_len);
            ipaddr.s_addr =  arp_show->ipaddr;
            sprintf(str, NIPQUAD_FMT, NIPQUAD(ipaddr));
            cmdline_printf(cl, "%-17s", str );

            cmdline_printf(cl, "%-10s", arp_show->iftype);

            if(!bcmp((caddr_t)(arp_show->ifaddr), ans_enet_zeroaddr, ANS_IFADDR_LEN))
            {
                cmdline_printf(cl, "%-20s", "(incomplete)" );
            }
            else
            {
                sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
                    arp_show->ifaddr[0], arp_show->ifaddr[1], arp_show->ifaddr[2],
                    arp_show->ifaddr[3], arp_show->ifaddr[4], arp_show->ifaddr[5]);

                cmdline_printf(cl, "%-20s", str );
            }
            cmdline_printf(cl,  "%-16s\n", arp_show->ifname);

            data_len += sizeof(ans_arp_show_t);
        }
    }
    return;

}

cmdline_parse_inst_t anscli_arp_show = {
  .f = anscli_arp_show_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip arp show ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_name,
    (void *)&anscli_arp_type,
    (void *)&anscli_action_show,
    NULL,
  },
};





/*********************************************************
*
*    log
*
*
**********************************************************/
cmdline_parse_token_string_t anscli_log_name =
  TOKEN_STRING_INITIALIZER(struct anscli_log_result, name, "log");
cmdline_parse_token_string_t anscli_log_type =
  TOKEN_STRING_INITIALIZER(struct anscli_log_result, type, "level");
cmdline_parse_token_string_t anscli_log_action =
  TOKEN_STRING_INITIALIZER(struct anscli_log_result, action, "set");
cmdline_parse_token_string_t anscli_log_level=
  TOKEN_STRING_INITIALIZER(struct anscli_log_result, level, NULL);


/*********************************************************
*
*    log level set
*
*
**********************************************************/
static void anscli_log_set_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    ans_conf_req_t conf_req;
    ans_conf_ack_t conf_ack;
    struct anscli_log_result *res = parsed_result;

    memset(&conf_req, 0, sizeof(conf_req));

    conf_req.msg_type = ANS_MSG_TYPE_LOG;
    conf_req.msg_action= ANS_MSG_ACTION_SET;

    if(0 == strcmp(res->level, "emerg"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_EMERG;
    }
    else if(0 == strcmp(res->level, "alert"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_ALERT;
    }
    else if(0 == strcmp(res->level, "crit"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_CRIT;
    }
    else if(0 == strcmp(res->level, "err"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_ERR;
    }
    else if(0 == strcmp(res->level, "warning"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_WARNING;
    }
    else if(0 == strcmp(res->level, "notice"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_NOTICE;
    }
    else if(0 == strcmp(res->level, "info"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_INFO;
    }
    else if(0 == strcmp(res->level, "debug"))
    {
        conf_req.msg_data.log_conf.level = RTE_LOG_DEBUG;
    }
    else
    {
        cmdline_printf(cl, "Invalid log level \n");
        return;
    }

    ret = anscli_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = anscli_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != ANSCLI_RECV_MSG)
    {
        cmdline_printf(cl, "No reply\n");
        return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Set log level failed,  error code %d \n", conf_ack.status);
         return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Set log level failed,  error code %d \n", conf_ack.status);
    }
    else
    {
         cmdline_printf(cl, "Set log level successfully \n");
    }

    return;

}


cmdline_parse_inst_t anscli_log_set = {
  .f = anscli_log_set_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "log level set [emerg | alert | crit | err | warning | notice | info | debug] \n",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_log_name,
    (void *)&anscli_log_type,
    (void *)&anscli_log_action,
    (void *)&anscli_log_level,
    NULL,
  },
};




/*********************************************************
*
*    quit
*
*
**********************************************************/
cmdline_parse_token_string_t anscli_quit_quit = TOKEN_STRING_INITIALIZER(struct anscli_quit_result, quit, "quit");

static void anscli_quit_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    struct anscli_quit_result *res = parsed_result;

    cmdline_printf(cl, "Quit ans command \n");
    cmdline_quit(cl);

}

cmdline_parse_inst_t anscli_quit = {
  .f = anscli_quit_parsed,            /* function to call */
  .data = NULL,                                                 /* 2nd arg of func */
  .help_str = "quit ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&anscli_quit_quit,
    NULL,
  },
};

/*********************************************************
*
*    help
*
*
**********************************************************/
static void anscli_help_parsed(__attribute__((unused)) void *parsed_result,
          struct cmdline *cl,
          __attribute__((unused)) void *data)
{
    cmdline_printf(cl,
           "ip addr add IFADDR dev STRING \n"
           "ip addr del IFADDR dev STRING\n"
           "ip addr show\n"
           "ip route add DESTIP via NEXTHOP\n"
           "ip route del DESTIP\n"
           "ip route show\n"
           "ip arp show\n"
           "log level set [emerg | alert | crit | err | warning | notice | info | debug] \n"
           "help\n"
           "quit\n"
           );
}

cmdline_parse_token_string_t anscli_help_help = TOKEN_STRING_INITIALIZER(struct anscli_help_result, help, "help");

cmdline_parse_inst_t anscli_help = {
  .f = anscli_help_parsed,  /* function to call */
  .data = NULL,                            /* 2nd arg of func */
  .help_str = "show help",
  .tokens = {                    /* token list, NULL terminated */
    (void *)&anscli_help_help,
    NULL,
  },
};


/**********************************************************/
/**********************************************************/
/****** CONTEXT (list of instruction) */

cmdline_parse_ctx_t ip_main_ctx[] = {
  (cmdline_parse_inst_t *)&anscli_ip_add,
  (cmdline_parse_inst_t *)&anscli_ip_del,
  (cmdline_parse_inst_t *)&anscli_ip_show,
  (cmdline_parse_inst_t *)&anscli_route_add,
  (cmdline_parse_inst_t *)&anscli_route_del,
  (cmdline_parse_inst_t *)&anscli_route_show,
  (cmdline_parse_inst_t *)&anscli_arp_show,
  (cmdline_parse_inst_t *)&anscli_log_set,
  (cmdline_parse_inst_t *)&anscli_help,
  (cmdline_parse_inst_t *)&anscli_quit,
  NULL,
};

