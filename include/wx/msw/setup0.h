/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/setup.h
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

// define this to 0 when building wxBase library - this can also be done from
// makefile/project file overriding the value here
#ifndef wxUSE_GUI
    #define wxUSE_GUI            1
#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// compatibility settings
// ----------------------------------------------------------------------------

// This setting determines the compatibility with 1.68 API:
// Level 0: no backward compatibility, all new features
// Level 1: some extra methods are defined for compatibility.
//
// Default is 0.
//
// Recommended setting: 0 (in fact the compatibility code is now very minimal
// so there is little advantage to setting it to 1.
#define WXWIN_COMPATIBILITY  0

// in wxMSW version 2.1.11 and earlier, wxIcon always derives from wxBitmap,
// but this is very dangerous because you can mistakenly pass an icon instead
// of a bitmap to a function taking "const wxBitmap&" - which will *not* work
// because an icon is not a valid bitmap
//
// Starting from 2.1.12, you have the choice under this backwards compatible
// behaviour (your code will still compile, but probably won't behave as
// expected!) and not deriving wxIcon class from wxBitmap, but providing a
// conversion ctor wxBitmap(const wxIcon&) instead.
//
// Recommended setting: 0
#define wxICON_IS_BITMAP    0

// ----------------------------------------------------------------------------
// debugging settings
// ----------------------------------------------------------------------------

// Generic comment about debugging settings: they are very useful if you don't
// use any other memory leak detection tools such as Purify/BoundsChecker, but
// are probably redundant otherwise. Also, Visual C++ CRT has the same features
// as wxWindows memory debugging subsystem built in since version 5.0 and you
// may prefer to use it instead of built in memory debugging code because it is
// faster and more fool proof.
//
// Using VC++ CRT memory debugging is enabled by default in debug mode
// (__WXDEBUG__) if wxUSE_GLOBAL_MEMORY_OPERATORS is *not* enabled (i.e. is 0)
// and if __NO_VC_CRTDBG__ is not defined.

// If 1, enables wxDebugContext, for writing error messages to file, etc. If
// __WXDEBUG__ is not defined, will still use normal memory operators. It's
// recommended to set this to 1, since you may well need to output an error log
// in a production version (or non-debugging beta).
//
// Default is 1.
//
// Recommended setting: 1 but see comment above
#define wxUSE_DEBUG_CONTEXT 1

// If 1, enables debugging versions of wxObject::new and wxObject::delete *IF*
// __WXDEBUG__ is also defined.
//
// WARNING: this code may not work with all architectures, especially if
// alignment is an issue. This switch is currently ignored for mingw / cygwin
//
// Default is 1
//
// Recommended setting: 1 but see comment in the beginning of this section
#define wxUSE_MEMORY_TRACING 1

// In debug mode, cause new and delete to be redefined globally.
// If this causes problems (e.g. link errors), set this to 0.
// This switch is currently ignored for mingw / cygwin
//
// Default is 1
//
// Recommended setting: 1 but see comment in the beginning of this section
#define wxUSE_GLOBAL_MEMORY_OPERATORS 1

// In debug mode, causes new to be defined to be WXDEBUG_NEW (see object.h). If
// this causes problems (e.g. link errors), set this to 0. You may need to set
// this to 0 if using templates (at least for VC++). This switch is currently
// ignored for mingw / cygwin
//
// Default is 1
//
// Recommended setting: 1 but see comment in the beginning of this section
#define wxUSE_DEBUG_NEW_ALWAYS 1

// wxHandleFatalExceptions() may be used to catch the program faults at run
// time and, instead of terminating the program with a usual GPF message box,
// call the user-defined wxApp::OnFatalException() function. If you set
// wxUSE_ON_FATAL_EXCEPTION to 0, wxHandleFatalExceptions() will not work.
//
// This setting is for Win32 only and can only be enabled if your compiler
// supports Win32 structured exception handling (currently only VC++ does)
//
// Default is 1
//
// Recommended setting: 1 if your compiler supports it.
#ifdef _MSC_VER
    #define wxUSE_ON_FATAL_EXCEPTION 1
#else
    #define wxUSE_ON_FATAL_EXCEPTION 0
#endif

// ----------------------------------------------------------------------------
// Unicode support
// ----------------------------------------------------------------------------

