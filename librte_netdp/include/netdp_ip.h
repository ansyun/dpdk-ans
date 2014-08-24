

#ifndef __NETDP_IP_H__
#define __NETDP_IP_H__

#include "netdp_ip_route.h"


/* IP Header file */
#define NETDP_IP_IPPROTO_IP	0
#define NETDP_IP_IPPROTO_ICMP	1
#define NETDP_IP_IPPROTO_TCP	6
#define NETDP_IP_IPPROTO_UDP	17
#define NETDP_IP_IPPROTO_RAW	255


/*
 * Internet implementation parameters.
 */
#define	NETDP_IP_MAXTTL		255		/* maximum time to live (seconds) */
#define	NETDP_IP_IPDEFTTL   	64		/* default ttl, from RFC 1340 */
#define	NETDP_IP_IPFRAGTTL	60		/* time to live for frags, slowhz */
#define	NETDP_IP_IPTTLDEC	       1		/* subtracted when forwarding */
#define	NETDP_IP_IP_MSS		576		/* default maximum segment size */


/*
 * 72 was chosen below because it is the size of a TCP/IP
 * header (40) + the minimum mss (32).
 */
#define NETDP_IP_IF_MINMTU       72
#define NETDP_IP_IF_MAXMTU       65535



struct netdp_ip_rtentry;
struct netdp_ip_rt_addrinfo;

/*
 * Definitions for internet protocol version 4.
 * Per RFC 791, September 1981.
 */
#define	NETDP_IP_IPVERSION	4

/**
 * Structure of an internet header, naked of options.
 */
struct netdp_ip_ip {
	uint32_t ip_hl:4;		/**< header length */
	uint32_t ip_v:4;		/**< version */
	uint32_t ip_tos:8;		/**< type of service */
	uint32_t ip_len:16;		/**< total length */
	uint32_t ip_id:16;		/**< identification */
	uint32_t ip_off:16;		/**< fragment offset field */
#define	NETDP_IP_IP_RF 0x8000		/**< reserved fragment flag */
#define	NETDP_IP_IP_DF 0x4000		/**< dont fragment flag */
#define	NETDP_IP_IP_MF 0x2000		/**< more fragments flag */
#define	NETDP_IP_IP_OFFMASK 0x1fff	/**< mask for fragmenting bits */
	uint32_t ip_ttl:8;		/**< time to live */
	uint32_t ip_p:8;		/**< protocol */
	uint32_t ip_sum:16;		/**< checksum */
	struct netdp_ip_in_addr ip_src;	/**< source address */
	struct netdp_ip_in_addr ip_dst;	/**< dest address */
};

typedef struct netdp_ip_ip netdp_ip_ip_t;

/*
 * Definitions for options.
 */
#define NETDP_IP_IPOPT_COPIED(o)         ((o)&0x80)
#define NETDP_IP_IPOPT_CLASS(o)          ((o)&0x60)
#define NETDP_IP_IPOPT_NUMBER(o)         ((o)&0x1f) 

#define NETDP_IP_IPOPT_CONTROL           0x00
#define NETDP_IP_IPOPT_RESERVED1         0x20
#define NETDP_IP_IPOPT_DEBMEAS           0x40
#define NETDP_IP_IPOPT_RESERVED2         0x60

#define NETDP_IP_IPOPT_EOL               0               /* end of option list */
#define NETDP_IP_IPOPT_NOP               1               /* no operation */

#define NETDP_IP_IPOPT_RR                7               /* record packet route */
#define NETDP_IP_IPOPT_TS                68              /* timestamp */
#define NETDP_IP_IPOPT_SECURITY          130             /* provide s,c,h,tcc */
#define NETDP_IP_IPOPT_LSRR              131             /* loose source route */
#define NETDP_IP_IPOPT_SATID             136             /* satnet id */
#define NETDP_IP_IPOPT_SSRR              137             /* strict source route */

/*
 * Offsets to fields in options other than EOL and NOP.
 */
#define NETDP_IP_IPOPT_OPTVAL            0               /* option ID */
#define NETDP_IP_IPOPT_OLEN              1               /* option length */
#define NETDP_IP_IPOPT_OFFSET            2               /* offset within option */
#define NETDP_IP_IPOPT_MINOFF            4               /* min value of above */

/** Definition for the return code to application from IP stack
 *
 */

typedef enum netdp_ip_ret_val{
        NETDP_IP_PKT_RCVD,
        NETDP_IP_FRAG_RCVD,
        NETDP_IP_REASS_PKT_RCVD,
        NETDP_IP_PKT_BAD,
} netdp_ip_ret_val_t;

/*
 * Definitions for IP type of service (ip_tos)
 */
#define NETDP_IP_IPTOS_LOWDELAY          0x10
#define NETDP_IP_IPTOS_THROUGHPUT        0x08
#define NETDP_IP_IPTOS_RELIABILITY       0x04
#define NETDP_IP_IPTOS_MINCOST           0x02
#if 1
/* ECN RFC3168 obsoletes RFC2481, and these will be deprecated soon. */
#define NETDP_IP_IPTOS_CE                0x01
#define NETDP_IP_IPTOS_ECT               0x02
#endif


extern  NETDP_COMMON_LIST_HEAD(netdp_ip_in_ifaddrhashhead, netdp_ip_in_ifaddr) *netdp_ip_in_ifaddrhashtbl;

#define NETDP_IP_INADDR_NHASH_LOG2 8
#define NETDP_IP_INADDR_NHASH (1<<8)
#define NETDP_IP_INADDR_HASHVAL(x) \
    ((x&0xff)^((x>>8)&0xff)^((x>>16)&0xff)^((x>>24)&0xff))
#define NETDP_IP_INADDR_HASH(x) \
    (&netdp_ip_in_ifaddrhashtbl[NETDP_IP_INADDR_HASHVAL(x)])

// netdp_ip_sockopt structure is a replica of sockopt.
// It will be used until the generic socket layer is
// available.
enum netdp_ip_sopt_dir { NETDP_IP_SOPT_GET, NETDP_IP_SOPT_SET };
struct netdp_ip_sockopt {
   enum    netdp_ip_sopt_dir sopt_dir; /* is this a get or a set? */
   int     sopt_level;     /* second arg of [gs]etsockopt */
   int     sopt_name;      /* third arg of [gs]etsockopt */
   void    *sopt_val;      /* fourth arg of [gs]etsockopt */
   size_t  sopt_valsize;   /* (almost) fifth arg of [gs]etsockopt */
   struct  thread *sopt_td; /* calling thread or null if kernel */
};

struct netdp_ip_ip_moptions;

/**
 * Function prototypes
 */ 

void netdp_ip_init(void);
void netdp_ip_input(netdp_enet_ifnet_t *ifp, struct rte_mbuf *m);
int netdp_ip_output(struct rte_mbuf *m, void *opt, struct netdp_ip_route *ro,
                     int flags, struct netdp_ip_ip_moptions *imo);
void netdp_ip_init_ip_cache(void);
int netdp_ip_forward_fragment(struct rte_mbuf *swp, uint16_t mtu);
int netdp_ip_ip_fragment(struct rte_mbuf *swp, uint16_t ifidx, uint16_t mtu, struct netdp_so_sockaddr *dst,
                       struct netdp_ip_route *ro, netdp_ip_cache_bucket_entry_t *entry, uint64_t fast_path);
void icmp_input(struct rte_mbuf *m);
struct netdp_ip_in_ifaddr *netdp_ip_ip_rtaddr(struct netdp_ip_in_addr dst);


#endif /* __NETDP_IP_H__ */
