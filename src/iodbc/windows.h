#ifndef DEBUG

#endif
#ifndef APWIN                     /* don't reinclude    */
#define APWIN
#ifndef DEBUG

#endif

#ifdef WININC
#  include <windows.h>              /* normal windows.h    */
#  ifdef WIN32
#    include <windowsx.h>         /* windows extensions  */
#    define GetCurrentTask	(HTASK)GetCurrentProcess
#    define GetDOSEnvironment	GetEnvironmentStrings
#    define PCALL
#  else
#    define PCALL PASCAL
#  endif
#else                             /* not windows         */
/******* Common definitions and typedefs **********************/
#ifdef HPUX
#  ifndef _VOID
#    define	VOID	int
#  endif
#  ifndef _UCHAR
     typedef	char	UCHAR;
#    define _UCHAR
#  endif
#  ifndef _SCHAR
     typedef	char	SCHAR;
#    define _SCHAR
#  endif
#endif /* HPUX */

#ifdef OSF1
#  ifndef _UCHAR
     typedef	char	UCHAR;
#    define _UCHAR
#  endif
#  ifndef _SDWORD
#    define _SDWORD
     typedef	int	SDWORD;
#  endif
#  ifndef _UDWORD
#    define _UDWORD
     typedef	unsigned int   UDWORD;
#  endif
#  ifndef _LDOUBLE
#    define _LDOUBLE
     typedef	double	LDOUBLE;
#  endif
#  ifndef _DWORD
#    define _DWORD
     typedef	int	DWORD;
#  endif
#endif /* OSF1 */

#if defined (SVR4) && defined(I386)
#  ifndef _UCHAR
     typedef	char	UCHAR;
#    define _UCHAR
#  endif
#  ifndef _LDOUBLE
#    define _LDOUBLE
     typedef	double	LDOUBLE;
#  endif
#endif /* SVR4 && I386 */

#ifndef _VOID
#  define _VOID
#  define VOID                void
#endif

#ifdef WIN
#  define LONG long
#endif

#ifndef _DWORD
#  define _DWORD
/*   typedef unsigned long       DWORD; */
#endif

#ifndef _UINT
#  define _UINT
   typedef unsigned int        UINT;
#endif

#ifdef PC
#  define FAR                 _far
#  define NEAR                _near
#  define PASCAL              _pascal
#  define CDECL               _cdecl
#  define WINAPI              _far _pascal
#  define CALLBACK            _far _pascal
#  define EXPORT	      _export
#else
#  define FAR
#  define NEAR
#  define PASCAL
#  define CDECL
#  define WINAPI
#  define CALLBACK
#  define EXPORT
#endif
#define PCALL PASCAL

/****** Simple types & common helper macros ********************/

/* typedef int                 BOOL; */
#define FALSE               0
#define TRUE                1

typedef unsigned char       BYTE;
/* typedef unsigned short      WORD; */

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((UINT)(w) >> 8) & 0xFF))

#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) |\
			    (((DWORD)((WORD)(high))) << 16)))

#define max(a,b)           (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

/****** Common pointer types **********************************/

#ifndef NULL
#define NULL                0
#endif

/****** Common handle types *************************************/

typedef UINT                    HANDLE;
/* typedef UINT                    HWND; */


#endif /* ifdef not WIN */

#endif /* ifdef APWIN */

/* end of apwin.h */
