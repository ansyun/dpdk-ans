#   BSD LICENSE
#
#   Copyright(c) 2015-2017 Ansyun <anssupport@163.com>. All rights reserved.
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

ifeq ($(RTE_ANS),)
$(error "Please define RTE_ANS environment variable")
endif

ifeq ($(RTE_TARGET),)
$(error "Please define RTE_TARGET environment variable")
endif

CC = gcc
RM = rm -f

CFLAGS += -O3 -DLOAD_CERTIFICATE\
	  -I$(RTE_ANS)/librte_ans/include \
          -I$(RTE_ANS)/librte_anssock/include 

LDLIBS += $(RTE_ANS)/librte_anssock/librte_anssock.a \
          -L$(RTE_SDK)/$(RTE_TARGET)/lib \
          -Wl,--whole-archive -Wl,-lrte_mbuf -Wl,-lrte_mempool_ring -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,--no-whole-archive -Wl,-export-dynamic \
          -lrt -pthread -ldl -lnuma \
          -lnsl -lresolv -rdynamic -lssl -lcrypto
         
          
          

OBJS = https_server.o ans_module.o
TARGET = https_server

$(TARGET):$(OBJS)                      
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS) $(LDLIBS)
        
$(OBJS):%.o:%.c          
	$(CC) -c $(CFLAGS) $< -o $@        
clean:                              
	-$(RM) $(TARGET) $(OBJS)

