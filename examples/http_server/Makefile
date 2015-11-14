#   BSD LICENSE
#
#   Copyright(c) 2010-2014 Netdp Corporation. All rights reserved.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

ifeq ($(RTE_ODP),)
$(error "Please define RTE_ODP environment variable")
endif

ifeq ($(RTE_TARGET),)
$(error "Please define RTE_TARGET environment variable")
endif

CC = gcc
RM = rm -f

ODP_PATH = $(RTE_ODP)

CFLAGS += -I$(ODP_PATH)/librte_netdp/include \
          -I$(ODP_PATH)/librte_netdpsock/include

LDLIBS += $(ODP_PATH)/librte_netdpsock/librte_netdpsock.a \
          $(RTE_SDK)/$(RTE_TARGET)/lib/librte_mbuf.a \
          $(RTE_SDK)/$(RTE_TARGET)/lib/librte_eal.a \
          $(RTE_SDK)/$(RTE_TARGET)/lib/librte_mempool.a \
          $(RTE_SDK)/$(RTE_TARGET)/lib/librte_ring.a \
          -lrt -pthread -ldl

OBJS = http_server.o 
TARGET = http_server

$(TARGET):$(OBJS)                      
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS) $(LDLIBS)
        
$(OBJS):%.o:%.c          
	$(CC) -c $(CFLAGS) $< -o $@        
clean:                              
	-$(RM) $(TARGET) $(OBJS)

