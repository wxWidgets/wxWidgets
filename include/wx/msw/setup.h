/////////////////////////////////////////////////////////////////////////////
// Name:        setup.h
// Purpose:     Configuration for the library
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __SETUPH__
#define __SETUPH__

/*
 * General features
 *
 */

#define WXWIN_COMPATIBILITY  1
                                // Compatibility with 1.66 API.
                                // Level 0: no backward compatibility, all new features
                                // Level 1: wxDC, OnSize (etc.) compatibility, but
                                //          some new features such as event tables

#define USE_AUTOTRANS	1
								// Define wxTString
#define USE_POSTSCRIPT  1
                        	// 0 for no PostScript device context
#define USE_AFM_FOR_POSTSCRIPT 0
                                // 1 to use font metric files in GetTextExtent
#define USE_METAFILE    1
                                // 0 for no Metafile and metafile device context
#define USE_FORM        0
                                // 0 for no wxForm
#define USE_IPC         1
                                // 0 for no interprocess comms
// Note: wxHELP uses IPC under X so these are interdependent!
#define USE_HELP        1
                                // 0 for no help facility
#define USE_RESOURCES   1
                        	// 0 for no wxGetResource/wxWriteResource
#define USE_CONSTRAINTS 1
                                // 0 for no window layout constraint system
                                
#define USE_TIMEDATE    1
                                // 0 for no wxTime/wxDate classes
                                
#define USE_CLIPBOARD   1
                        	// 0 for no clipboard functions
#define USE_SPLINES     1
                            	// 0 for no splines
#define USE_XFIG_SPLINE_CODE  1
                           	// 1 for XFIG spline code, 0 for AIAI spline code.
// AIAI spline code is slower, but freer of copyright issues.

#define USE_DRAG_AND_DROP 1
                        	// 0 for no drag and drop

#define USE_TOOLBAR       1
                                    // Define 1 to use toolbar classes
#define USE_BUTTONBAR     1
                                    // Define 1 to use buttonbar classes (enhanced toolbar
                                    // for MS Windows)
#define USE_GAUGE         1
                                    // Define 1 to use Microsoft's gauge (Windows)
                                    // or Bull's gauge (Motif) library (both in contrib).
#define USE_COMBOBOX      1
                                    // Define 1 to use COMBOXBOX control (Windows)
                                    // or FWW's ComboBox widget (Motif).
#define USE_RADIOBUTTON   1
                                    // Define 1 to use radio button control

#define USE_SCROLLBAR   1
                                    // Define 1 to compile contributed wxScrollBar class
#define USE_XPM_IN_X    1
#define USE_XPM_IN_MSW  0
                                    // Define 1 to support the XPM package in wxBitmap,
                                    // separated by platform. If 1, you must link in
                                    // the XPM library to your applications.
#define USE_IMAGE_LOADING_IN_X          1
                                  // Use dynamic icon/bitmap loading/saving code in utils/image under X.
                                  // If this is 1, you will need to link your applications
                                  // with image_X.lib. where X is motif, ol, or hp.

#define USE_IMAGE_LOADING_IN_MSW        1
                                  // Use dynamic DIB loading/saving code in utils/dib under MSW.
#define USE_RESOURCE_LOADING_IN_MSW     0
                                  // Use dynamic icon/cursor loading/saving code
                                  // under MSW.
#define USE_WX_RESOURCES        1
                                  // Use .wxr resource mechanism (requires PrologIO library)

#define USE_GNU_WXSTRING        0
                                  // Define 1 to use modified GNU wxString class
                                  // from (stefan.hammes@urz.uni-heidelberg.de) in contrib\string
                                  // TODO: why does this give an unresolved 'wxRegex::Search'
                                  // symbol if 1?

#define HAVE_SOCKET             1
                                  // Use WinSock if 1                                 
#define USE_DOC_VIEW_ARCHITECTURE 1
                                  // Set to 0 to disable document/view architecture
#define USE_PRINTING_ARCHITECTURE  1
                                    // Set to 0 to disable print/preview architecture code
#define USE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1
                                    // Set to 0 to disable PostScript print/preview architecture code
                                    // under Windows (just use Windows printing).
