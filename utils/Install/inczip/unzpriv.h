/*---------------------------------------------------------------------------

  unzpriv.h

  This header file contains private (internal) macros, typedefs, prototypes
  and global-variable declarations used by all of the UnZip source files.
  In a prior life it was part of the main unzip.h header, but now it is only
  included by that header if UNZIP_INTERNAL is defined.

  ---------------------------------------------------------------------------*/



#ifndef __unzpriv_h   /* prevent multiple inclusions */
#define __unzpriv_h

/* First thing: Signal all following code that we compile UnZip utilities! */
#ifndef UNZIP
#  define UNZIP
#endif

/* GRR 960204:  MORE defined here in preparation for removal altogether */
#ifndef MORE
#  define MORE
#endif

/* fUnZip should never need to be reentrant */
#ifdef FUNZIP
#  ifdef REENTRANT
#    undef REENTRANT
#  endif
#  ifdef DLL
#    undef DLL
#  endif
#endif

#if (defined(DLL) && !defined(REENTRANT))
#  define REENTRANT
#endif

#if (!defined(DYNAMIC_CRC_TABLE) && !defined(FUNZIP))
#  define DYNAMIC_CRC_TABLE
#endif

#if (defined(DYNAMIC_CRC_TABLE) && !defined(REENTRANT))
#  ifndef DYNALLOC_CRCTAB
#    define DYNALLOC_CRCTAB
#  endif
#endif

/*---------------------------------------------------------------------------
    OS-dependent configuration for UnZip internals
  ---------------------------------------------------------------------------*/

/* bad or (occasionally?) missing stddef.h: */
#if (defined(M_XENIX) || defined(DNIX))
#  define NO_STDDEF_H
#endif

#if (defined(M_XENIX) && !defined(M_UNIX))   /* SCO Xenix only, not SCO Unix */
#  define SCO_XENIX
#  define NO_LIMITS_H        /* no limits.h, but MODERN defined */
#  define NO_UID_GID         /* no uid_t/gid_t */
#  define size_t int
#endif

#ifdef realix   /* Modcomp Real/IX, real-time SysV.3 variant */
#  define SYSV
#  define NO_UID_GID         /* no uid_t/gid_t */
#endif

#if (defined(_AIX) && !defined(_ALL_SOURCE))
#  define _ALL_SOURCE
#endif

#if defined(apollo)          /* defines __STDC__ */
#    define NO_STDLIB_H
#endif

#ifdef DNIX
#  define SYSV
#  define SHORT_NAMES         /* 14-char limitation on path components */
/* #  define FILENAME_MAX  14 */
#  define FILENAME_MAX  NAME_MAX    /* GRR:  experiment */
#endif

#if (defined(SYSTEM_FIVE) || defined(__SYSTEM_FIVE))
#  ifndef SYSV
#    define SYSV
#  endif
#endif /* SYSTEM_FIVE || __SYSTEM_FIVE */
#if (defined(M_SYSV) || defined(M_SYS5))
#  ifndef SYSV
#    define SYSV
#  endif
#endif /* M_SYSV || M_SYS5 */
/* __SVR4 and __svr4__ catch Solaris on at least some combos of compiler+OS */
#if (defined(__SVR4) || defined(__svr4__) || defined(sgi) || defined(__hpux))
#  ifndef SYSV
#    define SYSV
#  endif
#endif /* __SVR4 || __svr4__ || sgi || __hpux */
#if (defined(LINUX) || defined(__QNX__))
#  ifndef SYSV
#    define SYSV
#  endif
#endif /* LINUX || __QNX__ */

#if (defined(ultrix) || defined(__ultrix) || defined(bsd4_2))
#  if (!defined(BSD) && !defined(SYSV))
#    define BSD
#  endif
#endif /* ultrix || __ultrix || bsd4_2 */
#if (defined(sun) || defined(pyr) || defined(CONVEX))
#  if (!defined(BSD) && !defined(SYSV))
#    define BSD
#  endif
#endif /* sun || pyr || CONVEX */

#ifdef pyr  /* Pyramid:  has BSD and AT&T "universes" */
#  ifdef BSD
#    define pyr_bsd
#    define USE_STRINGS_H  /* instead of more common string.h */
#    define ZMEM           /* ZMEM now uses bcopy/bzero: not in AT&T universe */
#  endif                   /* (AT&T memcpy claimed to be very slow, though) */
#  define DECLARE_ERRNO
#endif /* pyr */

/* stat() bug for Borland, VAX C (also GNU?), and Atari ST MiNT on TOS
 * filesystems:  returns 0 for wildcards!  (returns 0xffffffff on Minix
 * filesystem or `U:' drive under Atari MiNT.)  Watcom C was previously
 * included on this list; it would be good to know what version the problem
 * was fixed at, if it did exist.  Watcom 10.6 has a separate stat() problem:
 * it fails on "." when the current directory is a root.  This is covered by
 * giving it a separate definition of SSTAT in OS-specific header files. */
#if (defined(__TURBOC__) || defined(VMS) || defined(__MINT__))
#  define WILD_STAT_BUG
#endif

#ifdef WILD_STAT_BUG
#  define SSTAT(path,pbuf) (iswild(path) || stat(path,pbuf))
#else
#  define SSTAT stat
#endif

#ifdef REGULUS  /* returns the inode number on success(!)...argh argh argh */
#  define stat(p,s) zstat((p),(s))
#endif

#define STRNICMP zstrnicmp

/*---------------------------------------------------------------------------
    OS-dependent includes
  ---------------------------------------------------------------------------*/

#ifdef EFT
#  define LONGINT off_t  /* Amdahl UTS nonsense ("extended file types") */
#else
#  define LONGINT long
#endif

#ifdef MODERN
#  ifndef NO_STDDEF_H
#    include <stddef.h>
#  endif
#  ifndef NO_STDLIB_H
#    include <stdlib.h>  /* standard library prototypes, malloc(), etc. */
#  endif
   typedef size_t extent;
#else /* !MODERN */
#  ifndef AOS_VS         /* mostly modern? */
     LONGINT lseek();
#    ifdef VAXC          /* not fully modern, but does have stdlib.h and void */
#      include <stdlib.h>
#    else
       char *malloc();
#    endif /* ?VAXC */
#  endif /* !AOS_VS */
   typedef unsigned int extent;
#endif /* ?MODERN */


#ifndef MINIX            /* Minix needs it after all the other includes (?) */
#  include <stdio.h>
#endif
#include <ctype.h>       /* skip for VMS, to use tolower() function? */
#include <errno.h>       /* used in mapname() */
#ifdef USE_STRINGS_H
#  include <strings.h>   /* strcpy, strcmp, memcpy, index/rindex, etc. */
#else
#  include <string.h>    /* strcpy, strcmp, memcpy, strchr/strrchr, etc. */
#endif
#if (defined(MODERN) && !defined(NO_LIMITS_H))
#  include <limits.h>    /* GRR:  EXPERIMENTAL!  (can be deleted) */
#endif

/* this include must be down here for SysV.4, for some reason... */
#include <signal.h>      /* used in unzip.c, fileio.c */


/*---------------------------------------------------------------------------
    API (DLL) section:
  ---------------------------------------------------------------------------*/

#ifdef DLL
#  define MAIN   UZ_EXP UzpMain   /* was UzpUnzip */
#  ifdef OS2DLL
#    undef Info
#    define REDIRECTC(c)             varputchar(__G__ c)
#    define REDIRECTPRINT(buf,size)  varmessage(__G__ buf, size)
#    define FINISH_REDIRECT()        finish_REXX_redirect(__G)
#  else
#    define REDIRECTC(c)
#    define REDIRECTPRINT(buf,size)  0
#    define FINISH_REDIRECT()        close_redirect(__G)
#  endif
#endif

/*---------------------------------------------------------------------------
    Acorn RISCOS section:
  ---------------------------------------------------------------------------*/

#ifdef RISCOS
#  include "acorn/riscos.h"
#endif

/*---------------------------------------------------------------------------
    Amiga section:
  ---------------------------------------------------------------------------*/

#ifdef AMIGA
#  include "amiga/amiga.h"
#endif

/*---------------------------------------------------------------------------
    AOS/VS section (somewhat similar to Unix, apparently):
  ---------------------------------------------------------------------------*/

#ifdef AOS_VS
#  ifdef FILEIO_C
#    include "aosvs/aosvs.h"
#  endif
#endif

/*---------------------------------------------------------------------------
    Atari ST section:
  ---------------------------------------------------------------------------*/

#ifdef ATARI
#  include <time.h>
#  include <stat.h>
#  include <fcntl.h>
#  include <limits.h>
#  define SYMLINKS
#  define EXE_EXTENSION  ".tos"
#  ifndef DATE_FORMAT
#    define DATE_FORMAT  DF_DMY
#  endif
#  define DIR_END        '/'
#  define INT_SPRINTF
#  define timezone      _timezone
#  define lenEOL        2
#  define PutNativeEOL  {*q++ = native(CR); *q++ = native(LF);}
#  undef SHORT_NAMES
#  if (!defined(NOTIMESTAMP) && !defined(TIMESTAMP))
#    define TIMESTAMP
#  endif
#endif

/*---------------------------------------------------------------------------
    BeOS section:
  ---------------------------------------------------------------------------*/

#ifdef __BEOS__
#  include <sys/types.h>          /* [cjh]:  This is pretty much a generic  */
#  include <sys/stat.h>           /* POSIX 1003.1 system; see beos/ for     */
#  include <fcntl.h>              /* extra code to deal with our extra file */
#  include <sys/param.h>          /* attributes. */
#  include <unistd.h>
#  include <utime.h>
#  define DIRENT
#  include <time.h>
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_MDY  /* GRR:  customize with locale.h somehow? */
#  endif
#  define lenEOL        1
#  define PutNativeEOL  *q++ = native(LF);
#  define SCREENLINES   screenlines()
#  define USE_EF_UT_TIME
#  define SET_DIR_ATTRIB
#  if (!defined(NOTIMESTAMP) && !defined(TIMESTAMP))
#    define TIMESTAMP
#  endif
#  define RESTORE_UIDGID
#  define NO_GMTIME               /* maybe DR10 will have timezones... */
#  define INT_SPRINTF
#  define SYMLINKS
#  define MAIN main_stub          /* now that we're using a wrapper... */
#endif

/*---------------------------------------------------------------------------
    Human68k/X68000 section:
  ---------------------------------------------------------------------------*/

#ifdef __human68k__    /* DO NOT DEFINE DOS_OS2 HERE!  If Human68k is so much */
#  include <time.h>    /*  like MS-DOS and/or OS/2, create DOS_H68_OS2 macro. */
#  include <fcntl.h>
#  include <io.h>
#  include <conio.h>
#  include <jctype.h>
#  include <sys/stat.h>
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_YMD   /* Japanese standard */
#  endif
      /* GRR:  these EOL macros are guesses */
#  define lenEOL        2
#  define PutNativeEOL  {*q++ = native(CR); *q++ = native(LF);}
#  define EXE_EXTENSION ".exe"   /* just a guess... */
#endif

/*---------------------------------------------------------------------------
    Mac section:
  ---------------------------------------------------------------------------*/

#ifdef MACOS
#  include "maccfg.h"
#endif /* MACOS */

/*---------------------------------------------------------------------------
    MS-DOS, OS/2, FLEXOS section:
  ---------------------------------------------------------------------------*/

#ifdef WINDLL
#  define MSWIN
#  ifdef MORE
#    undef MORE
#  endif
#  ifdef OS2_EAS
#    undef OS2_EAS
#  endif
#endif

#if (defined(_MSC_VER) || (defined(M_I86) && !defined(__WATCOMC__)))
#  ifndef MSC
#    define MSC               /* This should work for older MSC, too!  */
#  endif
#endif

