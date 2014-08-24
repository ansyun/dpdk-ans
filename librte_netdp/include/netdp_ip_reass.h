
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


#ifndef _NETDP_IP_REASS_H__
#define _NETDP_IP_REASS_H__

/* Maximum number of ipv4 fragments per packet */
#define NETDP_IP_MAX_NUM_FRAGMENTS	400

/* Maximum number of ipv6 fragments per packet */
#define NETDP_IP_MAX_NUM_IP6_FRAGMENTS	400

/* Maximum number of reassemblies "in progress" at any given time */
#define NETDP_IP_MAX_REASSEMBLY_COUNT    (3*16*1024)

/* Maximum number of collisions */
#define NETDP_IP_MAX_REASSEMBLY_COLLISIONS  10

#define NETDP_IP_IPREASS_HASH1_BITS		16
#define NETDP_IP_IPREASS_HASH1_SIZE		(1 << IPREASS_HASH_BITS)
#define NETDP_IP_IPREASS_HASH1_MASK		(IPREASS_HASH_SIZE - 1)

#define NETDP_IP_IPREASS_HASH2_BITS		32
#define NETDP_IP_IPREASS_HASH2_SIZE		(1 << IPREASS_HASH_BITS)
#define NETDP_IP_IPREASS_HASH2_MASK		(IPREASS_HASH_SIZE - 1)

#define NETDP_IP_IPREASS_HASH_SIZE		(64*1024)	

#define NETDP_IP_IPREASS_CRC_POLYNOMIAL		0x1edc6f41

#define NETDP_IP_LAST_FRAG_BIT   2
#define NETDP_IP_LAST_FRAG_MASK  (1 << NETDP_IP_LAST_FRAG_BIT)

#define NETDP_IP_IPQ_TYPE0       0
#define NETDP_IP_IPQ_TYPE1       1
#define NETDP_IP_IPQ_TYPE2       2

#define NETDP_IP_PARENT_LIST_NDESC 3
#define NETDP_IP_CHILD_LIST_NDESC  5

#define NETDP_REASS_TIMEOUT 10

/** Reassembly queue header structure
 *
 */
typedef struct netdp_ip_ipq_hdr_t {
   /* Word 0 */
   uint64_t lock      : 1;
   uint64_t ndesc     : 3;          /** 3b number of desc */
   uint64_t ipq_type  : 2;          /** 2b ipq type */
   uint64_t ipq0_used : 1;          /** 1b ipq in use */
   uint64_t ipq1_used : 1;          /** 1b ipq in use */
   uint64_t res       : 16;         /** 16b reserved */
   uint64_t ptr       : 40;         /** 40b ptr to second level hash list 128 byte aligned */
} netdp_ip_ipq_hdr;


/** Reassembly queue element */
typedef struct netdp_ip_ipq_elem_info_t {

   /* Word 0 */
   uint64_t status : 8;   /** 1b => 0 | 0 | 0 | U | T | F | L | C | N */
                                /* T = timer started
                                 * F = first frag received
                                 * L = last frag received
                                 * C = TOS.CE received
                                 * N = TOS.Not-ECT received
                                 */

   uint64_t ndesc : 8;    /** 1b # frags in this packet */
   uint64_t proto : 8;     /** 1b protocol of this fragment */
   uint64_t id : 16;       /** 2b frag id for this group of fragments*/
   uint64_t nbuf : 8;
   uint64_t tag : 16;

   /* Word 1 */
   uint64_t total_len : 16;     /** 2b total length of the packet */
   uint64_t bytecount : 16;     /** 2b accumulated payload bytecount */
   uint64_t timer_info : 32;    /** 4b pointer to timer event */

   /* Word 2 */
   struct netdp_ip_in_addr    src;                /** 4b IPv4 source addresses */
   struct netdp_ip_in_addr    dst;                /** 4b IPv4 destination addresses */

} netdp_ip_ipq_elem_info;

/* Fragment descriptor structure */
typedef struct {
   /* Word 0 */

   /* Word 1 */
   uint64_t start_off : 16;
   uint64_t length : 16;
   uint64_t nbuf : 8;
   uint64_t first_buf_size : 16;
   uint64_t res0 : 8;
   /* Word 2 */
   uint64_t valid : 1;
   uint64_t res1: 23;
   uint64_t buffer_addr:40;
} netdp_ip_fdesc_t;

/* Fragment descriptor list structure */
typedef struct {
   /* Word 0 */
   uint64_t res0  : 1;
   uint64_t ndesc : 3;
   uint64_t res1  : 20;
   uint64_t next  : 40;
   /* Word 1 - Word 15 */
   netdp_ip_fdesc_t frag[5];
} netdp_ip_fdesc_list_t;


/* Fragment pointer (overflow) structure for reassembly queue */
typedef struct netdp_ip_ipq_next_t {
   uint64_t res : 24;
   uint64_t next : 40;
   netdp_ip_fdesc_t frag[5];
} netdp_ip_ipq_next;

/* Second level hash entry */
typedef struct netdp_ip_ipq_hl_entry_t {
   uint64_t val : 24;
   uint64_t ptr : 40;
} netdp_ip_ipq_hl_entry;

typedef struct {
   netdp_ip_ipq_elem_info info;
   netdp_ip_fdesc_t frag;
} netdp_ip_ipq_hte;

/* Reassembly queue element; may be one of three types
 * 	1) hash-table element
 *	2) stand-alone element
 *	3) second-level hash element */

