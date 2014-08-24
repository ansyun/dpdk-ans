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


#ifndef __NETDP_IP_IN_H__
#define __NETDP_IP_IN_H__

#if defined(__KERNEL__) && defined(linux)
/* Nothing*/
#else
#include <stdint.h>
#endif

#include "netdp_enet.h"

/* Standard well-known ports.  */
enum
  {
    CMV_IP_IPPORT_ECHO = 7,            /* Echo service.  */
    CMV_IP_IPPORT_DISCARD = 9,         /* Discard transmissions service.  */
    CMV_IP_IPPORT_SYSTAT = 11,         /* System status service.  */
    CMV_IP_IPPORT_DAYTIME = 13,        /* Time of day service.  */
    CMV_IP_IPPORT_NETSTAT = 15,        /* Network status service.  */
    CMV_IP_IPPORT_FTP = 21,            /* File Transfer Protocol.  */
    CMV_IP_IPPORT_TELNET = 23,         /* Telnet protocol.  */
    CMV_IP_IPPORT_SMTP = 25,           /* Simple Mail Transfer Protocol.  */
    CMV_IP_IPPORT_TIMESERVER = 37,     /* Timeserver service.  */
    CMV_IP_IPPORT_NAMESERVER = 42,     /* Domain Name Service.  */
    CMV_IP_IPPORT_WHOIS = 43,          /* Internet Whois service.  */
    CMV_IP_IPPORT_MTP = 57,
                                                                                                              
    CMV_IP_IPPORT_TFTP = 69,           /* Trivial File Transfer Protocol.  */
    CMV_IP_IPPORT_RJE = 77,
    CMV_IP_IPPORT_FINGER = 79,         /* Finger service.  */
    CMV_IP_IPPORT_TTYLINK = 87,
    CMV_IP_IPPORT_SUPDUP = 95,         /* SUPDUP protocol.  */
                                                                                                              
                                                                                                              
    CMV_IP_IPPORT_EXECSERVER = 512,    /* execd service.  */
    CMV_IP_IPPORT_LOGINSERVER = 513,   /* rlogind service.  */
    CMV_IP_IPPORT_CMDSERVER = 514,
    CMV_IP_IPPORT_EFSSERVER = 520,
                                                                                                              
    /* UDP ports.  */
    CMV_IP_IPPORT_BIFFUDP = 512,
    CMV_IP_IPPORT_WHOSERVER = 513,
    CMV_IP_IPPORT_ROUTESERVER = 520,
                                                                                                              
    /* Ports less than this value are reserved for privileged processes.  */
    CMV_IP_IPPORT_RESERVED = 1024,
                                                                                                              
    /* Ports greater this value are reserved for (non-privileged) servers.  */
    CMV_IP_IPPORT_USERRESERVED = 5000
  };

/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).         (IP_PORTRANGE_LOW)
 */
#define NETDP_IP_IPPORT_RESERVED         1024
                                                                                                              
/*
 * Default local port range, used by both IP_PORTRANGE_DEFAULT
 * and IP_PORTRANGE_HIGH.
 */
#define NETDP_IP_IPPORT_HIFIRSTAUTO      49152
#define NETDP_IP_IPPORT_HILASTAUTO       65535
                                                                                                              
/*
 * Scanning for a free reserved port return a value below IPPORT_RESERVED,
 * but higher than IPPORT_RESERVEDSTART.  Traditionally the start value was
 * 512, but that conflicts with some well-known-services that firewalls may
 * have a fit if we use.
 */
#define NETDP_IP_IPPORT_RESERVEDSTART    600
                                                                                                              
#define NETDP_IP_IPPORT_MAX              65535


typedef uint16_t netdp_ip_in_port_t;
typedef uint32_t netdp_ip_in_addr_t;

#define	NETDP_IP_INADDR_ANY	(uint32_t)0x00000000
#define	NETDP_IP_INADDR_BROADCAST	(uint32_t)0xffffffff	/* must be masked */

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define	NETDP_IP_IN_CLASSA(i)		(((uint32_t)(i) & 0x80000000) == 0)
#define	NETDP_IP_IN_CLASSA_NET		0xff000000
#define	NETDP_IP_IN_CLASSA_NSHIFT	24
#define	NETDP_IP_IN_CLASSA_HOST		0x00ffffff
#define	NETDP_IP_IN_CLASSA_MAX		128

#define	NETDP_IP_IN_CLASSB(i)		(((uint32_t)(i) & 0xc0000000) == 0x80000000)
#define	NETDP_IP_IN_CLASSB_NET		0xffff0000
#define	NETDP_IP_IN_CLASSB_NSHIFT	16
#define	NETDP_IP_IN_CLASSB_HOST		0x0000ffff
#define	NETDP_IP_IN_CLASSB_MAX		65536

#define	NETDP_IP_IN_CLASSC(i)		(((uint32_t)(i) & 0xe0000000) == 0xc0000000)
#define	NETDP_IP_IN_CLASSC_NET		0xffffff00
#define	NETDP_IP_IN_CLASSC_NSHIFT	8
#define	NETDP_IP_IN_CLASSC_HOST		0x000000ff

