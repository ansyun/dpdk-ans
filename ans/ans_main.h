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

#ifndef _ANS_MAIN_H_
#define _ANS_MAIN_H_

#define MAX_PKT_BURST     32
#define MAX_TX_BURST      32         /* set tx burst as 1 for lower packet latency, shall set to 32 ? */
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */

#define MAX_MBUF_NB     (1024 * 16)


/* Configure how many packets ahead to prefetch, when reading packets */
#define PREFETCH_OFFSET 3

#define MAX_JUMBO_PKT_LEN  9600


#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT RTE_MAX_ETHPORTS
#define MAX_RX_QUEUE_PER_PORT 128
#define INVALID_QUEUE_ID 0xff


#define CMD_LINE_OPT_CONFIG               "config"
#define CMD_LINE_OPT_WORKER              "worker"
#define CMD_LINE_OPT_NO_NUMA            "no-numa"
#define CMD_LINE_OPT_ENABLE_KNI        "enable-kni"
#define CMD_LINE_OPT_ENABLE_JUMBO   "enable-jumbo"
#define CMD_LINE_OPT_ENABLE_IPSYNC  "enable-ipsync"

/**
 * rx lcore config
*/
struct ans_lcore_rx
{
  uint8_t port_id;
  uint8_t queue_id;
  uint8_t lcore_id;
} __rte_cache_aligned;

/**
 * worker lcore config
*/
struct ans_lcore_worker
{
  uint8_t lcore_id;
} __rte_cache_aligned;

/**
 * tx queue of port
*/
struct ans_tx_queue
{
  struct rte_mbuf *pkts[MAX_PKT_BURST];
  uint16_t pkts_nb;
  uint8_t queue_id;
}__rte_cache_aligned;

/**
 * rx queue of port
*/
struct ans_rx_queue
{
  uint8_t port_id;
  uint8_t queue_id;
} __rte_cache_aligned;


struct ans_user_config
{
    uint8_t socket_nb;
    uint8_t lcore_nb;
    uint32_t port_mask;
    uint8_t promiscuous_on;        /*  Ports set in promiscuous mode off by default. */
    uint8_t kni_on;                        /*  kni is disable by default. */
    uint8_t ipsync_on;                 /*  ipsync is disable by default. sync ip/route from kernel, if ipsync is enable, kni shall also enable */
    uint8_t numa_on ;                  /* NUMA is enabled by default. */
    uint8_t jumbo_frame_on;
    uint16_t max_rx_pkt_len;
    uint16_t rx_nb;
    struct ans_lcore_rx lcore_rx[RTE_MAX_LCORE];

    uint16_t worker_nb;
    struct ans_lcore_worker lcore_worker[RTE_MAX_LCORE];
};

struct ans_lcore_config
{
    uint16_t n_rx_queue;
    struct ans_rx_queue rx_queue[MAX_RX_QUEUE_PER_LCORE];

    uint16_t n_tx_port;
    uint16_t port_id[RTE_MAX_ETHPORTS];
    struct ans_tx_queue tx_queue[RTE_MAX_ETHPORTS];

    uint8_t lcore_role;
} __rte_cache_aligned;

#define MAX_NB_SOCKETS    8
#define MEMPOOL_CACHE_SIZE 256

#define MAX_MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)

/*
 * Configurable number of RX/TX ring descriptors
 */
#define ANS_RX_DESC_DEFAULT 1024
#define ANS_TX_DESC_DEFAULT 2048

#define TIMER_RESOLUTION_CYCLES 20000000ULL /* around 10ms at 2 Ghz */


#endif /* _ANS_MAIN_H_ */
