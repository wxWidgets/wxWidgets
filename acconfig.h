/* Generate setup.h.in from this file using autoheader.                    */
/*                                                                         */
/* Version: $Id$                                                           */

#ifndef __GTKSETUPH__
#define __GTKSETUPH__

#ifdef __GNUG__
    #pragma interface
#endif

@TOP@

/* ------------------------------------------------------------------------ */
/* wxWindows version (not used but required by autoheader)                  */
/* ------------------------------------------------------------------------ */

/* needed for older automake? */
#undef WITH_SYMBOL_UNDERSCORE

/* ------------------------------------------------------------------------ */
/* define if support for GTK 1.2 features                                   */
/* ------------------------------------------------------------------------ */

#undef __WXGTK12__

/* ------------------------------------------------------------------------ */
/* define the system to compile                                             */
/* ------------------------------------------------------------------------ */

/*
 * Define to 1 for Unix[-like] system
 */
#define wxUSE_UNIX 1

#undef __UNIX__
#undef __LINUX__
#undef __SGI__
#undef __HPUX__
#undef __SYSV__
#undef __SVR4__
#undef __AIX__
#undef __SUN__
#undef __SOLARIS__
#undef __SUNOS__
#undef __ALPHA__
#undef __OSF__
#undef __BSD__
#undef __FREEBSD__
#undef __VMS__
#undef __ULTRIX__
#undef __DATA_GENERAL__
#undef __EMX__

#undef __WINDOWS__
#undef __WIN95__
#undef __WIN32__
#undef __GNUWIN32__

/* ------------------------------------------------------------------------ */
/* compiler options                                                         */
/* ------------------------------------------------------------------------ */

/*
 * Supports bool type
 */
#undef HAVE_BOOL

/* ------------------------------------------------------------------------ */
/* library options                                                          */
/* ------------------------------------------------------------------------ */

/*
 * Use zlib
 */
#define wxUSE_ZLIB 0
/*
 * Use libpng
 */
#define wxUSE_LIBPNG 0
/*
 * Use libjpeg
 */
#define wxUSE_LIBJPEG 0
/*
 * Use libgif
 */
#define wxUSE_LIBGIF 0
/*
 * Use iODBC
 */
#define wxUSE_ODBC 0
/*
 * Use Threads
 */
#define wxUSE_THREADS 0
/*
 * Have glibc2
 */
#define wxHAVE_GLIBC2 0
/*
 * Use libXpm
 */
#define wxHAVE_LIB_XPM 0
/*
 * Use OpenGL
 */
#define wxUSE_OPENGL 0

/* ------------------------------------------------------------------------ */
/* GUI or not GUI?                                                          */
/* ------------------------------------------------------------------------ */

/*
 * Use GUI
 */
#define wxUSE_GUI 1
/*
 * Defined if !wxUSE_GUI
 */
#undef wxUSE_NOGUI

/* ------------------------------------------------------------------------ */
/* "global" GUI options                                                     */
/* ------------------------------------------------------------------------ */

/*
 * Use constraints mechanism
 */
#define wxUSE_CONSTRAINTS 0

/*
 * Use validators
 */
#define wxUSE_VALIDATORS 0

/*
 * Use accelerators
 */
#define wxUSE_ACCEL 0

/*
 * Use wxTextEntryDialog
 */
#define wxUSE_TEXTDLG 0

/*
 * Use startup tips
 */
#define wxUSE_STARTUP_TIPS 0

/*
 * Use progress dialog
 */
#define wxUSE_PROGRESSDLG 0

/*
 * Use directory chooser dialog
 */
#define wxUSE_DIRDLG 0

/*
 * Joystick support (Linux/GTK only)
 */
#define wxUSE_JOYSTICK 0

/* ------------------------------------------------------------------------ */
/* GUI control options                                                      */
/* ------------------------------------------------------------------------ */

/*
 * Use this control
 */
#define wxUSE_CARET 0
/*
 * Use this control
 */
