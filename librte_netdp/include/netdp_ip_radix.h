
/** 
 * @file netdp_ip_radix.h
 *
 * Description: Definitions for radix-tree data structures.
 *
 * File version info: $Id: netdp_ip_radix.h,v 1.22 2006/11/01 01:39:24 fmasood Exp $ $Name: TCPIP_1_3_1_build_37 $
 */

/*
 * Copyright (c) 1988, 1989, 1993
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
 *	@(#)radix.h	8.2 (Berkeley) 10/31/94
 */

#ifndef __NETDP_IP_RADIX_H__
#define	__NETDP_IP_RADIX_H__


#include <stdint.h>
#include <string.h>
#include "rte_malloc.h"


#ifdef MALLOC_DECLARE
MALLOC_DECLARE(M_RTABLE);
#endif

/*
 * Radix search tree node layout.
 */

struct netdp_ip_radix_node {
	struct	netdp_ip_radix_mask *rn_mklist;		/* list of masks contained in subtree */
	struct	netdp_ip_radix_node *rn_parent;		/* parent */
	short	rn_bit;					/* bit offset; -1-index(netmask) */
	char	rn_bmask;				/* node: mask for bit test*/
	u_char	rn_flags;				/* enumerated next */
#define NETDP_IP_RNF_NORMAL	1			/* leaf contains normal route */
#define NETDP_IP_RNF_ROOT		2			/* leaf is root leaf for tree */
#define NETDP_IP_RNF_ACTIVE	4			/* This node is alive (for rtfree) */
	union {
		struct {				/* leaf only data: */
			caddr_t	rn_Key;		/* object of search */
			caddr_t	rn_Mask;	/* netmask, if present */
			struct	netdp_ip_radix_node *rn_Dupedkey;
		} rn_leaf;
		struct {				/* node only data: */
			int	rn_Off;			/* where to start compare */
			struct	netdp_ip_radix_node *rn_L;/* progeny */
			struct	netdp_ip_radix_node *rn_R;/* progeny */
		} rn_node;
	}		rn_u;
#ifdef NETDP_IP_RN_DEBUG
	int rn_info;
	struct netdp_ip_radix_node *rn_twin;
	struct netdp_ip_radix_node *rn_ybro;
#endif
};

#define	netdp_ip_rn_dupedkey	rn_u.rn_leaf.rn_Dupedkey
#define	netdp_ip_rn_key		rn_u.rn_leaf.rn_Key
#define	netdp_ip_rn_mask		rn_u.rn_leaf.rn_Mask
#define	netdp_ip_rn_offset	rn_u.rn_node.rn_Off
#define	netdp_ip_rn_left		rn_u.rn_node.rn_L
#define	netdp_ip_rn_right		rn_u.rn_node.rn_R

/*
 * Annotations to tree concerning potential routes applying to subtrees.
 */

struct netdp_ip_radix_mask {
	short	rm_bit;			/* bit offset; -1-index(netmask) */
	char	rm_unused;		/* cf. rn_bmask */
	u_char	rm_flags;		/* cf. rn_flags */
	struct	netdp_ip_radix_mask *rm_mklist;		/* more masks to try */
	union	{
		caddr_t	rmu_mask;		/* the mask */
		struct	netdp_ip_radix_node *rmu_leaf;	/* for normal routes */
	}	rm_rmu;
	int	rm_refs;		/* # of references to this struct */
};

#define	netdp_ip_rm_mask rm_rmu.rmu_mask
#define	netdp_ip_rm_leaf rm_rmu.rmu_leaf		/* extra field would make 32 bytes */

#if 0
#define NETDP_IP_MKGet(m) {\
	if (rn_mkfreelist) {\
		m = rn_mkfreelist; \
		rn_mkfreelist = (m)->rm_mklist; \
	} else \
		R_Malloc(m, struct radix_mask *, sizeof (*(m))); }
#else
#define NETDP_IP_MKGet(m) {\
	if (rn_mkfreelist) {\
		m = rn_mkfreelist; \
		rn_mkfreelist = (m)->rm_mklist; \
	} else \
		NETDP_IP_FPA_R_Malloc(m, struct netdp_ip_radix_mask *, sizeof (*(m))); }
#endif

#define NETDP_IP_MKFree(m) { (m)->rm_mklist = rn_mkfreelist; rn_mkfreelist = (m);}

typedef int netdp_ip_walktree_f_t(struct netdp_ip_radix_node *, void *);

