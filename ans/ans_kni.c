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
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <assert.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <spawn.h>

#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <net/if_arp.h>

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
#include <rte_version.h>
#if RTE_VERSION >= RTE_VERSION_NUM(17,11,0,0)
#  include <rte_bus_pci.h>
#endif

#include "ans_kni.h"


#ifndef ANS_KNI_RING_SIZE
#define ANS_KNI_RING_SIZE   4096
#endif

#define PKT_BURST_SZ                    32
#define MAX_PACKET_SZ                 2048

/* Total octets in ethernet header */
#define KNI_ENET_HEADER_SIZE    14

/* Total octets in the FCS */
#define KNI_ENET_FCS_SIZE       4


struct kni_port_params
{
    uint8_t port_id;  /* Port ID */
    unsigned lcore_id;  /* lcore ID bind to port */
    uint8_t queue_id;   /* tx queue id of lcore of the port */

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

static struct rte_mempool ** kni_pktmbuf_pool;

//static struct kni_interface_stats kni_stats[RTE_MAX_ETHPORTS];

/* Callback for request of configuring network interface up/down */
static int ans_kni_config_iface(uint16_t port_id, uint8_t if_up)
{
    int ret = 0;

    printf("Don't allow to set port UP/DOWN. Always return successfully \n");
    
/*
    if (port_id >= rte_eth_dev_count())
        return -EINVAL;

    ret = (if_up) ?
        rte_eth_dev_set_link_up(port_id) :
        rte_eth_dev_set_link_down(port_id);
*/
    return ret;
}

/* Callback Functions */
static int ans_kni_change_mtu(uint16_t port_id, unsigned int new_mtu)
{
    int ret;
    
    if (port_id >= rte_eth_dev_count())
        return -EINVAL;

    if (new_mtu > ETHER_MAX_LEN)
        return -EINVAL;

    /* Set new MTU */
    ret = rte_eth_dev_set_mtu(port_id, new_mtu);
    if (ret < 0)
        return ret;

    return 0;

}


/* KNI Module Interface */
int ans_kni_send_burst(struct rte_mbuf ** mbufs, unsigned nb_mbufs, unsigned port_id)
{
    if(unlikely(kni_port_params_array[port_id] == NULL))
        return -ENOENT;

    struct rte_ring * ring = kni_port_params_array[port_id]->ring;
    if(unlikely(ring == NULL))
        return -ENOENT;

    return rte_ring_enqueue_bulk(ring,(void **)mbufs, nb_mbufs, NULL);
}

/* Set kni interface mac */
int ans_kni_set_mac(char *name, uint8_t port)  
{  
    int fd = 0;  
    int ret = -1;  
    struct ifreq temp;  
    struct sockaddr* addr;  
    struct ether_addr eth_addr;

    rte_eth_macaddr_get(port, &eth_addr);

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
    {  
        printf("create fd failed when set kni MAC \n");
        return -1;  
    }  

    strcpy(temp.ifr_name, name);  
    addr = (struct sockaddr*)&temp.ifr_hwaddr;  

    addr->sa_family = ARPHRD_ETHER;  
    memcpy(addr->sa_data, eth_addr.addr_bytes, ETHER_ADDR_LEN);  

    ret = ioctl(fd, SIOCSIFHWADDR, &temp);  
    if(ret != 0)
    {
        printf("set mac failed, ret %d \n", ret);
    }
    
    close(fd);  
    return ret;  
}  


/**********************************************************************
*@description:
* Alloc KNI Devices for PORT_ID
*
*@parameters:
* [in]:
* [in]:
*
*@return values:
*
**********************************************************************/
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
    snprintf(conf.name, RTE_KNI_NAMESIZE, "veth%u", port_id);
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
    ops.change_mtu = ans_kni_change_mtu;
    ops.config_network_if =  ans_kni_config_iface;

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

