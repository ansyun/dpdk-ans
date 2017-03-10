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

/*
* This program is used to test ans user space tcp stack
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/times.h>

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <termios.h>
#include <sys/epoll.h>

#include <openssl/ssl.h>  
#include <openssl/err.h>  


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


#define MAX_FLOW_NUM 200000
#define BUFFER_SIZE 5000
#define MAX_EVENTS 512
#define MAX_CPUS 8
#define MAXBUF 1024 


char *https_200 = "HTTP/1.0 200 OK\r\n"
                 "Cache-Control: no-cache\r\n"
                 "Connection: close\r\n"
                 "Content-Type: text/html\r\n"
                 "\r\n"
                 "<html><body><h1>200 OK</h1>\nEverything is fine.\n</body></html>\n";
 
extern void ans_mod_init();

int dpdk_https_event(struct epoll_event ev,SSL *ssl)
{
    char recv_buf[BUFFER_SIZE];       
    int len; 
    int send_len = 0;     
    SSL_CTX *ctx;  
    char buf[MAXBUF + 1];  
  
    if (ev.events&EPOLLIN)
    {        
        /*Start dealing with the data on the each new connection */
        bzero(buf, MAXBUF + 1);  
        /* Receive the client's message*/  
        len = SSL_read(ssl, buf, MAXBUF);  
        if (len > 0)
        {
            printf("rev success :'%s' datelen is %d\n",  
                   buf, len);  
        }    
        else
        {
            printf("rev fail: errornum is %d ,errorinfo is '%s'\n",  
                     errno, strerror(errno));
            //goto finish;
        }
              
        /* Send messages to the client */  
        len = SSL_write(ssl, https_200, strlen(https_200));  
        if (len <= 0) 
        {  
            printf("message'%s'send fail errornum is %d£¬errorinfo is '%s'\n",  
                 buf, errno, strerror(errno));  
            goto finish;  
        } 
        else 
        {
            printf("smessage'%s'send success datelen is %d\n",  
                   buf, len);  
        }      
        /* The data on the processing each new connection sending and receiving ends*/  
        finish:  
        /*Close the SSL connection */  
        SSL_shutdown(ssl);  
        /* Free SSL */  
        SSL_free(ssl);  
        /* close socket */  
        close(ev.data.fd); 
      
    }
    
    return 0;
}