#if (defined(MSDOS) || defined(OS2) || defined(FLEXOS))
#  include <sys/types.h>      /* off_t, time_t, dev_t, ... */
#  include <sys/stat.h>
#  include <io.h>             /* lseek(), open(), setftime(), dup(), creat() */
#  include <time.h>           /* localtime() */
#  include <fcntl.h>          /* O_BINARY for open() w/o CR/LF translation */

#  ifdef OS2                  /* defined for all OS/2 compilers */
#    include "os2/os2cfg.h"
#  else
#    ifdef FLEXOS
#      include "flexos/flxcfg.h"
#    else
#      include "msdos/doscfg.h"
#    endif
#  endif

#  if (defined(_MSC_VER) && (_MSC_VER == 700) && !defined(GRR))
    /*
     * ARGH.  MSC 7.0 libraries think times are based on 1899 Dec 31 00:00, not
     *  1970 Jan 1 00:00.  So we have to diddle time_t's appropriately:  add or
     *  subtract 70 years' worth of seconds; i.e., number of days times 86400;
     *  i.e., (70*365 regular days + 17 leap days + 1 1899 day) * 86400 ==
     *  (25550 + 17 + 1) * 86400 == 2209075200 seconds.  We know time_t is an
     *  unsigned long (ulg) on the only system with this bug.
     */
#    define TIMET_TO_NATIVE(x)  (x) += (ulg)2209075200L;
#    define NATIVE_TO_TIMET(x)  (x) -= (ulg)2209075200L;
#  endif
#  if (defined(__BORLANDC__) && (__BORLANDC__ >= 0x0450))
#    define timezone      _timezone
#  endif
#  if (defined(__GO32__) || defined(FLEXOS))
#    define DIR_END       '/'
#  else
#    define DIR_END       '\\'  /* OS uses '\\' as directory separator */
#    define DIR_END2      '/'   /* also check for '/' (RTL may convert) */
#  endif
#  ifndef WIN32
#    ifdef DATE_FORMAT
#      undef DATE_FORMAT
#    endif
#    define DATE_FORMAT   dateformat()
#  endif
#  define lenEOL          2
#  define PutNativeEOL    {*q++ = native(CR); *q++ = native(LF);}
#  define USE_EF_UT_TIME
#endif /* MSDOS || OS2 || FLEXOS */

/*---------------------------------------------------------------------------
    MTS section (piggybacks UNIX, I think):
  ---------------------------------------------------------------------------*/

#ifdef MTS
#  include <sys/types.h>      /* off_t, time_t, dev_t, ... */
#  include <sys/stat.h>
#  include <sys/file.h>       /* MTS uses this instead of fcntl.h */
#  include <timeb.h>
#  include <time.h>
#  include <unix.h>           /* some important non-ANSI routines */
#  define mkdir(s,n) (-1)     /* no "make directory" capability */
#  define EBCDIC              /* set EBCDIC conversion on */
#  define NO_STRNICMP         /* unzip's is as good the one in MTS */
#  define USE_FWRITE
#  define close_outfile()  fclose(G.outfile)   /* can't set time on files */
#  define umask(n)            /* don't have umask() on MTS */
#  define FOPWT         "w"   /* open file for writing in TEXT mode */
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_MDY
#  endif
#  define lenEOL        1
#  define PutNativeEOL  *q++ = native(LF);
#endif /* MTS */

 /*---------------------------------------------------------------------------
    QDOS section
  ---------------------------------------------------------------------------*/

#ifdef QDOS
#  define DIRENT
#  include <fcntl.h>
#  include <unistd.h>
#  include <sys/stat.h>
#  include <time.h>
#  include "qdos/izqdos.h"
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_MDY
#  endif
#  define lenEOL        1
#  define PutNativeEOL  *q++ = native(LF);
#  define DIR_END       '_'
#  define RETURN        QReturn
#  undef PATH_MAX
#  define PATH_MAX      36
#  if (!defined(NOTIMESTAMP) && !defined(TIMESTAMP))
#    define TIMESTAMP
#  endif
#endif

/*---------------------------------------------------------------------------
    Tandem NSK section:
  ---------------------------------------------------------------------------*/

#ifdef TANDEM
#  include "tandem.h"
#  include <fcntl.h>
   /* use a single LF delimiter so that writes to 101 text files work */
#  define PutNativeEOL  *q++ = native(LF);
#  define lenEOL        1
#  ifndef DATE_FORMAT
#    define DATE_FORMAT  DF_DMY
#  endif
#  define USE_EF_UT_TIME
#  define RESTORE_UIDGID
#endif

/*---------------------------------------------------------------------------
    TOPS-20 section:
  ---------------------------------------------------------------------------*/

#ifdef TOPS20
#  include <sys/types.h>        /* off_t, time_t, dev_t, ... */
#  include <sys/stat.h>
#  include <sys/param.h>
#  include <sys/time.h>
#  include <sys/timeb.h>
#  include <sys/file.h>
#  include <timex.h>
#  include <monsym.h>           /* get amazing monsym() macro */
   extern int open(), close(), read();
   extern int stat(), unlink(), jsys(), fcntl();
   extern long lseek(), dup(), creat();
#  define strchr    index       /* GRR: necessary? */
#  define strrchr   rindex
#  define REALLY_SHORT_SYMS
#  define NO_MKDIR
#  define DIR_BEG       '<'
#  define DIR_END       '>'
#  define DIR_EXT       ".directory"
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_MDY
#  endif
#  define EXE_EXTENSION ".exe"  /* just a guess... */
#endif /* TOPS20 */

/*---------------------------------------------------------------------------
    Unix section:
  ---------------------------------------------------------------------------*/

#ifdef UNIX
#  include <sys/types.h>       /* off_t, time_t, dev_t, ... */
#  include <sys/stat.h>

#  ifndef COHERENT
#    include <fcntl.h>         /* O_BINARY for open() w/o CR/LF translation */
#  else /* COHERENT */
#    ifdef _I386
#      include <fcntl.h>       /* Coherent 4.0.x, Mark Williams C */
#    else
#      include <sys/fcntl.h>   /* Coherent 3.10, Mark Williams C */
#    endif
#    define SHORT_SYMS
#    ifndef __COHERENT__       /* Coherent 4.2 has tzset() */
#      define tzset  settz
#    endif
#  endif /* ?COHERENT */

#  ifndef NO_PARAM_H
#    ifdef NGROUPS_MAX
#      undef NGROUPS_MAX       /* SCO bug:  defined again in <sys/param.h> */
#    endif
#    ifdef BSD
#      define TEMP_BSD         /* may be defined again in <sys/param.h> */
#      undef BSD
#    endif
#    include <sys/param.h>     /* conflict with <sys/types.h>, some systems? */
#    ifdef TEMP_BSD
#      undef TEMP_BSD
#      ifndef BSD
#        define BSD
#      endif
#    endif
#  endif /* !NO_PARAM_H */

#  ifdef __osf__
#    define DIRENT
#    ifdef BSD
#      undef BSD
#    endif
#  endif /* __osf__ */

#  ifdef BSD
#    include <sys/time.h>
#    include <sys/timeb.h>
#    ifdef _AIX
#      include <time.h>
#    endif
#  else
#    include <time.h>
     struct tm *gmtime(), *localtime();
#  endif

#  if (defined(BSD4_4) || (defined(SYSV) && defined(MODERN)))
#    include <unistd.h>        /* this includes utime.h on SGIs */
#    if (defined(BSD4_4) || defined(linux))
#      include <utime.h>
#      define GOT_UTIMBUF
#    endif
#  endif

#  if (defined(V7) || defined(pyr_bsd))
#    define strchr   index
#    define strrchr  rindex
#  endif
#  ifdef V7
#    define O_RDONLY 0
#    define O_WRONLY 1
#    define O_RDWR   2
#  endif

#  ifdef MINIX
#    include <stdio.h>
#  endif
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_MDY    /* GRR:  customize with locale.h somehow? */
#  endif
#  define lenEOL        1
#  ifdef EBCDIC
#    define PutNativeEOL  *q++ = '\n';
#  else
#    define PutNativeEOL  *q++ = native(LF);
#  endif
#  define SCREENLINES   screenlines()
#  define USE_EF_UT_TIME
#  define SET_DIR_ATTRIB
#  if (!defined(TIMESTAMP) && !defined(NOTIMESTAMP))   /* GRR 970513 */
#    define TIMESTAMP
#  endif
#  define RESTORE_UIDGID
#endif /* UNIX */

/*---------------------------------------------------------------------------
    VM/CMS and MVS section:
  ---------------------------------------------------------------------------*/

#ifdef CMS_MVS
#  include "vmmvs.h"
#  define CLOSE_INFILE()  close_infile(__G)
#endif

/*---------------------------------------------------------------------------
    VMS section:
  ---------------------------------------------------------------------------*/

#ifdef VMS
#  include <types.h>                    /* GRR:  experimenting... */
#  include <stat.h>
#  include <time.h>                     /* the usual non-BSD time functions */
#  include <file.h>                     /* same things as fcntl.h has */
#  include <unixio.h>
#  include <rms.h>
#  define _MAX_PATH (NAM$C_MAXRSS+1)    /* to define FILNAMSIZ below */
#  ifdef RETURN_CODES  /* VMS interprets standard PK return codes incorrectly */
#    define RETURN(ret) return_VMS(__G__ (ret))   /* verbose version */
#    define EXIT(ret)   return_VMS(__G__ (ret))
#  else
#    define RETURN      return_VMS                /* quiet version */
#    define EXIT        return_VMS
#  endif
#  ifdef VMSCLI
#    define USAGE(ret)  VMSCLI_usage(__G__ (ret))
#  endif
#  define DIR_BEG       '['
#  define DIR_END       ']'
#  define DIR_EXT       ".dir"
#  ifndef DATE_FORMAT
#    define DATE_FORMAT DF_MDY
#  endif
#  define lenEOL        1
#  define PutNativeEOL  *q++ = native(LF);
#  define SCREENLINES   screenlines()
#  if (defined(__VMS_VERSION) && !defined(VMS_VERSION))
#    define VMS_VERSION __VMS_VERSION
#  endif
#  if (defined(__VMS_VER) && !defined(__CRTL_VER))
#    define __CRTL_VER __VMS_VER
#  endif
#  if ((!defined(__CRTL_VER)) || (__CRTL_VER < 70000000))
#    define NO_GMTIME           /* gmtime() of earlier VMS C RTLs is broken */
#  else
#    if (!defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME))
#      define USE_EF_UT_TIME
#    endif
#  endif
#  if (!defined(NOTIMESTAMP) && !defined(TIMESTAMP))
#    define TIMESTAMP
#  endif
#  define RESTORE_UIDGID
#endif /* VMS */

/*---------------------------------------------------------------------------
    Win32 (Windows 95/NT) section:
  ---------------------------------------------------------------------------*/

#if (defined(WIN32) && !defined(POCKET_UNZIP))
#  include "w32cfg.h"
#endif





/*************/
/*  Defines  */
/*************/

#define UNZIP_VERSION     20   /* compatible with PKUNZIP 2.0 */
#define VMS_UNZIP_VERSION 42   /* if OS-needed-to-extract is VMS:  can do */

#if (defined(MSDOS) || defined(OS2))
#  define DOS_OS2
#endif

#if (defined(OS2) || defined(WIN32))
#  define OS2_W32
#endif

#if (defined(DOS_OS2) || defined(WIN32))
#  define DOS_OS2_W32
#  define DOS_W32_OS2          /* historical:  don't use */
#endif

#if (defined(DOS_OS2_W32) || defined(__human68k__))
#  define DOS_H68_OS2_W32
#endif

#if (defined(DOS_OS2) || defined(FLEXOS))
#  define DOS_FLX_OS2
#endif

#if (defined(DOS_OS2_W32) || defined(FLEXOS))
#  define DOS_FLX_OS2_W32
#endif

#if (defined(DOS_H68_OS2_W32) || defined(FLEXOS))
#  define DOS_FLX_H68_OS2_W32
#endif

