/////////////////////////////////////////////////////////////////////////////
// Name:        setup.h
// Purpose:     Configuration for the library
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETUP_H_
#define _WX_SETUP_H_

// ----------------------------------------------------------------------------
// global settings
// ----------------------------------------------------------------------------

// define this to 0 when building wxBase library
#define wxUSE_GUI            1

#define WXWIN_COMPATIBILITY  0
                                // Compatibility with 1.68 API.
                                // Level 0: no backward compatibility, all new features
                                // Level 1: Some compatibility. In fact
                                // the compatibility code is now very minimal so there
                                // is little advantage to setting it to 1.

// ----------------------------------------------------------------------------
// General features
// ----------------------------------------------------------------------------

#define wxUSE_CONFIG           1
                                // Use wxConfig, with CreateConfig in wxApp

#define wxUSE_POSTSCRIPT  0
                                // 0 for no PostScript device context
#define wxUSE_AFM_FOR_POSTSCRIPT 0
                                // 1 to use font metric files in GetTextExtent
#define wxUSE_METAFILE    1
                                // 0 for no Metafile and metafile device context
#define wxUSE_IPC         1
                                // 0 for no interprocess comms
// Note: wxHELP uses IPC under X so these are interdependent!
#define wxUSE_HELP        1
                                // 0 for no help facility
#define wxUSE_RESOURCES   1
                                // 0 for no wxGetResource/wxWriteResource
#define wxUSE_CONSTRAINTS 1
                                // 0 for no window layout constraint system

#define wxUSE_TIMEDATE    1
                                // 0 for no wxTime/wxDate classes

#define wxUSE_CLIPBOARD   1
                                // 0 for no clipboard functions

#define wxUSE_SPLINES     1
                                // 0 for no splines

#define wxUSE_DRAG_AND_DROP 1
                                // 0 for no drag and drop

#define wxUSE_TOOLBAR      1
                                // Define 1 to use toolbar classes
#define wxUSE_BUTTONBAR    1
                                // Define 1 to use buttonbar classes (enhanced toolbar
                                // for MS Windows)
#define wxUSE_GAUGE        1
                                // Define 1 to use Microsoft's gauge (Windows)
                                // or Bull's gauge (Motif) library (both in contrib).
#define wxUSE_COMBOBOX     1
                                // Define 1 to use COMBOXBOX control (Windows)
                                // or FWW's ComboBox widget (Motif).
#define wxUSE_CHOICE       1
                                // Define 1 to use CHOICE

#define wxUSE_RADIOBUTTON  1
                                // Define 1 to use radio button control
#define wxUSE_RADIOBTN     1
                                // Unfortunately someone introduced this one, too
#define wxUSE_RADIOBOX     1
                                // And this... don't know which one is corect

#define wxUSE_SCROLLBAR    1
                                // Define 1 to compile contributed wxScrollBar class

#define wxUSE_CHECKBOX     1
                                // Define 1 to compile checkbox

#define wxUSE_LISTBOX      1
                                // Define 1 to compile listbox

#define wxUSE_SPINBTN      1
                                // Define 1 to compile spin button

#define wxUSE_SPINCTRL     1
                                // Define 1 to use wxSpinCtrl class

#define wxUSE_STATLINE     1
                                // use wxStaticLine class

#define wxUSE_CHECKLISTBOX 1
                                // Define 1 to compile check listbox

#define wxUSE_CHOICE       1
                                // Define 1 to compile choice

#define wxUSE_CARET        1
                                // Define 1 to use wxCaret class

#define wxUSE_SLIDER       1
                                // Define 1 to use wxSlider class

#define wxUSE_NEW_GRID     1
                                // Define 1 to use the new wxGrid class
                                // (still under development, define 0 to
                                //  use existing wxGrid class)
#define wxUSE_XPM_IN_MSW   1
                                // Define 1 to support the XPM package in wxBitmap.
#define wxUSE_IMAGE_LOADING_IN_MSW        1
                                // Use dynamic DIB loading/saving code in utils/dib under MSW.
