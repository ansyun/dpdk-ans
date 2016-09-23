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
 
#define _GNU_SOURCE             /* See feature_test_macros(7) */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <sched.h>

#include <rte_common.h>
#include <rte_vect.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_tailq.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_random.h>
#include <rte_timer.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_string_fns.h>

/* add by ans_team -- start */
#include <rte_spinlock.h>

#include "ans_init.h"
#include "ans_enet_intf.h"
#include "ans_ip_intf.h"

/* add by ans_team -- end */

#include "ans_main.h"
#include "ans_param.h"
#include "ans_kni.h"

static struct ans_user_config  ans_user_conf;
static struct ans_lcore_config ans_lcore_conf[RTE_MAX_LCORE];
static struct rte_mempool *ans_pktmbuf_pool[MAX_NB_SOCKETS];

static struct ans_lcore_params ans_lcore_params_default[] = 
{
	{0, 0, 0},
};

/* these config are used for kvm virtio nic */
#if 0

static  struct rte_eth_conf ans_port_conf = {
    .rxmode = {
        .max_rx_pkt_len = ETHER_MAX_LEN,
        .split_hdr_size = 0,
        .header_split   = 0, /**< Header Split disabled */
        .hw_ip_checksum = 0, /**< IP checksum offload disabled */
        .hw_vlan_filter = 0, /**< VLAN filtering disabled */
        .jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
        .hw_strip_crc   = 0, /**< CRC stripped by hardware */
    },
    .txmode = {
        .mq_mode = ETH_MQ_TX_NONE,
    },
};

static  struct rte_eth_rxconf ans_rx_conf = {
    .rx_thresh = {
        .pthresh = RX_PTHRESH,
        .hthresh = RX_HTHRESH,
        .wthresh = RX_WTHRESH,
    },
};

static  struct rte_eth_txconf ans_tx_conf = {
    .tx_thresh = {
        .pthresh = TX_PTHRESH,
        .hthresh = TX_HTHRESH,
        .wthresh = TX_WTHRESH,
    },
    .tx_free_thresh = 0, /* Use PMD default values */
    .tx_rs_thresh = 0, /* Use PMD default values */
    .txq_flags = ~ETH_TXQ_FLAGS_NOXSUMS  /* enable checksum for virtio */
};

#endif 


static struct rte_eth_rxconf ans_rx_conf = 
{
	.rx_thresh = 
       {
		.pthresh = RX_PTHRESH,
		.hthresh = RX_HTHRESH,
		.wthresh = RX_WTHRESH,
	},
	.rx_free_thresh = 32,
};

static struct rte_eth_txconf ans_tx_conf = 
{
	.tx_thresh = 
       {
		.pthresh = TX_PTHRESH,
		.hthresh = TX_HTHRESH,
		.wthresh = TX_WTHRESH,
	},
	.tx_free_thresh = 0, /* Use PMD default values */
	.tx_rs_thresh = 0, /* Use PMD default values */
	.txq_flags = ~ETH_TXQ_FLAGS_NOXSUMS /*= (ETH_TXQ_FLAGS_NOMULTSEGS |
			ETH_TXQ_FLAGS_NOVLANOFFL |
			ETH_TXQ_FLAGS_NOXSUMSCTP |
			ETH_TXQ_FLAGS_NOXSUMUDP |
			ETH_TXQ_FLAGS_NOXSUMTCP)*/

};


static struct rte_eth_conf ans_port_conf = 
{
	.rxmode = 
       {
		.mq_mode = ETH_MQ_RX_RSS,
		.max_rx_pkt_len = ETHER_MAX_LEN,
		.split_hdr_size = 0,
		.header_split   = 0,      /**< Header Split disabled */
		.hw_ip_checksum = 1, /**< IP/UDP/TCP checksum offload enable */
		.hw_vlan_filter = 0,     /**< VLAN filtering disabled */
		.jumbo_frame    = 0,   /**< Jumbo Frame Support disabled */
		.hw_strip_crc   = 0,    /**< CRC stripped by hardware */
	//	.enable_lro = 1,  /* enable LRO */
	},
	.rx_adv_conf = 
	{
		.rss_conf = 
             {
			.rss_hf = ETH_RSS_TCP|ETH_RSS_UDP,
		},
	},
	.txmode = 
	{
		.mq_mode = ETH_MQ_TX_NONE,
	},
};

