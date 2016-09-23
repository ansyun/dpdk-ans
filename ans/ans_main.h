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

#ifndef _ANS_MAIN_H_
#define _ANS_MAIN_H_

/*
 * RX and TX Prefetch, Host, and Write-back threshold values should be
 * carefully set for optimal performance. Consult the network
 * controller's datasheet and supporting DPDK documentation for guidance
 * on how these parameters should be set.
 */
#define RX_PTHRESH 8 /**< Default values of RX prefetch threshold reg. */
#define RX_HTHRESH 8 /**< Default values of RX host threshold reg. */
#define RX_WTHRESH 4 /**< Default values of RX write-back threshold reg. */

/*
 * These default values are optimized for use with the Intel(R) 82599 10 GbE
 * Controller and the DPDK ixgbe PMD. Consider using other values for other
 * network controllers and/or network drivers.
 */
#define TX_PTHRESH 36 /**< Default values of TX prefetch threshold reg. */
#define TX_HTHRESH 0  /**< Default values of TX host threshold reg. */
#define TX_WTHRESH 0  /**< Default values of TX write-back threshold reg. */

#define MAX_PKT_BURST     32
#define MAX_TX_BURST      16         /* set tx burst as 1 for lower packet latency */
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */

#define MAX_MBUF_NB     (1024 * 16)


/* Configure how many packets ahead to prefetch, when reading packets */
#define PREFETCH_OFFSET	3

#define MAX_JUMBO_PKT_LEN  9600


#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT RTE_MAX_ETHPORTS
#define MAX_RX_QUEUE_PER_PORT 128

#define CMD_LINE_OPT_CONFIG              "config"
#define CMD_LINE_OPT_NO_NUMA          "no-numa"
#define CMD_LINE_OPT_ENABLE_KNI      "enable-kni"
#define CMD_LINE_OPT_ENABLE_JUMBO "enable-jumbo"

#define MAX_LCORE_PARAMS 512

struct ans_lcore_params 
{
	uint8_t port_id;
	uint8_t queue_id;
	uint8_t lcore_id;
} __rte_cache_aligned;


struct ans_mbuf_table 
{
	uint16_t len;
	struct rte_mbuf *m_table[MAX_PKT_BURST];
};

struct ans_lcore_rx_queue 
{
	uint8_t port_id;
	uint8_t queue_id;
} __rte_cache_aligned;


struct ans_user_config
{
    uint8_t socket_nb;
    uint8_t lcore_nb;
    uint32_t lcore_mask;
    uint32_t port_mask;
    uint8_t promiscuous_on;        /*  Ports set in promiscuous mode off by default. */
    uint8_t kni_on;                        /*  kni is disable by default. */
    uint8_t numa_on ;                  /* NUMA is enabled by default. */
    uint8_t jumbo_frame_on;
    uint16_t max_rx_pkt_len;
    uint16_t lcore_param_nb;
    struct ans_lcore_params lcore_param[MAX_LCORE_PARAMS];
};

struct ans_lcore_config 
{
	uint16_t n_rx_queue;
	struct ans_lcore_rx_queue rx_queue_list[MAX_RX_QUEUE_PER_LCORE];
	uint16_t tx_queue_id[RTE_MAX_ETHPORTS];
	struct ans_mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
} __rte_cache_aligned;

#define MAX_NB_SOCKETS    8
#define MEMPOOL_CACHE_SIZE 256

#define MAX_MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)

/*
 * Configurable number of RX/TX ring descriptors
 */
#define ANS_RX_DESC_DEFAULT 128
#define ANS_TX_DESC_DEFAULT 512

#define TIMER_RESOLUTION_CYCLES 20000000ULL /* around 10ms at 2 Ghz */


#endif /* _ANS_MAIN_H_ */
