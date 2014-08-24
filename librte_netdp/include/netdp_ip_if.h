
/** 
 * @file netdp_ip_if.h
 *
 * Description: Interface request structure definition.
 *
 * File version info: $Id: netdp_ip_if.h,v 1.15 2007/01/05 23:29:38 ssamanta Exp $ $Name: TCPIP_1_3_1_build_37 $
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
 *	@(#)icmp_var.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: src/sys/netinet/icmp_var.h,v 1.24 2004/08/16 18:32:07 rwatson Exp $
 */

#ifndef __NETDP_IP_IF_H__
#define __NETDP_IP_IF_H__

/*
 * Length of interface external name, including terminating '\0'.
 * Note: this is the same size as a generic device's external name.
 */
#define	NETDP_IP_IF_NAMESIZE	16
#define	NETDP_IP_IFNAMSIZ		NETDP_IP_IF_NAMESIZE

/*
 * Interface request structure used for socket
 * ioctl's.  All interface ioctl's must have parameter
 * definitions which begin with ifr_name.  The
 * remainder may be interface specific.
 */
struct	netdp_ip_ifreq {
	char	netdp_ifr_name[NETDP_IP_IFNAMSIZ];		/* if name, e.g. "en0" */
	union {
		struct	netdp_so_sockaddr ifru_addr;
		struct	netdp_so_sockaddr ifru_dstaddr;
		struct	netdp_so_sockaddr ifru_broadaddr;
		short	ifru_flags[2];
		short	ifru_index;
		int	ifru_metric;
		int	ifru_mtu;
		int	ifru_phys;
		int	ifru_media;
		caddr_t	ifru_data;
		int	ifru_cap[2];
	} ifr_ifru;

#define	netdp_ip_ifr_addr		ifr_ifru.ifru_addr	/* address */
#define	netdp_ip_ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	netdp_ip_ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define	netdp_ip_ifr_flags	ifr_ifru.ifru_flags[0]	/* flags (low 16 bits) */
#define	netdp_ip_ifr_flagshigh	ifr_ifru.ifru_flags[1]	/* flags (high 16 bits) */
#define	netdp_ip_ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define	netdp_ip_ifr_mtu		ifr_ifru.ifru_mtu	/* mtu */
#define netdp_ip_ifr_phys		ifr_ifru.ifru_phys	/* physical wire */
#define netdp_ip_ifr_media	ifr_ifru.ifru_media	/* physical media */
#define	netdp_ip_ifr_data		ifr_ifru.ifru_data	/* for use by interface */
#define	netdp_ip_ifr_reqcap	ifr_ifru.ifru_cap[0]	/* requested capabilities */
#define	netdp_ip_ifr_curcap	ifr_ifru.ifru_cap[1]	/* current capabilities */
#define	netdp_ip_ifr_index	ifr_ifru.ifru_index	/* interface index */
};


/*
 * Structure used in SIOCGIFCONF request.
 * Used to retrieve interface configuration
 */
struct  netdp_ip_ifconf {
        int     ifc_len;                /* size of associated buffer */
        union {
                caddr_t ifcu_buf;
                struct  netdp_ip_ifreq *ifcu_req;
        } ifc_ifcu;
#define ifc_buf ifc_ifcu.ifcu_buf       /* buffer address */
#define ifc_req ifc_ifcu.ifcu_req       /* array of structures returned */
};




/*
 * Structure for SIOC[AGD]LIFADDR
 */
struct netdp_ip_if_laddrreq {
   char    iflr_name[NETDP_IP_IFNAMSIZ];
   u_int   flags;
#define NETDP_IP_IFLR_PREFIX     0x8000  /* in: prefix given  out: kernel fills id */
   u_int   prefixlen;         /* in/out */
   struct  netdp_so_sockaddr addr;   /* in/out */
   struct  netdp_so_sockaddr dstaddr; /* out */
};

#define NETDP_IP_IFQ_MAXLEN      50

#define NETDP_IP_IF_AFDATA_LOCK_INIT(ifp)  rte_spinlock_init(&(ifp)->if_afdata_mtx)
#define NETDP_IP_IF_AFDATA_LOCK(ifp)           rte_spinlock_lock(&(ifp)->if_afdata_mtx)
#define NETDP_IP_IF_AFDATA_TRYLOCK(ifp)     rte_spinlock_trylock(&(ifp)->if_afdata_mtx)
#define NETDP_IP_IF_AFDATA_UNLOCK(ifp)       rte_spinlock_unlock(&(ifp)->if_afdata_mtx)
#define NETDP_IP_IF_AFDATA_DESTROY(ifp)  

#endif /* __NETDP_IP_IF_H__ */
