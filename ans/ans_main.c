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
#include <signal.h>

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
#include <rte_meter.h>
#include <rte_spinlock.h>
#ifdef RTE_LIBRTE_PDUMP
#include <rte_pdump.h>
#endif

#include "ans_init.h"
#include "ans_ip_intf.h"

#include "ans_main.h"
#include "ans_param.h"
#include "ans_kni.h"

static struct ans_user_config  ans_user_conf;
static struct ans_lcore_config g_lcore_conf[ANS_MAX_NB_LCORE];
static struct rte_mempool *ans_pktmbuf_pool[ANS_MAX_NB_SOCKETS];
static uint8_t ans_stopped = 0;

static struct rte_eth_conf ans_port_conf =
{
  .rxmode =
   {
    .mq_mode = ETH_MQ_RX_RSS,
    .max_rx_pkt_len = ETHER_MAX_LEN,
    .split_hdr_size = 0,
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
static void ans_check_ports_link_status(uint32_t port_mask)
{
    uint8_t check_interval = 100; /* 100ms */
    uint8_t max_check_time = 90; /* 9s (90 * 100ms) in total */
    uint8_t portid, count, all_ports_up, print_flag = 0;
    struct rte_eth_link link;

    printf("\nChecking link status ");

    for (count = 0; count <= max_check_time; count++)
    {
        if (ans_stopped)
            return;

        all_ports_up = 1;
        RTE_ETH_FOREACH_DEV(portid)
        {
            if (ans_stopped)
                return;
       
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
static int ans_init_lcore_config(struct ans_user_config  *user_conf, struct ans_lcore_config *lcore_conf)
{
    uint16_t i, nb_rx_queue;
    uint8_t lcore;

    for (i = 0; i < user_conf->rx_nb; ++i)
    {
        lcore = user_conf->lcore_rx[i].lcore_id;
        nb_rx_queue = lcore_conf[lcore].n_rx_queue;
        if (nb_rx_queue >= MAX_RX_QUEUE_PER_LCORE)
        {
            printf("error: too many queues (%u) for lcore: %u\n",
              (unsigned)nb_rx_queue + 1, (unsigned)lcore);
            return -1;
        }
        else
        {
            lcore_conf[lcore].rx_queue[nb_rx_queue].port_id = user_conf->lcore_rx[i].port_id;
            lcore_conf[lcore].rx_queue[nb_rx_queue].queue_id = user_conf->lcore_rx[i].queue_id;
            lcore_conf[lcore].n_rx_queue++;
            lcore_conf[lcore].lcore_role |= ANS_LCORE_ROLE_RX;
        }
    }

    for (i = 0; i < user_conf->worker_nb; ++i)
    {
        lcore = user_conf->lcore_worker[i].lcore_id;
        lcore_conf[lcore].lcore_role |= ANS_LCORE_ROLE_WORKER;
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
static void ans_get_lcore_config(struct ans_lcore_config *lcore_conf, struct ans_init_config *init_conf)
{
    int i;
    uint8_t lcore_nb = 0; 

    for (i = 0; i < ANS_MAX_NB_LCORE; i++)
    {
        if(lcore_conf[i].lcore_role != ANS_LCORE_ROLE_DISABLE)
        {
            init_conf->lcore[lcore_nb].lcore_id = i;
            init_conf->lcore[lcore_nb].lcore_role = lcore_conf[i].lcore_role;
            lcore_nb++;
        }
    }

    init_conf->lcore_nb = lcore_nb;
    
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
static int ans_init_mbuf_pool(unsigned nb_mbuf, struct ans_user_config  *user_conf)
{
    int socketid;
    unsigned lcore_id;
    char str[64];

    memset(ans_pktmbuf_pool, 0, sizeof(ans_pktmbuf_pool));

    for (lcore_id = 0; lcore_id < ANS_MAX_NB_LCORE; lcore_id++)
    {
        if (rte_lcore_is_enabled(lcore_id) == 0)
            continue;

        if (user_conf->numa_on)
            socketid = rte_lcore_to_socket_id(lcore_id);
        else
            socketid = 0;

        if (socketid >= ANS_MAX_NB_SOCKETS)
        {
            rte_exit(EXIT_FAILURE, "Socket %d of lcore %u is out of range %d\n", socketid, lcore_id, ANS_MAX_NB_SOCKETS);
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

    for (i = 0; i < user_conf->rx_nb; ++i)
    {
        if (user_conf->lcore_rx[i].port_id == port && user_conf->lcore_rx[i].queue_id > queue)
          queue = user_conf->lcore_rx[i].queue_id;
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
static void ans_get_port_queue(const uint8_t port, struct ans_port_queue *port_queue, struct ans_lcore_config *lcore_conf)
{
    uint16_t i;
    uint8_t lcore_id;
    struct ans_lcore_config *lcore_queue;

    port_queue->rxq_nb = 0;
    port_queue->txq_nb = 0;
    
    for(lcore_id = 0; lcore_id < ANS_MAX_NB_LCORE; lcore_id++)
    {
        if (rte_lcore_is_enabled(lcore_id) == 0)
            continue;

        lcore_queue = &lcore_conf[lcore_id];

        /* get rx queue */
        for(i = 0; i < lcore_queue->n_rx_queue; i++)
        {
            if(lcore_queue->rx_queue[i].port_id == port)
            {
                port_queue->rx_qmapping[port_queue->rxq_nb].lcore_id  = lcore_id;
                port_queue->rx_qmapping[port_queue->rxq_nb].queue_id = lcore_queue->rx_queue[i].queue_id;
                port_queue->rxq_nb++;
            }
        }

        /* get tx queue */
        if(lcore_queue->tx_queue[port].queue_id != INVALID_QUEUE_ID)
        {
            port_queue->tx_qmapping[port_queue->txq_nb].lcore_id = lcore_id;
            port_queue->tx_qmapping[port_queue->txq_nb].queue_id = lcore_queue->tx_queue[port].queue_id;
            port_queue->txq_nb++;
        }

    }

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
static void ans_set_port_offload(struct rte_eth_dev_info *dev_info, struct rte_eth_conf *port_conf)
{
    /***TX offload***/
    port_conf->txmode.offloads = 0;
    if(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_IPV4_CKSUM)
    {
        port_conf->txmode.offloads |= DEV_TX_OFFLOAD_IPV4_CKSUM;
    }

    if(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_UDP_CKSUM)
    {
        port_conf->txmode.offloads |= DEV_TX_OFFLOAD_UDP_CKSUM;
    }

    if(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_TCP_CKSUM)
    {
        port_conf->txmode.offloads |= DEV_TX_OFFLOAD_TCP_CKSUM;
    }   

    if(dev_info->tx_offload_capa & DEV_TX_OFFLOAD_TCP_TSO)
    {
        port_conf->txmode.offloads |= DEV_TX_OFFLOAD_TCP_TSO;
    }  

    
    /***RX offload***/
    port_conf->rxmode.offloads = 0;
    if(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_VLAN_STRIP)
    {
        port_conf->rxmode.offloads |= DEV_RX_OFFLOAD_VLAN_STRIP;
    }

    if(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_IPV4_CKSUM)
    {
        port_conf->rxmode.offloads |= DEV_RX_OFFLOAD_IPV4_CKSUM;
    }

    if(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_UDP_CKSUM)
    {
        port_conf->rxmode.offloads |= DEV_RX_OFFLOAD_UDP_CKSUM;
    }

    if(dev_info->rx_offload_capa & DEV_RX_OFFLOAD_TCP_CKSUM)
    {
        port_conf->rxmode.offloads |= DEV_RX_OFFLOAD_TCP_CKSUM;
    }

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
static int ans_init_ports(struct ans_user_config  *user_conf, struct ans_lcore_config *lcore_conf)
{
    int ret;
    uint8_t portid;
    uint16_t queueid;
    unsigned lcore_id;
    uint8_t nb_rx_queue =0;
    uint8_t queue, socketid;
    uint32_t n_tx_queue, nb_lcores;
    struct ether_addr eth_addr;
    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf *txconf;
    struct rte_eth_rxconf *rxconf;
    struct rte_eth_conf port_conf;
    struct rte_eth_dev *dev;
    struct rte_eth_conf *conf;
            
    nb_lcores = rte_lcore_count();
    n_tx_queue = nb_lcores;
    if (n_tx_queue > MAX_TX_QUEUE_PER_PORT)
        n_tx_queue = MAX_TX_QUEUE_PER_PORT;

    printf("\nStart to Init port \n" );

    /* initialize all ports */
    RTE_ETH_FOREACH_DEV(portid)
    {
        /* skip ports that are not enabled */
        if ((user_conf->port_mask & (1 << portid)) == 0)
        {
            printf("\nSkipping disabled port %d\n", portid);
            continue;
        }

        port_conf = ans_port_conf;
        
        /* init port */
        printf("\t port %d:  \n", portid );
        rte_eth_dev_info_get(portid, &dev_info);
        
        printf("\t port name %s:  \n", dev_info.driver_name );
        printf("\t max_rx_queues %d: max_tx_queues:%d \n", dev_info.max_rx_queues, dev_info.max_tx_queues);
        printf("\t rx_offload_capa 0x%lx: tx_offload_capa:0x%lx \n", dev_info.rx_offload_capa, dev_info.tx_offload_capa);

        ans_set_port_offload(&dev_info, &port_conf);
        
        nb_rx_queue = ans_get_port_rx_queues_nb(portid, user_conf);

        port_conf.rx_adv_conf.rss_conf.rss_hf &= dev_info.flow_type_rss_offloads;

        printf("\t Creating queues: rx queue number=%d tx queue number=%u... \n", nb_rx_queue, (unsigned)n_tx_queue );

        ret = rte_eth_dev_configure(portid, nb_rx_queue, (uint16_t)n_tx_queue, &port_conf);
        if (ret < 0)
          rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%d\n", ret, portid);

        rte_eth_macaddr_get(portid, &eth_addr);

        printf ("\t MAC Address:%02X:%02X:%02X:%02X:%02X:%02X \n",
          eth_addr.addr_bytes[0], eth_addr.addr_bytes[1],
          eth_addr.addr_bytes[2], eth_addr.addr_bytes[3],
          eth_addr.addr_bytes[4], eth_addr.addr_bytes[5]);

        /* init one TX queue per couple (lcore,port) */
        queueid = 0;
        for (lcore_id = 0; lcore_id < ANS_MAX_NB_LCORE; lcore_id++)
        {
            if (rte_lcore_is_enabled(lcore_id) == 0)
            {
                /* if lcore isn't enable, set as a invalid queue id */
                lcore_conf[lcore_id].tx_queue[portid].queue_id  = INVALID_QUEUE_ID;
                continue;
            }
            
            if (user_conf->numa_on)
              socketid = (uint8_t)rte_lcore_to_socket_id(lcore_id);
            else
              socketid = 0;

            rte_eth_dev_info_get(portid, &dev_info);
            txconf = &dev_info.default_txconf;

            printf("\t Deault-- tx pthresh:%d, tx hthresh:%d, tx wthresh:%d, tx offloads:0x%lx \n", txconf->tx_thresh.pthresh,
                txconf->tx_thresh.hthresh, txconf->tx_thresh.wthresh, txconf->offloads);

            txconf->offloads = port_conf.txmode.offloads;

            printf("\t lcore id:%u, tx queue id:%d, socket id:%d \n", lcore_id, queueid, socketid);
            printf("\t Conf-- tx pthresh:%d, tx hthresh:%d, tx wthresh:%d, tx offloads:0x%lx \n", txconf->tx_thresh.pthresh,
                txconf->tx_thresh.hthresh, txconf->tx_thresh.wthresh, txconf->offloads);

            ret = rte_eth_tx_queue_setup(portid, queueid, ANS_TX_DESC_DEFAULT, socketid, txconf);
            if (ret < 0)
              rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup: err=%d, " "port=%d\n", ret, portid);


            struct ans_lcore_config * qconf = &lcore_conf[lcore_id];
            qconf->tx_queue[portid].queue_id = queueid;
            queueid++;

            qconf->port_id[qconf->n_tx_port] = portid;
            qconf->n_tx_port++;
            
        }

        printf("\n");

    }

    /* init memory */
    ret = ans_init_mbuf_pool(MAX_MBUF_NB * nb_lcores, user_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "init_mem failed\n");

    for (lcore_id = 0; lcore_id < ANS_MAX_NB_LCORE; lcore_id++)
    {
        if (rte_lcore_is_enabled(lcore_id) == 0)
            continue;

        printf("\nInitializing rx queues on lcore %u ... \n", lcore_id );

        /* init RX queues */
        for(queue = 0; queue < lcore_conf[lcore_id].n_rx_queue; ++queue)
        {
            portid = lcore_conf[lcore_id].rx_queue[queue].port_id;
            queueid = lcore_conf[lcore_id].rx_queue[queue].queue_id;
            
            dev = &rte_eth_devices[portid];
            conf = &dev->data->dev_conf;
            
            if (user_conf->numa_on)
                socketid = (uint8_t)rte_lcore_to_socket_id(lcore_id);
            else
                socketid = 0;

            rte_eth_dev_info_get(portid, &dev_info);
            rxconf = &dev_info.default_rxconf;

            printf("Default-- rx pthresh:%d, rx hthresh:%d, rx wthresh:%d, rx offloads:0x%lx  \n", rxconf->rx_thresh.pthresh,
                rxconf->rx_thresh.hthresh, rxconf->rx_thresh.wthresh, rxconf->offloads);

            /* use default rx conf */
            rxconf->offloads = conf->rxmode.offloads;

            printf("Conf-- rx pthresh:%d, rx hthresh:%d, rx wthresh:%d, rx offloads:0x%lx  \n", rxconf->rx_thresh.pthresh,
                rxconf->rx_thresh.hthresh, rxconf->rx_thresh.wthresh, rxconf->offloads);
         
            printf("port id:%d, rx queue id: %d, socket id:%d \n", portid, queueid, socketid);

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
static int ans_start_ports(struct ans_user_config  *user_conf)
{
    int ret;
    uint8_t portid;

    /* start ports */
    RTE_ETH_FOREACH_DEV(portid)
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

    ans_check_ports_link_status(user_conf->port_mask);

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
static int ans_stop_ports(struct ans_user_config  *user_conf)
{
    int portid;
    /* stop ports */
    RTE_ETH_FOREACH_DEV(portid)
    {
        if ((user_conf->port_mask & (1 << portid)) == 0)
        	continue;
        
            printf("Closing port %d...", portid);
            rte_eth_dev_stop(portid);
            rte_eth_dev_close(portid);
            printf(" Done\n");
    }
    printf("Bye...\n");

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
static inline int ans_send_burst(uint8_t port_id, uint16_t queue_id, struct rte_mbuf **tx_pkts, uint16_t nb_pkts)
{
    uint16_t send_nb, left_nb;

    send_nb = rte_eth_tx_burst(port_id, queue_id, tx_pkts, nb_pkts);
    if (unlikely(send_nb < nb_pkts))
    {
        left_nb = send_nb;
        do
        {
            rte_pktmbuf_free(tx_pkts[left_nb]);
        } while (++left_nb < nb_pkts);
    }
    
    ans_eth_stats(port_id, nb_pkts, nb_pkts - send_nb);

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
static inline int ans_send_packet(uint8_t port, struct rte_mbuf *m)
{
    struct ans_lcore_config *qconf;
    struct ans_tx_queue  *tx_queue;
    
    qconf = &g_lcore_conf[rte_lcore_id()];

    tx_queue = &qconf->tx_queue[port];

    tx_queue->pkts[tx_queue->pkts_nb++] = m;

    if(tx_queue->pkts_nb == MAX_TX_BURST)
    {
        ans_send_burst(port, tx_queue->queue_id, tx_queue->pkts, tx_queue->pkts_nb);
    
        tx_queue->pkts_nb = 0;  
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
uint16_t ans_tx_burst(uint16_t port_id, uint16_t queue_id, struct rte_mbuf **tx_pkts, uint16_t nb_pkts)
{
    return rte_eth_tx_burst(port_id, queue_id, tx_pkts, nb_pkts);
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
static inline int ans_bypass_packet(uint8_t port_id, struct rte_mbuf *m)
{
    int ret = 0;

    if(ans_user_conf.kni_on != 1)
    {
        rte_pktmbuf_free(m);
        return 0 ;
    }

    ret = ans_kni_send_burst(&m, 1, port_id);
    if(ret != 0 )
        rte_pktmbuf_free(m);

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
static int ans_main_loop(__attribute__((unused)) void *dummy)
{
    int i, nb_rx;
    unsigned lcore_id;
    uint64_t prev_tsc, diff_tsc, cur_tsc;
    uint8_t portid, queueid;
    struct ans_lcore_config *qconf;
    uint64_t timer_prev_tsc = 0, timer_cur_tsc, timer_diff_tsc, timer_10ms_tsc;
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    struct ans_tx_queue *tx_queue;
    struct ans_rx_queue *rx_queue;
    const uint64_t drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S * BURST_TX_DRAIN_US;

    prev_tsc = 0;

    lcore_id = rte_lcore_id();
    qconf = &g_lcore_conf[lcore_id];

    if (qconf->n_rx_queue == 0)
    {
        RTE_LOG(INFO, USER8, "lcore %u has nothing to do\n", lcore_id);
        return 0;
    }

    RTE_LOG(INFO, USER8, "main loop on lcore %u\n", lcore_id);

    for (i = 0; i < qconf->n_rx_queue; i++)
    {
        portid = qconf->rx_queue[i].port_id;
        queueid = qconf->rx_queue[i].queue_id;
        RTE_LOG(INFO, USER8, " -- lcoreid=%u portid=%hhu rxqueueid=%hhu\n", lcore_id, portid, queueid);
    }

    printf("hz: %ld \n", rte_get_tsc_hz());
    
    timer_10ms_tsc = rte_get_tsc_hz() / 100;
    
    while (!ans_stopped)
    {

        cur_tsc = rte_rdtsc();
        timer_cur_tsc = cur_tsc;
        
        ans_message_handle(cur_tsc);
  
        /*
         * Call the timer handler on each core: as we don't
         * need a very precise timer, so only call
         * rte_timer_manage() every ~10ms (at 2Ghz). In a real
         * application, this will enhance performances as
         * reading the HPET timer is not efficient.
         */
        timer_diff_tsc = timer_cur_tsc - timer_prev_tsc;
        if (timer_diff_tsc > timer_10ms_tsc)
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
            for (i = 0; i < qconf->n_tx_port; i++)
            {
                portid = qconf->port_id[i];
                ans_eth_tx_flush(portid);
            }

            prev_tsc = cur_tsc;
        }

        /*
        * Read packet from RX queues
        */
        for (i = 0; i < qconf->n_rx_queue; ++i)
        {
            rx_queue = &qconf->rx_queue[i];

            nb_rx = rte_eth_rx_burst(rx_queue->port_id, rx_queue->queue_id, pkts_burst, MAX_PKT_BURST);
            if (nb_rx == 0)
                continue;

            ans_eth_rx_burst(rx_queue->port_id, pkts_burst, nb_rx);
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
static void ans_signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) 
    {
        printf("\nSignal %d received, preparing to exit...\n", signum);

#ifdef RTE_LIBRTE_PDUMP
        /* uninitialize packet capture framework */
        rte_pdump_uninit();
#endif

        printf("Telling cores to stop...\n");
        ans_stopped = 1;
    }

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
int main(int argc, char **argv)
{
    int i;
    int ret;
    int s;
    unsigned lcore_id;
    struct ans_init_config init_conf;
    struct ans_user_config *user_conf;
    struct ans_lcore_config *lcore_conf;

    user_conf = &ans_user_conf;
    lcore_conf = g_lcore_conf;
    
    memset(user_conf, 0, sizeof(struct ans_user_config));
    memset(lcore_conf, 0, sizeof(g_lcore_conf));

    user_conf->numa_on = 1;

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
      rte_exit(EXIT_FAILURE, "Invalid EAL parameters\n");

    signal(SIGINT, ans_signal_handler);
    signal(SIGTERM, ans_signal_handler);

#ifdef RTE_LIBRTE_PDUMP
	/* initialize packet capture framework */
//	rte_pdump_init(NULL);
#endif

    argc -= ret;
    argv += ret;

    /* parse application arguments (after the EAL ones) */
    ret = ans_parse_args(argc, argv, user_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "Invalid ANS parameters\n");


    if(user_conf->jumbo_frame_on)
    {
        ans_port_conf.rxmode.offloads |= DEV_RX_OFFLOAD_JUMBO_FRAME;
        ans_port_conf.txmode.offloads |= DEV_TX_OFFLOAD_MULTI_SEGS;
        ans_port_conf.rxmode.max_rx_pkt_len = user_conf->max_rx_pkt_len;
    }

    if(user_conf->ipsync_on && !user_conf->kni_on)
    {
        rte_exit(EXIT_FAILURE, "ipsync is enable, kni shall be enable too.\n");
    }

    if(user_conf->rx_nb == 0)
    {
        rte_exit(EXIT_FAILURE, "No configure RX lcore \n");
    }

    if(user_conf->worker_nb >= 1)
    {
        rte_exit(EXIT_FAILURE, "Don't support worker for current ans version \n");
    }

    user_conf->lcore_nb = rte_lcore_count();

    if (ans_check_lcore_rx(user_conf) < 0)
        rte_exit(EXIT_FAILURE, "check rx lcore config failed\n");

    ret = ans_check_port_config(user_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "check_port_config failed\n");


    ret = ans_init_lcore_config(user_conf, lcore_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "init lcore config failed\n");

    ret = ans_init_ports(user_conf, lcore_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "Init ports failed\n");


    /* add by ans_team: support KNI interface at 2014-12-15 */
    if(user_conf->kni_on == 1)
        ans_kni_config(user_conf, lcore_conf, ans_pktmbuf_pool);


    /* add by ans_team ---start */
    ans_init_timer();
    printf("\n rx core: %d, worker core: %d, sockets number:%d, lcore number:%d \n", 
        user_conf->rx_nb, user_conf->worker_nb, user_conf->socket_nb, user_conf->lcore_nb);

    printf("start to init ans \n");

    ans_get_lcore_config(lcore_conf, &init_conf);
    
    init_conf.sock_nb = user_conf->lcore_nb * 128 * 1024;

    for(i = 0 ; i < ANS_MAX_NB_SOCKETS; i++)
    {
        init_conf.pktmbuf_pool[i] = ans_pktmbuf_pool[i];
    }

    init_conf.ip_sync = user_conf->ipsync_on;
    init_conf.port_send = ans_tx_burst;
    init_conf.port_bypass = ans_bypass_packet;

    ret = ans_initialize(&init_conf);
    if (ret != 0)
        rte_exit(EXIT_FAILURE, "Init ans failed \n");

    /* add by ans_team ---end */

    /* add by ans_team for testing ---start */
    uint8_t ifname[16];
    int portid;
    uint16_t kni_id;
    struct ether_addr eth_addr;
    uint16_t qmapping_nb;
    struct rte_eth_dev_info dev_info;
    
    RTE_ETH_FOREACH_DEV(portid)
    {
        /* skip ports that are not enabled */
        if ((user_conf->port_mask & (1 << portid)) == 0)
        {
            printf("\nSkipping disabled port %d\n", portid);
            continue;
        }
        
        rte_eth_dev_info_get(portid, &dev_info);
        
        memset(ifname, 0, sizeof(ifname));
        if(strcmp(dev_info.driver_name, "net_vhost") == 0)
        {
            sprintf(ifname, "vhost%d", portid);
        }
        else if(strcmp(dev_info.driver_name, "net_virtio_user") == 0)
        {
            sprintf(ifname, "virtio%d", portid);
        }
        else
        {
            sprintf(ifname, "veth%d", portid);
        }
        
        kni_id = ans_kni_id_get(portid);
        
        printf("add %s device, kni id %d \n", ifname, kni_id);
        rte_eth_macaddr_get(portid, &eth_addr);
        
        ans_iface_add(portid, kni_id, ifname, &eth_addr);

        /* set port queue mapping */
        struct ans_port_queue port_queue;
        ans_get_port_queue(portid, &port_queue, lcore_conf);
        
        ret = ans_iface_set_queue(ifname, &port_queue);
        if (ret != 0)
            rte_exit(EXIT_FAILURE, "set queue failed \n");

        /* host byte order */
        int ip_addr = 0x0a000002;
        ip_addr += portid << 16;

        printf("add IP %x on device %s \n", ip_addr, ifname);
        ans_add_ipaddr((char *)ifname, ip_addr, 24);
    }

    printf("show all IPs: \n");
    ans_show_ipaddr();
    printf("\n");
    
    /* add by ans_team ---end */

    /* add by ans_team for testing ---start */
    
    int route_ret = 0;
    printf("add static route \r\n");

    /* host byte order */
    route_ret = ans_add_route(0x0a0a0000, 24, 0x0a000005);

    ans_show_route();
    printf("\n");
    /* add by ans_team ---end */

    ans_start_ports(user_conf);

    /* launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(ans_main_loop, NULL, CALL_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id){
      if (rte_eal_wait_lcore(lcore_id) < 0)
        ret = -1;
        break;
    }

    ans_stop_ports(user_conf);

    return ret;
}
