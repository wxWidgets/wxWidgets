/////////////////////////////////////////////////////////////////////////////
// Name:        defs.h
// Purpose:     Declarations/definitions common to all wx source files
// Author:      Julian Smart and others
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEFS_H_
#define _WX_DEFS_H_

#ifdef __GNUG__
#pragma interface "defs.h"
#endif

#include <stddef.h>

#include "wx/setup.h"
#include "wx/version.h"

// ----------------------------------------------------------------------------
// compiler and OS identification
// ----------------------------------------------------------------------------

// OS
#if defined(__unix) || defined(__unix__) || defined(____SVR4____) || \
    defined(__LINUX__) || defined(__sgi ) || \
    defined(__hpux) || defined(sun) || defined(__SUN__) || defined(_AIX)

    #ifndef __UNIX__
        #define __UNIX__
    #endif // Unix

    // Helps SGI compilation, apparently
    #ifdef __SGI__
        #ifdef __GNUG__
            #define __need_wchar_t
        #else // !gcc
            // Note I use the term __SGI_CC__ for both cc and CC, its not a good
            // idea to mix gcc and cc/CC, the name mangling is different
            #define __SGI_CC__
        #endif // gcc/!gcc
    #endif  // SGI

    #if defined(sun) || defined(__SUN__)
        #ifndef __GNUG__
            #ifndef __SUNCC__
                #define __SUNCC__
            #endif // Sun CC
        #endif
    #endif // Sun

    #if defined(__hpux) && !defined(__HPUX__)
        #define __HPUX__
    #endif // HP-UX

#elif defined(applec) || defined(THINK_C) || ( defined( __MWERKS__ ) && !defined(__INTEL__) )
        // MacOS
#else   // Windows
    #ifndef __WINDOWS__
        #define __WINDOWS__
    #endif  // Windows

    // define another standard symbol for Microsoft Visual C++: the standard one
    // (_MSC_VER) is also defined by Metrowerks compiler
    #if defined(_MSC_VER) && !defined(__MWERKS__)
        #define __VISUALC__ _MSC_VER
    #elif defined(__BCPLUSPLUS__) && !defined(__BORLANDC__)
        #define __BORLANDC__
      #elif defined(__WATCOMC__)
    //#define __WATCOMC__
    #elif defined(__SC__)
        #define __SYMANTECC__
    #endif  // compiler
#endif  // OS

// suppress some Visual C++ warnings
#ifdef __VISUALC__
#   pragma warning(disable:4244)    // conversion from double to float
#   pragma warning(disable:4100)    // unreferenced formal parameter
#   pragma warning(disable:4511)    // copy ctor couldn't be generated
#   pragma warning(disable:4512)    // operator=() couldn't be generated
#endif // __VISUALC__

// suppress some Salford C++ warnings
#ifdef __SALFORDC__
#   pragma suppress 353             // Possible nested comments
#   pragma suppress 593             // Define not used
#   pragma suppress 61              // enum has no name (doesn't suppress!)
#   pragma suppress 106             // unnamed, unused parameter
#   pragma suppress 571             // Virtual function hiding
#endif // __SALFORDC__

// Digital Unix C++ compiler only defines this symbol for .cxx and .hxx files,
// so define it ourselves
#ifdef __DECCXX
    #define __cplusplus
#endif // __DECCXX

// Resolves linking problems under HP-UX
#if defined(__HPUX__) && defined(__GNUG__)
    #define va_list __gnuc_va_list
#endif // HP-UX

//////////////////////////////////////////////////////////////////////////////////
// Currently Only MS-Windows/NT, XView and Motif are supported
//
#if defined(__HPUX__) && !defined(__WXGTK__)
    #ifndef __WXMOTIF__
        #define __WXMOTIF__
    #endif // __WXMOTIF__
#endif

#if defined(__WXMOTIF__)
    #define __X__
#endif

#ifdef __WXMSW__
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

#if defined(TWIN32) && !defined(__TWIN32__)
#define __TWIN32__
#endif

#endif // wxMSW

// Make sure the environment is set correctly
#if defined(__WXMSW__) && defined(__X__)
    #error "Target can't be both X and Windows"
