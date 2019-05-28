#### TCP/IP stack for dpdk
--------------
ANS(accelerated network stack) is DPDK native TCP/IP stack and also refer to [FreeBSD](http://freebsd.org) implementation. ANS provides a userspace TCP/IP stack for use with the Intel [DPDK](http://dpdk.org/). 

--------------
![](/doc/design/ans_code.png)
- ans: accelerated network stack process.
 
- librte_ans: TCP/IP stack static library. ANS use dpdk mbuf, ring, memzone, mempool, timer, spinlock. so zero copy mbuf between dpdk and ANS. 

- librte_anssock: ANS socket lib for application, zero copy between ANS and application.

- librte_anscli: ANS cli lib for route/ip/neigh/link configuration.

- cli: Command for configure ANS tcp/ip stack.

- example: ANS application example.
 
- test: Example application with ANS for testing ANS tcp/ip stack

Support environment
  - EAL is based on dpdk-18.11;
  - linux version：
4.4.0-45-generic (Ubuntu 16.04.1 LTS).
  - gcc version：
gcc version 5.4.0 20160609.

Support feature:
 - ANS initialize;
 - ANS run in container;
 - Ether, zero copy between NIC and ANS TCP/IP stack;
 - ARP, ARP timeout;
 - IP layer, IP fragmentation and reassemble;
 - High performance routing;
 - ICMP;
 - ACL;
 - Bypass traffic to linux kernel;
 - Sync IP/Route from linux kernel;
 - Support dynamic routing(OSPF/BGP...);
 - Support DHCP client;
 - Command Line Interface:
     - Adding, deleting, showing IP addres;
     - Adding, deleting, showing static route;
     - Showing neigh table;
     - Showing interface and statistics;
     - Showing IP statistics;
     - Adding, deleting, showing ACL;
     - Adding, deleting, showing bypass rule;
     - Showing port queue lcore mapping;
     - Adding, deleting, showing flow filter rule;     
 - UDP protocol;
 - Socket layer;
 - Socket API compatible with BSD, APP can choice ANS socket or linux socket based on a switch.
    - socket/bind/connect/listen/close/send/recv/epoll/writev/readv/shutdown...;
 - Support openssl;
 - TCP protocol; 
    - Support reliable transmission;
    - Support dupack-based retransmission, timeout-based retransmission;
    - Support flow control;
    - Support congestion control: newreno/cubic/vegas...;
    - Support maximum segment size;
    - Support selective acknowledgments;
    - Support window scaling;
    - Support TCP timestamps;
    - Support TCP ECN;
    - Support keep alive;
    - Support SO_REUSEPORT, multi application can listen the same port;
    - Support multicore tcp stack, per tcp stack per lcore;
    - Support TSO. 
  - Vrouter;
     - Support vhost;
     - Support virtio-user;     
     - Support kni; 
     - Support tap; 
  - Hardware;     
     - x86: broadwell, haswell, ivybridge, knl, sandybridge, westmere and so on.
     - arm: arm64.
  - SoC and embeb system;      
     - disk: 4M, memory: 128M
     - Support TCP connection: 512, ACL: 16, application: 8
     
#### ANS User Guide
--------------
```
# git clone https://github.com/ansyun/dpdk-ans.git
# export RTE_ANS=/home/mytest/dpdk-ans
# ./install_deps.sh
# cd ans
# make
```
[ANS User Guide](https://github.com/ansyun/dpdk-ans/tree/master/doc/guides/ans_user_guide.pdf)

#### ANS Architecture
--------------
![](/doc/design/ans_arch.png)


#### TCP Deployment
--------------
```
         |-------|       |-------|       |-------|
APP      |anssock|       |anssock|       |anssock|
         |-------|       |-------|       |-------|
             |               |               |			
            fd              fd              fd
--------------------------------------------------
ANS          |               |               |
         |-------|       |-------|       |-------|
         | TCP   |       |  TCP  |       | TCP   |
         |---------------------------------------|       
         |               IP/ARP/ICMP             |
         |---------------------------------------|       
         |LCORE0 |       |LCORE1 |       |LCORE2 |
         |-------|       |-------|       |-------|
             |               |               |
         |---------------------------------------| 
         |                  NIC + RSS            | 
         |---------------------------------------| 
```
 - NIC distribute packets to different lcore based on RSS, so same TCP flow are handled in the same lcore.
 - Each lcore has own TCP stack, free lock.
 - IP/ARP/ICMP are shared between lcores.
 - APP process runs as a tcp server.
    - If App process only creates one listen socket, the listen socket only listens on one lcore and accept tcp connections from the lcore, so the APP process number shall large than the lcore number. The listen sockets of APP processes are created on each lcore averagely. For example: ans(with -c 0x3) run on two lcore, shall run two nginx(only run master ), one nginx listens on lcore0, another nginx listens on lcore1.
 - APP process can bind the same port if enable reuseport, APP process could accept tcp connection by round robin.
 - If NIC don't support multi queue or RSS, shall enhance ans_main.c, reserve one lcore to receive and send packets from NIC, and distribute packets to lcores of ANS tcp stack by software RSS.

#### Performance Testing
--------------
  [ANS Performance Test Report](https://github.com/ansyun/dpdk-ans/tree/master/doc/guides/ans_performance_report.pdf)
- TCP server performance testing
  
```
    |------------------------------| 
    |      TCP Server CPS          |
    |------------------------------| 
    |       ANS with epoll         | 
    |         (one core)           |
    |------------------------------|
    |     100k connection/s         | 
    |------------------------------| 
```
- L3 forwarding with NIC performance testing

  ENV: CPU- Intel(R) Xeon(R) CPU E5-2683 v3 @ 2.00GHz, NIC- Intel 82599ES 10-Gigabit,  Test tool:pktgen-DPDK
```
    |--------------------------------------| 
    |      L3 forwarding performance       |
    |             (one lcore)              |
    |--------------------------------------| 
    | Packet size(byte)| Throughput(Mpps)  | 
    |--------------------------------------|
    |     64           |       11.78       | 
    |--------------------------------------| 
    |     128          |      Line Rate    | 
    |--------------------------------------| 
 
```
- L3 forwarding with vhost/virtio performance testing

  ENV: Intel(R) Xeon(R) CPU E5-2618L v4 @ 2.20GHz, NIC- vhost/virtio,  Test tool:pktgen-DPDK
```
    |--------------------------------------| 
    |      L3 forwarding performance       |
    |             (one lcore)              |
    |--------------------------------------| 
    | Packet size(byte)| Throughput(Mpps)  | 
    |--------------------------------------|
    |     64           |       6.33        | 
    |--------------------------------------| 
    |     128          |       5.94        | 
    |--------------------------------------| 
    |     256          |     Line Rate     | 
    |--------------------------------------| 
```

- dpdk-redis performance testing
```
====ENV=== 
CPU:Intel(R) Xeon(R) CPU E5-2609 v4 @ 1.70GHz.
NIC:Ethernet controller: Intel Corporation 82599ES 10-Gigabit SFI/SFP+ Network Connection (rev 01) 
ANS run on a lcore.

root@ubuntu:~/src/dpdk-redis# ./src/redis-benchmark -h 10.0.0.2 -p 6379 -n 100000 -c 50 -q
PING_INLINE: 138888.89 requests per second
PING_BULK: 141242.94 requests per second
SET: 140449.44 requests per second
GET: 141043.72 requests per second
INCR: 141442.72 requests per second
LPUSH: 141043.72 requests per second
LPOP: 140449.44 requests per second
SADD: 141643.06 requests per second
SPOP: 141843.97 requests per second
LPUSH (needed to benchmark LRANGE): 141442.72 requests per second
LRANGE_100 (first 100 elements): 48192.77 requests per second
LRANGE_300 (first 300 elements): 14330.75 requests per second
LRANGE_500 (first 450 elements): 10405.83 requests per second
LRANGE_600 (first 600 elements): 7964.95 requests per second
MSET (10 keys): 107758.62 requests per second

```

- dpdk-nginx CPS performance
```
CPU: Intel(R) Xeon(R) CPU E5-2683 v3 @ 2.00GHz
NIC:82599ES 10-Gigabit SFI/SFP+ Network Connection (rev 01) 
ANS run on a lcore.
4 dpdk-nginx run on ANS.

# ./wrk --timeout=1 --latency -H "Connection: close" -t20 -c100 -d30s http://10.0.0.2
Running 30s test @ http://10.0.0.2
  20 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   480.80us   73.12us   4.66ms   88.86%
    Req/Sec     5.32k   125.94     6.98k    84.30%
  Latency Distribution
     50%  478.00us
     75%  505.00us
     90%  535.00us
     99%  648.00us
  3186335 requests in 30.10s, 2.51GB read
Requests/sec: 105860.26
Transfer/sec:     85.31MB

ANS run on two lcore.
8 dpdk-nginx run on ANS.
# ./wrk --timeout=1 --latency -H "Connection: close" -t20 -c100 -d30s http://10.0.0.2
Running 30s test @ http://10.0.0.2
  20 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   251.70us   89.45us   4.39ms   68.59%
    Req/Sec    10.28k   340.17    12.40k    67.67%
  Latency Distribution
     50%  246.00us
     75%  310.00us
     90%  363.00us
     99%  480.00us
  6155775 requests in 30.10s, 4.84GB read
Requests/sec: 204512.88
Transfer/sec:    164.81MB

```
- dpdk-nginx QPS performance
```
CPU: Intel(R) Xeon(R) CPU E5-2683 v3 @ 2.00GHz
NIC:82599ES 10-Gigabit SFI/SFP+ Network Connection (rev 01) 
ANS run on a lcore.
8 dpdk-nginx run on ANS.

# ./wrk --timeout=1 --latency -t20 -c100 -d30s http://10.0.0.2
Running 30s test @ http://10.0.0.2
  20 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   257.91us  407.36us  24.37ms   98.77%
    Req/Sec    20.85k     1.84k   26.88k    70.23%
  Latency Distribution
     50%  214.00us
     75%  289.00us
     90%  338.00us
     99%  825.00us
  12488349 requests in 30.10s, 9.89GB read
Requests/sec: 414900.42
Transfer/sec:    336.31MB

ANS run on two lcore.
10 dpdk-nginx run on ANS.
# ./wrk --timeout=1 --latency -t20 -c100 -d30s http://10.0.0.2
Running 30s test @ http://10.0.0.2
  20 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   184.60us  165.39us  12.97ms   98.92%
    Req/Sec    26.69k     0.93k   29.92k    79.17%
  Latency Distribution
     50%  186.00us
     75%  200.00us
     90%  216.00us
     99%  370.00us
  15985217 requests in 30.10s, 12.65GB read
Requests/sec: 531077.97
Transfer/sec:    430.48MB

```

#### Examples
-------
- [dpdk http server](https://github.com/ansyun/dpdk-ans/tree/master/examples/http_server): http server run on ANS tcp/ip stack.
- [dpdk https server](https://github.com/ansyun/dpdk-ans/tree/master/examples/https_server): https server run on ANS tcp/ip stack.
- [dpdk-nginx](https://github.com/ansyun/dpdk-nginx), nginx was porting to run on ANS tcp/ip stack.
- [dpdk-redis](https://github.com/ansyun/dpdk-redis), redis was porting to run on ANS tcp/ip stack.
- [dpdk-iperf](https://github.com/ansyun/dpdk-iperf), iperf was porting to run on ANS tcp/ip stack.
- [dpdk-httpperf](https://github.com/ansyun/dpdk-httpperf), wrk was porting to run on ANS tcp/ip stack.

#### [Wiki Page](https://github.com/ansyun/dpdk-ans/wiki)
-------
You can get more information and instructions from [wiki page](https://github.com/ansyun/dpdk-ans/wiki).

#### Notes
-------
- You shall use the same dpdk version as ans libs used.
- Shall use the same gcc version to compile your application.
- ANS socket application run as a secondary dpdk process, If you got below log, shall execute below commands to disable ASLR.
```
EAL: WARNING: Address Space Layout Randomization (ASLR) is enabled in the kernel.
EAL: This may cause issues with mapping memory into secondary processes
$ sudo sysctl -w kernel.randomize_va_space=0
```
- You shall modify the NIC configuration in ans_main.c based on your NIC type.
- ANS didn't support loopback interface, so socket client and server can't be in the same ANS tcp/ip stack.
- In order to improve ANS performance, you shall isolate ANS'lcore from kernel by isolcpus and isolcate interrupt from ANS's lcore by update /proc/irq/default_smp_affinity file.
- ANS run as dpdk primary process, when startup ANS, shall stop other secondary processes(nginx/redis/http_server).
- Don't run ANS on lcore0, it will effect ANS performance.

- You shall include dpdk libs as below way because mempool lib has __attribute__((constructor, used)) in latest version, otherwise your application would coredump.
```
  -$(RTE_ANS)/librte_anssock/librte_anssock.a \
  -L$(RTE_SDK)/$(RTE_TARGET)/lib \
  -Wl,--whole-archive -Wl,-lrte_mbuf -Wl,-lrte_mempool_ring -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,--no-whole-archive -Wl,-export-dynamic -lnuma \

```

#### Support
-------
BSD LICENSE, you may use ANS freely.

For free support, please use ANS team mail list at anssupport@163.com, or QQ Group:86883521 (FULL), 519855957(No Full), or https://dpdk-ans.slack.com.
