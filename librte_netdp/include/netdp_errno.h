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
 * Copyright (c) 1982, 1986, 1989, 1993
 *  The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)errno.h 8.5 (Berkeley) 1/21/94
 * $FreeBSD: release/10.0.0/sys/sys/errno.h 250250 2013-05-04 19:07:22Z pluknet $
 */

#ifndef __NETDP_ERRNO_H__
#define __NETDP_ERRNO_H__

#undef  _POSIX_SOURCE

#define	NETDP_EPERM		1		/* Operation not permitted */
#define	NETDP_ENOENT		2		/* No such file or directory */
#define	NETDP_ESRCH		3		/* No such process */
#define	NETDP_EINTR		4		/* Interrupted system call */
#define	NETDP_EIO		       5		/* Input/output error */
#define	NETDP_ENXIO		6		/* Device not configured */
#define	NETDP_E2BIG		7		/* Argument list too long */
#define	NETDP_ENOEXEC		8		/* Exec format error */
#define	NETDP_EBADF		9		/* Bad file descriptor */
#define	NETDP_ECHILD		10		/* No child processes */
#define	NETDP_EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#define	NETDP_ENOMEM		12		/* Cannot allocate memory */
#define	NETDP_EACCES		13		/* Permission denied */
#define	NETDP_EFAULT		14		/* Bad address */
#ifndef _POSIX_SOURCE
#define	NETDP_ENOTBLK		15		/* Block device required */
#endif
#define	NETDP_EBUSY		16		/* Device busy */
#define	NETDP_EEXIST		17		/* File exists */
#define	NETDP_EXDEV		18		/* Cross-device link */
#define	NETDP_ENODEV		19		/* Operation not supported by device */
#define	NETDP_ENOTDIR		20		/* Not a directory */
#define	NETDP_EISDIR		21		/* Is a directory */
#define	NETDP_EINVAL		22		/* Invalid argument */
#define	NETDP_ENFILE		23		/* Too many open files in system */
#define	NETDP_EMFILE		24		/* Too many open files */
#define	NETDP_ENOTTY		25		/* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define	NETDP_ETXTBSY		26		/* Text file busy */
#endif
#define	NETDP_EFBIG		27		/* File too large */
#define	NETDP_ENOSPC		28		/* No space left on device */
#define	NETDP_ESPIPE		29		/* Illegal seek */
#define	NETDP_EROFS		30		/* Read-only filesystem */
#define	NETDP_EMLINK		31		/* Too many links */
#define	NETDP_EPIPE		32		/* Broken pipe */

/* math software */
#define	NETDP_EDOM		33		/* Numerical argument out of domain */
#define	NETDP_ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	NETDP_EAGAIN		35		/* Resource temporarily unavailable */
#ifndef _POSIX_SOURCE
#define	NETDP_EWOULDBLOCK	NETDP_EAGAIN		/* Operation would block */
#define	NETDP_EINPROGRESS	36		/* Operation now in progress */
#define	NETDP_EALREADY	37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	NETDP_ENOTSOCK	38		/* Socket operation on non-socket */
#define	NETDP_EDESTADDRREQ	39		/* Destination address required */
#define	NETDP_EMSGSIZE	40		/* Message too long */
#define	NETDP_EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	NETDP_ENOPROTOOPT	42		/* Protocol not available */
#define	NETDP_EPROTONOSUPPORT	43		/* Protocol not supported */
#define	NETDP_ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	NETDP_EOPNOTSUPP	45		/* Operation not supported */
#define	NETDP_ENOTSUP		NETDP_EOPNOTSUPP	/* Operation not supported */
#define	NETDP_EPFNOSUPPORT	46		/* Protocol family not supported */
#define	NETDP_EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	NETDP_EADDRINUSE	48		/* Address already in use */
#define	NETDP_EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	NETDP_ENETDOWN	50		/* Network is down */
#define	NETDP_ENETUNREACH	51		/* Network is unreachable */
#define	NETDP_ENETRESET	52		/* Network dropped connection on reset */
#define	NETDP_ECONNABORTED	53		/* Software caused connection abort */
#define	NETDP_ECONNRESET	54		/* Connection reset by peer */
#define	NETDP_ENOBUFS		55		/* No buffer space available */
#define	NETDP_EISCONN		56		/* Socket is already connected */
#define	NETDP_ENOTCONN	57		/* Socket is not connected */
#define	NETDP_ESHUTDOWN	58		/* Can't send after socket shutdown */
#define	NETDP_ETOOMANYREFS	59		/* Too many references: can't splice */
#define	NETDP_ETIMEDOUT	60		/* Operation timed out */
#define	NETDP_ECONNREFUSED	61		/* Connection refused */

