/////////////////////////////////////////////////////////////////////////////
// Name:        setup.h
// Purpose:     Configuration for the library
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETUP_H_
#define _WX_SETUP_H_

/*
 * General features
 *
 */

#define wxUSE_CONFIG           1
                                // Use wxConfig, with CreateConfig in wxApp
#define _WX_GOODCOMPILER__
                                // gcc can have problems, but Windows compilers
                                // are generally OK.
#define WXWIN_COMPATIBILITY  1
                                // Compatibility with 1.66 API.
                                // Level 0: no backward compatibility, all new features
                                // Level 1: wxDC, OnSize (etc.) compatibility, but
                                //          some new features such as event tables

#define wxUSE_AUTOTRANS	1
								// Define wxTString
#define wxUSE_POSTSCRIPT  1
                        	// 0 for no PostScript device context
#define wxUSE_AFM_FOR_POSTSCRIPT 0
                                // 1 to use font metric files in GetTextExtent
#define wxUSE_METAFILE    1
                                // 0 for no Metafile and metafile device context
#define wxUSE_FORM        0
                                // 0 for no wxForm
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
#define wxUSE_XFIG_SPLINE_CODE  1
                           	// 1 for XFIG spline code, 0 for AIAI spline code.
// AIAI spline code is slower, but freer of copyright issues.

#define wxUSE_DRAG_AND_DROP 1
                        	// 0 for no drag and drop

#define wxUSE_TOOLBAR       1
                                    // Define 1 to use toolbar classes
#define wxUSE_BUTTONBAR     1
                                    // Define 1 to use buttonbar classes (enhanced toolbar
                                    // for MS Windows)
#define wxUSE_GAUGE         1
                                    // Define 1 to use Microsoft's gauge (Windows)
                                    // or Bull's gauge (Motif) library (both in contrib).
#define wxUSE_COMBOBOX      1
                                    // Define 1 to use COMBOXBOX control (Windows)
                                    // or FWW's ComboBox widget (Motif).
#define wxUSE_RADIOBUTTON   1
                                    // Define 1 to use radio button control

#define wxUSE_SCROLLBAR   1
                                    // Define 1 to compile contributed wxScrollBar class
#define wxUSE_XPM_IN_X    1
#define wxUSE_XPM_IN_MSW  0
                                    // Define 1 to support the XPM package in wxBitmap,
                                    // separated by platform. If 1, you must link in
                                    // the XPM library to your applications.
#define wxUSE_IMAGE_LOADING_IN_X          1
                                  // Use dynamic icon/bitmap loading/saving code in utils/image under X.
                                  // If this is 1, you will need to link your applications
                                  // with image_X.lib. where X is motif, ol, or hp.

#define wxUSE_IMAGE_LOADING_IN_MSW        1
                                  // Use dynamic DIB loading/saving code in utils/dib under MSW.
#define wxUSE_RESOURCE_LOADING_IN_MSW     1
                                  // Use dynamic icon/cursor loading/saving code
                                  // under MSW.
#define wxUSE_WX_RESOURCES        1
                                  // Use .wxr resource mechanism (requires PrologIO library)

#define wxUSE_GNU_WXSTRING        0
                                  // Define 1 to use modified GNU wxString class
                                  // from (stefan.hammes@urz.uni-heidelberg.de) in contrib\string
                                  // TODO: why does this give an unresolved 'wxRegex::Search'
                                  // symbol if 1?

#define HAVE_SOCKET             1
                                  // Use WinSock if 1                                 
#define wxUSE_DOC_VIEW_ARCHITECTURE 1
                                  // Set to 0 to disable document/view architecture
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
                                  // wxObject::delete *IF* WXDEBUG is also defined.
                                  // WARNING: this code may not work with all architectures, especially
                                  // if alignment is an issue.
