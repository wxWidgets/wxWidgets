/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statbmp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/window.h"
#include "wx/msw/private.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/statbmp.h"
#endif

#include <stdio.h>

// ---------------------------------------------------------------------------
// macors
// ---------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxStaticBitmap
// ---------------------------------------------------------------------------

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
                            const wxBitmap& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    Init();

    SetName(name);
    if (parent)
        parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    m_windowStyle = style;

    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));

#ifdef __WIN32__
    // create a static control with either SS_BITMAP or SS_ICON style depending
    // on what we have here
    const wxChar *classname = _T("STATIC");
    int winstyle = m_isIcon ? SS_ICON : SS_BITMAP;
#else // Win16
    const wxChar *classname = _T("BUTTON");
    int winstyle = BS_OWNERDRAW;
#endif // Win32

    m_hWnd = (WXHWND)::CreateWindow
                       (
                        classname,
                        _T(""),
                        winstyle | WS_CHILD | WS_VISIBLE,
                        0, 0, 0, 0,
                        (HWND)parent->GetHWND(),
                        (HMENU)m_windowId,
                        wxGetInstance(),
                        NULL
                       );

    wxCHECK_MSG( m_hWnd, FALSE, _T("Failed to create static bitmap") );

    SetBitmap(bitmap);

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(GetParent()->GetFont());

    SetSize(x, y, width, height);

    return TRUE;
}

bool wxStaticBitmap::ImageIsOk() const
{
    if ( m_isIcon && m_image.icon )
        return m_image.icon->Ok();
    else if ( m_image.bitmap )
        return m_image.bitmap->Ok();
    else
        return FALSE;
}

void wxStaticBitmap::Free()
{
    if ( m_isIcon )
        delete m_image.icon;
    else
        delete m_image.bitmap;

    m_image.icon = NULL;
}

wxSize wxStaticBitmap::DoGetBestSize()
{
    // reuse the current size (as wxWindow does) instead of using some
    // arbitrary default size (as wxControl, our immediate base class, does)
    return wxWindow::DoGetBestSize();
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    Free();

    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));
    if ( m_isIcon )
        m_image.icon = new wxIcon((const wxIcon&)bitmap);
    else
        m_image.bitmap = new wxBitmap(bitmap);

    int x, y;
    int w, h;
    GetPosition(&x, &y);
    GetSize(&w, &h);

#ifdef __WIN32__
    HANDLE handle = m_isIcon ? (HANDLE)m_image.icon->GetHICON()
                             : (HANDLE)m_image.bitmap->GetHBITMAP();
    ::SendMessage((HWND)m_hWnd, STM_SETIMAGE,
                  m_isIcon ? IMAGE_ICON : IMAGE_BITMAP, (LPARAM)handle);
#endif // Win32

    if ( ImageIsOk() )
    {
        int width = bitmap.GetWidth(),
            height = bitmap.GetHeight();
        if ( width && height )
        {
            w = width;
            h = height;

            ::MoveWindow((HWND)GetHWND(), x, y, width, height, FALSE);
        }
    }

    RECT rect ;
    rect.left   = x ;
    rect.top    = y ;
    rect.right  = x + w ;
    rect.bottom = y + h ;
    InvalidateRect((HWND)GetParent()->GetHWND(), &rect, TRUE);
}

// under Win32 we use the standard static control style for this
#ifdef __WIN16__
bool wxStaticBitmap::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;

    wxBitmap* bitmap = m_image.bitmap;
    if ( !bitmap->Ok() )
        return FALSE;

    HDC hDC = lpDIS->hDC;
    HDC memDC = ::CreateCompatibleDC(hDC);

    HBITMAP old = (HBITMAP) ::SelectObject(memDC, (HBITMAP) bitmap->GetHBITMAP());

    if (!old)
        return FALSE;

    int x = lpDIS->rcItem.left;
    int y = lpDIS->rcItem.top;
    int width = lpDIS->rcItem.right - x;
    int height = lpDIS->rcItem.bottom - y;

    // Centre the bitmap in the control area
    int x1 = (int) (x + ((width - bitmap->GetWidth()) / 2));
    int y1 = (int) (y + ((height - bitmap->GetHeight()) / 2));

    ::BitBlt(hDC, x1, y1, bitmap->GetWidth(), bitmap->GetHeight(), memDC, 0, 0, SRCCOPY);

    ::SelectObject(memDC, old);

    ::DeleteDC(memDC);

    return TRUE;
}

long wxStaticBitmap::MSWWindowProc(WXUINT nMsg,
                                   WXWPARAM wParam,
                                   WXLPARAM lParam)
{
    // Ensure that static items get messages. Some controls don't like this
    // message to be intercepted (e.g. RichEdit), hence the tests.
    if ( nMsg == WM_NCHITTEST )
        return (long)HTCLIENT;

    return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}
#endif // Win16
