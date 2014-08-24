

/** 
 * @file netdp_ip_icmp.h
 *
 * Description: Definitions for ICMP protocol.
 *
 * File version info: $Id: netdp_ip_icmp.h,v 1.10 2006/01/05 22:36:31 fmasood Exp $ $Name: TCPIP_1_3_1_build_37 $
 */


#ifndef __NETDP_IP_ICMP_H__
#define __NETDP_IP_ICMP_H__

#define NETDP_IP_MIN(a,b) ((a) < (b) ? (a) : (b))

/*
 * Interface Control Message Protocol Definitions.
 * Per RFC 792, September 1981.
 */

/*
 * Internal of an ICMP Router Advertisement
 */
struct netdp_ip_icmp_ra_addr {
	uint32_t ira_addr;
	uint32_t ira_preference;
};

/*
 * Structure of an icmp header.
 */
struct netdp_ip_icmp {
	uint8_t	 icmp_type;		/* type of message, see below */
	uint8_t	 icmp_code;		/* type sub code */
	uint16_t icmp_cksum;		/* ones complement cksum of struct */
	union {
		uint8_t ih_pptr;			/* ICMP_PARAMPROB */
		struct netdp_ip_in_addr ih_gwaddr;	/* ICMP_REDIRECT */
		struct ih_idseq {
			uint16_t icd_id;
			uint16_t icd_seq;
		} ih_idseq;
		int ih_void;

		/* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
		struct ih_pmtu {
			uint16_t ipm_void;
			uint16_t ipm_nextmtu;
		} ih_pmtu;

		struct ih_rtradv {
			uint8_t irt_num_addrs;
			uint8_t irt_wpa;
			uint16_t irt_lifetime;
		} ih_rtradv;
	} icmp_hun;
#define	netdp_ip_icmp_pptr	icmp_hun.ih_pptr
#define	netdp_ip_icmp_gwaddr	icmp_hun.ih_gwaddr
#define	netdp_ip_icmp_id		icmp_hun.ih_idseq.icd_id
#define	netdp_ip_icmp_seq	icmp_hun.ih_idseq.icd_seq
#define	netdp_ip_icmp_void	icmp_hun.ih_void
#define	netdp_ip_icmp_pmvoid	icmp_hun.ih_pmtu.ipm_void
#define	netdp_ip_icmp_nextmtu	icmp_hun.ih_pmtu.ipm_nextmtu
#define	netdp_ip_icmp_num_addrs	icmp_hun.ih_rtradv.irt_num_addrs
#define	netdp_ip_icmp_wpa	icmp_hun.ih_rtradv.irt_wpa
#define	netdp_ip_icmp_lifetime	icmp_hun.ih_rtradv.irt_lifetime
	union {
		struct id_ts {			/* ICMP Timestamp */
			uint32_t its_otime;	/* Originate */
			uint32_t its_rtime;	/* Receive */
			uint32_t its_ttime;	/* Transmit */
		} id_ts;
		struct id_ip  {
			struct netdp_ip_ip idi_ip;
			/* options and then 64 bits of data */
		} id_ip;
		struct netdp_ip_icmp_ra_addr id_radv;
		uint32_t id_mask;
		char	id_data[1];
	} icmp_dun;
#define	netdp_ip_icmp_otime	icmp_dun.id_ts.its_otime
#define	netdp_ip_icmp_rtime	icmp_dun.id_ts.its_rtime
#define	netdp_ip_icmp_ttime	icmp_dun.id_ts.its_ttime
#define	netdp_ip_icmp_ip		icmp_dun.id_ip.idi_ip
#define	netdp_ip_icmp_radv	icmp_dun.id_radv
#define	netdp_ip_icmp_mask	icmp_dun.id_mask
#define	netdp_ip_icmp_data	icmp_dun.id_data
};

/*
 * Lower bounds on packet lengths for various types.
 * For the error advice packets must first insure that the
 * packet is large enough to contain the returned ip header.
 * Only then can we do the check to see if 64 bits of packet
 * data have been returned, since we need to check the returned
 * ip header length.
 */