    ans_kni_set_mac(conf.name, port_id);

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
int ans_kni_config(struct ans_user_config * common_config, struct ans_lcore_queue *lcore_conf, struct rte_mempool * pktmbuf_pool[])
{
    uint32_t portmask    = common_config->port_mask;
    unsigned lcore_item  = 0;
    unsigned port_id = 0;
    unsigned lcore_id;
    uint8_t queue_id;

    // Link mbufs pool from outside modules.
    kni_pktmbuf_pool = pktmbuf_pool;

    // Bind params between lcores and KNI .
    for(port_id = 0; port_id < RTE_MAX_ETHPORTS; port_id++)
    {
        if((portmask & (1 << port_id)) == 0)
            continue;

        assert(kni_port_params_array[port_id] == NULL);

        lcore_id = lcore_item >= common_config->lcore_param_nb
            ? common_config->lcore_param[lcore_item = 0].lcore_id : common_config->lcore_param[lcore_item++].lcore_id;

        kni_port_params_array[port_id] = rte_zmalloc(NULL,sizeof(struct kni_port_params),0);
        kni_port_params_array[port_id]->port_id  = port_id;
        kni_port_params_array[port_id]->lcore_id = lcore_id;
        queue_id = lcore_conf[lcore_id].tx_queue[port_id].queue_id;
        kni_port_params_array[port_id]->queue_id = queue_id;

        printf("ans_kni: port_id=%d, lcore_id=%d, tx queue id=%d \n",port_id,lcore_id, queue_id);
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
uint16_t ans_kni_id_get(uint8_t port)
{
    uint32_t kni_id;
    const char *kni_name;
    struct kni_port_params *kni_port  = kni_port_params_array[port];

    if(kni_port == NULL)
    {
        return 0;
    }
    else
    {
        kni_name = rte_kni_get_name(kni_port->kni);
        kni_id = if_nametoindex(kni_name);
            
        return kni_id;
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
static inline void ans_kni_mbuf_free(struct rte_mbuf **pkts, unsigned num)
{
    unsigned i;

    if (unlikely(pkts == NULL))
        return;

    for (i = 0; i < num; i++)
    {
        rte_pktmbuf_free(pkts[i]);
        pkts[i] = NULL;
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
static int ans_kni_free(uint8_t port_id)
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
static inline void ans_kni_to_linux(struct kni_port_params *port_param)
{
    unsigned nb_rx, num;
    struct rte_mbuf *pkts_burst[PKT_BURST_SZ];

    /* handle kin request event */
    rte_kni_handle_request(port_param->kni);

    /* Burst rx from ring */
    nb_rx = rte_ring_dequeue_burst(port_param->ring, (void **)&pkts_burst, PKT_BURST_SZ, NULL);
    if(nb_rx == 0)
    {
        return;
    }
    
    /* Burst tx to kni */
    num = rte_kni_tx_burst(port_param->kni, pkts_burst, nb_rx);
    //kni_stats[port_id].rx_packets += num;
    if (unlikely(num < nb_rx))
    {
        /* Free mbufs not tx to kni interface */
        ans_kni_mbuf_free(&pkts_burst[num], nb_rx - num);
        //kni_stats[port_id].rx_dropped += nb_rx - num;
    }

    return;
}

 /**********************************************************************
*@description:
* Interface to dequeue mbufs from tx_q and burst tx
*
*@parameters:
* [in]:
* [in]:
*
*@return values:
*
**********************************************************************/
static inline void ans_kni_to_eth(struct kni_port_params *port_param)
{
    unsigned nb_tx, num;
    uint32_t nb_kni;
    struct rte_mbuf *pkts_burst[PKT_BURST_SZ];

    /* Burst rx from kni */
    num = rte_kni_rx_burst(port_param->kni, pkts_burst, PKT_BURST_SZ);
    if (unlikely(num > PKT_BURST_SZ))
    {
        RTE_LOG(ERR, USER8, "Error receiving from KNI\n");
        return;
    }

    if(num == 0)
    {
        return;
    }

    /* Burst tx to eth, only send to the first queue */
    nb_tx = rte_eth_tx_burst(port_param->port_id, port_param->queue_id, pkts_burst, (uint16_t)num);
    // kni_stats[port_id].tx_packets += nb_tx;

    if (unlikely(nb_tx < num))
    {
        /* Free mbufs not tx to NIC */
        ans_kni_mbuf_free(&pkts_burst[nb_tx], num - nb_tx);
        // kni_stats[port_id].tx_dropped += num - nb_tx;
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
void ans_kni_main()
{
    int i;
    uint8_t lcore_id = rte_lcore_id();
    struct kni_port_params * port_param;
    struct kni_lcore_params * lcore_param = kni_lcore_params_array[lcore_id];

    if(unlikely(!lcore_param))
        return;

    for(i = 0; i < lcore_param->nb_ports; i++)
    {
         port_param = lcore_param->port[i];

        ans_kni_to_linux(port_param);
        ans_kni_to_eth(port_param);
    }

    return;
}

