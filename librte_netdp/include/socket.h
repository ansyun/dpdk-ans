

#ifndef __SOCKET_H_
#define	__SOCKET_H_



#if defined(__KERNEL__) && defined(linux)
/* Nothing*/
#else
#include <sys/fcntl.h>   /* for FNONBIO and _FNONBLOCK */
#include <sys/time.h>
#endif /* __KERNEL__ */


#ifdef TCP_STATS
#include "netdp_tcp_if.h"
#endif /* TCP_STATS */


/*
 * Definitions related to sockets: types, address families, options.
 */

/*
 * Data types.
 */
#ifndef _SA_FAMILY_T_DECLARED
typedef uint8_t __netdp_so_sa_family_t;
typedef	__netdp_so_sa_family_t netdp_so_sa_family_t;
#define	_SA_FAMILY_T_DECLARED
#endif

#ifndef _SOCKLEN_T_DECLARED
typedef uint32_t __netdp_so_socklen_t;
typedef	__netdp_so_socklen_t netdp_so_socklen_t;
#define	_SOCKLEN_T_DECLARED
#endif


/*
 * Address families
 */
#define NETDP_SO_AF_UNSPEC       0               /* unspecified */
#define NETDP_SO_AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define NETDP_SO_AF_LINK         3               /* Link layer interface */
#define NETDP_SO_AF_ARP          4
#define NETDP_SO_AF_MAX          5


/*
 * Protocol families, same as address families for now.
 */
#define	NETDP_SO_PF_UNSPEC	NETDP_SO_AF_UNSPEC
#define	NETDP_SO_PF_INET		NETDP_SO_AF_INET
#define	NETDP_SO_PF_LINK		NETDP_SO_AF_LINK
#define	NETDP_SO_PF_ARP		NETDP_SO_AF_ARP
#define	NETDP_SO_PF_MAX		NETDP_SO_AF_MAX

/*
 * Types
 */
#define	NETDP_SO_SOCK_STREAM	1		/* stream socket */
#define	NETDP_SO_SOCK_DGRAM	2		/* datagram socket */
#define	NETDP_SO_SOCK_RAW	        3		/* raw-protocol interface */
#define	NETDP_SO_SOCK_RDM	        4		/* reliably-delivered message */
#define	NETDP_SO_SOCK_SEQPACKET	5		/* sequenced packet stream */


/*
 * Option flags per-socket.
 */

#define	NETDP_SO_SO_DEBUG	        0x0001		/* turn on debugging info recording - not currently supported */
#define	NETDP_SO_SO_ACCEPTCONN	0x0002		/* socket has had listen() - not currently supported */
#define	NETDP_SO_SO_REUSEADDR	0x0004		/* allow local address reuse - not currently supported */
#define	NETDP_SO_SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	NETDP_SO_SO_DONTROUTE	0x0010		/* just use interface addresses - not currently supported */
#define	NETDP_SO_SO_BROADCAST	0x0020		/* permit sending of broadcast msgs - not currently supported  */

#define	NETDP_SO_SO_USELOOPBACK	0x0040		/* bypass hardware when possible - not currently supported */

#define	NETDP_SO_SO_LINGER	0x0080		/* linger on close if data present */
#define	NETDP_SO_SO_OOBINLINE	0x0100		/* leave received OOB data in line */

#define	NETDP_SO_SO_REUSEPORT	0x0200		/* allow local address & port reuse - not currently supported */
#define	NETDP_SO_SO_TIMESTAMP	0x0400		/* timestamp received dgram traffic - not currently supported */
#define	NETDP_SO_SO_NOSIGPIPE	0x0800		/* no SIGPIPE from EPIPE - not currently supported  */
#define	NETDP_SO_SO_ACCEPTFILTER	0x1000		/* there is an accept filter - not currently supported */
#define	NETDP_SO_SO_BINTIME	0x2000		/* timestamp received dgram traffic - not currently supported */

#define NETDP_SO_SO_COMP          0x1000  /* unaccepted, complete connection - not currently supported */