/*----------------------------------------------------------------------------*/
int RunHttpsThread(void)
{
    int sockfd, new_fd ,ret,client_sockfd,client_sslsockfd;  
    socklen_t len;  
    int epoll_fd;
    int nfds;
    struct sockaddr_in my_addr, their_addr;  
    struct sockaddr_in remote_addr;
    struct epoll_event ev;  
    struct epoll_event events[MAX_EVENTS];  
    unsigned int myport, lisnum;  
    char buf[MAXBUF + 1];  
    SSL_CTX *ctx;  
    int opt_val = 1;
    int sin_size,r,err;    
    SSL *ssl; 
    BIO *bio_err = NULL;
    int result =0;
    int do_sslaccept = 0; 
     
    ans_mod_init();
  
    myport = 7838;   
    lisnum = 2048;  
  
    /* LIBSSL  init  */  
    SSL_library_init();  
    /* Load all SSL algorithm*/  
    OpenSSL_add_all_algorithms();  
    /*Load all SSL error*/  
    SSL_load_error_strings();  
    /* Produce a SSL CTX in SSL V2 and V3 standards compliant way */  
    ctx = SSL_CTX_new(SSLv23_server_method());  
   
    if (ctx == NULL) 
    {  
        exit(1);  
    } 
    /* Require calibration certificate  */
    /*Can choose not to calibration*/
    #ifdef LOAD_CERTIFICATE
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);  
    #endif
    /*Load the CA's certificate*/
    if(!SSL_CTX_load_verify_locations(ctx, "cacert.cer", NULL))
    { 
        printf("loda caverify fail\n");
    }
 
    /*Loading their own certificates  */
    if (SSL_CTX_use_certificate_file(ctx, "server.cer", SSL_FILETYPE_PEM) <= 0)
    {
        printf("loda certificate fail\n");
    }
 
    /*Loading their own key  */  
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
    {
        printf("loda PrivateKey fail\n");
    }
 
    /*check own key*/
    if (!SSL_CTX_check_private_key(ctx))
    {
        printf("check PrivateKey fail\n");
    }
    
    /* create  socket */  
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {  
        perror("socket");  
        exit(1);  
    } 
    else 
    {
        printf("socket created\n"); 
    }
          
    bzero(&my_addr, sizeof(my_addr));  
    my_addr.sin_family = AF_INET;  
    my_addr.sin_port = htons(myport);   
    my_addr.sin_addr.s_addr = INADDR_ANY;  
 
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))== -1) 
    {  
        perror("bind");  
        exit(1);  
    } 
    else 
    {
        printf("binded\n"); 
    }

    if (listen(sockfd, lisnum) == -1) 
    {  
        perror("listen");  
        exit(1);  
    } 
    else 
    {
        printf("begin listen\n");  
    }

    epoll_fd = epoll_create(MAX_EVENTS);  
    if(epoll_fd == -1)  
    {  
        printf("epoll_create failed \n"); 
        close(sockfd);
        return 1;     
    }  

    ev.events = EPOLLIN;  
    ev.data.fd = sockfd;  
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sockfd,&ev)==-1)  
    {  
        printf("epll_ctl:sockfd register failed");  
        close(sockfd);
        close(epoll_fd);
        return 1;     
    }  
 
    while(1)  
    {  
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);  
        if(nfds == -1)  
        {  
            printf("start epoll_wait failed \n");  
            close(sockfd);
            close(epoll_fd);
            return 1;     
        }  
        else if(nfds == 0)
        {
            printf("epoll timeout \n");
            continue;
        }
        
        int i;  
        for(i = 0; i < nfds; i++)  
        {  
            if(events[i].data.fd == sockfd)  
            {  
                if((client_sockfd = accept(sockfd, (struct sockaddr *)&remote_addr,&sin_size)) < 0)  
                {     
                    printf("accept client_sockfd failed \n");     
                    close(sockfd);
                    close(epoll_fd);
                    return 1;     
                }  
                ev.events = EPOLLIN;  
                ev.data.fd = client_sockfd;  
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD,client_sockfd,&ev)==-1)  
                {  
                    printf("epoll_ctl:client_sockfd register failed \n");  
                    close(sockfd);
                    close(epoll_fd);
                    return 1;     
                }  
                /*create a new SSL based on CTX*/  
                ssl = SSL_new(ctx);  
                /* join the Socket  to the SSL */  
                result = SSL_set_fd(ssl, client_sockfd);  
                do_sslaccept = 0;
              
            }  
            else if(events[i].data.fd == client_sockfd)
            {
                if(do_sslaccept)
                {
                    dpdk_https_event(events[i],ssl);
                }
                else
                {
                    /* To establish the SSL connection*/        
                    r = SSL_accept(ssl);
                    if(r == 1)
                    {
                        printf("accept  sslclient_sockfd success \n");
                        do_sslaccept = 1;
                        break;
                    }
                    err = SSL_get_error(ssl,r);
                    if(err == SSL_ERROR_NONE||
                       err == SSL_ERROR_WANT_WRITE||
                       err == SSL_ERROR_WANT_READ)
                    {
                        continue;
                    }
                    else 
                    {
                        printf("accept  sslclient_sockfd fail err is %d\n",err);
                        unsigned long l = 0;        
                        if (l = ERR_get_error())
                        {   
                            printf("ERROR:%s\n", ERR_error_string(l, NULL));
                        }

                    }
                }

            }
            else  
            {  
                continue;
            }  
        }  
    }
    
  
    /* Close socket */  
    close(sockfd);  
    /*Free CTX */  
    SSL_CTX_free(ctx);  
    return 0;  

}
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[] )
{
    int core =0;

    if(argc >= 2)
    {
        core = atoi(argv[1]);
        printf("affinity to core %d \n", core);
    }
    else
    {
        printf("affinity to 0 core by default \n");
    }
       
    /*initialize thread bind cpu*/
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET((unsigned)core, &cpus);
    sched_setaffinity(0, sizeof(cpus), &cpus);  

    RunHttpsThread();
      

}


