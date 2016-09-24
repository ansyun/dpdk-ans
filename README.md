####TCP/IP stack for dpdk
--------------
ANS(accelerated network stack) is porting from [FreeBSD](http://freebsd.org) TCP/IP stack, and provide a userspace TCP/IP stack for use with the Intel [dpdk](http://dpdk.org/). 

- ans: accelerated network stack process.
 
- librte_ans: TCP/IP stack static library. ANS use dpdk mbuf, ring, memzone, mempool, timer, spinlock. so zero copy mbuf between dpdk and ANS. 

- librte_anssock: ANS socket lib for application, zero copy between ANS and application.

- cli: Command for configure ANS tcp/ip stack.
 
- test: Example application with ANS for testing ANS tcp/ip stack

Support environment
  - EAL is based on dpdk-16.07
  - Development enviroment is based on x86_64-native-linuxapp-gcc
  - TCP/IP stack is based on FreeBSD 10.0-RELEASE
  - linux version：
3.16.0-30-generic
  - gcc version：
gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04)

Support feature:
 - ANS initialize
 - Ether, zero copy between NIC and ANS TCP/IP stack.
 - ARP, ARP timeout
 - IP layer, IP fragmentation and reassemble
 - Routing
 - ICMP
 - Commands for adding, deleting, showing IP address
 - Commands for adding, deleting, showing static route
 - Commands for showing ARP table
 - UDP protocol
 - Socket layer, share memory.
 - Socket API, socket/close/send/recv/epoll/writev/readv/shutdown.
 - TCP protocol
    - Free lock, hash table.
    - Support SO_REUSEPORT, multi application can listen the same port.
    - Support multicore tcp stack, per tcp stack per lcore.
    - Support TSO.
 - 
Next Planning
- Enhance socket API
- Performance testing.

####TCP Deployment
--------------
```
         |-------|       |-------|       |-------|
         |  APP  |       |  APP  |       |  APP  |
         |-------|       |-------|       |-------|
             |               |               |
--------------------------------------------------
anssock      |               |               |			
             fd              fd              fd
             |               |               |
--------------------------------------------------
ANS          |               |               |
         |-------|       |-------|       |-------|
         | TCP   |       |  TCP  |       | TCP   |
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
 - APP process runs as a tcp server.
  - If App process only creates one listen socket, the listen socket only listens on one lcore and accept tcp connections from the lcore, so the APP process number shall large than the lcore number. The listen sockets of APP processes are created on each lcore averagely. For example: ans(with -c 0x3) run on two lcore, shall run two nginx(only run master ), one nginx listens on lcore0, another nginx listens on lcore1.
  - If App process creates many listen sockets, the listen sockets number shall be equal to the lcore numbers. these listen sockets can be created on each lcore averagely too. For example: ans(with -c 0x3) run on two lcore, redis server(one process) shall create two listen socket, one listen socket is created on lcore0, another listen socket is created on lcore1.
 - APP process runs as a tcp client. App process can communicate with each lcore. The tcp connection can be located in specified lcore automaticly.
 - APP process can bind the same port if enable reuseport, APP process could accept tcp connection by round robin.
 - If NIC don't support multi queue or RSS, shall enhance ans_main.c, reserve one lcore to receive and send packets from NIC, and distribute packets to lcores of ANS tcp stack by software RSS.

####Performance Testing
--------------
- TCP server performance testing
 
 ENV: CPU- intel xeon 2.3G, NIC- 10G, Test tool:ab 

 Procedure: ab establish tcp connection to ANS tcp server, ab download one data, ANS tcp server close socket.

 Command: ab -n 1000000 -c 500  2.2.2.2:8089/
 
 Notes: shall increase test linux PC local port range (net.ipv4.ip_local_port_range = 1024 65000).
```
    |--------------------------------------| 
    |      TCP Server accept performance   |
    |--------------------------------------| 
    | Linux with epoll |   ANS with epoll  | 
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
- dpdk-redis performance testing
```
====ENV=== 
CPU:Intel(R) Xeon(R) CPU E5-2430 0 @ 2.20GHz.
NIC:Intel Corporation 82576 Gigabit Network Connection (rev 01) 
ANS run on a lcore.

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
ANS run on a lcore.
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
- Downloading file from dpdk-nginx
```
CPU:Intel(R) Xeon(R) CPU E5-2430 0 @ 2.20GHz.
NIC:Intel Corporation 82576 Gigabit Network Connection (rev 01) 
ANS run on a lcore.

# wget  2.2.2.2:80/nginx_large_data
--2016-04-24 08:46:24--  http://2.2.2.2/nginx_large_data
Connecting to 2.2.2.2:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: 80185146 (76M) [application/octet-stream]
Saving to: ‘nginx_large_data.3’

100%[=================================================>] 80,185,146   111MB/s   in 0.7s

2016-04-24 08:46:25 (111 MB/s) - ‘nginx_large_data.3’ saved [80185146/80185146]

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

####Examples
-------
- dpdk_tcp_server, tcp server run on ANS tcp/ip stack.
- [dpdk-nginx](https://github.com/opendp/dpdk-nginx), nginx was porting to run on ANS tcp/ip stack.

####[Wiki Page](https://github.com/dpdk-net/ans/wiki)
-------
You can get more information and instructions from [wiki page](https://github.com/opendp/dpdk-ans/wiki).

####Notes
-------
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

- You shall include dpdk libs as below way because mempool lib has __attribute__((constructor, used)) in dpdk-16.07 version, otherwise your application would coredump.
```
  -$(RTE_ANS)/librte_anssock/librte_anssock.a \
  -L$(RTE_SDK)/$(RTE_TARGET)/lib \
  -Wl,--whole-archive -Wl,-lrte_mbuf -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,--no-whole-archive -Wl,-export-dynamic \

```

####Support
-------
BSD LICENSE, you may use ANS freely.

For free support, please use ANS team mail list at zimeiw@163.com, or QQ Group:86883521, or https://dpdk-ans.slack.com.