#if (defined(TOPS20) || defined(VMS))
#  define T20_VMS
#endif

#if (defined(MSDOS) || defined(T20_VMS))
#  define DOS_T20_VMS
#endif

/* clean up with a few defaults */
#ifndef DIR_END
#  define DIR_END       '/'     /* last char before program name or filename */
#endif
#ifndef DATE_FORMAT
#  define DATE_FORMAT   DF_MDY  /* defaults to US convention */
#endif
#ifndef CLOSE_INFILE
#  define CLOSE_INFILE()  close(G.zipfd)
#endif
#ifndef RETURN
#  define RETURN        return  /* only used in main() */
#endif
#ifndef EXIT
#  define EXIT          exit
#endif
#ifndef USAGE
#  define USAGE(ret)    usage(__G__ (ret))    /* used in unzip.c, zipinfo.c */
#endif
#ifndef TIMET_TO_NATIVE         /* everybody but MSC 7.0 and Macintosh */
#  define TIMET_TO_NATIVE(x)
#  define NATIVE_TO_TIMET(x)
#endif

#if (defined(DOS_FLX_OS2_W32) || defined(UNIX) || defined(RISCOS))
#  ifndef HAVE_UNLINK
#    define HAVE_UNLINK
#  endif
#endif
#if (defined(AOS_VS) || defined(ATARI) || defined(__BEOS__)) /* GRR: others? */
#  ifndef HAVE_UNLINK
#    define HAVE_UNLINK
#  endif
#endif

/* OS-specific exceptions to the "ANSI <--> INT_SPRINTF" rule */

#if (!defined(PCHAR_SPRINTF) && !defined(INT_SPRINTF))
#  if (defined(SYSV) || defined(CONVEX) || defined(NeXT) || defined(BSD4_4))
#    define INT_SPRINTF      /* sprintf() returns int:  SysVish/Posix */
#  endif
#  if (defined(DOS_FLX_OS2_W32) || defined(VMS) || defined(AMIGA))
#    define INT_SPRINTF      /* sprintf() returns int:  ANSI */
#  endif
#  if (defined(ultrix) || defined(__ultrix)) /* Ultrix 4.3 and newer */
#    if (defined(POSIX) || defined(__POSIX))
#      define INT_SPRINTF    /* sprintf() returns int:  ANSI/Posix */
#    endif
#    ifdef __GNUC__
#      define PCHAR_SPRINTF  /* undetermined actual return value */
#    endif
#  endif
#  if (defined(__osf__) || defined(_AIX) || defined(CMS_MVS))
#    define INT_SPRINTF      /* sprintf() returns int:  ANSI/Posix */
#  endif
#  if defined(sun)
#    define PCHAR_SPRINTF    /* sprintf() returns char *:  SunOS cc *and* gcc */
#  endif
#endif

/* defaults that we hope will take care of most machines in the future */

#if (!defined(PCHAR_SPRINTF) && !defined(INT_SPRINTF))
#  ifdef __STDC__
#    define INT_SPRINTF      /* sprintf() returns int:  ANSI */
#  endif
#  ifndef INT_SPRINTF
#    define PCHAR_SPRINTF    /* sprintf() returns char *:  BSDish */
#  endif
#endif

#define MSG_STDERR(f)  (f & 1)        /* bit 0:  0 = stdout, 1 = stderr */
#define MSG_INFO(f)    ((f & 6) == 0) /* bits 1 and 2:  0 = info */
#define MSG_WARN(f)    ((f & 6) == 2) /* bits 1 and 2:  1 = warning */
#define MSG_ERROR(f)   ((f & 6) == 4) /* bits 1 and 2:  2 = error */
#define MSG_FATAL(f)   ((f & 6) == 6) /* bits 1 and 2:  (3 = fatal error) */
#define MSG_ZFN(f)     (f & 0x0008)   /* bit 3:  1 = print zipfile name */
#define MSG_FN(f)      (f & 0x0010)   /* bit 4:  1 = print filename */
#define MSG_LNEWLN(f)  (f & 0x0020)   /* bit 5:  1 = leading newline if !SOL */
#define MSG_TNEWLN(f)  (f & 0x0040)   /* bit 6:  1 = trailing newline if !SOL */
#define MSG_MNEWLN(f)  (f & 0x0080)   /* bit 7:  1 = trailing NL for prompts */
/* the following are subject to change */
#define MSG_NO_WGUI(f) (f & 0x0100)   /* bit 8:  1 = skip if Windows GUI */
#define MSG_NO_AGUI(f) (f & 0x0200)   /* bit 9:  1 = skip if Acorn GUI */
#define MSG_NO_DLL2(f) (f & 0x0400)   /* bit 10:  1 = skip if OS/2 DLL */
#define MSG_NO_NDLL(f) (f & 0x0800)   /* bit 11:  1 = skip if WIN32 DLL */
#define MSG_NO_WDLL(f) (f & 0x1000)   /* bit 12:  1 = skip if Windows DLL */

#if (defined(MORE) && !defined(SCREENLINES))
#  ifdef DOS_FLX_OS2_W32
#    define SCREENLINES 25  /* can be (should be) a function instead */
#  else
#    define SCREENLINES 24  /* VT-100s are assumed to be minimal hardware */
#  endif
#endif

#define DIR_BLKSIZ  64      /* number of directory entries per block
                             *  (should fit in 4096 bytes, usually) */
#ifndef WSIZE
#  define WSIZE     0x8000  /* window size--must be a power of two, and */
#endif                      /*  at least 32K for zip's deflate method */

#ifndef INBUFSIZ
#  if (defined(MED_MEM) || defined(SMALL_MEM))
#    define INBUFSIZ  2048  /* works for MS-DOS small model */
#  else
#    define INBUFSIZ  8192  /* larger buffers for real OSes */
#  endif
#endif

#ifndef __16BIT__
#  define nearmalloc  malloc
#  define nearfree    free
#  if (!defined(__IBMC__) || !defined(OS2))
#    ifndef near
#      define near
#    endif
#    ifndef far
#      define far
#    endif
#  endif
#endif

#if (defined(DYNALLOC_CRCTAB) && !defined(DYNAMIC_CRC_TABLE))
#  undef DYNALLOC_CRCTAB
#endif

#if (defined(DYNALLOC_CRCTAB) && defined(REENTRANT))
#  undef DYNALLOC_CRCTAB   /* not safe with reentrant code */
#endif

#if (defined(USE_ZLIB) && !defined(USE_OWN_CRCTAB))
#  ifdef DYNALLOC_CRCTAB
#    undef DYNALLOC_CRCTAB
#  endif
#endif

#if (defined(USE_ZLIB) && defined(ASM_CRC))
#  undef ASM_CRC
#endif

/* Logic for case of small memory, length of EOL > 1:  if OUTBUFSIZ == 2048,
 * OUTBUFSIZ>>1 == 1024 and OUTBUFSIZ>>7 == 16; therefore rawbuf is 1008 bytes
 * and transbuf 1040 bytes.  Have room for 32 extra EOL chars; 1008/32 == 31.5
 * chars/line, smaller than estimated 35-70 characters per line for C source
 * and normal text.  Hence difference is sufficient for most "average" files.
 * (Argument scales for larger OUTBUFSIZ.)
 */
#ifdef SMALL_MEM          /* i.e., 16-bit OSes:  MS-DOS, OS/2 1.x, etc. */
#  define LoadFarString(x)       fLoadFarString(__G__ (x))
#  define LoadFarStringSmall(x)  fLoadFarStringSmall(__G__ (x))
#  define LoadFarStringSmall2(x) fLoadFarStringSmall2(__G__ (x))
#  if (defined(_MSC_VER) && (_MSC_VER >= 600))
#    define zfstrcpy(dest, src)  _fstrcpy((dest), (src))
#  endif
#  ifndef Far
#    define Far far  /* __far only works for MSC 6.00, not 6.0a or Borland */
#  endif
#  define OUTBUFSIZ INBUFSIZ
#  if (lenEOL == 1)
#    define RAWBUFSIZ (OUTBUFSIZ>>1)
#  else
#    define RAWBUFSIZ ((OUTBUFSIZ>>1) - (OUTBUFSIZ>>7))
#  endif
#  define TRANSBUFSIZ (OUTBUFSIZ-RAWBUFSIZ)
   typedef short  shrint;            /* short/int or "shrink int" (unshrink) */
#else
#  define zfstrcpy(dest, src)       strcpy((dest), (src))
#  ifdef QDOS
#    define LoadFarString(x)        Qstrfix(x)   /* fix up _ for '.' */
#    define LoadFarStringSmall(x)   Qstrfix(x)
#    define LoadFarStringSmall2(x)  Qstrfix(x)
#  else
#    define LoadFarString(x)        x
#    define LoadFarStringSmall(x)   x
#    define LoadFarStringSmall2(x)  x
#  endif
#  ifdef MED_MEM
#    define OUTBUFSIZ 0xFF80         /* can't malloc arrays of 0xFFE8 or more */
#    define TRANSBUFSIZ 0xFF80
     typedef short  shrint;
#  else
#    define OUTBUFSIZ (lenEOL*WSIZE) /* more efficient text conversion */
#    define TRANSBUFSIZ (lenEOL*OUTBUFSIZ)
#    ifdef AMIGA
       typedef short shrint;
#    else
       typedef int  shrint;          /* for efficiency/speed, we hope... */
#    endif
#  endif /* ?MED_MEM */
#  define RAWBUFSIZ OUTBUFSIZ
#endif /* ?SMALL_MEM */

#ifndef Far
#  define Far
#endif

#ifndef MAIN
#  define MAIN  main
#endif

#ifdef SFX      /* disable some unused features for SFX executables */
#  ifndef NO_ZIPINFO
#    define NO_ZIPINFO
#  endif
#  ifdef TIMESTAMP
#    undef TIMESTAMP
#  endif
#endif

/* user may have defined both by accident...  NOTIMESTAMP takes precedence */
#if (defined(TIMESTAMP) && defined(NOTIMESTAMP))
#  undef TIMESTAMP
#endif

#if (!defined(COPYRIGHT_CLEAN) && !defined(USE_SMITH_CODE))
#  define COPYRIGHT_CLEAN
#endif

#if (!defined(LZW_CLEAN) && !defined(USE_UNSHRINK))
#  define LZW_CLEAN
#endif

#ifndef O_BINARY
#  define O_BINARY  0
#endif

#ifndef PIPE_ERROR
#  define PIPE_ERROR (errno == EPIPE)
#endif

/* File operations--use "b" for binary if allowed or fixed length 512 on VMS */
#ifdef VMS
#  define FOPR  "r","ctx=stm"
#  define FOPM  "r+","ctx=stm","rfm=fix","mrs=512"
#  define FOPW  "w","ctx=stm","rfm=fix","mrs=512"
#endif /* VMS */

#ifdef CMS_MVS
/* Binary files must be RECFM=F,LRECL=1 for ftell() to get correct pos */
/* ...unless byteseek is used.  Let's try that for a while.            */
#  define FOPR "rb,byteseek"
#  define FOPM "r+b,byteseek"
#  define FOPW "wb,recfm=v,lrecl=32760"
#  ifdef MVS
#    define FOPWT "w,lrecl=133"
#  else
#    define FOPWT "w"
#  endif
#endif /* CMS_MVS */

#ifdef TOPS20          /* TOPS-20 MODERN?  You kidding? */
#  define FOPW "w8"
#endif /* TOPS20 */

/* Defaults when nothing special has been defined previously. */
#ifdef MODERN
#  ifndef FOPR
#    define FOPR "rb"
#  endif
#  ifndef FOPM
#    define FOPM "r+b"
#  endif
#  ifndef FOPW
#    define FOPW "wb"
#  endif
#  ifndef FOPWT
#    define FOPWT "wt"
#  endif
#else /* !MODERN */
#  ifndef FOPR
#    define FOPR "r"
#  endif
#  ifndef FOPM
#    define FOPM "r+"
#  endif
#  ifndef FOPW
#    define FOPW "w"
#  endif
#  ifndef FOPWT
#    define FOPWT "w"
#  endif
#endif /* ?MODERN */