/*
 * Additional options, not kept in so_options.
 */
#define	NETDP_SO_SO_SNDBUF	0x1001		/* send buffer size - not applicable */
#define	NETDP_SO_SO_RCVBUF	0x1002		/* receive buffer size - not applicable */
#define	NETDP_SO_SO_SNDLOWAT	0x1003		/* send low-water mark - not applicable */
#define	NETDP_SO_SO_RCVLOWAT	0x1004		/* receive low-water mark- not applicable */
#define	NETDP_SO_SO_SNDTIMEO	0x1005		/* send timeout */
#define	NETDP_SO_SO_RCVTIMEO	0x1006		/* receive timeout  */

#define	NETDP_SO_SO_ERROR	        0x1007		/* get error status and clear */
#define	NETDP_SO_SO_TYPE		0x1008		/* get socket type */
#define	NETDP_SO_SO_LABEL	        0x1009		/* socket's MAC label */
#define	NETDP_SO_SO_PEERLABEL	0x1010		/* socket's peer's MAC label */

#define	NETDP_SO_SO_SNDHIWAT	0x1011		/* send high water mark */
#define	NETDP_SO_SO_RCVHIWAT	0x1012		/* receive high water mark */

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	NETDP_SO_SOL_SOCKET	0xffff		/* options for socket level */
#define	NETDP_SO_SOL_TCP	        (6)            

/*
 * howto arguments for shutdown(2), specified by Posix.1g.
 */
#define NETDP_TCP_SHUT_RD  	0x0             /* shut down the reading side */
#define NETDP_TCP_SHUT_WR         0x1             /* shut down the writing side */
#define NETDP_TCP_SHUT_RDWR       0x02            /* shut down both sides */

/** Structure used for manipulating linger option.
 */
struct netdp_so_linger {

	/** option on/off */
	int	l_onoff;		

	/** linger time */
	int	l_linger;		
};



/** Structure used by kernel to store most addresses.
 */
struct netdp_so_sockaddr {

	/** Total length */
	unsigned char	sa_len;		

	/** Address family */
	netdp_so_sa_family_t	sa_family;	

	/** Actually longer; address value */
	char		sa_data[14];	
};


/** Structure containing information about a single connection obtained via
 *  accept_multi call.
 */
struct netdp_so_accepted_sock_info
{
	/** fd of the socket */
        int              sockfd;         

	/** Socket address */
        struct netdp_so_sockaddr  addr;           

	/* Address length */
        unsigned int     addrlen;        

	/* Error status for this socket */
        int              sock_errno;     
};

/** Structure to hold multiple accepted connections 
 */
struct netdp_so_sockaddr_multi {

	/** Number of connections accepted */
        int sock_entry_count;

	/** Information about each accepted connection */
        struct netdp_so_accepted_sock_info *sock_info;
};


/** iovec structure
 */
struct netdp_so_iovec_desc {
	/** The iov_base has the following format.
             - address (40 bits)
             - size (16 bits)
             - pool (3 bits)
             - back (4 bits)
             - invert bit (1 bit)
	 */
        uint64_t iov_base;

	/** Size of data */
        size_t   iov_len;
};

/** The netdp iovec structure.
 *  All the pointers of iovec_ptr_array have the following format:
 *    - address (40 bits)
 *    - size (16 bits)
 *    - pool (3 bits)
 *    - back (4 bits)
 *    - invert bit (1 bit)
 */
struct netdp_so_iovec {

	/** Number of iovecs pointed to by iovec_ptr_array */
        int             iovec_count;

	/** Pointer to a list of iovec structure */
        struct netdp_so_iovec_desc *  iovec_ptr_array;              

	/** 64-but buffer address format containing information about how 
	 *  the netdp_iovec structure was allocated.
	 */
        uint64_t        iovec_struct_alloc_info;

	/** 64-bit buffer address format containing information about how 
	 *  the ptr array was allocated.
	 */
        uint64_t        iovec_ptr_array_alloc_info;

