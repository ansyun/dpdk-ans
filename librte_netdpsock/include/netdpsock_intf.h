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


#ifndef __NETDP_SOCKET_INTF_H__
#define __NETDP_SOCKET_INTF_H__

/**
 *  Init netdp socket lib and register a user to opendp. One process shall only call it once.
 *
 * @param   
 *
 * @return  
 *
 */
int netdpsock_init();

/**
 *  creates  an endpoint for communication and returns a descriptor.
 *
 * @param domain      
 *
 * @param type
 *
 * @param protocol
 *  
 * @return  
 * On success, a file descriptor for the new socket is returned.  On error, -1 is returned, and errno is set appropriately.
 */
int netdpsock_socket(int domain, int type, int protocol);

/**
 * binds a local IP address.
 *
 * @param sockfd
 *
 * @param addr
 *
 * @param addrlen
 *
 * @return  
 * On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
int netdpsock_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * connect to remote IP address.
 *
 * @param sockfd      
 *
 * @param addr
 *
 * @param addrlen
 *
 * @return  
 * If the connection or binding succeeds, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
 int netdpsock_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 *  sendto user data via socket. This is nonblocking function, so shall check EAGAIN.
 *
 * @param       
 * @param 
 *
 * @return  
 *   On success, these calls return the number of characters sent.  On error, -1 is returned, and errno is set appropriately.
 */
ssize_t netdpsock_sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);


/**
 *  send user data via socket. This is nonblocking function, so shall check EAGAIN.
 *
 * @param       
 * @param 
 *
 * @return  
 *  On success, these calls return the number of characters sent.  On error, -1 is returned, and errno is set appropriately.
 */
ssize_t netdpsock_send(int sockfd, const void *buf, size_t len, int flags);

/**
 * receive user data from socket. This function is designed as nonblocking function, so shall not set socket as nonblocking.
 *
 * @param       
 * @param 
 *
 * @return  
 *  These calls return the number of bytes received, or -1 if an error occurred.  In the event of an error, errno is set to indicate the error.  
 *  The return value will be 0 when the peer has performed an orderly shutdown.
 */      
ssize_t netdpsock_recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen);

/**
 * close a socket.
 *
 * @param       
 * @param 
 *
 * @return  
 * Returns zero on success.  On error, -1 is returned, and errno is set appropriately.
 */
int netdpsock_close(int fd);


/**
 * create a epoll socket.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
 int netdpsock_epoll_create(int size);

/**
 * update epoll socket event.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
 int netdpsock_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

/**
 * Waiting epoll socket event. Only support Edge  Triggered,  and ignore timeout, wait indefinitely if no event.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
 int netdpsock_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);


#endif /* __NETDP_SOCKET_INTF_H__ */
