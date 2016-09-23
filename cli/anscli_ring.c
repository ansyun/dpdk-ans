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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
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
#include <rte_per_lcore.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_string_fns.h>


#include <cmdline_rdline.h>
#include <cmdline_parse.h>
#include <cmdline_parse_ipaddr.h>
#include <cmdline_parse_num.h>
#include <cmdline_parse_string.h>
#include <cmdline.h>


#include "anscli_conf.h"
#include "ans_errno.h"
#include "ans_conf.h"
#include "anscli_ring.h"

struct rte_ring *anscli_ring_tx;
struct rte_ring *anscli_ring_rx;
struct rte_mempool *anscli_message_pool;


int anscli_ring_init(void)
{

    anscli_ring_rx = rte_ring_lookup(ANS_PRI_2_SEC);
    if(NULL == anscli_ring_rx)
    {
        printf("Lookup ring(%s) failed \n", ANS_PRI_2_SEC);
        return ANS_ECTRLRING;
    }

    anscli_ring_tx = rte_ring_lookup(ANS_SEC_2_PRI);
     if(NULL == anscli_ring_tx)
    {
        printf("Lookup ring(%s) failed \n", ANS_SEC_2_PRI);
        return ANS_ECTRLRING;
    }

    anscli_message_pool = rte_mempool_lookup(ANS_MSG_POOL_NAME);
     if(NULL == anscli_message_pool)
    {
        printf("Lookup message pool(%s) failed \n", ANS_MSG_POOL_NAME);
        return ANS_EMSGPOOL;
    }

    return 0;
}


int anscli_ring_recv(void *buff, int buff_len)
{
    void *msg;
    int wait_num = 0;
    while (wait_num < 10)
    {
        if (rte_ring_dequeue(anscli_ring_rx, &msg) < 0)
        {
            wait_num++;
            usleep(100000);  /* 100 ms */
            continue;
        }
        rte_memcpy(buff, msg, buff_len);
        rte_mempool_put(anscli_message_pool, msg);
        return ANSCLI_RECV_MSG;
    }

    return ANSCLI_NONRECV_MSG;
}

int anscli_ring_send(void *buff, int buff_len)
{
    void *msg;

    if(buff_len > ANS_RING_MSG_SIZE)
    {
        printf("Too long message size, max is %d \n", ANS_RING_MSG_SIZE);

        return ANS_EMSGPOOL;
    }

    if (rte_mempool_get(anscli_message_pool, &msg) < 0)
    {
        printf("Getting message failed \n");
        return ANS_EMSGPOOL;
    }

    rte_memcpy(msg, buff, buff_len);

    if (rte_ring_enqueue(anscli_ring_tx, msg) < 0)
    {
        printf("Sending message to ANS stack failed  \n");
        rte_mempool_put(anscli_message_pool, msg);
        return ANS_EMSGPOOL;
    }

    return 0;

}


