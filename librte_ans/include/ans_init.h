
/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2015-2017 Ansyun <anssupport@163.com>. All rights reserved.
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
 *     * Neither the name of Ansyun <anssupport@163.com> nor the names of its
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

#ifndef __ANS_INIT_H__
#define __ANS_INIT_H__

/**
 * @file
 *
 * ANS initialization API defines.
 *
 */

#define ANS_MAX_NB_LCORE    RTE_MAX_LCORE
#define ANS_MAX_NB_SOCKETS 8    /**< max number of sockets */

#define ANS_LCORE_ROLE_DISABLE 0  /**< lcore disable */
#define ANS_LCORE_ROLE_RX      0x1  /**< lcore for polling packet from nic */
#define ANS_LCORE_ROLE_WORKER  0x2  /**< lcore for high level packet handling */

/**
 * ANS lcore config
 */
struct ans_lcore
{
    uint8_t lcore_id;
    uint8_t lcore_role;
};

/**
 * ANS configuration
 */
struct ans_init_config 
{
    uint16_t lcore_nb;                                                                /**< lcore number */
    struct ans_lcore lcore[ANS_MAX_NB_LCORE];                                 /**< lcore configuration */
    uint32_t sock_nb;                                                                /**< support max sock connection number */
    uint8_t  ip_sync;                                                                 /**< 1: enable ip/route sync from kernel, otherwise disable sync */    
    cpu_set_t cpu_set;                                                              /**< system default cpu set */
    struct rte_mempool *pktmbuf_pool[ANS_MAX_NB_SOCKETS];     /**< mbuf pools for each sockets */

    uint16_t (*port_send)(uint16_t port_id, uint16_t queue_id, struct rte_mbuf **tx_pkts, uint16_t nb_pkts);  /** callback for sending one mbuf to port */

    int (*port_bypass)(uint8_t port, struct rte_mbuf *m);              /** callback for bypassing one mbuf to linux */

} __rte_cache_aligned;


/**
 * Init ans stack.
 * @param user_conf   
 *   user config.
 *
 * @return  0 - SUCCESS, non-zero - FAILURE, process shall exist.
 *
 */
int ans_initialize(struct ans_init_config *user_conf);

/**
 * Handle ans internal message.
 *
 *
 * @return  
 *
 */
void ans_message_handle(uint64_t cur_tsc);


#endif /* __ANS_INIT_H__ */