	/** void pointer for application use 
	 */
	void            *iovec_usr_data;
};


/**
 * Maximum number of netdp_so_status entries that can be passed to API call netdp_so_poll.
 **/
#define MAX_POLL_SOCKETS  (128)

/**
 * Structure passed by application to find out if there is any activity
 * (i.e. read, write or exception) on a given socket_id.
 **/
typedef struct
{
    /**
     * 32-bit value specified by the application (different from BSD-style FD)
     */
    uint32_t socket_id;
    
    /**
     * Set by the TCP stack to notify application of any received data 
     */
    uint8_t  read_ready;

    /**
     * Set by the TCP stack to notify application of status of a send() call
     * (non-blocking sockets)
     */
    uint8_t  write_ready;

    /**
     * Set by the TCP stack to notify application of any exception 
     */
    uint8_t  exception_ready;

    /**
     * For future use
     */
    uint8_t  reserved;

} netdp_so_status;



/** Maximum queue length specifiable by listen.
 */
#define	NETDP_SO_SOMAXCONN	128


/** Message header for recvmsg and sendmsg calls.
 *  Used value-result for recvmsg, value only for sendmsg.
 */
struct netdp_so_msghdr {

	/** optional address */
	void		 *msg_name;		

	/** size of address */
	netdp_so_socklen_t  msg_namelen;		

	/** scatter/gather array */
	struct netdp_so_iovec_desc	*msg_iov;		

	/** # elements in msg_iov */
	int		 msg_iovlen;		

	/** ancillary data, see below */
	void		 *msg_control;		

	/** ancillary data buffer len */
	netdp_so_socklen_t msg_controllen;	

	/** flags on received message */
	int		 msg_flags;		
};

#define	NETDP_SO_MSG_OOB		0x1		/* process out-of-band data */
#define	NETDP_SO_MSG_PEEK	        0x2		/* peek at incoming message */
#define	NETDP_SO_MSG_DONTROUTE	0x4		/* send without using routing tables */
#define	NETDP_SO_MSG_EOR		0x8		/* data completes record */
#define	NETDP_SO_MSG_TRUNC	0x10		/* data discarded before delivery */
#define	NETDP_SO_MSG_CTRUNC	0x20		/* control data lost before delivery */
#define	NETDP_SO_MSG_WAITALL	0x40		/* wait for full request or error */
#define	NETDP_SO_MSG_DONTWAIT	0x80		/* enables non-blocking operation */

/** Header for ancillary data objects in msg_control buffer.
 *  Used for additional information with/about a datagram
 *  not expressible by flags.  The format is a sequence
 *  of message elements headed by cmsghdr structures.
 */
struct netdp_so_cmsghdr {
	/** data byte count, including hdr */
	netdp_so_socklen_t	cmsg_len;		

	/** originating protocol */
	int		cmsg_level;		

	/** protocol-specific type */
	int		cmsg_type;		
        /* followed by	u_char  cmsg_data[]; */
};



#ifndef __KERNEL__
#include <sys/cdefs.h>
#endif /* __KERNEL__ */

#ifndef DOXYGEN_SHOULD_SKIP_THIS 

#ifndef __KERNEL__
__BEGIN_DECLS
#endif /* __KERNEL__ */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


/** Create a new socket of type "type" in domain "domain", using
 *  protocol "protocol"
 *
 *  @param domain   - AF_INET, AF_INET6, AF_INET_OCTEON, AF_INET6_OCTEON
 *  @param type     - SOCK_STREAM, SOCK_DGRAM
 *  @param protocol - IPPROTO_TCP, IPPROTO_UDP
 *  @return Success: >0 (valid file descriptor for the new socket); Failure: -1
 *                   is returned and errno is set appropriately.
 *
 *  Note: AF_INET, SOCK_STREAM and IPPROTO_TCP are only supported currently.
 */
int	netdp_so_socket(int domain, int type, int protocol);