#define	NETDP_IP_ICMP_MINLEN	8				/* abs minimum */
#define	NETDP_IP_ICMP_TSLEN	(8 + 3 * sizeof (n_time))	/* timestamp */
#define	NETDP_IP_ICMP_MASKLEN	12				/* address mask */
#define	NETDP_IP_ICMP_ADVLENMIN	(8 + sizeof (struct netdp_ip_ip) + 8)	/* min */
#define	NETDP_IP_ICMP_ADVLEN(p)	(8 + ((p)->icmp_ip.ip_hl << 2) + 8)
	/* N.B.: must separately check that ip_hl >= 5 */

/*
 * Definition of type and code field values.
 */
#define	NETDP_IP_ICMP_ECHOREPLY			0	/* echo reply */
#define	NETDP_IP_ICMP_UNREACH			3	/* dest unreachable, codes: */
#define	NETDP_IP_ICMP_UNREACH_NET			0	/* bad net */
#define	NETDP_IP_ICMP_UNREACH_HOST		1	/* bad host */
#define	NETDP_IP_ICMP_UNREACH_PROTOCOL		2	/* bad protocol */
#define	NETDP_IP_ICMP_UNREACH_PORT		3	/* bad port */
#define	NETDP_IP_ICMP_UNREACH_NEEDFRAG		4	/* IP_DF caused drop */
#define	NETDP_IP_ICMP_UNREACH_SRCFAIL		5	/* src route failed */
#define	NETDP_IP_ICMP_UNREACH_NET_UNKNOWN		6	/* unknown net */
#define	NETDP_IP_ICMP_UNREACH_HOST_UNKNOWN	7	/* unknown host */
#define	NETDP_IP_ICMP_UNREACH_ISOLATED		8	/* src host isolated */
#define	NETDP_IP_ICMP_UNREACH_NET_PROHIB		9	/* prohibited access */
#define	NETDP_IP_ICMP_UNREACH_HOST_PROHIB		10	/* ditto */
#define	NETDP_IP_ICMP_UNREACH_TOSNET		11	/* bad tos for net */
#define	NETDP_IP_ICMP_UNREACH_TOSHOST		12	/* bad tos for host */
#define	NETDP_IP_ICMP_UNREACH_FILTER_PROHIB	13	/* admin prohib */
#define	NETDP_IP_ICMP_UNREACH_HOST_PRECEDENCE	14	/* host prec vio. */
#define	NETDP_IP_ICMP_UNREACH_PRECEDENCE_CUTOFF	15	/* prec cutoff */
#define	NETDP_IP_ICMP_SOURCEQUENCH		4	/* packet lost, slow down */
#define	NETDP_IP_ICMP_REDIRECT			5	/* shorter route, codes: */
#define	NETDP_IP_ICMP_REDIRECT_NET		0	/* for network */
#define	NETDP_IP_ICMP_REDIRECT_HOST		1	/* for host */
#define	NETDP_IP_ICMP_REDIRECT_TOSNET		2	/* for tos and net */
#define	NETDP_IP_ICMP_REDIRECT_TOSHOST		3	/* for tos and host */
#define	NETDP_IP_ICMP_ALTHOSTADDR			6	/* alternate host address */
#define	NETDP_IP_ICMP_ECHO			8	/* echo service */
#define	NETDP_IP_ICMP_ROUTERADVERT		9	/* router advertisement */
#define	NETDP_IP_ICMP_ROUTERADVERT_NORMAL		0	/* normal advertisement */
#define	NETDP_IP_ICMP_ROUTERADVERT_NOROUTE_COMMON	16	/* selective routing */
#define	NETDP_IP_ICMP_ROUTERSOLICIT		10	/* router solicitation */
#define	NETDP_IP_ICMP_TIMXCEED			11	/* time exceeded, code: */
#define	NETDP_IP_ICMP_TIMXCEED_INTRANS		0	/* ttl==0 in transit */
#define	NETDP_IP_ICMP_TIMXCEED_REASS		1	/* ttl==0 in reass */
#define	NETDP_IP_ICMP_PARAMPROB			12	/* ip header bad */
#define	NETDP_IP_ICMP_PARAMPROB_ERRATPTR		0	/* error at param ptr */
#define	NETDP_IP_ICMP_PARAMPROB_OPTABSENT		1	/* req. opt. absent */
#define	NETDP_IP_ICMP_PARAMPROB_LENGTH		2	/* bad length */
#define	NETDP_IP_ICMP_TSTAMP			13	/* timestamp request */
#define	NETDP_IP_ICMP_TSTAMPREPLY			14	/* timestamp reply */
#define	NETDP_IP_ICMP_IREQ			15	/* information request */
#define	NETDP_IP_ICMP_IREQREPLY			16	/* information reply */
#define	NETDP_IP_ICMP_MASKREQ			17	/* address mask request */
#define	NETDP_IP_ICMP_MASKREPLY			18	/* address mask reply */
#define	NETDP_IP_ICMP_TRACEROUTE			30	/* traceroute */
#define	NETDP_IP_ICMP_DATACONVERR			31	/* data conversion error */
#define	NETDP_IP_ICMP_MOBILE_REDIRECT		32	/* mobile host redirect */
#define	NETDP_IP_ICMP_IPV6_WHEREAREYOU		33	/* IPv6 where-are-you */
#define	NETDP_IP_ICMP_IPV6_IAMHERE		34	/* IPv6 i-am-here */
#define	NETDP_IP_ICMP_MOBILE_REGREQUEST		35	/* mobile registration req */
#define	NETDP_IP_ICMP_MOBILE_REGREPLY		36	/* mobile registration reply */
#define	NETDP_IP_ICMP_SKIP			39	/* SKIP */
#define	NETDP_IP_ICMP_PHOTURIS			40	/* Photuris */
#define	NETDP_IP_ICMP_PHOTURIS_UNKNOWN_INDEX	1	/* unknown sec index */
#define	NETDP_IP_ICMP_PHOTURIS_AUTH_FAILED	2	/* auth failed */
#define	NETDP_IP_ICMP_PHOTURIS_DECRYPT_FAILED	3	/* decrypt failed */

