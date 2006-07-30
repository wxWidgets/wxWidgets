/*---------------------------------------------------------------------------
    Win32 specific configuration section:
  ---------------------------------------------------------------------------*/

#ifndef __w32cfg_h
#define __w32cfg_h

#if (defined(__CYGWIN__) && !defined(__CYGWIN32__))
#  define __CYGWIN32__        /* new versions of CygWin may not define this */
#endif

#include <sys/types.h>        /* off_t, time_t, dev_t, ... */
#include <sys/stat.h>
#include <io.h>               /* read(), open(), etc. */
#include <time.h>
#if (defined(__RSXNT__) || defined(__EMX__)) && !defined(tzset)
#  define tzset _tzset
#endif
#ifdef __MINGW32__
   extern void _tzset(void);  /* this is missing in <time.h> */
#  ifndef tzset
#    define tzset _tzset
#  endif
#endif
#include <memory.h>
#if (!defined(__RSXNT__) && !defined(__CYGWIN32__))
#  include <direct.h>         /* mkdir() */
#endif
#include <fcntl.h>
#ifdef __CYGWIN32__
#  include <unistd.h>
   extern int setmode(int, int);        /* this is missing in <fcntl.h> */
#endif
#if (defined(MSC) || defined(__WATCOMC__) || defined(__MINGW32__))
#  include <sys/utime.h>
#else
#  include <utime.h>
#endif

#if defined(FILEIO_C)
#  ifndef __CYGWIN32__
#    include <conio.h>
#  endif
#  include <windows.h>
#  ifdef __RSXNT__
#    include "win32/rsxntwin.h"
#  endif
#endif
#if (defined(ENVARGS_C) || defined(EXTRACT_C) || defined(UNZIP_C) || \
     defined(ZCRYPT_INTERNAL))
#  include <windows.h>
#  ifdef __RSXNT__
#    include "win32/rsxntwin.h"
#  endif
#endif
#if (defined(__CYGWIN32__) || defined(__MINGW32__))
   /* the following definitions are missing in their <windows.h> */
#  ifndef AnsiToOem
#    define AnsiToOem CharToOemA
#  endif
#  ifndef OemToAnsi
#    define OemToAnsi OemToCharA
#  endif
#endif

#define DIR_END       '\\'      /* OS uses '\\' as directory separator */
#define DIR_END2      '/'       /* also check for '/' (RTL may convert) */
#ifndef DATE_FORMAT
#  define DATE_FORMAT DF_MDY
#endif
#define lenEOL        2
#define PutNativeEOL  {*q++ = native(CR); *q++ = native(LF);}

#if (defined(__RSXNT__) && !defined(HAVE_MKTIME))
#  define HAVE_MKTIME           /* use mktime() in time conversion routines */
#endif
#if (defined(__CYGWIN32__) && defined(HAVE_MKTIME))
#  undef HAVE_MKTIME            /* Cygnus' mktime() implementation is buggy */
#endif
#if (!defined(NT_TZBUG_WORKAROUND) && !defined(NO_NT_TZBUG_WORKAROUND))
#  define NT_TZBUG_WORKAROUND
#endif
#if (!defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME))
#  define USE_EF_UT_TIME
#endif
#if (!defined(NOTIMESTAMP) && !defined(TIMESTAMP))
#  define TIMESTAMP
#endif
#if (!defined(NO_NTSD_EAS) && !defined(NTSD_EAS))
#  define NTSD_EAS      /* enable NTSD support unless explicitly suppressed */
#endif

/* handlers for OEM <--> ANSI string conversions */
#ifdef __RSXNT__
   /* RSXNT uses OEM coded strings in functions supplied by C RTL */
#  ifdef CRTL_CP_IS_ISO
#    undef CRTL_CP_IS_ISO
#  endif
#  ifndef CRTL_CP_IS_OEM
#    define CRTL_CP_IS_OEM
#  endif
#else
   /* "real" native WIN32 compilers use ANSI coded strings in C RTL calls */
#  ifndef CRTL_CP_IS_ISO
#    define CRTL_CP_IS_ISO
#  endif
#  ifdef CRTL_CP_IS_OEM
#    undef CRTL_CP_IS_OEM
#  endif
#endif

#ifdef CRTL_CP_IS_ISO
   /* C RTL's file system support assumes ANSI coded strings */
#  define ISO_TO_INTERN(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define OEM_TO_INTERN(src, dst)  OemToAnsi(src, dst)
#  define INTERN_TO_ISO(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define INTERN_TO_OEM(src, dst)  AnsiToOem(src, dst)
#endif /* CRTL_CP_IS_ISO */
#ifdef CRTL_CP_IS_OEM
   /* C RTL's file system support assumes OEM coded strings */
