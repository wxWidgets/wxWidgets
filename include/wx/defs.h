/////////////////////////////////////////////////////////////////////////////
// Name:        defs.h
// Purpose:     Declarations/definitions common to all wx source files
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEFS_H_
#define _WX_DEFS_H_

#ifdef __GNUG__
#pragma interface "defs.h"
#endif

#ifdef __WXGTK__

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"

#endif

#include <stddef.h>

#include "wx/setup.h"
#include "wx/version.h"

// Helps SGI compilation, apparently
#if defined(__SGI__) 
#if defined(__GNUG__)
#define __need_wchar_t
#else
/* Note I use the term __SGI_CC__ for both cc and CC, its not a good idea to 
 * mix gcc and cc/CC, the name mangling is different */
#define __SGI_CC__
#endif
#endif

// Eliminate double/float warnings
#ifdef _MSC_VER
# pragma warning(disable:4244)
#endif


//////////////////////////////////////////////////////////////////////////////////
// Currently Only MS-Windows/NT, XView and Motif are supported
//
#if defined(__HPUX__) && !defined(__WXMOTIF__)
# define __WXMOTIF__
#endif
#if defined(__WXMOTIF__)
# define __X__
#endif

// wxWindows checks for WIN32, not __WIN32__
#if ((defined(WIN32) || defined(__NT__)) && !defined(__WIN32__) && !defined(__WXSTUBS__))
#define __WIN32__
#endif

#ifndef __WIN32__
#define __WIN16__
#endif

#if !defined(__WIN95__) && (WINVER >= 0x0400)
#define __WIN95__
#endif

// Make sure the environment is set correctly
#if defined(__WXMSW__) && defined(__X__)
# error "Target can't be both X and Windows"
#elif !defined(__WXMOTIF__) && !defined(__WXMSW__) && !defined(__WXGTK__) && \
      !defined(__WXMAC__) && !defined(__X__) && !defined(__WXQT__) && !defined(__WXSTUBS__)
#error "No Target! Use -D[__WXMOTIF__|__WXGTK__|__WXMSW__|__WXMAC__|__WXQT__|__WXSTUBS__]"
#endif

#if defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXQT__) || defined(__WXSTUBS__)

// Bool is now obsolete, use bool instead
// typedef int Bool;

#ifndef TRUE
# define TRUE  1
# define FALSE 0
# define Bool_DEFINED
#endif

#elif defined(__WXMSW__)

#ifndef TRUE
# define TRUE  1
# define FALSE 0
#endif

#endif

// VC++ 4.0 is 1000.

// Add more tests here for compilers that don't already define bool.
#if ( defined(_MSC_VER) && (_MSC_VER <= 1000) ) || (defined(__BORLANDC__) && (__BORLANDC__ < 0x500))
typedef unsigned int bool;
#endif

#if ( defined(_MSC_VER) && (_MSC_VER <= 800) ) || defined(__GNUWIN32__)
#define byte unsigned char
#endif

typedef short int WXTYPE;
typedef int wxWindowID;

// Macro to cut down on compiler warnings.
#if REMOVE_UNUSED_ARG
#define WXUNUSED(identifier) /* identifier */
#else  // stupid, broken compiler
#define WXUNUSED(identifier) identifier
#endif

/*
 * Making or using wxWindows as a Windows DLL
 */

#ifdef __WXMSW__

#ifdef __BORLANDC__

#  ifdef WXMAKINGDLL
#    define WXDLLEXPORT __export
#    define WXDLLEXPORT_DATA(type) type __export
#    define WXDLLEXPORT_CTORFN __export
#  elif defined(WXUSINGDLL)
#    define WXDLLEXPORT __import
#    define WXDLLEXPORT_DATA(type) type __import
#    define WXDLLEXPORT_CTORFN
#  else
#    define WXDLLEXPORT
#    define WXDLLEXPORT_DATA(type) type
#    define WXDLLEXPORT_CTORFN
#  endif

#else

