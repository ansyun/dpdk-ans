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


#ifndef __NETDP_ENET_H__
#define __NETDP_ENET_H__


#include <stdint.h>
#include <stdio.h>

#include "netdp_ip_if.h"
#include "netdp_ip_fastpath.h"
#include "netdp_queue.h"

#define NETDP_ENET_MAX_PHYS_IFACES 32	/* Actual number of physical interfaces */
#define NETDP_ENET_MAX_IFACES      64	/* Maximum number of total physical/virtual/
				 * aggregate interfaces supported */
#define NETDP_ENET_IFIDX_LOOPBACK (NETDP_ENET_MAX_IFACES - 1)
#define NETDP_ENET_IFIDX_INVALID  NETDP_ENET_MAX_IFACES

#define NETDP_ENET_ETHER_ADDR_LEN 6	/* Length of Ethernet MAC addresses */
#define NETDP_ENET_ETHER_TYPE_LEN 2	/* length of the Ethernet type field */
#define NETDP_ENET_ETHER_HDR_LEN  (NETDP_ENET_ETHER_ADDR_LEN*2+NETDP_ENET_ETHER_TYPE_LEN)
#define NETDP_ENET_ETHERMTU       1500     /* TODO: Need to consider VLAN info here */

#define NETDP_ENET_ETHERTYPE_IP	0x0800 /* IP protocol */
#define NETDP_ENET_ETHERTYPE_ARP	0x0806 /* ARP protocol */

/**
 * Global lock for interface modifications Global lock for interface
 * modifications
 */
extern  rte_spinlock_t netdp_enet_ifnet_lock;

//delete by wuxp for temp
#define NETDP_ENET_IFNET_GBL_LOCK_INIT()  rte_spinlock_init(&netdp_enet_ifnet_lock)
#define NETDP_ENET_IFNET_GBL_LOCK()      rte_spinlock_lock(&netdp_enet_ifnet_lock)
#define NETDP_ENET_IFNET_GBL_UNLOCK()    rte_spinlock_unlock(&netdp_enet_ifnet_lock)


/** Function to be registered for the specific interrupt */
typedef int (*netdp_send_packet_cb)(struct rte_mbuf *m, uint8_t port);

/*
 * Structure of the Ethernet header.
 */
struct  netdp_enet_ether_header {
   uint8_t  ether_dhost[NETDP_ENET_ETHER_ADDR_LEN];
   uint8_t  ether_shost[NETDP_ENET_ETHER_ADDR_LEN];
   uint16_t ether_type;
};

typedef enum {
   NETDP_ENET_IFT_FREE,
   NETDP_ENET_IFT_PHYS,
   NETDP_ENET_IFT_VLAN,
   NETDP_ENET_IFT_LOOP,		// Loop back
   NETDP_ENET_IFT_8023AD
} netdp_enet_if_type;

/* if_flags */
#define NETDP_ENET_IFF_UP                     0x01
#define NETDP_ENET_IFF_BROADCAST      0x02
#define NETDP_ENET_IFF_DEBUG              0x04	/* Turn on debugging.  */
#define NETDP_ENET_IFF_LOOPBACK        0x08	/* Is a loopback net.  */
#define NETDP_ENET_IFF_POINTOPOINT  0x10
#define NETDP_ENET_IFF_RUNNING         0x40      /* resources allocated */
#define NETDP_ENET_IFF_NOARP            0x80      /* no address resolution protocol */
#define NETDP_ENET_IFF_PPROMISC       0x100     /* Receive all packets.  */
#define	NETDP_ENET_IFF_STATICARP   0x80000	/* static ARP */

#define NETDP_ENET_IFF_CANTCHANGE \
        (NETDP_ENET_IFF_BROADCAST|NETDP_ENET_IFF_POINTOPOINT|NETDP_ENET_IFF_RUNNING|NETDP_ENET_IFF_PPROMISC)

#define NETDP_ENET_MAX_SIZE    18
/**
 * Ethernet broadcast address: ff:ff:ff:ff:ff:ff
 */
extern uint8_t netdp_enet_enetbcastaddr[];

/**
 *  Structure defining network interface statistics.
 */
typedef struct {
   // Byte counts
   uint64_t ifi_ibytes;             /**< total number of octets received */
   uint64_t ifi_obytes;             /**< total number of octets sent */

   // Packet counts
   uint32_t ifi_ipackets;           /**< packets received on interface */
   uint32_t ifi_opackets;           /**< packets sent on interface */

   // Multicast packet counts
   uint32_t ifi_imcasts;            /**< packets received via multicast */
   uint32_t ifi_omcasts;            /**< packets sent via multicast */

   // Error counts
   uint32_t ifi_ierrors;            /**< input errors on interface */
   uint32_t ifi_oerrors;            /**< output errors on interface */
   uint32_t ifi_iqdrops;            /**< dropped on input, this interface */
   uint32_t ifi_noproto;            /**< destined for unsupported protocol */

   uint64_t reserved0[10];
} netdp_enet_ifnet_stats_t;

