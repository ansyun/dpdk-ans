/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2015-2017 Ansyun <anssupport@163.com>. All rights reserved.
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
 *     * Neither the name of Ansyun <anssupport@163.com> nor the names of its
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
 *
 *   Author: JiaKai (jiakai1000@gmail.com) and Bluestar (anssupport@163.com)
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
#include <assert.h>
 #include <unistd.h>
 
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
#include "anssock_intf.h"
#include "ans_errno.h"

#define _GNU_SOURCE
#define __USE_GNU

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>


/* 
 *  opendp socket fd large than linux "ulimit -n " value
 *  
*/
#define ANS_FD_BASE 2000

/* 1: redis socket will go through opendp stack, 0: go through linux stack */
int ans_sock_enable = 1; 

int ansfd_debug_flag = 0;

#define ANS_FD_DEBUG( fmt, args...)  \
  do {                                                           \
    if(ansfd_debug_flag == 1)   \
        printf(fmt ,  ## args);  \
  } while (0)
  
static int inited = 0;

static int (*real_close)(int);
static int (*real_socket)(int, int, int);
static int (*real_bind)(int, const struct sockaddr*, socklen_t);
static int (*real_connect)(int, const struct sockaddr*, socklen_t);
static int (*real_listen)(int, int);
static int (*real_accept)(int, struct sockaddr *, socklen_t *);
static int (*real_accept4)(int, struct sockaddr *, socklen_t *, int);
static ssize_t (*real_recv)(int, void *, size_t, int);
static ssize_t (*real_send)(int, const void *, size_t, int);
static int (*real_shutdown)(int, int);

static ssize_t (*real_writev)(int, const struct iovec *, int);
static ssize_t (*real_write)(int, const void *, size_t );
static ssize_t (*real_read)(int, void *, size_t );
static ssize_t (*real_readv)(int, const struct iovec *, int);

static int (*real_setsockopt)(int, int, int, const void *, socklen_t);
static int (*real_getpeername)(int , struct sockaddr *, socklen_t *);
static int (*real_getsockname)(int , struct sockaddr *, socklen_t *);

static int (*real_ioctl)(int, int, void *);

static int (*real_epoll_create)(int);
static int (*real_epoll_ctl)(int, int, int, struct epoll_event *);
static int (*real_epoll_wait)(int, struct epoll_event *, int, int);

/**
 * @param 
 *
 * @return  
 *
 */
void ans_mod_init()
{
    int rc;
       
#define INIT_FUNCTION(func) \
        real_##func = dlsym(RTLD_NEXT, #func); \
        assert(real_##func)

    INIT_FUNCTION(socket);
    INIT_FUNCTION(bind);
    INIT_FUNCTION(connect);
    INIT_FUNCTION(close);
    INIT_FUNCTION(listen);
    INIT_FUNCTION(accept);
    INIT_FUNCTION(accept4);
    INIT_FUNCTION(recv);
    INIT_FUNCTION(send);
    INIT_FUNCTION(shutdown);
    
    INIT_FUNCTION(writev);
    INIT_FUNCTION(write);
    INIT_FUNCTION(read);
    INIT_FUNCTION(readv);

    INIT_FUNCTION(setsockopt);
    INIT_FUNCTION(getpeername);
    INIT_FUNCTION(getsockname);

    INIT_FUNCTION(ioctl);

    INIT_FUNCTION(epoll_create);
    INIT_FUNCTION(epoll_ctl);
    INIT_FUNCTION(epoll_wait);
   /*    
    INIT_FUNCTION();
    INIT_FUNCTION();
    INIT_FUNCTION();
    */

#undef INIT_FUNCTION

    if(ans_sock_enable != 1)
    {
        printf("ans socket is disable \n");
        return;
    }

    rc = anssock_init(NULL);
    assert(0 == rc);

    inited = 1;
}

/**
 * @param 
 *
 * @return  
 *
 */
int socket(int domain, int type, int protocol)
{
    int rc;

     ANS_FD_DEBUG("socket create start , domain %d, type %d \n", domain, type);    
   
    if ((inited == 0) ||  (AF_INET != domain) || (SOCK_STREAM != type && SOCK_DGRAM != type))
    {
        rc = real_socket(domain, type, protocol);
        ANS_FD_DEBUG("linux socket fd %d \n", rc);    

        return rc;
    }

    assert(inited);
    rc = anssock_socket(domain, type, protocol);
    
    if(rc > 0)
        rc += ANS_FD_BASE;
    
    ANS_FD_DEBUG("ans socket fd %d \n", rc);    
    return rc;
}

/**
 * @param 
 *
 * @return  
 *
 */
int socketpair (__attribute__((unused)) int __domain, __attribute__((unused)) int __type,__attribute__((unused))  int __protocol, __attribute__((unused)) int __fds[2])
{
    return -1;
}

/**
 * @param 
 *
 * @return  
 *
 */
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    struct sockaddr_in *in_addr; 
    in_addr = (struct sockaddr_in *)addr;

    ANS_FD_DEBUG("bind ip: %x , port %d, family:%d \n", in_addr->sin_addr.s_addr, ntohs(in_addr->sin_port), in_addr->sin_family);

    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;
        return anssock_bind(sockfd, addr, addrlen);
    } 
    else 
    {
        return real_bind(sockfd, addr, addrlen);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{

    ANS_FD_DEBUG("fd(%d) start to connect \n", sockfd);

    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;
        return anssock_connect(sockfd, addr, addrlen);
    } 
    else 
    {
        return real_connect(sockfd, addr, addrlen);
    }
    
}

/**
 * @param 
 *
 * @return  
 *
 */
ssize_t send (int sockfd, const void *buf, size_t len, int flags)
{
    ssize_t n;
    
    ANS_FD_DEBUG("send data fd %d , len %lu \n", sockfd, len);

    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;
        ANS_FD_DEBUG("ans send data fd %d , len %lu \n", sockfd, len);

        n = anssock_send(sockfd, buf, len, flags);  
        
        ANS_FD_DEBUG("ans send: fd %d , len %lu, return value:%ld, errno:%d, strerror = %s \n", sockfd, len, n, errno, strerror(errno));

        return n;

    }
    else 
    {
        ANS_FD_DEBUG("linux send data fd %d , len %lu \n", sockfd, len);

        return real_send(sockfd, buf, len, flags);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t n;

//    ANS_FD_DEBUG("write data fd %d , len %lu \n", fd, count);

    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;

        n = anssock_write(fd, buf, count);
        
        ANS_FD_DEBUG("ans write: fd %d , len %lu, return value:%ld, errno:%d, strerror = %s \n", fd, count, n, errno, strerror(errno));

        return n;

    }
    else 
    {

        n = real_write(fd, buf, count);
        
        ANS_FD_DEBUG("linux write: fd %d , len %lu, return value:%ld, errno:%d, strerror = %s \n", fd, count, n, errno, strerror(errno));
     
        return n;
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    ssize_t rc;
    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;

        rc = anssock_recv(sockfd, buf, len, flags);
        if (-1 == rc && ANS_EAGAIN == errno)
        {
            errno = EAGAIN;
        }

        ANS_FD_DEBUG("ans fd %d recv data len %ld \n", sockfd, rc);

        return rc;
    } 
    else
    {
        rc = real_recv(sockfd, buf, len, flags);

        ANS_FD_DEBUG("linux fd %d recv data len %ld \n", sockfd, rc);
        
        return rc;
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
ssize_t read(int fd, void *buf, size_t count)
{
    ssize_t rc;
    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;

        rc = anssock_read(fd, buf, count);

        ANS_FD_DEBUG("ans fd %d read data len %ld, %d \n", fd, rc, errno);
        
        return rc;
    } 
    else
    {
        rc =real_read(fd, buf, count);
   //     ANS_FD_DEBUG("linux fd %d read data len %ld  \n", fd, rc);

        return rc;
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
ssize_t sendto(__attribute__((unused)) int sockfd, __attribute__((unused))const void *buf, __attribute__((unused))size_t len, __attribute__((unused))int flags,
                      __attribute__((unused))const struct sockaddr *dest_addr, __attribute__((unused))socklen_t addrlen)
{
    return -1;
}

/**
 * @param 
 *
 * @return  
 *
 */
 ssize_t recvfrom(__attribute__((unused))int sockfd, __attribute__((unused))void *buf, __attribute__((unused))size_t len, __attribute__((unused))int flags,
                        __attribute__((unused))struct sockaddr *src_addr, __attribute__((unused))socklen_t *addrlen)
{
    return -1;
}

/**
 * @param 
 *
 * @return  
 *
 */
 int getsockopt(__attribute__((unused))int sockfd, __attribute__((unused))int level, __attribute__((unused))int optname, 
        __attribute__((unused))void *optval, __attribute__((unused))socklen_t *optlen)
{
    return -1;
}

/**
 * @param 
 *
 * @return  
 *
 */
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;

        return anssock_setsockopt(sockfd, level, optname, optval, optlen);
    } 
    else 
    {
        return real_setsockopt(sockfd, level, optname, optval, optlen);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;

        return anssock_getpeername(sockfd, addr, addrlen);
    } 
    else 
    {
        return real_getpeername(sockfd, addr, addrlen);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;

        return anssock_getsockname(sockfd, addr, addrlen);
    } 
    else 
    {
        return real_getsockname(sockfd, addr, addrlen);
    }
}


/**
 * @param 
 *
 * @return  
 *
 */
int listen(int sockfd, int backlog)
{
    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;
        
        ANS_FD_DEBUG("ans listen fd %d, pid %d \n", sockfd, getpid());
        
        return anssock_listen(sockfd, backlog);
    }
    else
    {
        ANS_FD_DEBUG("linux listen fd %d , pid %d \n", sockfd, getpid());
  
        return real_listen(sockfd, backlog);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int rc;

    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;

        rc = anssock_accept(sockfd, addr, addrlen);
        addr->sa_family = AF_INET;

        ANS_FD_DEBUG("ans accept fd %d \n", rc);
        if(rc > 0 )
            rc += ANS_FD_BASE;
    } 
    else 
    {
        rc = real_accept(sockfd, addr, addrlen);
        ANS_FD_DEBUG("linux accept fd %d \n", rc);

    }
    return rc;
}