#  ifdef WXMAKINGDLL
#    define WXDLLEXPORT __declspec( dllexport )
#    define WXDLLEXPORT_DATA(type) __declspec( dllexport ) type
#    define WXDLLEXPORT_CTORFN __declspec( dllexport )
#  elif defined(WXUSINGDLL)
#    define WXDLLEXPORT __declspec( dllimport )
#    define WXDLLEXPORT_DATA(type) __declspec( dllimport ) type
#    define WXDLLEXPORT_CTORFN
#  else
#    define WXDLLEXPORT
#    define WXDLLEXPORT_DATA(type) type
#    define WXDLLEXPORT_CTORFN
#  endif

#endif

#else
// Non-Windows
#  define WXDLLEXPORT
#  define WXDLLEXPORT_DATA(type) type
#  define WXDLLEXPORT_CTORFN
#endif

// For ostream, istream ofstream
#if defined(__BORLANDC__) && defined( _RTLDLL )
#  define WXDLLIMPORT __import
#else
#  define WXDLLIMPORT
#endif

class WXDLLEXPORT wxObject;
class WXDLLEXPORT wxEvent;

 /** symbolic constant used by all Find()-like functions returning positive
      integer on success as failure indicator */
#define NOT_FOUND       (-1)

// ----------------------------------------------------------------------------
// Error codes
// ----------------------------------------------------------------------------

/// Standard error codes
enum  ErrCode
{
  /// invalid parameter (in broad sense)
  ERR_PARAM = (-4000),
  /// no more data (iteration functions usually return this)
  ERR_NODATA,
  /// user cancelled the operation
  ERR_CANCEL,
  /// no error (the only non negative error code)
  ERR_SUCCESS = 0
};

// ----------------------------------------------------------------------------
/** @name Very common macros */
// ----------------------------------------------------------------------------
//@{
/// delete pointer if it is not NULL and NULL it afterwards
// (checking that it's !NULL before passing it to delete is just a
//  a question of style, because delete will do it itself anyhow, but it might
//  be considered as an error by some overzealous debugging implementations of
//  the library, so we do it ourselves)
#if defined(__SGI_CC__)
// Okay this is bad styling, but the native SGI compiler is very picky, it 
// wont let you compare/assign between a NULL (void *) and another pointer
// type. To be really clean we'd need to pass in another argument, the type 
// of p. 
// Also note the use of 0L, this would allow future possible 64bit support 
// (as yet untested) by ensuring that we zero all the bits in a pointer 
// (which is always the same length as a long (at least with the LP64 standard) 
// --- offer aug 98
#define wxDELETE(p)      if ( (p) ) { delete (p); p = 0L; }
#else
#define wxDELETE(p)      if ( (p) != NULL ) { delete p; p = NULL; }
#endif /* __SGI__CC__ */

// delete an array and NULL it (see comments above)
#if defined(__SGI_CC__)
// see above comment.
#define wxDELETEA(p)      if ( (p) ) { delete [] (p); p = 0L; }
#else
#define wxDELETEA(p)      if ( ((void *) (p)) != NULL ) { delete [] p; p = NULL; }
#endif /* __SGI__CC__ */

/// size of statically declared array
#define WXSIZEOF(array)   (sizeof(array)/sizeof(array[0]))

// ----------------------------------------------------------------------------
// compiler and OS identification
// ----------------------------------------------------------------------------

// OS
#if     defined(__HPUX__) || defined(____SVR4____) || defined(__LINUX__) || defined(__sgi )
  #ifndef __UNIX__
    #define __UNIX__
  #endif
#endif

#ifndef __UNIX__                     // Windows
  #if   defined(_MSC_VER)
    #define __VISUALC__
  #elif defined(__BCPLUSPLUS__) && !defined(__BORLANDC__)
    #define __BORLANDC__
  #elif defined(__WATCOMC__)
    //#define __WATCOMC__
  #elif defined(__SC__)
    #define __SYMANTECC__
  #endif  // compiler

#endif  // OS

// ----------------------------------------------------------------------------
// compiler specific settings
// ----------------------------------------------------------------------------

// to allow compiling with warning level 4 under Microsoft Visual C++ some
// warnings just must be disabled
#ifdef  __VISUALC__
  #pragma warning(disable: 4514) // unreferenced inline func has been removed
/* 
  you might be tempted to disable this one also: triggered by CHECK and FAIL
  macros in debug.h, but it's, overall, is a rather useful one, so I leave it
  and will try to find some way to disable this warning just for CHECK/FAIL.
  Anyone?
*/
  #pragma warning(disable: 4127) // conditional expression is constant

