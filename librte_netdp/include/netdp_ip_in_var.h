
/** 
 * @file netdp_ip_in_var.h
 *
 * Description: Interface address structure and related definitions.
 *
 * File version info: $Id: netdp_ip_in_var.h,v 1.15 2006/01/31 01:29:38 harri Exp $ $Name: TCPIP_1_3_1_build_37 $
 */

/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef __NETDP_IP_IN_VAR_H__
#define __NETDP_IP_IN_VAR_H__

#include "socket.h"
#include "netdp_ip_in.h"
#include "netdp_ip_if.h"
#include "netdp_ip_if_var.h"

struct socket;

typedef struct {
    uint32_t ifa_ipackets;
    uint32_t ifa_opackets;
    uint64_t ifa_ibytes;
    uint64_t ifa_obytes;
} netdp_ip_ipstats_t;

/**
 * in_ifaddr -- interface address structure 
 * for IPv4 protocol 
 *
 * Length of this structure is 256 bytes.
 * (Only first 128 bytes are used by fast-path processing.)
 *
 */
struct netdp_ip_in_ifaddr { 
    // Words 0-8
    struct		netdp_ip_ifaddr ia_ifa;     /**< Protocol-independent info */

#define netdp_ip_ia_ifidx    ia_ifa.ifa_ifidx
#define netdp_ip_ia_flags    ia_ifa.ifa_flags

    // Words 9-10
    NETDP_COMMON_LIST_ENTRY(netdp_ip_in_ifaddr) ia_hash; /**< Entry in bucket of inet addresses */

    // Word 11
    netdp_ip_ipstats_t		*ipstats;	/**< Pointer to interface statistics */

    // Words 12-13
    struct netdp_ip_sockaddr_in	ia_addr;	/**< L3 address of interface */

    // Words 14-15
    struct netdp_ip_sockaddr_in	ia_sockmask;	/**< L3 bcast address of interface */

    // Words 16-17
    struct netdp_ip_sockaddr_in	ia_dstaddr;	/**< L3 network mask -- used to determine subnet*/

#define netdp_ip_ia_broadaddr    ia_dstaddr

    // Words 18-20
    netdp_ip_ipstats_t		ia_ipstats;	/**< Interface statistics */

    NETDP_COMMON_TAILQ_ENTRY(netdp_ip_in_ifaddr) ia_link;	/**< List of internet addresses */

    // Words 21-31
    uint64_t		reserved[9];
};

struct	netdp_ip_in_aliasreq {
	char	netdp_ip_ifra_name[NETDP_IP_IFNAMSIZ];		/**< if name, e.g. "en0" */
	struct	netdp_ip_sockaddr_in ifra_addr;
	struct	netdp_ip_sockaddr_in ifra_broadaddr;
#define netdp_ip_ifra_dstaddr ifra_broadaddr
	struct	netdp_ip_sockaddr_in ifra_mask;
};

extern  NETDP_COMMON_TAILQ_HEAD(netdp_ip_in_ifaddrhead, netdp_ip_in_ifaddr) netdp_ip_in_ifaddrhead;

/*
 * Given a pointer to an netdp_ip_in_ifaddr (netdp_ip_ifaddr),
 * return a pointer to the addr as a netdp_ip_sockaddr_in.
 */

#define NETDP_IP_IA_SIN(ia)    (&(((struct netdp_ip_in_ifaddr *)(ia))->ia_addr))
#define NETDP_IP_IA_DSTSIN(ia) (&(((struct netdp_ip_in_ifaddr *)(ia))->ia_dstaddr))
#define NETDP_IP_IA_NETSIN(ia) (&(((struct netdp_ip_in_ifaddr *)(ia))->ia_netmask))

/*
 * Macro for finding the interface (ifnet structure) corresponding to one
 * of our IP addresses.
 */
#define NETDP_IP_INADDR_TO_IFP(addr, ifp)	\
{					\
   struct netdp_ip_in_ifaddr *ia;		\
   NETDP_COMMON_LIST_FOREACH(ia, NETDP_IP_INADDR_HASH((addr).s_addr), ia_hash) \
   if (NETDP_IP_IA_SIN(ia)->sin_addr.s_addr == (addr).s_addr) \
      break;					\
   (ifp) = (ia == NULL) ? NULL : &netdp_enet_ifarr[ia->netdp_ip_ia_ifidx];	\
}

int	netdp_ip_in_control(struct socket *, u_long, caddr_t, uint16_t);

#endif /* __NETDP_IP_IN_VAR_H__ */
