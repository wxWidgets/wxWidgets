/* ../../include/wx/gtk/setup.h.  Generated automatically by configure.  */
/* wx_setup.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */

#ifndef __GTKSETUPH__
#define __GTKSETUPH__

#ifdef __GNUG__
#pragma interface
#endif

/* define the system to compile */
#define __GTK__ 1
#define __UNIX__ 1
#define __LINUX__ 1
/* #undef __SGI__ */
/* #undef __HPUX__ */
/* #undef __SYSV__ */
/* #undef __SVR4__ */
/* #undef __AIX__ */
/* #undef __SUN__ */
/* #undef __SOLARIS__ */
/* #undef __SUNOS__ */
/* #undef __ALPHA__ */
/* #undef __OSF__ */
/* #undef __BSD__ */
/* #undef __FREEBSD__ */
/* #undef __VMS__ */
/* #undef __ULTRIX__ */
/* #undef __DATA_GENERAL__ */

/*
 * Use zlib
 */
#define USE_ZLIB 1
/*
 * Use gdk_imlib
 */
#define USE_GDK_IMLIB 1
/*
 * Use libpng
 */
#define USE_LIBPNG 1
/*
 * Use Threads
 */
/* #undef USE_THREADS */
/* #undef USE_THREADS_POSIX */
/* #undef USE_THREADS_SGI */
/*
 * Use storable classes
 */
#define USE_STORABLE_CLASSES 1
/*
 * Use automatic translation via gettext() in wxTString
 */
#define USE_AUTOTRANS 1
/*
 * Use font metric files in GetTextExtent for wxPostScriptDC
 * Use consistent PostScript fonts for AFM and printing (!)
 */
#define USE_AFM_FOR_POSTSCRIPT 1
#define WX_NORMALIZED_PS_FONTS 1
/*
 * Use clipboard
 */
/* #undef USE_CLIPBOARD */
/*
 * Use wxWindows layout constraint system
 */
#define USE_CONSTRAINTS 1
/*
 * Use the document/view architecture
 */
#define USE_DOC_VIEW_ARCHITECTURE 1
/*
 * Use enhanced dialog
 */
/* #undef USE_ENHANCED_DIALOG */
/*
 * Use Form panel item placement
 */
/* #undef USE_FORM */
/*
 * Use fraction class
 */
#define USE_FRACTION 1
/*
 * Use gauge item
 */
#define USE_GAUGE 1
/*
 * Implement a GLCanvas class as an interface to OpenGL, using the GLX
 * extension to the X11 protocol.  You can use the (free) Mesa library
 * if you don't have a 'real' OpenGL.
 */
#define USE_GLX 0
/*
 * Use wxWindows help facility (needs USE_IPC 1)
 */
/* #undef USE_HELP */
/*
 * Use iostream.h rather than iostream
 */
#define USE_IOSTREAMH 1
/*
 * Use Interprocess communication
 */
#define USE_IPC 1
/*
 * Use Metafile and Metafile device context
 */
/* #undef USE_METAFILE */
/*
 * Use PostScript device context
 */
#define USE_POSTSCRIPT 1
/*
 * Use the print/preview architecture
 */
#define USE_PRINTING_ARCHITECTURE 1
/*
 * Use Prolog IO
 */
/* #undef USE_PROLOGIO */
/*
 * Use Remote Procedure Call (Needs USE_IPC and USE_PROLOGIO)
 */
/* #undef USE_RPC */
/*
 * Use wxGetResource & wxWriteResource (change .Xdefaults)
 */
#define USE_RESOURCES 1
/*
 * Use scrollbar item
 */
#define USE_SCROLLBAR 1
/*
 * Use time and date classes
 */
#define USE_TIMEDATE 1
/*
 * Use toolbar, use Xt port toolbar (3D look)
 */
#define USE_TOOLBAR 1
#define USE_XT_TOOLBAR 
/*
 * Enables old type checking mechanism (wxSubType)
 */
/* #undef USE_TYPETREE */
/*
 * Use virtual list box item
 */