#endif  // VC++

// Callback function type definition
typedef void (*wxFunction) (wxObject&, wxEvent&);

/*
 * Window style flags.
 * Values are chosen so they can be |'ed in a bit list.
 * Some styles are used across more than one group,
 * so the values mustn't clash with others in the group.
 * Otherwise, numbers can be reused across groups.
 *
 * From version 1.66:
 * Window (cross-group) styles now take up the first half
 * of the flag, and control-specific styles the
 * second half.
 * 
 */

/*
 * Window (Frame/dialog/subwindow/panel item) style flags
 */
#define wxVSCROLL           0x80000000
#define wxHSCROLL           0x40000000
#define wxCAPTION           0x20000000

// New styles
#define wxDOUBLE_BORDER     0x10000000
#define wxSUNKEN_BORDER     0x08000000
#define wxRAISED_BORDER     0x04000000
#define wxBORDER            0x02000000
#define wxSIMPLE_BORDER     0x02000000
#define wxSTATIC_BORDER     0x01000000
#define wxTRANSPARENT_WINDOW 0x00100000
#define wxNO_BORDER         0x00200000

#define wxUSER_COLOURS      0x00800000
                                 // Override CTL3D etc. control colour processing to
                                 // allow own background colour
                                 // OBSOLETE - use wxNO_3D instead
#define wxNO_3D             0x00800000
                                 // Override CTL3D or native 3D styles for children
#define wxCLIP_CHILDREN     0x00400000
                                 // Clip children when painting, which reduces flicker in
                                 // e.g. frames and splitter windows, but can't be used in
                                 // a panel where a static box must be 'transparent' (panel
                                 // paints the background for it)

// Add this style to a panel to get tab traversal working
// outside of dialogs.
#define wxTAB_TRAVERSAL     0x00080000

// Orientations
#define wxHORIZONTAL     0x01
#define wxVERTICAL       0x02
#define wxBOTH           (wxVERTICAL|wxHORIZONTAL)
#define wxCENTER_FRAME   0x04  /* centering into frame rather than screen */

/*
 * Frame/dialog style flags
 */
#define wxSTAY_ON_TOP       0x8000
#define wxICONIZE           0x4000
#define wxMINIMIZE          wxICONIZE
#define wxMAXIMIZE          0x2000
#define wxTHICK_FRAME       0x1000
#define wxSYSTEM_MENU       0x0800
#define wxMINIMIZE_BOX      0x0400
#define wxMAXIMIZE_BOX      0x0200
#define wxTINY_CAPTION_HORIZ 0x0100
#define wxTINY_CAPTION_VERT 0x0080
#define wxRESIZE_BOX        wxMAXIMIZE_BOX
#define wxRESIZE_BORDER	    0x0040
#define wxDIALOG_MODAL      0x0020
#define wxDIALOG_MODELESS   0x0000

#define wxDEFAULT_FRAME_STYLE    (wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN)

#if WXWIN_COMPATIBILITY
#define wxDEFAULT_FRAME wxDEFAULT_FRAME_STYLE
#endif

#define wxDEFAULT_DIALOG_STYLE	(wxSYSTEM_MENU|wxCAPTION|wxTHICK_FRAME)

/*
 * Subwindow style flags
 */
#define wxRETAINED          0x0001
#define wxBACKINGSTORE      wxRETAINED
// wxCanvas or wxPanel can optionally have a thick frame under MS Windows.
// #define wxTHICK_FRAME       0x1000

/*
 * wxToolBar style flags
 */
 
#define wxTB_3DBUTTONS      0x8000
#define wxTB_HORIZONTAL     0x0002
#define wxTB_VERTICAL       0x0004
// Flatbar/Coolbar under Win98
#define wxTB_FLAT           0x0008

/*
 * Apply to all panel items
 */
 
#define wxCOLOURED          0x0800
// Alignment for panel item labels: replaces characters with zeros
// when creating label, so spaces can be included in string for alignment.
#define wxFIXED_LENGTH      0x0400
#define wxALIGN_LEFT        0x0000
#define wxALIGN_CENTER      0x0100
#define wxALIGN_CENTRE      0x0100
#define wxALIGN_RIGHT       0x0200

