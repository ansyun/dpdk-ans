/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __NETDP_SOCKET_MSG_H__
#define __NETDP_SOCKET_MSG_H__


#ifdef __cplusplus
extern "C" {
#endif

/* For the netconn API, these values are use as a bitmask! */
#define NETCONN_SHUT_RD   1
#define NETCONN_SHUT_WR   2
#define NETCONN_SHUT_RDWR (NETCONN_SHUT_RD | NETCONN_SHUT_WR)

/* IP addresses and port numbers are expected to be in
 * the same byte order as in the corresponding pcb.
 */
/** This struct includes everything that is necessary to execute a function
    for a netconn in another thread context (mainly used to process netconns
    in the tcpip_thread context to be thread safe). */
struct api_msg_msg {
  /** The netconn which to process - always needed: it includes the semaphore
      which is used to block the application thread until the function finished. */
  struct netconn *conn;
  /** The return value of the function executed in tcpip_thread. */
  int err;
  /** Depending on the executed function, one of these union members is used */
  union {
    /** used for do_send */
    struct {
      struct rte_mbuf *m, *ptr;
      uint32_t addr;
      uint16_t port;
    }b;

    /** used for do_newconn */
    struct {
      uint8_t proto;
    } n;
    /** used for do_bind and do_connect */
    struct {
      uint32_t *ipaddr;
      uint16_t port;
    } bc;
    /** used for do_getaddr */
    struct {
      uint32_t *ipaddr;
      uint16_t *port;
      uint8_t local;
    } ad;
    /** used for do_write */
    struct {
      const void *dataptr;
      size_t len;
      uint8_t apiflags;
    } w;
    /** used for do_recv */
    struct {
      uint32_t len;
    } r;
    /** used for do_close (/shutdown) */
    struct {
      uint8_t shut;
    } sd;
#if TCP_LISTEN_BACKLOG
    struct {
      uint8_t backlog;
    } lb;
#endif /* TCP_LISTEN_BACKLOG */
  } msg;
};

/** This struct contains a function to execute in another thread context and
    a struct api_msg_msg that serves as an argument for this function.
    This is passed to tcpip_apimsg to execute functions in tcpip_thread context. */
struct api_msg {
  /** function to execute in tcpip_thread context */
  void (* function)(struct api_msg_msg *msg);
  /** arguments for this function */
  struct api_msg_msg msg;
};


void netdp_do_newconn ( struct api_msg_msg *msg);
void netdp_do_delconn( struct api_msg_msg *msg);
void netdp_do_bind( struct api_msg_msg *msg);
void netdp_do_connect( struct api_msg_msg *msg);
void netdp_do_disconnect( struct api_msg_msg *msg);
void do_listen          ( struct api_msg_msg *msg);
void netdp_do_send( struct api_msg_msg *msg);
void do_recv            ( struct api_msg_msg *msg);
void do_write           ( struct api_msg_msg *msg);
void netdp_do_getaddr( struct api_msg_msg *msg);
void netdp_do_close( struct api_msg_msg *msg);


#ifdef __cplusplus
}
#endif

#endif /* __NETDP_SOCKET_MSG_H__ */
