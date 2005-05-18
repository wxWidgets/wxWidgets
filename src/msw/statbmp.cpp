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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#include "wx/sysopt.h"

#include <stdio.h>

// ---------------------------------------------------------------------------
// macors
// ---------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxStaticBitmapStyle )

wxBEGIN_FLAGS( wxStaticBitmapStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

wxEND_FLAGS( wxStaticBitmapStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBitmap, wxControl,"wx/statbmp.h")

wxBEGIN_PROPERTIES_TABLE(wxStaticBitmap)
    wxPROPERTY_FLAGS( WindowStyle , wxStaticBitmapStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxStaticBitmap)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxStaticBitmap, wxWindow* , Parent , wxWindowID , Id , wxBitmap, Bitmap, wxPoint , Position , wxSize , Size )

#else
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

/*
    TODO PROPERTIES :
        bitmap
*/

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

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxGDIImage& bitmap,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    // we may have either bitmap or icon: if a bitmap with mask is passed, we
    // will transform it to an icon ourselves because otherwise the mask will
    // be ignored by Windows
    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));

    wxGDIImage *image = ConvertImage( bitmap );
    m_isIcon = image->IsKindOf( CLASSINFO(wxIcon) );

    // create the native control
    if ( !MSWCreateControl(_T("STATIC"), wxEmptyString, pos, size) )
    {
        // control creation failed
        return false;
    }

    // no need to delete the new image
    SetImageNoCopy(image);

    // GetBestSize will work properly now, so set the best size if needed
    SetBestSize(size);

    return true;
}

wxBorder wxStaticBitmap::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

WXDWORD wxStaticBitmap::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // what kind of control are we?
    msStyle |= m_isIcon ? SS_ICON : SS_BITMAP;

    // we use SS_CENTERIMAGE to prevent the control from resizing the bitmap to
    // fit to its size -- this is unexpected and doesn't happen in other ports
    msStyle |= SS_CENTERIMAGE;

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
    if ( ImageIsOk() )
    {
        wxSize best(m_image->GetWidth(), m_image->GetHeight());
        CacheBestSize(best);
        return best;
    }

    // this is completely arbitrary
    return wxSize(16, 16);
}

void wxStaticBitmap::SetImage( const wxGDIImage* image )
{
    wxGDIImage* convertedImage = ConvertImage( *image );
    SetImageNoCopy( convertedImage );
    InvalidateBestSize();
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
    ::InvalidateRect(GetHwndOf(GetParent()), &rect, TRUE);
}

WXLRESULT wxStaticBitmap::MSWWindowProc(WXUINT nMsg,
                                   WXWPARAM wParam,
                                   WXLPARAM lParam)
{
#ifndef __WXWINCE__
    static int s_useHTClient = -1;
    if (s_useHTClient == -1)
        s_useHTClient = wxSystemOptions::GetOptionInt(wxT("msw.staticbitmap.htclient"));
    if (s_useHTClient == 1)
    {
        // Ensure that static items get messages. Some controls don't like this
        // message to be intercepted (e.g. RichEdit), hence the tests.
        // Also, this code breaks some other processing such as enter/leave tracking
        // so it's off by default.
        
        if ( nMsg == WM_NCHITTEST )
            return (long)HTCLIENT;
    }
#endif

    return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}

#endif // wxUSE_STATBMP

