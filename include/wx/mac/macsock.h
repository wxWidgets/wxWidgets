/*********************************************************************
Project	:	GUSI				-	Grand Unified Socket Interface
File		:	GUSI.h			-	Socket calls
Author	:	Matthias Neeracher
Language	:	MPW C/C++
*********************************************************************/

#ifndef _GUSI_
#define _GUSI_

#ifdef __MWERKS__
	#ifndef macintosh
		#define macintosh 1
	#endif
#endif

#ifdef macintosh
#include <errno.h>
#else
#ifndef KERNEL
extern int errno;			/* global error number */
#endif
#endif

#define	__P(protos)	protos		/* full-blown ANSI C */

#define	EPERM			1		/* Operation not permitted */
/* MSL defines ENOMEM, EACCES, ENOENT, ENOSYS. We give in. */
#ifndef ENOENT
#define	ENOENT		2		/* No such file or directory */
#endif
#define	ESRCH			3		/* No such process */
#define	EINTR			4		/* Interrupted system call */
#define	EIO			5		/* Input/output error */
#define	ENXIO			6		/* Device not configured */
#define	E2BIG			7		/* Argument list too long */
#define	ENOEXEC		8		/* Exec format error */
#define	EBADF			9		/* Bad file descriptor */
#define	ECHILD		10		/* No child processes */
#define	EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */
#ifndef ENOMEM	
#define	ENOMEM		12		/* Cannot allocate memory */
#define	EACCES		13		/* Permission denied */
#endif
#define	EFAULT		14		/* Bad address */
#ifndef _POSIX_SOURCE
#define	ENOTBLK		15		/* Block device required */
#define	EBUSY			16		/* Device busy */
#endif
#define	EEXIST		17		/* File exists */
#define	EXDEV			18		/* Cross-device link */
#define	ENODEV		19		/* Operation not supported by device */
#define	ENOTDIR		20		/* Not a directory */
#define	EISDIR		21		/* Is a directory */
#define	EINVAL		22		/* Invalid argument */
#define	ENFILE		23		/* Too many open files in system */
#define	EMFILE		24		/* Too many open files */
#define	ENOTTY		25		/* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define	ETXTBSY		26		/* Text file busy */
#endif
#define	EFBIG			27		/* File too large */
#define	ENOSPC		28		/* No space left on device */
#define	ESPIPE		29		/* Illegal seek */
#define	EROFS			30		/* Read-only file system */
#define	EMLINK		31		/* Too many links */
#define	EPIPE			32		/* Broken pipe */

#ifndef __MWERKS__
/* math software */
#define	EDOM			33		/* Numerical argument out of domain */
#define	ERANGE		34		/* Result too large */
#endif

/* non-blocking and interrupt i/o */
#if defined(macintosh) 
#ifndef EAGAIN
#define EAGAIN 		11
#endif
#else
#define	EAGAIN		35		/* Resource temporarily unavailable */
#endif
#define	EWOULDBLOCK	EAGAIN	/* Operation would block */
#ifndef _POSIX_SOURCE
/* Did I tell that MSL also occupies errnos 33-40? We give in. */
#if ENOMEM==37
#define	EINPROGRESS	136		/* Operation now in progress */
#define	EALREADY		137		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	ENOTSOCK		138		/* Socket operation on non-socket */
#define	EDESTADDRREQ	139		/* Destination address required */
#define	EMSGSIZE		140		/* Message too long */
#else
#define	EINPROGRESS	36		/* Operation now in progress */
#define	EALREADY		37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	ENOTSOCK		38		/* Socket operation on non-socket */
#define	EDESTADDRREQ	39		/* Destination address required */
#define	EMSGSIZE		40		/* Message too long */
#endif
#define	EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	ENOPROTOOPT	42		/* Protocol not available */
#define	EPROTONOSUPPORT	43		/* Protocol not supported */
#define	ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	EOPNOTSUPP		45		/* Operation not supported on socket */
#define	EPFNOSUPPORT	46		/* Protocol family not supported */
#define	EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	EADDRINUSE		48		/* Address already in use */
#define	EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	ENETDOWN			50		/* Network is down */
#define	ENETUNREACH		51		/* Network is unreachable */
#define	ENETRESET		52		/* Network dropped connection on reset */
#define	ECONNABORTED	53		/* Software caused connection abort */
#define	ECONNRESET		54		/* Connection reset by peer */
#define	ENOBUFS			55		/* No buffer space available */
#define	EISCONN			56		/* Socket is already connected */
#define	ENOTCONN			57		/* Socket is not connected */
#define	ESHUTDOWN		58		/* Can't send after socket shutdown */
#define	ETOOMANYREFS	59		/* Too many references: can't splice */
#define	ETIMEDOUT		60		/* Connection timed out */
#define	ECONNREFUSED	61		/* Connection refused */