/**********************************************************************
*@description:
* Check the link status of all ports in up to 9s, and print them finally
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static void ans_check_ports_link_status(uint8_t port_num, uint32_t port_mask)
{
    uint8_t check_interval = 100; /* 100ms */
    uint8_t max_check_time = 90; /* 9s (90 * 100ms) in total */
    uint8_t portid, count, all_ports_up, print_flag = 0;
    struct rte_eth_link link;

    printf("\nChecking link status ");

    for (count = 0; count <= max_check_time; count++) 
    {
    	all_ports_up = 1;
    	for (portid = 0; portid < port_num; portid++) 
       {
    		if ((port_mask & (1 << portid)) == 0)
    			continue;
            
    		memset(&link, 0, sizeof(link));
    		rte_eth_link_get_nowait(portid, &link);
    		/* print link status if flag set */
    		if (print_flag == 1) 
              {
    			if (link.link_status)
    				printf("Port %d Link Up - speed %u " "Mbps - %s\n", (uint8_t)portid,
    					(unsigned)link.link_speed, (link.link_duplex == ETH_LINK_FULL_DUPLEX) ? ("full-duplex") : ("half-duplex\n"));
    			else
    				printf("Port %d Link Down\n", (uint8_t)portid);
                
    			continue;
    		}
    		/* clear all_ports_up flag if any link down */
    		if (link.link_status == 0) 
              {
    			all_ports_up = 0;
    			break;
    		}
    	}
        
    	/* after finally printing all link status, get out */
    	if (print_flag == 1)
    		break;

    	if (all_ports_up == 0) 
       {
            printf(".");
            rte_delay_ms(check_interval);
    	}

    	/* set the print_flag if all ports up or timeout */
    	if (all_ports_up == 1 || count == (max_check_time - 1)) 
       {
            print_flag = 1;
            printf("done\n");
    	}
    }
}



/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static int ans_init_lcore_rx_queues(struct ans_user_config  *user_conf, struct ans_lcore_config *lcore_conf)
{
    uint16_t i, nb_rx_queue;
    uint8_t lcore;

    for (i = 0; i < user_conf->lcore_param_nb; ++i) 
    {
        lcore = user_conf->lcore_param[i].lcore_id;
        nb_rx_queue = lcore_conf[lcore].n_rx_queue;
        if (nb_rx_queue >= MAX_RX_QUEUE_PER_LCORE)
        {
            printf("error: too many queues (%u) for lcore: %u\n",
            	(unsigned)nb_rx_queue + 1, (unsigned)lcore);
            return -1;
        } 
        else 
        {
            lcore_conf[lcore].rx_queue_list[nb_rx_queue].port_id = user_conf->lcore_param[i].port_id;
            lcore_conf[lcore].rx_queue_list[nb_rx_queue].queue_id = user_conf->lcore_param[i].queue_id;
            lcore_conf[lcore].n_rx_queue++;
        }
    }
    return 0;
}

/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static int ans_init_mbuf_pool(unsigned nb_mbuf, struct ans_user_config  *user_conf)
{
    int socketid;
    unsigned lcore_id;
    char str[64];

    memset(ans_pktmbuf_pool, 0, sizeof(ans_pktmbuf_pool));

    for (lcore_id = 0; lcore_id < RTE_MAX_LCORE; lcore_id++) 
    {
        if (rte_lcore_is_enabled(lcore_id) == 0)
            continue;

        if (user_conf->numa_on)
            socketid = rte_lcore_to_socket_id(lcore_id);
        else
            socketid = 0;

        if (socketid >= MAX_NB_SOCKETS)
        {
            rte_exit(EXIT_FAILURE, "Socket %d of lcore %u is out of range %d\n", socketid, lcore_id, MAX_NB_SOCKETS);
        }
        
        if (ans_pktmbuf_pool[socketid] == NULL) 
        {
            snprintf(str, sizeof(str), "ans_mbuf_pool_%d", socketid);
            ans_pktmbuf_pool[socketid] = rte_pktmbuf_pool_create(str, nb_mbuf, MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, socketid);

            
            if (ans_pktmbuf_pool[socketid] == NULL)
                rte_exit(EXIT_FAILURE, "Cannot init mbuf pool on socket %d\n", socketid);
            else
                printf("Allocated mbuf pool on socket %d, mbuf number: %d \n", socketid, nb_mbuf);

            user_conf->socket_nb++;

        }
    }
    return 0;
}

