
/** 
 * @file netdp_ip_route.h
 *
 * Description: Routing definitions.
 *
 * File version info: $Id: netdp_ip_route.h,v 1.23 2006/01/05 22:36:31 fmasood Exp $ $Name: TCPIP_1_3_1_build_37 $
 */

/*
 * Copyright (c) 1980, 1986, 1993
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *	@(#)route.h	8.4 (Berkeley) 1/9/95
 * $FreeBSD: src/sys/net/route.h,v 1.54 2003/11/20 20:07:37 andre Exp $
 */

#ifndef __NETDP_IP_ROUTE_H__
#define __NETDP_IP_ROUTE_H__

#include "netdp_enet.h"
#include "socket.h"

/*
 * Kernel resident routing tables.
 *
 * The routing tables are initialized when interface addresses
 * are set by making entries for all directly connected interfaces.
 */

/*
 * A route consists of a destination address and a reference
 * to a routing entry.  These are often held by protocols
 * in their control blocks, e.g. inpcb.
 */
struct netdp_ip_route {
	struct	netdp_ip_rtentry *ro_rt;
	struct	netdp_so_sockaddr ro_dst;
};

/*
 * These numbers are used by reliable protocols for determining
 * retransmission behavior and are included in the routing structure.
 */
struct netdp_ip_rt_metrics_lite {
	u_long	rmx_mtu;	/* MTU for this path */
	u_long	rmx_expire;	/* lifetime for route, e.g. redirect */
	u_long	rmx_pksent;	/* packets sent using this route */
};

struct netdp_ip_rt_metrics {
	u_long	rmx_locks;	/* Kernel must leave these values alone */
	u_long	rmx_mtu;	/* MTU for this path */
	u_long	rmx_hopcount;	/* max hops expected */
	u_long	rmx_expire;	/* lifetime for route, e.g. redirect */
	u_long	rmx_recvpipe;	/* inbound delay-bandwidth product */
	u_long	rmx_sendpipe;	/* outbound delay-bandwidth product */
	u_long	rmx_ssthresh;	/* outbound gateway buffer limit */
	u_long	rmx_rtt;	/* estimated round trip time */
	u_long	rmx_rttvar;	/* estimated rtt variance */
	u_long	rmx_pksent;	/* packets sent using this route */
	u_long	rmx_filler[4];	/* will be used for T/TCP later */
};

/*
 * rmx_rtt and rmx_rttvar are stored as microseconds;
 * RTTTOPRHZ(rtt) converts to a value suitable for use
 * by a protocol slowtimo counter.
 */
#define	NETDP_IP_RTM_RTTUNIT	1000000	/* units for rtt, rttvar, as units per sec */
#define	NETDP_IP_RTTTOPRHZ(r)	((r) / (NETDP_IP_RTM_RTTUNIT / NETDP_IP_PR_SLOWHZ))

/*
 * XXX kernel function pointer `rt_output' is visible to applications.
 */
struct mbuf;

/*
 * We distinguish between routes to hosts and routes to networks,
 * preferring the former if available.  For each route we infer
 * the interface to use from the gateway address supplied when
 * the route was entered.  Routes that forward packets through
 * gateways are marked so that the output routines know to address the
 * gateway rather than the ultimate destination.
 */
#ifndef NETDP_IP_RNF_NORMAL
#include "netdp_ip_radix.h"
#endif
struct netdp_ip_rtentry {
	struct	netdp_ip_radix_node rt_nodes[2];	/* tree glue, and other values */
#define	netdp_ip_rt_key(r)	((struct netdp_so_sockaddr *)((r)->rt_nodes->netdp_ip_rn_key))
#define	netdp_ip_rt_mask(r)	((struct netdp_so_sockaddr *)((r)->rt_nodes->netdp_ip_rn_mask))
	struct	netdp_so_sockaddr *rt_gateway;	/* value */
	long	rt_refcnt;		/* # held references */
	u_long	rt_flags;		/* up/down?, host/net */
	uint16_t rt_ifidx;		/* the answer: interface to use */
	struct	netdp_ip_ifaddr *rt_ifa;		/* the answer: interface address to use */
	struct	netdp_so_sockaddr *rt_genmask;	/* for generation of cloned routes */
	caddr_t	rt_llinfo;		/* pointer to link level info cache */
	struct	netdp_ip_rt_metrics_lite rt_rmx;	/* metrics used by rx'ing protocols */
	struct	netdp_ip_rtentry *rt_gwroute;	/* implied entry for gatewayed routes */
	int	(*rt_output)(netdp_enet_ifnet_t *, struct mbuf *, struct netdp_so_sockaddr *,
		    struct netdp_ip_rtentry *);
					/* output routine for this (rt,if) */
	struct	netdp_ip_rtentry *rt_parent; 	/* cloning parent of this route */
	/* XXX ugly, user apps use this definition but don't have a mtx def */
	rte_spinlock_t	rt_lock;	/* spinlock for routing entry */
};

