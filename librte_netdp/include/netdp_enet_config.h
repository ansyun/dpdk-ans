


#ifndef __NETDP_ENET_CONFIG_H__ 
#define __NETDP_ENET_CONFIG_H__

#include "netdp_ip_in_var.h"

#define BUF_SIZE (3 * 1024)
extern  uint64_t etherstatusmask, etherportmask, activeportmask;


void netdp_enet_intf_add(uint8_t port, uint8_t *if_name, struct ether_addr *mac_addr);


void netdp_enet_intf_del(uint8_t port);


/**
 * Displays the interface information (L2 and L3 addresses).  
 * The maximum size of information displayed is bound by the value of BUF_SIZE
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_show(void);



/**
 * Retrieves the interface information.
 * The maximum size of information displayed is bound by the value of BUF_SIZE
 *
 * @param len           input - length of buf; output - length of retrieved information
 * @param buf           pointer to location where the retrieved information is stored
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_get(int *len, caddr_t buf);



/**
 * Retrieves MTU value for an interface.
 *
 * @param name      name of the interface for which the MTU is retrieved
 * @param len       value of retrieved MTU
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_get_mtu(caddr_t name, int *len);



/**
 * Set MTU value for an interface.
 *
 * @param name      name of the interface for which the MTU is set
 * @param len       value of MTU to be set
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_set_mtu(caddr_t name, int *len);



/**
 * Retrieves flag value for an interface.
 *
 * @param name      name of the interface for which the flag value is retrieved
 * @param flags     value of retrieved flags
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_get_flags(caddr_t name, int *flags);



/**
 * Set flag value for an interface.
 *
 * @param name      name of the interface for which the flag value is set
 * @param flags     value of flag to be set
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_set_flags(caddr_t name, int *flags);



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

int netdp_enet_intf_add_ipaddr(caddr_t name, int ip_addr, int netmask);

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
int netdp_enet_intf_del_ipaddr(caddr_t name, int ip_addr, int netmask);


/**
 * Add alias IP address for an interface. The address is added with a mask of 255.255.255.255
 *
 * @param name      name of the interface for which the alias IP address is set
 * @param ip_addr   alias IP address to add
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_add_alias_ipaddr(caddr_t name, int ip_addr);



/**
 * Delete alias IP address for an interface.
 *
 * @param name      name of the interface for which the alias IP address is deleted.
 * @param ip_addr   alias IP address to delete 
 *
 * @return  0 - SUCCESS, non-zero - FAILURE
 *
 */

int netdp_enet_intf_del_alias_ipaddr(caddr_t name, int ip_addr);

#endif
