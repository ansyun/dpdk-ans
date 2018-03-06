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
  - EAL is based on dpdk-17.05.2;
  - Development enviroment is based on x86_64-native-linuxapp-gcc;
  - linux version：
4.4.0-45-generic (Ubuntu 16.04.1 LTS).
  - gcc version：
gcc version 5.4.0 20160609.

Support feature:
 - ANS initialize;
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
 - Socket API compatible with BSD, socket/bind/connect/listen/close/send/recv/epoll/writev/readv/shutdown...;
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
    - Support keep alive;
    - Support SO_REUSEPORT, multi application can listen the same port;
    - Support multicore tcp stack, per tcp stack per lcore;
    - Support TSO. 
 
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
    |     80k connection/s         | 
    |------------------------------| 
```
- L3 forwarding performance testing

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
- http server connection performance
```
CPU:Intel(R) Xeon(R) CPU E5-2430 0 @ 2.20GHz.
NIC:Intel Corporation 82576 Gigabit Network Connection (rev 01) 
ANS run on a lcore.
examples/http_server run as http server.

root@h163:~# ab -n 30000 -c 500 10.0.0.2:8089/
This is ApacheBench, Version 2.3 <$Revision: 1528965 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 10.0.0.2 (be patient)
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
Server Hostname:        10.0.0.2
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

- dpdk-nginx over ans performance
```
CPU:Intel(R) Xeon(R) CPU E5-2670 0 @ 2.60GHz.
NIC:82599ES 10-Gigabit SFI/SFP+ Network Connection (rev 01) 
ANS run on a lcore.
6 dpdk-nginx run on ANS.

./wrk -c 5k -d30s -t16  http://10.0.0.2/
Running 30s test @ http://10.0.0.2/
  16 threads and 5000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    41.93ms  119.14ms   1.73s    92.61%
    Req/Sec    18.16k     1.73k   26.34k    76.00%
  8700983 requests in 30.11s, 6.88GB read
Requests/sec: 288956.57
Transfer/sec:    233.95MB

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
  -Wl,--whole-archive -Wl,-lrte_mbuf -Wl,-lrte_mempool_ring -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,--no-whole-archive -Wl,-export-dynamic \

```

#### Support
-------
BSD LICENSE, you may use ANS freely.

For free support, please use ANS team mail list at anssupport@163.com, or QQ Group:86883521 (FULL), 519855957(No Full), or https://dpdk-ans.slack.com.