#define wxUSE_BMPBUTTON 0
/*
 * Use this control
 */
#define wxUSE_CHECKBOX 0
/*
 * Use this control
 */
#define wxUSE_CHECKLISTBOX 0
/*
 * Use this control
 */
#define wxUSE_COMBOBOX 0
/*
 * Use this control
 */
#define wxUSE_CHOICE 0
/*
 * Use this control
 */
#define wxUSE_GAUGE 0
/*
 * Use this control
 */
#define wxUSE_GRID 0
/*
 * Use this control
 */
#define wxUSE_IMAGLIST 0
/*
 * Use this control
 */
#define wxUSE_LISTBOX 0
/*
 * Use this control
 */
#define wxUSE_LISTCTRL 0
/*
 * Use this control
 */
#define wxUSE_NOTEBOOK 0
/*
 * Use this control
 */
#define wxUSE_RADIOBOX 0
/*
 * Use this control
 */
#define wxUSE_RADIOBTN 0
/*
 * Use this control
 */
#define wxUSE_SASH 0
/*
 * Use this control
 */
#define wxUSE_SCROLLBAR 0
/*
 * Use this control
 */
#define wxUSE_SLIDER 0
/*
 * Use this control
 */
#define wxUSE_SPINBTN 0
/*
 * Use this control
 */
#define wxUSE_SPLITTER 0
/*
 * Use this control
 */
#define wxUSE_STATBMP 0
/*
 * Use this control
 */
#define wxUSE_STATBOX 0
/*
 * Use this control
 */
#define wxUSE_STATLINE 0
/*
 * Use this control
 */
#define wxUSE_STATUSBAR 0
/*
 * Use Tab dialog class - obsolete, use wxNotebook instead
 */
#define wxUSE_TAB_DIALOG 0
/* compatibility */
#define wxUSE_TABDIALOG wxUSE_TAB_DIALOG
/*
 * Use this control
 */
#define wxUSE_TOOLBAR 0

#ifdef __WXWINE__
  #if wxUSE_TOOLBAR
    #define wxUSE_BUTTONBAR 1
  #endif
#endif

/*
 * Use this control
 */
#define wxUSE_TREECTRL 0

/* ------------------------------------------------------------------------ */
/* non-GUI options                                                          */
/* ------------------------------------------------------------------------ */

/*
 * Use wxLongLong (a.k.a. int64) class
 */
#define wxUSE_LONGLONG 0

/*
 * Use wxFile class
 */
#define wxUSE_FILE 0
/*
 * Use wxTextFile class
 */
#define wxUSE_TEXTFILE 0
/*
 * Use log classes and logging functions
 */
#define wxUSE_LOG 0
/*
 * Use time and date classes
 */
#define wxUSE_TIMEDATE 0
/*
 * Use wave class
 */
#define wxUSE_WAVE 0
/*
 * Use config system
 */
#define wxUSE_CONFIG 0
/*
 * Use intl system
 */
#define wxUSE_INTL 0
/*
 * Use streams
 */
#define wxUSE_STREAMS 0
/*
 * Use class serialization
 */
#define wxUSE_SERIAL 0
/*
 * Use sockets
 */
#define wxUSE_SOCKETS 0
/*
 * Use standard C++ streams if 1. If 0, use wxWin
 * streams implementation.
 */
#define wxUSE_STD_IOSTREAM  0
/*
 * wxLibrary class
 */
#define wxUSE_DYNLIB_CLASS 1

/* ------------------------------------------------------------------------ */
/* PS options                                                               */
/* ------------------------------------------------------------------------ */

/*
 * Use font metric files in GetTextExtent for wxPostScriptDC
 * Use consistent PostScript fonts for AFM and printing (!)
 */
#define wxUSE_AFM_FOR_POSTSCRIPT 0
#define wxUSE_NORMALIZED_PS_FONTS 0

