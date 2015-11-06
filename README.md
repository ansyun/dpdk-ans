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
 - UDP protocol
 - Socket layer, share memory.
 - Socket API, socket/close/send/recv/epoll.
 - TCP protocol, no lock, hash table, per tcp stack per lcore.(currently only run in one lcore, will enhance it)
 - 
Next Planning
- Enhance TCP Flow control and Congestion control
- TCP RSS
- Performance testing.
- IPFW


####Performance Testing
--------------
- TCP server performance testing
 
 ENV: CPU- intel xeon 2.3G, NIC- 10G, Test tool:ab 

 Procedure: ab establish tcp connection to netdp tcp server, ab download one data, netdp tcp server close socket.

 Command: ab -n 1000000 -c 500  2.2.2.2:8089/
```
    |--------------------------------------| 
    |      TCP Server accept performance   |
    |--------------------------------------| 
    | Linux with epoll | NETDP with epoll  | 
    |    (Multi core)  |    (one core)     |
    |--------------------------------------|
    | 53k connection/s | 43k connection/s  | 
    |--------------------------------------| 
```

- L3 forwarding performance testing

  ENV: CPU- intel xeon 2.3G, NIC- 10G, one lcore rx packets-->l3 forwarding --> tx packets,  Test tool:pktgen-DPDK
```
    |--------------------------------------| 
    |      L3 forwarding performance       |
    |             (one lcore)              |
    |--------------------------------------| 
    | Packet size(byte)| Throughput(Mpps)  | 
    |--------------------------------------|
    |     64           |      3.682        | 
    |--------------------------------------| 
    |     128          |      3.682        | 
    |--------------------------------------| 
    |     256          |      3.683        | 
    |--------------------------------------| 
    |     512          |      2.35         |
    |--------------------------------------| 
    |     1024         |      1.197        | 
    |--------------------------------------| 
    |     1500         |      0.822        | 
    |--------------------------------------| 
```

####Examples
-------
- opendp_tcp_server, tcp server run on opendp tcp/ip stack.
- [dpdk-nginx](https://github.com/opendp/dpdk-nginx), nginx was porting to run on opendp tcp/ip stack.

####[Wiki Page](https://github.com/dpdk-net/netdp/wiki)
-------
You can get more information and instructions from [wiki page](https://github.com/dpdk-net/netdp/wiki).

####Notes
-------
- Netdp socket application run as a secondary dpdk process, If you got below log, shall execute below commands to disable ASLR.
```
EAL: WARNING: Address Space Layout Randomization (ASLR) is enabled in the kernel.
EAL: This may cause issues with mapping memory into secondary processes
$ sudo sysctl -w kernel.randomize_va_space=0
```
####Support
-------
BSD LICENSE, you may use netdp freely.

For free support, please use netdp team mail list at zimeiw@163.com. or QQ Group:86883521
