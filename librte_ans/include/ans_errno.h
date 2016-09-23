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

#ifndef __ANS_ERRNO_H__
#define __ANS_ERRNO_H__

#undef  _POSIX_SOURCE

#ifdef  __linux__
#include <errno.h>

#define	ANS_EPERM		EPERM		/* Operation not permitted */
#define	ANS_ENOENT	ENOENT		/* No such file or directory */
#define	ANS_ESRCH		ESRCH		/* No such process */
#define	ANS_EINTR		EINTR		/* Interrupted system call */
#define	ANS_EIO		EIO  		/* Input/output error */
#define	ANS_ENXIO		ENXIO		/* Device not configured */
#define	ANS_E2BIG		E2BIG		/* Argument list too long */
#define	ANS_ENOEXEC	ENOEXEC		/* Exec format error */
#define	ANS_EBADF		EBADF		/* Bad file descriptor */
#define	ANS_ECHILD		ECHILD		/* No child processes */
#define	ANS_EDEADLK	EDEADLK		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#define	ANS_ENOMEM	ENOMEM		/* Cannot allocate memory */
#define	ANS_EACCES		EACCES		/* Permission denied */
#define	ANS_EFAULT		EFAULT		/* Bad address */
#ifndef _POSIX_SOURCE
#define	ANS_ENOTBLK	ENOTBLK		/* Block device required */
#endif
#define	ANS_EBUSY		EBUSY		/* Device busy */
#define	ANS_EEXIST		EEXIST		/* File exists */
#define	ANS_EXDEV		EXDEV		/* Cross-device link */
#define	ANS_ENODEV	ENODEV		/* Operation not supported by device */
#define	ANS_ENOTDIR	ENOTDIR		/* Not a directory */
#define	ANS_EISDIR		EISDIR		/* Is a directory */
#define	ANS_EINVAL		EINVAL		/* Invalid argument */
#define	ANS_ENFILE		ENFILE		/* Too many open files in system */
#define	ANS_EMFILE		EMFILE		/* Too many open files */
#define	ANS_ENOTTY	ENOTTY		/* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define	ANS_ETXTBSY	ETXTBSY		/* Text file busy */
#endif
#define	ANS_EFBIG		EFBIG		/* File too large */
#define	ANS_ENOSPC	ENOSPC		/* No space left on device */
#define	ANS_ESPIPE		ESPIPE		/* Illegal seek */
#define	ANS_EROFS		EROFS		/* Read-only filesystem */
#define	ANS_EMLINK		EMLINK		/* Too many links */
#define	ANS_EPIPE		EPIPE		/* Broken pipe */

/* math software */
#define	ANS_EDOM		EDOM		/* Numerical argument out of domain */
#define	ANS_ERANGE	ERANGE		/* Result too large */

/* non-blocking and interrupt i/o */
#define	ANS_EAGAIN	EAGAIN		/* Resource temporarily unavailable */
#ifndef _POSIX_SOURCE
#define	ANS_EWOULDBLOCK	ANS_EAGAIN		/* Operation would block */
#define	ANS_EINPROGRESS	EINPROGRESS		/* Operation now in progress */
#define	ANS_EALREADY	EALREADY		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	ANS_ENOTSOCK	ENOTSOCK		/* Socket operation on non-socket */
#define	ANS_EDESTADDRREQ	EDESTADDRREQ		/* Destination address required */
#define	ANS_EMSGSIZE	EMSGSIZE		/* Message too long */
#define	ANS_EPROTOTYPE	EPROTOTYPE		/* Protocol wrong type for socket */
#define	ANS_ENOPROTOOPT	ENOPROTOOPT		/* Protocol not available */
#define	ANS_EPROTONOSUPPORT	EPROTONOSUPPORT		/* Protocol not supported */
#define	ANS_ESOCKTNOSUPPORT	EPROTONOSUPPORT		/* Socket type not supported */
#define	ANS_EOPNOTSUPP	EOPNOTSUPP		/* Operation not supported */
#define	ANS_ENOTSUP		ANS_EOPNOTSUPP	/* Operation not supported */
#define	ANS_EPFNOSUPPORT	EPFNOSUPPORT		/* Protocol family not supported */
#define	ANS_EAFNOSUPPORT	EAFNOSUPPORT		/* Address family not supported by protocol family */
#define	ANS_EADDRINUSE	EADDRINUSE		/* Address already in use */
#define	ANS_EADDRNOTAVAIL	EADDRNOTAVAIL		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	ANS_ENETDOWN	ENETDOWN		/* Network is down */
#define	ANS_ENETUNREACH	ENETUNREACH		/* Network is unreachable */
#define	ANS_ENETRESET	ENETRESET		/* Network dropped connection on reset */
#define	ANS_ECONNABORTED	ECONNABORTED		/* Software caused connection abort */
#define	ANS_ECONNRESET	ECONNRESET		/* Connection reset by peer */
#define	ANS_ENOBUFS		ENOBUFS		/* No buffer space available */
#define	ANS_EISCONN		EISCONN		/* Socket is already connected */
#define	ANS_ENOTCONN	ENOTCONN		/* Socket is not connected */
#define	ANS_ESHUTDOWN	ESHUTDOWN		/* Can't send after socket shutdown */
#define	ANS_ETOOMANYREFS	ETOOMANYREFS		/* Too many references: can't splice */
#define	ANS_ETIMEDOUT	ETIMEDOUT		/* Operation timed out */
#define	ANS_ECONNREFUSED	ECONNREFUSED		/* Connection refused */