#  define ISO_TO_INTERN(src, dst)  AnsiToOem(src, dst)
#  define OEM_TO_INTERN(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#  define INTERN_TO_ISO(src, dst)  OemToAnsi(src, dst)
#  define INTERN_TO_OEM(src, dst)  {if ((src) != (dst)) strcpy((dst), (src));}
#endif /* CRTL_CP_IS_OEM */
#define _OEM_INTERN(str1) OEM_TO_INTERN(str1, str1)
#define _ISO_INTERN(str1) ISO_TO_INTERN(str1, str1)
#ifndef WINDLL
   /* Despite best intentions, for the command-line version UzpPassword()
    * could return either character set, depending on whether running under
    * Win95 (DOS-session) or WinNT (native WinNT command interpreter)! */
#  define STR_TO_CP2(dst, src)  (AnsiToOem(src, dst), dst)
#  define STR_TO_CP3(dst, src)  (OemToAnsi(src, dst), dst)
#else
   /* The WINDLL front end is known to supply ISO/ANSI-coded passwords! */
#  define STR_TO_CP2(dst, src)  (AnsiToOem(src, dst), dst)
#endif
/* dummy defines to disable these functions, they are not needed */
#define STR_TO_OEM
#define STR_TO_ISO

/* Static variables that we have to add to Uz_Globs: */
#define SYSTEM_SPECIFIC_GLOBALS \
    int created_dir, renamed_fullpath, fnlen;\
    unsigned nLabelDrive;\
    char lastRootPath[4];\
    int lastVolOldFAT, lastVolLocTim;\
    char *rootpath, *buildpathHPFS, *buildpathFAT, *endHPFS, *endFAT;\
    char *dirname, *wildname, matchname[FILNAMSIZ];\
    int rootlen, have_dirname, dirnamelen, notfirstcall;\
    zvoid *wild_dir;

/* created_dir, renamed_fullpath, fnlen, and nLabelDrive are used by   */
/*    both mapname() and checkdir().                                   */
/* lastRootPath and lastVolOldFAT are used by IsVolumeOldFAT().        */
/* rootlen, rootpath, buildpathHPFS, buildpathFAT, endHPFS, and endFAT */
/*    are used by checkdir().                                          */
/* wild_dir, dirname, wildname, matchname[], dirnamelen, have_dirname, */
/*    and notfirstcall are used by do_wild().                          */

#if (defined(_MSC_VER) && !defined(MSC))
#  define MSC
#endif

/* This replacement for C-RTL-supplied getch() (or similar) functionality
 * avoids leaving unabsorbed LFs in the keyboard buffer under Windows95,
 * and supports the <ALT>+[0]<digit><digit><digit> feature.
 */
int getch_win32  OF((void));

/* This patch of stat() is useful for at least two compilers.  It is   */
/* difficult to take a stat() of a root directory under Windows95, so  */
/* zstat_win32() detects that case and fills in suitable values.       */
#ifndef __RSXNT__
#  ifndef W32_STATROOT_FIX
#    define W32_STATROOT_FIX
#  endif
#endif /* !__RSXNT__ */

#if (defined(NT_TZBUG_WORKAROUND) || defined(W32_STATROOT_FIX))
#  define W32_STAT_BANDAID
#  if (defined(NT_TZBUG_WORKAROUND) && defined(REENTRANT))
#    define __W32STAT_GLOBALS__     Uz_Globs *pG,
#    define __W32STAT_G__           pG,
#  else
#    define __W32STAT_GLOBALS__
#    define __W32STAT_G__
#  endif
#  undef SSTAT
#  ifdef WILD_STAT_BUG
#    define SSTAT(path, pbuf) (iswild(path) || zstat_win32(__W32STAT_G__ path, pbuf))
#  else
#    define SSTAT(path, pbuf) zstat_win32(__W32STAT_G__ path, pbuf)
#  endif
#endif

#ifdef __WATCOMC__
#  ifdef __386__
#    ifndef WATCOMC_386
#      define WATCOMC_386
#    endif
#    define __32BIT__
#    undef far
#    define far
#    undef near
#    define near

/* Get asm routines to link properly without using "__cdecl": */
#    ifndef USE_ZLIB
#      pragma aux crc32         "_*" parm caller [] value [eax] modify [eax]
#      pragma aux get_crc_table "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif /* !USE_ZLIB */
#  endif /* __386__ */

#  ifndef EPIPE
#    define EPIPE -1
#  endif
#  define PIPE_ERROR (errno == EPIPE)
#endif /* __WATCOMC__ */

#endif /* !__w32cfg_h */
