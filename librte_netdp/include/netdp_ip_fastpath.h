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


/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following edsclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following edsclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE EDSCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#ifndef __NETDP_IP_FASTPATH_H__ // {
#define __NETDP_IP_FASTPATH_H__

#define NETDP_IP_CACHE_ENTRIES_PER_BUCKET	5
#define NETDP_IP_CACHE_TIME_SHIFT		27	// Gives a resolution of 200ms or so
#define NETDP_IP_CACHE_TIME_BITMASK	0x7f
#define NETDP_IP_CACHE_READ_RETRY_COUNT	10

typedef union {
   uint64_t u64;
   struct {
      uint32_t addr;
      uint16_t ifidx;
      uint16_t in_use : 1;
      uint16_t unused : 12;
      uint16_t id     : 3;
   } s;
} netdp_ip_cache_bucket_w0_t;

typedef union {
   uint64_t u64;
   struct {
      uint8_t edst[6];
      uint8_t unused;
      uint8_t zeroes : 5;
      uint8_t id     : 3;
   } s;
} netdp_ip_cache_bucket_w1_t;

typedef union {
   uint64_t u64;
   struct {
      uint64_t ifa_ptr   : 40;
      uint64_t pending   : 1;
      uint64_t timestamp : 7;
      uint64_t zeroes    : 13;
      uint64_t id        : 3;
   } s;
} netdp_ip_cache_bucket_w2_t;

typedef struct {
   netdp_ip_cache_bucket_w0_t word0;
   netdp_ip_cache_bucket_w1_t word1;
   netdp_ip_cache_bucket_w2_t word2;
} netdp_ip_cache_bucket_entry_t;

typedef struct {
   rte_spinlock_t lock;
   uint32_t idx;
   netdp_ip_cache_bucket_entry_t entry[NETDP_IP_CACHE_ENTRIES_PER_BUCKET];
} netdp_ip_cache_bucket_t;

extern  netdp_ip_cache_bucket_t *netdp_ip_cache;

#endif // __NETDP_IP_FASTPATH_H__ 

