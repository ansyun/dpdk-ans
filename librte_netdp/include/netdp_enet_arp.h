


#ifndef __NETDP_ENET_ARP_H__
#define __NETDP_ENET_ARP_H__

#include "netdp_ip_in.h"

#define NETDP_ENET_ARP_MAXTRIES 5

/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */

/*
 * ARP sockaddr structure
 */
struct netdp_enet_sockaddr_inarp {
   uint8_t	sin_len;
   uint8_t	sin_family;
   uint16_t	sin_port;
   struct	netdp_ip_in_addr sin_addr;
   struct	netdp_ip_in_addr sin_srcaddr;
   uint16_t	sin_tos;
   uint16_t	sin_other;
#define NETDP_ENET_SIN_PROXY 1
};

/*
 * ARP header structure
 */
struct  netdp_enet_arphdr {
   uint16_t        ar_hrd;      /* format of hardware address */

#define NETDP_ENET_ARPHRD_ETHER    1       /* ethernet hardware format */

   uint16_t        ar_pro;      /* format of protocol address */
   uint8_t         ar_hln;      /* length of hardware address */
   uint8_t         ar_pln;      /* length of protocol address */
   uint16_t        ar_op;       /* one of: */

#define NETDP_ENET_ARPOP_REQUEST   1       /* request to resolve address */
#define NETDP_ENET_ARPOP_REPLY     2       /* response to previous request */
#define NETDP_ENET_ARPOP_REVREQUEST 3      /* request protocol address given hardware */
#define NETDP_ENET_ARPOP_REVREPLY  4       /* response giving protocol address */
#define NETDP_ENET_ARPOP_INVREQUEST 8      /* request to identify peer */
#define NETDP_ENET_ARPOP_INVREPLY  9       /* response identifying peer */

};

#define netdp_enet_ar_sha(ap)      (((caddr_t)((ap)+1)) +   0)
#define netdp_enet_ar_spa(ap)      (((caddr_t)((ap)+1)) +   (ap)->ar_hln)
#define netdp_enet_ar_tha(ap)      (((caddr_t)((ap)+1)) +   (ap)->ar_hln + (ap)->ar_pln)
#define netdp_enet_ar_tpa(ap)      (((caddr_t)((ap)+1)) + 2*(ap)->ar_hln + (ap)->ar_pln)

#define netdp_enet_arphdr_len2(ar_hln, ar_pln)                                     \
   (sizeof(struct netdp_enet_arphdr) + 2*(ar_hln) + 2*(ar_pln))

#define netdp_enet_arphdr_len(ap)  (netdp_enet_arphdr_len2((ap)->ar_hln, (ap)->ar_pln))

/*
 * IP and ethernet specific routing flags
 */
#define	NETDP_ENET_RTF_USETRAILERS	NETDP_IP_RTF_PROTO1	/* use trailers */
#define NETDP_ENET_RTF_ANNOUNCE		NETDP_IP_RTF_PROTO2	/* announce new arp entry */

/*
 * Function prototypes
 */
void netdp_enet_arp_ifinit(uint16_t ifidx, struct netdp_ip_ifaddr *);
void netdp_enet_in_arpinput(netdp_enet_ifnet_t *ifp, struct rte_mbuf *m);

#endif 
