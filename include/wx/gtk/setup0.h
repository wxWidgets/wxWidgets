/* setup.h.  Generated automatically by configure.  */
/* setup.h.in.  Generated automatically from configure.in by autoheader.  */
/* Generate setup.h.in from this file using autoheader.                    */
/*                                                                         */
/* Version: $Id$                                                           */

/* This define (__WX_SETUP_H__) is used both to insure setup.h is included 
 * only once and to indicate that we are building using configure. */
#ifndef __WX_SETUP_H__
#define __WX_SETUP_H__

#ifdef __GNUG__
    #pragma interface
#endif


/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if the X Window System is missing or not being used.  */
/* #undef X_DISPLAY_MISSING */

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

#define __WXGTK12__ 1

/*
 * Define to 1 for Unix[-like] system
 */
#define wxUSE_UNIX 1

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
/* #undef __EMX__ */

/* #undef __WINDOWS__ */
/* #undef __WIN95__ */
/* #undef __WIN32__ */
/* #undef __GNUWIN32__ */
/* #undef WINVER */

/*
 * Supports bool type
 */
#define HAVE_BOOL 1

/*
 * Use zlib
 */
#define wxUSE_ZLIB 1
/*
 * Use libpng
 */
#define wxUSE_LIBPNG 1
/*
 * Use libjpeg
 */
#define wxUSE_LIBJPEG 1
/*
 * Use libgif
 */
#define wxUSE_LIBGIF 1
/*
 * Use iODBC
 */
#define wxUSE_ODBC 0
/*
 * Use Threads
 */
#define wxUSE_THREADS 1
/*
 * Have glibc2
 */
#define wxHAVE_GLIBC2 1
/*
 * Use libXpm
 */
#define wxHAVE_LIB_XPM 0
/*
 * Use OpenGL
 */
#define wxUSE_OPENGL 0

/*
 * Use GUI
 */
#define wxUSE_GUI 1
/*
 * Defined if !wxUSE_GUI
 */
/* #undef wxUSE_NOGUI */

/*
 * Use constraints mechanism
 */
#define wxUSE_CONSTRAINTS 1

/*
 * Use validators
 */
#define wxUSE_VALIDATORS 1

/*
 * Use accelerators
 */
#define wxUSE_ACCEL 1

/*
 * Use wxTextEntryDialog
 */
#define wxUSE_TEXTDLG 1

/*
 * Use startup tips
 */
#define wxUSE_STARTUP_TIPS 1

/*
 * Use progress dialog
 */
#define wxUSE_PROGRESSDLG 1

/*
 * Use directory chooser dialog
 */
#define wxUSE_DIRDLG 1

/*
 * Joystick support (Linux/GTK only)
 */
#define wxUSE_JOYSTICK 0

/*
 * Use this control
 */
#define wxUSE_CARET 1
/*
 * Use this control
 */
#define wxUSE_BMPBUTTON 1
/*
 * Use this control
 */
#define wxUSE_CHECKBOX 1
/*
 * Use this control
 */
#define wxUSE_CHECKLISTBOX 1
/*
 * Use this control
 */
#define wxUSE_COMBOBOX 1
/*
 * Use this control
 */
#define wxUSE_CHOICE 1
/*
 * Use this control
 */
#define wxUSE_GAUGE 1
/*
 * Use this control
 */
#define wxUSE_GRID 1
/*
 * Use this control
 */
#define wxUSE_IMAGLIST 1
/*
 * Use this control
 */
#define wxUSE_LISTBOX 1
/*
 * Use this control
 */
#define wxUSE_LISTCTRL 1
/*
 * Use this control
 */
#define wxUSE_NOTEBOOK 1
/*
 * Use this control
 */
#define wxUSE_RADIOBOX 1
/*
 * Use this control
 */
#define wxUSE_RADIOBTN 1
/*
 * Use this control
 */
#define wxUSE_SASH 1
/*
 * Use this control
 */
#define wxUSE_SCROLLBAR 1
/*
 * Use this control
 */
#define wxUSE_SLIDER 1
/*
 * Use this control
 */
#define wxUSE_SPINBTN 1
/*
 * Use this control
 */
#define wxUSE_SPLITTER 1
/*
 * Use this control
 */
#define wxUSE_STATBMP 1
/*
 * Use this control
 */
#define wxUSE_STATBOX 1
/*
 * Use this control
 */
#define wxUSE_STATLINE 1
/*
 * Use this control
 */
#define wxUSE_STATUSBAR 1
/*
 * Use Tab dialog class - obsolete, use wxNotebook instead
 */
#define wxUSE_TAB_DIALOG 0
/* compatibility */
#define wxUSE_TABDIALOG wxUSE_TAB_DIALOG
/*
 * Use this control
 */
#define wxUSE_TOOLBAR 1

#if defined(__WXWINE__) || defined(__GNUWIN32__)
  #if wxUSE_TOOLBAR
    #define wxUSE_BUTTONBAR 1
  #endif