// Set wxUSE_UNICODE to 1 to compile wxWindows in Unicode mode: wxChar will be
// defined as wchar_t, wxString will use Unicode internally. If you set this
// to 1, you must use wxT() macro for all literal strings in the program.
//
// Unicode is currently only fully supported under Windows NT/2000 (Windows 9x
// doesn't support it and the programs compiled in Unicode mode will not run
// under 9x).
//
// Default is 0
//
// Recommended setting: 0 (unless you only plan to use Windows NT/2000)
#define wxUSE_UNICODE 0

// Setting wxUSE_WCHAR_T to 1 gives you some degree of Unicode support without
// compiling the program in Unicode mode. More precisely, it will be possible
// to construct wxString from a wide (Unicode) string and convert any wxString
// to Unicode.
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_WCHAR_T 1

// ----------------------------------------------------------------------------
// global features
// ----------------------------------------------------------------------------

// Support for message/error logging. This includes wxLogXXX() functions and
// wxLog and derived classes. Don't set this to 0 unless you really know what
// you are doing.
//
// Default is 1
//
// Recommended setting: 1 (always)
#define wxUSE_LOG 1

// Support for multithreaded applications: if 1, compile in thread classes
// (thread.h) and make the library a bit more thread safe. Although thread
// support is quite stable by now, you may still consider recompiling the
// library without it if you have no use for it - this will result in a
// somewhat smaller and faster operation.
//
// This is ignored under Win16, threads are only supported under Win32.
//
// Default is 1
//
// Recommended setting: 0 unless you do plan to develop MT applications
#define wxUSE_THREADS 1

// If enabled (1), compiles wxWindows streams classes
#define wxUSE_STREAMS       1

// Use standard C++ streams if 1. If 0, use wxWin streams implementation.
#define wxUSE_STD_IOSTREAM  0

// Use serialization (requires utils/serialize)
#define wxUSE_SERIAL        0

// ----------------------------------------------------------------------------
// non GUI features selection
// ----------------------------------------------------------------------------

// Set wxUSE_LONGLONG to 1 to compile the wxLongLong class. This is a 64 bit
// integer which is implemented in terms of native 64 bit integers if any or
// uses emulation otherwise.
//
// This class is required by wxDateTime and so you should enable it if you want
// to use wxDateTime. For most modern platforms, it will use the native 64 bit
// integers in which case (almost) all of its functions are inline and it
// almost does not take any space, so there should be no reason to switch it
// off.
//
// Recommended setting: 1
#define wxUSE_LONGLONG      1

// Set wxUSE_TIMEDATE to 1 to compile the wxDateTime and related classes which
// allow to manipulate dates, times and time intervals. wxDateTime replaces the
// old wxTime and wxDate classes which are still provided for backwards
// compatibility (and implemented in terms of wxDateTime).
//
// Note that this class is relatively new and is still officially in alpha
// stage because some features are not yet (fully) implemented. It is already
// quite useful though and should only be disabled if you are aiming at
// absolutely minimal version of the library.
//
// Requires: wxUSE_LONGLONG
//
// Recommended setting: 1
#define wxUSE_TIMEDATE      1

// Setting wxUSE_CONFIG to 1 enables the use of wxConfig and related classes
// which allow the application to store its settings in the persistent
// storage. Setting this to 1 will also enable on-demand creation of the
// global config object in wxApp.
//
// See also wxUSE_CONFIG_NATIVE below.
//
// Recommended setting: 1
#define wxUSE_CONFIG           1

// If wxUSE_CONFIG is 1, you may choose to use either the native config
// classes under Windows (using .INI files under Win16 and the registry under
// Win32) or the portable text file format used by the config classes under
// Unix.
//
// Default is 1 to use native classes. Note that you may still use
// wxFileConfig even if you set this to 1 - just the config object created by
// default for the applications needs will be a wxRegConfig or wxIniConfig and
// not wxFileConfig.
//
// Recommended setting: 1
#define wxUSE_CONFIG_NATIVE   1

// If wxUSE_DIALUP_MANAGER is 1, compile in wxDialUpManager class which allows
// to connect/disconnect from the network and be notified whenever the dial-up
// network connection is established/terminated.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_DIALUP_MANAGER   1

// Compile in wxLibrary class for run-time DLL loading and function calling
//
// This setting is for Win32 only
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_DYNLIB_CLASS  1

// Set to 1 to use socket classes
#define wxUSE_SOCKETS       1