/*
 * If <limits.h> exists on most systems, should include that, since it may
 * define some or all of the following:  NAME_MAX, PATH_MAX, _POSIX_NAME_MAX,
 * _POSIX_PATH_MAX.
 */
#ifdef DOS_FLX_OS2
#  include <limits.h>
#endif

#ifndef PATH_MAX
#  ifdef MAXPATHLEN
#    define PATH_MAX      MAXPATHLEN    /* in <sys/param.h> on some systems */
#  else
#    ifdef _MAX_PATH
#      define PATH_MAX    _MAX_PATH
#    else
#      if FILENAME_MAX > 255
#        define PATH_MAX  FILENAME_MAX  /* used like PATH_MAX on some systems */
#      else
#        define PATH_MAX  1024
#      endif
#    endif /* ?_MAX_PATH */
#  endif /* ?MAXPATHLEN */
#endif /* !PATH_MAX */

#define FILNAMSIZ  PATH_MAX

#ifdef SHORT_SYMS                   /* Mark Williams C, ...? */
#  define extract_or_test_files     xtr_or_tst_files
#  define extract_or_test_member    xtr_or_tst_member
#endif

#ifdef REALLY_SHORT_SYMS            /* TOPS-20 linker:  first 6 chars */
#  define process_cdir_file_hdr     XXpcdfh
#  define process_local_file_hdr    XXplfh
#  define extract_or_test_files     XXxotf  /* necessary? */
#  define extract_or_test_member    XXxotm  /* necessary? */
#  define check_for_newer           XXcfn
#  define overwrite_all             XXoa
#  define process_all_files         XXpaf
#  define extra_field               XXef
#  define explode_lit8              XXel8
#  define explode_lit4              XXel4
#  define explode_nolit8            XXnl8
#  define explode_nolit4            XXnl4
#  define cpdist8                   XXcpdist8
#  define inflate_codes             XXic
#  define inflate_stored            XXis
#  define inflate_fixed             XXif
#  define inflate_dynamic           XXid
#  define inflate_block             XXib
#  define maxcodemax                XXmax
#endif

#ifndef S_TIME_T_MAX            /* max value of signed (>= 32-bit) time_t */
#  define S_TIME_T_MAX  ((time_t)(ulg)0x7fffffffL)
#endif
#ifndef U_TIME_T_MAX            /* max value of unsigned (>= 32-bit) time_t */
#  define U_TIME_T_MAX  ((time_t)(ulg)0xffffffffL)
#endif
#ifdef DOSTIME_MINIMUM          /* min DOSTIME value (1980-01-01) */
#  undef DOSTIME_MINIMUM
#endif
#define DOSTIME_MINIMUM ((ulg)0x00210000L)
#ifdef DOSTIME_2038_01_18       /* approximate DOSTIME equivalent of */
#  undef DOSTIME_2038_01_18     /*  the signed-32-bit time_t limit */
#endif
#define DOSTIME_2038_01_18 ((ulg)0x74320000L)

#ifdef QDOS
#  define ZSUFX         "_zip"
#  define ALT_ZSUFX     ".zip"
#else
#  ifdef RISCOS
#    define ZSUFX       "/zip"
#  else
#    define ZSUFX       ".zip"
#  endif
#  define ALT_ZSUFX     ".ZIP"   /* Unix-only so far (only case-sensitive fs) */
#endif

#define CENTRAL_HDR_SIG   "\001\002"   /* the infamous "PK" signature bytes, */
#define LOCAL_HDR_SIG     "\003\004"   /*  sans "PK" (so unzip executable not */
#define END_CENTRAL_SIG   "\005\006"   /*  mistaken for zipfile itself) */
#define EXTD_LOCAL_SIG    "\007\010"   /* [ASCII "\113" == EBCDIC "\080" ??] */

/* choice of activities for do_string() */
#define SKIP              0             /* skip header block */
#define DISPLAY           1             /* display archive comment (ASCII) */
#define DISPL_8           5             /* display file comment (ext. ASCII) */
#define DS_FN             2             /* read filename (ext. ASCII) */
#define EXTRA_FIELD       3             /* copy extra field into buffer */
#define DS_EF             3
#ifdef AMIGA
#  define FILENOTE        4
#endif

#define DOES_NOT_EXIST    -1   /* return values for check_for_newer() */
#define EXISTS_AND_OLDER  0
#define EXISTS_AND_NEWER  1

#define ROOT              0    /* checkdir() extract-to path:  called once */
#define INIT              1    /* allocate buildpath:  called once per member */
#define APPEND_DIR        2    /* append a dir comp.:  many times per member */
#define APPEND_NAME       3    /* append actual filename:  once per member */
#define GETPATH           4    /* retrieve the complete path and free it */
#define END               5    /* free root path prior to exiting program */

/* version_made_by codes (central dir):  make sure these */
/*  are not defined on their respective systems!! */
#define FS_FAT_           0    /* filesystem used by MS-DOS, OS/2, Win32 */
#define AMIGA_            1
#define VMS_              2
#define UNIX_             3
#define VM_CMS_           4
#define ATARI_            5    /* what if it's a minix filesystem? [cjh] */
#define FS_HPFS_          6    /* filesystem used by OS/2 (and NT 3.x) */
#define MAC_              7    /* HFS filesystem used by MacOS */
#define Z_SYSTEM_         8
#define CPM_              9
#define TOPS20_           10
#define FS_NTFS_          11   /* filesystem used by Windows NT */
#define QDOS_             12
#define ACORN_            13   /* Archimedes Acorn RISC OS */
#define FS_VFAT_          14   /* filesystem used by Windows 95, NT */
#define MVS_              15
#define BEOS_             16   /* hybrid POSIX/database filesystem */
#define TANDEM_           17   /* Tandem/NSK */
#define NUM_HOSTS         18   /* index of last system + 1 */

#define STORED            0    /* compression methods */
#define SHRUNK            1
#define REDUCED1          2
#define REDUCED2          3
#define REDUCED3          4
#define REDUCED4          5
#define IMPLODED          6
#define TOKENIZED         7
#define DEFLATED          8
#define ENHDEFLATED       9
#define DCLIMPLODED      10
#define NUM_METHODS      11    /* index of last method + 1 */
/* don't forget to update list_files(), extract.c and zipinfo.c appropriately
 * if NUM_METHODS changes */

/* (the PK-class error codes are public and have been moved into unzip.h) */

#define DF_MDY            0    /* date format 10/26/91 (USA only) */
#define DF_DMY            1    /* date format 26/10/91 (most of the world) */
#define DF_YMD            2    /* date format 91/10/26 (a few countries) */

/*---------------------------------------------------------------------------
    Extra-field block ID values and offset info.
  ---------------------------------------------------------------------------*/
/* extra-field ID values, all little-endian: */
#define EF_AV        0x0007    /* PKWARE's authenticity verification */
#define EF_OS2       0x0009    /* OS/2 extended attributes */
#define EF_PKW32     0x000a    /* PKWARE's Win95/98/WinNT filetimes */
#define EF_PKVMS     0x000c    /* PKWARE's VMS */
#define EF_PKUNIX    0x000d    /* PKWARE's Unix */
#define EF_IZVMS     0x4d49    /* Info-ZIP's VMS ("IM") */
#define EF_IZUNIX    0x5855    /* Info-ZIP's old Unix[1] ("UX") */
#define EF_IZUNIX2   0x7855    /* Info-ZIP's new Unix[2] ("Ux") */
#define EF_TIME      0x5455    /* universal timestamp ("UT") */
#define EF_MAC3      0x334d    /* Info-ZIP's new Macintosh (= "M3") */
#define EF_JLMAC     0x07c8    /* Johnny Lee's old Macintosh (= 1992) */
#define EF_ZIPIT     0x2605    /* Thomas Brown's Macintosh (ZipIt) */
#define EF_ZIPIT2    0x2705    /* T. Brown's Mac (ZipIt) v 1.3.8 and newer ? */
#define EF_VMCMS     0x4704    /* Info-ZIP's VM/CMS ("\004G") */
#define EF_MVS       0x470f    /* Info-ZIP's MVS ("\017G") */
#define EF_ACL       0x4c41    /* (OS/2) access control list ("AL") */
#define EF_NTSD      0x4453    /* NT security descriptor ("SD") */
#define EF_BEOS      0x6542    /* BeOS ("Be") */
#define EF_QDOS      0xfb4a    /* SMS/QDOS ("J\373") */
#define EF_AOSVS     0x5356    /* AOS/VS ("VS") */
#define EF_SPARK     0x4341    /* David Pilling's Acorn/SparkFS ("AC") */
#define EF_MD5       0x4b46    /* Fred Kantor's MD5 ("FK") */
#define EF_ASIUNIX   0x756e    /* ASi's Unix ("nu") */

#define EB_HEADSIZE       4    /* length of extra field block header */
#define EB_ID             0    /* offset of block ID in header */
#define EB_LEN            2    /* offset of data length field in header */
#define EB_UCSIZE_P       0    /* offset of ucsize field in compr. data */
#define EB_CMPRHEADLEN    6    /* lenght of compression header */

#define EB_UX_MINLEN      8    /* minimal "UX" field contains atime, mtime */
#define EB_UX_FULLSIZE    12   /* full "UX" field (atime, mtime, uid, gid) */
#define EB_UX_ATIME       0    /* offset of atime in "UX" extra field data */
#define EB_UX_MTIME       4    /* offset of mtime in "UX" extra field data */
#define EB_UX_UID         8    /* byte offset of UID in "UX" field data */
#define EB_UX_GID         10   /* byte offset of GID in "UX" field data */

#define EB_UX2_MINLEN     4    /* minimal "Ux" field contains UID/GID */
#define EB_UX2_UID        0    /* byte offset of UID in "Ux" field data */
#define EB_UX2_GID        2    /* byte offset of GID in "Ux" field data */
#define EB_UX2_VALID      (1 << 8)      /* UID/GID present */

#define EB_UT_MINLEN      1    /* minimal UT field contains Flags byte */
#define EB_UT_FLAGS       0    /* byte offset of Flags field */
#define EB_UT_TIME1       1    /* byte offset of 1st time value */
#define EB_UT_FL_MTIME    (1 << 0)      /* mtime present */
#define EB_UT_FL_ATIME    (1 << 1)      /* atime present */
#define EB_UT_FL_CTIME    (1 << 2)      /* ctime present */

#define EB_FLGS_OFFS      4    /* offset of flags area in generic compressed
                                  extra field blocks (OS2, NT, and others) */
#define EB_OS2_HLEN       4    /* size of OS2/ACL compressed data header */
#define EB_BEOS_HLEN      5    /* length of BeOS e.f attribute header */
#define EB_BE_FL_UNCMPR   0x01 /* "BeOS attributes uncompressed" bit flag */
#define EB_MAC3_HLEN      14   /* length of Mac3 attribute block header */
#define EB_M3_FL_DATFRK   0x01 /* "this entry is data fork" flag */
#define EB_M3_FL_UNCMPR   0x04 /* "Mac3 attributes uncompressed" bit flag */
#define EB_M3_FL_TIME64   0x08 /* "Mac3 time fields are 64 bit wide" flag */
#define EB_M3_FL_NOUTC    0x10 /* "Mac3 timezone offset fields missing" flag */

#define EB_NTSD_C_LEN     4    /* length of central NT security data */
#define EB_NTSD_L_LEN     5    /* length of minimal local NT security data */
#define EB_NTSD_VERSION   4    /* offset of NTSD version byte */
#define EB_NTSD_MAX_VER   (0)  /* maximum version # we know how to handle */

