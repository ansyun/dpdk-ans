/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 Netdp Corporation. All rights reserved.
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
 *     * Neither the name of Netdp Corporation nor the names of its
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
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_string_fns.h>


#include <cmdline_rdline.h>
#include <cmdline_parse.h>
#include <cmdline_parse_ipaddr.h>
#include <cmdline_parse_num.h>
#include <cmdline_parse_string.h>
#include <cmdline.h>


#include "netdpcmd_ip.h"
#include "netdp_errno.h"
#include "netdp_ring.h"


uint32_t netmask_len2int(int mask_len)
{
    int i;
    uint32_t i_mask;
    
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


cmdline_parse_token_string_t netdpcmd_name =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, name, "ip");
cmdline_parse_token_string_t netdpcmd_action_add =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, action, "add");
cmdline_parse_token_string_t netdpcmd_action_del =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, action, "del");
cmdline_parse_token_string_t netdpcmd_action_show =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, action, "show");

cmdline_parse_token_string_t netdpcmd_addr_type =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, type, "addr");
cmdline_parse_token_ipaddr_t netdpcmd_addr_ip =
  TOKEN_IPV4NET_INITIALIZER(struct netdpcmd_ip_addr_result, ipaddr);
cmdline_parse_token_string_t netdpcmd_addr_dev =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, dev, "dev");
cmdline_parse_token_string_t netdpcmd_addr_iface=
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, iface, NULL);

cmdline_parse_token_string_t netdpcmd_route_type =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, type, "route");
cmdline_parse_token_ipaddr_t netdpcmd_route_destip =
  TOKEN_IPV4NET_INITIALIZER(struct netdpcmd_ip_addr_result, destip);
cmdline_parse_token_string_t netdpcmd_route_via =
  TOKEN_STRING_INITIALIZER(struct netdpcmd_ip_addr_result, via, "via");
cmdline_parse_token_ipaddr_t netdpcmd_route_nexthop =
  TOKEN_IPV4_INITIALIZER(struct netdpcmd_ip_addr_result, nexthop);


/*********************************************************
*
*    IP address add
*
*
**********************************************************/
static void netdpcmd_ip_add_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    netdp_conf_req_t conf_req;
    netdp_conf_ack_t conf_ack;
    struct netdpcmd_ip_addr_result *res = parsed_result;
    char ip_str[INET6_ADDRSTRLEN];
    
    snprintf(ip_str, sizeof(ip_str), NIPQUAD_FMT, NIPQUAD(res->ipaddr.addr.ipv4));

    cmdline_printf(cl, "cmd: %s %s %s %s/%d %s %s\n", res->name, res->type, res->action, ip_str, res->ipaddr.prefixlen, res->dev, res->iface);

    if(strlen(res->name) > NETDP_IFNAME_LEN_MAX -1)
    {
        cmdline_printf(cl, "Too long device name, max length is %d \n", NETDP_IFNAME_LEN_MAX -1);
        return;
    }

    memset(&conf_req, 0, sizeof(conf_req));

    conf_req.msg_type = NETDP_MSG_TYPE_IPADDR;
    conf_req.msg_action= NETDP_MSG_ACTION_ADD;

    strcpy(conf_req.msg_data.ipaddr_conf.ifname, res->iface);

    conf_req.msg_data.ipaddr_conf.ip.ip_addr = res->ipaddr.addr.ipv4.s_addr;
    conf_req.msg_data.ipaddr_conf.ip.netmask = netmask_len2int(res->ipaddr.prefixlen);
        
    ret = netdpcmd_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = netdpcmd_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != NETDPCMD_RECV_MSG)
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


cmdline_parse_inst_t netdpcmd_ip_add = {
  .f = netdpcmd_ip_add_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip addr add 2.2.2.2/24 dev eth2",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_name,
    (void *)&netdpcmd_addr_type,
    (void *)&netdpcmd_action_add,
    (void *)&netdpcmd_addr_ip,
    (void *)&netdpcmd_addr_dev,
    (void *)&netdpcmd_addr_iface,
    NULL,
  },
};

