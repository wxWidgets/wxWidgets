/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWindows itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include <windows.h>

// undefine conflicting symbols which were defined in windows.h
#include "wx/msw/winundef.h"

class WXDLLEXPORT wxFont;

// ---------------------------------------------------------------------------
// private constants
// ---------------------------------------------------------------------------

// Conversion
static const double METRIC_CONVERSION_CONSTANT = 0.0393700787;

// Scaling factors for various unit conversions
static const double mm2inches = (METRIC_CONVERSION_CONSTANT);
static const double inches2mm = (1/METRIC_CONVERSION_CONSTANT);

static const double mm2twips = (METRIC_CONVERSION_CONSTANT*1440);
static const double twips2mm = (1/(METRIC_CONVERSION_CONSTANT*1440));

static const double mm2pt = (METRIC_CONVERSION_CONSTANT*72);
static const double pt2mm = (1/(METRIC_CONVERSION_CONSTANT*72));

// ---------------------------------------------------------------------------
// standard icons from the resources
// ---------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern HICON) wxSTD_FRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxSTD_MDIPARENTFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxSTD_MDICHILDFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_FRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_MDIPARENTFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_MDICHILDFRAME_ICON;
WXDLLEXPORT_DATA(extern HFONT) wxSTATUS_LINE_FONT;

// ---------------------------------------------------------------------------
// this defines a CASTWNDPROC macro which casts a pointer to the type of a
// window proc
// ---------------------------------------------------------------------------
#ifdef __GNUWIN32__
#  define CASTWNDPROC (long unsigned)
#else
#  ifdef __BORLANDC__

#  ifdef __WIN32__
#    define CASTWNDPROC
#  else
       typedef int (pascal * WndProcCast) ();
#      define CASTWNDPROC (WndProcCast)
#  endif

#  else
#    if defined (__WIN32__) && defined(STRICT)
       typedef long (_stdcall * WndProcCast) (HWND, unsigned int, unsigned int, long);
#      define CASTWNDPROC (WndProcCast)
#    elif defined(__WIN16__)
#    ifdef __BORLANDC__
       typedef int (pascal * WndProcCast) ();
#      define CASTWNDPROC (WndProcCast)
#    else
       typedef int (PASCAL * WndProcCast) ();
#      define CASTWNDPROC (WndProcCast)
#    endif
#    else
#      define CASTWNDPROC
#    endif
#  endif
#endif

// ---------------------------------------------------------------------------
// some stuff for old Windows versions (FIXME: what does it do here??)
// ---------------------------------------------------------------------------

#if !defined(APIENTRY)  // NT defines APIENTRY, 3.x not
    #define APIENTRY FAR PASCAL
#endif

#ifdef __WIN32__
    #define _EXPORT
#else
    #define _EXPORT _export
#endif

#ifndef __WIN32__
    typedef signed short int SHORT;
#endif

#if !defined(__WIN32__)  // 3.x uses FARPROC for dialogs
    #define DLGPROC FARPROC
#endif

#if wxUSE_PENWIN
    WXDLLEXPORT void wxRegisterPenWin();
    WXDLLEXPORT void wxCleanUpPenWin();
    WXDLLEXPORT void wxEnablePenAppHooks (bool hook);
#endif // wxUSE_PENWIN

#if wxUSE_ITSY_BITSY
    #define IBS_HORZCAPTION    0x4000L
    #define IBS_VERTCAPTION    0x8000L

    UINT    WINAPI ibGetCaptionSize( HWND hWnd  ) ;
    UINT    WINAPI ibSetCaptionSize( HWND hWnd, UINT nSize ) ;
    LRESULT WINAPI ibDefWindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
    VOID    WINAPI ibAdjustWindowRect( HWND hWnd, LPRECT lprc ) ;
#endif // wxUSE_ITSY_BITSY

#if wxUSE_CTL3D
    #include "wx/msw/ctl3d/ctl3d.h"
#endif // wxUSE_CTL3D

/*
 * Decide what window classes we're going to use
 * for this combination of CTl3D/FAFA settings
 */

#define STATIC_CLASS     "STATIC"
#define STATIC_FLAGS     (SS_LEFT|WS_CHILD|WS_VISIBLE)
#define CHECK_CLASS      "BUTTON"
#define CHECK_FLAGS      (BS_AUTOCHECKBOX|WS_TABSTOP|WS_CHILD)
#define CHECK_IS_FAFA   FALSE
#define RADIO_CLASS      "BUTTON"
#define RADIO_FLAGS      (BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE)
#define RADIO_SIZE       20
#define RADIO_IS_FAFA   FALSE
#define PURE_WINDOWS
#define GROUP_CLASS      "BUTTON"
#define GROUP_FLAGS      (BS_GROUPBOX|WS_CHILD|WS_VISIBLE)

