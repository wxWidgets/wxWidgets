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

#if wxUSE_STATBMP

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

// we may have either bitmap or icon: if a bitmap with mask is passed, we
// will transform it to an icon ourselves because otherwise the mask will
// be ignored by Windows
// note that this function will create a new object every time
// it is called even if the image needs no conversion

#ifndef __WIN16__

static wxGDIImage* ConvertImage( const wxGDIImage& bitmap )
{
    bool isIcon = bitmap.IsKindOf( CLASSINFO(wxIcon) );

    if( !isIcon )
    {
        wxASSERT_MSG( wxDynamicCast(&bitmap, wxBitmap),
                      _T("not an icon and not a bitmap?") );

        const wxBitmap& bmp = (const wxBitmap&)bitmap;
        wxMask *mask = bmp.GetMask();
        if ( mask && mask->GetMaskBitmap() )
        {
            wxIcon* icon = new wxIcon;
            icon->CopyFromBitmap(bmp);

            return icon;
        }

        return new wxBitmap( bmp );
    }

    // copying a bitmap is a cheap operation
    return new wxIcon( (const wxIcon&)bitmap );
}

#endif

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
                            const wxGDIImage& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    // default border for this control is none
    if ( (style & wxBORDER_MASK) == wxBORDER_DEFAULT )
    {
        style |= wxBORDER_NONE;
    }
    
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
    wxGDIImage *image = (wxGDIImage *)NULL;
    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));

#ifdef __WIN16__
    wxASSERT_MSG( !m_isIcon, "Icons are not supported in wxStaticBitmap under WIN16." );
    image = &bitmap;
#endif

#ifndef __WIN16__
    image = ConvertImage( bitmap );
    m_isIcon = image->IsKindOf( CLASSINFO(wxIcon) );
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

    if ( m_windowStyle & wxCLIP_SIBLINGS )
        winstyle |= WS_CLIPSIBLINGS;


    m_hWnd = (WXHWND)::CreateWindow
                       (
                        classname,
                        wxT(""),
                        // NOT DISABLED!!! We want to move it in Dialog Editor.
                        winstyle | WS_CHILD | WS_VISIBLE /* | WS_CLIPSIBLINGS */ , // | WS_DISABLED,
                        0, 0, 0, 0,
                        (HWND)parent->GetHWND(),
                        (HMENU)m_windowId,
                        wxGetInstance(),
                        NULL
                       );

    wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create static bitmap") );

    // no need to delete the new image
    SetImageNoCopy( image );

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

void wxStaticBitmap::SetImage( const wxGDIImage* image )
{
    wxGDIImage* convertedImage = ConvertImage( *image );
    SetImageNoCopy( convertedImage );
}

void wxStaticBitmap::SetImageNoCopy( wxGDIImage* image)
{
    Free();

    m_isIcon = image->IsKindOf( CLASSINFO(wxIcon) );
    // the image has already been copied
    m_image = image;

    int x, y;
    int w, h;
    GetPosition(&x, &y);
    GetSize(&w, &h);

#ifdef __WIN32__
    HANDLE handle = (HANDLE)m_image->GetHandle();
    LONG style = ::GetWindowLong( (HWND)GetHWND(), GWL_STYLE ) ;
    ::SetWindowLong( (HWND)GetHWND(), GWL_STYLE, ( style & ~( SS_BITMAP|SS_ICON ) ) |
                     ( m_isIcon ? SS_ICON : SS_BITMAP ) );
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

#endif // wxUSE_STATBMP