// Set to 1 to enable virtual file systems
// (needed by wxHTML)
#define wxUSE_FILESYSTEM    1
#define wxUSE_FS_ZIP        1
#define wxUSE_FS_INET       1

// input stream for reading from zip archives
#define wxUSE_ZIPSTREAM     1

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

#define wxUSE_DYNAMIC_CLASSES     1
                                // If 1, enables provision of run-time type information.
                                // NOW MANDATORY: don't change.

// ----------------------------------------------------------------------------
// Optional controls
// ----------------------------------------------------------------------------

// wxToolBar related settings: if wxUSE_TOOLBAR is 0, don't compile any toolbar
// classes at all. Otherwise, use the native toolbar class unless
// wxUSE_TOOLBAR_NATIVE is 0. Additionally, the generic toolbar class which
// supports some features which might not be supported by the native wxToolBar
// class may be compiled in if wxUSE_TOOLBAR_SIMPLE is 1.
//
// Default is 1 for all settings.
//
// Recommended setting: 1 for wxUSE_TOOLBAR and wxUSE_TOOLBAR_NATIVE and 0 for
// wxUSE_TOOLBAR_SIMPLE (the default is 1 mainly for backwards compatibility).
#define wxUSE_TOOLBAR 1
#define wxUSE_TOOLBAR_NATIVE 1
#define wxUSE_TOOLBAR_SIMPLE 1

// wxNotebook is a control with several "tabs" located on one of its sides. It
// may be used ot logically organise the data presented to the user instead of
// putting everything in one huge dialog. It replaces wxTabControl and related
// classes of wxWin 1.6x.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_NOTEBOOK 1

// The corresponding controls will be compiled in if wxUSE_<CONTROL> is set to
// 1 and not compiled into the library otherwise.
//
// Default is 1 for everything.
//
// Recommended setting: 1 (library might fail to compile for some combinations
// of disabled controls)
#define wxUSE_CARET        1
#define wxUSE_CHECKBOX     1
#define wxUSE_CHECKLISTBOX 1
#define wxUSE_CHOICE       1
#define wxUSE_COMBOBOX     1
#define wxUSE_GAUGE        1
#define wxUSE_LISTBOX      1
#define wxUSE_RADIOBOX     1
#define wxUSE_RADIOBTN     1
#define wxUSE_SASH         1        // wxSashWindow
#define wxUSE_SCROLLBAR    1
#define wxUSE_SLIDER       1
#define wxUSE_SPINBTN      1
#define wxUSE_SPINCTRL     1
#define wxUSE_STATLINE     1
#define wxUSE_STATUSBAR    1
#define wxUSE_TOOLTIPS     1        // wxToolTip and wxWindow::SetToolTip()

// Two status bar implementations are available under Win32: the generic one
// or the wrapper around native control. For native look and feel the native
// version should be used.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_NATIVE_STATUSBAR        1

// this setting is obsolete, value is ignored
#define wxUSE_BUTTONBAR    1

// wxGrid class comes in two flavours: the original (pre wxWin 2.2) one and
// the new, much imporved and enhanced version. The new version is backwards
// compatible with the old one and should be used whenever possible, i.e. if
// you set wxUSE_GRID to 1, set wxUSE_NEW_GRID to 1 too.
//
// Default is 1 for both options.
//
// Recommended setting: 1 for wxUSE_NEW_GRID, 0 if you have an old code using
// wxGrid and 100% backwards compatible (with all old wxGrid quirks) is
// essential.
//
// WIN16/BC++ resets wxUSE_NEW_GRID to 0 because it exceeds the data limit.
#define wxUSE_GRID         1
#define wxUSE_NEW_GRID     1

// wxValidator class and related methods
#define wxUSE_VALIDATORS 1

// wxAcceleratorTable/Entry classes and support for them in wxMenu(Bar)
#define wxUSE_ACCEL 1

// ----------------------------------------------------------------------------
// common dialogs
// ----------------------------------------------------------------------------

// Define 1 to use generic dialogs in Windows, even though they duplicate
// native common dialog (e.g. wxColourDialog). This is mainly useful for
// testing.
//
// Default is 0
//
// Recommended setting: 0
#define wxUSE_GENERIC_DIALOGS_IN_MSW 0

// On rare occasions (e.g. using DJGPP) may want to omit common dialogs (e.g.
// file selector, printer dialog). Switching this off also switches off the
// printing architecture and interactive wxPrinterDC.
//
// Default is 1
//
// Recommended setting: 1 (unless it really doesn't work)
#define wxUSE_COMMON_DIALOGS 1