#define	ELOOP				62		/* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define	ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define	EHOSTDOWN		64		/* Host is down */
#define	EHOSTUNREACH	65		/* No route to host */
#endif /* _POSIX_SOURCE */
#define	ENOTEMPTY		66		/* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define	EPROCLIM			67		/* Too many processes */
#define	EUSERS			68		/* Too many users */
#define	EDQUOT			69		/* Disc quota exceeded */

/* Network File System */
#define	ESTALE			70		/* Stale NFS file handle */
#define	EREMOTE			71		/* Too many levels of remote in path */
#define	EBADRPC			72		/* RPC struct is bad */
#define	ERPCMISMATCH	73		/* RPC version wrong */
#define	EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	EPROGMISMATCH	75		/* Program version wrong */
#define	EPROCUNAVAIL	76		/* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define	ENOLCK			77		/* No locks available */

#ifndef ENOSYS
#define	ENOSYS			78		/* Function not implemented */
#endif

#define	EFTYPE			79		/* Inappropriate file type or format */

#ifdef KERNEL
/* pseudo-errors returned inside kernel to modify return to process */
#define	ERESTART	-1		/* restart syscall */
#define	EJUSTRETURN	-2		/* don't modify regs, just return */
#endif

#define	NBBY	8		/* number of bits in a byte */

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef	FD_SETSIZE
#ifdef macintosh
#define 	FD_SETSIZE	64
#else
#define	FD_SETSIZE	256
#endif
#endif

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))

// #include <sys/types.h>
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* Sys V compatibility */

typedef	char *	caddr_t;		/* core address */
typedef	long	daddr_t;		/* disk address */
typedef	short	dev_t;			/* device number */
typedef	u_long	ino_t;			/* inode number */
typedef	long	off_t;			/* file offset (should be a quad) */
typedef	u_short	nlink_t;		/* link count */
typedef	long	swblk_t;		/* swap offset */
typedef	long	segsz_t;		/* segment size */
typedef	u_short	uid_t;			/* user id */
typedef	u_short	gid_t;			/* group id */
typedef	short	pid_t;			/* process id */
typedef	u_short	mode_t;			/* permissions */
typedef u_long	fixpt_t;		/* fixed point number */


// Feel free to increase FD_SETSIZE as needed 
#define GUSI_MAX_FD	FD_SETSIZE

//#include <sys/cdefs.h>

#ifndef macintosh
#ifdef __MWERKS__
#define macintosh 1
#endif
#endif

#if defined(__cplusplus)
#define	__BEGIN_DECLS	extern "C" {
#define	__END_DECLS	}
#else
#define	__BEGIN_DECLS
#define	__END_DECLS
#endif

//#include <compat.h>
//#include <sys/ioctl.h>
#define	IOCPARM_MASK	0x1fff		/* parameter length, at most 13 bits */
#define	IOCPARM_LEN(x)	(((x) >> 16) & IOCPARM_MASK)
#define	IOCBASECMD(x)	((x) & ~IOCPARM_MASK)
#define	IOCGROUP(x)	(((x) >> 8) & 0xff)

#define	IOCPARM_MAX	NBPG		/* max size of ioctl, mult. of NBPG */
#define	IOC_VOID	0x20000000	/* no parameters */
#define	IOC_OUT		0x40000000	/* copy out parameters */
#define	IOC_IN		0x80000000	/* copy in parameters */
#define	IOC_INOUT	(IOC_IN|IOC_OUT)
#define	IOC_DIRMASK	0xe0000000	/* mask for IN/OUT/VOID */