#define wxUSE_RESOURCE_LOADING_IN_MSW     1
                                // Use dynamic icon/cursor loading/saving code
                                // under MSW.
#define wxUSE_WX_RESOURCES        1
                                // Use .wxr resource mechanism (requires PrologIO library)

#define wxUSE_STARTUP_TIPS        1
                                // support for startup tips (wxShowTip &c)

#define wxUSE_DOC_VIEW_ARCHITECTURE 1
                                // Set to 0 to disable document/view architecture
#define wxUSE_MDI_ARCHITECTURE    1
                                // Set to 0 to disable MDI document/view architecture
#define wxUSE_PRINTING_ARCHITECTURE  1
                                // Set to 0 to disable print/preview architecture code
#define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1
                                // Set to 0 to disable PostScript print/preview architecture code
                                // under Windows (just use Windows printing).
#define wxUSE_DYNAMIC_CLASSES     1
                                // If 1, enables provision of run-time type information.
                                // NOW MANDATORY: don't change.
#define wxUSE_MEMORY_TRACING      1
                                // If 1, enables debugging versions of wxObject::new and
                                // wxObject::delete *IF* __WXDEBUG__ is also defined.
                                // WARNING: this code may not work with all architectures, especially
                                // if alignment is an issue.
#define wxUSE_DEBUG_CONTEXT       1
                                // If 1, enables wxDebugContext, for
                                // writing error messages to file, etc. 
                                // If __WXDEBUG__ is not defined, will still use
                                // normal memory operators.
                                // It's recommended to set this to 1,
                                // since you may well need to output
                                // an error log in a production
                                // version (or non-debugging beta)
#define wxUSE_GLOBAL_MEMORY_OPERATORS 1
                                // In debug mode, cause new and delete to be redefined globally.
                                // If this causes problems (e.g. link errors), set this to 0.

#define wxUSE_DEBUG_NEW_ALWAYS 1
                                // In debug mode, causes new to be defined to
                                // be WXDEBUG_NEW (see object.h).
                                // If this causes problems (e.g. link errors), set this to 0.
                                // You may need to set this to 0 if using templates (at least
                                // for VC++).

#define REMOVE_UNUSED_ARG   1
                                // Set this to 0 if your compiler can't cope
                                // with omission of prototype parameters.

#define wxUSE_ODBC          0
                                // Define 1 to use ODBC classes

#define wxODBC_FWD_ONLY_CURSORS 1
                                // Some databases/ODBC drivers only allow forward scrolling cursors.
                                // Unless you specifically want to use backward scrolling
                                // cursors, and you know that all of the databases/ODBC drivers
                                // that you will use these odbc classes with allow backward 
                                // scrolling cursors, this setting should remain set to 1
                                // for maximum database/driver compatibilty

#ifndef __MWERKS__
#define wxUSE_IOSTREAMH     1
#else
#define wxUSE_IOSTREAMH     1
#endif
                                // VC++ 4.2 and above allows <iostream> and <iostream.h>
                                // but you can't mix them. Set to 1 for <iostream.h>,
                                // 0 for <iostream>

#define wxUSE_STREAMS       1
                                // If enabled (1), compiles wxWindows streams classes

#define wxUSE_STD_IOSTREAM  0
                                // Use standard C++ streams if 1. If 0, use wxWin
                                // streams implementation.

#define wxUSE_WXCONFIG      1
                                // if enabled, compiles built-in OS independent wxConfig
                                // class and it's file (any platform) and registry (Win)
                                // based implementations
#define wxUSE_THREADS       1
                                // support for multithreaded applications: if
                                // 1, compile in thread classes (thread.h)
                                // and make the library thread safe
#define wxUSE_ZLIB          1
                                // Use zlib for compression in streams and PNG code
#define wxUSE_LIBPNG        1
                                // Use PNG bitmap/image code
#define wxUSE_LIBJPEG       1
                                // Use JPEG bitmap/image code
#define wxUSE_GIF           1
                                // Use GIF bitmap/image code
#define wxUSE_PNM           1
                                // Use PNM bitmap/image code