#endif

/*
 * Use this control
 */
#define wxUSE_TREECTRL 1

/*
 * Use wxLongLong (a.k.a. int64) class
 */
#define wxUSE_LONGLONG 0

/*
 * Use wxFile class
 */
#define wxUSE_FILE 1
/*
 * Use wxTextFile class
 */
#define wxUSE_TEXTFILE 1
/*
 * Use log classes and logging functions
 */
#define wxUSE_LOG 1
/*
 * Use time and date classes
 */
#define wxUSE_TIMEDATE 1
/*
 * Use wave class
 */
#define wxUSE_WAVE 0
/*
 * Use config system
 */
#define wxUSE_CONFIG 1
/*
 * Use intl system
 */
#define wxUSE_INTL 1
/*
 * Use streams
 */
#define wxUSE_STREAMS 1
/*
 * Use class serialization
 */
#define wxUSE_SERIAL 1
/*
 * Use sockets
 */
#define wxUSE_SOCKETS 1
/*
 * Use standard C++ streams if 1. If 0, use wxWin
 * streams implementation.
 */
#define wxUSE_STD_IOSTREAM  0
/*
 * wxLibrary class
 */
#define wxUSE_DYNLIB_CLASS 1

/*
 * Use font metric files in GetTextExtent for wxPostScriptDC
 * Use consistent PostScript fonts for AFM and printing (!)
 */
#define wxUSE_AFM_FOR_POSTSCRIPT 1
#define wxUSE_NORMALIZED_PS_FONTS 1

/* compatibility */
#define WX_NORMALIZED_PS_FONTS wxUSE_NORMALIZED_PS_FONTS

/*
 * Use PostScript device context
 */
#define wxUSE_POSTSCRIPT 1

/*
 * Compile wxString with wide character (Unicode) support?
 */
#define wxUSE_UNICODE 0

/*
 * Work around a bug in GNU libc 5.x wcstombs() implementation.
 *
 * Note that you must link your programs with libc.a if you enable this and you
 * have libc 5 (you should enable this for libc6 where wcsrtombs() is
 * thread-safe version of wcstombs()).
 */
#define wxUSE_WCSRTOMBS 0

/*
 * On some platforms overloading on size_t/int doesn't work, yet we'd like
 * to define both size_t and int version of wxString::operator[] because it
 * should really be size_t, but a lot of old, broken code uses int indices.
 */
#define wxUSE_SIZE_T_STRING_OPERATOR 0

/*
 * Use the new experimental implementation of wxString::Printf()?
 *
 * Warning: enabling this may cause internal compiler errors with gcc!
 */
#define wxUSE_EXPERIMENTAL_PRINTF 0

/*
 * Use Interprocess communication
 */
#define wxUSE_IPC 1
/*
 * Use wxGetResource & wxWriteResource (change .Xdefaults)
 */
#define wxUSE_X_RESOURCES 0
/*
 * Use clipboard
 */
#define wxUSE_CLIPBOARD 1
/*
 * Use tooltips
 */
#define wxUSE_TOOLTIPS 1
/*
 * Use dnd
 */
#define wxUSE_DRAG_AND_DROP 1
/*
 * Use spline
 */
#define wxUSE_SPLINES 1
/*
 * Use wxLibrary class
 */
#define wxUSE_DYNLIB_CLASS 1

/*
 * Use the mdi architecture
 */
#define wxUSE_MDI_ARCHITECTURE 1
/*
 * Use the document/view architecture
 */
#define wxUSE_DOC_VIEW_ARCHITECTURE 1
/*
 * Use the print/preview architecture
 */
#define wxUSE_PRINTING_ARCHITECTURE 1

/*
 * Use Prolog IO
 */
#define wxUSE_PROLOGIO 1
/*
 * Use wxWindows resource loading (.wxr-files) (Needs wxUSE_PROLOGIO 1)
 */
#define wxUSE_RESOURCES 1

/* for compatibility */
#define wxUSE_WX_RESOURCES wxUSE_RESOURCES

/*
 * Use wxWindows help facility (needs wxUSE_IPC 1)
 */
#define wxUSE_HELP 1
/*
 * Use iostream.h rather than iostream
 */
#define wxUSE_IOSTREAMH 1
/*
 * Use Apple Ieee-double converter
 */
#define wxUSE_APPLE_IEEE 1
/*
 * Compatibility with 1.66 API.
 * Level 0: no backward compatibility, all new features
 * Level 1: wxDC, OnSize (etc.) compatibility, but
 * some new features such as event tables
 */
#define WXWIN_COMPATIBILITY 0
/*
 * Compatibility with 2.0 API.
 */
#define WXWIN_COMPATIBILITY_2 1
/*
 * Enables debugging: memory tracing, assert, etc., contains debug level
 */