#define USE_DYNAMIC_CLASSES     1
                                  // If 1, enables provision of run-time type information.
                                  // NOW MANDATORY: don't change.
#define USE_MEMORY_TRACING      1
                                  // If 1, enables debugging versions of wxObject::new and
                                  // wxObject::delete *IF* DEBUG is also defined.
                                  // WARNING: this code may not work with all architectures, especially
                                  // if alignment is an issue.
#define USE_DEBUG_CONTEXT       1
                                  // If 1, enables wxDebugContext, for
                                  // writing error messages to file, etc. 
                                  // If DEBUG is not defined, will still use
                                  // normal memory operators.
                                  // It's recommended to set this to 1,
                                  // since you may well need to output
                                  // an error log in a production
                                  // version (or non-debugging beta)
#define USE_GLOBAL_MEMORY_OPERATORS 1
                                  // In debug mode, cause new and delete to be redefined globally.
                                  // If this causes problems (e.g. link errors), set this to 0.

#define REMOVE_UNUSED_ARG 1
                                  // Set this to 0 if your compiler can't cope
                                  // with omission of prototype parameters.

#define USE_C_MAIN 0
                                  // Set to 1 to use main.c instead of main.cpp (UNIX only)

#define USE_ODBC                   1
                                    // Define 1 to use ODBC classes

#define USE_ODBC_IN_MSW_ONLY           1

#if USE_ODBC && USE_ODBC_IN_MSW_ONLY
#undef USE_ODBC
#define USE_ODBC 0
#endif

#define USE_IOSTREAMH     1
                                  // VC++ 4.2 and above allows <iostream> and <iostream.h>
                                  // but you can't mix them. Set to 1 for <iostream.h>,
                                  // 0 for <iostream>

/*
 * Finer detail
 *
 */

/*
 * Motif and XView
 *
 */

#define WX_STANDARD_GRAPHICS 0
                                  // If 1, normalizes X drawing code to behave exactly as
                                  // as MSW. If 0, is compatible with existing applications.
                                  // Some Xlib drawing primitives have non-intuitive behaviour!

#define USE_GADGETS                0
                                    // More efficient to use gadgets for some
                                    // widgets in Motif. 0 for no gadgets.
                                    // Please note: there is no reason to not
                                    // use it except if you intend to modify
                                    // color of individuals items OR
                                    // you need to move panel items interactively
#define USE_BUTTON_GADGET          0
                                    // On JACS's system, gadget buttons
                                    // interfere with default button setting.
#define PIXEL0_DISABLE             0
                                    // Define as 1 to disallow allocation
                                    // of pixel #0 (wxXOR problem).
                                    // JACS - I found this caused problems.

#define MOTIF_MENUBAR_DELETE_FIX   0
                                    // On some systems (Ultrix, OSF), deleting a frame
                                    // from within a menu callback causes a crash.
                                    // Set to 1 to avoid deleting the menubar handle directly,
                                    // which seems to cure it.
#define	DEFAULT_FILE_SELECTOR_SIZE 0
				    // Let Motif defines the size of File
				    // Selector Box (if 1), or fix it to
				    // wxFSB_WIDTH x wxFSB_HEIGHT (if 0)
#define	wxFSB_WIDTH                600
#define wxFSB_HEIGHT               500

#define	MOTIF_MANAGE               1
				    // Control default style of Dialogs
				    // 1: use wxMOTIF_RESIZE as default
				    // 0: do not use wxMOTIF_RESIZE as default


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

#define USE_COMMON_DIALOGS         1
                                    // On rare occasions (e.g. using DJGPP) may want
                                    // to omit common dialogs
                                    // (e.g. file selector, printer dialog).
                                    // Switching this off also switches off
                                    // the printing architecture and interactive
                                    // wxPrinterDC.
#define USE_GREY_BACKGROUND        1
                                    // If 1, uses grey (gray!) panels
                                    // in FAFA and non-FAFA, non-CTL3D modes.
                                    // I (JACS) think the controls look better
                                    // this way. CTL3D always uses grey panels.