#elif !defined(__WXMOTIF__) && !defined(__WXMSW__) && !defined(__WXGTK__) && \
      !defined(__WXMAC__) && !defined(__X__) && !defined(__WXQT__) && !defined(__WXSTUBS__)
    #error "No Target! Use -D[__WXMOTIF__|__WXGTK__|__WXMSW__|__WXMAC__|__WXQT__|__WXSTUBS__]"
#endif

// ============================================================================
// non portable C++ features
// ============================================================================

// ----------------------------------------------------------------------------
// check for native bool type and TRUE/FALSE constants
// ----------------------------------------------------------------------------

#if defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXQT__) || defined(__WXSTUBS__)
    // Bool is now obsolete, use bool instead
    // typedef int Bool;

    #ifndef TRUE
        #define TRUE  1
        #define FALSE 0
        #define Bool_DEFINED
    #endif
#elif defined(__WXMSW__)
    #ifndef TRUE
        #define TRUE  1
        #define FALSE 0
    #endif
#endif // TRUE/FALSE

// Add more tests here for Windows compilers that already define bool
// (under Unix, configure tests for this)
#ifndef HAVE_BOOL
    #if defined( __MWERKS__ )
        #if (__MWERKS__ >= 0x1000) && !__option(bool)
            #define HAVE_BOOL
        #endif
    #elif defined(__VISUALC__) && (__VISUALC__ == 1020)
        // in VC++ 4.2 the bool keyword is reserved (hence can't be typedefed)
        // but not implemented, so we must #define it
        #define bool unsigned int
    #elif defined(__VISUALC__) && (__VISUALC__ == 1010)
        // For VisualC++ 4.1, we need to define
        // bool as something between 4.0 & 5.0...
        typedef unsigned int wxbool;
        #define bool wxbool
        #define HAVE_BOOL
    #elif defined(__VISUALC__) && (__VISUALC__ > 1020)
        // VC++ supports bool since 4.2
        #define HAVE_BOOL
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x500)
        // Borland 5.0+ supports bool
        #define HAVE_BOOL
    #elif defined(__WATCOMC__) && (__WATCOMC__ >= 1100)
        // Watcom 11+ supports bool
        #define HAVE_BOOL
    #elif defined(__GNUWIN32__)
        // Cygwin supports bool
        #define HAVE_BOOL
    #endif // compilers
#endif // HAVE_BOOL

#if !defined(HAVE_BOOL) && !defined(bool)
    // NB: of course, this doesn't replace the standard type, because, for
    //     example, overloading based on bool/int parameter doesn't work and
    //     so should be avoided in portable programs
    typedef unsigned int bool;
#endif // bool

typedef unsigned char wxByte;
typedef short int WXTYPE;

// special care should be taken with this type under Windows where the real
// window id is unsigned, so we must always do the cast before comparing them
// (or else they would be always different!). Usign wxGetWindowId() which does
// the cast itself is recommended. Note that this type can't be unsigned
// because -1 is a valid (and largely used) value for window id.
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

// _declspec works in BC++ 5 and later, as well as VC++
#if defined(__VISUALC__) || defined(__BORLANDC__)

#  ifdef WXMAKINGDLL
#    define WXDLLEXPORT _declspec( dllexport )
#    define WXDLLEXPORT_DATA(type) _declspec( dllexport ) type
#    define WXDLLEXPORT_CTORFN
#  elif defined(WXUSINGDLL)
#    define WXDLLEXPORT _declspec( dllimport )
#    define WXDLLEXPORT_DATA(type) _declspec( dllimport ) type
#    define WXDLLEXPORT_CTORFN
#  else
#    define WXDLLEXPORT
#    define WXDLLEXPORT_DATA(type) type
#    define WXDLLEXPORT_CTORFN
#  endif

#else
#  define WXDLLEXPORT
#  define WXDLLEXPORT_DATA(type) type
#  define WXDLLEXPORT_CTORFN
#endif

#else // !Windows
#  define WXDLLEXPORT
#  define WXDLLEXPORT_DATA(type) type
#  define WXDLLEXPORT_CTORFN
#endif // Win/!Win

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
#define wxNOT_FOUND       (-1)

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

// Use of these suppresses some compiler warnings
WXDLLEXPORT_DATA(extern const bool) wxTrue;
WXDLLEXPORT_DATA(extern const bool) wxFalse;