#define WXDEBUG 1
/*
 * Enables debugging version of wxObject::new and wxObject::delete (IF WXDEBUG)
 * WARNING: this code may not work with all architectures, especially
 * if alignment is an issue.
 */
#define wxUSE_MEMORY_TRACING 0
/*
 * Enable debugging version of global memory operators new and delete
 * Disable it, If this causes problems (e.g. link errors)
 */
#define wxUSE_DEBUG_NEW_ALWAYS 0
/*
 * VZ: What does this one do? (FIXME)
 */
#define wxUSE_DEBUG_CONTEXT 0
/*
 * In debug mode, causes new to be defined to
 * be WXDEBUG_NEW (see object.h). If this causes
 * problems (e.g. link errors), set this to 0.
 */
#define wxUSE_GLOBAL_MEMORY_OPERATORS 0
/*
 * Matthews garbage collection (used for MrEd?)
 */
#define WXGARBAGE_COLLECTION_ON 0
/*
 * Use splines
 */
#define wxUSE_SPLINES 1
/*
 * wxUSE_DYNAMIC_CLASSES is TRUE for the Xt port
 */
#define wxUSE_DYNAMIC_CLASSES 1

/*
 * Support for metafiles
 */
#define wxUSE_METAFILE 0

/*
 * wxMiniFrame
 */
#define wxUSE_MINIFRAME 1

/*
 * wxHTML
 */
#define wxUSE_HTML 1

/*
 * (virtual) filesystems code
 */
#define wxUSE_FS_INET 1
#define wxUSE_FS_ZIP 1

/*
 * A class that shows info window when app is busy
 * (works exactly like wxBusyCursor)
 */
#define wxUSE_BUSYINFO 1 

/*
 * Zip stream for accessing files stored inside .zip archives
 */
#define wxUSE_ZIPSTREAM 1

 
/*
 * Disable this if your compiler can't cope
 * with omission of prototype parameters.
 */
#define REMOVE_UNUSED_ARG 1
/*
 * The const keyword is being introduced more in wxWindows.
 * You can use this setting to maintain backward compatibility.
 * If 0: will use const wherever possible.
 * If 1: will use const only where necessary
 *       for precompiled headers to work.
 * If 2: will be totally backward compatible, but precompiled
 *       headers may not work and program size will be larger.
 */
#define CONST_COMPATIBILITY 0

/* The type of 3rd argument to getsockname() - usually size_t or int */
#define SOCKLEN_T socklen_t

/* Define if you have dlopen() */
#define HAVE_DLOPEN 1

/* Define if you have nanosleep() */
#define HAVE_NANOSLEEP 1

/* Define if you have sched_yield */
#define HAVE_SCHED_YIELD 1

/* Define if you have pthread_cancel */
#define HAVE_PTHREAD_CANCEL 1

/* Define if you have all functions to set thread priority */
#define HAVE_THREAD_PRIORITY_FUNCTIONS 1

/* Define if you can specify exit functions to a thread */
/* #undef HAVE_THREAD_CLEANUP_FUNCTIONS */

/* Define if you have shl_load() */
/* #undef HAVE_SHL_LOAD */

/* Define if you have vsnprintf() */
#define HAVE_VSNPRINTF 1

/* Define if you have usleep() */
/* #undef HAVE_USLEEP */

/* Define if you have wcslen function  */
#define HAVE_WCSLEN 1

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a int *.  */
#define SIZEOF_INT_P 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 8

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* Define if you have the dlopen function.  */
#define HAVE_DLOPEN 1

/* Define if you have the gethostname function.  */
/* #undef HAVE_GETHOSTNAME */

/* Define if you have the nanosleep function.  */
#define HAVE_NANOSLEEP 1

/* Define if you have the shl_load function.  */
/* #undef HAVE_SHL_LOAD */

/* Define if you have the uname function.  */
#define HAVE_UNAME 1

/* Define if you have the usleep function.  */
/* #undef HAVE_USLEEP */

/* Define if you have the vfork function.  */
#define HAVE_VFORK 1

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the vsscanf function.  */
#define HAVE_VSSCANF 1

/* Define if you have the <X11/XKBlib.h> header file.  */
#define HAVE_X11_XKBLIB_H 1

/* Define if you have the <fnmatch.h> header file.  */
#define HAVE_FNMATCH_H 1

/* Define if you have the <iostream> header file.  */
#define HAVE_IOSTREAM 1

/* Define if you have the <linux/joystick.h> header file.  */
/* #undef HAVE_LINUX_JOYSTICK_H */

/* Define if you have the <sched.h> header file.  */
#define HAVE_SCHED_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <wchar.h> header file.  */
#define HAVE_WCHAR_H 1

/* Define if you have the <wcstr.h> header file.  */
/* #undef HAVE_WCSTR_H */

/* Name of package */
#define PACKAGE "wxWindows"

/* Version number of package */
#define VERSION "2.1.0"


#endif /* __WX_SETUP_H__ */
