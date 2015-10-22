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


#ifndef __NETDP_MSG_H__
#define __NETDP_MSG_H__

#define NETDP_MSG_POOL_NAME "NETDP_CTRL_MSG_POOL"
#define NETDP_SEC_2_PRI           "NETDP_CTRL_SEC_2_PRI"
#define NETDP_PRI_2_SEC           "NETDP_CTRL_PRI_2_SEC"

#define NETDP_RING_MSG_SIZE   256
#define NETDP_IFNAME_LEN_MAX   16
#define NETDP_IP_PER_IF_MAX      20
#define NETDP_RT_FLAG_LEN         16
#define NETDP_IFADDR_LEN           6 


typedef enum 
{
   NETDP_MSG_TYPE_IPADDR,
   NETDP_MSG_TYPE_ROUTE,
   NETDP_MSG_TYPE_ARP,
   NETDP_MSG_TYPE_LOG,
} netdp_msg_type_e;

typedef enum 
{
   NETDP_MSG_ACTION_ADD,
   NETDP_MSG_ACTION_DEL,
   NETDP_MSG_ACTION_SHOW,
   NETDP_MSG_ACTION_SET,
} netdp_msg_action_e;

typedef struct 
{
  uint32_t ip_addr;
  uint32_t netmask;
}netdp_ipaddr_t;


typedef struct 
{
  char ifname[NETDP_IFNAME_LEN_MAX];
  netdp_ipaddr_t ip;
}netdp_ipaddr_conf_t;

typedef struct 
{
  netdp_ipaddr_t dest;
  uint32_t gateway;
}netdp_route_conf_t;

typedef struct 
{
}netdp_arp_conf_t;

typedef struct 
{
  uint32_t level;
}netdp_log_conf_t;

typedef union
{
   netdp_ipaddr_conf_t   ipaddr_conf;
   netdp_route_conf_t     route_conf;
   netdp_arp_conf_t        arp_conf;
   netdp_log_conf_t        log_conf;

}netdp_conf_data_t;

typedef struct
{
  netdp_msg_type_e   msg_type;
  netdp_msg_action_e msg_action;
  netdp_conf_data_t    msg_data;
}netdp_conf_req_t;

typedef struct
{
    char ifname[NETDP_IFNAME_LEN_MAX];
    unsigned char ifaddr[NETDP_IFADDR_LEN];
    netdp_ipaddr_t ip;  
}netdp_ipaddr_show_t;

typedef struct
{
  netdp_ipaddr_t dest;
  uint32_t gateway;
  char rt_flags[NETDP_RT_FLAG_LEN];
  char ifname[NETDP_IFNAME_LEN_MAX];
}netdp_route_show_t;

typedef struct
{
    uint32_t ipaddr;
    char  iftype[8];
    char ifname[NETDP_IFNAME_LEN_MAX];
    unsigned char ifaddr[NETDP_IFADDR_LEN];
}netdp_arp_show_t;

typedef union
{
   netdp_ipaddr_show_t   ipaddr_show;
   netdp_route_show_t    route_show;
   netdp_arp_show_t       arp_show;
}netdp_show_data_t;

typedef struct
{
    int  status;
    int  data_len;
    netdp_msg_type_e   msg_type;
    netdp_msg_action_e msg_action;
    netdp_show_data_t   msg_data;
}netdp_conf_ack_t;

#endif /* __NETDP_MSG_H__ */
