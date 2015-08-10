####TCP/IP stack for dpdk
--------------
Netdp is porting from [FreeBSD](http://freebsd.org) TCP/IP stack, and provide a userspace TCP/IP stack for use with the Intel [dpdk](http://dpdk.org/). 

- librte_netdp: TCP/IP stack static library. netdp use dpdk mbuf, ring, memzone, mempool, timer, spinlock. so zero copy mbuf between dpdk and netdp. 

- librte_netdpsock: Netdp socket lib for application, zero copy between netdp and application.

- netdp_cmd: Command for configure netdp tcp/ip stack.
 
- netdp_test: Example application with netdp for testing netdp tcp/ip stack

Support environment
  - EAL is based on dpdk-2.0.0
  - Development enviroment is based on x86_64-native-linuxapp-gcc
  - TCP/IP stack is based on FreeBSD 10.0-RELEASE
  - linux version：
Linux ubuntu 3.11.0-15-generic #25~precise1-Ubuntu SMP Thu Jan 30 17:39:31 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
  - gcc version：
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/4.6/lto-wrapper
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu/Linaro 4.6.3-1ubuntu5' --with-bugurl=file:///usr/share/doc/gcc-4.6/README.Bugs --enable-languages=c,c++,fortran,objc,obj-c++ --prefix=/usr --program-suffix=-4.6 --enable-shared --enable-linker-build-id --with-system-zlib --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --with-gxx-include-dir=/usr/include/c++/4.6 --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --enable-gnu-unique-object --enable-plugin --enable-objc-gc --disable-werror --with-arch-32=i686 --with-tune=generic --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 4.6.3 (Ubuntu/Linaro 4.6.3-1ubuntu5)

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
 - 
Next Planning
- Porting TCP from FreeBSD to netdp
- Open data plane.
- VxVLAN.
- Performance testing.
- IPFW

####[Wiki Page](https://github.com/dpdk-net/netdp/wiki)
-------
You can get more information and instructions from [wiki page](https://github.com/dpdk-net/netdp/wiki).

####Support
-------
BSD LICENSE, you may use netdp freely.

For free support, please use netdp team mail list at zimeiw@163.com. or QQ Group:86883521