struct netdp_ip_radix_node_head {
	struct	netdp_ip_radix_node *rnh_treetop;
	int	rnh_addrsize;		/* permit, but not require fixed keys */
	int	rnh_pktsize;		/* permit, but not require fixed keys */
	struct	netdp_ip_radix_node *(*rnh_addaddr)	/* add based on netdp_so_sockaddr */
		(void *v, void *mask,
		     struct netdp_ip_radix_node_head *head, struct netdp_ip_radix_node nodes[]);
	struct	netdp_ip_radix_node *(*rnh_addpkt)	/* add based on packet hdr */
		(void *v, void *mask,
		     struct netdp_ip_radix_node_head *head, struct netdp_ip_radix_node nodes[]);
	struct	netdp_ip_radix_node *(*rnh_deladdr)	/* remove based on netdp_so_sockaddr */
		(void *v, void *mask, struct netdp_ip_radix_node_head *head);
	struct	netdp_ip_radix_node *(*rnh_delpkt)	/* remove based on packet hdr */
		(void *v, void *mask, struct netdp_ip_radix_node_head *head);
	struct	netdp_ip_radix_node *(*rnh_matchaddr)	/* locate based on netdp_so_sockaddr */
		(void *v, struct netdp_ip_radix_node_head *head);
	struct	netdp_ip_radix_node *(*rnh_lookup)	/* locate based on netdp_so_sockaddr */
		(void *v, void *mask, struct netdp_ip_radix_node_head *head);
	struct	netdp_ip_radix_node *(*rnh_matchpkt)	/* locate based on packet hdr */
		(void *v, struct netdp_ip_radix_node_head *head);
	int	(*rnh_walktree)			/* traverse tree */
		(struct netdp_ip_radix_node_head *head, netdp_ip_walktree_f_t *f, void *w);
	int	(*rnh_walktree_from)		/* traverse tree below a */
		(struct netdp_ip_radix_node_head *head, void *a, void *m,
		     netdp_ip_walktree_f_t *f, void *w);
	void	(*rnh_close)	/* do something when the last ref drops */
		(struct netdp_ip_radix_node *rn, struct netdp_ip_radix_node_head *head);
	struct	netdp_ip_radix_node rnh_nodes[3];	/* empty tree for common case */

//add by wuxp for temp 
	rte_spinlock_recursive_t rnh_lock;	/* locks entire radix tree */
};

#define NETDP_IP_Bcmp(a, b, n) bcmp(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define NETDP_IP_Bcopy(a, b, n) bcopy(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define NETDP_IP_Bzero(p, n) bzero((caddr_t)(p), (unsigned)(n));

#if defined(__KERNEL__) && defined(linux)
#define NETDP_IP_R_Malloc(p, t, n) (p = (t)(kmalloc((unsigned long)(n), GFP_ATOMIC)))
#define NETDP_IP_Free(p) kfree((caddr_t)p);
#elif !defined(__KERNEL__) && defined(linux)
#define NETDP_IP_R_Malloc(p, t, n) (p = (t)(malloc((unsigned long)(n))))
#define NETDP_IP_Free(p) free((caddr_t)p);
#else
#define NETDP_IP_R_Malloc(p, t, n) (p = (t)(NETDP_COMMON_MALLOC_TSAFE(netdp_common_arenas, (unsigned long)(n))))
//#define NETDP_IP_R_Zalloc(p, t, n) (p = (t)(NETDP_COMMON_MALLOC_TSAFE(netdp_common_arenas, (unsigned long)(n))))
#define NETDP_IP_Free(p) NETDP_COMMON_FREE_TSAFE((caddr_t)p);
#endif


#define	NETDP_IP_RADIX_NODE_HEAD_LOCK_INIT(rnh)	rte_spinlock_recursive_init (&(rnh)->rnh_lock);	\
				NETDP_IP_PRINT_FN2("rte_spinlock_recursive_init(%p)\n", &(rnh)->rnh_lock)
#define	NETDP_IP_RADIX_NODE_HEAD_LOCK(rnh)    rte_spinlock_recursive_lock(&(rnh)->rnh_lock);	\
				NETDP_IP_PRINT_FN2("rte_spinlock_recursive_lock(%p)\n", &(rnh)->rnh_lock)
#define	NETDP_IP_RADIX_NODE_HEAD_UNLOCK(rnh)  rte_spinlock_recursive_unlock(&(rnh)->rnh_lock);	\
				NETDP_IP_PRINT_FN2("rte_spinlock_recursive_unlock(%p)\n", &(rnh)->rnh_lock)
#define	NETDP_IP_RADIX_NODE_HEAD_DESTROY(rnh)	
#define	NETDP_IP_RADIX_NODE_HEAD_LOCK_ASSERT(rnh) \
   NETDP_COMMON_KASSERT (1, ("RNH header not locked!"));


#define NETDP_IP_FPA_R_Malloc(p, t, n)	p=rte_malloc(NULL,  n, CACHE_LINE_SIZE);						
#define NETDP_IP_FPA_Free(p, n)	              rte_free(p);					
#define NETDP_IP_FPA_R_Zalloc(p, t, n)      p=rte_zmalloc(NULL,  n, CACHE_LINE_SIZE);						

void	 netdp_ip_rn_init(void);
int	 netdp_ip_rn_inithead(void **, int);
int	 netdp_ip_rn_refines(void *, void *);
struct netdp_ip_radix_node
	 *netdp_ip_rn_addmask(void *, int, int),
	 *netdp_ip_rn_addroute (void *, void *, struct netdp_ip_radix_node_head *,
			struct netdp_ip_radix_node [2]),
	 *netdp_ip_rn_delete(void *, void *, struct netdp_ip_radix_node_head *),
	 *netdp_ip_rn_lookup (void *v_arg, void *m_arg,
		        struct netdp_ip_radix_node_head *head),
	 *netdp_ip_rn_match(void *, struct netdp_ip_radix_node_head *);

#ifdef NETDP_IP_ROUTE_DEBUG // {
void netdp_ip_rn_dbg_print(struct netdp_ip_radix_node *node);
#endif // NETDP_IP_ROUTE_DEBUG }

#endif /* __NETDP_IP_RADIX_H__ */
