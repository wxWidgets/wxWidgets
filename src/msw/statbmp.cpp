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

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxStaticBitmap
// ---------------------------------------------------------------------------

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
                            const wxGDIImage& bitmap,
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

    // we may have either bitmap or icon: if a bitmap with mask is passed, we
    // will transform it to an icon ourselves because otherwise the mask will
    // be ignored by Windows
    wxIcon *icon = (wxIcon *)NULL;
    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));

#ifdef __WIN16__
    wxASSERT_MSG( !m_isIcon, "Icons are not supported in wxStaticBitmap under WIN16." );
#endif

#ifndef __WIN16__
    if ( !m_isIcon )
    {
        const wxBitmap& bmp = (const wxBitmap&)bitmap;
        wxMask *mask = bmp.GetMask();
        if ( mask && mask->GetMaskBitmap() )
        {
            icon = new wxIcon;
            icon->CopyFromBitmap(bmp);

            m_isIcon = TRUE;
        }
    }
#endif

#ifdef __WIN32__
    // create a static control with either SS_BITMAP or SS_ICON style depending
    // on what we have here
    const wxChar *classname = wxT("STATIC");
    int winstyle = m_isIcon ? SS_ICON : SS_BITMAP;
#else // Win16
    const wxChar *classname = wxT("BUTTON");
    int winstyle = BS_OWNERDRAW;
#endif // Win32

    m_hWnd = (WXHWND)::CreateWindow
                       (
                        classname,
                        wxT(""),
                        // NOT DISABLED!!! We want to move it in Dialog Editor.
                        winstyle | WS_CHILD | WS_VISIBLE, // | WS_DISABLED,
                        0, 0, 0, 0,
                        (HWND)parent->GetHWND(),
                        (HMENU)m_windowId,
                        wxGetInstance(),
                        NULL
                       );

    wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create static bitmap") );

    SetImage(icon ? icon : &bitmap);
    delete icon; // may be NULL, ok

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(GetParent()->GetFont());

    SetSize(x, y, width, height);

    return TRUE;
}

bool wxStaticBitmap::ImageIsOk() const
{
    return m_image && m_image->Ok();
}

void wxStaticBitmap::Free()
{
    delete m_image;

    m_image = NULL;
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    // reuse the current size (as wxWindow does) instead of using some
    // arbitrary default size (as wxControl, our immediate base class, does)
    return wxWindow::DoGetBestSize();
}

void wxStaticBitmap::SetImage(const wxGDIImage* image)
{
    Free();

    const wxIcon *icon = wxDynamicCast(image, wxIcon);
    m_isIcon = icon != NULL;
    if ( m_isIcon )
    {
        m_image = new wxIcon(*icon);
    }
    else
    {
        wxASSERT_MSG( wxDynamicCast(image, wxBitmap),
                      _T("not an icon and not a bitmap?") );

        const wxBitmap *bitmap = (wxBitmap *)image;

        m_image = new wxBitmap(*bitmap);
    }

    int x, y;
    int w, h;
    GetPosition(&x, &y);
    GetSize(&w, &h);

#ifdef __WIN32__
    HANDLE handle = (HANDLE)m_image->GetHandle();
    ::SendMessage(GetHwnd(), STM_SETIMAGE,
                  m_isIcon ? IMAGE_ICON : IMAGE_BITMAP, (LPARAM)handle);
#endif // Win32

    if ( ImageIsOk() )
    {
        int width = image->GetWidth(),
            height = image->GetHeight();
        if ( width && height )
        {
            w = width;
            h = height;

            ::MoveWindow(GetHwnd(), x, y, width, height, FALSE);
        }
    }

    RECT rect;
    rect.left   = x;
    rect.top    = y;
    rect.right  = x + w;
    rect.bottom = y + h;
    InvalidateRect(GetHwndOf(GetParent()), &rect, TRUE);
}

// under Win32 we use the standard static control style for this
#ifdef __WIN16__
bool wxStaticBitmap::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;

    wxCHECK_MSG( !m_isIcon, FALSE, _T("icons not supported in wxStaticBitmap") );

    wxBitmap* bitmap = (wxBitmap *)m_image;
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
#endif // Win16

// We need this or the control can never be moved e.g. in Dialog Editor.
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

