####TCP/IP stack for dpdk
--------------
Netdp is porting from [FreeBSD](http://freebsd.org) TCP/IP stack, and provide a userspace TCP/IP stack for use with the Intel [dpdk](http://dpdk.org/). 

- librte_netdp: TCP/IP stack static library. netdp use dpdk mbuf, ring, memzone, mempool, timer, spinlock. so zero copy mbuf between dpdk and netdp. 
 
- netdp_cmd: Command for configure netdp tcp/ip stack.
 
- netdp_test: Example application with netdp for testing netdp tcp/ip stack

Support environment
  - EAL is based on dpdk-1.7.1
  - Development enviroment is based on x86_64-native-linuxapp-gcc
  - TCP/IP stack is based on FreeBSD 10.0-RELEASE

Support feature:
 - Netdp initialize
 - Ether
 - ARP, ARP timeout
 - IP layer, IP fragmentation and reassemble
 - Routing
 - ICMP
 - Commands for adding, deleting, showing IP address
 - Commands for adding, deleting, showing static route
 - Commands for showing ARP table
 - UDP
 - UDP socket
 - Netdp socket lib for application
 - 
Next Planning
- Porting TCP from FreeBSD to netdp
- Porting socket from FreeBSD to netdp
- Open data plane.

####[Wiki Page](https://github.com/dpdk-net/netdp/wiki)
-------
You can get more information and instructions from wiki page.

####Support
-------
BSD LICENSE, you may use netdp freely.

For free support, please use netdp team mail list at zimeiw@163.com. or QQ group:86883521