#define USE_ITSY_BITSY             1
                                    // Define 1 to use Microsoft's ItsyBitsy
                                    // small title bar library
#define USE_BITMAP_MESSAGE         1
                                    // Define 1 to use bitmap messages.
#define USE_PORTABLE_FONTS_IN_MSW  0
                                    // Define 1 to use new portable font scheme in Windows
                                    // (used by default under X)
#define FONT_SIZE_COMPATIBILITY    0
                                    // Define 1 for font size to be backward compatible
                                    // to 1.63 and earlier. 1.64 and later define point
                                    // sizes to be compatible with Windows.
#define USE_GENERIC_DIALOGS_IN_MSW 1
                                    // Define 1 to use generic dialogs in Windows, even though
                                    // they duplicate native common dialog (e.g. wxColourDialog)
#define USE_PENWINDOWS 0
                                    // Set to 1 to use PenWindows

#define USE_OWNER_DRAWN             1
                                    // Owner-drawn menus and listboxes

#define USE_NATIVE_STATUSBAR        1
                                    // Set to 0 to use cross-platform wxStatusBar

/*
 * Any platform
 *
 */

#define USE_TYPEDEFS               0
                                    // Use typedefs not classes for wxPoint
				    // and others, to reduce overhead and avoid
				    // MS C7 memory bug. Bounds checker
				    // complains about deallocating
                                    // arrays of wxPoints if wxPoint is a class.

#if (!defined(WIN32) && !defined(__WIN32__)) || defined(__GNUWIN32__) || defined(__BORLANDC__)
// Can't use OLE drag and drop in Windows 3.1 because we don't know how
// to implement UUIDs
// GnuWin32 doesn't have appropriate headers for e.g. IUnknown.
#undef USE_DRAG_AND_DROP
#define USE_DRAG_AND_DROP 0
#endif

// Only WIN32 supports wxStatusBar95
#if !defined(__WIN32__) && USE_NATIVE_STATUSBAR
#undef  USE_NATIVE_STATUSBAR
#define USE_NATIVE_STATUSBAR 0
#endif

// Minimal setup e.g. for compiling small utilities
#define MINIMAL_WXWINDOWS_SETUP 0

#if MINIMAL_WXWINDOWS_SETUP
#undef USE_POSTSCRIPT
# define USE_POSTSCRIPT 0
#undef USE_PRINTING_ARCHITECTURE
# define USE_PRINTING_ARCHITECTURE 0
#undef USE_POSTSCRIPT_ARCHITECTURE_IN_MSW
# define USE_POSTSCRIPT_ARCHITECTURE_IN_MSW 0
#undef USE_METAFILE
# define USE_METAFILE 0
#undef USE_FORM
# define USE_FORM 0
#undef USE_SPLINES
# define USE_SPLINES 0
#undef USE_SCROLLBAR
# define USE_SCROLLBAR 0
#undef USE_COMBOBOX
# define USE_COMBOBOX 0
#undef USE_RADIOBUTTON
# define USE_RADIOBUTTON 0
#undef USE_XPM_IN_MSW
# define USE_XPM_IN_MSW 0
#undef USE_WX_RESOURCES
# define USE_WX_RESOURCES 0
#undef USE_DOC_VIEW_ARCHITECTURE
# define USE_DOC_VIEW_ARCHITECTURE 0
#undef USE_GNU_WXSTRING
# define USE_GNU_WXSTRING 0
#undef USE_ODBC
# define USE_ODBC 0
#undef USE_TIMEDATE
# define USE_TIMEDATE 0
#undef CTL3D
# define CTL3D 0
#undef USE_ITSY_BITSY
# define USE_ITSY_BITSY 0
#undef USE_IMAGE_LOADING_IN_MSW
# define USE_IMAGE_LOADING_IN_MSW 0
#undef USE_GAUGE
# define USE_GAUGE 0
#undef USE_RESOURCE_LOADING_IN_MSW
# define USE_RESOURCE_LOADING_IN_MSW 0
#undef USE_DRAG_AND_DROP
# define USE_DRAG_AND_DROP 0
#endif

#endif
    // __SETUPH__
