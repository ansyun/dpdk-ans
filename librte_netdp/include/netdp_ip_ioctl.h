
/** 
 * @file netdp_ip_ioctl.h
 *
 * Description: 
 *
 * File version info: $Id: netdp_ip_ioctl.h,v 1.3 2006/01/21 00:54:58 harri Exp $ $Name: TCPIP_1_3_1_build_37 $
 */

// Used for custom ioctl
#define NETDP_IP_MAC_DST_SET 0
#define NETDP_IP_MAC_DST_GET 1

#define NETDP_IP_IPFWCONTROL_IOCTL_CODE   0
#define NETDP_IP_CUSTOM_IOCTL_CODE        1
#define NETDP_IP_IPFWCONTROL_IOCTL        _IOWR(0x82, NETDP_IP_IPFWCONTROL_IOCTL_CODE, void*)
#define NETDP_IP_CUSTOM_IOCTL             _IOWR(0x82, NETDP_IP_CUSTOM_IOCTL_CODE, void*)
