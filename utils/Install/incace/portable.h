/****************************************************************/
/*                                                              */
/*   A collection of routines used in making ACE portable for   */
/*   different computers                                        */
/*                                                              */
/****************************************************************/

#ifndef __portable_h
#define __portable_h

#include "os.h"

#ifdef  HI_LO_BYTE_ORDER

/* All kinds of inplace swap routines, to reverse from LOHI to HILO byte order */

#ifdef AMIGA

#if __SASC && __VERSION__>=6 && __REVISION__>=58

#define WORDswap(n)  (*(n) = __builtin_rol(*(n), 8, 1))
#define LONGswap(n)  ( WORDswap(&((WORD *)(n))[0]),\
                       WORDswap(&((WORD *)(n))[1]),\
                       *(n) = __builtin_rol(*(n), 16, 2) )
#else /* __SASC */

#define EORSWAP

#endif /* !__SASC */

#endif /* AMIGA  */


#ifdef EORSWAP

/*  With some compilers and processors these work faster then the
 *  regular swap below, for example on a Amiga 68040 using SAS C 6.58.
 *  But using builtin rotates works even faster, see above.
 */

#define WORDswap(n) (((BYTE*)(n))[0] ^= ((BYTE*)(n))[1],\
                     ((BYTE*)(n))[1] ^= ((BYTE*)(n))[0],\
                     ((BYTE*)(n))[0] ^= ((BYTE*)(n))[1])
#define LONGswap(n) (((BYTE*)(n))[0] ^= ((BYTE*)(n))[3],\
                     ((BYTE*)(n))[3] ^= ((BYTE*)(n))[0],\
                     ((BYTE*)(n))[0] ^= ((BYTE*)(n))[3],\
                     ((BYTE*)(n))[1] ^= ((BYTE*)(n))[2],\
                     ((BYTE*)(n))[2] ^= ((BYTE*)(n))[1],\
                     ((BYTE*)(n))[1] ^= ((BYTE*)(n))[2])
#endif  /* EORSWAP */

#ifndef WORDswap

/* If not yet defined use the standard swaps */

#define WORDswap(n)  (*(n) = (*(n) << 8) | (*(n) >> 8))
#define LONGswap(n)  ( WORDswap(&((WORD *)(n))[0]),\
                       WORDswap(&((WORD *)(n))[1]),\
                       *(n) = (*(n) >> 16) | (*(n) << 16) )
#endif /* WORDSWAP */

#endif /* HI_LO_BYTE_ORDER */


/* GENERIC: Convert to LONG or WORD from BYTE-Pointer-to-LOHI-byte-order data,
 *          without worrying if the bytes are word alined in memory.
 *  p is a pointer to char.
 */

#ifdef HI_LO_BYTE_ORDER

#define BUFP2WORD(p) ((UWORD)*(p)++ | ((*(p)++)<<8))
#define BUFP2LONG(p) ((ULONG)*(p)++ | ((*(p)++)<<8) | ((*(p)++)<<16) | ((*(p)++)<<24))

#define BUF2WORD(p) ((UWORD)*(p) | (*((p)+1)<<8))
#define BUF2LONG(p) ((ULONG)*(p) | (*((p)+1)<<8) | (*((p)+2)<<16) | (*((p)+3)<<24))

#else /* HI_LO_BYTE_ORDER */

#define BUFP2WORD(p) *(UWORD*)((p+=2)-2)
#define BUFP2LONG(p) *(ULONG*)((p+=4)-4)

#define BUF2WORD(p) (*(UWORD*)p)
#define BUF2LONG(p) (*(ULONG*)p)

#endif /* !HI_LO_BYTE_ORDER */

/* Timestamp macros */

#define get_tx(m,d,h,n) (((ULONG)m<<21)+((ULONG)d<<16)+((ULONG)h<<11)+(n<<5))
#define get_tstamp(y,m,d,h,n,s) ((((ULONG)(y-1980))<<25)+get_tx(m,d,h,n)+(s/2))

#define ts_year(ts)  ((UINT)((ts >> 25) & 0x7f) + 1980)
#define ts_month(ts) ((UINT)(ts >> 21) & 0x0f)      // 1..12 means Jan..Dec
#define ts_day(ts)   ((UINT)(ts >> 16) & 0x1f)      // 1..31 means 1st..31st
#define ts_hour(ts)  ((UINT)(ts >> 11) & 0x1f)
#define ts_min(ts)   ((UINT)(ts >> 5) & 0x3f)
#define ts_sec(ts)   ((UINT)((ts & 0x1f) * 2))


#endif /* __portable_h */