#define EB_ASI_CRC32      0    /* offset of ASI Unix field's crc32 checksum */
#define EB_ASI_MODE       4    /* offset of ASI Unix permission mode field */

/*---------------------------------------------------------------------------
    True sizes of the various headers, as defined by PKWARE--so it is not
    likely that these will ever change.  But if they do, make sure both these
    defines AND the typedefs below get updated accordingly.
  ---------------------------------------------------------------------------*/
#define LREC_SIZE   26   /* lengths of local file headers, central */
#define CREC_SIZE   42   /*  directory headers, and the end-of-    */
#define ECREC_SIZE  18   /*  central-dir record, respectively      */

#define MAX_BITS    13                 /* used in unshrink() */
#define HSIZE       (1 << MAX_BITS)    /* size of global work area */

#define LF     10        /* '\n' on ASCII machines; must be 10 due to EBCDIC */
#define CR     13        /* '\r' on ASCII machines; must be 13 due to EBCDIC */
#define CTRLZ  26        /* DOS & OS/2 EOF marker (used in fileio.c, vms.c) */

#ifdef EBCDIC
#  define foreign(c)    ascii[(uch)(c)]
#  define native(c)     ebcdic[(uch)(c)]
#  define NATIVE        "EBCDIC"
#  define NOANSIFILT
#endif

#if (defined(CRAY) && defined(ZMEM))
#  undef ZMEM
#endif

#ifdef ZMEM
#  undef ZMEM
#  define memcmp(b1,b2,len)      bcmp(b2,b1,len)
#  define memcpy(dest,src,len)   bcopy(src,dest,len)
#  define memzero                bzero
#else
#  define memzero(dest,len)      memset(dest,0,len)
#endif

#ifdef VMS
#  define ENV_UNZIP       "UNZIP_OPTS"     /* names of environment variables */
#  define ENV_ZIPINFO     "ZIPINFO_OPTS"
#endif /* VMS */
#ifdef RISCOS
#  define ENV_UNZIP       "Unzip$Options"
#  define ENV_ZIPINFO     "Zipinfo$Options"
#  define ENV_UNZIPEXTS   "Unzip$Exts"
#endif /* RISCOS */
#ifndef ENV_UNZIP
#  define ENV_UNZIP       "UNZIP"          /* the standard names */
#  define ENV_ZIPINFO     "ZIPINFO"
#endif
#define ENV_UNZIP2        "UNZIPOPT"     /* alternate names, for zip compat. */
#define ENV_ZIPINFO2      "ZIPINFOOPT"

#if (!defined(QQ) && !defined(NOQQ))
#  define QQ
#endif

#ifdef QQ                         /* Newtware version:  no file */
#  define QCOND     (!uO.qflag)   /*  comments with -vq or -vqq */
#else                             /* Bill Davidsen version:  no way to */
#  define QCOND     (longhdr)     /*  kill file comments when listing */
#endif

#ifdef OLD_QQ
#  define QCOND2    (uO.qflag < 2)
#else
#  define QCOND2    (!uO.qflag)
#endif

#ifndef TRUE
#  define TRUE      1   /* sort of obvious */
#endif
#ifndef FALSE
#  define FALSE     0
#endif

#ifndef SEEK_SET
#  define SEEK_SET  0
#  define SEEK_CUR  1
#  define SEEK_END  2
#endif

#if (defined(UNIX) && defined(S_IFLNK) && !defined(MTS))
#  define SYMLINKS
#  ifndef S_ISLNK
#    define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#  endif
#endif /* UNIX && S_IFLNK && !MTS */

#ifndef S_ISDIR
#  ifdef CMS_MVS
#    define S_ISDIR(m)  (FALSE)
#  else
#    define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
# endif
#endif

#ifndef IS_VOLID
#  define IS_VOLID(m)  ((m) & 0x08)
#endif





/**************/
/*  Typedefs  */
/**************/

#ifdef NO_UID_GID
#  ifdef UID_USHORT
     typedef unsigned short  uid_t;    /* TI SysV.3 */
     typedef unsigned short  gid_t;
#  else
     typedef unsigned int    uid_t;    /* SCO Xenix */
     typedef unsigned int    gid_t;
#  endif
#endif

#if (defined(WIN32) || defined(sgi) || defined(GOT_UTIMBUF) || defined(ATARI))
   typedef struct utimbuf ztimbuf;
#else
   typedef struct ztimbuf {
       time_t actime;        /* new access time */
       time_t modtime;       /* new modification time */
   } ztimbuf;
#endif

typedef struct iztimes {
   time_t atime;             /* new access time */
   time_t mtime;             /* new modification time */
   time_t ctime;             /* used for creation time; NOT same as st_ctime */
} iztimes;

#ifdef SET_DIR_ATTRIB
   typedef struct dirtime {  /* temporary struct for holding directory info */
       struct dirtime *next; /*  until can be sorted and set at end */
       char *fn;             /* filename of directory */
       union {
           iztimes t3;       /* mtime, atime, ctime */
           ztimbuf t2;       /* modtime, actime */
       } u;
       unsigned perms;       /* same as min_info.file_attr */
       int have_uidgid;      /* flag */
       ush uidgid[2];
   } dirtime;
#endif /* SET_DIR_ATTRIB */

typedef struct min_info {
    long offset;
    ulg crc;                 /* crc (needed if extended header) */
    ulg compr_size;          /* compressed size (needed if extended header) */
    ulg uncompr_size;        /* uncompressed size (needed if extended header) */
    int hostnum;
    unsigned file_attr;      /* local flavor, as used by creat(), chmod()... */
    unsigned encrypted : 1;  /* file encrypted: decrypt before uncompressing */
    unsigned ExtLocHdr : 1;  /* use time instead of CRC for decrypt check */
    unsigned textfile : 1;   /* file is text (according to zip) */
    unsigned textmode : 1;   /* file is to be extracted as text */
    unsigned lcflag : 1;     /* convert filename to lowercase */
    unsigned vollabel : 1;   /* "file" is an MS-DOS volume (disk) label */
} min_info;

typedef struct VMStimbuf {
    char *revdate;    /* (both roughly correspond to Unix modtime/st_mtime) */
    char *credate;
} VMStimbuf;

/*---------------------------------------------------------------------------
    Zipfile work area declarations.
  ---------------------------------------------------------------------------*/

#ifdef MALLOC_WORK
   union work {
     struct {                 /* unshrink(): */
       shrint *Parent;          /* (8193 * sizeof(shrint)) */
       uch *value;
       uch *Stack;
     } shrink;
     uch *Slide;              /* explode(), inflate(), unreduce() */
   };
#else /* !MALLOC_WORK */
   union work {
     struct {                 /* unshrink(): */
       shrint Parent[HSIZE];    /* (8192 * sizeof(shrint)) == 16KB minimum */
       uch value[HSIZE];        /* 8KB */
       uch Stack[HSIZE];        /* 8KB */
     } shrink;                  /* total = 32KB minimum; 80KB on Cray/Alpha */
     uch Slide[WSIZE];        /* explode(), inflate(), unreduce() */
   };
#endif /* ?MALLOC_WORK */

#define slide  G.area.Slide

#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
#  define redirSlide G.redirect_sldptr
#else
#  define redirSlide G.area.Slide
#endif

/*---------------------------------------------------------------------------
    Zipfile layout declarations.  If these headers ever change, make sure the
    xxREC_SIZE defines (above) change with them!
  ---------------------------------------------------------------------------*/

   typedef uch   local_byte_hdr[ LREC_SIZE ];
#      define L_VERSION_NEEDED_TO_EXTRACT_0     0
#      define L_VERSION_NEEDED_TO_EXTRACT_1     1
#      define L_GENERAL_PURPOSE_BIT_FLAG        2
#      define L_COMPRESSION_METHOD              4
#      define L_LAST_MOD_DOS_DATETIME           6
#      define L_CRC32                           10
#      define L_COMPRESSED_SIZE                 14
#      define L_UNCOMPRESSED_SIZE               18
#      define L_FILENAME_LENGTH                 22
#      define L_EXTRA_FIELD_LENGTH              24

   typedef uch   cdir_byte_hdr[ CREC_SIZE ];
#      define C_VERSION_MADE_BY_0               0
#      define C_VERSION_MADE_BY_1               1
#      define C_VERSION_NEEDED_TO_EXTRACT_0     2
#      define C_VERSION_NEEDED_TO_EXTRACT_1     3
#      define C_GENERAL_PURPOSE_BIT_FLAG        4
#      define C_COMPRESSION_METHOD              6
#      define C_LAST_MOD_DOS_DATETIME           8
#      define C_CRC32                           12
#      define C_COMPRESSED_SIZE                 16
#      define C_UNCOMPRESSED_SIZE               20
#      define C_FILENAME_LENGTH                 24
#      define C_EXTRA_FIELD_LENGTH              26
#      define C_FILE_COMMENT_LENGTH             28
#      define C_DISK_NUMBER_START               30
#      define C_INTERNAL_FILE_ATTRIBUTES        32
#      define C_EXTERNAL_FILE_ATTRIBUTES        34
#      define C_RELATIVE_OFFSET_LOCAL_HEADER    38

   typedef uch   ec_byte_rec[ ECREC_SIZE+4 ];
/*     define SIGNATURE                         0   space-holder only */
#      define NUMBER_THIS_DISK                  4
#      define NUM_DISK_WITH_START_CENTRAL_DIR   6
#      define NUM_ENTRIES_CENTRL_DIR_THS_DISK   8
#      define TOTAL_ENTRIES_CENTRAL_DIR         10
#      define SIZE_CENTRAL_DIRECTORY            12
#      define OFFSET_START_CENTRAL_DIRECTORY    16
#      define ZIPFILE_COMMENT_LENGTH            20


   typedef struct local_file_header {                 /* LOCAL */
       uch version_needed_to_extract[2];
       ush general_purpose_bit_flag;
       ush compression_method;
       ulg last_mod_dos_datetime;
       ulg crc32;
       ulg csize;
       ulg ucsize;
       ush filename_length;
       ush extra_field_length;
   } local_file_hdr;

#if 0
   typedef struct central_directory_file_header {     /* CENTRAL */
       uch version_made_by[2];
       uch version_needed_to_extract[2];
       ush general_purpose_bit_flag;
       ush compression_method;
       ulg last_mod_dos_datetime;
       ulg crc32;
       ulg csize;
       ulg ucsize;
       ush filename_length;
       ush extra_field_length;
       ush file_comment_length;
       ush disk_number_start;
       ush internal_file_attributes;
       ulg external_file_attributes;
       ulg relative_offset_local_header;
   } cdir_file_hdr;
#endif /* 0 */

   typedef struct end_central_dir_record {            /* END CENTRAL */
       ush number_this_disk;
       ush num_disk_start_cdir;
       ush num_entries_centrl_dir_ths_disk;
       ush total_entries_central_dir;
       ulg size_central_directory;
       ulg offset_start_central_directory;
       ush zipfile_comment_length;
   } ecdir_rec;


/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model).
   Valid extra bits are 0..13.  e == 15 is EOB (end of block), e == 16
   means that v is a literal, 16 < e < 32 means that v is a pointer to
   the next table, which codes e - 16 bits, and lastly e == 99 indicates
   an unused code.  If a code with e == 99 is looked up, this implies an
   error in the data. */

struct huft {
    uch e;                /* number of extra bits or operation */
    uch b;                /* number of bits in this code or subcode */
    union {
        ush n;            /* literal, length base, or distance base */
        struct huft *t;   /* pointer to next level of table */
    } v;
};


typedef struct _APIDocStruct {
    char *compare;
    char *function;
    char *syntax;
    char *purpose;
} APIDocStruct;




/*************/
/*  Globals  */
/*************/

#if (defined(OS2) && !defined(FUNZIP))
#  include "os2/os2data.h"
#endif

#include "globals.h"



