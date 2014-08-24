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


/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following edsclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following edsclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE EDSCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */




/** 
 * @file netdp_errno.h
 *
 * Description: netdp specific error codes
 *
 * 
 */

#ifndef __NETDP_COMMON_ERRNO_H__
#define __NETDP_COMMON_ERRNO_H__

#ifndef __KERNEL__
//add by wuxp for temp
#define _REENT_ONLY
 
#ifndef _REENT_ONLY
#define errno (*__errno())
extern int *__errno (void);
#endif

extern int _sys_nerr;

#define __errno_r(ptr) ((ptr)->_errno)

/* --- end of slight redundancy (the use of struct _reent->_errno is
 *        hard-coded in perror.c so why pretend anything else could work too ?
 *        */

#define __set_errno(x) (errno = (x))

#endif /* __KERNEL__ */

//delete by wuxp for temp
#define netdp_common_panic(...) 
#define NETDP_COMMON_KASSERT(exp,msg) \
    { \
    if(!(exp)) netdp_common_panic msg; \
    }

static inline int netdp_common_imax(int a, int b) { return (a > b ? a : b); }
static inline int netdp_common_imin(int a, int b) { return (a < b ? a : b); }
	

//define BSD_COMPLIANCE_ERROR_CODES
 

#ifdef  BSD_COMPLIANCE_ERROR_CODES

#define	NETDP_COMMON_EPERM		1		/* Operation not permitted */
#define	NETDP_COMMON_ENOENT		2		/* No such file or directory */
#define	NETDP_COMMON_ESRCH		3		/* No such process */
#define	NETDP_COMMON_EINTR		4		/* Interrupted system call */
#define	NETDP_COMMON_EIO			5		/* Input/output error */
#define	NETDP_COMMON_ENXIO		6		/* Device not configured */
#define	NETDP_COMMON_E2BIG		7		/* Argument list too long */
#define	NETDP_COMMON_ENOEXEC		8		/* Exec format error */
#define	NETDP_COMMON_EBADF		9		/* Bad file descriptor */
#define	NETDP_COMMON_ECHILD		10		/* No child processes */
#define	NETDP_COMMON_EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#define	NETDP_COMMON_ENOMEM		12		/* Cannot allocate memory */
#define	NETDP_COMMON_EACCES		13		/* Permission denied */
#define	NETDP_COMMON_EFAULT		14		/* Bad address */
#ifndef _POSIX_SOURCE
#define	NETDP_COMMON_ENOTBLK		15		/* Block device required */
#endif
#define	NETDP_COMMON_EBUSY		16		/* Device busy */
#define	NETDP_COMMON_EEXIST		17		/* File exists */
#define	NETDP_COMMON_EXDEV		18		/* Cross-device link */
#define	NETDP_COMMON_ENODEV		19		/* Operation not supported by device */
#define	NETDP_COMMON_ENOTDIR		20		/* Not a directory */
#define	NETDP_COMMON_EISDIR		21		/* Is a directory */
#define	NETDP_COMMON_EINVAL		22		/* Invalid argument */
#define	NETDP_COMMON_ENFILE		23		/* Too many open files in system */
#define	NETDP_COMMON_EMFILE		24		/* Too many open files */
#define	NETDP_COMMON_ENOTTY		25		/* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define	NETDP_COMMON_ETXTBSY		26		/* Text file busy */
#endif
#define	NETDP_COMMON_EFBIG		27		/* File too large */
#define	NETDP_COMMON_ENOSPC		28		/* No space left on device */
#define	NETDP_COMMON_ESPIPE		29		/* Illegal seek */
#define	NETDP_COMMON_EROFS		30		/* Read-only filesystem */
#define	NETDP_COMMON_EMLINK		31		/* Too many links */
#define	NETDP_COMMON_EPIPE		32		/* Broken pipe */