/*
 * Following structure necessary for 4.3 compatibility;
 * We should eventually move it to a compat file.
 */
struct netdp_ip_ortentry {
	u_long	rt_hash;		/* to speed lookups */
	struct	netdp_so_sockaddr rt_dst;	/* key */
	struct	netdp_so_sockaddr rt_gateway;	/* value */
	short	rt_flags;		/* up/down?, host/net */
	short	rt_refcnt;		/* # held references */
	u_long	rt_use;			/* raw # packets forwarded */
	uint32_t rt_ifidx;		/* the answer: interface to use */
};

#define netdp_ip_rt_use rt_rmx.rmx_pksent

#define	NETDP_IP_RTF_UP		0x1		/* route usable */
#define	NETDP_IP_RTF_GATEWAY	0x2		/* destination is a gateway */
#define	NETDP_IP_RTF_HOST	0x4		/* host entry (net otherwise) */
#define	NETDP_IP_RTF_REJECT	0x8		/* host or net unreachable */
#define	NETDP_IP_RTF_DYNAMIC	0x10		/* created dynamically (by redirect) */
#define	NETDP_IP_RTF_MODIFIED	0x20		/* modified dynamically (by redirect) */
#define NETDP_IP_RTF_DONE	0x40		/* message confirmed */
/*			0x80		   unused, was RTF_DELCLONE */
#define NETDP_IP_RTF_CLONING	0x100		/* generate new routes on use */
#define NETDP_IP_RTF_XRESOLVE	0x200		/* external daemon resolves name */
#define NETDP_IP_RTF_LLINFO	0x400		/* generated by link layer (e.g. ARP) */
#define NETDP_IP_RTF_STATIC	0x800		/* manually added */
#define NETDP_IP_RTF_BLACKHOLE	0x1000		/* just discard pkts (during updates) */
#define NETDP_IP_RTF_PROTO2	0x4000		/* protocol specific routing flag */
#define NETDP_IP_RTF_PROTO1	0x8000		/* protocol specific routing flag */

/* XXX: temporary to stay API/ABI compatible with userland */
#define NETDP_IP_RTF_PRCLONING	0x10000		/* unused, for compatibility */

#define NETDP_IP_RTF_WASCLONED	0x20000		/* route generated through cloning */
#define NETDP_IP_RTF_PROTO3	0x40000		/* protocol specific routing flag */
/*			0x80000		   unused */
#define NETDP_IP_RTF_PINNED	0x100000	/* future use */
#define	NETDP_IP_RTF_LOCAL	0x200000 	/* route represents a local address */
#define	NETDP_IP_RTF_BROADCAST	0x400000	/* route represents a bcast address */
#define	NETDP_IP_RTF_MULTICAST	0x800000	/* route represents a mcast address */
					/* 0x1000000 and up unassigned */

/*
 * Routing statistics.
 */
struct	netdp_ip_rtstat {
	short	rts_badredirect;	/* bogus redirect calls */
	short	rts_dynamic;		/* routes created by redirects */
	short	rts_newgateway;		/* routes modified by redirects */
	short	rts_unreach;		/* lookups which failed */
	short	rts_wildcard;		/* lookups satisfied by a wildcard */
};
/*
 * Structures for routing messages.
 */
struct netdp_ip_rt_msghdr {
	u_short	rtm_msglen;	/* to skip over non-understood messages */
	u_char	rtm_version;	/* future binary compatibility */
	u_char	rtm_type;	/* message type */
	u_short	rtm_index;	/* index for associated ifp */
	int	rtm_flags;	/* flags, incl. kern & message, e.g. DONE */
	int	rtm_addrs;	/* bitmask identifying sockaddrs in msg */
	pid_t	rtm_pid;	/* identify sender */
	int	rtm_seq;	/* for sender to identify action */
	int	rtm_errno;	/* why failed */
	int	rtm_use;	/* from rtentry */
	u_long	rtm_inits;	/* which metrics we are initializing */
	struct	netdp_ip_rt_metrics rtm_rmx; /* metrics themselves */
};