/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static uint8_t ans_get_port_rx_queues_nb(const uint8_t port, struct ans_user_config  *user_conf)
{
    int queue = -1;
    uint16_t i;

    for (i = 0; i < user_conf->lcore_param_nb; ++i) 
    {
        if (user_conf->lcore_param[i].port_id == port && user_conf->lcore_param[i].queue_id > queue)
        	queue = user_conf->lcore_param[i].queue_id;
    }
    return (uint8_t)(++queue);
}

/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static int ans_init_ports(unsigned short nb_ports, struct ans_user_config  *user_conf, struct ans_lcore_config *lcore_conf)
{
    int ret;
    uint8_t portid; 
    uint16_t queueid;
    unsigned lcore_id;
    uint8_t nb_rx_queue =0;
    uint8_t max_rx_queue =0;
    uint8_t queue, socketid;
    uint32_t n_tx_queue, nb_lcores, nb_mbuf;
    struct ether_addr eth_addr;
    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf *txconf;
    struct rte_eth_rxconf *rxconf;

    nb_lcores = rte_lcore_count();
    n_tx_queue = nb_lcores;
    if (n_tx_queue > MAX_TX_QUEUE_PER_PORT)
    	n_tx_queue = MAX_TX_QUEUE_PER_PORT;

    printf("\nStart to Init port \n" );

    /* initialize all ports */
    for (portid = 0; portid < nb_ports; portid++) 
    {
        /* skip ports that are not enabled */
        if ((user_conf->port_mask & (1 << portid)) == 0) 
        {
            printf("\nSkipping disabled port %d\n", portid);
            continue;
        }

        /* init port */
        printf("\t port %d:  \n", portid );
        rte_eth_dev_info_get(portid, &dev_info);
        printf("\t port name %s:  \n", dev_info.driver_name );
        printf("\t max_rx_queues %d: max_tx_queues:%d \n", dev_info.max_rx_queues, dev_info.max_tx_queues);
        printf("\t rx_offload_capa %d: tx_offload_capa:%d \n", dev_info.rx_offload_capa, dev_info.tx_offload_capa);

        nb_rx_queue = ans_get_port_rx_queues_nb(portid, user_conf);

        if(max_rx_queue < nb_rx_queue)
            max_rx_queue = nb_rx_queue;
        
        printf("\t Creating queues: rx queue number=%d tx queue number=%u... \n", nb_rx_queue, (unsigned)n_tx_queue );

        ret = rte_eth_dev_configure(portid, nb_rx_queue, (uint16_t)n_tx_queue, &ans_port_conf);
        if (ret < 0)
        	rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%d\n", ret, portid);

        rte_eth_macaddr_get(portid, &eth_addr);

        printf ("\t MAC Address:%02X:%02X:%02X:%02X:%02X:%02X \n", 
        	eth_addr.addr_bytes[0], eth_addr.addr_bytes[1],
        	eth_addr.addr_bytes[2], eth_addr.addr_bytes[3],
        	eth_addr.addr_bytes[4], eth_addr.addr_bytes[5]);

        /* init one TX queue per couple (lcore,port) */
        queueid = 0;
        for (lcore_id = 0; lcore_id < RTE_MAX_LCORE; lcore_id++)
        {
            if (rte_lcore_is_enabled(lcore_id) == 0)
            	continue;

            if (user_conf->numa_on)
            	socketid = (uint8_t)rte_lcore_to_socket_id(lcore_id);
            else
            	socketid = 0;

            rte_eth_dev_info_get(portid, &dev_info);
            txconf = &dev_info.default_txconf;
            if (ans_port_conf.rxmode.jumbo_frame)
                txconf->txq_flags = 0;

            printf("\t Deault-- tx pthresh:%d, tx hthresh:%d, tx wthresh:%d, txq_flags:0x%x \n", txconf->tx_thresh.pthresh,
                txconf->tx_thresh.hthresh, txconf->tx_thresh.wthresh, txconf->txq_flags);

            /* for igb driver, shall set it as 16 to improve performance */
         //   txconf->tx_thresh.wthresh = 16;
            txconf = &ans_tx_conf;
            
            printf("\t lcore id:%u, tx queue id:%d, socket id:%d \n", lcore_id, queueid, socketid);
            printf("\t Conf-- tx pthresh:%d, tx hthresh:%d, tx wthresh:%d, txq_flags:0x%x \n", txconf->tx_thresh.pthresh,
                txconf->tx_thresh.hthresh, txconf->tx_thresh.wthresh, txconf->txq_flags);
         
            ret = rte_eth_tx_queue_setup(portid, queueid, ANS_TX_DESC_DEFAULT, socketid, txconf);
            if (ret < 0)
            	rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup: err=%d, " "port=%d\n", ret, portid);

            lcore_conf[lcore_id].tx_queue_id[portid] = queueid;
            
            queueid++;
        }
        
        printf("\n");

    }

    nb_mbuf = RTE_MAX((nb_ports*nb_rx_queue*ANS_RX_DESC_DEFAULT +	
				nb_ports*nb_lcores*MAX_PKT_BURST +					
				nb_ports*n_tx_queue*ANS_TX_DESC_DEFAULT +	
				nb_lcores*MEMPOOL_CACHE_SIZE), MAX_MBUF_NB);
				
    /* init memory */
    ret = ans_init_mbuf_pool(nb_mbuf, user_conf);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "init_mem failed\n");

    for (lcore_id = 0; lcore_id < RTE_MAX_LCORE; lcore_id++) 
    {
        if (rte_lcore_is_enabled(lcore_id) == 0)
            continue;
        
        printf("\nInitializing rx queues on lcore %u ... \n", lcore_id );

        /* init RX queues */
        for(queue = 0; queue < lcore_conf[lcore_id].n_rx_queue; ++queue) 
        {
            portid = lcore_conf[lcore_id].rx_queue_list[queue].port_id;
            queueid = lcore_conf[lcore_id].rx_queue_list[queue].queue_id;

            if (user_conf->numa_on)
                socketid = (uint8_t)rte_lcore_to_socket_id(lcore_id);
            else
                socketid = 0;

            rte_eth_dev_info_get(portid, &dev_info);
            rxconf = &dev_info.default_rxconf;
            
            printf("Default-- rx pthresh:%d, rx hthresh:%d, rx wthresh:%d \n", rxconf->rx_thresh.pthresh,
                rxconf->rx_thresh.hthresh, rxconf->rx_thresh.wthresh);

            rxconf = &ans_rx_conf;

            printf("port id:%d, rx queue id: %d, socket id:%d \n", portid, queueid, socketid);

            printf("Conf-- rx pthresh:%d, rx hthresh:%d, rx wthresh:%d \n", rxconf->rx_thresh.pthresh,
                rxconf->rx_thresh.hthresh, rxconf->rx_thresh.wthresh);

            /* use NIC default rx conf */
            ret = rte_eth_rx_queue_setup(portid, queueid, ANS_RX_DESC_DEFAULT, socketid, rxconf, ans_pktmbuf_pool[socketid]);
            if (ret < 0)
                rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup: err=%d," "port=%d\n", ret, portid);
        }
    }

    return 0;
}



