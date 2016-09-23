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
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <assert.h>

#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_tailq.h>
#include <rte_per_lcore.h>
#include <rte_atomic.h>
#include <rte_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_log.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_cycles.h>
#include <rte_malloc.h>
#include <rte_kni.h>

#include "ans_kni.h"


#ifndef ANS_KNI_RING_SIZE
#define ANS_KNI_RING_SIZE   4096
#endif

#define PKT_BURST_SZ                    32
#define MAX_PACKET_SZ                 2048
#define RTE_LOGTYPE_APP             RTE_LOGTYPE_USER1

/* Total octets in ethernet header */
#define KNI_ENET_HEADER_SIZE    14

/* Total octets in the FCS */
#define KNI_ENET_FCS_SIZE       4


struct kni_port_params
{
  uint8_t port_id;  /* Port ID */
  unsigned lcore_id;  /* lcore ID bind to port */
  struct rte_kni  * kni; /* KNI context pointers */
  struct rte_ring * ring; /* Ring used to recieve packets from other cores */

} __rte_cache_aligned;

struct kni_lcore_params
{
  struct kni_port_params * port[RTE_MAX_ETHPORTS];
  unsigned nb_ports;

} __rte_cache_aligned;

struct kni_port_params  * kni_port_params_array[RTE_MAX_ETHPORTS];
struct kni_lcore_params * kni_lcore_params_array[RTE_MAX_LCORE];

//TODO:
static struct rte_mempool ** kni_pktmbuf_pool;

/* Options for configuring ethernet port */
static struct rte_eth_conf port_conf = 
{
  .rxmode = 
       {
    .header_split = 0,      /* Header Split disabled */
    .hw_ip_checksum = 0,    /* IP checksum offload disabled */
    .hw_vlan_filter = 0,    /* VLAN filtering disabled */
    .jumbo_frame = 0,       /* Jumbo Frame Support disabled */
    .hw_strip_crc = 0,      /* CRC stripped by hardware */
  },
  .txmode = 
  {
    .mq_mode = ETH_MQ_TX_NONE,
  },
};



//static struct kni_interface_stats kni_stats[RTE_MAX_ETHPORTS];
static int kni_change_mtu(uint8_t port_id, unsigned new_mtu);
static int kni_config_network_interface(uint8_t port_id, uint8_t if_up);
static int ans_kni_alloc(uint8_t port_id);

/* KNI Module Interface */
int ans_kni_sendpkt_burst(struct rte_mbuf ** mbufs, unsigned nb_mbufs, unsigned port_id)
{
    if(unlikely(kni_port_params_array[port_id] == NULL))
        return -ENOENT;

    struct rte_ring * ring = kni_port_params_array[port_id]->ring;
    if(unlikely(ring == NULL))
        return -ENOENT;

    return rte_ring_enqueue_bulk(ring,(void **)mbufs,nb_mbufs);
}

int ans_kni_init()
{
    for (int port = 0; port < RTE_MAX_ETHPORTS; port++) 
    {
        /* Skip ports that are not enabled */
        if(kni_port_params_array[port] == NULL)
            continue;

        ans_kni_alloc(port);
    }

    return 0;
}

int ans_kni_config(struct ans_user_config * common_config, struct rte_mempool * pktmbuf_pool[])
{
    uint32_t portmask    = common_config->port_mask;
    unsigned lcore_item  = 0;
    unsigned port_id = 0;

    // Link mbufs pool from outside modules.
    kni_pktmbuf_pool = pktmbuf_pool;

    // Bind params between lcores and KNI .
    for(port_id = 0; port_id < RTE_MAX_ETHPORTS; port_id++)
    {
        if((portmask & (1 << port_id)) == 0)
            continue;

        assert(kni_port_params_array[port_id] == NULL);

        unsigned lcore_id = lcore_item >= common_config->lcore_param_nb 
            ? common_config->lcore_param[lcore_item = 0].lcore_id : common_config->lcore_param[lcore_item++].lcore_id;

        kni_port_params_array[port_id] = rte_zmalloc(NULL,sizeof(struct kni_port_params),0);
        kni_port_params_array[port_id]->port_id  = port_id;
        kni_port_params_array[port_id]->lcore_id = lcore_id;

        printf("ans_kni: port_id=%d,lcore_id=%d\n",port_id,lcore_id);
    }

    for(int i = 0; i < RTE_MAX_ETHPORTS; i++)
    {
        if(kni_port_params_array[i] == NULL)
            continue;

        struct kni_port_params * d  = kni_port_params_array[i];

        if(kni_lcore_params_array[d->lcore_id] == NULL)
        {
            kni_lcore_params_array[d->lcore_id] = rte_zmalloc(NULL,sizeof(struct kni_lcore_params),0);
        }

        unsigned nb_ports = kni_lcore_params_array[d->lcore_id]->nb_ports;
        kni_lcore_params_array[d->lcore_id]->port[nb_ports] = d;
        kni_lcore_params_array[d->lcore_id]->nb_ports++;
    }


    /* Invoke rte KNI init to preallocate the ports */
    rte_kni_init(port_id++);

    ans_kni_init();

    return 0;
}


