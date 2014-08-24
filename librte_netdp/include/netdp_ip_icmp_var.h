
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


#ifndef __NETDP_IP_ICMP_VAR_H__
#define __NETDP_IP_ICMP_VAR_H__


/*
 * Variables related to this implementation
 * of the internet control message protocol.
 */
struct	netdp_ip_icmpstat {
/* statistics related to icmp packets generated */
	uint32_t	icps_error;		/* # of calls to icmp_error */
	uint32_t	icps_oldshort;		/* no error 'cuz old ip too short */
	uint32_t	icps_oldicmp;		/* no error 'cuz old was icmp */
	uint32_t	icps_outhist[NETDP_IP_ICMP_MAXTYPE + 1];
/* statistics related to input messages processed */
	uint32_t	icps_badcode;		/* icmp_code out of range */
	uint32_t	icps_tooshort;		/* packet < ICMP_MINLEN */
	uint32_t	icps_checksum;		/* bad checksum */
	uint32_t	icps_badlen;		/* calculated bound mismatch */
	uint32_t	icps_reflect;		/* number of responses */
	uint32_t	icps_inhist[NETDP_IP_ICMP_MAXTYPE + 1];
	uint32_t	icps_bmcastecho;	/* b/mcast echo requests dropped */
	uint32_t	icps_bmcasttstamp;	/* b/mcast tstamp requests dropped */
	uint32_t	icps_badaddr;		/* bad return address */
	uint32_t	icps_noroute;		/* no route back */
};

/*
 * Names for ICMP sysctl objects
 */
#define	NETDP_IP_ICMPCTL_MASKREPL		1	/* allow replies to netmask requests */
#define	NETDP_IP_ICMPCTL_STATS		2	/* statistics (read-only) */
#define NETDP_IP_ICMPCTL_ICMPLIM		3
#define NETDP_IP_ICMPCTL_MAXID		4

#define NETDP_IP_ICMPCTL_NAMES { \
	{ 0, 0 }, \
	{ "maskrepl", CTLTYPE_INT }, \
	{ "stats", CTLTYPE_STRUCT }, \
	{ "icmplim", CTLTYPE_INT }, \
}

#define NETDP_IP_BANDLIM_UNLIMITED -1
#define NETDP_IP_BANDLIM_ICMP_UNREACH 0
#define NETDP_IP_BANDLIM_ICMP_ECHO 1
#define NETDP_IP_BANDLIM_ICMP_TSTAMP 2
#define NETDP_IP_BANDLIM_RST_CLOSEDPORT 3 /* No connection, and no listeners */
#define NETDP_IP_BANDLIM_RST_OPENPORT 4   /* No connection, listener */
#define NETDP_IP_BANDLIM_MAX 4

#endif /* __NETDP_IP_ICMP_VAR_H__ */