#define wxUSE_PCX           1
                                // Use PCX bitmap/image code
#define wxUSE_SERIAL        0
                                // Use serialization (requires utils/serialize)
#define wxUSE_DYNLIB_CLASS  0
                                // Compile in wxLibrary class for run-time
                                // DLL loading and function calling
#define wxUSE_TOOLTIPS      1
                                // Define to use wxToolTip class and
                                // wxWindow::SetToolTip() method
#define wxUSE_SOCKETS       1
                                // Set to 1 to use socket classes
#define wxUSE_HTML          0
                                // Set to 1 to use wxHTML sub-library
#define wxUSE_FS_ZIP        0
#define wxUSE_FS_INET       0   // Set to 1 to enable virtual file systems

#define wxUSE_BUSYINFO      0
                                // wxBusyInfo displays window with message
                                // when app is busy. Works in same way as
                                // wxBusyCursor
#define wxUSE_ZIPSTREAM     0
                                // input stream for reading from zip archives

/*
 * Finer detail
 *
 */

#define wxUSE_APPLE_IEEE          1
                                // if enabled, the float codec written by Apple
                                // will be used to write, in a portable way,
                                // float on the disk

// use wxFile class - required by i18n code, wxConfig and others - recommended
#define wxUSE_FILE                1

// use wxTextFile class: requires wxFile, required by wxConfig
#define wxUSE_TEXTFILE            1

// i18n support: _() macro, wxLocale class. Requires wxFile
#define wxUSE_INTL                1

// wxLogXXX functions - highly recommended
#define wxUSE_LOG                 1

// wxValidator class
#define wxUSE_VALIDATORS 1

// wxAcceleratorTable/Entry classes and support for them in wxMenu(Bar)
#define wxUSE_ACCEL 1

// wxSashWindow class
#define wxUSE_SASH 1

// text entry dialog and wxGetTextFromUser function
#define wxUSE_TEXTDLG 1

// wxToolBar class
#define wxUSE_TOOLBAR 1

// wxStatusBar class
#define wxUSE_STATUSBAR 1

// progress dialog class for lengthy operations
#define wxUSE_PROGRESSDLG 1

// wxDirDlg class for getting a directory name from user
#define wxUSE_DIRDLG 1

/*
 * MS Windows/Windows NT
 *
 */

#define wxUSE_OLE           1
                                // drag-and-drop, clipboard, OLE Automation

#if defined(__WIN95__)
#define wxUSE_CTL3D                      0
#else
#define wxUSE_CTL3D                      1
                                // Define 1 to use Microsoft CTL3D library.
                                // See note above about using FAFA and CTL3D.
#endif

#define wxUSE_COMMON_DIALOGS         1
                                // On rare occasions (e.g. using DJGPP) may want
                                // to omit common dialogs
                                // (e.g. file selector, printer dialog).
                                // Switching this off also switches off
                                // the printing architecture and interactive
                                // wxPrinterDC.
#define wxUSE_ITSY_BITSY             1
                                // Define 1 to use Microsoft's ItsyBitsy
                                // small title bar library, for wxMiniFrame
#define wxUSE_BITMAP_MESSAGE         1
                                // Define 1 to use bitmap messages.
#define wxUSE_PORTABLE_FONTS_IN_MSW  0
                                // Define 1 to use new portable font scheme in Windows
                                // (used by default under X)
#define wxFONT_SIZE_COMPATIBILITY    0
                                // Define 1 for font size to be backward compatible
                                // to 1.63 and earlier. 1.64 and later define point
                                // sizes to be compatible with Windows.
#define wxUSE_GENERIC_DIALOGS_IN_MSW 1
                                // Define 1 to use generic dialogs in Windows, even though
                                // they duplicate native common dialog (e.g. wxColourDialog)
#define wxUSE_PENWINDOWS 0
                                // Set to 1 to use PenWindows

#define wxUSE_OWNER_DRAWN             1
                                // Owner-drawn menus and listboxes

#define wxUSE_NATIVE_STATUSBAR        1
                                // Set to 0 to use cross-platform wxStatusBar