#define	ANS_ELOOP		ELOOP		/* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define	ANS_ENAMETOOLONG	ENAMETOOLONG		/* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define	ANS_EHOSTDOWN	EHOSTDOWN		/* Host is down */
#define	ANS_EHOSTUNREACH	EHOSTUNREACH		/* No route to host */
#endif /* _POSIX_SOURCE */
#define	ANS_ENOTEMPTY	ENOTEMPTY		/* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define	ANS_EPROCLIM	EPROCLIM		/* Too many processes */
#define	ANS_EUSERS		EUSERS			/* Too many users */
#define	ANS_EDQUOT	EDQUOT		/* Disc quota exceeded */

/* Network File System */
#define	ANS_ESTALE		ESTALE		/* Stale NFS file handle */
#define	ANS_EREMOTE		EREMOTE		/* Too many levels of remote in path */
#define	ANS_EBADRPC		EBADRPC		/* RPC struct is bad */
#define	ANS_ERPCMISMATCH	ERPCMISMATCH		/* RPC version wrong */
#define	ANS_EPROGUNAVAIL	EPROGUNAVAIL		/* RPC prog. not avail */
#define	ANS_EPROGMISMATCH	EPROGMISMATCH		/* Program version wrong */
#define	ANS_EPROCUNAVAIL	EPROCUNAVAIL		/* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define	ANS_ENOLCK		ENOLCK		/* No locks available */
#define	ANS_ENOSYS	ENOSYS		/* Function not implemented */

#ifndef _POSIX_SOURCE
#define	ANS_EFTYPE		EFTYPE		/* Inappropriate file type or format */
#define	ANS_EAUTH		EAUTH		/* Authentication error */
#define	ANS_ENEEDAUTH	ENEEDAUTH		/* Need authenticator */
#define	ANS_EIDRM		EIDRM		/* Identifier removed */
#define	ANS_ENOMSG	ENOMSG		/* No message of desired type */
#define	ANS_EOVERFLOW	EOVERFLOW		/* Value too large to be stored in data type */
#define	ANS_ECANCELED	ECANCELED		/* Operation canceled */
#define	ANS_EILSEQ		EILSEQ			/* Illegal byte sequence */
#define	ANS_ENOATTR	ENOATTR		/* Attribute not found */

#define	ANS_EDOOFUS	EDOOFUS		/* Programming error */
#endif /* _POSIX_SOURCE */

#define	ANS_EBADMSG		EBADMSG		/* Bad message */
#define	ANS_EMULTIHOP	       MULTIHOP		/* Multihop attempted */
#define	ANS_ENOLINK		ENOLINK		/* Link has been severed */
#define	ANS_EPROTO		EPROTO		/* Protocol error */

#ifndef _POSIX_SOURCE
#define	ANS_ENOTCAPABLE	ENOTCAPABLE		/* Capabilities insufficient */
#define	ANS_ECAPMODE	ECAPMODE		/* Not permitted in capability mode */
#define	ANS_ENOTRECOVERABLE	ENOTRECOVERABLE		/* State not recoverable */
#define	ANS_EOWNERDEAD	EOWNERDEAD		/* Previous owner died */
#endif /* _POSIX_SOURCE */

#ifndef _POSIX_SOURCE
#define	ANS_ELAST		ELAST		/* Must be equal largest errno */
#endif /* _POSIX_SOURCE */

