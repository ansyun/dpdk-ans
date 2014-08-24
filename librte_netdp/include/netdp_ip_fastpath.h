
/** 
 * @file netdp_ip_fastpath.h
 *
 * Description: IP fast path defitions.
 *
 * File version info: $Id: netdp_ip_fastpath.h,v 1.12 2006/11/09 20:38:24 harri Exp $ $Name: TCPIP_1_3_1_build_37 $
 */

#ifndef __NETDP_IP_FASTPATH_H__ // {
#define __NETDP_IP_FASTPATH_H__

#define NETDP_IP_CACHE_ENTRIES_PER_BUCKET	5
#define NETDP_IP_CACHE_TIME_SHIFT		27	// Gives a resolution of 200ms or so
#define NETDP_IP_CACHE_TIME_BITMASK	0x7f
#define NETDP_IP_CACHE_READ_RETRY_COUNT	10

typedef union {
   uint64_t u64;
   struct {
      uint32_t addr;
      uint16_t ifidx;
      uint16_t in_use : 1;
      uint16_t unused : 12;
      uint16_t id     : 3;
   } s;
} netdp_ip_cache_bucket_w0_t;

typedef union {
   uint64_t u64;
   struct {
      uint8_t edst[6];
      uint8_t unused;
      uint8_t zeroes : 5;
      uint8_t id     : 3;
   } s;
} netdp_ip_cache_bucket_w1_t;

typedef union {
   uint64_t u64;
   struct {
      uint64_t ifa_ptr   : 40;
      uint64_t pending   : 1;
      uint64_t timestamp : 7;
      uint64_t zeroes    : 13;
      uint64_t id        : 3;
   } s;
} netdp_ip_cache_bucket_w2_t;

typedef struct {
   netdp_ip_cache_bucket_w0_t word0;
   netdp_ip_cache_bucket_w1_t word1;
   netdp_ip_cache_bucket_w2_t word2;
} netdp_ip_cache_bucket_entry_t;

typedef struct {
   rte_spinlock_t lock;
   uint32_t idx;
   netdp_ip_cache_bucket_entry_t entry[NETDP_IP_CACHE_ENTRIES_PER_BUCKET];
} netdp_ip_cache_bucket_t;

extern  netdp_ip_cache_bucket_t *netdp_ip_cache;

#endif // __NETDP_IP_FASTPATH_H__ 

