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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <termios.h>
#ifndef __linux__
  #ifdef __FreeBSD__
    #include <sys/socket.h>
  #else
    #include <net/socket.h>
  #endif
#endif

#include <rte_common.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_tailq.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_string_fns.h>

#include "netdp_errno.h"
#include "netdp_socket_intf.h"

struct rte_ring *netdpsock_ring_tx;
struct rte_ring *netdpsock_ring_rx;
struct rte_mempool *netdpsock_message_pool;

const struct rte_memzone    *netdpsock_mz = NULL;
netdp_sock_ring_conf_t *netdpsock_ring_conf = NULL;
netdp_sock_ring_t netdpsock_ring;

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
int netdpsock_ring_init(void)  
{
    int i ;
    
    memset(&netdpsock_ring, 0, sizeof(netdpsock_ring));
    
    netdpsock_mz = rte_memzone_lookup(NETDP_SOCK_RING_MZ);
    if (netdpsock_mz == NULL)
    	return NETDP_EMZ;

    for(i = 0; i < NETDP_SUPPORT_APP_MAX; i++)
    {
        netdpsock_ring_conf = (netdp_sock_ring_conf_t *)(netdpsock_mz->addr + i * sizeof(netdp_sock_ring_conf_t));

        if(netdpsock_ring_conf->ring_tag == NETDP_SOCK_RING_UNUSED)
        {
            netdpsock_ring_conf->ring_tag = NETDP_SOCK_RING_USED;
            break;
        }
    }

    if(netdpsock_ring_conf == NULL)
    {
        NETDP_ERR("netdpsock: no unused ring conf \n");
        return NETDP_EMZ;
    }

    
    netdpsock_ring.ctrl_ring_rx = rte_ring_lookup(netdpsock_ring_conf->ctrl_ring_tx);
    if(NULL == netdpsock_ring.ctrl_ring_rx)
    {
        NETDP_ERR("Lookup ring(%s) failed \n", netdpsock_ring_conf->ctrl_ring_tx);
        return NETDP_ECTRLRING;
    }

    netdpsock_ring.ctrl_ring_tx = rte_ring_lookup(netdpsock_ring_conf->ctrl_ring_rx);
    if(NULL == netdpsock_ring.ctrl_ring_tx)
    {
        NETDP_ERR("Lookup ring(%s) failed \n", netdpsock_ring_conf->ctrl_ring_tx);
        return NETDP_ECTRLRING;
    }
    
    netdpsock_ring.ctrl_msg_pool = rte_mempool_lookup(netdpsock_ring_conf->ctrl_msg_pool);
     if(NULL == netdpsock_ring.ctrl_msg_pool)
    {
        NETDP_ERR("Lookup message pool(%s) failed \n", netdpsock_ring_conf->ctrl_msg_pool);
        return NETDP_EMSGPOOL;
    }

    netdpsock_ring.data_ring_rx = rte_ring_lookup(netdpsock_ring_conf->data_ring_tx);
    if(NULL == netdpsock_ring.data_ring_rx)
    {
        NETDP_ERR("Lookup ring(%s) failed \n", netdpsock_ring_conf->data_ring_rx);
        return NETDP_ECTRLRING;
    }

    netdpsock_ring.data_ring_tx = rte_ring_lookup(netdpsock_ring_conf->data_ring_rx);
    if(NULL == netdpsock_ring.data_ring_tx)
    {
        NETDP_ERR("Lookup ring(%s) failed \n", netdpsock_ring_conf->data_ring_tx);
        return NETDP_ECTRLRING;
    }
    
    netdpsock_ring.data_msg_pool = rte_mempool_lookup(netdpsock_ring_conf->data_msg_pool);
     if(NULL == netdpsock_ring.data_msg_pool)
    {
        NETDP_ERR("Lookup message pool(%s) failed \n", netdpsock_ring_conf->data_msg_pool);
        return NETDP_EMSGPOOL;
    }
        
    return NETDP_EOK;
}

/*
int netdpsock_ring_recv(void *buff, int buff_len)
{
    void *msg;
    int wait_num = 0; 
    while (wait_num < 100)
    {
        if (rte_ring_dequeue(netdpsock_ring_rx, &msg) < 0)
        {
            wait_num++;
            usleep(200);
            continue;
        }
        rte_memcpy(buff, msg, buff_len);
        rte_mempool_put(netdpsock_message_pool, msg);
        return NETDPCMD_RECV_MSG;
    }

    return NETDPCMD_NONRECV_MSG;
}

int netdpsock_ring_send(void *buff, int buff_len)
{
    void *msg;

    if(buff_len > NETDP_RING_MSG_SIZE)
    {
        printf("Too long message size, max is %d \n", NETDP_RING_MSG_SIZE);

        return NETDP_EMSGPOOL;
    }

    if (rte_mempool_get(netdpsock_message_pool, &msg) < 0)
    {
        printf("Getting message failed \n");
        return NETDP_EMSGPOOL;
    }

    rte_memcpy(msg, buff, buff_len);
        
    if (rte_ring_enqueue(netdpsock_ring_tx, msg) < 0) 
    {
        printf("Sending message to NETDP stack failed  \n");
        rte_mempool_put(netdpsock_message_pool, msg);
        return NETDP_EMSGPOOL;
    }

    return 0;

}

*/