#define	NETDP_ELOOP		62		/* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define	NETDP_ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define	NETDP_EHOSTDOWN	64		/* Host is down */
#define	NETDP_EHOSTUNREACH	65		/* No route to host */
#endif /* _POSIX_SOURCE */
#define	NETDP_ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define	NETDP_EPROCLIM	67		/* Too many processes */
#define	NETDP_EUSERS		68		/* Too many users */
#define	NETDP_EDQUOT		69		/* Disc quota exceeded */

/* Network File System */
#define	NETDP_ESTALE		70		/* Stale NFS file handle */
#define	NETDP_EREMOTE		71		/* Too many levels of remote in path */
#define	NETDP_EBADRPC		72		/* RPC struct is bad */
#define	NETDP_ERPCMISMATCH	73		/* RPC version wrong */
#define	NETDP_EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	NETDP_EPROGMISMATCH	75		/* Program version wrong */
#define	NETDP_EPROCUNAVAIL	76		/* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define	NETDP_ENOLCK		77		/* No locks available */
#define	NETDP_ENOSYS		78		/* Function not implemented */

#ifndef _POSIX_SOURCE
#define	NETDP_EFTYPE		79		/* Inappropriate file type or format */
#define	NETDP_EAUTH		80		/* Authentication error */
#define	NETDP_ENEEDAUTH	81		/* Need authenticator */
#define	NETDP_EIDRM		82		/* Identifier removed */
#define	NETDP_ENOMSG		83		/* No message of desired type */
#define	NETDP_EOVERFLOW	84		/* Value too large to be stored in data type */
#define	NETDP_ECANCELED	85		/* Operation canceled */
#define	NETDP_EILSEQ		86		/* Illegal byte sequence */
#define	NETDP_ENOATTR		87		/* Attribute not found */

#define	NETDP_EDOOFUS		88		/* Programming error */
#endif /* _POSIX_SOURCE */

#define	NETDP_EBADMSG		89		/* Bad message */
#define	NETDP_EMULTIHOP	90		/* Multihop attempted */
#define	NETDP_ENOLINK		91		/* Link has been severed */
#define	NETDP_EPROTO		92		/* Protocol error */

#ifndef _POSIX_SOURCE
#define	NETDP_ENOTCAPABLE	93		/* Capabilities insufficient */
#define	NETDP_ECAPMODE	94		/* Not permitted in capability mode */
#define	NETDP_ENOTRECOVERABLE	95		/* State not recoverable */
#define	NETDP_EOWNERDEAD	96		/* Previous owner died */
#endif /* _POSIX_SOURCE */

#ifndef _POSIX_SOURCE
#define	NETDP_ELAST		96		/* Must be equal largest errno */
#endif /* _POSIX_SOURCE */

#ifdef _KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	NETDP_ERESTART	(-1)		/* restart syscall */
#define	NETDP_EJUSTRETURN	(-2)		/* don't modify regs, just return */
#define	NETDP_ENOIOCTL	(-3)		/* ioctl not handled by this layer */
#define	NETDP_EDIRIOCTL	(-4)		/* do direct ioctl in GEOM */
#endif

#define  NETDP_EOK               0

#define  NETDP_ECTRLRING        1000
#define  NETDP_EMSGPOOL         1001
#define  NETDP_EMZ              1002
#define  NETDP_ESOCKET          1003
#define  NETDP_ESEM             1004

#define  NETDP_EIPFRAG          2000


#define NETDP_ERRMEM        NETDP_ENOMEM   /* Out of memory error.     */
#define NETDP_ERRBUF        NETDP_ENOBUFS    /* Buffer error.            */
#define NETDP_ERRTIMEOUT    NETDP_EWOULDBLOCK    /* Timeout.                 */
#define NETDP_ERRRTE        NETDP_EHOSTUNREACH    /* Routing problem.         */
#define NETDP_ERRINPROGRESS NETDP_EINPROGRESS    /* Operation in progress    */
#define NETDP_ERRVAL        NETDP_EINVAL    /* Illegal value.           */
#define NETDP_ERRWOULDBLOCK NETDP_EWOULDBLOCK    /* Operation would block.   */
#define NETDP_ERRUSE        NETDP_EADDRINUSE    /* Address in use.          */
#define NETDP_ERRISCONN     NETDP_EALREADY    /* Already connected.       */

//#define NETDP_ERRIS_FATAL(e) ((e) < NETDP_ERRISCONN)

#define NETDP_ERRABRT       NETDP_ECONNABORTED   /* Connection aborted.      */
#define NETDP_ERRRST        NETDP_ECONNRESET   /* Connection reset.        */
#define NETDP_ERRCLSD       NETDP_ENOTCONN   /* Connection closed.       */
#define NETDP_ERRCONN       NETDP_ENOTCONN   /* Not connected.           */
#define NETDP_ERRARG        NETDP_EIO   /* Illegal argument.        */

#define NETDP_FALSE   0
#define NETDP_TRUE    1

#endif
