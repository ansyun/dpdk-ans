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
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <termios.h>
#ifndef __linux__
  #ifdef __FreeBSD__
    #include <sys/socket.h>
  #else
    #include <net/socket.h>
  #endif
#endif

#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_tailq.h>
#include <rte_eal.h>
#include <rte_debug.h>
#include <rte_log.h>

#include "anscli_main.h"
#include "anscli_intf.h"


/**********************************************************************
*@description:
*  display usage
*
*
**********************************************************************/
static void anscli_print_usage(void)
{
    printf ("anscli has four input format: \n"
    "  ./anscli  - enter cli window with default file-prefix \n"
    "  ./anscli --file-prefix=NAME - enter cli window with specified file-prefix,  eg: ./anscli --file-prefix=testprefix \n"
    "  ./anscli \"CMD\" - enter CMD with default file-prefix, eg: ./anscli \"help\" \n"
    "  ./anscli --file-prefix=NAME \"CMD\" - enter CMD with specified file-prefix, eg: ./anscli --file-prefix=testprefix \"help\" \n"
    "  Notes: the file-prefix shall same as ans's file-prefix \n");
    
    return;
}

/**********************************************************************
*@description:
*  anscli main
*
*
**********************************************************************/
int main(int argc, char **argv)
{
    int ret;
    char *cmd;
    short has_cmd = 0;
    int     param_num = 8;
    char *param[9] = {"anscli",
                               "-c",
                               "1",
                               "-n",
                               "1",
                               "--no-pci",
                               "--socket-mem=1",
                               "--proc-type=secondary",
                               NULL};

    if(argc >= 2)
    {
        if(strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            anscli_print_usage();
            return 0;
        }

        if(strncmp(argv[1], "--file-prefix", strlen("--file-prefix")) == 0)
        {
            param[8] = argv[1];
            param_num++;

            if(argc > 2)
            {
                has_cmd = 1;
                cmd = argv[2];
            }
            else
            {
                has_cmd = 0;
            }
        }
        else
        {
            has_cmd = 1;
            cmd = argv[1];
        }

    }
    else
    {
        has_cmd = 0;
    }
    
    rte_log_set_global_level(RTE_LOG_ERR);
    ret = rte_eal_init(param_num, param);
    if (ret < 0)
        rte_panic("Cannot init EAL\n");
    
    if(has_cmd)
    {
        anscli_start(cmd);
    }
    else
    {
        anscli_start(NULL);
    }
    
    return 0;
}