int ans_kni_destory()
{
    for (int port = 0; port < RTE_MAX_ETHPORTS; port++) 
    {
               /* Skip ports that are not enabled */
        if(kni_port_params_array[port] == NULL)
            continue;

        rte_kni_release(kni_port_params_array[port]->kni);
        rte_free(kni_port_params_array[port]);
    }
}

static void kni_burst_free_mbufs(struct rte_mbuf **pkts, unsigned num)
{
    unsigned i;

    if (pkts == NULL)
        return;

    for (i = 0; i < num; i++) 
    {
        rte_pktmbuf_free(pkts[i]);
        pkts[i] = NULL;
    }
}

static int kni_free_kni(uint8_t port_id)
{
    uint8_t i = 0;
    struct kni_port_params **p = kni_port_params_array;

    if (port_id >= RTE_MAX_ETHPORTS || !p[port_id])
        return -1;

    /* fix me */
    rte_kni_release(p[i]->kni);
    p[i]->kni = NULL;

    return 0;
}

/* Alloc KNI Devices for PORT_ID */
static int ans_kni_alloc(uint8_t port_id)
{
  uint8_t i;
  struct rte_kni *kni;
  struct rte_kni_conf conf;
  struct kni_port_params **params = kni_port_params_array;

  unsigned lcore_id = params[port_id]->lcore_id;
  unsigned lcore_socket = rte_lcore_to_socket_id(lcore_id);
  struct rte_mempool * kni_mempool = kni_pktmbuf_pool[lcore_socket];

  if (port_id >= RTE_MAX_ETHPORTS || !params[port_id])
    return -1;

  memset(&conf, 0, sizeof(conf));
  snprintf(conf.name, RTE_KNI_NAMESIZE,"keth%u", port_id);
  conf.group_id = (uint16_t)port_id;
  conf.mbuf_size = MAX_PACKET_SZ;

  struct rte_kni_ops ops;
  struct rte_eth_dev_info dev_info;

  memset(&dev_info, 0, sizeof(dev_info));
  rte_eth_dev_info_get(port_id, &dev_info);
  conf.addr = dev_info.pci_dev->addr;
  conf.id = dev_info.pci_dev->id;

  memset(&ops, 0, sizeof(ops));
  ops.port_id = port_id;
  ops.change_mtu = kni_change_mtu;
  ops.config_network_if = kni_config_network_interface;

  kni = rte_kni_alloc(kni_mempool, &conf, &ops);

  if (!kni)
    rte_exit(EXIT_FAILURE, "Fail to create kni for " "port: %d\n", port_id);

  params[port_id]->kni = kni;

  /* Create Ring to recieve the pkts from other cores */
  char ring_name[32];
  snprintf(ring_name,sizeof(ring_name),"kni_ring_s%u_p%u",lcore_socket,port_id);

  params[port_id]->ring = rte_ring_create(ring_name,ANS_KNI_RING_SIZE, lcore_socket,RING_F_SC_DEQ);
  
  if(!params[port_id]->ring)
    rte_exit(EXIT_FAILURE, "Fail to create ring for kni %s",ring_name);

  return 0;
}

static void kni_ring_to_kni(struct kni_port_params *p)
{
    uint8_t i, port_id;
    unsigned nb_rx, num;
    struct rte_mbuf *pkts_burst[PKT_BURST_SZ];

    if (p == NULL)
        return;

    port_id = p->port_id;

    /* Burst rx from ring */
    nb_rx = rte_ring_dequeue_burst(p->ring,(void **)&pkts_burst, PKT_BURST_SZ);

    if (unlikely(nb_rx > PKT_BURST_SZ))
    {
        RTE_LOG(ERR, APP, "Error receiving from eth\n");
        return;
    }

    /* Burst tx to kni */
    num = rte_kni_tx_burst(p->kni, pkts_burst, nb_rx);
    //kni_stats[port_id].rx_packets += num;

    rte_kni_handle_request(p->kni);
    if (unlikely(num < nb_rx)) 
    {
        /* Free mbufs not tx to kni interface */
        kni_burst_free_mbufs(&pkts_burst[num], nb_rx - num);
        //kni_stats[port_id].rx_dropped += nb_rx - num;
    }

    return;
}