/* #undef USE_VLBOX */
/*
 * Use wxWindows resource loading (.wxr-files) (Needs USE_PROLOGIO 1)
 */
#define USE_WX_RESOURCES 1
/*
 * Use wxGraph
 */
/* #undef USE_WXGRAPH */
/*
 * Use wxTree
 */

/*
 * Use wxConfig profile management classes (wxFileConfig only under Unix)
 */
#define USE_WXCONFIG 1

/********************** DO NOT CHANGE BELOW THIS POINT **********************/

/**************************** DEBUGGING FEATURES ****************************/

/* Compatibility with 1.66 API.
   Level 0: no backward compatibility, all new features
   Level 1: wxDC, OnSize (etc.) compatibility, but
   some new features such as event tables */
#define WXWIN_COMPATIBILITY  1
/*
 * Enables debugging: memory tracing, assert, etc.
 */
#define DEBUG 1
/*
 * Enables debugging version of wxObject::new and wxObject::delete (IF DEBUG)
 * WARNING: this code may not work with all architectures, especially
 * if alignment is an issue.
 */
/* #undef USE_MEMORY_TRACING */
/*
 * Enable debugging version of global memory operators new and delete
 * Disable it, If this causes problems (e.g. link errors)
 */
/* #undef USE_GLOBAL_MEMORY_OPERATORS */
/*
 * If WXDEBUG && USE_MEMORY_TRACING && USE_GLOBAL_MEMORY_OPERATORS
 * used to debug the memory allocation of wxWindows Xt port code
 */
#define USE_INTERNAL_MEMORY_TRACING 0
/*
 * Matthews garbage collection (used for MrEd?)
 */
#define WXGARBAGE_COLLECTION_ON 0

/**************************** COMPILER FEATURES *****************************/

/*
 * Disable this if your compiler can't cope
 * with omission of prototype parameters.
 */
#define REMOVE_UNUSED_ARG 1
/*
 * The const keyword is being introduced more in wxWindows.
 * You can use this setting to maintain backward compatibility.
 * If 0:	will use const wherever possible.
 * If 1:	will use const only where necessary
 *              for precompiled headers to work.
 * If 2:	will be totally backward compatible, but precompiled
 *	headers may not work and program size will be larger.
 */
#define CONST_COMPATIBILITY 0

/************************ WINDOWS 3.1 COMPATIBILITY *************************/

/*
 * Normalize X drawing code to behave exactly as MSW.
 */
#define WX_STANDARD_GRAPHICS 0

/******************* other stuff **********************************/
/*
 * Support image loading for wxBitmap (wxImage is needed for this)
 */
#define USE_IMAGE_LOADING 0
#define WXIMAGE_INCLUDE "../../utils/image/src/wx_image.h"
/*
 * Use splines
 */
#define USE_SPLINES 1

/*
 * USE_DYNAMIC_CLASSES is TRUE for the Xt port
 */
#define USE_DYNAMIC_CLASSES 1
/*
 * USE_EXTENDED_STATICS is FALSE for the Xt port
*/
#define USE_EXTENDED_STATICS 0

/*************************** IMAKEFILE EVALUATIOS ***************************/

#if USE_XPM
	#define USE_XPM_IN_X 1
#else
	#define USE_XPM_IN_X 0
#endif
#if USE_IMAGE_LOADING
	#define USE_IMAGE_LOADING_IN_X 1
#else
	#define USE_IMAGE_LOADING_IN_X 0
#endif

/* here comes the system-specific stuff */

/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file. */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define if type char is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* #undef __CHAR_UNSIGNED__ */
#endif

/* Define if the closedir function returns void instead of int.  */
/* #undef CLOSEDIR_VOID */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define for DGUX with <sys/dg_sys_info.h>.  */
/* #undef DGUX */

/* Define if you have <dirent.h>.  */
/* #undef DIRENT */

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
#define GETGROUPS_T gid_t

/* Define if the `getloadavg' function needs to be run setuid or setgid.  */
/* #undef GETLOADAVG_PRIVILEGED */