/**
 * @param 
 *
 * @return  
 *
 */
int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
    int rc;

    if (sockfd > ANS_FD_BASE) 
    {
        sockfd -= ANS_FD_BASE;

        rc = anssock_accept(sockfd, addr, addrlen);
        addr->sa_family = AF_INET;
        
        ANS_FD_DEBUG("ans accep4t fd %d, errno %d \n", rc, errno);
        
        if(rc > 0 )
            rc += ANS_FD_BASE;

    } 
    else 
    {
        rc = real_accept4(sockfd, addr, addrlen, flags);
        ANS_FD_DEBUG("linux accept4 fd %d, errno %d \n", rc, errno);
    }
    return rc;
}

/**
 * @param 
 *
 * @return  
 *
 */
int shutdown (int fd, int how)
{
    ANS_FD_DEBUG("ans shutdown fd %d, how %d,  pid %d \n", fd, how, getpid());

    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;

        return anssock_shutdown(fd, how);;
    } 
    else
    {
        return real_shutdown(fd, how);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
int close(int fd)
{
    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;

       ANS_FD_DEBUG("ans close fd %d, pid %d \n", fd, getpid());

        return anssock_close(fd);
    }
    else
    {
     //   ANS_FD_DEBUG("linux close fd %d \n", fd);
     
        return real_close(fd);
    }
}

/**
 * @param 
 *
 * @return  
 *
 */
int epoll_create (int size)
{
    int rc;

    ANS_FD_DEBUG("epoll create  start, EPOLL_CTL_ADD %d ,EPOLL_CTL_DEL %d, EPOLLOUT 0x%x, EPOLLIN:0x%x \n", EPOLL_CTL_ADD, EPOLL_CTL_DEL, EPOLLOUT, EPOLLIN);

    if (inited == 1) 
    {
        rc = anssock_epoll_create(size);
        if(rc > 0)
            rc += ANS_FD_BASE;
        
         ANS_FD_DEBUG("ans epoll fd %d \n", rc);
      
    } 
    else 
    {
        rc = real_epoll_create(size);
        ANS_FD_DEBUG("linux epoll fd %d \n", rc);
    }
    return rc;
}

/**
 * @param 
 *
 * @return  
 *
 */
int epoll_create1 (__attribute__((unused))int __flags)
{
    return -1;
}

/**
 * @param 
 *
 * @return  
 *
 */
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    int rc;

    ANS_FD_DEBUG("epoll ctl  start, epfd %d ,op %d, fd %d, event:0x%x \n", epfd, op, fd, event->events);

    if (epfd > ANS_FD_BASE) 
    {
        if(fd <= ANS_FD_BASE)
        {
            printf("skip linux fd %d \n", fd);
            return 0;
        }
        epfd -= ANS_FD_BASE;
        fd -= ANS_FD_BASE;

        rc = anssock_epoll_ctl(epfd, op, fd, event);
    }
    else 
    {
        if(fd > ANS_FD_BASE)
        {
            printf("skip ans fd %d \n", fd);
            return 0;
        }

        rc = real_epoll_ctl(epfd, op, fd, event);
    }
    return rc;
}

 int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    int rc;

    if (epfd > ANS_FD_BASE) 
    {
        epfd -= ANS_FD_BASE;

     //   ANS_FD_DEBUG("ans epoll_wait: fd %d maxevents %d , timeout %d \n", epfd, maxevents, timeout);       
 
        rc = anssock_epoll_wait(epfd, events, maxevents, timeout);
    }
    else
    {
      //  ANS_FD_DEBUG("linux epoll_wait: fd %d maxevents %d , timeout %d \n", epfd, maxevents, timeout);       

        rc = real_epoll_wait(epfd, events, maxevents, timeout);
    }
    return rc;
}