typedef struct netdp_ip_ipq {
   netdp_ip_ipq_hdr hdr;
   union {
      struct {
	 netdp_ip_ipq_hte entry[2]; 
      } ipq_hte_list;
      struct {
         netdp_ip_fdesc_t frag[3];
         uint64_t res : 24;
         uint64_t next_ipq : 40;
         netdp_ip_ipq_elem_info info;
      } ipq_sae;
      struct {
         netdp_ip_ipq_hl_entry entry[5];
      } ipq_hlist;
   }ipq_type;
} netdp_ip_ipq;

#ifdef NETDP_IP_REASS_STATS
typedef struct netdp_ip_rass_stats_t {
  int64_t reass_in_progress;
  int64_t reass_dropped;
  int64_t reass_overlap;
  int64_t reass_timedout;
} netdp_ip_reass_stats_t;

extern  netdp_ip_reass_stats_t netdp_ip_reass_stats;
#endif

#define netdp_ip_ipq_ht   ipq_type.ipq_hte_list  /* ipq hash-table element */
#define netdp_ip_ipq_sa   ipq_type.ipq_sae    	/* ipq stand-alone element */
#define netdp_ip_ipq_hl   ipq_type.ipq_hlist     /* ipq 2nd-level hash element */

#define NETDP_IP_PAGE_COUNT 64
#define NETDP_IP_LINE_COUNT 16
#define NETDP_IP_BIT_COUNT  64

#define NETDP_IP_IPV4_HDR_LEN 20

void netdp_ip_reass_init(void);
int netdp_ip_ip_reass(struct rte_mbuf *swp);
void netdp_ip_reass_garbage_collector(void);
uint32_t tcp_calc_hw_tag(struct rte_mbuf *swp);
void netdp_ip_reass_ipq_sae_free(struct netdp_ip_ipq *ipq);
void netdp_ip_reass_ipq_walk_n_free(uint16_t idx);
uint32_t netdp_ip_reass_bucket_lookup(struct netdp_ip_ip* ip);
uint16_t netdp_ip_reass_tcp_checksum_verify(struct rte_mbuf *swp);


struct netdp_ip_ipq *netdp_ip_ipq_type2_alloc(struct netdp_ip_ipq *ipq, struct netdp_ip_ip *ip);
void netdp_ip_ipq_type0_alloc(struct netdp_ip_ipq *ipq, struct netdp_ip_ip *ip, uint8_t i, uint32_t idx);
void netdp_ip_reass_ipq_type2_free(struct netdp_ip_ipq *ipq, uint64_t hole_list);
void netdp_ip_reass_fdesc_merge_forward(netdp_ip_fdesc_t *fdesc, netdp_ip_fdesc_t *fdesc_next);
void netdp_ip_reass_prep_sw_wqe(struct rte_mbuf *swp, struct netdp_ip_ip *ip, netdp_ip_fdesc_t *fdesc);
uint8_t netdp_ip_ipq_type0_free(struct rte_mbuf *swp, netdp_ip_ipq *ipq, struct netdp_ip_ip *ip, 
					uint8_t i, uint32_t idx);
struct netdp_ip_ipq *netdp_ip_ipq_type2_walk(struct netdp_ip_ipq *ipq, struct netdp_ip_ip *ip, 
					uint64_t *hole_list);
void netdp_ip_reass_fdesc_move_up(netdp_ip_fdesc_list_t *fdesc_list, uint16_t copy_count, 
					uint16_t fdesc_off);
void netdp_ip_fdesc_list_write_fdesc(struct rte_mbuf *swp, struct netdp_ip_ipq *ipq, 
					struct netdp_ip_ip *ip, netdp_ip_fdesc_t *fdesc_ptr);
void netdp_ip_reass_ipq_type1_free(struct netdp_ip_ipq *ipq, uint64_t prev_list, 
					uint64_t hole_list, uint8_t hole_offset);
//netdp_ip_fdesc_t *netdp_ip_fdesc_walk_n_alloc(struct netdp_ip_ipq *ipq, struct rte_mbuf *swp, struct netdp_ip_ip *ip, 
void netdp_ip_fdesc_walk_n_alloc(struct netdp_ip_ipq *ipq, struct rte_mbuf *swp, struct netdp_ip_ip *ip, 
					uint64_t current_byte_offset);
struct netdp_ip_ipq *netdp_ip_ipq_type1_alloc(struct netdp_ip_ipq *ipq_ptr, uint64_t *hole_list, 
					uint8_t *hole_offset, uint32_t idx);
void netdp_ip_reass_fdesc_merge(struct rte_mbuf *swp, struct netdp_ip_ip* ip, netdp_ip_fdesc_t *fdesc, 
					uint16_t *byte_count, uint16_t overlap);
void netdp_ip_reass_fdesc_merge_behind(struct rte_mbuf *swp, struct netdp_ip_ip* ip, netdp_ip_fdesc_t *fdesc, 
					uint16_t *byte_count, uint16_t overlap); 
int netdp_ip_fdesc_list_move_down(netdp_ip_fdesc_t *prev, netdp_ip_fdesc_t *next, netdp_ip_fdesc_list_t *fdesc_list, 
					uint16_t fdesc_off, uint16_t copy_count);
void netdp_ip_reass_check_forward_merge(struct netdp_ip_ipq *ipq, netdp_ip_fdesc_list_t *fdesc_list, 
					netdp_ip_fdesc_t *fdesc_ptr, uint16_t fdesc_off, 
						uint16_t ndesc);
struct netdp_ip_ipq *netdp_ip_ipq_type1_walk(struct netdp_ip_ipq *ipq_ptr, struct netdp_ip_ip *ip, uint64_t *prev_list, 
					uint64_t *hole_list, uint8_t *hole_offset, 
						uint32_t idx);

#endif // _NETDP_IP_REASS_H__