/* math software */
#define	NETDP_COMMON_EDOM		33		/* Numerical argument out of domain */
#define	NETDP_COMMON_ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	NETDP_COMMON_EAGAIN		35		/* Resource temporarily unavailable */
#ifndef _POSIX_SOURCE
#define	NETDP_COMMON_EWOULDBLOCK	NETDP_COMMON_EAGAIN	/* Operation would block */
#define	NETDP_COMMON_EINPROGRESS	36		/* Operation now in progress */
#define	NETDP_COMMON_EALREADY	37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	NETDP_COMMON_ENOTSOCK	38		/* Socket operation on non-socket */
#define	NETDP_COMMON_EDESTADDRREQ	39		/* Destination address required */
#define	NETDP_COMMON_EMSGSIZE	40		/* Message too long */
#define	NETDP_COMMON_EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	NETDP_COMMON_ENOPROTOOPT	42		/* Protocol not available */
#define	NETDP_COMMON_EPROTONOSUPPORT	43		/* Protocol not supported */
#define	NETDP_COMMON_ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	NETDP_COMMON_EOPNOTSUPP	45		/* Operation not supported */
#define	NETDP_COMMON_ENOTSUP	NETDP_COMMON_EOPNOTSUPP	/* Operation not supported */
#define	NETDP_COMMON_EPFNOSUPPORT	46		/* Protocol family not supported */
#define	NETDP_COMMON_EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	NETDP_COMMON_EADDRINUSE	48		/* Address already in use */
#define	NETDP_COMMON_EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	NETDP_COMMON_ENETDOWN	50		/* Network is down */
#define	NETDP_COMMON_ENETUNREACH	51		/* Network is unreachable */
#define	NETDP_COMMON_ENETRESET	52		/* Network dropped connection on reset */
#define	NETDP_COMMON_ECONNABORTED	53		/* Software caused connection abort */
#define	NETDP_COMMON_ECONNRESET	54		/* Connection reset by peer */
#define	NETDP_COMMON_ENOBUFS		55		/* No buffer space available */
#define	NETDP_COMMON_EISCONN		56		/* Socket is already connected */
#define	NETDP_COMMON_ENOTCONN	57		/* Socket is not connected */
#define	NETDP_COMMON_ESHUTDOWN	58		/* Can't send after socket shutdown */
#define	NETDP_COMMON_ETOOMANYREFS	59		/* Too many references: can't splice */
#define	NETDP_COMMON_ETIMEDOUT	60		/* Operation timed out */
#define	NETDP_COMMON_ECONNREFUSED	61		/* Connection refused */

#define	NETDP_COMMON_ELOOP		62		/* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define	NETDP_COMMON_ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define	NETDP_COMMON_EHOSTDOWN	64		/* Host is down */
#define	NETDP_COMMON_EHOSTUNREACH	65		/* No route to host */
#endif /* _POSIX_SOURCE */
#define	NETDP_COMMON_ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define	NETDP_COMMON_EPROCLIM	67		/* Too many processes */
#define	NETDP_COMMON_EUSERS		68		/* Too many users */
#define	NETDP_COMMON_EDQUOT		69		/* Disc quota exceeded */

/* Network File System */
#define	NETDP_COMMON_ESTALE		70		/* Stale NFS file handle */
#define	NETDP_COMMON_EREMOTE		71		/* Too many levels of remote in path */
#define	NETDP_COMMON_EBADRPC		72		/* RPC struct is bad */
#define	NETDP_COMMON_ERPCMISMATCH	73		/* RPC version wrong */
#define	NETDP_COMMON_EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	NETDP_COMMON_EPROGMISMATCH	75		/* Program version wrong */
#define	NETDP_COMMON_EPROCUNAVAIL	76		/* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define	NETDP_COMMON_ENOLCK		77		/* No locks available */
#define	NETDP_COMMON_ENOSYS		78		/* Function not implemented */

#ifndef _POSIX_SOURCE
#define	NETDP_COMMON_EFTYPE		79		/* Inappropriate file type or format */
#define	NETDP_COMMON_EAUTH		80		/* Authentication error */
#define	NETDP_COMMON_ENEEDAUTH	81		/* Need authenticator */
#define	NETDP_COMMON_EIDRM		82		/* Identifier removed */
#define	NETDP_COMMON_ENOMSG		83		/* No message of desired type */
#define	NETDP_COMMON_EOVERFLOW	84		/* Value too large to be stored in data type */
#define	NETDP_COMMON_ECANCELED	85		/* Operation canceled */
#define	NETDP_COMMON_EILSEQ		86		/* Illegal byte sequence */
#define	NETDP_COMMON_ENOATTR		87		/* Attribute not found */

#define NETDP_COMMON_EDOOFUS		88		/* Programming error */

#define	NETDP_COMMON_ELAST		88		/* Must be equal largest errno */

#endif /* _POSIX_SOURCE */

#ifdef _KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	NETDP_COMMON_ERESTART	(-1)		/* restart syscall */
#define	NETDP_COMMON_EJUSTRETURN	(-2)		/* don't modify regs, just return */
#define	NETDP_COMMON_ENOIOCTL	(-3)		/* ioctl not handled by this layer */
#define	NETDP_COMMON_EDIRIOCTL	(-4)		/* do direct ioctl in GEOM */
#endif

#else /* BSD_COMPLIANCE_ERROR_CODES */