// text entry dialog and wxGetTextFromUser function
#define wxUSE_TEXTDLG 1

// progress dialog class for lengthy operations
#define wxUSE_PROGRESSDLG 1

// wxBusyInfo displays window with message when app is busy. Works in same way
// as wxBusyCursor
#define wxUSE_BUSYINFO      1

// wxDirDlg class for getting a directory name from user
#define wxUSE_DIRDLG 1

// support for startup tips (wxShowTip &c)
#define wxUSE_STARTUP_TIPS 1

// ----------------------------------------------------------------------------
// Metafiles support
// ----------------------------------------------------------------------------

// Windows supports the graphics format known as metafile which is, though not
// portable, is widely used under Windows and so is supported by wxWin (under
// Windows only, of course). Win16 (Win3.1) used the so-called "Window
// MetaFiles" or WMFs which were replaced with "Enhanced MetaFiles" or EMFs in
// Win32 (Win9x, NT, 2000). Both of these are supported in wxWin and, by
// default, WMFs will be used under Win16 and EMFs under Win32. This may be
// changed by setting wxUSE_WIN_METAFILES_ALWAYS to 1 and/or setting
// wxUSE_ENH_METAFILE to 0. You may also set wxUSE_METAFILE to 0 to not compile
// in any metafile related classes at all.
//
// Default is 1 for wxUSE_ENH_METAFILE and 0 for wxUSE_WIN_METAFILES_ALWAYS.
//
// Recommended setting: default or 0 for everything for portable programs.
#define wxUSE_METAFILE              1
#define wxUSE_ENH_METAFILE          1
#define wxUSE_WIN_METAFILES_ALWAYS  0

// ----------------------------------------------------------------------------
// Big GUI components
// ----------------------------------------------------------------------------

// Set to 0 to disable document/view architecture
#define wxUSE_DOC_VIEW_ARCHITECTURE 1

// Set to 0 to disable MDI document/view architecture
#define wxUSE_MDI_ARCHITECTURE    1

// Set to 0 to disable print/preview architecture code
#define wxUSE_PRINTING_ARCHITECTURE  1

// wxHTML sublibrary allows to display HTML in wxWindow programs and much,
// much more.
//
// Default is 1.
//
// Recommended setting: 1 (wxHTML is great!), set to 0 if you want compile a
// smaller library.
#define wxUSE_HTML          1

// wxPlot is a class to display functions plots in wxWindow.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_PLOT           1

// OpenGL canvas
#define wxUSE_GLCANVAS       0

// wxTreeLayout class
#define wxUSE_TREELAYOUT     1

// ----------------------------------------------------------------------------
// miscellaneous settings
// ----------------------------------------------------------------------------

#define wxUSE_IPC         1
                                // 0 for no interprocess comms
#define wxUSE_HELP        1
                                // 0 for no help facility
#define wxUSE_MS_HTML_HELP 0
                                // 0 for no MS HTML Help
#define wxUSE_RESOURCES   1
                                // 0 for no wxGetResource/wxWriteResource
#define wxUSE_CONSTRAINTS 1
                                // 0 for no window layout constraint system

#define wxUSE_CLIPBOARD   1
                                // 0 for no clipboard functions

#define wxUSE_SPLINES     1
                                // 0 for no splines

#define wxUSE_DRAG_AND_DROP 1
                                // 0 for no drag and drop

#define wxUSE_XPM_IN_MSW   1
                                // 0 for no XPM support in wxBitmap.
                                // Default is 1, as XPM is now fully
                                // supported this makes easier the issue
                                // of portable icons and bitmaps.
#define wxUSE_IMAGE_LOADING_IN_MSW        1
                                // Use dynamic DIB loading/saving code in utils/dib under MSW.
#define wxUSE_RESOURCE_LOADING_IN_MSW     1
                                // Use dynamic icon/cursor loading/saving code
                                // under MSW.
#define wxUSE_WX_RESOURCES        1
                                // Use .wxr resource mechanism (requires PrologIO library)

// ----------------------------------------------------------------------------
// Postscript support settings
// ----------------------------------------------------------------------------

#define wxUSE_POSTSCRIPT  0
                                // 0 for no PostScript device context
#define wxUSE_AFM_FOR_POSTSCRIPT 0
                                // 1 to use font metric files in GetTextExtent