#define NETDP_IP_RTM_VERSION	5	/* Up the ante and ignore older versions */

/*
 * Message types.
 */
#define NETDP_IP_RTM_ADD		0x1	/* Add Route */
#define NETDP_IP_RTM_DELETE	0x2	/* Delete Route */
#define NETDP_IP_RTM_CHANGE	0x3	/* Change Metrics or flags */
#define NETDP_IP_RTM_GET		0x4	/* Report Metrics */
#define NETDP_IP_RTM_LOSING	0x5	/* Kernel Suspects Partitioning */
#define NETDP_IP_RTM_REDIRECT	0x6	/* Told to use different route */
#define NETDP_IP_RTM_MISS		0x7	/* Lookup failed on this address */
#define NETDP_IP_RTM_LOCK		0x8	/* fix specified metrics */
#define NETDP_IP_RTM_OLDADD	0x9	/* caused by SIOCADDRT */
#define NETDP_IP_RTM_OLDDEL	0xa	/* caused by SIOCDELRT */
#define NETDP_IP_RTM_RESOLVE	0xb	/* req to resolve dst to LL addr */
#define NETDP_IP_RTM_NEWADDR	0xc	/* address being added to iface */
#define NETDP_IP_RTM_DELADDR	0xd	/* address being removed from iface */
#define NETDP_IP_RTM_IFINFO	0xe	/* iface going up/down etc. */
#define	NETDP_IP_RTM_NEWMADDR	0xf	/* mcast group membership being added to if */
#define	NETDP_IP_RTM_DELMADDR	0x10	/* mcast group membership being deleted */
#define	NETDP_IP_RTM_IFANNOUNCE	0x11	/* iface arrival/departure */

/*
 * Bitmask values for rtm_inits and rmx_locks.
 */
#define NETDP_IP_RTV_MTU		0x1	/* init or lock _mtu */
#define NETDP_IP_RTV_HOPCOUNT	0x2	/* init or lock _hopcount */
#define NETDP_IP_RTV_EXPIRE	0x4	/* init or lock _expire */
#define NETDP_IP_RTV_RPIPE	0x8	/* init or lock _recvpipe */
#define NETDP_IP_RTV_SPIPE	0x10	/* init or lock _sendpipe */
#define NETDP_IP_RTV_SSTHRESH	0x20	/* init or lock _ssthresh */
#define NETDP_IP_RTV_RTT		0x40	/* init or lock _rtt */
#define NETDP_IP_RTV_RTTVAR	0x80	/* init or lock _rttvar */

/*
 * Bitmask values for rtm_addrs.
 */
#define NETDP_IP_RTA_DST		0x1	/* destination sockaddr present */
#define NETDP_IP_RTA_GATEWAY	0x2	/* gateway sockaddr present */
#define NETDP_IP_RTA_NETMASK	0x4	/* netmask sockaddr present */
#define NETDP_IP_RTA_GENMASK	0x8	/* cloning mask sockaddr present */
#define NETDP_IP_RTA_IFP		0x10	/* interface name sockaddr present */
#define NETDP_IP_RTA_IFA		0x20	/* interface addr sockaddr present */
#define NETDP_IP_RTA_AUTHOR	0x40	/* sockaddr for author of redirect */
#define NETDP_IP_RTA_BRD		0x80	/* for NEWADDR, broadcast or p-p dest addr */

/*
 * Index offsets for sockaddr array for alternate internal encoding.
 */
#define NETDP_IP_RTAX_DST		0	/* destination sockaddr present */
#define NETDP_IP_RTAX_GATEWAY	1	/* gateway sockaddr present */
#define NETDP_IP_RTAX_NETMASK	2	/* netmask sockaddr present */
#define NETDP_IP_RTAX_GENMASK	3	/* cloning mask sockaddr present */
#define NETDP_IP_RTAX_IFP		4	/* interface name sockaddr present */
#define NETDP_IP_RTAX_IFA		5	/* interface addr sockaddr present */
#define NETDP_IP_RTAX_AUTHOR	6	/* sockaddr for author of redirect */
#define NETDP_IP_RTAX_BRD		7	/* for NEWADDR, broadcast or p-p dest addr */
#define NETDP_IP_RTAX_MAX		8	/* size of array to allocate */