/* Define if the `getpgrp' function takes no argument.  */
/* #undef GETPGRP_VOID */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca, as a function or macro.  */
/* #undef HAVE_ALLOCA */

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if your system has its own `getloadavg' function.  */
/* #undef HAVE_GETLOADAVG */

/* Define if you have the getmntent function.  */
/* #undef HAVE_GETMNTENT */

/* Define if the `long double' type works.  */
#define HAVE_LONG_DOUBLE 1

/* Define if you support file names longer than 14 characters.  */
#define HAVE_LONG_FILE_NAMES 1

/* Define if you have a working `mmap' system call.  */
/* #undef HAVE_MMAP */

/* Define if system calls automatically restart after interruption
   by a signal.  */
/* #undef HAVE_RESTARTABLE_SYSCALLS */

/* Define if your struct stat has st_blksize.  */
#define HAVE_ST_BLKSIZE 1

/* Define if your struct stat has st_blocks.  */
#define HAVE_ST_BLOCKS 1

/* Define if you have the strcoll function and it is properly defined.  */
/* #undef HAVE_STRCOLL */

/* Define if your struct stat has st_rdev.  */
#define HAVE_ST_RDEV 1

/* Define if you have the strftime function.  */
/* #undef HAVE_STRFTIME */

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if your struct tm has tm_zone.  */
/* #undef HAVE_TM_ZONE */

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
#define HAVE_TZNAME 1

/* Define if you have <unistd.h>.  */
/* #undef HAVE_UNISTD_H */

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* #undef HAVE_UTIME_NULL */

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
/* #undef HAVE_VPRINTF */

/* Define if you have the wait3 system call.  */
/* #undef HAVE_WAIT3 */

/* Define as __inline if that's what the C compiler calls it.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define if major, minor, and makedev are declared in <mkdev.h>.  */
/* #undef MAJOR_IN_MKDEV */

/* Define if major, minor, and makedev are declared in <sysmacros.h>.  */
/* #undef MAJOR_IN_SYSMACROS */

/* Define if on MINIX.  */
/* #undef _MINIX */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define if you don't have <dirent.h>, but have <ndir.h>.  */
/* #undef NDIR */

/* Define if you have <memory.h>, and <string.h> doesn't declare the
   mem* functions.  */
/* #undef NEED_MEMORY_H */

/* Define if your struct nlist has an n_un member.  */
/* #undef NLIST_NAME_UNION */

/* Define if you have <nlist.h>.  */
/* #undef NLIST_STRUCT */

/* Define if your C compiler doesn't accept -c and -o together.  */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if the setvbuf function takes the buffering type as its second
   argument and the buffer pointer as the third, as on System V
   before release 3.  */
/* #undef SETVBUF_REVERSED */

/* Define SIZESOF for some Objects  */
#define SIZEOF_INT 4
#define SIZEOF_INT_P 4
#define SIZEOF_LONG 4

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define on System V Release 4.  */
/* #undef SVR4 */

/* Define on BSD  */
/* #undef BSD */

/* Define on System V */
/* #undef SYSV */

/* Define if you don't have <dirent.h>, but have <sys/dir.h>.  */
/* #undef SYSDIR */

/* Define if you don't have <dirent.h>, but have <sys/ndir.h>.  */
/* #undef SYSNDIR */

/* Define if `sys_siglist' is declared by <signal.h>.  */
/* #undef SYS_SIGLIST_DECLARED */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define for Encore UMAX.  */
/* #undef UMAX */

/* Define for Encore UMAX 4.3 that has <inq_status/cpustats.h>
   instead of <sys/cpustats.h>.  */
/* #undef UMAX4_3 */

/* Define if you do not have <strings.h>, index, bzero, etc..  */
/* #undef USG */

/* Define if the system is System V Release 4 */
/* #undef SVR4 */

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* Define if the closedir function returns void instead of int.  */
/* #undef VOID_CLOSEDIR */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

#endif /* __GTKSETUPH__ */


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */
