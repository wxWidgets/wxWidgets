/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxGDIImage& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    // we may have either bitmap or icon: if a bitmap with mask is passed, we
    // will transform it to an icon ourselves because otherwise the mask will
    // be ignored by Windows
    wxGDIImage *image = (wxGDIImage *)NULL;
    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));

#ifdef __WIN16__
    wxASSERT_MSG( !m_isIcon, "Icons are not supported in wxStaticBitmap under WIN16." );
    image = &bitmap;
#else // Win32
    image = ConvertImage( bitmap );
    m_isIcon = image->IsKindOf( CLASSINFO(wxIcon) );
#endif // Win16/32

    // create the native control
    if ( !MSWCreateControl(
#ifdef __WIN32__
                           _T("STATIC"),
#else // Win16
                           _T("BUTTON"),
#endif // Win32/16
                           wxEmptyString, pos, size) )
    {
        // control creation failed
        return FALSE;
    }

    // no need to delete the new image
    SetImageNoCopy(image);

    return TRUE;
}

wxBorder wxStaticBitmap::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

WXDWORD wxStaticBitmap::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

#ifdef __WIN32__
    // what kind of control are we?
    msStyle |= m_isIcon ? SS_ICON : SS_BITMAP;

    // we use SS_CENTERIMAGE to prevent the control from resizing the bitmap to
    // fit to its size -- this is unexpected and doesn't happen in other ports
    msStyle |= SS_CENTERIMAGE;
#else // Win16
    msStyle |= BS_OWNERDRAW;
#endif // Win32/16

    return msStyle;
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
