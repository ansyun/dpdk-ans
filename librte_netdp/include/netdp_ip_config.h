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


#ifndef __NETDP_IP_CONFIG_H__ 
#define __NETDP_IP_CONFIG_H__

struct walkarg {
        int     w_op;
	int     w_arg;
	int     w_given;
	int     w_needed;
        int     w_tmemsize;
        caddr_t w_where;
        caddr_t w_tmem;
};


/**
 * Default route creation routine.
 * Creates a default route (ip addr: 0.0.0.0  netmask: 0.0.0.0)
 * pointing to a gateway with address 'gateway_addr'
 *
 * @param gateway_addr  IP address of gateway
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 *
 */

int netdp_ip_add_default_route(uint32_t gateway_addr);



/**
 * Default route deletion.
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 *
 */

int netdp_ip_del_default_route();



/**
 * Routing table addition.
 *
 * @param dest_ipaddr   ip address of the destination for which the route is being added
 * @param gateway_valid indicates whether the 'gateway_addr' is valid
 * @param gateway_addr  ip address of gateway
 * @param netmask       netmask to be used with route
 * @param flags         flags for this routing entry
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_ip_add_route(uint32_t dest_ipaddr, uint8_t gateway_valid, uint32_t gateway_addr,
                     uint32_t netmask, int flags);



/**
 * Routing table deletion.
 *
 * @param dest_ipaddr   ip address of the destination for which the route is being deleted
 * @param netmask       netmask to be used with route
 * @param flags         flags for this routing entry (same value as used during route addition)
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_ip_del_route(uint32_t dest_ipaddr, uint32_t netmask, int flags);



/**
 * Retrieves the routing table information.
 *
 * @param error         returns 0 - SUCCESS, non-zero - FAILURE
 * @param length        length of the routing information 
 *
 * @return  char pointer to location containing the routing information
 *
 */

caddr_t netdp_ip_route_get_all(int *error, int *length);



/**
 * Displays the routing table information.
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_ip_route_show_all(void);



/**
 * Displays the routing table information for a given network.
 *
 * @param net_ipaddr    ip address of the network for which the routes are displayed
 * @param netmask       netmask to be used
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_ip_route_show(uint32_t net_ipaddr, uint32_t netmask);

#endif 
