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

// define this to 1 if you want icons to behave as bitmaps
#define wxICON_IS_BITMAP     0

// define this to 0 when building wxBase library
#define wxUSE_GUI            1

// ----------------------------------------------------------------------------
// General features
// ----------------------------------------------------------------------------

#define wxUSE_FILESYSTEM       1 // better have this is you want to compile wxHTML

#define wxUSE_PCX              1 // Utilize PCX images

#define wxUSE_PNM              1 // Utilize PNM images

#define wxUSE_GIF              1 // Utilize GIF images

#if defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )
#define wxUSE_LONGLONG        1
#endif

#define wxUSE_NET_API          1 // Utilize OS/2's UPM netapi's

#define wxUSE_CONFIG           1
                                // Use wxConfig, with CreateConfig in wxApp

#define wxUSE_POSTSCRIPT  1
                            // 0 for no PostScript device context
#define wxUSE_AFM_FOR_POSTSCRIPT 1
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

#define wxUSE_PLOT        1
                                // 0 for no generic plot functions

#define wxUSE_SPLINES     1
                                    // 0 for no splines

#define wxUSE_DRAG_AND_DROP 1
                                // 0 for no drag and drop

#define wxUSE_BUTTONBAR     1
                                    // Define 1 to use buttonbar classes (enhanced toolbar
                                    // for MS Windows)
#define wxUSE_GAUGE         1
                                    // Define 1 to use Microsoft's gauge (Windows)
                                    // or Bull's gauge (Motif) library (both in contrib).
#define wxUSE_COMBOBOX      1
                                    // Define 1 to use COMBOXBOX control (Windows)
                                    // or FWW's ComboBox widget (Motif).
#define wxUSE_CHOICE      1
                                    // Define 1 to use CHOICE

#define wxUSE_RADIOBUTTON   1
                                    // Define 1 to use radio button control
#define wxUSE_RADIOBTN   1
                                    // Unfortunately someone introduced this one, too

#define wxUSE_SCROLLBAR   1
                                    // Define 1 to compile contributed wxScrollBar class

#define wxUSE_CHECKBOX 1
                                    // Define 1 to compile checkbox

#define wxUSE_LISTBOX 1
                                    // Define 1 to compile listbox

#define wxUSE_SPINBTN 1
                                    // Define 1 to compile spin button

// use wxStaticLine class (separator line in the dialog)?
#define wxUSE_STATLINE 1

#define wxUSE_CHECKLISTBOX 1
                                    // Define 1 to compile check listbox
#define wxUSE_CARET       1
                                    // Define 1 to use wxCaret class
#define wxUSE_XPM_IN_OS2  1
                                    // Define 1 to support the XPM package in wxBitmap.
#define wxUSE_IMAGE_LOADING_IN_OS2        1
                                    // Use dynamic DIB loading/saving code in utils/dib under OS2.
#define wxUSE_RESOURCE_LOADING_IN_OS2     1
                                    // Use dynamic icon/cursor loading/saving code
                                    // under OS2.
#define wxUSE_WX_RESOURCES        1
                                  // Use .wxr resource mechanism (requires PrologIO library)

// support for startup tips (wxShowTip &c)
#define wxUSE_STARTUP_TIPS       1

#define wxUSE_DOC_VIEW_ARCHITECTURE 1
                                  // Set to 0 to disable document/view architecture
#define wxUSE_MDI_ARCHITECTURE  1
                                  // Set to 0 to disable MDI document/view architecture
#define wxUSE_PRINTING_ARCHITECTURE  1
                                    // Set to 0 to disable print/preview architecture code
#define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_OS2 1
                                // Set to 0 to disable PostScript print/preview architecture code
                                // under OS/2 PM (just use PM printing).
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
#if defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )
#define wxUSE_GLOBAL_MEMORY_OPERATORS 0
#else
#define wxUSE_GLOBAL_MEMORY_OPERATORS 1
#endif
                                  // In debug mode, cause new and delete to be redefined globally.
                                  // If this causes problems (e.g. link errors), set this to 0.

#define wxUSE_DEBUG_NEW_ALWAYS 1
                                  // In debug mode, causes new to be defined to
                                  // be WXDEBUG_NEW (see object.h).
                                  // If this causes problems (e.g. link errors), set this to 0.
                                  // You may need to set this to 0 if using templates (at least
                                  // for VC++).

#define REMOVE_UNUSED_ARG 1
                                  // Set this to 0 if your compiler can't cope
                                  // with omission of prototype parameters.

#define wxUSE_ODBC                1
                                  // Define 1 to use ODBC classes

#define wxODBC_FWD_ONLY_CURSORS	1
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

#define wxUSE_IOSTREAMH     1
                                  // VC++ 4.2 and above allows <iostream> and <iostream.h>
                                  // but you can't mix them. Set to 1 for <iostream.h>,
                                  // 0 for <iostream>

#define wxUSE_STREAMS       1
                                  // If enabled (1), compiles wxWindows streams classes

#define wxUSE_STD_IOSTREAM  1
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
                                  // Use PNG bitmap code
#define wxUSE_LIBJPEG       1
                                  // Use JPEG bitmap code
#define wxUSE_SERIAL        1
                                  // Use serialization (requires utils/serialize)
#define wxUSE_DYNLIB_CLASS  1
                                  // Compile in wxLibrary class for run-time
                                  // DLL loading and function calling
#define wxUSE_TOOLTIPS      1
                                  // Define to use wxToolTip class and
                                  // wxWindow::SetToolTip() method
#define wxUSE_SOCKETS       1
                                  // Set to 1 to use socket classes
#define wxUSE_HTML          1
                                  // Set to 1 to use wxHTML sub-library
#define wxUSE_FS_ZIP        1
#define wxUSE_FS_INET       1     // Set to 1 to enable virtual file systems

#define wxUSE_BUSYINFO      1
                                  // wxBusyInfo displays window with message
                                  // when app is busy. Works in same way as
                                  // wxBusyCursor
#define wxUSE_ZIPSTREAM     1
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
#define wxUSE_LOG                   1

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
#define wxUSE_TOOLBAR_NATIVE 1

// wxStatusBar class
#define wxUSE_STATUSBAR 1

// wxSpinCtrl class
#define wxUSE_SPINCTRL 1

// progress dialog class for lengthy operations
#define wxUSE_PROGRESSDLG 1

// wxDirDlg class for getting a directory name from user
#define wxUSE_DIRDLG 1

#define wxUSE_OWNER_DRAWN             1
                                    // Owner-drawn menus and listboxes

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

#define wxUSE_PORTABLE_FONTS_IN_OS2  0
                                // Define 1 to use new portable font scheme in Windows
                                // (used by default under X)

#define wxUSE_GENERIC_DIALOGS_IN_OS2 0
                                // Define 1 to use generic dialogs in Windows, even though
                                // they duplicate native common dialog (e.g. wxColourDialog)
#endif
    // _WX_SETUP_H_
