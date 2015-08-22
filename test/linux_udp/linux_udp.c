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
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>


#define UDP_REMOTE_PORT_START 8888
#define SEND_MAX_SIZE  500
#define RECV_MAX_SIZE  20480

struct epoll_event events[20];

int sockfd;
int remote_port_num = 100;

void udp_send_thread()  
{  
    int i;
    int data_num = 0;
    char sendline[SEND_MAX_SIZE] = {2};
    struct sockaddr_in remote_addr;

    bzero(&remote_addr, sizeof(remote_addr));
    while(1)
    {
        for(i = 0; i < remote_port_num; i++)
        {
            remote_addr.sin_family = AF_INET;
            remote_addr.sin_port = htons(UDP_REMOTE_PORT_START + i);
            remote_addr.sin_addr.s_addr = inet_addr("2.2.2.2");

            sprintf(sendline, "Hello, dpdk_udp, num:%d !", data_num);
            sendline[SEND_MAX_SIZE -1] = 0;
            sendto(sockfd, sendline, sizeof(sendline) , 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
        }
        data_num++;
        usleep(2000);
    }
    
}


int main(int argc, char **argv)
{
    int i;
    int flags, ret;  
    int epfd;
    int loop = 0;
    struct epoll_event event;
    struct sockaddr_in servaddr;
    pthread_t id;  


    /* create epoll socket */
    epfd = epoll_create(10);
    if(epfd < 0)
    {
        printf("create epoll socket failed \n");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        printf("create udp socket failed \n");
        return -1;
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9999);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    flags = fcntl (sockfd, F_GETFL, 0);  
    if (flags == -1)  
    {  
        printf("fcntl get udp socket flag failed \n");
        return -1;
    }  

    flags |= O_NONBLOCK;  
    ret = fcntl (sockfd, F_SETFL, flags);  
    if (ret == -1)  
    {  
        printf("fcntl set udp socket flag failed \n");
        return -1;
    }  

    event.data.fd = sockfd;  
    event.events = EPOLLIN | EPOLLET;  

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if(ret != 0)
    {
        printf("epoll ctl failed \n");
        return -1;
    }

    printf("start linux udp application \n");

      
    ret=pthread_create(&id, NULL, (void *) udp_send_thread, NULL);  
    if(ret!=0)  
    {  
        printf ("Create pthread error!\n");  
        return 0;  
    }  
    
    char recvline[RECV_MAX_SIZE];

    int event_num = 0;
    int recv_len = 0;
    
    while(1)
    {

        event_num = epoll_wait (epfd, events, 20, -1);
        for(i = 0; i < event_num; i++)
        {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))  
            {  
                printf("socket(%d) error\n", events[i].data.fd);
                close (events[i].data.fd);  
                continue;  
            }   
            else if (events[i].events & EPOLLIN)
            {
                while(1)
                {
                    recv_len = recvfrom(events[i].data.fd, recvline, RECV_MAX_SIZE, 0, NULL, NULL);
                    if((recv_len <= 0) && (EAGAIN != errno))  
                    {
                        printf("socke error, recv_len:%d, errno:%d \n", recv_len, errno);
                        break;
                    }
                    else if((recv_len <= 0) && (EAGAIN == errno))
                    {
                 //       printf("no data in socket, recv_len:%d, errno:%d \n", recv_len, errno);
                        break;
                    }

                    printf("Data len: %d Recv: %s \n", recv_len, recvline);
                }
            
            }
            else
            {
                printf("unknow event %x, fd:%d \n", events[i].events, events[i].data.fd);
            }
            
        }
    
    }


   close(sockfd);
   close(epfd);
    
}