#define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1
                                // Set to 0 to disable PostScript print/preview architecture code
                                // under Windows (just use Windows printing).

// ----------------------------------------------------------------------------
// database classes
// ----------------------------------------------------------------------------

#define wxUSE_ODBC          0
                                // Define 1 to use ODBC classes

#define wxODBC_FWD_ONLY_CURSORS	 1
                                // For backward compatibility reasons, this parameter now only
                                // controls the default scrolling method used by cursors.  This
                                // default behavior can be overriden by setting the second param
                                // of wxDB::GetDbConnection() to indicate whether the connection
                                // (and any wxTable()s that use the connection) should support
                                // forward only scrolling of cursors, or both forward and backward
                                // Support for backward scrolling cursors is dependent on the
                                // data source as well as the ODBC driver being used.

#define wxODBC_BACKWARD_COMPATABILITY 0
                                // Default is 0.  Set to 1 to use the deprecated classes, enum
                                // types, function, member variables.  With a setting of 1, full
                                // backward compatability with the 2.0.x release is possible.
                                // It is STRONGLY recommended that this be set to 0, as 
                                // future development will be done only on the non-deprecated
                                // functions/classes/member variables/etc.

// ----------------------------------------------------------------------------
// other compiler (mis)features
// ----------------------------------------------------------------------------

// Set this to 0 if your compiler can't cope with omission of prototype
// parameters.
//
// Default is 1.
//
// Recommended setting: 1 (should never need to set this to 0)
#define REMOVE_UNUSED_ARG   1

// VC++ 4.2 and above allows <iostream> and <iostream.h> but you can't mix
// them. Set to 1 for <iostream.h>, 0 for <iostream>
//
// Default is 1.
//
// Recommended setting: whatever your compiler likes more
#define wxUSE_IOSTREAMH     1

// ----------------------------------------------------------------------------
// image format support
// ----------------------------------------------------------------------------

#define wxUSE_ZLIB          1
                                // Use zlib for compression in streams and PNG code
#define wxUSE_LIBPNG        1
                                // Use PNG bitmap/image code
#define wxUSE_LIBJPEG       1
                                // Use JPEG bitmap/image code
#define wxUSE_LIBTIFF       1
                                // Use TIFF bitmap/image code
#define wxUSE_GIF           1
                                // Use GIF bitmap/image code
#define wxUSE_PNM           1
                                // Use PNM bitmap/image code
#define wxUSE_PCX           1
                                // Use PCX bitmap/image code

// ----------------------------------------------------------------------------
// Windows-only settings
// ----------------------------------------------------------------------------

// Make settings compatible with MFC
#define wxUSE_MFC           0

// required for drag-and-drop, clipboard, OLE Automation
#define wxUSE_OLE           1

#if defined(__WIN95__)
#define wxUSE_CTL3D                      0
#else
// Define 1 to use Microsoft CTL3D library.
#define wxUSE_CTL3D                      1
#endif

// can we use RICHEDIT control?
#if defined(__WIN95__) && !defined(__TWIN32__) && !defined(__GNUWIN32_OLD__)
#define wxUSE_RICHEDIT 1
#else
#define wxUSE_RICHEDIT 0
#endif

#define wxUSE_ITSY_BITSY             1
                                // Define 1 to use Microsoft's ItsyBitsy
                                // small title bar library, for wxMiniFrame.
                                // This setting is only used for Win3.1;
                                // Win9x and NT use native miniframes
                                // support instead.
#define wxUSE_BITMAP_MESSAGE         1
                                // Define 1 to use bitmap messages.
#define wxFONT_SIZE_COMPATIBILITY    0
                                // Define 1 for font size to be backward compatible
                                // to 1.63 and earlier. 1.64 and later define point
                                // sizes to be compatible with Windows.
#define wxUSE_PENWINDOWS             0
                                // Set to 1 to use PenWindows

#define wxUSE_OWNER_DRAWN             1
                                // Owner-drawn menus and listboxes

#if defined(__MINGW32__) && ((__GNUC__>2) ||((__GNUC__==2) && (__GNUC_MINOR__>=95)))
#ifndef wxUSE_NORLANDER_HEADERS
#   define wxUSE_NORLANDER_HEADERS 1
#endif
#endif

// ----------------------------------------------------------------------------
// disable the settings which don't work for some compilers
// ----------------------------------------------------------------------------