/*************************/
/*  Function Prototypes  */
/*************************/

/*---------------------------------------------------------------------------
    Functions in unzip.c (initialization routines):
  ---------------------------------------------------------------------------*/

#ifndef WINDLL
   int    MAIN                   OF((int argc, char **argv));
   int    unzip                  OF((__GPRO__ int argc, char **argv));
   int    uz_opts                OF((__GPRO__ int *pargc, char ***pargv));
   int    usage                  OF((__GPRO__ int error));
#endif /* !WINDLL */

/*---------------------------------------------------------------------------
    Functions in process.c (main driver routines):
  ---------------------------------------------------------------------------*/

int      process_zipfiles        OF((__GPRO));
void     free_G_buffers          OF((__GPRO));
/* static int    do_seekable     OF((__GPRO__ int lastchance)); */
/* static int    find_ecrec      OF((__GPRO__ long searchlen)); */
int      uz_end_central          OF((__GPRO));
int      process_cdir_file_hdr   OF((__GPRO));
int      get_cdir_ent            OF((__GPRO));
int      process_local_file_hdr  OF((__GPRO));
unsigned ef_scan_for_izux        OF((uch *ef_buf, unsigned ef_len, int ef_is_c,
                                     ulg dos_mdatetime,
                                     iztimes *z_utim, ush *z_uidgid));

#ifndef SFX

/*---------------------------------------------------------------------------
    Functions in zipinfo.c (`zipinfo-style' listing routines):
  ---------------------------------------------------------------------------*/

#ifndef NO_ZIPINFO
#ifndef WINDLL
   int   zi_opts                 OF((__GPRO__ int *pargc, char ***pargv));
#endif
int      zi_end_central          OF((__GPRO));
int      zipinfo                 OF((__GPRO));
/* static int      zi_long       OF((__GPRO__ ulg *pEndprev)); */
/* static int      zi_short      OF((__GPRO)); */
/* static char    *zi_time       OF((__GPRO__ ZCONST ulg *datetimez,
                                     ZCONST time_t *modtimez, char *d_t_str));*/
#endif /* !NO_ZIPINFO */

/*---------------------------------------------------------------------------
    Functions in list.c (generic zipfile-listing routines):
  ---------------------------------------------------------------------------*/

int      list_files              OF((__GPRO));
#ifdef TIMESTAMP
   int   get_time_stamp          OF((__GPRO__  time_t *last_modtime,
                                     unsigned *nmember));
#endif
int      ratio                   OF((ulg uc, ulg c));
void     fnprint                 OF((__GPRO));

#endif /* !SFX */

/*---------------------------------------------------------------------------
    Functions in fileio.c:
  ---------------------------------------------------------------------------*/

int      open_input_file      OF((__GPRO));
int      open_outfile         OF((__GPRO));                    /* also vms.c */
void     undefer_input        OF((__GPRO));
void     defer_leftover_input OF((__GPRO));
unsigned readbuf              OF((__GPRO__ char *buf, register unsigned len));
int      readbyte             OF((__GPRO));
int      fillinbuf            OF((__GPRO));
#ifdef FUNZIP
   int   flush                OF((__GPRO__ ulg size));  /* actually funzip.c */
#else
   int   flush                OF((__GPRO__ uch *buf, ulg size, int unshrink));
#endif
/* static int  disk_error     OF((__GPRO)); */
void     handler              OF((int signal));
time_t   dos_to_unix_time     OF((ulg dos_datetime));
int      check_for_newer      OF((__GPRO__ char *filename)); /* os2,vmcms,vms */
int      do_string            OF((__GPRO__ unsigned int len, int option));
ush      makeword             OF((ZCONST uch *b));
ulg      makelong             OF((ZCONST uch *sig));
#if (!defined(STR_TO_ISO) || defined(NEED_STR2ISO))
   char *str2iso              OF((char *dst, ZCONST char *src));
#endif
#if (!defined(STR_TO_OEM) || defined(NEED_STR2OEM))
   char *str2oem              OF((char *dst, ZCONST char *src));
#endif
int      zstrnicmp            OF((register ZCONST char *s1,
                                  register ZCONST char *s2,
                                  register unsigned n));
#ifdef REGULUS
   int zstat                  OF((char *p, struct stat *s));
#endif
#ifdef ZMEM   /* MUST be ifdef'd because of conflicts with the standard def. */
   zvoid *memset OF((register zvoid *, register int, register unsigned int));
   int    memcmp OF((register ZCONST zvoid*, register ZCONST zvoid *,
                     register unsigned int));
   zvoid *memcpy OF((register zvoid *, register ZCONST zvoid *,
                     register unsigned int));
#endif
#ifdef SMALL_MEM
   char *fLoadFarString       OF((__GPRO__ const char Far *sz));
   char *fLoadFarStringSmall  OF((__GPRO__ const char Far *sz));
   char *fLoadFarStringSmall2 OF((__GPRO__ const char Far *sz));
   #ifndef zfstrcpy
     char Far * Far zfstrcpy  OF((char Far *s1, const char Far *s2));
   #endif
#endif


/*---------------------------------------------------------------------------
    Functions in extract.c:
  ---------------------------------------------------------------------------*/

int    extract_or_test_files     OF((__GPRO));
/* static int   store_info          OF((void)); */
/* static int   extract_or_test_member   OF((__GPRO)); */
/* static int   TestExtraField   OF((__GPRO__ uch *ef, unsigned ef_len)); */
/* static int   test_OS2         OF((__GPRO__ uch *eb, unsigned eb_size)); */
/* static int   test_NT          OF((__GPRO__ uch *eb, unsigned eb_size)); */
int    memextract                OF((__GPRO__ uch *tgt, ulg tgtsize,
                                     uch *src, ulg srcsize));
int    memflush                  OF((__GPRO__ uch *rawbuf, ulg size));
char  *fnfilter                  OF((ZCONST char *raw, uch *space));

/*---------------------------------------------------------------------------
    Decompression functions:
  ---------------------------------------------------------------------------*/

#if (!defined(SFX) && !defined(FUNZIP))
int    explode                   OF((__GPRO));                  /* explode.c */
#endif
int    huft_free                 OF((struct huft *t));          /* inflate.c */
int    huft_build                OF((__GPRO__ ZCONST unsigned *b, unsigned n,
                                     unsigned s, ZCONST ush *d, ZCONST ush *e,
                                     struct huft **t, int *m));
#ifdef USE_ZLIB
   int    UZinflate              OF((__GPRO));                  /* inflate.c */
#  define inflate_free(x)        inflateEnd(&((Uz_Globs *)(&G))->dstrm)
#else
   int    inflate                OF((__GPRO));                  /* inflate.c */
   int    inflate_free           OF((__GPRO));                  /* inflate.c */
#endif /* ?USE_ZLIB */
#if (!defined(SFX) && !defined(FUNZIP))
void   unreduce                  OF((__GPRO));                 /* unreduce.c */
/* static void  LoadFollowers    OF((__GPRO__ f_array *follower, uch *Slen));
                                                                * unreduce.c */
int    unshrink                  OF((__GPRO));                 /* unshrink.c */
/* static void  partial_clear    OF((__GPRO));                  * unshrink.c */
#endif /* !SFX && !FUNZIP */

/*---------------------------------------------------------------------------
    Internal API functions (only included in DLL versions):
  ---------------------------------------------------------------------------*/

#ifdef DLL
   void     setFileNotFound       OF((__GPRO));                     /* api.c */
   int      unzipToMemory         OF((__GPRO__ char *zip, char *file,
                                      UzpBuffer *retstr));          /* api.c */
   int      redirect_outfile      OF((__GPRO));                     /* api.c */
   int      writeToMemory         OF((__GPRO__ uch *rawbuf, ulg size));
   int      close_redirect        OF((__GPRO));                     /* api.c */
   /* this obsolescent entry point kept for compatibility: */
   int      UzpUnzip              OF((int argc, char **argv));/* use UzpMain */
#ifdef OS2DLL
   int      varmessage            OF((__GPRO__ uch *buf, ulg size));
   int      varputchar            OF((__GPRO__ int c));         /* rexxapi.c */
   int      finish_REXX_redirect  OF((__GPRO));                 /* rexxapi.c */
#endif
#ifdef API_DOC
   void     APIhelp               OF((__GPRO__ int argc, char **argv));
#endif                                                          /* apihelp.c */
#endif /* DLL */

/*---------------------------------------------------------------------------
    Acorn RISC OS-only functions:
  ---------------------------------------------------------------------------*/

#ifdef RISCOS
   int   isRISCOSexfield     OF((void *extra_field));             /* acorn.c */
   void  setRISCOSexfield    OF((char *path, void *extra_field)); /* acorn.c */
   void  printRISCOSexfield  OF((int isdir, void *extra_field));  /* acorn.c */
#endif

/*---------------------------------------------------------------------------
    Human68K-only functions:
  ---------------------------------------------------------------------------*/

#ifdef __human68k__
   void  InitTwentyOne       OF((void));
#endif

/*---------------------------------------------------------------------------
    Macintosh-only functions:
  ---------------------------------------------------------------------------*/

#ifdef MACOS
   void    screenOpen        OF((char *));                    /* macscreen.c */
   void    screenControl     OF((char *, int));               /* macscreen.c */
   void    screenDump        OF((char *, long));              /* macscreen.c */
   void    screenUpdate      OF((WindowPtr));                 /* macscreen.c */
   void    screenClose       OF((void));                      /* macscreen.c */
   int     macgetch          OF((void));                      /* macscreen.c */

   int     macmkdir     OF((char *));                               /* mac.c */
   short   macopen      OF((char *, short));                        /* mac.c */
   short   maccreat     OF((char *));                               /* mac.c */
   short   macread      OF((short, char *, unsigned));              /* mac.c */
   long    macwrite     OF((short, char *, unsigned));              /* mac.c */
   short   macclose     OF((short));                                /* mac.c */
   long    maclseek     OF((short, long, short));                   /* mac.c */
   char   *macfgets     OF((char *, int, FILE *));                  /* mac.c */
   int     macfprintf   OF((FILE *, char *, ...));                  /* mac.c */
   int     macprintf    OF((char *, ...));                          /* mac.c */
#endif

/*---------------------------------------------------------------------------
    MSDOS-only functions:
  ---------------------------------------------------------------------------*/

#if (defined(MSDOS) && (defined(__GO32__) || defined(__EMX__)))
   unsigned _dos_getcountryinfo(void *);                          /* msdos.c */
#if (!defined(__DJGPP__) || (__DJGPP__ < 2))
   unsigned _dos_setftime(int, unsigned short, unsigned short);   /* msdos.c */
   unsigned _dos_setfileattr(char *, unsigned);                   /* msdos.c */
   unsigned _dos_creat(char *, unsigned, int *);                  /* msdos.c */
   void _dos_getdrive(unsigned *);                                /* msdos.c */
   unsigned _dos_close(int);                                      /* msdos.c */
#endif /* !__DJGPP__ || (__DJGPP__ < 2) */
#endif

/*---------------------------------------------------------------------------
    OS/2-only functions:
  ---------------------------------------------------------------------------*/

#ifdef OS2   /* GetFileTime conflicts with something in Win32 header files */
#if (defined(REENTRANT) && defined(USETHREADID))
   ulg   GetThreadId          OF((void));
#endif
   int   GetCountryInfo       OF((void));                           /* os2.c */
   long  GetFileTime          OF((ZCONST char *name));              /* os2.c */
/* static void  SetPathAttrTimes OF((__GPRO__ int flags, int dir));    os2.c */
/* static int   SetEAs        OF((__GPRO__ const char *path,
                                  void *eablock));                     os2.c */
/* static int   SetACL        OF((__GPRO__ const char *path,
                                  void *eablock));                     os2.c */
