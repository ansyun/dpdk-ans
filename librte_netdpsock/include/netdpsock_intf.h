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
 * @param  file_prefix 
 *  Prefix for hugepage filenames, shall be same as opendp startup parameter(--file-prefix).
 *  If input is NULL, the default file-prefix is "rte"
 * @return  
 *
 */
int netdpsock_init(char *file_prefix);

/**
 *  Creates  an endpoint for communication and returns a descriptor.
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
 * Binds a local IP address.
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
 * Connect to remote IP address.
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
 *   If errno is EAGAIN, no buffer for sending data.
 */
ssize_t netdpsock_sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);


/**
 *  Send user data via socket. This is nonblocking function, so shall check EAGAIN.
 *
 * @param       
 * @param 
 *
 * @return  
 *  On success, these calls return the number of characters sent.  On error, -1 is returned, and errno is set appropriately.
 *   If errno is EAGAIN, no buffer for sending data.
 */
ssize_t netdpsock_send(int sockfd, const void *buf, size_t len, int flags);

/**
 * Receive user data from socket. This function is designed as nonblocking function, so shall not set socket as nonblocking and work with epoll.
 *
 * @param       
 * @param 
 *
 * @return  
 *  These calls return the number of bytes received, or -1 if an error occurred.  In the event of an error, errno is set to indicate the error.  
 *  If errno is EAGAIN, no data are present to be received.
 */      
ssize_t netdpsock_recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen);


/**
 * Receive user data from socket. This function is designed as nonblocking function, so shall not set socket as nonblocking.
 *
 * @param       
 * @param 
 *
 * @return  
 *  These calls return the number of bytes received, or -1 if an error occurred.  In the event of an error, errno is set to indicate the error.  
 *  If errno is EAGAIN, no data are present to be received.
 */ 
ssize_t netdpsock_recv(int sockfd, void *buf, size_t len, int flags);


/**
 * Listen for connections on a socket
 *
 * @param       
 * @param  backlog 
 * The  backlog  argument defines the maximum length to which the queue of pending connection. default value is 2048
 * @return  
 * On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
int netdpsock_listen(int sockfd, int backlog);

/**
 * Accept a new socket. This function is designed as nonblocking function, so shall not set socket as nonblocking and work with epoll.
 *
 * @param       
 * @param  
 *
 * @return  
 * On success, these system calls return a nonnegative integer that is a descriptor for the accepted socket.  On error, -1 is returned, and errno is set appropriately.
 * If errno is EAGAIN, no connections are present to be accepted.
 */
int netdpsock_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * Close a socket.
 *
 * @param       
 * @param 
 *
 * @return  
 * Returns zero on success.  On error, -1 is returned, and errno is set appropriately.
 */
int netdpsock_close(int fd);


/**
 * Create a epoll socket.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
 int netdpsock_epoll_create(int size);

/**
 * Update epoll socket event.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
 int netdpsock_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

/**
 * Waiting epoll socket event. Only support Edge Triggered.
 *
 * @param maxevents
 * max events shall less than 2048
 * @param timeout
 * The timeout argument specifies the minimum number of milliseconds that netdpsock_epoll_wait() will block. Specifying a timeout of -1 causes epoll_wait() to block indefinitely, 
 * while specifying a timeout equal to zero cause epoll_wait() to return immediately, even if no events are available.
 *
 * @return  
 * When successful, netdpsock_epoll_wait() returns the number of file descriptors ready for read, or zero if no file descriptor became ready during the requested timeout milliseconds.   
 * When  an  error  occurs,  netdpsock_epoll_wait() returns -1 and errno is set appropriately. *
 */
 int netdpsock_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

/**
 * This is dummy API, always return 0, it is still not implemented.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
int netdpsock_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

/**
 * This is dummy API, always return 0, it is still not implemented.
 *
 * @param       
 * @param 
 *
 * @return  
 *
 */
int netdpsock_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

/**
 *  Returns  the address of the peer connected to the socket sockfd.
 *
 * @param       
 * @param 
 *
 * @return  
 * On  success,  zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
int netdpsock_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


/**
 * Returns the current address to which the socket sockfd is bound.
 *
 * @param       
 * @param 
 *
 * @return  
 * On  success,  zero is returned.  On error, -1 is returned, and errno is set appropriately.
 */
 int netdpsock_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * Enable netdpsock API debug log
 *
 * @param flag  
 * 1: enable debug log, 0: disable log
 * @return  
 * 
 */
 void netdpsock_enable_log(int flag);

#endif /* __NETDP_SOCKET_INTF_H__ */
