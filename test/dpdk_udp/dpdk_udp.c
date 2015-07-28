/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 Netdp Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Netdp Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <termios.h>
#ifndef __linux__
  #ifdef __FreeBSD__
    #include <sys/socket.h>
  #else
    #include <net/socket.h>
  #endif
#endif

#include <sys/time.h>

#include "netdpsock_intf.h"

/*
for(i = 0; i < 10; i++)
{
    gettimeofday(&start, NULL);

    ret = socket(AF_INET, SOCK_DGRAM, 0);	

    gettimeofday(&end, NULL);
    interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

    printf("stardard socket, interval = %fms\n", interval/1000.0);
}

for(i = 0; i < 10; i++)
{
    gettimeofday(&start, NULL);

    ret = netdpsock_socket(AF_INET, SOCK_DGRAM, 0);	

    gettimeofday(&end, NULL);
    interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

    printf("netdp socket, interval = %fms\n", interval/1000.0);
}
*/

int main(void)
{
    int ret;
    int i = 0 ;
    int fd, recvfd = 0;
    char send_data[2048];
    struct timeval start, end;
    struct sockaddr_in addr_in;  
    struct sockaddr_in remote_addr;  

    int interval;

    ret = netdpsock_init("dpdk_udp");
    if(ret != 0)
        printf("init sock ring failed \n");

    
    fd = netdpsock_socket(AF_INET, SOCK_DGRAM, 0);	
    if(fd < 0)
    {
        printf("create socket failed \n");
        return -1;
    }

    memset(&addr_in, 0, sizeof(addr_in));      
    addr_in.sin_family = AF_INET;  
    addr_in.sin_port   = htons(8888);  
    addr_in.sin_addr.s_addr = htonl(0x02020202); 

    ret =  netdpsock_bind(fd, (struct sockaddr *)&addr_in, sizeof(addr_in) );
    if(ret != 0)
    {
        printf("bind socket failed \n");
        netdpsock_close(fd);
        return -1;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));      
    remote_addr.sin_family = AF_INET;  
    remote_addr.sin_port   = htons(9999);  
    remote_addr.sin_addr.s_addr = htonl(0x02020205); 

    printf("start dpdk udp application \n");


    char recv_buf[2038];
    int recv_len; 
    while(i < 2000000)
    {

        sprintf(send_data, "Hello, linux_udp, num:%d !", i);

        netdpsock_sendto(fd, send_data, strlen(send_data), 0, (struct sockaddr *)&remote_addr,  sizeof(remote_addr));

        recv_len = netdpsock_recvfrom(&recvfd, recv_buf, 2048, 0, NULL, NULL);

        i++;

     //   if((i % 100000)  == 0 )
            printf("Recv: %s \n", recv_buf);
        
        
    }

    netdpsock_close(fd);
    return 0;
}