struct netdp_ip_rt_addrinfo {
	int	rti_addrs;
	struct	netdp_so_sockaddr *rti_info[NETDP_IP_RTAX_MAX];
	int	rti_flags;
	struct	netdp_ip_ifaddr *rti_ifa;
	uint32_t rti_ifidx;
};



#if 0
#define NETDP_IP_PRINT_FN3(format, args...) \
   NETDP_COMMON_PRINT_FN(format, ##args);
#define NETDP_IP_PRINT_FN2(format, args...) \
   NETDP_COMMON_PRINT_FN(format, ##args);
#else
#define NETDP_IP_PRINT_FN2(format, args...)
#define NETDP_IP_PRINT_FN3(format, args...) 
#endif

#define	NETDP_IP_RT_LOCK_INIT(_rt) NETDP_IP_PRINT_FN2("rte_spinlock_init(%p) \n", &(_rt)->rt_lock);\
			  rte_spinlock_init(&(_rt)->rt_lock)
#define	NETDP_IP_RT_LOCK(_rt) rte_spinlock_lock(&(_rt)->rt_lock);	\
		     NETDP_IP_PRINT_FN2("rte_spinlock_lock(%p) \n", &(_rt)->rt_lock)
#define	NETDP_IP_RT_UNLOCK(_rt) rte_spinlock_unlock(&(_rt)->rt_lock);	\
	             NETDP_IP_PRINT_FN2("rte_spinlock_unlock(%p) \n", &(_rt)->rt_lock);
#define	NETDP_IP_NETDP_RT_LOCK_INIT(_rt) rte_spinlock_init(&(_rt)->rt_lock);	\
	                      NETDP_IP_PRINT_FN2("rte_spinlock_init(%p) \n", &(_rt)->rt_lock)
#define NETDP_IP_NETDP_RT_LOCK(_rt) rte_spinlock_lock(&(_rt)->rt_lock);	\
                         NETDP_IP_PRINT_FN2("rte_spinlock_lock(%p) \n", &(_rt)->rt_lock)
#define NETDP_IP_NETDP_RT_UNLOCK(_rt) rte_spinlock_unlock(&(_rt)->rt_lock);	\
                           NETDP_IP_PRINT_FN2("rte_spinlock_unlock(%p) \n", &(_rt)->rt_lock)

#define	NETDP_IP_RT_LOCK_DESTROY(_rt)	
#define	NETDP_IP_RT_LOCK_ASSERT(_rt)

#define	NETDP_IP_RT_ADDREF(_rt)	do {					\
	NETDP_IP_RT_LOCK_ASSERT(_rt);					\
	NETDP_COMMON_KASSERT((_rt)->rt_refcnt >= 0,				\
		("negative refcnt %ld", (_rt)->rt_refcnt));	\
	(_rt)->rt_refcnt++;					\
	NETDP_IP_PRINT_FN2("RT_ADDREF %p to %d \n", _rt, (_rt)->rt_refcnt);	\
} while (0);
#define	NETDP_IP_RT_REMREF(_rt)	do {					\
	NETDP_IP_RT_LOCK_ASSERT(_rt);					\
	NETDP_COMMON_KASSERT((_rt)->rt_refcnt > 0,				\
		("bogus refcnt %ld", (_rt)->rt_refcnt));	\
	(_rt)->rt_refcnt--;					\
	NETDP_IP_PRINT_FN2("RT_REMREF %p to %d \n", _rt, (_rt)->rt_refcnt);	\
} while (0);

#define	NETDP_IP_RTFREE_LOCKED(_rt) do {					\
	NETDP_IP_PRINT_FN2("RTFREE_LOCKED start \n");	\
		if ((_rt)->rt_refcnt <= 1)			\
			netdp_ip_rtfree(_rt);				\
		else {						\
			NETDP_IP_RT_REMREF(_rt);				\
			NETDP_IP_RT_UNLOCK(_rt);				\
		}						\
		/* guard against invalid refs */		\
		_rt = 0;					\
	NETDP_IP_PRINT_FN2("RTFREE_LOCKED end \n");	\
	} while (0)