/** Await a connection on socket fd.
 *  When a connection arrives, open a new socket to communicate 
 *  with it, set "*addr" (which is "*addr_len" bytes long) to the 
 *  address of the connecting peer and "*addr_len" to the address's 
 *  actual length, and return the new socket's descriptor
 *
 *  @param fd        - listening socket descriptor
 *  @param addr      - pointer to the remote address of the newly accepted connection
 *  @param addr_len  - pointer to the len of the 'addr' in bytes
 *  @return            Success: 0; Failure: -1
 *  
 *  Note: ECONNABORTED error code is not supported.
 */
int	netdp_so_accept(int fd, struct netdp_so_sockaddr *addr, netdp_so_socklen_t *addr_len);

/** Await connections from multiple clients on a socket fd.
 *  When a connection arrives, open a new socket to communicate 
 *  with it, set "*addr" (which is "*addr_len" bytes long) to the 
 *  address of the connecting peer and "*addr_len" to the address's 
 *  actual length, and return the new socket's descriptor
 *
 *  @param listen_fd      - listening socket descriptor
 *  @param sockaddr_multi - pointer to the remote address of the newly accepted 
 *                          connection
 *  @param timeout        - upper bound on the amount of time elapsed before the
 *                          function returns. If it is NULL, this function will 
 *                          block until atleast one connection is established.
 *  @return                 Success: 0; Failure: -1
 *
 *  Note: timeout is not currently implemented.
 */
int     netdp_so_accept_multi (int listen_fd, struct netdp_so_sockaddr_multi *sockaddr_multi, int timeout);

/** Gives the socket fd a local address
 *
 *  @param fd   - a valid socket descriptor
 *  @param addr - local address to bind
 *  @param len  - length of address in bytes
 *  @return       Success: 0; Failure: -1
 *
 */
int	netdp_so_bind(int fd, const struct netdp_so_sockaddr *addr, netdp_so_socklen_t len);

/** Opens a connection to a remote peer.
 *  For connectionless socket types, just set the default address
 *  to send to and the only address from which to accept transmissions
 *
 *  @param fd   - a valid socket descriptor
 *  @param addr - remote address to connect to
 *  @param len  - length of address in bytes
 *  @return       Success: 0; Failure: -1
 *
 *  Note: Only blocking mode is supported.
 */
int	netdp_so_connect(int fd, const struct netdp_so_sockaddr *addr, netdp_so_socklen_t len);

/** Get the current value for socket fd's option "optname" at protocol 
 *  level "level" into "optval"
 *
 *  @param  fd      - a valid socket descriptor
 *  @param  level   - option level (NETDP_SO_SOL_SOCKET is supported currently)
 *  @param  optname - option name
 *  @param  optval  - pointer to the buffer which will hold the option vlaue
 *  @param  optlen  - pointer to int which will hold the option length in bytes
 *  @return Success: 0; Failure: -1
 *
 *  Note: Please refer to socket.h file for supported options.
 */
int	netdp_so_getsockopt(int fd, int level, int optname, void * optval, netdp_so_socklen_t *optlen);

/** Set the socket fd's option "optname" at protocol level "level"
 *  to "*optval" (which is "optlen" bytes long)
 *
 *  @param  fd      - a valid socket descriptor
 *  @param  level   - option level (NETDP_SO_SOL_SOCKET is supported currently)
 *  @param  optname - option name
 *  @param  optval  - pointer to the buffer containing the option related data
 *  @param  optlen  - option value length in bytes
 *
 *  @return Success: 0; Failure: -1
 *
 *  Note: Please refer to socket.h file for supported options.
 */
int	netdp_so_setsockopt(int fd, int level, int optname, const void *optval, netdp_so_socklen_t optlen);

/** Prepare to accept connections on a socket
 *
 *  @param fd      - a valid socket descriptor
 *  @param backlog - number of connection requests queued
 *                   before further requests are refused
 *  @return          Success: 0; Failure: -1
 *  
 */
int	netdp_so_listen(int fd, int backlog);

