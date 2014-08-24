

#ifndef __NETDP_INIT_H__
#define __NETDP_INIT_H__

#include "netdp_enet.h"


typedef void (*netdp_alloc_mbuf_cb)(uint8_t lcore_id, struct rte_mbuf **m);

extern netdp_alloc_mbuf_cb alloc_mbuf_fn; 

int netdp_init(void);
int netdp_register(netdp_send_packet_cb send_cb, netdp_alloc_mbuf_cb alloc_cb);

#endif /* __NETDP_INIT_H__ */