/*
 * Styles for wxListBox
 */

#define wxLB_SORT           0x0010
#define wxLB_SINGLE         0x0020
#define wxLB_MULTIPLE       0x0040
#define wxLB_EXTENDED       0x0080
// wxLB_OWNERDRAW is Windows-only
#define wxLB_OWNERDRAW      0x0100
#define wxLB_NEEDED_SB      0x0200
#define wxLB_ALWAYS_SB      0x0400
#define wxLB_HSCROLL        wxHSCROLL

/*
 * wxTextCtrl style flags
 */
#define wxPROCESS_ENTER     0x0004
#define wxPASSWORD          0x0008
#define wxTE_PROCESS_ENTER  wxPROCESS_ENTER
#define wxTE_PASSWORD       wxPASSWORD
#define wxTE_READONLY       0x0010
#define wxTE_MULTILINE      0x0020

// MSW-only
#define wxTE_RICHTEXT       0x0020

/*
 * wxComboBox style flags
 */
#define wxCB_SIMPLE         0x0004
#define wxCB_SORT           0x0008
#define wxCB_READONLY       0x0010
#define wxCB_DROPDOWN       0x0020

/*
 * wxRadioBox/wxRadioButton style flags
 */
#define wxRA_HORIZONTAL     wxHORIZONTAL
#define wxRA_VERTICAL       wxVERTICAL
#define wxRB_GROUP          0x0004

/*
 * wxGauge flags
 */
#define wxGA_PROGRESSBAR     0x0004
#define wxGA_HORIZONTAL      wxHORIZONTAL
#define wxGA_VERTICAL        wxVERTICAL

/*
 * wxSlider flags
 */
 
#define wxSL_HORIZONTAL      wxHORIZONTAL
#define wxSL_VERTICAL        wxVERTICAL
// The next one is obsolete - use scroll events instead
#define wxSL_NOTIFY_DRAG     0x0000
#define wxSL_AUTOTICKS       0x0008
// #define wxSL_MANUALTICKS     0x0010
#define wxSL_LABELS          0x0020
#define wxSL_LEFT            0x0040
#define wxSL_TOP             0x0080
#define wxSL_RIGHT           0x0100
#define wxSL_BOTTOM          0x0200
#define wxSL_BOTH            0x0400
#define wxSL_SELRANGE        0x0800

/*
 * wxScrollBar flags
 */
 
#define wxSB_HORIZONTAL      wxHORIZONTAL
#define wxSB_VERTICAL        wxVERTICAL

/*
 * wxButton flags
 */

#define wxBU_AUTODRAW        0x0004
#define wxBU_NOAUTODRAW      0x0000

/*
 * wxTreeCtrl flags
 */

#define wxTR_HAS_BUTTONS     0x0004
#define wxTR_EDIT_LABELS     0x0008
#define wxTR_LINES_AT_ROOT   0x0010

/*
 * wxListCtrl flags
 */

#define wxLC_ICON            0x0004
#define wxLC_SMALL_ICON      0x0008
#define wxLC_LIST            0x0010
#define wxLC_REPORT          0x0020
#define wxLC_ALIGN_TOP       0x0040
#define wxLC_ALIGN_LEFT      0x0080
#define wxLC_AUTOARRANGE     0x0100
#define wxLC_USER_TEXT       0x0200
#define wxLC_EDIT_LABELS     0x0400
#define wxLC_NO_HEADER       0x0800
#define wxLC_NO_SORT_HEADER  0x1000
#define wxLC_SINGLE_SEL      0x2000
#define wxLC_SORT_ASCENDING  0x4000
#define wxLC_SORT_DESCENDING 0x8000

#define wxLC_MASK_TYPE       (wxLC_ICON | wxLC_SMALL_ICON | wxLC_LIST | wxLC_REPORT)
#define wxLC_MASK_ALIGN      (wxLC_ALIGN_TOP | wxLC_ALIGN_LEFT)
#define wxLC_MASK_SORT       (wxLC_SORT_ASCENDING | wxLC_SORT_DESCENDING)

// Omitted because (a) too much detail (b) not enough style flags
// #define wxLC_NO_SCROLL
// #define wxLC_NO_LABEL_WRAP
// #define wxLC_OWNERDRAW_FIXED
// #define wxLC_SHOW_SEL_ALWAYS