/** Receive the data using an opened socket
 *
 *  @param fd    - a valid socket descriptor
 *  @param msg   - pointer to a byte array which will hold the message
 *  @param len   - length of the received message in bytes 
 *  @param flags - additional receive flags
 *
 *  @return        Success: >0 (number of bytes received); Failure: -1
 */
int	netdp_so_recv(int fd, void *msg, size_t len, int flags);

/** Receive the data using an opened socket
 *
 *  @param fd      - a valid socket descriptor
 *  @param msg     - pointer to a byte array which will hold the message
 *  @param len     - length of the received message in bytes 
 *  @param flags   - additional receive flags
 *  @param from    - pointer to structure holding the foreign address and port
 *  @param fromlen - pointer to netdp_so_socklen_t holding the size of the address structure
 *
 *  @return        Success: >0 (number of bytes received); Failure: -1
 */
int netdp_so_recvfrom (int fd, void *msg, size_t len, int flags, 
                     struct netdp_so_sockaddr *from, netdp_so_socklen_t *fromlen);

/** Send the data using an opened socket
 *
 *  @param fd    - socket descriptor
 *  @param msg   - pointer to byte array containg the message
 *  @param len   - length of message in bytes
 *  @param flags - additional send flags
 *
 *  @return        Success: >0 (number of bytes sent); Failure: -1
 *
 */
int netdp_so_send(int fd, const void *msg, size_t len, int flags);


/** Send the data to a remote system using its ip address
 *
 *  @param fd    - socket descriptor
 *  @param buf   - pointer to byte array containg the message
 *  @param len   - length of message in bytes
 *  @param flags - additional send flags
 *  @param to    - pointer to structure holding the foreign address and port
 *  @param tolen - size of netdp_so_sockaddr structure
 *
 *  @return        Success: >0 (number of bytes sent); Failure: -1
 *
 */
int  netdp_so_sendto (int fd, const void *buf, size_t len, int flags, 
		    const struct netdp_so_sockaddr *to, netdp_so_socklen_t tolen);


/** Reads from a socket
 *  @param  fd     - a valid socket descriptor
 *  @param  buffer - buffer to hold the received data
 *  @param  count  - number of bytes to receive
 *  @return          >0: Success (number of bytes read); 
 *                   -1: Failure
 *                    0: Connection closed by the peer
 */
int     netdp_so_read(int fd, void* buffer, int count);

/** Reads  data  from file descriptor fd, and puts the result in the
 *  buffers described by vector
 * 
 *  @param  fd        - a valid socket descriptor
 *  @param  vector    - buffers to hold the data
 *  @param  veccount  - buffer count in vector
 *  @return             >0: Success (number of bytes read); 
 *                      -1: Failure
 */
int     netdp_so_readv(int fd, struct netdp_so_iovec_desc *vector, int veccount);

/** write to a socket
 *  @param  fd     - a valid socket descriptor
 *  @param  buffer - buffer holding the data to be sent
 *  @param  count  - number of bytes to send
 *  @return         >0: Success (number of bytes sent); 
 *                  -1: Failure
 *                   0: Connection closed by the peer
 */
int     netdp_so_write(int fd, void *buffer, int count);

/** Writes  data  to  file  descriptor  fd  from  the buffers
 *  described by vector.
 * 
 *  @param  fd        - a valid socket descriptor
 *  @param  vector    - buffers to hold the data
 *  @param  veccount  - buffer count in vector
 *  @return             >0: Success (number of bytes sent); 
 *                      -1: Failure
 */
int     netdp_so_writev(int fd, struct netdp_so_iovec_desc *vector, int veccount);

/** Close an already opened socket.
 *
 *  @param fd    - a valid socket descriptor
 *
 *  @return        Success: 0; Failure: -1
 */
int     netdp_so_close(int fd);

/** Get and set the socket fd's options through file control.
 *
 *  @param  fd   - a valid socket descriptor
 *  @param  cmd  - command to get or set
 *  @param  ...  - command related additional parameters
 *
 *  @return Success: 0; Failure: -1
 */
int     netdp_so_fcntl (int fd, int cmd, ...);


