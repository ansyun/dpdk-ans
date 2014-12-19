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
 *  The Regents of the University of California.  All rights reserved.
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

#ifndef __NETDP_ENET_INTF_H__ 
#define __NETDP_ENET_INTF_H__


/* if_flags */
#define NETDP_ENET_IFF_UP                     0x01
#define NETDP_ENET_IFF_BROADCAST      0x02
#define NETDP_ENET_IFF_DEBUG              0x04        /* Turn on debugging.  */
#define NETDP_ENET_IFF_LOOPBACK         0x08        /* Is a loopback net.  */
#define NETDP_ENET_IFF_POINTOPOINT   0x10
#define NETDP_ENET_IFF_RUNNING           0x40        /* resources allocated */
#define NETDP_ENET_IFF_NOARP              0x80         /* no address resolution protocol */
#define NETDP_ENET_IFF_PPROMISC         0x100       /* Receive all packets.  */
#define NETDP_ENET_IFF_STATICARP       0x80000   /* static ARP */

/**
 * Add an interface.
 *
 * @param port      port of the interface 
 * @param if_name      name of the interface 
 * @param mac_addr   mac address of the interface
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */
int netdp_intf_add(uint8_t port, uint8_t *if_name, struct ether_addr *mac_addr);

/**
 * Delete an interface.
 *
 * @param port      port of the interface 
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */
int netdp_intf_del(uint8_t port);

/**
 * Retrieves MTU value for an interface.
 *
 * @param name      name of the interface for which the MTU is retrieved
 * @param len       value of retrieved MTU
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_intf_get_mtu(caddr_t name, int *mtu);



/**
 * Set MTU value for an interface.
 *
 * @param name      name of the interface for which the MTU is set
 * @param len       value of MTU to be set
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_intf_set_mtu(caddr_t name, int *mtu);



/**
 * Retrieves flag value for an interface.
 *
 * @param name      name of the interface for which the flag value is retrieved
 * @param flags     value of retrieved flags
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_intf_get_flags(caddr_t name, int *flags);



/**
 * Set flag value for an interface.
 *
 * @param name      name of the interface for which the flag value is set
 * @param flags     value of flag to be set
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_intf_set_flags(caddr_t name, int *flags);



/**
 * Add IP address for an interface.
 *
 * @param name      name of the interface for which the IP address is set
 * @param ip_addr   IP address to add
 * @param netmask   mask for the IP address
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_intf_add_ipaddr(caddr_t name, int ip_addr, int netmask);

/**
 * Delete IP address for an interface.
 *
 * @param name      name of the interface for which the IP address is set
 * @param ip_addr   IP address to add
 * @param netmask   mask for the IP address
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */
int netdp_intf_del_ipaddr(caddr_t name, int ip_addr, int netmask);

/**
 * Displays the interface information (L2 and L3 addresses).  
 * The maximum size of information displayed is bound by the value of BUF_SIZE
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_intf_show(void);


#endif
