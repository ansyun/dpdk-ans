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


#ifndef __NETDP_INIT_H__
#define __NETDP_INIT_H__

#define NETDP_MAX_NB_SOCKETS 8

#define NETDP_RSS_KEY_SIZE 40

#define NETDP_HW_CHKSUM_DISABLE 0   /* NIC don't support hardware checksum */
#define NETDP_HW_CHKSUM_ENABLE   1     /* NIC support hardware checksum */

/**
 *
 *
 */
struct netdp_init_config 
{
    uint64_t lcore_mask;                                                                   /* lcore which used to run netdp */
    uint32_t max_sock_conn;                                                            /* support max sock connection */
    uint32_t max_tcp_conn_per_lcore;                                            /* shall be power of 2 */
    uint8_t   hw_ip_checksum;                                                          /* If IP checksum enable */
    struct rte_mempool *pktmbuf_pool[NETDP_MAX_NB_SOCKETS];  /* mbuf pools for each sockets */
    uint8_t rss_key[NETDP_RSS_KEY_SIZE];
} __rte_cache_aligned;


typedef int (*netdp_send_packet_cb)(struct rte_mbuf *m, uint8_t port);

/**
 * @param user_conf   : user config.
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */
int netdp_initialize(struct netdp_init_config *user_conf);

/**
 * 
 * @param send_cb     
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */
int netdp_register(netdp_send_packet_cb send_cb);

/**
 * 
 *
 * @param m      
 * @param portid   
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */
 int netdp_packet_handle(struct rte_mbuf *m, uint8_t portid);


/**
 * Dequeue message from NETDP_SEC_2_PRI rte_ring, and then handle it.
 * Only handle one message for each loop.
 *
 * @return  
 *
 */
void netdp_message_handle(unsigned lcore_id);


#endif /* __NETDP_INIT_H__ */
