/////////////////////////////////////////////////////////////////////////////
// Name:        wince/missing.h
// Purpose:     Missing things in WinCE
// Author:      Marco Cavallini
// Modified by:
// Created:     16/11/2002
// RCS-ID:      
// Copyright:   (c) KOAN SAS ( www.koansoftware.com )
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CE_MISSING_H_
#define _WX_CE_MISSING_H_

#include "wx/msw/private.h"
#include "shellapi.h"

inline BOOL IsIconic( HWND hWnd )
{
    // Probably not right...
#if 0
    long style = GetWindowLong(hWnd, GWL_STYLE);
    return ((style & WS_MINIMIZE) == 0);
#endif
    return FALSE;
}

#ifdef WIN32_PLATFORM_PSPC
#define SM_CXCURSOR             13
#define SM_CYCURSOR             14
#endif

// Missing from eVC 4 for some reason
#ifndef I_IMAGENONE
#define I_IMAGENONE (-2)
#endif

#ifndef TBSTYLE_NO_DROPDOWN_ARROW
#define TBSTYLE_NO_DROPDOWN_ARROW 0x0080
#endif

#if _WIN32_WCE >= 400
// aygshell.h missing from eVC 4 for some reason
#define SHCMBM_GETMENU      (WM_USER + 402)
#define SHCMBM_SETSUBMENU   (WM_USER + 400) // wparam == id of button, lParam == hmenu, return is old hmenu
#define SHCMBM_GETSUBMENU   (WM_USER + 401) // lParam == ID
#define SHCMBM_GETMENU      (WM_USER + 402) // get the owning hmenu (as specified in the load resource)

#define SHIDIF_DONEBUTTON           0x0001
#define SHIDIF_SIZEDLG              0x0002
#define SHIDIF_SIZEDLGFULLSCREEN    0x0004
#define SHIDIF_SIPDOWN              0x0008
#define SHIDIF_FULLSCREENNOMENUBAR  0x0010
#define SHIDIF_EMPTYMENU            0x0020

#define SHFS_SHOWTASKBAR            0x0001
#define SHFS_HIDETASKBAR            0x0002
#define SHFS_SHOWSIPBUTTON          0x0004
#define SHFS_HIDESIPBUTTON          0x0008
#define SHFS_SHOWSTARTICON          0x0010
#define SHFS_HIDESTARTICON          0x0020

typedef struct tagSHMENUBARINFO
{
    DWORD cbSize;               // IN  - Indicates which members of struct are valid
	HWND hwndParent;            // IN
    DWORD dwFlags;              // IN  - Some features we want
    UINT nToolBarId;            // IN  - Which toolbar are we using
    HINSTANCE hInstRes;         // IN  - Instance that owns the resources
    int nBmpId;
    int cBmpImages;             // IN  - Count of bitmap images
	HWND hwndMB;                // OUT
    COLORREF clrBk;             // IN  - background color of the menu bar (excluding sip)
} SHMENUBARINFO, *PSHMENUBARINFO;

#ifdef __cplusplus
extern "C" {
#endif

BOOL SHFullScreen(HWND hwndRequester, DWORD dwState);

WINSHELLAPI BOOL  SHCreateMenuBar(SHMENUBARINFO *pmbi);

#ifdef __cplusplus
}
#endif

#endif

/*
Most of these are in MSVC++6.0   <wx\wince\winuser.h>
see also                         <wx\msw\gnuwin32\winresrc.h>
*/

// ----------------------------------------------------------------------------
// Used in msgdlg.cpp, evtloop.cpp
// ----------------------------------------------------------------------------

#ifndef MB_TASKMODAL
#define MB_TASKMODAL 0x2000
#endif

////////////////////////////////////////////////////////

// JACS: I've commented these out in order to start from
// scratch. I don't think we should simply add styles,
// classes etc. when we don't know if they're supported
// by WinCE libraries.

#if 0

/*
 * Extended Window Styles
 */
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L

#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L

#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L

#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L


#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)


/*
 * Flags for TrackPopupMenu
 */
#define TPM_LEFTBUTTON  0x0000L
#define TPM_RIGHTBUTTON 0x0002L
#define TPM_LEFTALIGN   0x0000L
#define TPM_CENTERALIGN 0x0004L
#define TPM_RIGHTALIGN  0x0008L

#define TPM_TOPALIGN        0x0000L
#define TPM_VCENTERALIGN    0x0010L
#define TPM_BOTTOMALIGN     0x0020L

#define TPM_HORIZONTAL      0x0000L     /* Horz alignment matters more */
#define TPM_VERTICAL        0x0040L     /* Vert alignment matters more */
#define TPM_NONOTIFY        0x0080L     /* Don't send any notification msgs */
#define TPM_RETURNCMD       0x0100L

#define TPM_RECURSE         0x0001L



////////////////////////////////////////////////////////
// Missing declarations into WinCE include files (MCK)
////////////////////////////////////////////////////////

HCURSOR WINAPI 
DestroyCursor(HCURSOR hCursor);

HCURSOR WINAPI 
CreateCursor(HINSTANCE hInst, 
    int xHotSpot, 
    int yHotSpot, 
    int nWidth, 
    int nHeight, 
    CONST VOID *pvANDPlane, 
    CONST VOID *pvXORPlane ); 

////////////////////////////////////////////////////////

int WINAPI
GetScrollPos(
    HWND hWnd,
    int nBar);

bool WINAPI
GetScrollRange(
    HWND hWnd,
    int nBar,
    LPINT lpMinPos,
    LPINT lpMaxPos);

bool WINAPI
ScrollWindow(
    HWND hWnd,
    int XAmount,
    int YAmount,
    CONST RECT *lpRect,
    CONST RECT *lpClipRect);

LRESULT WINAPI
CallWindowProcA(
    WNDPROC lpPrevWndFunc,
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

LRESULT WINAPI
CallWindowProcW(
    WNDPROC lpPrevWndFunc,
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

#ifdef UNICODE
#define CallWindowProc  CallWindowProcW
#else
#define CallWindowProc  CallWindowProcA
#endif // !UNICODE

#endif
 // 0

////////////////////////////////////////////////////////

#endif // _WX_CE_MISSING_H_
