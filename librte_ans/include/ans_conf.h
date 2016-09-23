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


#ifndef __ANS_MSG_H__
#define __ANS_MSG_H__

#define ANS_MSG_POOL_NAME "ANS_CTRL_MSG_POOL"
#define ANS_SEC_2_PRI           "ANS_CTRL_SEC_2_PRI"
#define ANS_PRI_2_SEC           "ANS_CTRL_PRI_2_SEC"

#define ANS_RING_MSG_SIZE   256
#define ANS_IFNAME_LEN_MAX   16
#define ANS_IP_PER_IF_MAX      20
#define ANS_RT_FLAG_LEN         16
#define ANS_IFADDR_LEN           6 

#define ANS_TIMER_RESOLUTION_CYCLES 20000000ULL /* around 10ms at 2 Ghz */

typedef enum 
{
   ANS_MSG_TYPE_IPADDR,
   ANS_MSG_TYPE_ROUTE,
   ANS_MSG_TYPE_ARP,
   ANS_MSG_TYPE_LOG,
} ans_msg_type_e;

typedef enum 
{
   ANS_MSG_ACTION_ADD,
   ANS_MSG_ACTION_DEL,
   ANS_MSG_ACTION_SHOW,
   ANS_MSG_ACTION_SET,
} ans_msg_action_e;

typedef struct 
{
  uint32_t ip_addr;
  uint32_t netmask;
}ans_ipaddr_t;


typedef struct 
{
  char ifname[ANS_IFNAME_LEN_MAX];
  ans_ipaddr_t ip;
}ans_ipaddr_conf_t;

typedef struct 
{
  ans_ipaddr_t dest;
  uint32_t gateway;
}ans_route_conf_t;

typedef struct 
{
}ans_arp_conf_t;

typedef struct 
{
  uint32_t level;
}ans_log_conf_t;

typedef union
{
   ans_ipaddr_conf_t   ipaddr_conf;
   ans_route_conf_t     route_conf;
   ans_arp_conf_t        arp_conf;
   ans_log_conf_t        log_conf;

}ans_conf_data_t;

typedef struct
{
  ans_msg_type_e   msg_type;
  ans_msg_action_e msg_action;
  ans_conf_data_t    msg_data;
}ans_conf_req_t;

typedef struct
{
    char ifname[ANS_IFNAME_LEN_MAX];
    unsigned char ifaddr[ANS_IFADDR_LEN];
    ans_ipaddr_t ip;  
}ans_ipaddr_show_t;

typedef struct
{
  ans_ipaddr_t dest;
  uint32_t gateway;
  char rt_flags[ANS_RT_FLAG_LEN];
  char ifname[ANS_IFNAME_LEN_MAX];
}ans_route_show_t;

typedef struct
{
    uint32_t ipaddr;
    char  iftype[8];
    char ifname[ANS_IFNAME_LEN_MAX];
    unsigned char ifaddr[ANS_IFADDR_LEN];
}ans_arp_show_t;

typedef union
{
   ans_ipaddr_show_t   ipaddr_show;
   ans_route_show_t    route_show;
   ans_arp_show_t       arp_show;
}ans_show_data_t;

typedef struct
{
    int  status;
    int  data_len;
    ans_msg_type_e   msg_type;
    ans_msg_action_e msg_action;
    ans_show_data_t   msg_data;
}ans_conf_ack_t;

#endif /* __ANS_MSG_H__ */