/**
 * @param 
 *
 * @return  
 *
 */
int epoll_pwait (__attribute__((unused))int __epfd, __attribute__((unused))struct epoll_event *__events, __attribute__((unused))int __maxevents,
    __attribute__((unused))int __timeout, __attribute__((unused))const __sigset_t *__ss)
{
    return -1;
}


/**
 * @param 
 *
 * @return  
 *
 */
int ioctl(int fd, int request, void *p)
{
    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;

        //return anssock_ioctl(fd, request, p);
        return 0;
    } 
    else
    {
        return real_ioctl(fd, request, p);
    }
}


/**
 * @param 
 *
 * @return  
 *
 */
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t rc;

    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;
        
        ANS_FD_DEBUG("ans writev data fd %d , iovcnt %d \n", fd, iovcnt);
        rc = anssock_writev(fd, iov, iovcnt);
    }
    else 
    {
     //   ANS_FD_DEBUG("linux writev data fd %d , len %d \n", fd, iovcnt);

        rc = real_writev(fd, iov, iovcnt);
    }
    return rc;
}

/**
 * @param 
 *
 * @return  
 *
 */
 ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t rc;

    if (fd > ANS_FD_BASE) 
    {
        fd -= ANS_FD_BASE;

        ANS_FD_DEBUG("ans fd %d readv with iovcnt %d \n", fd, iovcnt);
        rc =anssock_readv(fd, iov, iovcnt);
        return rc;
    } 
    else
    {
        rc =real_readv(fd, iov, iovcnt);

        return rc;
    }


}

    