NETDP_COMMON_TAILQ_HEAD(netdp_enet_ifaddrhead, netdp_ip_ifaddr); /* instantiation is preserved in the list */

struct netdp_ip_rtentry;

/**
 *  Structure defining a network interface.
 */
typedef struct {
   // Word 1
   uint64_t	if_type:   8;          /**< NETDP_ENET_IFT_PHYS,NETDP_ENET_IFT_VLAN,NETDP_ENET_IFT_8023AD,NETDP_ENET_IFT_FREE */
   uint64_t	if_flags: 24;          /**< up/down, broadcast, etc. */
   uint64_t	if_nxt_idx: 16;        /**< Next ifnet elelement */
   uint64_t	if_index: 16;          /**< Index of this element */

   // Word 2
   uint64_t	if_metric: 32;         /**< routing metric (external only) */
   uint64_t	if_aggreg_idx: 16;     /**< Aggregate link idx */
   uint64_t	if_mtu: 16;            /**< MTU for this interface */

   // Word 3
   uint64_t	if_vlan_id: 16;        /**< VLAN Id */
   uint64_t	if_real_idx:16;        /**< Real port for NETDP_ENET_IFT_VLAN:  * NETDP_ENET_IFT_PHYS or NETDP_ENET_IFT_8023AD */
   uint64_t	if_port:32;       /**< Physical port mask for NETDP_ENET_IFT_AGGREG */

   // Word 4
   uint16_t	if_reserved;
   uint8_t	if_addr[NETDP_ENET_ETHER_ADDR_LEN];/**< MAC address */

   // Words 5-6
   uint8_t       if_xname[NETDP_IP_IFNAMSIZ];     /**< external name (name + unit) */

   // Word 7
   uint32_t      if_dunit;               /**< unit or IF_DUNIT_NONE */
   uint32_t      if_pcount;              /**< number of promiscuous listeners */

   // Word 8
   uint32_t      if_capabilities;        /**< interface capabilities */
   uint32_t      if_capenable;           /**< enabled features */

   // Word 9-10
   struct netdp_enet_ifaddrhead	if_addrhead;	/**< linked list of addresses per if */
#define netdp_enet_if_addrlist if_addrhead

   /* procedure handles */
   // Word 11
   int          (*if_ioctl)             /** ioctl routine */
      (uint16_t, int, caddr_t);

   // Word 12
   void        (*if_init)              /** Initialization routine */
      (void *);

   // Word 13
   int        (*if_output)             /** Output routine */
      (uint16_t, struct rte_mbuf *, struct netdp_so_sockaddr *, struct netdp_ip_rtentry *, void *);

   // Word 14
   void	*if_afdata;		/**< Used by IPv6 */

   // Word 15
   uint32_t	if_afdata_initialized;
   rte_spinlock_t if_afdata_mtx;
   
   // Words 16 
   uint64_t	reserved;
} netdp_enet_ifnet_t;

/**
 * Array of netdp_enet_ifnet_t interfaces.
 */
extern  netdp_enet_ifnet_t netdp_enet_ifarr[NETDP_ENET_MAX_IFACES];

// ===================
// Function prototypes
// ===================

struct netdp_ip_rtentry;
struct netdp_so_sockaddr;

extern int netdp_enet_init(void);
extern uint64_t netdp_enet_input(struct rte_mbuf *m, uint8_t portid);
extern int netdp_enet_output(uint16_t, struct rte_mbuf*, struct netdp_so_sockaddr* , struct netdp_ip_rtentry*, void *);
extern uint64_t netdp_enet_if_attach(netdp_enet_ifnet_t *ifp, uint16_t ifidx);
extern void netdp_enet_in_arpinput(netdp_enet_ifnet_t *ifp, struct rte_mbuf *m);
extern int netdp_enet_ether_ioctl(uint16_t, int, caddr_t);
extern int netdp_enet_arpresolve(uint16_t ifidx, struct netdp_ip_rtentry *rt, struct rte_mbuf *m, struct netdp_so_sockaddr *dst, u_char *desten, struct netdp_ip_rtentry *rt0);
 
 
int netdp_register_callback(netdp_send_packet_cb cb);


#endif 