/** Wait for a number of socket descriptors to change status.
 *  @param nfds      - number of netdp_so_status entries passed
 *  @param status    - list of netdp_so_status entries 
 *  @param timeout   - upper bound on the amount of time elapsed before the
 *                     function returns. If it is NULL, this function will block 
 *                     until one of the fds are ready for read, write or have
 *                     an exception. If it is zero, the poll returns immediately
 *  @return            Success: >0 number of socket fds that had some activity 
 *                            : =0 if the timeout expires before anything interesting happens
 *                     Failure: -1
 */
int     netdp_so_poll (int nfds, netdp_so_status *status, struct timeval *timeout);


/** Shutdown all or part of a full-duplex TCP connection.
 *
 *  @param fd     - a valid socket descriptor
 *  @param how    - shutdown type indicator; 
 *                  (NETDP_TCP_SHUT_RD : shut down the reading side,
 *                  NETDP_TCP_SHUT_WR  : shut down the writing side,
 *                  NETDP_TCP_SHUT_RDWR : shut down both sides)
 *
 *  @return         Success: 0; Failure: -1
 */
int	netdp_so_shutdown(int fd, int how);


#ifndef DOXYGEN_SHOULD_SKIP_THIS 


#endif /* DOXYGEN_SHOULD_SKIP_THIS */



#ifndef DOXYGEN_SHOULD_SKIP_THIS 

#ifndef __KERNEL__
__END_DECLS
#endif /* __KERNEL__ */

#endif /* DOXYGEN_SHOULD_SKIP_THIS  */

/** Frees a netdp_so_iovec structure 
 *
 *  @param vector    - pointer to netdp_so_iovec structure 
 *
 *  @return            0: Success 
 *                    -1: Failure
 *
 * The iovec_struct_alloc_info and iovec_ptr_array_alloc_info
 * have the same format as that of an individual 64 bit
 * address format. These two fields hold the information of how 
 * the structure and array was being allocated - so that it can be de-allocated
 * correctly.
 *
 * If both the iovec_ptr_array_alloc_info and the 
 * iovec_struct_alloc_info are non-zero, the function
 * netdp_free_zc_iovec will de-allocate both the ptr array
 * as well as the netdp_so_iovec structure after de-allocating
 * the individual buffers stored by iovec_ptr_array.
 */

int netdp_so_free_zc_iovec(struct netdp_so_iovec *vector);

/** Allocates a netdp_so_iovec structure to hold "size" data
 *  @param vector    - pointer to netdp_so_iovec structure address which holds the data
 *  @param size      - size of the buffer to be allocated
 *  @return            0: Success 
 *                    -1: Failure
 *
 */
int netdp_so_alloc_zc_iovec(struct netdp_so_iovec** vector, int size);

/** Zero copy read
 *  @param sockfd    - a valid socket descriptor
 *  @param vector    - pointer to netdp_so_iovec structure address which holds the data
 *  @return           >0: Success (number of bytes read); 
 *                    -1: Failure: error during read;
 *                     0: Failure: connection closed by the peer
 */
int netdp_so_read_zc(int sockfd, struct netdp_so_iovec** vector);

/** Zero copy write
 *  @param sockfd       - a valid socket descriptor
 *  @param vector       - pointer to netdp_so_iovec structure address which holds the data
 *  @param delete_iovec - 1: socket layer will free the iovec structure
 *                        0: application layer will free the iovec structure
 *  @return               >0: Success (number of bytes written); 
 *                        -1: Failure: error during write;
 *                         0: Failure: connection closed by the peer
 */
int netdp_so_write_zc(int sockfd, struct netdp_so_iovec* vector, char delete_iovec);


#ifdef TCP_STATS
/** Get TCP stats
 *  @param stats        - pointer to a tcp stat (netdp_tcp_stats_t) structure
 *  @return                0: Success 
 *                        -1: Failure
 */
int netdp_so_get_tcpstats (netdp_tcp_stats_t *stats);
#endif /* TCP_STATS */

#endif /* __SOCKET_H_ */