/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static int ans_start_ports(unsigned short nb_ports, struct ans_user_config  *user_conf)
{
    int ret;
    uint8_t portid;

    /* start ports */
    for (portid = 0; portid < nb_ports; portid++) 
    {
    	if ((user_conf->port_mask & (1 << portid)) == 0) 
        {
    		continue;
    	}
    	/* Start device */
    	ret = rte_eth_dev_start(portid);
    	if (ret < 0)
    		rte_exit(EXIT_FAILURE, "rte_eth_dev_start: err=%d, port=%d\n", ret, portid);

    	/*
    	 * If enabled, put device in promiscuous mode.
    	 * This allows IO forwarding mode to forward packets
    	 * to itself through 2 cross-connected  ports of the
    	 * target machine.
    	 */
    	if (user_conf->promiscuous_on)
    		rte_eth_promiscuous_enable(portid);
    }

    ans_check_ports_link_status((uint8_t)nb_ports, user_conf->port_mask);

    return 0;
}
/**********************************************************************
*@description:
* Send burst of packets on an output interface
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static inline int ans_send_burst(struct ans_lcore_config *qconf, uint16_t n, uint8_t port)
{
    struct rte_mbuf **m_table;
    int ret;
    uint16_t queueid;

    queueid = qconf->tx_queue_id[port];
    m_table = (struct rte_mbuf **)qconf->tx_mbufs[port].m_table;

    ret = rte_eth_tx_burst(port, queueid, m_table, n);
    if (unlikely(ret < n)) 
    {
        do 
        {
            rte_pktmbuf_free(m_table[ret]);
        } while (++ret < n);
    }

    return 0;
}

/**********************************************************************
*@description:
* Enqueue a single packet, and send burst if queue is filled
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static inline int ans_send_single_packet(struct rte_mbuf *m, uint8_t port)
{
    uint32_t lcore_id;
    uint16_t len;
    struct ans_lcore_config *qconf;

    lcore_id = rte_lcore_id();

    qconf = &ans_lcore_conf[lcore_id];
    len = qconf->tx_mbufs[port].len;
    qconf->tx_mbufs[port].m_table[len] = m;
    len++;

    /* enough pkts to be sent */
    if (unlikely(len == MAX_TX_BURST))
    {
        ans_send_burst(qconf, MAX_TX_BURST, port);
        len = 0;
    }

    qconf->tx_mbufs[port].len = len;
    return 0;
}