// ----------------------------------------------------------------------------
// compiler specific settings
// ----------------------------------------------------------------------------

// to allow compiling with warning level 4 under Microsoft Visual C++ some
// warnings just must be disabled
#ifdef  __VISUALC__
  #pragma warning(disable: 4514) // unreferenced inline func has been removed
/*
  you might be tempted to disable this one also: triggered by CHECK and FAIL
  macros in debug.h, but it's, overall, a rather useful one, so I leave it and
  will try to find some way to disable this warning just for CHECK/FAIL. Anyone?
*/
  #pragma warning(disable: 4127) // conditional expression is constant
#endif  // VC++

#if defined(__MWERKS__)
    #undef try
    #undef except
    #undef finally
    #define except(x) catch(...)
#endif // Metrowerks

// where should i put this? we need to make sure of this as it breaks
// the <iostream> code.
#if !wxUSE_IOSTREAMH && defined(__WXDEBUG__)
#ifndef __MWERKS__
#undef __WXDEBUG__
#endif
#endif

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
#define wxSTAY_ON_TOP           0x8000
#define wxICONIZE               0x4000
#define wxMINIMIZE              wxICONIZE
#define wxMAXIMIZE              0x2000
#define wxTHICK_FRAME           0x1000
#define wxSYSTEM_MENU           0x0800
#define wxMINIMIZE_BOX          0x0400
#define wxMAXIMIZE_BOX          0x0200
#define wxTINY_CAPTION_HORIZ    0x0100
#define wxTINY_CAPTION_VERT     0x0080
#define wxRESIZE_BOX            wxMAXIMIZE_BOX
#define wxRESIZE_BORDER         0x0040
#define wxDIALOG_MODAL          0x0020
#define wxDIALOG_MODELESS       0x0000
/* Add for normal Windows frame behaviour */
#define wxFRAME_FLOAT_ON_PARENT 0x0020


#if WXWIN_COMPATIBILITY
#define wxDEFAULT_FRAME wxDEFAULT_FRAME_STYLE
#endif

#define wxDEFAULT_FRAME_STYLE    (wxSYSTEM_MENU|wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN)

#ifdef __WXMSW__
#   define wxDEFAULT_DIALOG_STYLE  (wxSYSTEM_MENU|wxCAPTION|wxTHICK_FRAME)
#else
// Under Unix, the dialogs don't have a system menu. Specifying
// wxSYSTEM_MENU here, will make a close button appear.
#   define wxDEFAULT_DIALOG_STYLE  (wxCAPTION)
#endif


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
// Flatbar/Coolbar under Win98/ GTK 1.2
#define wxTB_FLAT           0x0008
// use native docking under GTK
#define wxTB_DOCKABLE       0x0010

/*
 * wxMenuBar style flags
 */
// use native docking
#define wxMB_DOCKABLE       0x0001


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
#define wxTE_PROCESS_TAB    0x0040

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

// New, more intuitive names to specify majorDim argument

// Same as wxRA_HORIZONTAL
#define wxRA_SPECIFY_COLS   0x0001
// Same as wxRA_VERTICAL
#define wxRA_SPECIFY_ROWS   0x0002

// Old names for compatibility
#define wxRA_HORIZONTAL     wxHORIZONTAL
#define wxRA_VERTICAL       wxVERTICAL
#define wxRB_GROUP          0x0004

/*
 * wxGauge flags
 */
#define wxGA_PROGRESSBAR     0x0004
#define wxGA_HORIZONTAL      wxHORIZONTAL
#define wxGA_VERTICAL        wxVERTICAL
/* Windows only */
#define wxGA_SMOOTH          0x0008

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
 * wxButton flags (Win32 only)
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
 * wxSplitterWindow flags
 */

#define wxSP_NOBORDER       0x0000
#define wxSP_3D             0x0004
#define wxSP_BORDER         0x0008
#define wxSP_PERMIT_UNSPLIT 0x0010

/*
 * wxFrame extra flags
 */

// No title on taskbar
#define wxFRAME_TOOL_WINDOW 0x0004

/*
 * wxTabCtrl flags
 */