/*
 * wxSpinButton flags
 */

#define wxSP_VERTICAL       0x0004
#define wxSP_HORIZONTAL     0x0008
#define wxSP_ARROW_KEYS     0x0010
#define wxSP_WRAP           0x0020

/*
 * wxSplitterWnd flags
 */

#define wxSP_NOBORDER       0x0000
#define wxSP_3D             0x0004
#define wxSP_BORDER         0x0008

/*
 * wxTabCtrl flags
 */

#define wxTAB_MULTILINE     0x0000
#define wxTAB_RIGHTJUSTIFY  0x0004
#define wxTAB_FIXEDWIDTH    0x0008
#define wxTAB_OWNERDRAW     0x0010

// Sorry, I changed my mind about these names...
#define wxTC_MULTILINE     0x0000
#define wxTC_RIGHTJUSTIFY  0x0004
#define wxTC_FIXEDWIDTH    0x0008
#define wxTC_OWNERDRAW     0x0010

/*
 * wxStatusBar95 flags
 */

#define wxST_SIZEGRIP       0x0002

/*
 * GDI descriptions
 */

enum {
// Text font families
  wxDEFAULT    = 70,
  wxDECORATIVE,
  wxROMAN,
  wxSCRIPT,
  wxSWISS,
  wxMODERN,
  wxTELETYPE,  /* @@@@ */

// Proportional or Fixed width fonts (not yet used)
  wxVARIABLE   = 80,
  wxFIXED,

  wxNORMAL     = 90,
  wxLIGHT,
  wxBOLD,
// Also wxNORMAL for normal (non-italic text)
  wxITALIC,
  wxSLANT,

// Pen styles
  wxSOLID      =   100,
  wxDOT,
  wxLONG_DASH,
  wxSHORT_DASH,
  wxDOT_DASH,
  wxUSER_DASH,

  wxTRANSPARENT,

// Brush & Pen Stippling. Note that a stippled pen cannot be dashed!!
// Note also that stippling a Pen IS meaningfull, because a Line is
// drawn with a Pen, and without any Brush -- and it can be stippled.
  wxSTIPPLE =          110,
  wxBDIAGONAL_HATCH,
  wxCROSSDIAG_HATCH,
  wxFDIAGONAL_HATCH,
  wxCROSS_HATCH,
  wxHORIZONTAL_HATCH,
  wxVERTICAL_HATCH,
#define IS_HATCH(s)	((s)>=wxBDIAGONAL_HATCH && (s)<=wxVERTICAL_HATCH)

  wxJOIN_BEVEL =     120,
  wxJOIN_MITER,
  wxJOIN_ROUND,

  wxCAP_ROUND =      130,
  wxCAP_PROJECTING,
  wxCAP_BUTT
};


// Logical ops
typedef enum {
  wxCLEAR,      // 0
  wxXOR,        // src XOR dst
  wxINVERT,     // NOT dst
  wxOR_REVERSE, // src OR (NOT dst)
  wxAND_REVERSE,// src AND (NOT dst)
  wxCOPY,       // src
  wxAND,        // src AND dst
  wxAND_INVERT, // (NOT src) AND dst
  wxNO_OP,      // dst
  wxNOR,        // (NOT src) AND (NOT dst)
  wxEQUIV,      // (NOT src) XOR dst
  wxSRC_INVERT, // (NOT src)
  wxOR_INVERT,  // (NOT src) OR dst
  wxNAND,       // (NOT src) OR (NOT dst)
  wxOR,         // src OR dst
  wxSET,        // 1
  wxSRC_OR,     // source _bitmap_ OR destination
  wxSRC_AND     // source _bitmap_ AND destination
} form_ops_t;

// Flood styles
#define  wxFLOOD_SURFACE   1
#define  wxFLOOD_BORDER    2

// Polygon filling mode
#define  wxODDEVEN_RULE    1
#define  wxWINDING_RULE    2

// ToolPanel in wxFrame
#define	wxTOOL_TOP	   1
#define	wxTOOL_BOTTOM	   2
#define	wxTOOL_LEFT	   3
#define	wxTOOL_RIGHT	   4

// Dialog specifiers/return values

