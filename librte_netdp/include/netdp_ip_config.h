

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