/*
#define BITCHECK_FLAGS   (FB_BITMAP|FC_BUTTONDRAW|FC_DEFAULT|WS_VISIBLE)
#define BITRADIO_FLAGS   (FC_BUTTONDRAW|FB_BITMAP|FC_RADIO|WS_CHILD|WS_VISIBLE)
*/

// ---------------------------------------------------------------------------
// misc macros
// ---------------------------------------------------------------------------

#define MEANING_CHARACTER '0'
#define DEFAULT_ITEM_WIDTH  200
#define DEFAULT_ITEM_HEIGHT 80

// Scale font to get edit control height
#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (3*(cy)/2)

// Generic subclass proc, for panel item moving/sizing and intercept
// EDIT control VK_RETURN messages
extern LONG APIENTRY _EXPORT
  wxSubclassedGenericControlProc(WXHWND hWnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

// ---------------------------------------------------------------------------
// constants which might miss from some compilers' headers
// ---------------------------------------------------------------------------

#if !defined(__WIN32__) && !defined(WS_EX_CLIENTEDGE)
    #define WS_EX_CLIENTEDGE 0
#endif

#if defined(__WIN32__) && !defined(WS_EX_CLIENTEDGE)
    #define WS_EX_CLIENTEDGE 0x00000200L
#endif

#ifndef ENDSESSION_LOGOFF
    #define ENDSESSION_LOGOFF    0x80000000
#endif

// ---------------------------------------------------------------------------
// debug messages
// ---------------------------------------------------------------------------
#if defined(__WIN95__) && defined(__WXDEBUG__) && wxUSE_DBWIN32

    #ifndef __TWIN32__
        #ifdef OutputDebugString
            #undef OutputDebugString
        #endif

        #define OutputDebugString OutputDebugStringW95
    #endif // __TWIN32__

    extern void OutputDebugStringW95(const wxChar*, ...);
#endif // USE_DBWIN32

// ---------------------------------------------------------------------------
// macros to make casting between WXFOO and FOO a bit easier: the GetFoo()
// returns Foo cast to the Windows type for oruselves, while GetFoosFoo() takes
// an argument which should be a pointer to wxFoo (is this really clear?)
// ---------------------------------------------------------------------------

#define GetHwnd()               ((HWND)GetHWND())
#define GetWinHwnd(win)         ((HWND)((win)->GetHWND()))

#define GetHdc()                ((HDC)GetHDC())

#define GetHicon()              ((HICON)GetHICON())
#define GetIconHicon(icon)      ((HICON)(icon).GetHICON())

#define GetHaccel()             ((HACCEL)GetHACCEL())
#define GetTableHaccel(table)   ((HACCEL)((table).GetHACCEL()))

// ---------------------------------------------------------------------------
// global data
// ---------------------------------------------------------------------------

// The MakeProcInstance version of the function wxSubclassedGenericControlProc
WXDLLEXPORT_DATA(extern FARPROC) wxGenericControlSubClassProc;
WXDLLEXPORT_DATA(extern wxChar*) wxBuffer;
WXDLLEXPORT_DATA(extern HINSTANCE) wxhInstance;

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

WXDLLEXPORT HINSTANCE wxGetInstance();
WXDLLEXPORT void wxSetInstance(HINSTANCE hInst);

WXDLLEXPORT wxWindow* wxFindWinFromHandle(WXHWND hWnd);

WXDLLEXPORT void wxGetCharSize(WXHWND wnd, int *x, int *y,wxFont *the_font);
WXDLLEXPORT void wxFillLogFont(LOGFONT *logFont, wxFont *font);
WXDLLEXPORT wxFont wxCreateFontFromLogFont(LOGFONT *logFont);

WXDLLEXPORT void wxSliderEvent(WXHWND control, WXWORD wParam, WXWORD pos);
WXDLLEXPORT void wxScrollBarEvent(WXHWND hbar, WXWORD wParam, WXWORD pos);

// Find maximum size of window/rectangle
WXDLLEXPORT extern void wxFindMaxSize(WXHWND hwnd, RECT *rect);

WXDLLEXPORT wxWindow* wxFindControlFromHandle(WXHWND hWnd);
WXDLLEXPORT void wxAddControlHandle(WXHWND hWnd, wxWindow *item);

// Safely get the window text (i.e. without using fixed size buffer)
WXDLLEXPORT extern wxString wxGetWindowText(WXHWND hWnd);

// get the window class name
WXDLLEXPORT extern wxString wxGetWindowClass(WXHWND hWnd);

// get the window id (should be unsigned, hence this is not wxWindowID which
// is, for mainly historical reasons, signed)
WXDLLEXPORT extern WXWORD wxGetWindowId(WXHWND hWnd);

// Does this window style specify any border?
inline bool wxStyleHasBorder(long style)
{
  return (style & (wxSIMPLE_BORDER | wxRAISED_BORDER |
                   wxSUNKEN_BORDER | wxDOUBLE_BORDER)) != 0;
}

#endif
    // _WX_PRIVATE_H_