/*********************************************************
*
*    IP address del
*
*
**********************************************************/
static void netdpcmd_ip_del_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int ret = 0;
    netdp_conf_req_t conf_req;
    netdp_conf_ack_t conf_ack;
    struct netdpcmd_ip_addr_result *res = parsed_result;
    char ip_str[INET6_ADDRSTRLEN];

    snprintf(ip_str, sizeof(ip_str), NIPQUAD_FMT, NIPQUAD(res->ipaddr.addr.ipv4));

    cmdline_printf(cl, "cmd: %s %s %s %s/%d %s %s\n", res->name, res->type, res->action, ip_str, res->ipaddr.prefixlen, res->dev, res->iface);


    if(strlen(res->name) > NETDP_IFNAME_LEN_MAX -1)
    {
        cmdline_printf(cl, "Too long device name, max length is %d \n", NETDP_IFNAME_LEN_MAX -1);
        return;
    }


    conf_req.msg_type = NETDP_MSG_TYPE_IPADDR;
    conf_req.msg_action= NETDP_MSG_ACTION_DEL;

    strcpy(conf_req.msg_data.ipaddr_conf.ifname, res->iface);

    conf_req.msg_data.ipaddr_conf.ip.ip_addr = res->ipaddr.addr.ipv4.s_addr;
    conf_req.msg_data.ipaddr_conf.ip.netmask = netmask_len2int(res->ipaddr.prefixlen);
        
    ret = netdpcmd_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = netdpcmd_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != NETDPCMD_RECV_MSG)
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

cmdline_parse_inst_t netdpcmd_ip_del = {
  .f = netdpcmd_ip_del_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip addr del 2.2.2.2/24 dev eth2",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_name,
    (void *)&netdpcmd_addr_type,
    (void *)&netdpcmd_action_del,
    (void *)&netdpcmd_addr_ip,
    (void *)&netdpcmd_addr_dev,
    (void *)&netdpcmd_addr_iface,
    NULL,
  },
};

/*********************************************************
*
*    IP address show
*
*
**********************************************************/
static void netdpcmd_ip_show_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    int i = 0;
    int ret = 0;
    netdp_conf_req_t conf_req;
    netdp_conf_ack_t conf_ack;
    struct netdpcmd_ip_addr_result *res = parsed_result;
    struct in_addr ipaddr;
    int mask_len;
 
    conf_req.msg_type = NETDP_MSG_TYPE_IPADDR;
    conf_req.msg_action= NETDP_MSG_ACTION_SHOW;
        
     ret = netdpcmd_ring_send((void *) &conf_req, sizeof(conf_req));

    memset(&conf_ack, 0, sizeof(conf_ack));

     ret = netdpcmd_ring_recv((void *) &conf_ack, sizeof(conf_ack));

    if(ret != NETDPCMD_RECV_MSG)
    {
        cmdline_printf(cl, "No reply\n");
        return;
    }

    if(conf_ack.status != 0)
    {
         cmdline_printf(cl, "Show IP address failed,  error code %d \n", conf_ack.status);
         return;
    }
    
    cmdline_printf(cl, "\n%s\t", conf_ack.msg_data.ipaddr_show.ifname);
    cmdline_printf(cl, "HWaddr %02x:%02x:%02x:%02x:%02x:%02x\n", 
        conf_ack.msg_data.ipaddr_show.ifaddr[0],
        conf_ack.msg_data.ipaddr_show.ifaddr[1],
        conf_ack.msg_data.ipaddr_show.ifaddr[2],
        conf_ack.msg_data.ipaddr_show.ifaddr[3],
        conf_ack.msg_data.ipaddr_show.ifaddr[4],
        conf_ack.msg_data.ipaddr_show.ifaddr[5]
        );
    

    for(i = 0; i < NETDP_IP_PER_IF_MAX; i++ )
    {
        memset(&ipaddr, 0, sizeof(ipaddr));

        if(conf_ack.msg_data.ipaddr_show.ip[i].ip_addr == 0)
            continue;
        
        ipaddr.s_addr =  conf_ack.msg_data.ipaddr_show.ip[i].ip_addr;
        mask_len =  netmask_int2len(ntohl(conf_ack.msg_data.ipaddr_show.ip[i].netmask));
        cmdline_printf(cl, "\tinet addr:" NIPQUAD_FMT "/%d\n" , NIPQUAD(ipaddr), mask_len);

    }

    return;
 }

cmdline_parse_inst_t netdpcmd_ip_show = {
  .f = netdpcmd_ip_show_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip addr show",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_name,
    (void *)&netdpcmd_addr_type,
    (void *)&netdpcmd_action_show,
    NULL,
  },
};



/*********************************************************
*
*    route add
*
*
**********************************************************/
static void netdpcmd_route_add_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
  struct netdpcmd_ip_addr_result *res = parsed_result;
//  char ip_str[INET6_ADDRSTRLEN];

  //     snprintf(ip_str, sizeof(ip_str), NIPQUAD_FMT, NIPQUAD(res->ipaddr.addr.ipv4));

  cmdline_printf(cl, "cmd: %s %s %s \n", res->name, res->type, res->action);

}