#define _IOC(inout,group,num,len) \
	(inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define	_IO(g,n)	_IOC(IOC_VOID,	(g), (n), 0)
#define	_IOR(g,n,t)	_IOC(IOC_OUT,	(g), (n), sizeof(t))
#define	_IOW(g,n,t)	_IOC(IOC_IN,	(g), (n), sizeof(t))
/* this should be _IORW, but stdio got there first */
#define	_IOWR(g,n,t)	_IOC(IOC_INOUT,	(g), (n), sizeof(t))

#define	FIOCLEX		_IO('f', 1)		/* set close on exec on fd */
#define	FIONCLEX	_IO('f', 2)		/* remove close on exec */
#define	FIONREAD	_IOR('f', 127, int)	/* get # bytes to read */
#define	FIONBIO		_IOW('f', 126, int)	/* set/clear non-blocking i/o */
#define	FIOASYNC	_IOW('f', 125, int)	/* set/clear async i/o */
#define	FIOSETOWN	_IOW('f', 124, int)	/* set owner */
#define	FIOGETOWN	_IOR('f', 123, int)	/* get owner */

__BEGIN_DECLS
#ifdef macintosh
int ioctl(int fildes, unsigned int cmd, unsigned long *arg);
#else
int	ioctl __P((int, unsigned long, ...));
#endif
__END_DECLS

//#include <sys/fcntl.h>
//#include <sys/stat.h>
//#include <dirent.h>
//#include <Types.h>
//#include <Events.h>
//#include <Files.h>
//#include <AppleTalk.h>
//#include <CTBUtilities.h>
//#include <Packages.h>
//#include <PPCToolBox.h>
//#include <StandardFile.h>
//#include <stdio.h>
//#include <sys/time.h>


/*
 * Definitions for byte order, according to byte significance from low
 * address to high.
 */
#define	LITTLE_ENDIAN	1234	/* LSB first: i386, vax */
#define	BIG_ENDIAN		4321	/* MSB first: 68000, ibm, net */
#define	PDP_ENDIAN		3412	/* LSB first in word, MSW first in long */

#define	BYTE_ORDER	BIG_ENDIAN

__BEGIN_DECLS
unsigned long	htonl (unsigned long);
unsigned short	htons (unsigned short);
unsigned long	ntohl (unsigned long);
unsigned short	ntohs (unsigned short);
__END_DECLS

/*
 * Macros for network/external number representation conversion.
 */
#if BYTE_ORDER == BIG_ENDIAN && !defined(lint)
#define	ntohl(x)	(x)
#define	ntohs(x)	(x)
#define	htonl(x)	(x)
#define	htons(x)	(x)

#define	NTOHL(x)	(x)
#define	NTOHS(x)	(x)
#define	HTONL(x)	(x)
#define	HTONS(x)	(x)

#else

#define	NTOHL(x)	(x) = ntohl((u_long)x)
#define	NTOHS(x)	(x) = ntohs((u_short)x)
#define	HTONL(x)	(x) = htonl((u_long)x)
#define	HTONS(x)	(x) = htons((u_short)x)
#endif



/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981.
 */

/*
 * Protocols
 */
#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP	1		/* control message protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 	/* tp-4 w/ class negotiation */
#define	IPPROTO_EON		80		/* ISO cnlp */

#define	IPPROTO_RAW		255	/* raw IP packet */
#define	IPPROTO_MAX		256


/*
 * Local port number conventions:
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.
 */
#define	IPPORT_RESERVED		1024
#define	IPPORT_USERRESERVED	5000

/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr {
	u_long s_addr;
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define	IN_CLASSA(i)		(((long)(i) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		0x00ffffff
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(i)		(((long)(i) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		0x0000ffff
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(i)		(((long)(i) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		0x000000ff

#define	IN_CLASSD(i)		(((long)(i) & 0xf0000000) == 0xe0000000)
#define	IN_MULTICAST(i)		IN_CLASSD(i)

#define	IN_EXPERIMENTAL(i)	(((long)(i) & 0xe0000000) == 0xe0000000)
#define	IN_BADCLASS(i)		(((long)(i) & 0xf0000000) == 0xf0000000)

#define	INADDR_ANY		(u_long)0x00000000
#define	INADDR_BROADCAST	(u_long)0xffffffff	/* must be masked */
#ifndef KERNEL
#define	INADDR_NONE		0xffffffff		/* -1 return */
#endif

#define	IN_LOOPBACKNET		127			/* official! */

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
	u_char	sin_len;
	u_char	sin_family;
	u_short	sin_port;
	struct	in_addr sin_addr;
	char		sin_zero[8];
};

/*
 * Structure used to describe IP options.
 * Used to store options internally, to pass them to a process,
 * or to restore options retrieved earlier.
 * The ip_dst is used for the first-hop gateway when using a source route
 * (this gets put into the header proper).
 */
#ifdef __MWERKS__
#pragma cplusplus off
#endif
struct ip_opts {
	struct	in_addr ip_dst;		/* first hop, 0 w/o src rt */
	char		ip_opts[40];		/* actually variable in size */
};
#ifdef __MWERKS__
#pragma cplusplus reset
#endif

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define	IP_OPTIONS	1	/* buf/ip_opts; set/get IP per-packet options */
#define	IP_HDRINCL	2	/* int; header is included with data (raw) */
#define	IP_TOS		3	/* int; IP type of service and precedence */
#define	IP_TTL		4	/* int; IP time to live */
#define	IP_RECVOPTS	5	/* bool; receive all IP options w/datagram */
#define	IP_RECVRETOPTS	6	/* bool; receive IP options for response */
#define	IP_RECVDSTADDR	7	/* bool; receive IP dst addr w/datagram */
#define	IP_RETOPTS	8	/* ip_opts; set/get IP per-packet options */

/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;		/* and microseconds */
};

struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;			/* type of dst correction */
};
#define	DST_NONE		0	/* not on dst */
#define	DST_USA		1	/* USA style dst */
#define	DST_AUST		2	/* Australian style dst */
#define	DST_WET		3	/* Western European dst */
#define	DST_MET		4	/* Middle European dst */
#define	DST_EET		5	/* Eastern European dst */
#define	DST_CAN		6	/* Canada */
//#include <sys/socket.h>

/*
 * Definitions related to sockets: types, address families, options.
 */

/*
 * Types
 */
#define	SOCK_STREAM		1		/* stream socket */
#define	SOCK_DGRAM		2		/* datagram socket */
#define	SOCK_RAW			3		/* raw-protocol interface */
#define	SOCK_RDM			4		/* reliably-delivered message */
#define	SOCK_SEQPACKET	5		/* sequenced packet stream */

/*
 * Option flags per-socket.
 */
#define	SO_DEBUG			0x0001		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x0002		/* socket has had listen() */
#define	SO_REUSEADDR	0x0004		/* allow local address reuse */
#define	SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	SO_DONTROUTE	0x0010		/* just use interface addresses */
#define	SO_BROADCAST	0x0020		/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	SO_LINGER		0x0080		/* linger on close if data present */
#define	SO_OOBINLINE	0x0100		/* leave received OOB data in line */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF		0x1001		/* send buffer size */
#define SO_RCVBUF		0x1002		/* receive buffer size */
#define SO_SNDLOWAT	0x1003		/* send low-water mark */
#define SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	0x1005		/* send timeout */
#define SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR		0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */

/*
 * Structure used for manipulating linger option.
 */
struct	linger {
	int	l_onoff;			/* option on/off */
	int	l_linger;		/* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	SOL_SOCKET	0xffff		/* options for socket level */

/*
 * Address families.
 */
#ifdef macintosh
#define	AF_UNSPEC		 0		/* unspecified 									*/
#define	AF_UNIX			 1		/* local to host (pipes, portals) 			*/
#define	AF_INET			 2		/* internetwork: UDP, TCP, etc. 				*/
#define	AF_CTB			 3		/* Apple Comm Toolbox (not yet supported) */
#define	AF_FILE			 4		/* Normal File I/O (used internally)		*/
#define	AF_PPC			 5		/* PPC Toolbox										*/
#define	AF_PAP			 6		/* Printer Access Protocol (client only)	*/
#define	AF_APPLETALK	16		/* Apple Talk 										*/

#define	AF_MAX		20
#else
#define	ATALK_SYMADDR 272		/* Symbolic Address for AppleTalk 			*/
#define	AF_UNSPEC	0		/* unspecified */
#define	AF_UNIX		1		/* local to host (pipes, portals) */
#define	AF_INET		2		/* internetwork: UDP, TCP, etc. */
#define	AF_IMPLINK	3		/* arpanet imp addresses */
#define	AF_PUP		4		/* pup protocols: e.g. BSP */
#define	AF_CHAOS		5		/* mit CHAOS protocols */
#define	AF_NS			6		/* XEROX NS protocols */
#define	AF_ISO		7		/* ISO protocols */
#define	AF_OSI		AF_ISO
#define	AF_ECMA		8		/* european computer manufacturers */
#define	AF_DATAKIT	9		/* datakit protocols */
#define	AF_CCITT		10		/* CCITT protocols, X.25 etc */
#define	AF_SNA		11		/* IBM SNA */
#define AF_DECnet		12		/* DECnet */
#define AF_DLI			13		/* DEC Direct data link interface */
#define AF_LAT			14		/* LAT */
#define	AF_HYLINK	15		/* NSC Hyperchannel */
#define	AF_APPLETALK16		/* Apple Talk */
#define	AF_ROUTE		17		/* Internal Routing Protocol */
#define	AF_LINK		18		/* Link layer interface */
#define	pseudo_AF_XTP	19		/* eXpress Transfer Protocol (no AF) */

#define	AF_MAX		20
#endif

/*
 * Structure used by kernel to store most
 * addresses.
 */
#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
struct sockaddr {
	u_char	sa_len;			/* total length */
	u_char	sa_family;		/* address family */
	char	sa_data[14];		/* actually longer; address value */
};
#if defined(powerc) || defined (__powerc)
#pragma options align=reset
#endif

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto {
	u_short	sp_family;		/* address family */
	u_short	sp_protocol;		/* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#ifdef macintosh
#define	PF_UNSPEC		 AF_UNSPEC		/* unspecified 									*/
#define	PF_UNIX			 AF_UNIX			/* local to host (pipes, portals) 			*/
#define	PF_INET			 AF_INET			/* internetwork: UDP, TCP, etc. 				*/
#define	PF_CTB			 AF_CTB			/* Apple Comm Toolbox (not yet supported) */
#define	PF_FILE			 AF_FILE			/* Normal File I/O (used internally)		*/
#define	PF_PPC			 AF_PPC			/* PPC Toolbox										*/
#define	PF_PAP			 AF_PAP			/* Printer Access Protocol (client only)	*/
#define	PF_APPLETALK	 AF_APPLETALK	/* Apple Talk 										*/
#else
#define	PF_UNSPEC	AF_UNSPEC
#define	PF_UNIX		AF_UNIX
#define	PF_INET		AF_INET
#define	PF_IMPLINK	AF_IMPLINK
#define	PF_PUP		AF_PUP
#define	PF_CHAOS		AF_CHAOS
#define	PF_NS			AF_NS
#define	PF_ISO		AF_ISO
#define	PF_OSI		AF_ISO
#define	PF_ECMA		AF_ECMA
#define	PF_DATAKIT	AF_DATAKIT
#define	PF_CCITT		AF_CCITT
#define	PF_SNA		AF_SNA
#define PF_DECnet		AF_DECnet
#define PF_DLI			AF_DLI
#define PF_LAT			AF_LAT
#define	PF_HYLINK	AF_HYLINK
#define	PF_APPLETALK	AF_APPLETALK
#define	PF_ROUTE		AF_ROUTE
#define	PF_LINK		AF_LINK
#define	PF_XTP		pseudo_AF_XTP	/* really just proto family, no AF */
#endif

#define	PF_MAX		AF_MAX

/*
 * Maximum queue length specifiable by listen.
 */
#define	SOMAXCONN	5

/*
 * Message header for recvmsg and sendmsg calls.
 * Used value-result for recvmsg, value only for sendmsg.
 */
struct msghdr {
	caddr_t	msg_name;		/* optional address */
	u_int		msg_namelen;		/* size of address */
	struct	iovec *msg_iov;		/* scatter/gather array */
	u_int		msg_iovlen;		/* # elements in msg_iov */
	caddr_t	msg_control;		/* ancillary data, see below */
	u_int		msg_controllen;		/* ancillary data buffer len */
	int		msg_flags;		/* flags on received message */
};

#define	MSG_OOB			0x1		/* process out-of-band data */
#define	MSG_PEEK			0x2		/* peek at incoming message */
#define	MSG_DONTROUTE	0x4		/* send without using routing tables */
#define	MSG_EOR			0x8		/* data completes record */
#define	MSG_TRUNC		0x10		/* data discarded before delivery */
#define	MSG_CTRUNC		0x20		/* control data lost before delivery */
#define	MSG_WAITALL		0x40		/* wait for full request or error */

/*
 * Header for ancillary data objects in msg_control buffer.
 * Used for additional information with/about a datagram
 * not expressible by flags.  The format is a sequence
 * of message elements headed by cmsghdr structures.
 */
struct cmsghdr {
	u_int	cmsg_len;		/* data byte count, including hdr */
	int	cmsg_level;		/* originating protocol */
	int	cmsg_type;		/* protocol-specific type */
/* followed by	u_char  cmsg_data[]; */
};

/* given pointer to struct adatahdr, return pointer to data */
#define	CMSG_DATA(cmsg)		((u_char *)((cmsg) + 1))

/* given pointer to struct adatahdr, return pointer to next adatahdr */
#define	CMSG_NXTHDR(mhdr, cmsg)	\
	(((caddr_t)(cmsg) + (cmsg)->cmsg_len + sizeof(struct cmsghdr) > \
	    (mhdr)->msg_control + (mhdr)->msg_controllen) ? \
	    (struct cmsghdr *)NULL : \
	    (struct cmsghdr *)((caddr_t)(cmsg) + ALIGN((cmsg)->cmsg_len)))

#define	CMSG_FIRSTHDR(mhdr)	((struct cmsghdr *)(mhdr)->msg_control)

/* "Socket"-level control message types: */
#define	SCM_RIGHTS	0x01		/* access rights (array of int) */

/*
 * 4.3 compat sockaddr, move to compat file later
 */
struct osockaddr {
	u_short	sa_family;			/* address family */
	char		sa_data[14];		/* up to 14 bytes of direct address */
};

/*
 * 4.3-compat message header (move to compat file later).
 */
struct omsghdr {
	caddr_t	msg_name;			/* optional address */
	int		msg_namelen;		/* size of address */
	struct	iovec *msg_iov;	/* scatter/gather array */
	int		msg_iovlen;			/* # elements in msg_iov */
	caddr_t	msg_accrights;		/* access rights sent/received */
	int		msg_accrightslen;
};

//#include <string.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <sys/un.h>
//#include <unistd.h>
//#include <machine/endian.h>
//

typedef enum spin_msg {
	SP_MISC,				/* some weird thing, usually just return immediately if you get this */
	SP_SELECT,			/* in a select call */
	SP_NAME,				/* getting a host by name */
	SP_ADDR,				/* getting a host by address */
	SP_STREAM_READ,	/* Stream read call */
	SP_STREAM_WRITE,	/* Stream write call */
	SP_DGRAM_READ,		/* Datagram read call */
	SP_DGRAM_WRITE,	/* Datagram write call */
	SP_SLEEP,			/* sleeping, passes ticks left to sleep */
	SP_AUTO_SPIN		/* Autospin, passes argument to SpinCursor */
} spin_msg;

typedef int (*GUSISpinFn)(spin_msg msg, long param);
typedef void (*GUSIEvtHandler)(EventRecord * ev);
typedef GUSIEvtHandler	GUSIEvtTable[24];

/*
 * Address families, defined in sys/socket.h
 *
 
#define	AF_UNSPEC		 0		// unspecified
#define	AF_UNIX			 1		// local to host (pipes, portals)
#define	AF_INET			 2		// internetwork: UDP, TCP, etc.
#define	AF_CTB			 3		// Apple Comm Toolbox (not yet supported)
#define	AF_FILE			 4		// Normal File I/O (used internally)
#define	AF_PPC			 5		// PPC Toolbox
#define	AF_PAP			 6		// Printer Access Protocol (client only)
#define	AF_APPLETALK	16		// Apple Talk

*/

#define	ATALK_SYMADDR 272		/* Symbolic Address for AppleTalk 			*/

/*
 * Some Implementations of GUSI require you to call GUSISetup for the
 * socket families you'd like to have defined. It's a good idea to call
 * this for *all* implementations.
 *
 * GUSIDefaultSetup() will include all socket families.
 *
 * Never call any of the GUSIwithXXX routines directly.
 */

__BEGIN_DECLS
void GUSIwithAppleTalkSockets();
void GUSIwithInternetSockets();
void GUSIwithPAPSockets();
void GUSIwithPPCSockets();
void GUSIwithUnixSockets();
void GUSIwithSIOUXSockets();
void GUSIwithMPWSockets();

void GUSISetup(void (*socketfamily)());
void GUSIDefaultSetup();
__END_DECLS
/*
 * Types,  defined in sys/socket.h
 *

#define	SOCK_STREAM		 1		// stream socket 
#define	SOCK_DGRAM		 2		// datagram socket

*/

/*
 * Defined in sys/un.h
 *
 
struct sockaddr_un {
	short		sun_family;
	char 		sun_path[108];
};

*/

#ifndef PRAGMA_ALIGN_SUPPORTED
#error Apple had some fun with the conditional macros again
#endif

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

/*
struct sockaddr_atlk {
	short			family;
	AddrBlock	addr;
};

struct sockaddr_atlk_sym {
	short			family;
	EntityName	name;
};

struct sockaddr_ppc {
	short					family;
	LocationNameRec	location;
	PPCPortRec			port;
};

// Definitions for choose() 

#define 	CHOOSE_DEFAULT	1		
#define	CHOOSE_NEW		2		
#define	CHOOSE_DIR		4		

typedef struct {
	short			numTypes;
	SFTypeList	types;
} sa_constr_file;

typedef struct {
	short			numTypes;
	NLType		types;
} sa_constr_atlk;


// Definitions for sa_constr_ppc 

#define PPC_CON_NEWSTYLE		0x8000	
#define PPC_CON_MATCH_NAME		0x0001	
#define PPC_CON_MATCH_TYPE 	0x0002 	
#define PPC_CON_MATCH_NBP		0x0004	

typedef struct	{
	short			flags;
	Str32			nbpType;
	PPCPortRec	match;
} sa_constr_ppc;

*/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

__BEGIN_DECLS
/* 
 * IO/Socket stuff, defined elsewhere (unistd.h, sys/socket.h
 *
*/
int socket(int domain, int type, short protocol);
int bind(int s, void *name, int namelen);
int connect(int s, void *addr, int addrlen);
int listen(int s, int qlen);
int accept(int s, void *addr, int *addrlen);
int close(int s);
int read(int s, char *buffer, unsigned buflen);
int readv(int s, struct iovec *iov, int count);
int recv(int s, void *buffer, int buflen, int flags);
int recvfrom(int s, void *buffer, int buflen, int flags, void *from, int *fromlen);
int recvmsg(int s,struct msghdr *msg,int flags);
int write(int s, const char *buffer, unsigned buflen);
int writev(int s, struct iovec *iov, int count);
int send(int s, void *buffer, int buflen, int flags);
int sendto (int s, void *buffer, int buflen, int flags, void *to, int tolen);
int sendmsg(int s,struct msghdr *msg,int flags);
int select(int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int getdtablesize(void);
int getsockname(int s, void *name, int *namelen);
int getpeername(int s, struct sockaddr *name, int *namelen);
int shutdown(int s, int how);
int fcntl(int s, unsigned int cmd, int arg);
int dup(int s);
int dup2(int s, int s1);
//int ioctl(int d, unsigned int request, long *argp);
int getsockopt(int s, int level, int optname, char *optval, int * optlen);
int setsockopt(int s, int level, int optname, char *optval, int optlen);
int isatty(int);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);
int creat(const char*);
int faccess(char*, unsigned int, long*);
long lseek(int, long, int);
int open(const char*, int);
int unlink(char*);
int symlink(char* linkto, char* linkname);
int readlink(char* path, char* buf, int bufsiz);
int truncate(char *path, long length);
int ftruncate(int fd, long length);
int chdir(char * path);
int mkdir(char * path);
int rmdir(char * path);
char * getcwd(char * buf, int size);


/* 
 * Defined in stdio.h
 */
 
#ifdef __MWERKS__
void fsetfileinfo (char *filename, unsigned long newcreator, unsigned long newtype);
#endif

void fgetfileinfo (char *filename, unsigned long * creator, unsigned long * type);

#ifdef __MWERKS__
FILE *fdopen(int fd, const char *mode);
int fwalk(int (*func)(FILE * stream));
#endif

int choose(
		int 		domain,
		int 		type,
		char * 	prompt,
		void * 	constraint,
		int 		flags,
		void * 	name,
		int * 	namelen);

/* 
 * Hostname routines, defined in netdb.h
 *
*/ 

/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	**h_addr_list;	/* list of addresses from name server */
#define	h_addr	h_addr_list[0]	/* address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent {
	char		*n_name;	/* official name of net */
	char		**n_aliases;	/* alias list */
	int		n_addrtype;	/* net address type */
	unsigned long	n_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#define	HOST_NOT_FOUND	1 /* Authoritative Answer Host not found */
#define	TRY_AGAIN	2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define	NO_RECOVERY	3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	NO_DATA		4 /* Valid name, no data record of requested type */
#define	NO_ADDRESS	NO_DATA		/* no address, look for MX record */

int gethostname(char *machname, long buflen);

void		endhostent __P((void));
void		endnetent __P((void));
void		endprotoent __P((void));
void		endservent __P((void));
struct hostent	*gethostbyaddr __P((const char *, int, int));
struct hostent	*gethostbyname __P((const char *));
struct hostent *gethostent __P((void));
struct netent	*getnetbyaddr __P((long, int)); /* u_long? */
struct netent	*getnetbyname __P((const char *));
struct netent	*getnetent __P((void));
struct protoent	*getprotobyname __P((const char *));
struct protoent	*getprotobynumber __P((int));
struct protoent	*getprotoent __P((void));
struct servent	*getservbyname __P((const char *, const char *));
struct servent	*getservbyport __P((int, const char *));
struct servent	*getservent __P((void));
void		herror __P((const char *));
void		sethostent __P((int));
/* void		sethostfile __P((const char *)); */
void		setnetent __P((int));
void		setprotoent __P((int));
void		setservent __P((int));


char * inet_ntoa(struct in_addr inaddr);
struct in_addr inet_addr(const char *address);

/* 
 * GUSI supports a number of hooks. Every one of them has a different prototype, but needs
 * to be passed as a GUSIHook
 */

typedef enum {
	GUSI_SpinHook,	/* A GUSISpinFn, to be called when a call blocks */
	GUSI_ExecHook, /* Boolean (*hook)(const GUSIFileRef & ref), decides if file is executable */
	GUSI_FTypeHook,/* Boolean (*hook)(const FSSpec & spec) sets a default file type */
	GUSI_SpeedHook /* A long integer, to be added to the cursor spin variable */
} GUSIHookCode;

typedef void (*GUSIHook)(void);
void GUSISetHook(GUSIHookCode code, GUSIHook hook);
GUSIHook GUSIGetHook(GUSIHookCode code);

/* 
 * What to do when a routine blocks
 */

/* Defined for compatibility */
#define GUSISetSpin(routine)	GUSISetHook(GUSI_SpinHook, (GUSIHook)routine)
#define GUSIGetSpin()			(GUSISpinFn) GUSIGetHook(GUSI_SpinHook)

int GUSISetEvents(GUSIEvtTable table);
GUSIEvtHandler * GUSIGetEvents(void);

extern GUSIEvtHandler	GUSISIOWEvents[];

#define SIGPIPE	13
#define SIGALRM	14

/* 
 * BSD memory routines, defined in compat.h
 *
*/
#define index(a, b)						strchr(a, b)
#define rindex(a, b)						strrchr(a, b)
#define bzero(from, len) 				memset(from, 0, len)
#define bcopy(from, to, len)			memcpy(to, from, len)
#define bcmp(s1, s2, len)				memcmp(s1, s2, len)
#define bfill(from, len, x)			memset(from, x, len)


__END_DECLS

	typedef struct wxSockInternal 
	{
		long nothing ;
	} ;