#ifdef _KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	ANS_ERESTART	(-1)		/* restart syscall */
#define	ANS_EJUSTRETURN	(-2)		/* don't modify regs, just return */
#define	ANS_ENOIOCTL	(-3)		/* ioctl not handled by this layer */
#define	ANS_EDIRIOCTL	(-4)		/* do direct ioctl in GEOM */
#endif

#else

#define	ANS_EPERM		1		/* Operation not permitted */
#define	ANS_ENOENT		2		/* No such file or directory */
#define	ANS_ESRCH		3		/* No such process */
#define	ANS_EINTR		4		/* Interrupted system call */
#define	ANS_EIO		       5		/* Input/output error */
#define	ANS_ENXIO		6		/* Device not configured */
#define	ANS_E2BIG		7		/* Argument list too long */
#define	ANS_ENOEXEC		8		/* Exec format error */
#define	ANS_EBADF		9		/* Bad file descriptor */
#define	ANS_ECHILD		10		/* No child processes */
#define	ANS_EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#define	ANS_ENOMEM		12		/* Cannot allocate memory */
#define	ANS_EACCES		13		/* Permission denied */
#define	ANS_EFAULT		14		/* Bad address */
#ifndef _POSIX_SOURCE
#define	ANS_ENOTBLK		15		/* Block device required */
#endif
#define	ANS_EBUSY		16		/* Device busy */
#define	ANS_EEXIST		17		/* File exists */
#define	ANS_EXDEV		18		/* Cross-device link */
#define	ANS_ENODEV		19		/* Operation not supported by device */
#define	ANS_ENOTDIR		20		/* Not a directory */
#define	ANS_EISDIR		21		/* Is a directory */
#define	ANS_EINVAL		22		/* Invalid argument */
#define	ANS_ENFILE		23		/* Too many open files in system */
#define	ANS_EMFILE		24		/* Too many open files */
#define	ANS_ENOTTY		25		/* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define	ANS_ETXTBSY		26		/* Text file busy */
#endif
#define	ANS_EFBIG		27		/* File too large */
#define	ANS_ENOSPC		28		/* No space left on device */
#define	ANS_ESPIPE		29		/* Illegal seek */
#define	ANS_EROFS		30		/* Read-only filesystem */
#define	ANS_EMLINK		31		/* Too many links */
#define	ANS_EPIPE		32		/* Broken pipe */

/* math software */
#define	ANS_EDOM		33		/* Numerical argument out of domain */
#define	ANS_ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	ANS_EAGAIN		35		/* Resource temporarily unavailable */
#ifndef _POSIX_SOURCE
#define	ANS_EWOULDBLOCK	ANS_EAGAIN		/* Operation would block */
#define	ANS_EINPROGRESS	36		/* Operation now in progress */
#define	ANS_EALREADY	37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	ANS_ENOTSOCK	38		/* Socket operation on non-socket */
#define	ANS_EDESTADDRREQ	39		/* Destination address required */
#define	ANS_EMSGSIZE	40		/* Message too long */
#define	ANS_EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	ANS_ENOPROTOOPT	42		/* Protocol not available */
#define	ANS_EPROTONOSUPPORT	43		/* Protocol not supported */
#define	ANS_ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	ANS_EOPNOTSUPP	45		/* Operation not supported */
#define	ANS_ENOTSUP		ANS_EOPNOTSUPP	/* Operation not supported */
#define	ANS_EPFNOSUPPORT	46		/* Protocol family not supported */
#define	ANS_EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	ANS_EADDRINUSE	48		/* Address already in use */
#define	ANS_EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	ANS_ENETDOWN	50		/* Network is down */
#define	ANS_ENETUNREACH	51		/* Network is unreachable */
#define	ANS_ENETRESET	52		/* Network dropped connection on reset */
#define	ANS_ECONNABORTED	53		/* Software caused connection abort */
#define	ANS_ECONNRESET	54		/* Connection reset by peer */
#define	ANS_ENOBUFS		55		/* No buffer space available */
#define	ANS_EISCONN		56		/* Socket is already connected */
#define	ANS_ENOTCONN	57		/* Socket is not connected */
#define	ANS_ESHUTDOWN	58		/* Can't send after socket shutdown */
#define	ANS_ETOOMANYREFS	59		/* Too many references: can't splice */
#define	ANS_ETIMEDOUT	60		/* Operation timed out */
#define	ANS_ECONNREFUSED	61		/* Connection refused */

#define	ANS_ELOOP		62		/* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define	ANS_ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define	ANS_EHOSTDOWN	64		/* Host is down */
#define	ANS_EHOSTUNREACH	65		/* No route to host */
#endif /* _POSIX_SOURCE */
#define	ANS_ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define	ANS_EPROCLIM	67		/* Too many processes */
#define	ANS_EUSERS		68		/* Too many users */
#define	ANS_EDQUOT		69		/* Disc quota exceeded */

