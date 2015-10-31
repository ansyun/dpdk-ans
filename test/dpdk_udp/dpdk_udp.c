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
#include <sys/epoll.h>

#ifndef __linux__
  #ifdef __FreeBSD__
    #include <sys/socket.h>
  #else
    #include <net/socket.h>
  #endif
#endif

#include <sys/time.h>

#include "netdpsock_intf.h"
#include "netdp_errno.h"


struct epoll_event events[20];

int main(void)
{
    int ret;
    int i = 0 ;
    int fd, recvfd = 0;
    int epfd;
    int data_num =0;
    char send_data[2048];
    struct timeval start, end;
    struct sockaddr_in addr_in;  
    struct sockaddr_in remote_addr;  
    struct epoll_event event;
    char recv_buf[2038];
    int recv_len; 

    ret = netdpsock_init(NULL);
    if(ret != 0)
        printf("init sock ring failed \n");


    /* create epoll socket */
    epfd = netdpsock_epoll_create(0);
    if(epfd < 0)
    {
        printf("create epoll socket failed \n");
        return -1;
    }

    fd = netdpsock_socket(AF_INET, SOCK_DGRAM, 0);	
    if(fd < 0)
    {
        printf("create socket failed \n");
        netdpsock_close(epfd);
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
        netdpsock_close(epfd);
        return -1;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));      
    remote_addr.sin_family = AF_INET;  
    remote_addr.sin_port   = htons(9999);  
    remote_addr.sin_addr.s_addr = htonl(0x02020205); 


    event.data.fd = fd;  
    event.events = EPOLLIN | EPOLLET;  

    ret = netdpsock_epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret != 0)
    {
        printf("epoll ctl failed \n");
        netdpsock_close(fd);
        netdpsock_close(epfd);
        return -1;
    }

    printf("start dpdk udp application \n");

    int event_num = 0;
    memset(send_data, 0, sizeof(send_data));
    
    while(1)
    {
        event_num = netdpsock_epoll_wait (epfd, events, 20, -1);
        if(event_num <= 0)
        {
            printf("epoll_wait failed \n");
            continue;
        }
            
        for(i = 0; i < event_num; i++)
        {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))  
            {  
                printf("dpdk socket(%d) error\n", events[i].data.fd);
                netdpsock_close (events[i].data.fd);  
                continue;  
            } 
            
            if (events[i].events & EPOLLIN)
            {
                while(1)
                {
                    recv_len = netdpsock_recvfrom(events[i].data.fd, recv_buf, 2048, 0, NULL, NULL);

                    if(recv_len > 0)  
                    {  
                        printf("Recv: %s \n", recv_buf);

                        data_num++;
                        sprintf(send_data, "Hello, linux_udp, num:%d !", data_num);

                        netdpsock_sendto(events[i].data.fd, send_data, strlen(send_data) + 1, 0, (struct sockaddr *)&remote_addr,  sizeof(remote_addr));
                    } 
                    else if(recv_len < 0)
                    {
                        if (errno == NETDP_EAGAIN)   
                        {
                            break;
                        }
                        else
                        {
                            printf("remote close the socket, errno %d \n", errno);
                            netdpsock_close(events[i].data.fd);
                            break;
                        }
                    }
                    else
                    {
                        printf("remote close the socket, len %d \n", recv_len);
                        netdpsock_close(events[i].data.fd);
                        break;
                    }

                }
            
            }
            else
            {
                printf("unknow event %x, fd:%d \n", events[i].events, events[i].data.fd);
            }

            
        }
    
    }



    netdpsock_close(fd);
    netdpsock_close(epfd);

    return 0;
}
