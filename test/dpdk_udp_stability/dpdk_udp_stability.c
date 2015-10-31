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

#define UDP_SOCK_NUM   100
#define UDP_PORT_START 8888

#define UDP_SEND_MAX_SIZE  500
#define UDP_RECV_MAX_SIZE  20480


struct udp_sock
{
    int fd;
    int packets_nb;
    unsigned port;
} ;

struct udp_sock  g_udp_sock[UDP_SOCK_NUM];

struct epoll_event events[20];


int udp_sock_create(int epfd, unsigned port)
{
    int fd;
    int ret;
    struct sockaddr_in addr_in;  
    struct epoll_event event;
 
    fd = netdpsock_socket(AF_INET, SOCK_DGRAM, 0);	
    if(fd < 0)
    {
        printf("create socket for port(%d) failed \n", port);
        return -1;
    }

    memset(&addr_in, 0, sizeof(addr_in));      
    addr_in.sin_family = AF_INET;  
    addr_in.sin_port   = htons(port);  
    addr_in.sin_addr.s_addr = htonl(0x02020202); 

    ret =  netdpsock_bind(fd, (struct sockaddr *)&addr_in, sizeof(addr_in) );
    if(ret != 0)
    {
        printf("bind socket for port(%d) failed \n", port);
        return -1;
    }

    event.data.fd = fd;  
    event.events = EPOLLIN | EPOLLET;  

    ret = netdpsock_epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret != 0)
    {
        printf("epoll ctl failed \n");
        return -1;
    }


    return fd;
    
}

struct udp_sock *udp_sock_find(int fd)
{
    int i;
    
    for(i = 0; i < UDP_SOCK_NUM; i++)
    {
        if(g_udp_sock[i].fd == fd)
        {
            return &g_udp_sock[i];
        }
    }
    return NULL;
}


int udp_sock_handle(int epfd, struct epoll_event *event, int curr_event, int sum_event)
{
    int ret;
    char send_data[UDP_SEND_MAX_SIZE] = {3};
    char recv_buf[UDP_RECV_MAX_SIZE];
    int recv_len; 
    socklen_t addrlen = 0;
    struct sockaddr dest_addr;
    struct udp_sock * sock;
    struct epoll_event rm_event;

    memset(send_data, 0, sizeof(send_data));

    if ((event->events & EPOLLERR) || (event->events & EPOLLHUP) || (!(event->events & EPOLLIN)))  
    {  
        printf("dpdk socket(%d) error, event:0x%x\n", event->data.fd, event->events);
        netdpsock_close (event->data.fd);  
        return -1;  
    }   

    if (event->events & EPOLLIN)
    {

        sock = udp_sock_find(event->data.fd);
        if(sock == NULL)
        {
            printf("can't find sock by fd(%d) curr event:%d, sum event:%d \n", event->data.fd, curr_event, sum_event);
            return 0;
        }
            
        while(1)
        {
            recv_len = netdpsock_recvfrom(event->data.fd, recv_buf, UDP_RECV_MAX_SIZE, 0, &dest_addr, &addrlen);
            if(recv_len == 0)
            {
               // printf("no data in socket \n");
                break;
            }

            if(recv_len < 0)
            {
                printf("recv msg failed, fd %d, packets num %d \n", event->data.fd, sock->packets_nb);
                continue;
            }

            printf("Recv: %s \n", recv_buf);

            sock->packets_nb++;
            
            sprintf(send_data, "Hello, linux_udp, fd:%d, num:%d !", event->data.fd, sock->packets_nb);
            send_data[UDP_SEND_MAX_SIZE -1] = 0;
            
            netdpsock_sendto(event->data.fd, send_data, sizeof(send_data), 0, &dest_addr, addrlen);

        }

        if(sock->packets_nb > 100 )
        {
            rm_event.data.fd = sock->fd;  
            rm_event.events = 0;  
        //    printf("close fd %d \n", sock->fd);

            ret = netdpsock_epoll_ctl(epfd, EPOLL_CTL_DEL, sock->fd, &rm_event);
           
            netdpsock_close(sock->fd);
            
            sock->fd = -1;
            sock->fd = udp_sock_create(epfd, sock->port);
            sock->packets_nb = 0;
        }
        
    }
    else
    {
        printf("unknow event %x, fd:%d \n", event->events, event->data.fd);
    }

    return 0;
        
}


int main(void)
{
    int ret;
    int i = 0 ;
    int fd[UDP_SOCK_NUM];
    int epfd;
    int packet_nb = 1;
    struct sockaddr_in remote_addr;  
    struct epoll_event event;

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

    for(i = 0; i < UDP_SOCK_NUM; i++)
    {
        g_udp_sock[i].fd = -1;
        g_udp_sock[i].fd = udp_sock_create(epfd, UDP_PORT_START + i);
        g_udp_sock[i].packets_nb = 0;
        g_udp_sock[i].port = UDP_PORT_START + i;
    }
        
    printf("start dpdk udp application \n");

    int event_num = 0;
    
    while(1)
    {
        event_num = netdpsock_epoll_wait (epfd, events, 20, -1);
      //   printf("epoll wait recv event num: %d \n", event_num);
       if(event_num <= 0)
           printf("epoll wait failed, event num(%d) \n", event_num);

        for(i = 0; i < event_num; i++)
        {
            udp_sock_handle(epfd, &events[i], i, event_num);
        }
    
    }

    netdpsock_close(epfd);

    return 0;
}
