
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


#ifndef __NETDP_IP_VAR_H__
#define __NETDP_IP_VAR_H__

/* flags passed to ip_output as last parameter */
#define	NETDP_IP_IP_FORWARDING		0x1		/* most of ip header exists */
#define	NETDP_IP_IP_RAWOUTPUT		0x2		/* raw ip header exists */
#define	NETDP_IP_IP_SENDONES		       0x4		/* send all-ones broadcast */
#define	NETDP_IP_IP_ROUTETOIF		NETDP_SO_SO_DONTROUTE	/* bypass routing tables */
#define	NETDP_IP_IP_ALLOWBROADCAST	NETDP_SO_SO_BROADCAST	/* can send broadcast packets */

//#ifdef NETDP_IP_STATISTICS
typedef struct {
        uint64_t  ips_total;              /* total packets received */
        uint64_t  ips_badsum;             /* checksum bad */
        uint64_t  ips_tooshort;           /* packet too short */
        uint64_t  ips_toosmall;           /* not enough data */
        uint64_t  ips_badhlen;            /* ip header length < data size */
        uint64_t  ips_badlen;             /* ip length < ip header length */
        uint64_t  ips_fragments;          /* fragments received */
        uint64_t  ips_fragdropped;        /* frags dropped (dups, out of space) */
        uint64_t  ips_fragtimeout;        /* fragments timed out */
        uint64_t  ips_forward;            /* packets forwarded */
        uint64_t  ips_fastforward;        /* packets fast forwarded */
        uint64_t  ips_cantforward;        /* packets rcvd for unreachable dest */
        uint64_t  ips_redirectsent;       /* packets forwarded on same net */
        uint64_t  ips_noproto;            /* unknown or unsupported protocol */
        uint64_t  ips_delivered;          /* datagrams delivered to upper level*/
        uint64_t  ips_localout;           /* total ip packets generated here */
        uint64_t  ips_odropped;           /* lost packets due to nobufs, etc. */
        uint64_t  ips_reassembled;        /* total packets reassembled ok */
        uint64_t  ips_fragmented;         /* datagrams successfully fragmented */
        uint64_t  ips_ofragments;         /* output fragments created */
        uint64_t  ips_cantfrag;           /* don't fragment flag was set, etc. */
        uint64_t  ips_badoptions;         /* error in option processing */
        uint64_t  ips_noroute;            /* packets discarded due to no route */
        uint64_t  ips_badvers;            /* ip version != 4 */
        uint64_t  ips_rawout;             /* total raw ip packets generated */
        uint64_t  ips_toolong;            /* ip length > max ip packet size */
        uint64_t  ips_notmember;          /* multicasts for unregistered grps */
        uint64_t  ips_nogif;              /* no match gif found */
        uint64_t  ips_badaddr;            /* invalid address on header */
} netdp_ip_ipstat_t;
//#endif

//
//  Keep NETDP_IP_IPID_BLOCK_SIZE a power of two for higher performance
//
#define NETDP_IP_IPID_BLOCK_LOG2_SIZE	4
#define NETDP_IP_IPID_BLOCK_SIZE		(1 << NETDP_IP_IPID_BLOCK_LOG2_SIZE)
#define NETDP_IP_IPID_BLOCK_MASK		(NETDP_IP_IPID_BLOCK_SIZE - 1)
#define NETDP_IP_PSEUDO_RANDOM_IPID


#define NETDP_IP_STATISTICS  /* Enable IP statistics */

#ifdef NETDP_IP_STATISTICS
#define NETDP_IPSTAT_INC(param)  param++;
#endif

#endif /* __NETDP_IP_VAR_H__ */
