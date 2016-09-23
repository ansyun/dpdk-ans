/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 ANS Corporation. All rights reserved.
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
 *     * Neither the name of ANS Corporation nor the names of its
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


#ifndef _ANSCLI_CONF_H_
#define _ANSCLI_CONF_H_

/* not defined under linux */
#define NIPQUAD_FMT "%u.%u.%u.%u"
#define NIPQUAD(addr)       \
  (unsigned)((unsigned char *)&addr)[0],  \
  (unsigned)((unsigned char *)&addr)[1],  \
  (unsigned)((unsigned char *)&addr)[2],  \
  (unsigned)((unsigned char *)&addr)[3]

#define ANSCLI_RECV_MSG         1
#define ANSCLI_NONRECV_MSG  2



/* help */
struct anscli_help_result
{
  cmdline_fixed_string_t help;
};

/* quit */
struct anscli_quit_result
{
  cmdline_fixed_string_t quit;
};

/* ip addr/route add/del/show */
struct anscli_iproute_result {
  cmdline_fixed_string_t name;  /* ip */
  cmdline_fixed_string_t type;   /* addr/route */
  cmdline_fixed_string_t action; /* add/del/show */
  cmdline_ipaddr_t ipaddr;       /* ip address */
  cmdline_fixed_string_t dev;   /* device */
  cmdline_fixed_string_t iface;   /* interface */

  cmdline_fixed_string_t via;  /* via */
  cmdline_ipaddr_t destip;       /* dest network */
  cmdline_ipaddr_t gateway;       /* nexthot*/

};

/* set log level */
struct anscli_log_result {
  cmdline_fixed_string_t name;  /* log */
  cmdline_fixed_string_t type;   /* level */
  cmdline_fixed_string_t action; /* set */
  cmdline_fixed_string_t level; /* err/info */
};

extern cmdline_parse_ctx_t ip_main_ctx[];


#endif /* _ANSCLI_CONF_H_ */