#define	NETDP_IP_ICMP_MAXTYPE			40

#define	NETDP_IP_ICMP_INFOTYPE(type) \
	((type) == NETDP_IP_ICMP_ECHOREPLY || (type) == NETDP_IP_ICMP_ECHO || \
	(type) == NETDP_IP_ICMP_ROUTERADVERT || (type) == NETDP_IP_ICMP_ROUTERSOLICIT || \
	(type) == NETDP_IP_ICMP_TSTAMP || (type) == NETDP_IP_ICMP_TSTAMPREPLY || \
	(type) == NETDP_IP_ICMP_IREQ || (type) == NETDP_IP_ICMP_IREQREPLY || \
	(type) == NETDP_IP_ICMP_MASKREQ || (type) == NETDP_IP_ICMP_MASKREPLY)

void icmp_error(struct rte_mbuf *m, int type, int code, uint32_t dest, uint16_t mtu);
void icmp_input(struct rte_mbuf *m);


//add by wuxp for temp
static inline uint16_t
get_16b_sum(uint16_t *ptr16, uint32_t nr)
{
	uint32_t sum = 0;
	while (nr > 1)
	{
		sum +=*ptr16;
		nr -= sizeof(uint16_t);
		ptr16++;
		if (sum > UINT16_MAX)
			sum -= UINT16_MAX;
	}

	/* If length is in odd bytes */
	if (nr)
		sum += *((uint8_t*)ptr16);

	sum = ((sum & 0xffff0000) >> 16) + (sum & 0xffff);
	sum &= 0x0ffff;
	return (uint16_t)sum;
}

#endif /* __NETDP_IP_ICMP_H__ */