#define wxOK                0x0001
#define wxYES_NO            0x0002
#define wxCANCEL            0x0004
#define wxYES               0x0008
#define wxNO                0x0010

#define wxICON_EXCLAMATION  0x0020
#define wxICON_HAND         0x0040
#define wxICON_QUESTION     0x0080
#define wxICON_INFORMATION  0x0100

#define wxICON_STOP         wxICON_HAND
#define wxICON_ASTERISK     wxICON_INFORMATION
#define wxICON_MASK         (0x0020|0x0040|0x0080|0x0100)

#define wxCENTRE            0x0200
#define wxCENTER wxCENTRE

// Possible SetSize flags

// Use internally-calculated width if -1
#define wxSIZE_AUTO_WIDTH       0x0001
// Use internally-calculated height if -1
#define wxSIZE_AUTO_HEIGHT      0x0002
// Use internally-calculated width and height if each is -1
#define wxSIZE_AUTO             (wxSIZE_AUTO_WIDTH|wxSIZE_AUTO_HEIGHT)
// Ignore missing (-1) dimensions (use existing).
// For readability only: test for wxSIZE_AUTO_WIDTH/HEIGHT in code.
#define wxSIZE_USE_EXISTING     0x0000
// Allow -1 as a valid position
#define wxSIZE_ALLOW_MINUS_ONE  0x0004
// Don't do parent client adjustments (for implementation only)
#define wxSIZE_NO_ADJUSTMENTS   0x0008


// Data format for drag & drop and clipboard operations
// numbers as per winuser.h

enum wxDataFormat
{
  wxDF_TEXT =             1,  /* CF_TEXT */
  wxDF_BITMAP =           2,  /* CF_BITMAP */
  wxDF_METAFILE =         3,  /* CF_METAFILEPICT */
  wxDF_DIB =              8,  /* CF_DIB */
  wxDF_OEMTEXT =          7,  /* CF_OEMTEXT */
  wxDF_FILENAME =         15  /* CF_HDROP */
};

// Virtual keycodes

enum _Virtual_keycodes {
 WXK_BACK    =   8,
 WXK_TAB     =   9,
 WXK_RETURN  =	13,
 WXK_ESCAPE  =	27,
 WXK_SPACE   =	32,
 WXK_DELETE  = 127,

 WXK_START   = 300,
 WXK_LBUTTON,
 WXK_RBUTTON,
 WXK_CANCEL,
 WXK_MBUTTON,
 WXK_CLEAR,
 WXK_SHIFT,
 WXK_CONTROL,
 WXK_MENU,
 WXK_PAUSE,
 WXK_CAPITAL,
 WXK_PRIOR,  // Page up
 WXK_NEXT,   // Page down
 WXK_END,
 WXK_HOME,
 WXK_LEFT,
 WXK_UP,
 WXK_RIGHT,
 WXK_DOWN,
 WXK_SELECT,
 WXK_PRINT,
 WXK_EXECUTE,
 WXK_SNAPSHOT,
 WXK_INSERT,
 WXK_HELP,
 WXK_NUMPAD0,
 WXK_NUMPAD1,
 WXK_NUMPAD2,
 WXK_NUMPAD3,
 WXK_NUMPAD4,
 WXK_NUMPAD5,
 WXK_NUMPAD6,
 WXK_NUMPAD7,
 WXK_NUMPAD8,
 WXK_NUMPAD9,
 WXK_MULTIPLY,
 WXK_ADD,
 WXK_SEPARATOR,
 WXK_SUBTRACT,
 WXK_DECIMAL,
 WXK_DIVIDE,
 WXK_F1,
 WXK_F2,
 WXK_F3,
 WXK_F4,
 WXK_F5,
 WXK_F6,
 WXK_F7,
 WXK_F8,
 WXK_F9,
 WXK_F10,
 WXK_F11,
 WXK_F12,
 WXK_F13,
 WXK_F14,
 WXK_F15,
 WXK_F16,
 WXK_F17,
 WXK_F18,
 WXK_F19,
 WXK_F20,
 WXK_F21,
 WXK_F22,
 WXK_F23,
 WXK_F24,
 WXK_NUMLOCK,
 WXK_SCROLL,
 WXK_PAGEUP,
 WXK_PAGEDOWN
};