/* Network File System */
#define	ANS_ESTALE		70		/* Stale NFS file handle */
#define	ANS_EREMOTE		71		/* Too many levels of remote in path */
#define	ANS_EBADRPC		72		/* RPC struct is bad */
#define	ANS_ERPCMISMATCH	73		/* RPC version wrong */
#define	ANS_EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	ANS_EPROGMISMATCH	75		/* Program version wrong */
#define	ANS_EPROCUNAVAIL	76		/* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define	ANS_ENOLCK		77		/* No locks available */
#define	ANS_ENOSYS		78		/* Function not implemented */

#ifndef _POSIX_SOURCE
#define	ANS_EFTYPE		79		/* Inappropriate file type or format */
#define	ANS_EAUTH		80		/* Authentication error */
#define	ANS_ENEEDAUTH	81		/* Need authenticator */
#define	ANS_EIDRM		82		/* Identifier removed */
#define	ANS_ENOMSG		83		/* No message of desired type */
#define	ANS_EOVERFLOW	84		/* Value too large to be stored in data type */
#define	ANS_ECANCELED	85		/* Operation canceled */
#define	ANS_EILSEQ		86		/* Illegal byte sequence */
#define	ANS_ENOATTR		87		/* Attribute not found */

#define	ANS_EDOOFUS		88		/* Programming error */
#endif /* _POSIX_SOURCE */

#define	ANS_EBADMSG		89		/* Bad message */
#define	ANS_EMULTIHOP	90		/* Multihop attempted */
#define	ANS_ENOLINK		91		/* Link has been severed */
#define	ANS_EPROTO		92		/* Protocol error */

#ifndef _POSIX_SOURCE
#define	ANS_ENOTCAPABLE	93		/* Capabilities insufficient */
#define	ANS_ECAPMODE	94		/* Not permitted in capability mode */
#define	ANS_ENOTRECOVERABLE	95		/* State not recoverable */
#define	ANS_EOWNERDEAD	96		/* Previous owner died */
#endif /* _POSIX_SOURCE */

#ifndef _POSIX_SOURCE
#define	ANS_ELAST		96		/* Must be equal largest errno */
#endif /* _POSIX_SOURCE */

#ifdef _KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	ANS_ERESTART	(-1)		/* restart syscall */
#define	ANS_EJUSTRETURN	(-2)		/* don't modify regs, just return */
#define	ANS_ENOIOCTL	(-3)		/* ioctl not handled by this layer */
#define	ANS_EDIRIOCTL	(-4)		/* do direct ioctl in GEOM */
#endif

#endif


#define  ANS_EOK               0

#define  ANS_ECTRLRING        1000
#define  ANS_EMSGPOOL         1001
#define  ANS_EMZ              1002
#define  ANS_ESOCKET          1003
#define  ANS_ESEM             1004

#define  ANS_EIPFRAG          2000


#define ANS_ERRMEM        ANS_ENOMEM   /* Out of memory error.     */
#define ANS_ERRBUF        ANS_ENOBUFS    /* Buffer error.            */
#define ANS_ERRTIMEOUT    ANS_EWOULDBLOCK    /* Timeout.                 */
#define ANS_ERRRTE        ANS_EHOSTUNREACH    /* Routing problem.         */
#define ANS_ERRINPROGRESS ANS_EINPROGRESS    /* Operation in progress    */
#define ANS_ERRVAL        ANS_EINVAL    /* Illegal value.           */
#define ANS_ERRWOULDBLOCK ANS_EWOULDBLOCK    /* Operation would block.   */
#define ANS_ERRUSE        ANS_EADDRINUSE    /* Address in use.          */
#define ANS_ERRISCONN     ANS_EALREADY    /* Already connected.       */

//#define ANS_ERRIS_FATAL(e) ((e) < ANS_ERRISCONN)

#define ANS_ERRABRT       ANS_ECONNABORTED   /* Connection aborted.      */
#define ANS_ERRRST        ANS_ECONNRESET   /* Connection reset.        */
#define ANS_ERRCLSD       ANS_ENOTCONN   /* Connection closed.       */
#define ANS_ERRCONN       ANS_ENOTCONN   /* Not connected.           */
#define ANS_ERRARG        ANS_EIO   /* Illegal argument.        */

#define ANS_FALSE   0
#define ANS_TRUE    1


#endif