/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static void ans_init_timer()
{
    /* init RTE timer library */
    rte_timer_subsystem_init();
    return;
}

/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static inline void ans_to_linux(unsigned port_id, struct rte_mbuf *m)
{
    int ret = 0;

    if(ans_user_conf.kni_on != 1)
    {
        rte_pktmbuf_free(m);
        return;
    }

    ret = ans_kni_sendpkt_burst(&m, 1, port_id);
    if(ret != 0 )
        rte_pktmbuf_free(m);
    
    return;
}

/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
static int ans_main_loop(__attribute__((unused)) void *dummy)
{
    int ret;
    unsigned nb_ports;
    int i, j, nb_rx;
    unsigned lcore_id;
    uint64_t prev_tsc, diff_tsc, cur_tsc;
    uint8_t portid, queueid;
    struct ans_lcore_config *qconf;
    uint64_t timer_prev_tsc = 0, timer_cur_tsc, timer_diff_tsc;
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];

    const uint64_t drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S * BURST_TX_DRAIN_US;

    prev_tsc = 0;

    lcore_id = rte_lcore_id();
    qconf = &ans_lcore_conf[lcore_id];

    if (qconf->n_rx_queue == 0) 
    {
        RTE_LOG(INFO, USER8, "lcore %u has nothing to do\n", lcore_id);
        return 0;
    }

    RTE_LOG(INFO, USER8, "main loop on lcore %u\n", lcore_id);

    for (i = 0; i < qconf->n_rx_queue; i++)
    {
        portid = qconf->rx_queue_list[i].port_id;
        queueid = qconf->rx_queue_list[i].queue_id;
        RTE_LOG(INFO, USER8, " -- lcoreid=%u portid=%hhu rxqueueid=%hhu\n", lcore_id, portid, queueid);
    }

    nb_ports = rte_eth_dev_count();
    printf("nb ports %d hz: %ld \n", nb_ports, rte_get_tsc_hz());

    while (1)
    {

        cur_tsc = rte_rdtsc();

        /* add by ans_team ---start */
        ans_message_handle(lcore_id, cur_tsc);
        /* add by ans_team ---end */


        /*
         * Call the timer handler on each core: as we don't
         * need a very precise timer, so only call
         * rte_timer_manage() every ~10ms (at 2Ghz). In a real
         * application, this will enhance performances as
         * reading the HPET timer is not efficient.
         */
        timer_cur_tsc = rte_rdtsc();
        timer_diff_tsc = timer_cur_tsc - timer_prev_tsc;
        if (timer_diff_tsc > TIMER_RESOLUTION_CYCLES) 
        {
        	rte_timer_manage();
        	timer_prev_tsc = timer_cur_tsc;
        }

        /*
        * TX burst queue drain
        */
        diff_tsc = cur_tsc - prev_tsc;
        if (unlikely(diff_tsc > drain_tsc)) 
        {

            /*
            * This could be optimized (use queueid instead of
            * portid), but it is not called so often
            */
            for (portid = 0; portid < nb_ports; portid++) 
            {
                if (qconf->tx_mbufs[portid].len == 0)
                    continue;
                
                ans_send_burst(qconf, qconf->tx_mbufs[portid].len,  portid);
                
                qconf->tx_mbufs[portid].len = 0;
            }

            prev_tsc = cur_tsc;
        }

        /*
        * Read packet from RX queues
        */
        for (i = 0; i < qconf->n_rx_queue; ++i) 
        {
            portid = qconf->rx_queue_list[i].port_id;
            queueid = qconf->rx_queue_list[i].queue_id;
            nb_rx = rte_eth_rx_burst(portid, queueid, pkts_burst, MAX_PKT_BURST);
            if (nb_rx == 0)
                continue;

            /* Prefetch first packets */
            for (j = 0; j < PREFETCH_OFFSET && j < nb_rx; j++) 
            {
                rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[j], void *));
            }

            /* Prefetch and forward already prefetched packets */
            for (j = 0; j < (nb_rx - PREFETCH_OFFSET); j++) 
            {
                rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[j + PREFETCH_OFFSET], void *));

                /* add by ans_team ---start */

                ret = ans_packet_handle(pkts_burst[j], portid);
                if(ret == ANS_MBUF_CONTINUE)
                    ans_to_linux(portid, pkts_burst[j]);
      
                /* add by ans_team ---end */
            }

            /* Forward remaining prefetched packets */
            for (; j < nb_rx; j++)
            {

                /* add by ans_team ---start */

                ret = ans_packet_handle(pkts_burst[j], portid);
               if(ret == ANS_MBUF_CONTINUE)
                   ans_to_linux(portid, pkts_burst[j]);
          
                /* add by ans_team ---end */
            }
        }

        /* to support KNI, at 2014-12-15 */
        if(ans_user_conf.kni_on == 1)
            ans_kni_main();
        
    }
}


