####TCP/IP stack for dpdk
--------------
Netdp is porting from [FreeBSD](http://freebsd.org) TCP/IP stack, and provide a userspace TCP/IP stack for use with the Intel [dpdk](http://dpdk.org/). 

- librte_netdp: TCP/IP stack static library. netdp use dpdk mbuf, ring, memzone, mempool, timer, spinlock. so zero copy mbuf between dpdk and netdp. 

- librte_netdpsock: Netdp socket lib for application, zero copy between netdp and application.

- netdp_cmd: Command for configure netdp tcp/ip stack.
 
- netdp_test: Example application with netdp for testing netdp tcp/ip stack

Support environment
  - EAL is based on dpdk-2.1.0
  - Development enviroment is based on x86_64-native-linuxapp-gcc
  - TCP/IP stack is based on FreeBSD 10.0-RELEASE
  - linux version：
3.16.0-30-generic
  - gcc version：
gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04)

Support feature:
 - Netdp initialize
 - Ether, zero copy between NIC and netdp TCP/IP stack.
 - ARP, ARP timeout
 - IP layer, IP fragmentation and reassemble
 - Routing
 - ICMP
 - Commands for adding, deleting, showing IP address
 - Commands for adding, deleting, showing static route
 - Commands for showing ARP table
 - UDP
 - Socket layer
 - Support epoll
 - No lock in UDP stack and socket layer.
 - Running on multiple cores.
 - Support tcp client(draft version, only run in one lcore)
 - 
Next Planning
- Porting TCP from FreeBSD to netdp
- RSS
- Performance testing.
- IPFW

####[Wiki Page](https://github.com/dpdk-net/netdp/wiki)
-------
You can get more information and instructions from [wiki page](https://github.com/dpdk-net/netdp/wiki).

####Support
-------
BSD LICENSE, you may use netdp freely.

For free support, please use netdp team mail list at zimeiw@163.com. or QQ Group:86883521