/**
 * Interface to dequeue mbufs from tx_q and burst tx
 */
static void kni_kni_to_eth(struct kni_port_params *p)
{
    uint8_t i, port_id;
    unsigned nb_tx, num;
    uint32_t nb_kni;
    struct rte_mbuf *pkts_burst[PKT_BURST_SZ];

    if (p == NULL)
        return;

    port_id = p->port_id;

    /* Burst rx from kni */
    num = rte_kni_rx_burst(p->kni, pkts_burst, PKT_BURST_SZ);
    if (unlikely(num > PKT_BURST_SZ)) 
    {
        RTE_LOG(ERR, APP, "Error receiving from KNI\n");
        return;
    }

    /* Burst tx to eth, only send to the first queue */
    nb_tx = rte_eth_tx_burst(port_id, 0, pkts_burst, (uint16_t)num);
    // kni_stats[port_id].tx_packets += nb_tx;

    if (unlikely(nb_tx < num)) 
    {
        /* Free mbufs not tx to NIC */
        kni_burst_free_mbufs(&pkts_burst[nb_tx], num - nb_tx);
        // kni_stats[port_id].tx_dropped += num - nb_tx;
    }

    return;
}


void ans_kni_main()
{
    uint8_t lcore_id = rte_lcore_id();
    struct kni_lcore_params * lcore = kni_lcore_params_array[lcore_id];

    if(unlikely(lcore == NULL))
        return;

    for(int i = 0; i < lcore->nb_ports; i++)
    {
        struct kni_port_params * port = lcore->port[i];

        kni_ring_to_kni(port);
        kni_kni_to_eth(port);
    }

    return;
}


/* Callback Functions */

static int kni_change_mtu(uint8_t port_id, unsigned new_mtu)
{
  int ret;
  struct rte_eth_conf conf;

  if (port_id >= rte_eth_dev_count()) {
    RTE_LOG(ERR, APP, "Invalid port id %d\n", port_id);
    return -EINVAL;
  }

  RTE_LOG(INFO, APP, "Change MTU of port %d to %u\n", port_id, new_mtu);

  /* Stop specific port */
  rte_eth_dev_stop(port_id);

  memcpy(&conf, &port_conf, sizeof(conf));
  /* Set new MTU */
  if (new_mtu > ETHER_MAX_LEN)
    conf.rxmode.jumbo_frame = 1;
  else
    conf.rxmode.jumbo_frame = 0;

  /* mtu + length of header + length of FCS = max pkt length */
  conf.rxmode.max_rx_pkt_len = new_mtu + KNI_ENET_HEADER_SIZE +
              KNI_ENET_FCS_SIZE;
  ret = rte_eth_dev_configure(port_id, 1, 1, &conf);
  if (ret < 0) {
    RTE_LOG(ERR, APP, "Fail to reconfigure port %d\n", port_id);
    return ret;
  }

  /* Restart specific port */
  ret = rte_eth_dev_start(port_id);
  if (ret < 0) {
    RTE_LOG(ERR, APP, "Fail to restart port %d\n", port_id);
    return ret;
  }

  return 0;
}

/* Callback for request of configuring network interface up/down */
static int kni_config_network_interface(uint8_t port_id, uint8_t if_up)
{
  int ret = 0;

  if (port_id >= rte_eth_dev_count() || port_id >= RTE_MAX_ETHPORTS) {
    RTE_LOG(ERR, APP, "Invalid port id %d\n", port_id);
    return -EINVAL;
  }

  RTE_LOG(INFO, APP, "Configure network interface of %d %s\n",
          port_id, if_up ? "up" : "down");

  if (if_up != 0) { /* Configure network interface up */
    rte_eth_dev_stop(port_id);
    ret = rte_eth_dev_start(port_id);
  } else /* Configure network interface down */
    rte_eth_dev_stop(port_id);

  if (ret < 0)
    RTE_LOG(ERR, APP, "Failed to start port %d\n", port_id);

  return ret;
}