/* compatibility */
#define WX_NORMALIZED_PS_FONTS wxUSE_NORMALIZED_PS_FONTS

/*
 * Use PostScript device context
 */
#define wxUSE_POSTSCRIPT 0

/* ------------------------------------------------------------------------ */
/* wxString options                                                         */
/* ------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------ */
/* misc options                                                             */
/* ------------------------------------------------------------------------ */

/*
 * Use Interprocess communication
 */
#define wxUSE_IPC 0
/*
 * Use wxGetResource & wxWriteResource (change .Xdefaults)
 */
#define wxUSE_X_RESOURCES 0
/*
 * Use clipboard
 */
#define wxUSE_CLIPBOARD 0
/*
 * Use tooltips
 */
#define wxUSE_TOOLTIPS 0
/*
 * Use dnd
 */
#define wxUSE_DRAG_AND_DROP 0
/*
 * Use spline
 */
#define wxUSE_SPLINES 0
/*
 * Use wxLibrary class
 */
#define wxUSE_DYNLIB_CLASS 0

/* ------------------------------------------------------------------------ */
/* architecture options                                                     */
/* ------------------------------------------------------------------------ */

/*
 * Use the mdi architecture
 */
#define wxUSE_MDI_ARCHITECTURE 0
/*
 * Use the document/view architecture
 */
#define wxUSE_DOC_VIEW_ARCHITECTURE 0
/*
 * Use the print/preview architecture
 */
#define wxUSE_PRINTING_ARCHITECTURE 0

/* ------------------------------------------------------------------------ */
/* Prolog and wxWindows' resource system options                            */
/* ------------------------------------------------------------------------ */

/*
 * Use Prolog IO
 */
#define wxUSE_PROLOGIO 0
/*
 * Use wxWindows resource loading (.wxr-files) (Needs wxUSE_PROLOGIO 1)
 */
#define wxUSE_RESOURCES 0

/* for compatibility */
#define wxUSE_WX_RESOURCES wxUSE_RESOURCES

/* ------------------------------------------------------------------------ */
/* the rest                                                                 */
/* ------------------------------------------------------------------------ */

/*
 * Use wxWindows help facility (needs wxUSE_IPC 1)
 */
#define wxUSE_HELP 0
/*
 * Use iostream.h rather than iostream
 */
#define wxUSE_IOSTREAMH 0
/*
 * Use Apple Ieee-double converter
 */
#define wxUSE_APPLE_IEEE 0
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
#define WXDEBUG 0
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
#define wxUSE_SPLINES 0
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
#define wxUSE_MINIFRAME 0

/*
 * wxHTML
 */
#define wxUSE_HTML 0

/*
 * (virtual) filesystems code
 */
#define wxUSE_FS_INET 0
#define wxUSE_FS_ZIP 0

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

/* ------------------------------------------------------------------------ */
/* System-specific stuff                                                    */
/* ------------------------------------------------------------------------ */

/* The type of 3rd argument to getsockname() - usually size_t or int */
#undef SOCKLEN_T

/* Define if you have dlopen() */
#undef HAVE_DLOPEN

/* Define if you have nanosleep() */
#undef HAVE_NANOSLEEP

/* Define if you have sched_yield */
#undef HAVE_SCHED_YIELD

/* Define if you have pthread_cancel */
#undef HAVE_PTHREAD_CANCEL

/* Define if you have all functions to set thread priority */
#undef HAVE_THREAD_PRIORITY_FUNCTIONS

/* Define if you can specify exit functions to a thread */
#undef HAVE_THREAD_CLEANUP_FUNCTIONS

/* Define if you have shl_load() */
#undef HAVE_SHL_LOAD

/* Define if you have vsnprintf() */
#undef HAVE_VSNPRINTF

/* Define if you have usleep() */
#undef HAVE_USLEEP

/* Define if you have wcslen function  */
#undef HAVE_WCSLEN

@BOTTOM@

#endif /* __GTKSETUPH__ */
