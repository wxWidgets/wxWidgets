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

#define INCL_BASE
#define INCL_PM
#define INCL_GPI
#define INCL_WINSYS
#define INCL_SHLERRORS
#include <os2.h>

class WXDLLEXPORT wxFont;
class WXDLLEXPORT wxWindow;

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
// window proc for PM.
// MPARAM is a void * but is really a 32-bit value
// ---------------------------------------------------------------------------

typedef MRESULT (APIENTRY * WndProcCast) (HWND, ULONG, MPARAM, MPARAM);
#define CASTWNDPROC (WndProcCast)

#if wxUSE_ITSY_BITSY
    #define IBS_HORZCAPTION    0x4000L
    #define IBS_VERTCAPTION    0x8000L

    UINT    APIENTRY ibGetCaptionSize( HWND hWnd  ) ;
    UINT    APIENTRY ibSetCaptionSize( HWND hWnd, UINT nSize ) ;
    MRESULT APIENTRY ibDefWindowProc( HWND hWnd, ULONG ulMsg, MPARAM wParam, MPARAM lParam ) ;
    VOID    APIENTRY ibAdjustWindowRect( HWND hWnd, LPRECT lprc ) ;
#endif // wxUSE_ITSY_BITSY

/*
 * Decide what window classes we're going to use
 * for this combination of CTl3D/FAFA settings
 */

#define STATIC_CLASS     _T("STATIC")
#define STATIC_FLAGS     (SS_TEXT|DT_LEFT|SS_LEFT|WS_VISIBLE)
#define CHECK_CLASS      _T("BUTTON")
#define CHECK_FLAGS      (BS_AUTOCHECKBOX|WS_TABSTOP)
#define CHECK_IS_FAFA   FALSE
#define RADIO_CLASS      _T("BUTTON" )
#define RADIO_FLAGS      (BS_AUTORADIOBUTTON|WS_VISIBLE)
#define RADIO_SIZE       20
#define RADIO_IS_FAFA   FALSE
#define PURE_WINDOWS
/*  PM has no group box button style
#define GROUP_CLASS      "BUTTON"
#define GROUP_FLAGS      (BS_GROUPBOX|WS_CHILD|WS_VISIBLE)
*/

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
extern LONG APIENTRY wxSubclassedGenericControlProc(WXHWND hWnd, WXDWORD message, WXWPARAM wParam, WXLPARAM lParam);

// ---------------------------------------------------------------------------
// constants which might miss from some compilers' headers
// ---------------------------------------------------------------------------

#if !defined(WS_EX_CLIENTEDGE)
    #define WS_EX_CLIENTEDGE 0
#endif

#ifndef ENDSESSION_LOGOFF
    #define ENDSESSION_LOGOFF    0x80000000
#endif

// ---------------------------------------------------------------------------
// debug messages -- OS/2 has no native debug output system
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// macros to make casting between WXFOO and FOO a bit easier: the GetFoo()
// returns Foo cast to the Windows type for oruselves, while GetFooOf() takes
// an argument which should be a pointer or reference to the object of the
// corresponding class (this depends on the macro)
// ---------------------------------------------------------------------------

#define GetHwnd()               ((HWND)GetHWND())
#define GetHwndOf(win)          ((HWND)((win)->GetHWND()))
// old name
#define GetWinHwnd              GetHwndOf

#define GetHdc()                ((HDC)GetHDC())
#define GetHdcOf(dc)            ((HDC)(dc).GetHDC())

#define GetHicon()              ((HICON)GetHICON())
#define GetHiconOf(icon)        ((HICON)(icon).GetHICON())

#define GetHaccel()             ((HACCEL)GetHACCEL())
#define GetHaccelOf(table)      ((HACCEL)((table).GetHACCEL()))

#define GetHmenu()              ((HMENU)GetHMenu())
#define GetHmenuOf(menu)        ((HMENU)menu->GetHMenu())

// ---------------------------------------------------------------------------
// global data
// ---------------------------------------------------------------------------

// The MakeProcInstance version of the function wxSubclassedGenericControlProc
WXDLLEXPORT_DATA(extern) wxGenericControlSubClassProc;
WXDLLEXPORT_DATA(extern wxChar*) wxBuffer;
WXDLLEXPORT_DATA(extern HINSTANCE) wxhInstance;

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

extern "C"
{
WXDLLEXPORT HINSTANCE wxGetInstance();
}

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

// Convert a PM Error code to a string
WXDLLEXPORT extern wxString wxPMErrorToStr(ERRORID vError);

// Does this window style specify any border?
inline bool wxStyleHasBorder(long style)
{
  return (style & (wxSIMPLE_BORDER | wxRAISED_BORDER |
                   wxSUNKEN_BORDER | wxDOUBLE_BORDER)) != 0;
}

#endif
    // _WX_PRIVATE_H_
