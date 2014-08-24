
/** 
 * @file netdp_ip_if_var.h
 *
 * Description: Interface definitions.
 * File version info: $Id: netdp_ip_if_var.h,v 1.19 2006/01/31 01:29:38 harri Exp $ $Name: TCPIP_1_3_1_build_37 $
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

#ifndef __NETDP_IP_IF_VAR_H__
#define __NETDP_IP_IF_VAR_H__

#include "netdp_queue.h"
#include "netdp_enet.h"

struct netdp_ip_rtentry;
struct netdp_ip_rt_addrinfo;

/* Forward structure declarations */
#define NETDP_IP_IFA_LOCK(ifa)	NETDP_ENET_IFNET_GBL_LOCK();
#define NETDP_IP_IFA_UNLOCK(ifa)	NETDP_ENET_IFNET_GBL_UNLOCK();
#define	NETDP_IP_IFA_DESTROY(ifa) NETDP_IP_IFA_UNLOCK(ifa);

#define	NETDP_IP_IFAFREE(ifa)				\
do {						\
   NETDP_IP_IFA_LOCK(ifa);				\
      if (--(ifa)->ifa_refcnt == 0) {		\
	 NETDP_IP_IFA_DESTROY(ifa);			\
	    /* Change this to FPA free: NETDP_COMMON_FREE_TSAFE(ifa);*/		\
      } else 					\
	 NETDP_IP_IFA_UNLOCK(ifa);			\
} while (0)

#define NETDP_IP_IFAREF(ifa)		\
do {				\
   NETDP_IP_IFA_LOCK(ifa);		\
      ++(ifa->ifa_refcnt);	\
      NETDP_IP_IFA_UNLOCK(ifa);		\
} while(0);

/**
 * Generic interface address structure.
 */
struct netdp_ip_ifaddr { 
   // Word 1
   struct netdp_so_sockaddr	*ifa_addr;		/**< Pointer to L3 address of interface */

   // Word 2
   struct netdp_so_sockaddr	*ifa_dstaddr;		/**< Pointer to L3 bcast addr of interface */
#define netdp_ip_ifa_broadaddr ifa_dstaddr

   // Word 3
   struct netdp_so_sockaddr	*ifa_netmask;		/**< Pointer to L3 network mask -- used to determine subnet*/

   // Word 4
   /** check or clean routes (+ or -)'d */
   void		(*ifa_rtrequest) (int, struct netdp_ip_rtentry *, struct netdp_ip_rt_addrinfo *);

   // Words 5-6
   NETDP_COMMON_TAILQ_ENTRY(netdp_ip_ifaddr)	ifa_link;		/**< Next ifaddr */
#define netdp_ip_ifa_list ifa_link

   // Word 7
   uint16_t		ifa_ifidx;		/**< Back-pointer to L2 interface */
   uint16_t		ifa_flags;		/**< Mostly rt_flags for cloning */
   uint16_t		ifa_refcnt;		/**< Reference count */
   uint16_t		ifa_reserved;		/**< Reserved */

   // Word 8
   uint16_t		ifa_if_mtu;		/**< MTU of this interface (copied from ifnet) */
   uint8_t		ifa_if_addr[NETDP_ENET_ETHER_ADDR_LEN];	/**< L2 address of interface (copied from ifnet) */

   // Word 9
   uint64_t		ifa_if_type     : 8;	/**< Interface type (copied from ifnet) */
   uint64_t		ifa_if_flags    : 24;	/**< Interface flags (copied from ifnet) */
   uint64_t		ifa_if_vlan_id  : 16;	/**< vlad_id (copied from ifnet) */
   uint64_t		ifa_if_real_idx : 16;	/**< Real port for IFT_VLAN (copied from ifnet) */
};

struct	netdp_ip_ifaddr *netdp_ip_ifa_ifwithdstaddr(struct netdp_so_sockaddr *);
struct	netdp_ip_ifaddr *netdp_ip_ifa_ifwithnet(struct netdp_so_sockaddr *);
struct	netdp_ip_ifaddr *netdp_ip_ifa_ifwithroute(int, struct netdp_so_sockaddr *, struct netdp_so_sockaddr *);
struct	netdp_ip_ifaddr *netdp_ip_ifa_ifwithaddr(struct netdp_so_sockaddr *);
struct	netdp_ip_ifaddr *netdp_ip_ifaof_ifpforaddr(struct netdp_so_sockaddr *, uint32_t ifidx);

#define	NETDP_IP_IFA_ROUTE	NETDP_IP_RTF_UP		/**< route installed */

#define NETDP_IP_IF_LLADDR(ifidx) (netdp_enet_ifarr[(ifidx)].if_addr)

#endif /* __NETDP_IP_IF_VAR_H__ */