#define	NETDP_IP_RTFREE(_rt) do {					\
	NETDP_IP_PRINT_FN2("NETDP_IP_RT_FREE start \n");	\
		NETDP_IP_RT_LOCK(_rt);					\
		NETDP_IP_RTFREE_LOCKED(_rt);				\
	NETDP_IP_PRINT_FN2("NETDP_IP_RT_FREE end \n");	\
	} while (0)

#define	NETDP_IP_RO_RTFREE(_ro) do {					\
	if ((_ro)->ro_rt) {					\
			NETDP_IP_RT_LOCK((_ro)->ro_rt);			\
			NETDP_IP_RTFREE_LOCKED((_ro)->ro_rt);		\
	}							\
} while (0)


extern struct netdp_ip_radix_node_head *netdp_ip_rt_tables[NETDP_SO_AF_MAX+1];

struct netdp_ip_ifmultiaddr;

void	 netdp_ip_route_init(void);
int	 netdp_ip_rt_getifa(struct netdp_ip_rt_addrinfo *);
void	 netdp_ip_rt_ifannouncemsg(netdp_enet_ifnet_t *, int);
void	 netdp_ip_rt_ifmsg(netdp_enet_ifnet_t *);
void	 netdp_ip_rt_missmsg(int, struct netdp_ip_rt_addrinfo *, int, int);
void	 netdp_ip_rt_newaddrmsg(int, struct netdp_ip_ifaddr *, int, struct netdp_ip_rtentry *);
void	 netdp_ip_rt_newmaddrmsg(int, struct netdp_ip_ifmultiaddr *);
void	 netdp_ip_rtalloc(struct netdp_ip_route *, uint64_t locking);
int	 netdp_ip_rt_setgate(struct netdp_ip_rtentry *, struct netdp_so_sockaddr *, struct netdp_so_sockaddr *);
void	 netdp_ip_rtalloc_ign(struct netdp_ip_route *, u_long);
/* NB: the rtentry is returned locked */
struct netdp_ip_rtentry *netdp_ip_rtalloc1(struct netdp_so_sockaddr *, int, u_long);
int	 netdp_ip_rtexpunge(struct netdp_ip_rtentry *);
void	 netdp_ip_rtfree(struct netdp_ip_rtentry *);
int	 netdp_ip_rtinit(struct netdp_ip_ifaddr *, int, int);
int	 netdp_ip_rtioctl(u_long, caddr_t);
void	 netdp_ip_rtredirect(struct netdp_so_sockaddr *, struct netdp_so_sockaddr *,
	    struct netdp_so_sockaddr *, int, struct netdp_so_sockaddr *);
int	 netdp_ip_rtrequest(int, struct netdp_so_sockaddr *,
	    struct netdp_so_sockaddr *, struct netdp_so_sockaddr *, int, struct netdp_ip_rtentry **);
int	 netdp_ip_rtrequest1(int, struct netdp_ip_rt_addrinfo *, struct netdp_ip_rtentry **);
static inline int netdp_ip_rt_check(struct netdp_ip_rtentry **lrt, struct netdp_ip_rtentry **lrt0, struct netdp_so_sockaddr *dst);
int	 netdp_ip_rt_check_full(struct netdp_ip_rtentry **, struct netdp_ip_rtentry **, struct netdp_so_sockaddr *);

void	netdp_ip_init_route_cache(void);

#ifdef NETDP_ROUTE_DEBUG	// { 
void	netdp_ip_route_debug_print(void);
#endif // } NETDP_ROUTE_DEBUG

static inline int netdp_ip_rt_check(struct netdp_ip_rtentry **lrt, struct netdp_ip_rtentry **lrt0, struct netdp_so_sockaddr *dst)
{
   struct netdp_ip_rtentry *rt;
   struct netdp_ip_rtentry *rt0;
   rt0 = *lrt0;
   rt = rt0;

   if (rt) {
      if ((rt->rt_flags & (NETDP_IP_RTF_UP | NETDP_IP_RTF_GATEWAY)) == NETDP_IP_RTF_UP) 
	 goto done;
      else {
	 if (rt->rt_gwroute) {
	    rt = rt->rt_gwroute;
	    if (rt->rt_flags & NETDP_IP_RTF_UP)
	       goto done;
	 }
      }
   }

   // Slow-path rt_check
   netdp_ip_rt_check_full(lrt, lrt0, dst);
   return(0);

done:
   *lrt = rt;
   *lrt0 = rt0;
   return (0);
}

#endif // __NETDP_IP_ROUTE_H__