#define	NETDP_IP_IN_CLASSD(i)		(((uint32_t)(i) & 0xf0000000) == 0xe0000000)
#define	NETDP_IP_IN_CLASSD_NET		0xf0000000	/* These ones aren't really */
#define	NETDP_IP_IN_CLASSD_NSHIFT	28		/* net and host fields, but */
#define	NETDP_IP_IN_CLASSD_HOST		0x0fffffff	/* routing needn't know.    */
#define	NETDP_IP_IN_MULTICAST(i)		NETDP_IP_IN_CLASSD(i)

#define	NETDP_IP_IN_EXPERIMENTAL(i)	(((uint32_t)(i) & 0xf0000000) == 0xf0000000)
#define	NETDP_IP_IN_BADCLASS(i)		(((uint32_t)(i) & 0xf0000000) == 0xf0000000)

#define NETDP_IP_IN_LINKLOCAL(i)		(((u_int32_t)(i) & 0xffff0000) == 0xa9fe0000)
#define NETDP_IP_IN_LOOPBACK(i)	  	       (((u_int32_t)(i) & 0xff000000) == 0x7f000000)
#define NETDP_IP_IN_ZERONET(i)		       (((u_int32_t)(i) & 0xff000000) == 0)


#define	NETDP_IP_INADDR_LOOPBACK		(uint32_t)0x7f000001

#define	NETDP_IP_INADDR_UNSPEC_GROUP	(u_int32_t)0xe0000000	/* 224.0.0.0 */
#define	NETDP_IP_INADDR_ALLHOSTS_GROUP	(u_int32_t)0xe0000001	/* 224.0.0.1 */
#define	NETDP_IP_INADDR_ALLRTRS_GROUP	(u_int32_t)0xe0000002	/* 224.0.0.2 */
#define	NETDP_IP_INADDR_ALLMDNS_GROUP	(u_int32_t)0xe00000fb	/* 224.0.0.251 */
#define	NETDP_IP_INADDR_MAX_LOCAL_GROUP	(u_int32_t)0xe00000ff	/* 224.0.0.255 */

#define	NETDP_IP_IN_LOOPBACKNET		127			/* official! */

#define NETDP_IP_IPPROTO_MAX 256

/* Internet address (a structure for historical reasons). */
#ifndef _NETDP_IP_STRUCT_IN_ADDR_DECLARED
struct netdp_ip_in_addr {
    netdp_ip_in_addr_t s_addr;
};
#define _NETDP_IP_STRUCT_IN_ADDR_DECLARED
#endif

/* Socket address, internet style. */
struct netdp_ip_sockaddr_in {
    uint8_t			sin_len;
    uint8_t			sin_family;
    netdp_ip_in_port_t		sin_port;
    struct netdp_ip_in_addr	sin_addr;
    char			sin_zero[8];
};

#define NETDP_ENET_SIN(s) ((struct netdp_ip_sockaddr_in *)s)
#define NETDP_ENET_SDL(s) ((struct netdp_ip_sockaddr_dl *)s)


int netdp_ip_in_localaddr(struct netdp_ip_in_addr in);
int netdp_ip_in_broadcast(struct netdp_ip_in_addr, uint16_t ifidx);
extern char *netdp_ip_inet_ntoa(struct netdp_ip_in_addr); /* in libkern */

#define netdp_ip_in_nullhost(x)	((x).s_addr == NETDP_IP_INADDR_ANY)
#define netdp_ip_satosin(sa)	       ((struct netdp_ip_sockaddr_in *)(sa))
#define netdp_ip_sintosa(sin)         ((struct netdp_so_sockaddr *)(sin))
#define netdp_ip_ifatoia(ifa)	      ((struct netdp_ip_in_ifaddr *)(ifa))

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define NETDP_IP_IP_FW_TABLE_ADD            40   /* add entry */
#define NETDP_IP_IP_FW_TABLE_DEL             41   /* delete entry */
#define NETDP_IP_IP_FW_TABLE_FLUSH         42   /* flush table */
#define NETDP_IP_IP_FW_TABLE_GETSIZE     43   /* get table size */
#define NETDP_IP_IP_FW_TABLE_LIST            44   /* list table contents */

#define NETDP_IP_IP_FW_ADD               50   /* add a firewall rule to chain */
#define NETDP_IP_IP_FW_DEL                51   /* delete a firewall rule from chain */
#define NETDP_IP_IP_FW_FLUSH            52   /* flush firewall rule chain */
#define NETDP_IP_IP_FW_ZERO              53   /* clear single/all firewall counter(s) */
#define NETDP_IP_IP_FW_GET                54   /* get entire firewall rule chain */
#define NETDP_IP_IP_FW_RESETLOG      55   /* reset logging counters */

#define NETDP_IP_IP_FW_CALLBACK_LIST     56   /* Retrieve list of callback functions */


#endif /* __NETDP_IP_IN_H__ */
