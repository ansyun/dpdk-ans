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

/*
* This program is used to test netdp user space tcp stack
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

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

#define _GNU_SOURCE
#define __USE_GNU
#ifdef __USE_GNU
/* Access macros for `cpu_set'.  */
#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)
#endif
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/syscall.h> 

#define MAX_FLOW_NUM 200000
#define BUFFER_SIZE 5000
#define MAX_EVENTS 512
#define MAX_CPUS 8
static pthread_t app_thread[MAX_CPUS];
char *http_200 = "HTTP/1.0 200 OK\r\n"
                 "Cache-Control: no-cache\r\n"
                 "Connection: close\r\n"
                 "Content-Type: text/html\r\n"
                 "\r\n"
                 "<html><body><h1>200 OK</h1>\nEverything is fine.\n</body></html>\n";


/*----------------------------------------------------------------------------*/
int 
GetNumCPUs() 
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}
/*----------------------------------------------------------------------------*/
pid_t 
Gettid()
{
	return syscall(__NR_gettid);
}
/*----------------------------------------------------------------------------*/
static int
HandleReadEvent(int epoll_fd, struct epoll_event ev)
{
	int rd;
	int i;
	int len;
	int sent;
	char recv_buf[BUFFER_SIZE];
	int sockid = ev.data.fd;
	/* HTTP request handling */
	rd = netdpsock_recvfrom(sockid, recv_buf, BUFFER_SIZE, 0, NULL, NULL);

	if (rd <= 0) {
		return rd;
	}
	/* just response http 200*/
	len = strlen(http_200);
	sent = netdpsock_send(ev.data.fd, http_200, len, 0);

	ev.events = EPOLLIN | EPOLLOUT;
	netdpsock_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockid, NULL);
	//printf("read and close sockid:%d\n", sockid); //test purpose
	return rd;
}
/*----------------------------------------------------------------------------*/
int RunServerThread(void *arg)
{
	int core = *(int *)arg;
	int ret;
	int server_sockfd;
	int client_sockfd;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	int sin_size;
	int do_accept;

	/*initialize thread bind cpu*/
	cpu_set_t cpus;
	size_t n = GetNumCPUs();
	if (core < 0 || core >= (int) n) {
		errno = -EINVAL;
		exit(-1);
	}

	CPU_ZERO(&cpus);
	CPU_SET((unsigned)core, &cpus);
	sched_setaffinity(Gettid(), sizeof(cpus), &cpus);

	ret = netdpsock_init(NULL);
	if (ret != 0)
		printf("init sock failed \n");

	// end initialized
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(8089);

	if ((server_sockfd = netdpsock_socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error \n");
		return 1;
	}

	if (netdpsock_bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("bind error \n");
		return 1;
	}

	if (netdpsock_listen(server_sockfd, 2048) < 0)
	{
		printf("listen error \n");
		return 1;
	}

	sin_size = sizeof(struct sockaddr_in);
	/* wait for incoming accept events */
   	/* create epoll descriptor */
	int epoll_fd;
	epoll_fd = netdpsock_epoll_create(MAX_EVENTS);
	if (epoll_fd == -1)
	{
		printf("epoll_create failed \n");
		return 1;
	}

	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = server_sockfd;

	if (netdpsock_epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &ev) == -1)
	{
		printf("epll_ctl:server_sockfd register failed");
		netdpsock_close(server_sockfd);
		netdpsock_close(epoll_fd);
		return 1;
	}

	int nfds;
	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&my_addr;
	int ipAddr = pV4Addr->sin_addr.s_addr;
	char ipstr[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &ipAddr, ipstr, INET_ADDRSTRLEN );
	printf("open socket on ip:%s  port: %d\n", ipstr, ntohs(my_addr.sin_port));

	while (1)
	{
		nfds = netdpsock_epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds == -1)  {
			printf("start epoll_wait failed");
			netdpsock_close(server_sockfd);
			netdpsock_close(epoll_fd);
			return 1;
		}

		int i;
		do_accept = 0;
		for (i = 0; i < nfds; i++)
		{
			int sockid = events[i].data.fd;
			if (sockid == server_sockfd) { //accept case
				do_accept = 1;
			} 
                     else
                    {
        			if (events[i].events & EPOLLERR) { //epoll error event
        				int err;
        				socklen_t len = sizeof(err);

        				/* error on the connection */
        				netdpsock_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockid, NULL);
        				netdpsock_close(sockid);
        			} 
                            if (events[i].events == EPOLLIN) { //epollin  read and write
        				int ret = HandleReadEvent(epoll_fd, events[i]);
        				netdpsock_close(sockid);
        			} 
                            if (events[i].events == EPOLLOUT) { //epollout write
        				int LEN = strlen(http_200);
        				netdpsock_send(sockid, http_200, LEN, 0);
        				netdpsock_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockid, NULL);
        				netdpsock_close(sockid);
        			} 
                            if (events[i].events == EPOLLHUP) { //remote close the socket
        				netdpsock_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockid, NULL);
        				netdpsock_close(sockid);
        			}
                    }
		}
		if (do_accept) {
			while (1) {
				int c = netdpsock_accept(server_sockfd, NULL, NULL);
				if (c >= 0) {
					if (c >= MAX_FLOW_NUM) {
						printf("Invalid socket id %d.\n", c);
						exit(-1);
					}
					struct epoll_event ev;
					//accept connection and wait EPOLLIN EVENT
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = c;
					netdpsock_epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c, &ev);
					//      printf("Socket %d registered.\n", c);
				} else {  //c<0
					/*     printf("mtcp_accept() error %s\n",
					           strerror(errno));*/
					break;
				}
			}
		}//end if
	}
	netdpsock_close(server_sockfd);

	return 0;
}
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[] )
{
	int i;
	int core_limit = 1; //test use only 1 core
	int cores[core_limit];
	for (i = 0; i < core_limit; i++) {
		cores[i] = i;
		if (pthread_create(&app_thread[i],
		                   NULL, RunServerThread, (void *)&cores[i])) {
			perror("pthread_create");
			printf("Failed to create server thread.\n");
			exit(-1);
		}
	}
	for (i = 0; i < core_limit; i++) {
		pthread_join(app_thread[i], NULL);
	}

	exit(EXIT_SUCCESS);
}
