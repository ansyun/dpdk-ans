

#ifndef __NETDP_IP_INLINE_H__
#define __NETDP_IP_INLINE_H__

#include "netdp_queue.h"

#include "socket.h"
#include "netdp_ip_if_var.h"
#include "netdp_ip_in_var.h"
#include "netdp_ip_in.h"
#include "netdp_ip_fastpath.h"
#include "netdp_ip_var.h"

#include "netdp_errno.h"

/// @cond TEST
extern  netdp_ip_ipstat_t netdp_ip_ipstat;
/// @endcond

/**
 * Looks up a local IP address 
 *
 * @param ipaddr IP destination address
 * @return netdp_ip_in_ifaddr structure
 */
static inline struct netdp_ip_in_ifaddr* netdp_ip_lookup(uint32_t ipaddr)
{
   struct netdp_ip_in_ifaddr *ia;
   NETDP_COMMON_LIST_FOREACH(ia, NETDP_IP_INADDR_HASH(ipaddr), ia_hash) 
   {
      if (NETDP_IP_IA_SIN(ia)->sin_addr.s_addr == ipaddr)
	 break; 
   }
   return ia;
}


/// @endcond

static inline void netdp_ip_cache_entry_free(uint32_t ip_dst)
{
	return;
}

/**
 * Function to calculate IPv4 header checksum.
 * 
 * @param *ip pointer to the beginning of IP header
 * 
 * @return 16-bit one's complement IPv4 checksum.
 * No alignment requirements.
 */

static inline uint16_t netdp_ip_calculate_ip_header_checksum(struct netdp_ip_ip *ip)
{
   uint64_t sum;
   uint16_t *ptr = (uint16_t*) ip;
   uint8_t *bptr = (uint8_t*) ip;

   sum  = ptr[0];		
   sum += ptr[1];
   sum += ptr[2];
   sum += ptr[3];
   sum += ptr[4];
   // Skip checksum field
   sum += ptr[6];
   sum += ptr[7];
   sum += ptr[8];
   sum += ptr[9];

   // Check for options
   if ((bptr[0] != 0x45)) goto slow_cksum_calc;

return_from_slow_cksum_calc:

   sum = (uint16_t) sum + (sum >> 16);
   sum = (uint16_t) sum + (sum >> 16);
   return ((uint16_t) (sum ^ 0xffff));

slow_cksum_calc:
   // Addes IPv4 options into the checksum (if present)
   {
      uint64_t len = (bptr[0] & 0xf) - 5;
      ptr = &ptr[len<<1];

      while (len-- > 0) {
	 sum += *ptr++;
	 sum += *ptr++;
      }
   }

   goto return_from_slow_cksum_calc;
}





static inline void netdp_ip_dump_stats(void)
{
   printf("Total packets received         = %ld\n", netdp_ip_ipstat.ips_total);
   printf("Bad IP checksum                = %ld\n", netdp_ip_ipstat.ips_badsum);
   printf("Too short packets              = %ld\n", netdp_ip_ipstat.ips_tooshort);
   printf("Too small packets              = %ld\n", netdp_ip_ipstat.ips_toosmall);
   printf("Bad IP header length packets   = %ld\n", netdp_ip_ipstat.ips_badhlen);
   printf("Bad IP length packet           = %ld\n", netdp_ip_ipstat.ips_badlen);
   printf("IP fragments received          = %ld\n", netdp_ip_ipstat.ips_fragments);
   printf("IP fragments dropped           = %ld\n", netdp_ip_ipstat.ips_fragdropped);
   printf("IP fragments timeedout         = %ld\n", netdp_ip_ipstat.ips_fragtimeout);
   printf("IP packets forwarded           = %ld\n", netdp_ip_ipstat.ips_forward);
   printf("IP packets fastforwarded       = %ld\n", netdp_ip_ipstat.ips_fastforward);
   printf("IP packets cant be forwarded   = %ld\n", netdp_ip_ipstat.ips_cantforward);
   printf("ICMP redirect sent             = %ld\n", netdp_ip_ipstat.ips_redirectsent);
   printf("IP unknown proto rcvd          = %ld\n", netdp_ip_ipstat.ips_noproto);
   printf("IP pkts delivered to app       = %ld\n", netdp_ip_ipstat.ips_delivered);
   printf("Total IP pkts generated        = %ld\n", netdp_ip_ipstat.ips_localout);
   printf("Packets dropped due to nobuf   = %ld\n", netdp_ip_ipstat.ips_odropped);
   printf("Total IP packets reassembled   = %ld\n", netdp_ip_ipstat.ips_reassembled);
   printf("Totoal IP packets fragmented   = %ld\n", netdp_ip_ipstat.ips_fragmented);
   printf("Total IP fragments created     = %ld\n", netdp_ip_ipstat.ips_ofragments);
   printf("Total packets cant be frag     = %ld\n", netdp_ip_ipstat.ips_cantfrag);
   printf("IP packets with option error   = %ld\n", netdp_ip_ipstat.ips_badoptions);
   printf("Pkts dropped due to noroute    = %ld\n", netdp_ip_ipstat.ips_noroute);
   printf("Rcvd pkts with bad version     = %ld\n", netdp_ip_ipstat.ips_badvers);
   printf("Total raw IP pkt output        = %ld\n", netdp_ip_ipstat.ips_rawout);
   printf("Too long packets               = %ld\n", netdp_ip_ipstat.ips_toolong);
   printf("Non member packet count        = %ld\n", netdp_ip_ipstat.ips_notmember);
   printf("No GIF match                   = %ld\n", netdp_ip_ipstat.ips_nogif);
   printf("Packets with bad IP address    = %ld\n", netdp_ip_ipstat.ips_badaddr);
   printf("\n\n\n\n\n\n\n\n");
}
/// @endcond


#endif /* __NETDP_IP_INLINE_H__ */