cmdline_parse_inst_t netdpcmd_route_add = {
  .f = netdpcmd_route_add_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip route add 2.2.2.2/24  via 3.3.3.3",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_name,
    (void *)&netdpcmd_route_type,
    (void *)&netdpcmd_action_add,
    (void *)&netdpcmd_route_destip,
    (void *)&netdpcmd_route_via,
    (void *)&netdpcmd_route_nexthop,
    NULL,
  },
};

/*********************************************************
*
*    route del
*
*
**********************************************************/
static void netdpcmd_route_del_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
  struct netdpcmd_ip_addr_result *res = parsed_result;
//  char ip_str[INET6_ADDRSTRLEN];

  //     snprintf(ip_str, sizeof(ip_str), NIPQUAD_FMT, NIPQUAD(res->ipaddr.addr.ipv4));

  cmdline_printf(cl, "cmd: %s %s %s \n", res->name, res->type, res->action);

}

cmdline_parse_inst_t netdpcmd_route_del = {
  .f = netdpcmd_route_add_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip route del 2.2.2.2/24 ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_name,
    (void *)&netdpcmd_route_type,
    (void *)&netdpcmd_action_del,
    (void *)&netdpcmd_route_destip,
    NULL,
  },
};


/*********************************************************
*
*    route show
*
*
**********************************************************/
static void netdpcmd_route_show_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
  struct netdpcmd_ip_addr_result *res = parsed_result;
//  char ip_str[INET6_ADDRSTRLEN];

  //     snprintf(ip_str, sizeof(ip_str), NIPQUAD_FMT, NIPQUAD(res->ipaddr.addr.ipv4));

  cmdline_printf(cl, "cmd: %s %s %s \n", res->name, res->type, res->action);

}

cmdline_parse_inst_t netdpcmd_route_show = {
  .f = netdpcmd_route_show_parsed,            /* function to call */
  .data = NULL,                                   /* 2nd arg of func */
  .help_str = "ip route show ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_name,
    (void *)&netdpcmd_route_type,
    (void *)&netdpcmd_action_show,
    NULL,
  },
};

/*********************************************************
*
*    quit
*
*
**********************************************************/
cmdline_parse_token_string_t netdpcmd_quit_quit = TOKEN_STRING_INITIALIZER(struct netdpcmd_quit_result, quit, "quit");

static void netdpcmd_quit_parsed(void *parsed_result,
             struct cmdline *cl,
             __attribute__((unused)) void *data)
{
    struct netdpcmd_quit_result *res = parsed_result;

    cmdline_printf(cl, "Quit netdp command \n");
    cmdline_quit(cl);

}

cmdline_parse_inst_t netdpcmd_quit = {
  .f = netdpcmd_quit_parsed,            /* function to call */
  .data = NULL,                                                 /* 2nd arg of func */
  .help_str = "quit ",
  .tokens = {                                    /* token list, NULL terminated */
    (void *)&netdpcmd_quit_quit,
    NULL,
  },
};

/*********************************************************
*
*    help
*
*
**********************************************************/
static void netdpcmd_help_parsed(__attribute__((unused)) void *parsed_result,
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
           "help\n"
           "quit\n"
           );
}

cmdline_parse_token_string_t netdpcmd_help_help = TOKEN_STRING_INITIALIZER(struct netdpcmd_help_result, help, "help");

cmdline_parse_inst_t netdpcmd_help = {
  .f = netdpcmd_help_parsed,  /* function to call */
  .data = NULL,                            /* 2nd arg of func */
  .help_str = "show help",
  .tokens = {                    /* token list, NULL terminated */
    (void *)&netdpcmd_help_help,
    NULL,
  },
};


/**********************************************************/
/**********************************************************/
/****** CONTEXT (list of instruction) */

cmdline_parse_ctx_t ip_main_ctx[] = {
  (cmdline_parse_inst_t *)&netdpcmd_ip_add,
  (cmdline_parse_inst_t *)&netdpcmd_ip_del,
  (cmdline_parse_inst_t *)&netdpcmd_ip_show,
  (cmdline_parse_inst_t *)&netdpcmd_route_add,
  (cmdline_parse_inst_t *)&netdpcmd_route_del,
  (cmdline_parse_inst_t *)&netdpcmd_route_show,
  (cmdline_parse_inst_t *)&netdpcmd_help,
  (cmdline_parse_inst_t *)&netdpcmd_quit,
  NULL,
};