#define wxUSE_DBWIN32                 1
                                // Use Andrew Tucker's OutputDebugString implementation
                                // (required on Win95 only). See utils.cpp.

/*
 * Any platform
 *
 */

#define wxUSE_TYPEDEFS               0
                                // Use typedefs not classes for wxPoint
                                // and others, to reduce overhead and avoid
                                // MS C7 memory bug. Bounds checker
                                // complains about deallocating
                                // arrays of wxPoints if wxPoint is a class.

#if defined(__MINGW32__) && ((__GNUC__>2) ||((__GNUC__==2) && (__GNUC_MINOR__>=95)))
#ifndef wxUSE_NORLANDER_HEADERS
#   define wxUSE_NORLANDER_HEADERS 1
#endif
#endif

#if (!defined(WIN32) && !defined(__WIN32__)) || (defined(__GNUWIN32__) && !defined(wxUSE_NORLANDER_HEADERS))
// Can't use OLE drag and drop in Windows 3.1 because we don't know how
// to implement UUIDs
// GnuWin32 doesn't have appropriate headers for e.g. IUnknown.
#undef wxUSE_DRAG_AND_DROP
#define wxUSE_DRAG_AND_DROP 0
#endif

// Only WIN32 supports wxStatusBar95
#if !defined(__WIN32__) && wxUSE_NATIVE_STATUSBAR
#undef  wxUSE_NATIVE_STATUSBAR
#define wxUSE_NATIVE_STATUSBAR 0
#endif

// Salford C++ doesn't like some of the memory operator definitions
#ifdef __SALFORDC__
#undef  wxUSE_MEMORY_TRACING
#define wxUSE_MEMORY_TRACING      0

#undef wxUSE_GLOBAL_MEMORY_OPERATORS
#define wxUSE_GLOBAL_MEMORY_OPERATORS 0

#undef wxUSE_DEBUG_NEW_ALWAYS
#define wxUSE_DEBUG_NEW_ALWAYS 0

#undef wxUSE_THREADS
#define wxUSE_THREADS 0

#undef wxUSE_DBWIN32
#define wxUSE_DBWIN32 0

#undef wxUSE_OWNER_DRAWN
#define wxUSE_OWNER_DRAWN 0
#endif

#ifdef __TWIN32__

#undef wxUSE_THREADS
#define wxUSE_THREADS 0

#undef wxUSE_DBWIN32
#define wxUSE_DBWIN32 0

#undef wxUSE_ODBC
#define wxUSE_ODBC 0

#endif

#if defined(__WXMSW__) && defined(__BORLANDC__)
#undef wxUSE_ODBC
#define wxUSE_ODBC 0
#endif

// BC++/Win16 can't cope with the amount of data in resource.cpp
#if defined(__WIN16__) && defined(__BORLANDC__)
#undef wxUSE_WX_RESOURCES
#define wxUSE_WX_RESOURCES        0
#endif

#if defined(__WXMSW__) && defined(__WATCOMC__)
#undef wxUSE_LIBJPEG
#define wxUSE_LIBJPEG 0
#endif

#if defined(__WXMSW__) && !defined(__WIN32__)

#undef wxUSE_SOCKETS
#define wxUSE_SOCKETS 0

#undef wxUSE_THREADS
#define wxUSE_THREADS 0

#undef wxUSE_TOOLTIPS
#define wxUSE_TOOLTIPS 0

#undef wxUSE_SPINCTRL
#define wxUSE_SPINCTRL 0

#undef wxUSE_SPINBTN
#define wxUSE_SPINBTN 0

#undef wxUSE_LIBPNG
#define wxUSE_LIBPNG 0

#undef wxUSE_LIBJPEG
#define wxUSE_LIBJPEG 0

#undef wxUSE_GIF
#define wxUSE_GIF 0

#undef wxUSE_PNM
#define wxUSE_PNM 0

#undef wxUSE_PCX
#define wxUSE_PCX 0

#endif

#endif
    // _WX_SETUP_H_