/* static int   IsFileNameValid OF((const char *name));                os2.c */
/* static void  map2fat       OF((char *pathcomp, char **pEndFAT));    os2.c */
/* static int   SetLongNameEA OF((char *name, char *longname));        os2.c */
/* static void  InitNLS       OF((void));                              os2.c */
   int   IsUpperNLS           OF((int nChr));                       /* os2.c */
   int   ToLowerNLS           OF((int nChr));                       /* os2.c */
   void  DebugMalloc          OF((void));                           /* os2.c */
#endif

/*---------------------------------------------------------------------------
    QDOS-only functions:
  ---------------------------------------------------------------------------*/

#ifdef QDOS
   int    QMatch              (uch, uch);
   void   QFilename           (__GPRO__ char *);
   char  *Qstrfix             (char *);
   int    QReturn             (int zip_error);
#endif

/*---------------------------------------------------------------------------
    TOPS20-only functions:
  ---------------------------------------------------------------------------*/

#ifdef TOPS20
   int    upper               OF((char *s));                     /* tops20.c */
   int    enquote             OF((char *s));                     /* tops20.c */
   int    dequote             OF((char *s));                     /* tops20.c */
   int    fnlegal             OF(()); /* error if prototyped? */ /* tops20.c */
#endif

/*---------------------------------------------------------------------------
    VM/CMS- and MVS-only functions:
  ---------------------------------------------------------------------------*/

#ifdef CMS_MVS
   extent getVMMVSexfield     OF((char *type, uch *ef_block, unsigned datalen));
   FILE  *vmmvs_open_infile   OF((__GPRO));                       /* vmmvs.c */
   void   close_infile        OF((__GPRO));                       /* vmmvs.c */
#endif

/*---------------------------------------------------------------------------
    VMS-only functions:
  ---------------------------------------------------------------------------*/

#ifdef VMS
   int    check_format        OF((__GPRO));                         /* vms.c */
/* int    open_outfile        OF((__GPRO));           * (see fileio.c) vms.c */
/* int    flush               OF((__GPRO__ uch *rawbuf, unsigned size,
                                  int final_flag));   * (see fileio.c) vms.c */
#ifdef RETURN_CODES
   void   return_VMS          OF((__GPRO__ int zip_error));         /* vms.c */
#else
   void   return_VMS          OF((int zip_error));                  /* vms.c */
#endif
#ifdef VMSCLI
   ulg    vms_unzip_cmdline   OF((int *, char ***));            /* cmdline.c */
   int    VMSCLI_usage        OF((__GPRO__ int error));         /* cmdline.c */
#endif
#endif

/*---------------------------------------------------------------------------
    WIN32-only functions:
  ---------------------------------------------------------------------------*/

#ifdef WIN32
   int   IsWinNT        OF((void));                               /* win32.c */
#ifdef NTSD_EAS
   void  process_defer_NT     OF((__GPRO));                       /* win32.c */
   int   test_NTSD      OF((__GPRO__ uch *eb, unsigned eb_size,
                            uch *eb_ucptr, ulg eb_ucsize));       /* win32.c */
#  define TEST_NTSD     test_NTSD
#endif
#ifdef W32_STAT_BANDAID
   int   zstat_win32    OF((__W32STAT_GLOBALS__
                            const char *path, struct stat *buf)); /* win32.c */
#endif
#endif

/*---------------------------------------------------------------------------
    Miscellaneous/shared functions:
  ---------------------------------------------------------------------------*/

Uz_Globs *globalsCtor    OF((void));                            /* globals.c */

void     envargs         OF((__GPRO__ int *Pargc, char ***Pargv,
                             ZCONST char *envstr, ZCONST char *envstr2));
                                                                /* envargs.c */
void     mksargs         OF((int *argcp, char ***argvp));       /* envargs.c */

int      match           OF((ZCONST char *s, ZCONST char *p,
                             int ic));                            /* match.c */
int      iswild          OF((ZCONST char *p));                    /* match.c */

#ifdef DYNALLOC_CRCTAB
   void     free_crc_table  OF((void));                          /* crctab.c */
#endif
#ifndef USE_ZLIB
   ZCONST ulg near *get_crc_table  OF((void));         /* funzip.c, crctab.c */
   ulg      crc32           OF((ulg crc, ZCONST uch *buf, extent len));
#endif /* !USE_ZLIB */                        /* assembler source or crc32.c */

int      dateformat      OF((void));              /* currently, only msdos.c */
#ifndef WINDLL
   void  version         OF((__GPRO));                              /* local */
#endif
int      mapattr         OF((__GPRO));                              /* local */
int      mapname         OF((__GPRO__ int renamed));                /* local */
int      checkdir        OF((__GPRO__ char *pathcomp, int flag));   /* local */
char    *do_wild         OF((__GPRO__ char *wildzipfn));            /* local */
char    *GetLoadPath     OF((__GPRO));                              /* local */
#if (defined(MORE) && (defined(UNIX) || defined(VMS) || defined(__BEOS__)))
   int screenlines       OF((void));                                /* local */
#endif
#ifndef MTS /* macro in MTS */
   void  close_outfile   OF((__GPRO));                              /* local */
#endif
#ifdef SET_DIR_ATTRIB
   int   set_direc_attribs  OF((__GPRO__ dirtime *d));              /* local */
#endif
#ifdef TIMESTAMP
# ifdef WIN32
   int   stamp_file      OF((__GPRO__
                             ZCONST char *fname, time_t modtime));  /* local */
# else
   int   stamp_file      OF((ZCONST char *fname, time_t modtime));  /* local */
# endif
#endif
#ifdef SYSTEM_SPECIFIC_CTOR
   void  SYSTEM_SPECIFIC_CTOR   OF((__GPRO));                       /* local */
#endif





/************/
/*  Macros  */
/************/

#ifndef MAX
#  define MAX(a,b)   ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#  define MIN(a,b)   ((a) < (b) ? (a) : (b))
#endif

#ifdef DEBUG
#  define Trace(x)   fprintf x
#else
#  define Trace(x)
#endif

#ifdef DEBUG_TIME
#  define TTrace(x)  fprintf x
#else
#  define TTrace(x)
#endif

#ifdef NO_DEBUG_IN_MACROS
#  define MTrace(x)
#else
#  define MTrace(x)  Trace(x)
#endif

#if (defined(UNIX) || defined(T20_VMS)) /* generally old systems */
#  define ToLower(x)   ((char)(isupper((int)x)? tolower((int)x) : x))
#else
#  define ToLower      tolower          /* assumed "smart"; used in match() */
#endif

#ifdef USE_STRM_INPUT
   /* ``Replace'' the unbuffered UNIX style I/O function with similar
    * standard C functions from <stdio.h>.
    */
#  define read(fd,buf,n) fread((buf),1,(n),(FILE *)(fd))
#  define lseek(fd,o,w) fseek((FILE *)(fd),(o),(w))
#  define close(fd) fclose((FILE *)(fd))
#endif /* USE_STRM_INPUT */

/* The return value of the Info() "macro function" is never checked in
 * UnZip. Otherwise, to get the same behaviour as for (*G.message)(), the
 * Info() definition for "FUNZIP" would have to be corrected:
 * #define Info(buf,flag,sprf_arg) \
 *      (fprintf((flag)&1? stderr : stdout, \
 *               (char *)(sprintf sprf_arg, (buf))) == EOF)
 */
#ifndef Info   /* may already have been defined for redirection */
#  ifdef FUNZIP
#    define Info(buf,flag,sprf_arg) \
     fprintf((flag)&1? stderr : stdout, (char *)(sprintf sprf_arg, (buf)))
#  else
#    ifdef INT_SPRINTF  /* optimized version for "int sprintf()" flavour */
#      define Info(buf,flag,sprf_arg) \
       (*G.message)((zvoid *)&G, (uch *)(buf), (ulg)sprintf sprf_arg, (flag))
#    else          /* generic version, does not use sprintf() return value */
#      define Info(buf,flag,sprf_arg) \
       (*G.message)((zvoid *)&G, (uch *)(buf), \
                     (ulg)(sprintf sprf_arg, strlen((char *)(buf))), (flag))
#    endif
#  endif
#endif /* !Info */

#undef Info
#define Info(buf,flag,sprf_arg)  (0)

/*  The following macros wrappers around the fnfilter function are used many
 *  times to prepare archive entry names or name components for displaying
 *  listings and (warning/error) messages. They use sections in the upper half
 *  of 'slide' as buffer, since their output is normally fed through the
 *  Info() macro with 'slide' (the start of this area) as message buffer.
 */
#define FnFilter1(fname)  fnfilter((fname), slide + (WSIZE>>1))
#define FnFilter2(fname)  fnfilter((fname), slide + ((WSIZE>>1) + (WSIZE>>2)))

#ifndef FUNZIP   /* used only in inflate.c */
#  define MESSAGE(str,len,flag)  (*G.message)((zvoid *)&G,(str),(len),(flag))
#endif

#if 0            /* Optimization: use the (const) result of crc32(0L,NULL,0) */
#  define CRCVAL_INITIAL  crc32(0L, NULL, 0)
#else
#  define CRCVAL_INITIAL  0L
#endif

#ifndef TEST_NTSD               /* "NTSD valid?" checking function */
#  define TEST_NTSD     NULL    /*   ... is not available */
#endif

/*
 *  Seek to the block boundary of the block which includes abs_offset,
 *  then read block into input buffer and set pointers appropriately.
 *  If block is already in the buffer, just set the pointers.  This macro
 *  is used by uz_end_central (process.c), zi_end_central (zipinfo.c) and
 *  do_string (fileio.c).  A slightly modified version is embedded within
 *  extract_or_test_files (extract.c).  readbyte() and readbuf() (fileio.c)
 *  are compatible.  NOTE THAT abs_offset is intended to be the "proper off-
 *  set" (i.e., if there were no extra bytes prepended); cur_zipfile_bufstart
 *  contains the corrected offset.
 *
 *  Since ZLSEEK is never used during decompression, it is safe to use the
 *  slide[] buffer for the error message.
 *
 *  The awkward "%cbs_offset" construct is provided for the obnoxious Solaris
 *  compiler, which wants to do macro substitution inside strings.
 */

#ifndef ZLSEEK
#  ifdef USE_STRM_INPUT
#    define _ZLS_RELOAD(abs_offset) {\
         MTrace((stderr, "ZLSEEK: %cbs_offset = %ld, G.extra_bytes = %ld\n",\
           'a', (abs_offset), G.extra_bytes));\
         fseek(G.zipfd,(LONGINT)bufstart,SEEK_SET);\
         G.cur_zipfile_bufstart = ftell(G.zipfd);\
         MTrace((stderr,\
           "       request = %ld, (abs+extra) = %ld, inbuf_offset = %ld\n",\
           request, ((abs_offset)+G.extra_bytes), inbuf_offset));\
         MTrace((stderr, "       bufstart = %ld, cur_zipfile_bufstart = %ld\n",\
           bufstart, G.cur_zipfile_bufstart));\
         if ((G.incnt = fread((char *)G.inbuf,1,INBUFSIZ,G.zipfd)) <= 0)\
             return(PK_EOF);\
         G.inptr = G.inbuf + (int)inbuf_offset;\
         G.incnt -= (int)inbuf_offset;\
     }
#  else /* !USE_STRM_INPUT */
#    define _ZLS_RELOAD(abs_offset) {\
         MTrace((stderr, "ZLSEEK: %cbs_offset = %ld, G.extra_bytes = %ld\n",\
           'a', (abs_offset), G.extra_bytes));\
         G.cur_zipfile_bufstart = lseek(G.zipfd,(LONGINT)bufstart,SEEK_SET);\
         MTrace((stderr,\
           "       request = %ld, (abs+extra) = %ld, inbuf_offset = %ld\n",\
           request, ((abs_offset)+G.extra_bytes), inbuf_offset));\
         MTrace((stderr, "       bufstart = %ld, cur_zipfile_bufstart = %ld\n",\
           bufstart, G.cur_zipfile_bufstart));\
         if ((G.incnt = read(G.zipfd,(char *)G.inbuf,INBUFSIZ)) <= 0)\
             return(PK_EOF);\
         G.inptr = G.inbuf + (int)inbuf_offset;\
         G.incnt -= (int)inbuf_offset;\
     }
