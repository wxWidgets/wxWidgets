/////////////////////////////////////////////////////////////////////////////
// Name:        private.h
// Purpose:     Private declarations
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_H_
#define _WX_PRIVATE_H_

#include "wx/defs.h"

#include <windows.h>

#define VIEWPORT_EXTENT 1000

class WXDLLEXPORT wxFont ;

WXDLLEXPORT void wxGetCharSize(WXHWND wnd, int *x, int *y,wxFont *the_font);
WXDLLEXPORT void wxSliderEvent(WXHWND control, WXWORD wParam, WXWORD pos);
WXDLLEXPORT wxWindow* wxFindWinFromHandle(WXHWND hWnd);
WXDLLEXPORT void wxScrollBarEvent(WXHWND hbar, WXWORD wParam, WXWORD pos);

WXDLLEXPORT_DATA(extern HICON) wxSTD_FRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxSTD_MDIPARENTFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxSTD_MDICHILDFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_FRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_MDIPARENTFRAME_ICON;
WXDLLEXPORT_DATA(extern HICON) wxDEFAULT_MDICHILDFRAME_ICON;
WXDLLEXPORT_DATA(extern HFONT) wxSTATUS_LINE_FONT;

WXDLLEXPORT HINSTANCE wxGetInstance();
WXDLLEXPORT void wxFillLogFont(LOGFONT *logFont, wxFont *font);
WXDLLEXPORT wxFont wxCreateFontFromLogFont(LOGFONT *logFont); // , bool createNew = TRUE);

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

#if !defined(APIENTRY)  // NT defines APIENTRY, 3.x not
#define APIENTRY FAR PASCAL
#endif

#ifdef __WIN32__
#define _EXPORT /**/
#else
#define _EXPORT _export
typedef signed short int SHORT ;
#endif

#if !defined(__WIN32__)  // 3.x uses FARPROC for dialogs
#define DLGPROC FARPROC
#endif

#if wxUSE_PENWIN
WXDLLEXPORT void wxRegisterPenWin(void);
WXDLLEXPORT void wxCleanUpPenWin(void);
WXDLLEXPORT void wxEnablePenAppHooks (bool hook);
#endif

#if wxUSE_ITSY_BITSY
#define IBS_HORZCAPTION    0x4000L
#define IBS_VERTCAPTION    0x8000L

UINT    WINAPI ibGetCaptionSize( HWND hWnd  ) ;
UINT    WINAPI ibSetCaptionSize( HWND hWnd, UINT nSize ) ;
LRESULT WINAPI ibDefWindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam ) ;
VOID    WINAPI ibAdjustWindowRect( HWND hWnd, LPRECT lprc ) ;
#endif

/* When implementing a new item, be sure to:
 *
 * - add the item to the parent panel
 * - set window_parent to the parent
 * - NULL any extra child window pointers not created for this item
 *   (e.g. label control that wasn't needed)
 * - delete any extra child windows in the destructor (e.g. label control)
 * - implement GetSize and SetSize
 * - to find panel position if coordinates are (-1, -1), use GetPosition
 * - call AdvanceCursor after creation, for panel layout mechanism.
 *
 */

#if CTL3D
#include <wx/msw/ctl3d/ctl3d.h>
#endif

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

#define MEANING_CHARACTER '0'
#define DEFAULT_ITEM_WIDTH  200
#define DEFAULT_ITEM_HEIGHT 80

// Scale font to get edit control height
#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (3*(cy)/2)

// Generic subclass proc, for panel item moving/sizing and intercept
// EDIT control VK_RETURN messages
extern LONG APIENTRY _EXPORT
  wxSubclassedGenericControlProc(WXHWND hWnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

// Find maximum size of window/rectangle
WXDLLEXPORT extern void wxFindMaxSize(WXHWND hwnd, RECT *rect);

// List of scrollbar controls
WXDLLEXPORT_DATA(extern wxList) wxScrollBarList;
// The MakeProcInstance version of the function wxSubclassedGenericControlProc
WXDLLEXPORT_DATA(extern FARPROC) wxGenericControlSubClassProc;
WXDLLEXPORT_DATA(extern char*) wxBuffer;
WXDLLEXPORT_DATA(extern HINSTANCE) wxhInstance;

WXDLLEXPORT wxWindow* wxFindControlFromHandle(WXHWND hWnd);
WXDLLEXPORT void wxAddControlHandle(WXHWND hWnd, wxWindow *item);

// Safely get the window text (i.e. without using fixed size buffer)
WXDLLEXPORT extern wxString wxGetWindowText(WXHWND hWnd);

// Does this window style specify any border?
inline bool wxStyleHasBorder(long style)
{
  return (style & (wxSIMPLE_BORDER | wxRAISED_BORDER |
                   wxSUNKEN_BORDER | wxDOUBLE_BORDER)) != 0;
}

#if !defined(__WIN32__) && !defined(WS_EX_CLIENTEDGE)
  #define WS_EX_CLIENTEDGE 0
#endif

#if defined(__WIN32__) && !defined(WS_EX_CLIENTEDGE)
  #define WS_EX_CLIENTEDGE 0x00000200L
#endif

#if defined(__WIN95__) && defined(__WXDEBUG__) && wxUSE_DBWIN32
#ifdef OutputDebugString
#undef OutputDebugString
#endif

#define OutputDebugString OutputDebugStringW95
extern void OutputDebugStringW95(const char*, ...);
#endif

#endif
    // _WX_PRIVATE_H_