#define wxTC_MULTILINE     0x0000
#define wxTC_RIGHTJUSTIFY  0x0004
#define wxTC_FIXEDWIDTH    0x0008
#define wxTC_OWNERDRAW     0x0010

/*
 * wxNotebook flags
 */
#define wxNB_FIXEDWIDTH    0x0008

/*
 * wxStatusBar95 flags
 */

#define wxST_SIZEGRIP       0x0002

/*
 * wxStaticLine flags
 */
#define wxLI_HORIZONTAL      wxHORIZONTAL
#define wxLI_VERTICAL        wxVERTICAL

/*
 * wxProgressDialog flags
 */
#define wxPD_CAN_ABORT      0x0001
#define wxPD_APP_MODAL      0x0002
#define wxPD_AUTO_HIDE      0x0004

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
#define IS_HATCH(s)    ((s)>=wxBDIAGONAL_HATCH && (s)<=wxVERTICAL_HATCH)

  wxJOIN_BEVEL =     120,
  wxJOIN_MITER,
  wxJOIN_ROUND,

  wxCAP_ROUND =      130,
  wxCAP_PROJECTING,
  wxCAP_BUTT
};


// Logical ops
typedef enum
{
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
#define    wxTOOL_TOP       1
#define    wxTOOL_BOTTOM       2
#define    wxTOOL_LEFT       3
#define    wxTOOL_RIGHT       4

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

enum wxDataFormatId
{
  wxDF_INVALID =          0,
  wxDF_TEXT =             1,  /* CF_TEXT */
  wxDF_BITMAP =           2,  /* CF_BITMAP */
  wxDF_METAFILE =         3,  /* CF_METAFILEPICT */
  wxDF_SYLK =             4,
  wxDF_DIF =              5,
  wxDF_TIFF =             6,
  wxDF_OEMTEXT =          7,  /* CF_OEMTEXT */
  wxDF_DIB =              8,  /* CF_DIB */
  wxDF_PALETTE =          9,
  wxDF_PENDATA =          10,
  wxDF_RIFF =             11,
  wxDF_WAVE =             12,
  wxDF_UNICODETEXT =      13,
  wxDF_ENHMETAFILE =      14,
  wxDF_FILENAME =         15, /* CF_HDROP */
  wxDF_LOCALE =           16,
  wxDF_PRIVATE =          20,
  wxDF_MAX
};

/* Virtual keycodes */

enum wxKeyCode
{
  WXK_BACK    =    8,
  WXK_TAB     =    9,
  WXK_RETURN  =    13,
  WXK_ESCAPE  =    27,
  WXK_SPACE   =    32,
  WXK_DELETE  =    127,

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
  WXK_PRIOR,  /* Page up */
  WXK_NEXT,   /* Page down */
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
  WXK_PAGEDOWN,

  WXK_NUMPAD_SPACE,
  WXK_NUMPAD_TAB,
  WXK_NUMPAD_ENTER,
  WXK_NUMPAD_F1,
  WXK_NUMPAD_F2,
  WXK_NUMPAD_F3,
  WXK_NUMPAD_F4,
  WXK_NUMPAD_HOME,
  WXK_NUMPAD_LEFT,
  WXK_NUMPAD_UP,
  WXK_NUMPAD_RIGHT,
  WXK_NUMPAD_DOWN,
  WXK_NUMPAD_PRIOR,
  WXK_NUMPAD_PAGEUP,
  WXK_NUMPAD_NEXT,
  WXK_NUMPAD_PAGEDOWN,
  WXK_NUMPAD_END,
  WXK_NUMPAD_BEGIN,
  WXK_NUMPAD_INSERT,
  WXK_NUMPAD_DELETE,
  WXK_NUMPAD_EQUAL,
  WXK_NUMPAD_MULTIPLY,
  WXK_NUMPAD_ADD,
  WXK_NUMPAD_SEPARATOR,
  WXK_NUMPAD_SUBTRACT,
  WXK_NUMPAD_DECIMAL,
  WXK_NUMPAD_DIVIDE
};

/* OS mnemonics -- Identify the running OS (useful for Windows)
 * [Not all platforms are currently available or supported] */
enum
{
  wxUNKNOWN_PLATFORM,
  wxCURSES,                 // Text-only CURSES
  wxXVIEW_X,                // Sun's XView OpenLOOK toolkit
  wxMOTIF_X,                // OSF Motif 1.x.x
  wxCOSE_X,                 // OSF Common Desktop Environment
  wxNEXTSTEP,               // NeXTStep
  wxMACINTOSH,              // Apple System 7
  wxGTK,                    // GTK
  wxQT,                     // Qt
  wxGEOS,                   // GEOS
  wxOS2_PM,                 // OS/2 Workplace
  wxWINDOWS,                // Windows or WfW
  wxPENWINDOWS,             // Windows for Pen Computing
  wxWINDOWS_NT,             // Windows NT
  wxWIN32S,                 // Windows 32S API
  wxWIN95,                  // Windows 95
  wxWIN386                  // Watcom 32-bit supervisor modus
};

/* Standard menu identifiers */
#define wxID_LOWEST             4999

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
#define wxID_DUPLICATE          5035
#define wxID_SELECTALL          5036

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

#define wxID_HIGHEST            5999

// Mapping modes (as per Windows)
#define wxMM_TEXT             1
#define wxMM_LOMETRIC         2
#define wxMM_HIMETRIC         3
#define wxMM_LOENGLISH        4
#define wxMM_HIENGLISH        5
#define wxMM_TWIPS            6
#define wxMM_ISOTROPIC        7
#define wxMM_ANISOTROPIC      8

#define wxMM_POINTS           9
#define wxMM_METRIC          10

/* Shortcut for easier dialog-unit-to-pixel conversion */
#define wxDLG_UNIT(parent, pt) parent->ConvertDialogToPixels(pt)

/* Paper types */
typedef enum {
    wxPAPER_NONE,               // Use specific dimensions
    wxPAPER_LETTER,             // Letter, 8 1/2 by 11 inches
    wxPAPER_LEGAL,              // Legal, 8 1/2 by 14 inches
    wxPAPER_A4,                 // A4 Sheet, 210 by 297 millimeters
    wxPAPER_CSHEET,             // C Sheet, 17 by 22 inches
    wxPAPER_DSHEET,             // D Sheet, 22 by 34 inches
    wxPAPER_ESHEET,             // E Sheet, 34 by 44 inches
    wxPAPER_LETTERSMALL,        // Letter Small, 8 1/2 by 11 inches
    wxPAPER_TABLOID,            // Tabloid, 11 by 17 inches
    wxPAPER_LEDGER,             // Ledger, 17 by 11 inches
    wxPAPER_STATEMENT,          // Statement, 5 1/2 by 8 1/2 inches
    wxPAPER_EXECUTIVE,          // Executive, 7 1/4 by 10 1/2 inches
    wxPAPER_A3,                 // A3 sheet, 297 by 420 millimeters
    wxPAPER_A4SMALL,            // A4 small sheet, 210 by 297 millimeters
    wxPAPER_A5,                 // A5 sheet, 148 by 210 millimeters
    wxPAPER_B4,                 // B4 sheet, 250 by 354 millimeters
    wxPAPER_B5,                 // B5 sheet, 182-by-257-millimeter paper
    wxPAPER_FOLIO,              // Folio, 8-1/2-by-13-inch paper
    wxPAPER_QUARTO,             // Quarto, 215-by-275-millimeter paper
    wxPAPER_10X14,              // 10-by-14-inch sheet
    wxPAPER_11X17,              // 11-by-17-inch sheet
    wxPAPER_NOTE,               // Note, 8 1/2 by 11 inches
    wxPAPER_ENV_9,              // #9 Envelope, 3 7/8 by 8 7/8 inches
    wxPAPER_ENV_10,             // #10 Envelope, 4 1/8 by 9 1/2 inches
    wxPAPER_ENV_11,             // #11 Envelope, 4 1/2 by 10 3/8 inches
    wxPAPER_ENV_12,             // #12 Envelope, 4 3/4 by 11 inches
    wxPAPER_ENV_14,             // #14 Envelope, 5 by 11 1/2 inches
    wxPAPER_ENV_DL,             // DL Envelope, 110 by 220 millimeters
    wxPAPER_ENV_C5,             // C5 Envelope, 162 by 229 millimeters
    wxPAPER_ENV_C3,             // C3 Envelope, 324 by 458 millimeters
    wxPAPER_ENV_C4,             // C4 Envelope, 229 by 324 millimeters
    wxPAPER_ENV_C6,             // C6 Envelope, 114 by 162 millimeters
    wxPAPER_ENV_C65,            // C65 Envelope, 114 by 229 millimeters
    wxPAPER_ENV_B4,             // B4 Envelope, 250 by 353 millimeters
    wxPAPER_ENV_B5,             // B5 Envelope, 176 by 250 millimeters
    wxPAPER_ENV_B6,             // B6 Envelope, 176 by 125 millimeters
    wxPAPER_ENV_ITALY,          // Italy Envelope, 110 by 230 millimeters
    wxPAPER_ENV_MONARCH,        // Monarch Envelope, 3 7/8 by 7 1/2 inches
    wxPAPER_ENV_PERSONAL,       // 6 3/4 Envelope, 3 5/8 by 6 1/2 inches
    wxPAPER_FANFOLD_US,         // US Std Fanfold, 14 7/8 by 11 inches
    wxPAPER_FANFOLD_STD_GERMAN, // German Std Fanfold, 8 1/2 by 12 inches
    wxPAPER_FANFOLD_LGL_GERMAN, // German Legal Fanfold, 8 1/2 by 13 inches

    wxPAPER_ISO_B4,             // B4 (ISO) 250 x 353 mm
    wxPAPER_JAPANESE_POSTCARD,  // Japanese Postcard 100 x 148 mm
    wxPAPER_9X11,               // 9 x 11 in
    wxPAPER_10X11,              // 10 x 11 in
    wxPAPER_15X11,              // 15 x 11 in
    wxPAPER_ENV_INVITE,         // Envelope Invite 220 x 220 mm
    wxPAPER_LETTER_EXTRA,       // Letter Extra 9 \275 x 12 in
    wxPAPER_LEGAL_EXTRA,        // Legal Extra 9 \275 x 15 in
    wxPAPER_TABLOID_EXTRA,      // Tabloid Extra 11.69 x 18 in
    wxPAPER_A4_EXTRA,           // A4 Extra 9.27 x 12.69 in
    wxPAPER_LETTER_TRANSVERSE,  // Letter Transverse 8 \275 x 11 in
    wxPAPER_A4_TRANSVERSE,      // A4 Transverse 210 x 297 mm
    wxPAPER_LETTER_EXTRA_TRANSVERSE, // Letter Extra Transverse 9\275 x 12 in
    wxPAPER_A_PLUS,             // SuperA/SuperA/A4 227 x 356 mm
    wxPAPER_B_PLUS,             // SuperB/SuperB/A3 305 x 487 mm
    wxPAPER_LETTER_PLUS,        // Letter Plus 8.5 x 12.69 in
    wxPAPER_A4_PLUS,            // A4 Plus 210 x 330 mm
    wxPAPER_A5_TRANSVERSE,      // A5 Transverse 148 x 210 mm
    wxPAPER_B5_TRANSVERSE,      // B5 (JIS) Transverse 182 x 257 mm
    wxPAPER_A3_EXTRA,           // A3 Extra 322 x 445 mm
    wxPAPER_A5_EXTRA,           // A5 Extra 174 x 235 mm
    wxPAPER_B5_EXTRA,           // B5 (ISO) Extra 201 x 276 mm
    wxPAPER_A2,                 // A2 420 x 594 mm
    wxPAPER_A3_TRANSVERSE,      // A3 Transverse 297 x 420 mm
    wxPAPER_A3_EXTRA_TRANSVERSE // A3 Extra Transverse 322 x 445 mm

} wxPaperSize ;

/* Printing orientation */
#ifndef wxPORTRAIT
#define wxPORTRAIT      1
#define wxLANDSCAPE     2
#endif

/* Duplex printing modes
 */

typedef enum {
    wxDUPLEX_SIMPLEX, // Non-duplex
    wxDUPLEX_HORIZONTAL,
    wxDUPLEX_VERTICAL
} wxDuplexMode;

/* Print quality.
 */

#define wxPRINT_QUALITY_HIGH    -1
#define wxPRINT_QUALITY_MEDIUM  -2
#define wxPRINT_QUALITY_LOW     -3
#define wxPRINT_QUALITY_DRAFT   -4

typedef int wxPrintQuality;

/* Print mode (currently PostScript only)
 */

typedef enum {
    wxPRINT_MODE_NONE =    0,
    wxPRINT_MODE_PREVIEW = 1,   // Preview in external application
    wxPRINT_MODE_FILE =    2,   // Print to file
    wxPRINT_MODE_PRINTER = 3    // Send to printer
} wxPrintMode;


#ifdef __WXMSW__
// Stand-ins for Windows types, to avoid #including all of windows.h
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
typedef unsigned long   WXHTREEITEM;

typedef void *          WXDRAWITEMSTRUCT;
typedef void *          WXMEASUREITEMSTRUCT;
typedef void *          WXLPCREATESTRUCT;

#ifdef __GNUWIN32__
    typedef int (*WXFARPROC)();
#elif defined(__WIN32__)
    typedef int (__stdcall *WXFARPROC)();
#else
    typedef int (*WXFARPROC)();
#endif

typedef WXHWND WXWidget;
#endif // MSW

#ifdef __WXMOTIF__
/* Stand-ins for X/Xt/Motif types */
typedef void*           WXWindow;
typedef void*           WXWidget;
typedef void*           WXAppContext;
typedef void*           WXColormap;
typedef void            WXDisplay;
typedef void            WXEvent;
typedef void*           WXCursor;
typedef void*           WXPixmap;
typedef void*           WXFontStructPtr;
typedef void*           WXGC;
typedef void*           WXRegion;
typedef void*           WXFont;
typedef void*           WXImage;
typedef void*           WXCursor;
typedef void*           WXFontList;
#endif // Motif

#ifdef __WXGTK__
/* Stand-ins for GLIB types */
typedef int            gint;
typedef unsigned       guint;
typedef unsigned long  gulong;
typedef void*          gpointer;

/* Stand-ins for GDK types */
typedef gulong                  GdkAtom;
typedef struct _GdkColor        GdkColor;
typedef struct _GdkColormap     GdkColormap;
typedef struct _GdkFont         GdkFont;
typedef struct _GdkGC           GdkGC;
typedef struct _GdkWindow       GdkWindow;
typedef struct _GdkWindow       GdkBitmap;
typedef struct _GdkWindow       GdkPixmap;
typedef struct _GdkCursor       GdkCursor;
typedef struct _GdkRegion       GdkRegion;
typedef struct _GdkDragContext  GdkDragContext;

/* Stand-ins for GTK types */
typedef struct _GtkWidget         GtkWidget;
typedef struct _GtkStyle          GtkStyle;
typedef struct _GtkAdjustment     GtkAdjustment;
typedef struct _GtkList           GtkList;
typedef struct _GtkToolbar        GtkToolbar;
typedef struct _GtkTooltips       GtkTooltips;
typedef struct _GtkNotebook       GtkNotebook;
typedef struct _GtkNotebookPage   GtkNotebookPage;
typedef struct _GtkAccelGroup     GtkAccelGroup;
typedef	struct _GtkItemFactory    GtkItemFactory;
typedef	struct _GtkSelectionData  GtkSelectionData;

typedef GtkWidget *WXWidget;
#endif // GTK

// This is required because of clashing macros in windows.h, which may be
// included before or after wxWindows classes, and therefore must be
// disabled here before any significant wxWindows headers are included.
#ifdef __WXMSW__
#ifdef GetClassInfo
#undef GetClassInfo
#endif

#ifdef GetClassName
#undef GetClassName
#endif

#ifdef DrawText
#undef DrawText
#endif

#ifdef GetCharWidth
#undef GetCharWidth
#endif

#ifdef StartDoc
#undef StartDoc
#endif

#ifdef FindWindow
#undef FindWindow
#endif

#ifdef FindResource
#undef FindResource
#endif
#endif
  // __WXMSW__

// ---------------------------------------------------------------------------
// macro to define a class without copy ctor nor assignment operator
// ---------------------------------------------------------------------------

#define DECLARE_NO_COPY_CLASS(classname)        \
    private:                                    \
        classname(const classname&);            \
        classname& operator=(const classname&)

#endif
    // _WX_DEFS_H_