// Colours - see wx_gdi.cc for database

// OS mnemonics -- Identify the running OS (useful for Windows)
// [Not all platforms are currently available or supported]
enum {
  wxUNKNOWN_PLATFORM,
  wxCURSES,     // Text-only CURSES
  wxXVIEW_X,	// Sun's XView OpenLOOK toolkit
  wxMOTIF_X,	// OSF Motif 1.x.x
  wxCOSE_X,	    // OSF Common Desktop Environment
  wxNEXTSTEP,	// NeXTStep
  wxMACINTOSH,	// Apple System 7
  wxGTK,	    // GTK
  wxQT,	        // Qt
  wxGEOS,	    // GEOS
  wxOS2_PM,	    // OS/2 Workplace
  wxWINDOWS,	// Windows or WfW
  wxPENWINDOWS,	// Windows for Pen Computing
  wxWINDOWS_NT,	// Windows NT
  wxWIN32S,	    // Windows 32S API
  wxWIN95,	    // Windows 95
  wxWIN386	    // Watcom 32-bit supervisor modus
};

// Printing
#ifndef wxPORTRAIT
#define wxPORTRAIT      1
#define wxLANDSCAPE     2
#endif

// Standard menu identifiers
#define wxID_OPEN               5000
#define wxID_CLOSE              5001
#define wxID_NEW                5002
#define wxID_SAVE               5003
#define wxID_SAVEAS             5004
#define wxID_REVERT             5005
#define wxID_EXIT               5006
#define wxID_UNDO               5007
#define wxID_REDO               5008
#define wxID_HELP               5009
#define wxID_PRINT              5010
#define wxID_PRINT_SETUP        5011
#define wxID_PREVIEW            5012
#define wxID_ABOUT              5013
#define wxID_HELP_CONTENTS      5014
#define wxID_HELP_COMMANDS      5015
#define wxID_HELP_PROCEDURES    5016
#define wxID_HELP_CONTEXT       5017

#define wxID_CUT                5030
#define wxID_COPY               5031
#define wxID_PASTE              5032
#define wxID_CLEAR              5033
#define wxID_FIND               5034

#define wxID_FILE1              5050
#define wxID_FILE2              5051
#define wxID_FILE3              5052
#define wxID_FILE4              5053
#define wxID_FILE5              5054
#define wxID_FILE6              5055
#define wxID_FILE7              5056
#define wxID_FILE8              5057
#define wxID_FILE9              5058

#define wxID_OK                 5100
#define wxID_CANCEL             5101
#define wxID_APPLY              5102
#define wxID_YES                5103
#define wxID_NO                 5104
#define wxID_STATIC             5105

#ifdef __WXMSW__
// Stand-ins for Windows types, to avoid
// #including all of windows.h

typedef unsigned long   WXHWND;
typedef unsigned long   WXHANDLE;
typedef unsigned long   WXHICON;
typedef unsigned long   WXHFONT;
typedef unsigned long   WXHMENU;
typedef unsigned long   WXHPEN;
typedef unsigned long   WXHBRUSH;
typedef unsigned long   WXHPALETTE;
typedef unsigned long   WXHCURSOR;
typedef unsigned long   WXHRGN;
typedef unsigned long   WXHACCEL;
typedef unsigned long   WXHINSTANCE;
typedef unsigned long   WXHBITMAP;
typedef unsigned long   WXHIMAGELIST;
typedef unsigned long   WXHGLOBAL;
typedef unsigned long   WXHDC;
typedef unsigned int    WXUINT;
typedef unsigned long   WXDWORD;
typedef unsigned short  WXWORD;
typedef unsigned int    WXWPARAM;
typedef long            WXLPARAM;
typedef unsigned long   WXCOLORREF;
typedef void *          WXRGNDATA;
typedef void *          WXMSG;
typedef unsigned long   WXHCONV;
typedef unsigned long   WXHKEY;
typedef void *          WXDRAWITEMSTRUCT;
typedef void *          WXMEASUREITEMSTRUCT;
typedef void *          WXLPCREATESTRUCT;
#ifdef __GNUWIN32__
typedef int (*WXFARPROC)();
#else
typedef int (__stdcall *WXFARPROC)();
#endif

#endif

#endif
    // _WX_DEFS_H_
