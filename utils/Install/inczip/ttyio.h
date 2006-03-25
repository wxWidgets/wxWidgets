/*
   ttyio.h
 */

#ifndef __ttyio_h   /* don't include more than once */
#define __ttyio_h

#ifndef __crypt_h
#  include "crypt.h"  /* ensure that encryption header file has been seen */
#endif

#if (CRYPT || (defined(UNZIP) && !defined(FUNZIP)))
/*
 * Non-echo keyboard/console input support is needed and enabled.
 */

#ifndef __G         /* UnZip only, for now (DLL stuff) */
#  define __G
#  define __G__
#  define __GDEF
#  define __GPRO    void
#  define __GPRO__
#endif

#ifndef ZCONST      /* UnZip only (until have configure script like Zip) */
#  define ZCONST const
#endif

#if (defined(MSDOS) || defined(OS2) || defined(WIN32))
#  ifndef DOS_OS2_W32
#    define DOS_OS2_W32
#  endif
#endif

#if (defined(DOS_OS2_W32) || defined(__human68k__))
#  ifndef DOS_H68_OS2_W32
#    define DOS_H68_OS2_W32
#  endif
#endif

#if (defined(DOS_OS2_W32) || defined(FLEXOS))
#  ifndef DOS_FLX_OS2_W32
#    define DOS_FLX_OS2_W32
#  endif
#endif

#if (defined(DOS_H68_OS2_W32) || defined(FLEXOS))
#  ifndef DOS_FLX_H68_OS2_W32
#    define DOS_FLX_H68_OS2_W32
#  endif
#endif

#if (defined(VM_CMS) || defined(MVS))
#  ifndef CMS_MVS
#    define CMS_MVS
#  endif
#endif


/* Function prototypes */

/* The following systems supply a `non-echo' character input function "getch()"
 * (or an alias) and do not need the echoff() / echon() function pair.
 */
#ifdef AMIGA
#  define echoff(f)
#  define echon()
#  define getch() Agetch()
#  define HAVE_WORKING_GETCH
#endif /* AMIGA */

#ifdef ATARI
#  define echoff(f)
#  define echon()
#  include <osbind.h>
#  define getch() (Cnecin() & 0x000000ff)
#  define HAVE_WORKING_GETCH
#endif

#ifdef MACOS
#  define echoff(f)
#  define echon()
#  define getch() macgetch()
#  define HAVE_WORKING_GETCH
#endif

#ifdef QDOS
#  define echoff(f)
#  define echon()
#  define HAVE_WORKING_GETCH
#endif

#ifdef RISCOS
#  define echoff(f)
#  define echon()
#  define getch() SWI_OS_ReadC()
#  define HAVE_WORKING_GETCH
#endif

#ifdef DOS_H68_OS2_W32
#  define echoff(f)
#  define echon()
#  ifdef WIN32
#    ifndef getch
#      define getch() getch_win32()
#    endif
#  else /* !WIN32 */
#    ifdef __EMX__
#      ifndef getch
#        define getch() _read_kbd(0, 1, 0)
#      endif
#    else /* !__EMX__ */
#      ifdef __GO32__
#        include <pc.h>
#        define getch() getkey()
#      else /* !__GO32__ */
#        include <conio.h>
#      endif /* ?__GO32__ */
#    endif /* ?__EMX__ */
#  endif /* ?WIN32 */
#  define HAVE_WORKING_GETCH
#endif /* DOS_H68_OS2_W32 */

#ifdef FLEXOS
#  define echoff(f)
#  define echon()
#  define getch() getchar() /* not correct, but may not be on a console */
#  define HAVE_WORKING_GETCH
#endif

/* For VM/CMS and MVS, we do not (yet) have any support to switch terminal
 * input echo on and off. The following "fake" definitions allow inclusion
 * of crypt support and UnZip's "pause prompting" features, but without
 * any echo suppression.
 */
#ifdef CMS_MVS
#  define echoff(f)
#  define echon()
#endif

/* VMS has a single echo() function in ttyio.c to toggle terminal
 * input echo on and off.
 */
#ifdef VMS
#  define echoff(f)  echo(0)
#  define echon()    echo(1)
   int echo OF((int));
#endif

/* For all other systems, ttyio.c supplies the two functions Echoff() and
 * Echon() for suppressing and (re)enabling console input echo.
 */
#ifndef echoff
#  define echoff(f)  Echoff(__G__ f)
#  define echon()    Echon(__G)
   void Echoff OF((__GPRO__ int f));
   void Echon OF((__GPRO));
#endif

/* this stuff is used by MORE and also now by the ctrl-S code; fileio.c only */
#if (defined(UNZIP) && !defined(FUNZIP))
#  ifdef HAVE_WORKING_GETCH
#    define FGETCH(f)  getch()
#  endif
#  ifndef FGETCH
     /* default for all systems where no getch()-like function is available */
     int zgetch OF((__GPRO__ int f));
#    define FGETCH(f)  zgetch(__G__ f)
#  endif
#endif /* UNZIP && !FUNZIP */

#if (CRYPT && !defined(WINDLL))
   char *getp OF((__GPRO__ ZCONST char *m, char *p, int n));
#endif

#else /* !(CRYPT || (UNZIP && !FUNZIP)) */

/*
 * No need for non-echo keyboard/console input; provide dummy definitions.
 */
#define echoff(f)
#define echon()

#endif /* ?(CRYPT || (UNZIP && !FUNZIP)) */

#endif /* !__ttyio_h */