#  endif /* ?USE_STRM_INPUT */

#  define ZLSEEK(abs_offset) {\
       LONGINT request = (abs_offset) + G.extra_bytes;\
       LONGINT inbuf_offset = request % INBUFSIZ;\
       LONGINT bufstart = request - inbuf_offset;\
\
       if (request < 0) {\
           Info(slide, 1, ((char *)slide, LoadFarStringSmall(SeekMsg),\
             G.zipfn, LoadFarString(ReportMsg)));\
           return(PK_BADERR);\
       } else if (bufstart != G.cur_zipfile_bufstart)\
           _ZLS_RELOAD(abs_offset)\
       else {\
           G.incnt += (G.inptr-G.inbuf) - (int)inbuf_offset;\
           G.inptr = G.inbuf + (int)inbuf_offset;\
       }\
   }
#endif /* !ZLSEEK */

#define SKIP_(length) if(length&&((error=do_string(__G__ length,SKIP))!=0))\
  {error_in_archive=error; if(error>1) return error;}

/*
 *  Skip a variable-length field, and report any errors.  Used in zipinfo.c
 *  and unzip.c in several functions.
 *
 *  macro SKIP_(length)
 *      ush length;
 *  {
 *      if (length && ((error = do_string(length, SKIP)) != 0)) {
 *          error_in_archive = error;   /-* might be warning *-/
 *          if (error > 1)              /-* fatal *-/
 *              return (error);
 *      }
 *  }
 *
 */


#ifdef FUNZIP
#  define FLUSH(w)  flush(__G__ (ulg)(w))
#  define NEXTBYTE  getc(G.in)   /* redefined in crypt.h if full version */
#else
#  define FLUSH(w)  if (G.mem_mode) memflush(__G__ redirSlide,(ulg)(w)); \
     else flush(__G__ redirSlide,(ulg)(w),0)
#  define NEXTBYTE  (--G.incnt >= 0 ? (int)(*G.inptr++) : readbyte(__G))
#endif


#define READBITS(nbits,zdest) {if(nbits>G.bits_left) {int temp; G.zipeof=1;\
  while (G.bits_left<=8*(int)(sizeof(G.bitbuf)-1) && (temp=NEXTBYTE)!=EOF) {\
  G.bitbuf|=(ulg)temp<<G.bits_left; G.bits_left+=8; G.zipeof=0;}}\
  zdest=(shrint)((ush)G.bitbuf&mask_bits[nbits]);G.bitbuf>>=nbits;\
  G.bits_left-=nbits;}

/*
 * macro READBITS(nbits,zdest)    * only used by unreduce and unshrink *
 *  {
 *      if (nbits > G.bits_left) {  * fill G.bitbuf, 8*sizeof(ulg) bits *
 *          int temp;
 *
 *          G.zipeof = 1;
 *          while (G.bits_left <= 8*(int)(sizeof(G.bitbuf)-1) &&
 *                 (temp = NEXTBYTE) != EOF) {
 *              G.bitbuf |= (ulg)temp << G.bits_left;
 *              G.bits_left += 8;
 *              G.zipeof = 0;
 *          }
 *      }
 *      zdest = (shrint)((ush)G.bitbuf & mask_bits[nbits]);
 *      G.bitbuf >>= nbits;
 *      G.bits_left -= nbits;
 *  }
 *
 */


/* GRR:  should change name to STRLOWER and use StringLower if possible */

/*
 *  Copy the zero-terminated string in str1 into str2, converting any
 *  uppercase letters to lowercase as we go.  str2 gets zero-terminated
 *  as well, of course.  str1 and str2 may be the same character array.
 */
#ifdef __human68k__
#  define TOLOWER(str1, str2) \
   { \
       char *p=(str1), *q=(str2); \
       uch c; \
       while ((c = *p++) != '\0') { \
           if (iskanji(c)) { \
               if (*p == '\0') \
                   break; \
               *q++ = c; \
               *q++ = *p++; \
           } else \
               *q++ = isupper(c) ? tolower(c) : c; \
       } \
       *q = '\0'; \
   }
#else
#  define TOLOWER(str1, str2) \
   { \
       char  *p, *q; \
       p = (str1) - 1; \
       q = (str2); \
       while (*++p) \
           *q++ = (char)(isupper((int)(*p))? tolower((int)(*p)) : *p); \
       *q = '\0'; \
   }
#endif
/*
 *  NOTES:  This macro makes no assumptions about the characteristics of
 *    the tolower() function or macro (beyond its existence), nor does it
 *    make assumptions about the structure of the character set (i.e., it
 *    should work on EBCDIC machines, too).  The fact that either or both
 *    of isupper() and tolower() may be macros has been taken into account;
 *    watch out for "side effects" (in the C sense) when modifying this
 *    macro.
 */

#ifndef foreign
#  define foreign(c)  (c)
#endif

#ifndef native
#  define native(c)   (c)
#  define A_TO_N(str1)
#else
#  ifndef NATIVE
#    define NATIVE     "native chars"
#  endif
#  define A_TO_N(str1) {register uch *p;\
     for (p=(uch *)(str1); *p; p++) *p=native(*p);}
#endif
/*
 *  Translate the zero-terminated string in str1 from ASCII to the native
 *  character set. The translation is performed in-place and uses the
 *  "native" macro to translate each character.
 *
 *  NOTE:  Using the "native" macro means that is it the only part of unzip
 *    which knows which translation table (if any) is actually in use to
 *    produce the native character set.  This makes adding new character set
 *    translation tables easy, insofar as all that is needed is an appropriate
 *    "native" macro definition and the translation table itself.  Currently,
 *    the only non-ASCII native character set implemented is EBCDIC, but this
 *    may not always be so.
 */


/* default setup for internal codepage: assume ISO 8859-1 compatibility!! */
#if (!defined(NATIVE) && !defined(CRTL_CP_IS_ISO) && !defined(CRTL_CP_IS_OEM))
#  define CRTL_CP_IS_ISO
#endif


/*  Translate "extended ASCII" chars (OEM coding for DOS and OS/2; else
 *  ISO-8859-1 [ISO Latin 1, Win Ansi,...]) into the internal "native"
 *  code page.  As with A_TO_N(), conversion is done in place.
 */
#ifndef _ISO_INTERN
#  ifdef CRTL_CP_IS_OEM
#    ifndef IZ_ISO2OEM_ARRAY
#      define IZ_ISO2OEM_ARRAY
#    endif
#    define _ISO_INTERN(str1) {register uch *p;\
       for (p=(uch *)(str1); *p; p++)\
         *p = native((*p & 0x80) ? iso2oem[*p & 0x7f] : *p);}
#  else
#    define _ISO_INTERN(str1)   A_TO_N(str1)
#  endif
#endif

#ifndef _OEM_INTERN
#  ifdef CRTL_CP_IS_OEM
#    define _OEM_INTERN(str1)   A_TO_N(str1)
#  else
#    ifndef IZ_OEM2ISO_ARRAY
#      define IZ_OEM2ISO_ARRAY
#    endif
#    define _OEM_INTERN(str1) {register uch *p;\
       for (p=(uch *)(str1); *p; p++)\
         *p = native((*p & 0x80) ? oem2iso[*p & 0x7f] : *p);}
#  endif
#endif

#ifndef STR_TO_ISO
#  ifdef CRTL_CP_IS_ISO
#    define STR_TO_ISO          strcpy
#  else
#    define STR_TO_ISO          str2iso
#    define NEED_STR2ISO
#  endif
#endif

#ifndef STR_TO_OEM
#  ifdef CRTL_CP_IS_OEM
#    define STR_TO_OEM          strcpy
#  else
#    define STR_TO_OEM          str2oem
#    define NEED_STR2OEM
#  endif
#endif

#if (!defined(INTERN_TO_ISO) && !defined(ASCII2ISO))
#  ifdef CRTL_CP_IS_OEM
     /* know: "ASCII" is "OEM" */
#    define ASCII2ISO(c) (((c) & 0x80) ? oem2iso[(c) & 0x7f] : (c))
#    if (defined(NEED_STR2ISO) && !defined(CRYP_USES_OEM2ISO))
#      define CRYP_USES_OEM2ISO
#    endif
#  else
     /* assume: "ASCII" is "ISO-ANSI" */
#    define ASCII2ISO(c) (c)
#  endif
#endif

#if (!defined(INTERN_TO_OEM) && !defined(ASCII2OEM))
#  ifdef CRTL_CP_IS_OEM
     /* know: "ASCII" is "OEM" */
#    define ASCII2OEM(c) (c)
#  else
     /* assume: "ASCII" is "ISO-ANSI" */
#    define ASCII2OEM(c) (((c) & 0x80) ? iso2oem[(c) & 0x7f] : (c))
#    if (defined(NEED_STR2OEM) && !defined(CRYP_USES_ISO2OEM))
#      define CRYP_USES_ISO2OEM
#    endif
#  endif
#endif

/* codepage conversion setup for testp() in crypt.c */
#ifdef CRTL_CP_IS_ISO
#  ifndef STR_TO_CP2
#    define STR_TO_CP2  STR_TO_OEM
#  endif
#else
#  ifdef CRTL_CP_IS_OEM
#    ifndef STR_TO_CP2
#      define STR_TO_CP2  STR_TO_ISO
#    endif
#  else /* native internal CP is neither ISO nor OEM */
#    ifndef STR_TO_CP1
#      define STR_TO_CP1  STR_TO_ISO
#    endif
#    ifndef STR_TO_CP2
#      define STR_TO_CP2  STR_TO_OEM
#    endif
#  endif
#endif


/* Convert filename (and file comment string) into "internal" charset.
 * This macro assumes that Zip entry filenames are coded in OEM (IBM DOS)
 * codepage when made on
 *  -> DOS (this includes 16-bit Windows 3.1)  (FS_FAT_)
 *  -> OS/2                                    (FS_HPFS_)
 *  -> Win95/WinNT with Nico Mak's WinZip      (FS_NTFS_ && hostver == "5.0")
 *
 * All other ports are assumed to code zip entry filenames in ISO 8859-1.
 */
#ifndef Ext_ASCII_TO_Native
#  define Ext_ASCII_TO_Native(string, hostnum, hostver) \
    if ((hostnum) == FS_FAT_ || (hostnum) == FS_HPFS_ || \
        ((hostnum) == FS_NTFS_ && (hostver) == 50)) { \
        _OEM_INTERN((string)); \
    } else { \
        _ISO_INTERN((string)); \
    }
#endif



/**********************/
/*  Global constants  */
/**********************/

   extern ZCONST ush near  mask_bits[];
   extern ZCONST char *fnames[2];

#ifdef EBCDIC
   extern ZCONST uch ebcdic[];
#endif
#ifdef IZ_ISO2OEM_ARRAY
   extern ZCONST uch Far iso2oem[];
#endif
#ifdef IZ_OEM2ISO_ARRAY
   extern ZCONST uch Far oem2iso[];
#endif

   extern ZCONST char Far  VersionDate[];
   extern ZCONST char Far  CentSigMsg[];
   extern ZCONST char Far  EndSigMsg[];
   extern ZCONST char Far  SeekMsg[];
   extern ZCONST char Far  FilenameNotMatched[];
   extern ZCONST char Far  ExclFilenameNotMatched[];
   extern ZCONST char Far  ReportMsg[];

#ifndef SFX
   extern ZCONST char Far  CompiledWith[];
#endif /* !SFX */



/***********************************/
/*  Global (shared?) RTL variables */
/***********************************/

#ifdef DECLARE_ERRNO
   extern int             errno;
#endif


#endif /* !__unzpriv_h */