/**********************************************************************
*@description:
* 
*
*@parameters:
* [in]: 
* [in]: 
*
*@return values: 
*
**********************************************************************/
int main(int argc, char **argv)
{
    int i;
    int ret;
    unsigned nb_ports;
    unsigned lcore_id;
    struct ans_init_config init_conf;
    int s;
    
    memset(&ans_user_conf, 0, sizeof(ans_user_conf));
    memset(ans_lcore_conf, 0, sizeof(ans_lcore_conf));

    ans_user_conf.numa_on = 1;
    ans_user_conf.lcore_param_nb = sizeof(ans_lcore_params_default) / sizeof(ans_lcore_params_default[0]);
    rte_memcpy(ans_user_conf.lcore_param, ans_lcore_params_default, sizeof(ans_lcore_params_default));

    CPU_ZERO(&init_conf.cpu_set);

    s = sched_getaffinity(0, sizeof(cpu_set_t), &init_conf.cpu_set);
    if (s != 0) 
    {
    	printf("ANS sched_getaffinity failed:%s\n", strerror(errno));
    	return -1;
    }

    /* init EAL */   

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "ODP:Invalid EAL parameters\n");

    argc -= ret;
    argv += ret;
    
    /* parse application arguments (after the EAL ones) */
    ret = ans_parse_args(argc, argv, &ans_user_conf);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "Invalid ODP parameters\n");


    if(ans_user_conf.jumbo_frame_on)
    {
        ans_port_conf.rxmode.jumbo_frame = 1;
        ans_port_conf.rxmode.max_rx_pkt_len = ans_user_conf.max_rx_pkt_len;
    }

    ans_user_conf.lcore_nb = rte_lcore_count();

    if (ans_check_lcore_params(&ans_user_conf) < 0)
        rte_exit(EXIT_FAILURE, "check_lcore_params failed\n");


    ret = ans_init_lcore_rx_queues(&ans_user_conf, ans_lcore_conf);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "init_lcore_rx_queues failed\n");

    

    nb_ports = rte_eth_dev_count();
    if (nb_ports > RTE_MAX_ETHPORTS)
    	nb_ports = RTE_MAX_ETHPORTS;


    ret = ans_check_port_config(nb_ports, &ans_user_conf);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "check_port_config failed\n");


    ret = ans_init_ports(nb_ports, &ans_user_conf, ans_lcore_conf);
    if (ret < 0)
    	rte_exit(EXIT_FAILURE, "Init ports failed\n");


    /* add by ans_team: support KNI interface at 2014-12-15 */
    if(ans_user_conf.kni_on == 1)
        ans_kni_config(&ans_user_conf, ans_pktmbuf_pool);

    
    /* add by ans_team ---start */
    ans_init_timer();
    printf("core mask: %x, sockets number:%d, lcore number:%d \n", ans_user_conf.lcore_mask, ans_user_conf.socket_nb, ans_user_conf.lcore_nb);

    printf("start to init ans \n");
    init_conf.max_sock_conn = ans_user_conf.lcore_nb * 128 * 1024;

    init_conf.lcore_mask = ans_user_conf.lcore_mask;
    for(i = 0 ; i < MAX_NB_SOCKETS; i++)
    {
        init_conf.pktmbuf_pool[i] = ans_pktmbuf_pool[i];
    }

    ret = ans_initialize(&init_conf);
    if (ret != 0)
   	rte_exit(EXIT_FAILURE, "Init ans failed \n");

  
    ans_register(ans_send_single_packet);
    /* add by ans_team ---end */

    /* add by ans_team for testing ---start */
    uint8_t ifname[16];
    int portid;
    struct ether_addr eth_addr;

    for(portid= 0; portid < nb_ports; portid++)
    {
        /* skip ports that are not enabled */
        if ((ans_user_conf.port_mask & (1 << portid)) == 0) 
        {
            printf("\nSkipping disabled port %d\n", portid);
            continue;
        }
    
        memset(ifname, 0, sizeof(ifname));

        sprintf(ifname, "eth%d", portid);

        printf("add %s device\r\n", ifname);
        rte_eth_macaddr_get(portid, &eth_addr);

        ans_intf_add(portid,  ifname, &eth_addr);

        int ip_addr = 0x0200000a;
        ip_addr += portid << 16;

        printf("add IP %x on device %s \n", ip_addr, ifname);
        ans_intf_add_ipaddr((caddr_t)ifname, ip_addr, 0x00ffffff);  
    }
    /* add by ans_team ---end */

    /* add by ans_team for testing ---start */

    printf("Show interface \n");
    ans_intf_show();

    int route_ret = 0;
    printf("add static route \r\n");

    route_ret = ans_add_route(0x00000a0a, 1, 0x0500000a, 0x00ffffff, ANS_IP_RTF_GATEWAY);

    ans_route_show_all();

    printf("\n");
    /* add by ans_team ---end */

    ans_start_ports(nb_ports, &ans_user_conf);

    /* launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(ans_main_loop, NULL, CALL_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id){
    	if (rte_eal_wait_lcore(lcore_id) < 0)
    		return -1;
    }
}
