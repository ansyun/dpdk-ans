####TCP/IP stack for dpdk
--------------
Netdp is porting from [FreeBSD](http://freebsd.org) TCP/IP stack, and provide a userspace TCP/IP stack for use with the Intel [dpdk](http://dpdk.org/). 

- librte_netdp: TCP/IP stack static library. netdp use dpdk mbuf, ring, memzone, mempool, timer, spinlock. so zero copy mbuf between dpdk and netdp. 

- librte_netdpsock: Netdp socket lib for application, zero copy between netdp and application.

- netdp_cmd: Command for configure netdp tcp/ip stack.
 
- netdp_test: Example application with netdp for testing netdp tcp/ip stack

Support environment
  - EAL is based on dpdk-2.2.0
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
 - TCP protocol
    - Free lock, hash table.
    - Support SO_REUSEPORT, multi application can listen the same port.
    - Support multicore tcp stack, per tcp stack per lcore.
 - 
Next Planning
- Enhance socket API
- Performance testing.

####TCP ARCH
--------------
```
         |-------|       |-------|       |-------|
         |  APP  |       |  APP  |       |  APP  |
         |       |       |       |       |       |
         |       |       |       |       |       |
         |-------|       |-------|       |-------|
             |               |               |
--------------------------------------------------
netdpsock    |               |               |			
             fd              fd              fd
             |               |               |
--------------------------------------------------
netdp        |               |               |
         |-------|       |-------|       |-------|
         | TCP   |       |  TCP  |       | TCP   |
         |       |       |       |       |       |
         |       |       |       |       |       |
         |       |       |       |       |       |
         |---------------------------------------|       
         |               IP/ARP/ICMP             |
         |---------------------------------------|       
         |       |       |       |       |       |
         |LCORE0 |       |LCORE1 |       |LCORE2 |
         |-------|       |-------|       |-------|
             |               |               |
             ---------------RSS---------------
                             | 
         |---------------------------------------| 
         |                  NIC                  | 
         |---------------------------------------| 
```
 - NIC distribute packets to different lcore based on RSS, so same TCP flow are handled in the same lcore.
 - Each lcore has own TCP stack. so no share data between lcores, free lock.
 - IP/ARP/ICMP are shared between lcores.
 - APP process runs as a tcp server, only listens on one lcore and accept tcp connections from the lcore, so the APP process number shall large than the lcore number. The APP processes are deployed on each lcore automaticly and averagely.
 - APP process runs as a tcp client, app process can communicate with each lcore. The tcp connection can be located in specified lcore automaticly.
 - APP process can bind the same port if enable reuseport, APP process could accept tcp connection by round robin.
 - If NIC don't support multi queue or RSS, shall enhance opendp_main.c, reserve one lcore to receive and send packets from NIC, and distribute packets to lcores of netdp tcp stack by software RSS.

####Performance Testing
--------------
- TCP server performance testing
 
 ENV: CPU- intel xeon 2.3G, NIC- 10G, Test tool:ab 

 Procedure: ab establish tcp connection to netdp tcp server, ab download one data, netdp tcp server close socket.

 Command: ab -n 1000000 -c 500  2.2.2.2:8089/
 
 Notes: shall increase test linux PC local port range (net.ipv4.ip_local_port_range = 1024 65000).
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
- TCP socket data transmission performance
 
One socket receive 190Mbyte tcp payload, one socket send 130Mbyte tcp payload
```
Communication(synchronization)  0 runtime:	 0.734931 s
Communication(synchronization)  1 runtime:	 0.469566 s
Communication(synchronization)  2 runtime:	 0.449729 s
Communication(synchronization)  3 runtime:	 0.648432 s
Communication(synchronization)  4 runtime:	 0.449422 s
Communication(synchronization)  5 runtime:	 0.647259 s
Communication(synchronization)  6 runtime:	 0.457027 s
Communication(synchronization)  7 runtime:	 0.457691 s
Communication(synchronization)  8 runtime:	 0.67568 s
Communication(synchronization)  9 runtime:	 0.736285 s
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
- dpdk-redis performance testing
```
====ENV=== 
CPU:Intel(R) Xeon(R) CPU E5-2430 0 @ 2.20GHz.
NIC:Intel Corporation 82576 Gigabit Network Connection (rev 01) 
OPENDP run on a lcore.

root@h163:~/dpdk-redis# ./src/redis-benchmark -h 2.2.2.2  -p 6379 -n 100000 -c 50 -q
PING_INLINE: 86655.11 requests per second
PING_BULK: 90497.73 requests per second
SET: 84317.03 requests per second
GET: 85106.38 requests per second
INCR: 86580.09 requests per second
LPUSH: 83263.95 requests per second
LPOP: 83612.04 requests per second
SADD: 85034.02 requests per second
SPOP: 86430.43 requests per second
LPUSH (needed to benchmark LRANGE): 84245.99 requests per second
LRANGE_100 (first 100 elements): 46948.36 requests per second
LRANGE_300 (first 300 elements): 19615.54 requests per second
LRANGE_500 (first 450 elements): 11584.80 requests per second
LRANGE_600 (first 600 elements): 10324.18 requests per second
MSET (10 keys): 66401.06 requests per second

```
- http server connection performance
```
CPU:Intel(R) Xeon(R) CPU E5-2430 0 @ 2.20GHz.
NIC:Intel Corporation 82576 Gigabit Network Connection (rev 01) 
OPENDP run on a lcore.
examples/http_server run as http server.

root@h163:~# ab -n 30000 -c 500 2.2.2.2:8089/
This is ApacheBench, Version 2.3 <$Revision: 1528965 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 2.2.2.2 (be patient)
Completed 3000 requests
Completed 6000 requests
Completed 9000 requests
Completed 12000 requests
Completed 15000 requests
Completed 18000 requests
Completed 21000 requests
Completed 24000 requests
Completed 27000 requests
Completed 30000 requests
Finished 30000 requests


Server Software:
Server Hostname:        2.2.2.2
Server Port:            8089

Document Path:          /
Document Length:        63 bytes

Concurrency Level:      500
Time taken for tests:   0.642 seconds
Complete requests:      30000
Failed requests:        0
Total transferred:      4530000 bytes
HTML transferred:       1890000 bytes
Requests per second:    46695.59 [#/sec] (mean)
Time per request:       10.708 [ms] (mean)
Time per request:       0.021 [ms] (mean, across all concurrent requests)
Transfer rate:          6885.78 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        2    5   0.9      5       7
Processing:     2    6   1.0      6      10
Waiting:        1    4   1.0      4       9
Total:          7   11   0.5     11      14

Percentage of the requests served within a certain time (ms)
  50%     11
  66%     11
  75%     11
  80%     11
  90%     11
  95%     12
  98%     12
  99%     13
 100%     14 (longest request)
root@h163:~#

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