#if defined(__GNUWIN32__)
// These don't work as expected for mingw32 and cygwin32
#undef  wxUSE_MEMORY_TRACING
#define wxUSE_MEMORY_TRACING            0

#undef  wxUSE_GLOBAL_MEMORY_OPERATORS
#define wxUSE_GLOBAL_MEMORY_OPERATORS   0

#undef  wxUSE_DEBUG_NEW_ALWAYS
#define wxUSE_DEBUG_NEW_ALWAYS          0

#undef wxUSE_MS_HTML_HELP
#define wxUSE_MS_HTML_HELP 0
#endif // __GNUWIN32__

// MFC duplicates these operators
#if wxUSE_MFC
#undef  wxUSE_GLOBAL_MEMORY_OPERATORS
#define wxUSE_GLOBAL_MEMORY_OPERATORS   0

#undef  wxUSE_DEBUG_NEW_ALWAYS
#define wxUSE_DEBUG_NEW_ALWAYS          0
#endif // wxUSE_MFC

// ODBC classes aren't Unicode-compatible yet
#if wxUSE_UNICODE
#undef wxUSE_ODBC
#define wxUSE_ODBC 0
#endif

#if (!defined(WIN32) && !defined(__WIN32__)) || (defined(__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS)
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

#if !wxUSE_OWNER_DRAWN
#undef wxUSE_CHECKLISTBOX
#define wxUSE_CHECKLISTBOX 0
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

#undef wxUSE_OWNER_DRAWN
#define wxUSE_OWNER_DRAWN 0
#endif // __SALFORDC__

#ifdef __TWIN32__

#undef wxUSE_THREADS
#define wxUSE_THREADS 0

#undef wxUSE_ODBC
#define wxUSE_ODBC 0

#endif // __TWIN32__

// BC++/Win16 can't cope with the amount of data in resource.cpp
#if defined(__WIN16__) && defined(__BORLANDC__)
#undef wxUSE_WX_RESOURCES
#define wxUSE_WX_RESOURCES        0

#undef wxUSE_ODBC
#define wxUSE_ODBC                0

#undef wxUSE_NEW_GRID
#define wxUSE_NEW_GRID            0
#endif

#if defined(__BORLANDC__) && (__BORLANDC__ < 0x500)
// BC++ 4.0 can't compile JPEG library
#undef wxUSE_LIBJPEG
#define wxUSE_LIBJPEG 0
#endif

#if defined(__BORLANDC__)
// Need a BC++-specific htmlhelp.lib before we can enable this
#undef wxUSE_MS_HTML_HELP
#define wxUSE_MS_HTML_HELP 0
#endif

#if defined(__WXMSW__) && defined(__WATCOMC__)
#undef wxUSE_LIBJPEG
#define wxUSE_LIBJPEG 0

#undef wxUSE_LIBTIFF
#define wxUSE_LIBTIFF 0

#undef  wxUSE_GLCANVAS
#define wxUSE_GLCANVAS 0

#undef wxUSE_MS_HTML_HELP
#define wxUSE_MS_HTML_HELP 0
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

#undef wxUSE_LIBTIFF
#define wxUSE_LIBTIFF 0

#undef wxUSE_GIF
#define wxUSE_GIF 0

#undef wxUSE_PNM
#define wxUSE_PNM 0

#undef wxUSE_PCX
#define wxUSE_PCX 0

#undef wxUSE_GLCANVAS
#define wxUSE_GLCANVAS 0

#undef wxUSE_MS_HTML_HELP
#define wxUSE_MS_HTML_HELP 0
#endif // Win16

// ----------------------------------------------------------------------------
// check the settings consistency: do it here to abort compilation immediately
// and not almost in the very end when the relevant file fails to compile and
// you need to modify setup.h and rebuild everything
// ----------------------------------------------------------------------------

#if wxUSE_TIMEDATE && !wxUSE_LONGLONG
    #error wxDateTime requires wxLongLong
#endif

#if wxUSE_TEXTFILE && !wxUSE_FILE
    #error You cannot compile wxTextFile without wxFile
#endif

#if wxUSE_FILESYSTEM && !wxUSE_STREAMS
    #error You cannot compile virtual file systems without wxUSE_STREAMS
#endif

#if wxUSE_HTML && !wxUSE_FILESYSTEM
    #error You cannot compile wxHTML without virtual file systems
#endif

// add more tests here...

#endif
    // _WX_SETUP_H_