#define NETDP_COMMON_EPERM 1	/* Not super-user */
#define NETDP_COMMON_ENOENT 2	/* No such file or directory */
#define NETDP_COMMON_ESRCH 3	/* No such process */
#define NETDP_COMMON_EINTR 4	/* Interrupted system call */
#define NETDP_COMMON_EIO 5	/* I/O error */
#define NETDP_COMMON_ENXIO 6	/* No such device or address */
#define NETDP_COMMON_E2BIG 7	/* Arg list too long */
#define NETDP_COMMON_ENOEXEC 8	/* Exec format error */
#define NETDP_COMMON_EBADF 9	/* Bad file number */
#define NETDP_COMMON_ECHILD 10	/* No children */
#define NETDP_COMMON_EAGAIN 11	/* No more processes */
#define NETDP_COMMON_ENOMEM 12	/* Not enough core */
#define NETDP_COMMON_EACCES 13	/* Permission denied */
#define NETDP_COMMON_EFAULT 14	/* Bad address */
#define NETDP_COMMON_ENOTBLK 15	/* Block device required */
#define NETDP_COMMON_EBUSY 16	/* Mount device busy */
#define NETDP_COMMON_EEXIST 17	/* File exists */
#define NETDP_COMMON_EXDEV 18	/* Cross-device link */
#define NETDP_COMMON_ENODEV 19	/* No such device */
#define NETDP_COMMON_ENOTDIR 20	/* Not a directory */
#define NETDP_COMMON_EISDIR 21	/* Is a directory */
#define NETDP_COMMON_EINVAL 22	/* Invalid argument */
#define NETDP_COMMON_ENFILE 23	/* Too many open files in system */
#define NETDP_COMMON_EMFILE 24	/* Too many open files */
#define NETDP_COMMON_ENOTTY 25	/* Not a typewriter */
#define NETDP_COMMON_ETXTBSY 26	/* Text file busy */
#define NETDP_COMMON_EFBIG 27	/* File too large */
#define NETDP_COMMON_ENOSPC 28	/* No space left on device */
#define NETDP_COMMON_ESPIPE 29	/* Illegal seek */
#define NETDP_COMMON_EROFS 30	/* Read only file system */
#define NETDP_COMMON_EMLINK 31	/* Too many links */
#define NETDP_COMMON_EPIPE 32	/* Broken pipe */
#define NETDP_COMMON_EDOM 33	/* Math arg out of domain of func */
#define NETDP_COMMON_ERANGE 34	/* Math result not representable */
#define NETDP_COMMON_ENOMSG 35	/* No message of desired type */
#define NETDP_COMMON_EIDRM 36	/* Identifier removed */
#define NETDP_COMMON_ECHRNG 37	/* Channel number out of range */
#define NETDP_COMMON_EL2NSYNC 38	/* Level 2 not synchronized */
#define NETDP_COMMON_EL3HLT 39	/* Level 3 halted */
#define NETDP_COMMON_EL3RST 40	/* Level 3 reset */
#define NETDP_COMMON_ELNRNG 41	/* Link number out of range */
#define NETDP_COMMON_EUNATCH 42	/* Protocol driver not attached */
#define NETDP_COMMON_ENOCSI 43	/* No CSI structure available */
#define NETDP_COMMON_EL2HLT 44	/* Level 2 halted */
#define NETDP_COMMON_EDEADLK 45	/* Deadlock condition */
#define NETDP_COMMON_ENOLCK 46	/* No record locks available */
#define NETDP_COMMON_EBADE 50	/* Invalid exchange */
#define NETDP_COMMON_EBADR 51	/* Invalid request descriptor */
#define NETDP_COMMON_EXFULL 52	/* Exchange full */
#define NETDP_COMMON_ENOANO 53	/* No anode */
#define NETDP_COMMON_EBADRQC 54	/* Invalid request code */
#define NETDP_COMMON_EBADSLT 55	/* Invalid slot */
#define NETDP_COMMON_EDEADLOCK 56	/* File locking deadlock error */
#define NETDP_COMMON_EBFONT 57	/* Bad font file fmt */
#define NETDP_COMMON_ENOSTR 60	/* Device not a stream */
#define NETDP_COMMON_ENODATA 61	/* No data (for no delay io) */
#define NETDP_COMMON_ETIME 62	/* Timer expired */
#define NETDP_COMMON_ENOSR 63	/* Out of streams resources */
#define NETDP_COMMON_ENONET 64	/* Machine is not on the network */
#define NETDP_COMMON_ENOPKG 65	/* Package not installed */
#define NETDP_COMMON_EREMOTE 66	/* The object is remote */
#define NETDP_COMMON_ENOLINK 67	/* The link has been severed */
#define NETDP_COMMON_EADV 68	/* Advertise error */
#define NETDP_COMMON_ESRMNT 69	/* Srmount error */
#define NETDP_COMMON_ECOMM 70	/* Communication error on send */
#define NETDP_COMMON_EPROTO 71	/* Protocol error */
#define NETDP_COMMON_EMULTIHOP 74	/* Multihop attempted */
#define NETDP_COMMON_ELBIN 75	/* Inode is remote (not really error) */
#define NETDP_COMMON_EDOTDOT 76	/* Cross mount point (not really error) */
#define NETDP_COMMON_EBADMSG 77	/* Trying to read unreadable message */
#define NETDP_COMMON_EFTYPE 79	/* Inappropriate file type or format */
#define NETDP_COMMON_ENOTUNIQ 80	/* Given log. name not unique */
#define NETDP_COMMON_EBADFD 81	/* f.d. invalid for this operation */
#define NETDP_COMMON_EREMCHG 82	/* Remote address changed */
#define NETDP_COMMON_ELIBACC 83	/* Can't access a needed shared lib */
#define NETDP_COMMON_ELIBBAD 84	/* Accessing a corrupted shared lib */
#define NETDP_COMMON_ELIBSCN 85	/* .lib section in a.out corrupted */
#define NETDP_COMMON_ELIBMAX 86	/* Attempting to link in too many libs */
#define NETDP_COMMON_ELIBEXEC 87	/* Attempting to exec a shared library */
#define NETDP_COMMON_ENOSYS 88	/* Function not implemented */
#define NETDP_COMMON_ENMFILE 89      /* No more files */
#define NETDP_COMMON_ENOTEMPTY 90	/* Directory not empty */
#define NETDP_COMMON_ENAMETOOLONG 91	/* File or path name too long */
#define NETDP_COMMON_ELOOP 92	/* Too many symbolic links */
#define NETDP_COMMON_EOPNOTSUPP 95	/* Operation not supported on transport endpoint */
#define NETDP_COMMON_EPFNOSUPPORT 96 /* Protocol family not supported */
#define NETDP_COMMON_ECONNRESET 104  /* Connection reset by peer */
#define NETDP_COMMON_ENOBUFS 105	/* No buffer space available */
#define NETDP_COMMON_EAFNOSUPPORT 106 /* Address family not supported by protocol family */
#define NETDP_COMMON_EPROTOTYPE 107	/* Protocol wrong type for socket */
#define NETDP_COMMON_ENOTSOCK 108	/* Socket operation on non-socket */
#define NETDP_COMMON_ENOPROTOOPT 109	/* Protocol not available */
#define NETDP_COMMON_ESHUTDOWN 110	/* Can't send after socket shutdown */
#define NETDP_COMMON_ECONNREFUSED 111	/* Connection refused */
#define NETDP_COMMON_EADDRINUSE 112		/* Address already in use */
#define NETDP_COMMON_ECONNABORTED 113	/* Connection aborted */
#define NETDP_COMMON_ENETUNREACH 114		/* Network is unreachable */
#define NETDP_COMMON_ENETDOWN 115		/* Network interface is not configured */
#define NETDP_COMMON_ETIMEDOUT 116		/* Connection timed out */
#define NETDP_COMMON_EHOSTDOWN 117		/* Host is down */
#define NETDP_COMMON_EHOSTUNREACH 118	/* Host is unreachable */
#define NETDP_COMMON_EINPROGRESS 119		/* Connection already in progress */
#define NETDP_COMMON_EALREADY 120		/* Socket already connected */
#define NETDP_COMMON_EDESTADDRREQ 121	/* Destination address required */
#define NETDP_COMMON_EMSGSIZE 122		/* Message too long */
#define NETDP_COMMON_EPROTONOSUPPORT 123	/* Unknown protocol */
#define NETDP_COMMON_ESOCKTNOSUPPORT 124	/* Socket type not supported */
#define NETDP_COMMON_EADDRNOTAVAIL 125	/* Address not available */
#define NETDP_COMMON_ENETRESET 126
#define NETDP_COMMON_EISCONN 127		/* Socket is already connected */
#define NETDP_COMMON_ENOTCONN 128		/* Socket is not connected */
#define NETDP_COMMON_ETOOMANYREFS 129
#define NETDP_COMMON_EPROCLIM 130
#define NETDP_COMMON_EUSERS 131
#define NETDP_COMMON_EDQUOT 132
#define NETDP_COMMON_ESTALE 133
#define NETDP_COMMON_ENOTSUP 134		/* Not supported */
#define NETDP_COMMON_ENOMEDIUM 135   /* No medium (in tape drive) */
#define NETDP_COMMON_ENOSHARE 136    /* No such host or network path */
#define NETDP_COMMON_ECASECLASH 137  /* Filename exists with different case */
#define NETDP_COMMON_EILSEQ 138
#define NETDP_COMMON_EOVERFLOW 139	/* Value too large for defined data type */

#endif /* BSD_COMPLIANCE_ERROR_CODES */

#endif /* __NETDP_COMMON_ERRNO_H__ */