#define wxUSE_DEBUG_CONTEXT       1
                                  // If 1, enables wxDebugContext, for
                                  // writing error messages to file, etc. 
                                  // If WXDEBUG is not defined, will still use
                                  // normal memory operators.
                                  // It's recommended to set this to 1,
                                  // since you may well need to output
                                  // an error log in a production
                                  // version (or non-debugging beta)
#define wxUSE_GLOBAL_MEMORY_OPERATORS 1
                                  // In debug mode, cause new and delete to be redefined globally.
                                  // If this causes problems (e.g. link errors), set this to 0.

#define REMOVE_UNUSED_ARG 1
                                  // Set this to 0 if your compiler can't cope
                                  // with omission of prototype parameters.

#define wxUSE_C_MAIN 0
                                  // Set to 1 to use main.c instead of main.cpp (UNIX only)

#define wxUSE_ODBC                   1
                                    // Define 1 to use ODBC classes

#define wxUSE_IOSTREAMH     1
                                  // VC++ 4.2 and above allows <iostream> and <iostream.h>
                                  // but you can't mix them. Set to 1 for <iostream.h>,
                                  // 0 for <iostream>

#define wxUSE_WXCONFIG      1
                                  // if enabled, compiles built-in OS independent wxConfig
                                  // class and it's file (any platform) and registry (Win)
                                  // based implementations
/*
 * Finer detail
 *
 */

#define wxUSE_APPLE_IEEE	  1
				// if enabled, the float codec written by Apple
				// will be used to write, in a portable way,
				// float on the disk

/*
 * MS Windows/Windows NT
 *
 */

#if defined(__WIN95__)
#define CTL3D                      0
#else
#define CTL3D                      1
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
#define FONT_SIZE_COMPATIBILITY    0
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

#if (!defined(WIN32) && !defined(__WIN32__)) || defined(__GNUWIN32__) || defined(__BORLANDC__)
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

// Minimal setup e.g. for compiling small utilities
#define MINIMAL_WXWINDOWS_SETUP 0

#if MINIMAL_WXWINDOWS_SETUP
#undef wxUSE_POSTSCRIPT
# define wxUSE_POSTSCRIPT 0
#undef wxUSE_PRINTING_ARCHITECTURE
# define wxUSE_PRINTING_ARCHITECTURE 0
#undef wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW
# define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW 0
#undef wxUSE_METAFILE
# define wxUSE_METAFILE 0
#undef wxUSE_FORM
# define wxUSE_FORM 0
#undef wxUSE_SPLINES
# define wxUSE_SPLINES 0
#undef wxUSE_SCROLLBAR
# define wxUSE_SCROLLBAR 0
#undef wxUSE_COMBOBOX
# define wxUSE_COMBOBOX 0
#undef wxUSE_RADIOBUTTON
# define wxUSE_RADIOBUTTON 0
#undef wxUSE_XPM_IN_MSW
# define wxUSE_XPM_IN_MSW 0
#undef wxUSE_WX_RESOURCES
# define wxUSE_WX_RESOURCES 0
#undef wxUSE_DOC_VIEW_ARCHITECTURE
# define wxUSE_DOC_VIEW_ARCHITECTURE 0
#undef wxUSE_GNU_WXSTRING
# define wxUSE_GNU_WXSTRING 0
#undef wxUSE_ODBC
# define wxUSE_ODBC 0
#undef wxUSE_TIMEDATE
# define wxUSE_TIMEDATE 0
#undef CTL3D
# define CTL3D 0
#undef wxUSE_ITSY_BITSY
# define wxUSE_ITSY_BITSY 0
#undef wxUSE_IMAGE_LOADING_IN_MSW
# define wxUSE_IMAGE_LOADING_IN_MSW 0
#undef wxUSE_GAUGE
# define wxUSE_GAUGE 0
#undef wxUSE_RESOURCE_LOADING_IN_MSW
# define wxUSE_RESOURCE_LOADING_IN_MSW 0
#undef wxUSE_DRAG_AND_DROP
# define wxUSE_DRAG_AND_DROP 0
#endif

#endif
    // _WX_SETUP_H_
