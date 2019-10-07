/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBMP

#include "wx/statbmp.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/icon.h"
    #include "wx/dcclient.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/dib.h"
#include "wx/msw/private/winstyle.h"

#include "wx/sysopt.h"

#include <stdio.h>

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxStaticBitmap, wxStaticBitmapBase)
    EVT_SIZE(wxStaticBitmap::WXHandleSize)
wxEND_EVENT_TABLE()

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
    bool isIcon = bitmap.IsKindOf( wxCLASSINFO(wxIcon) );

    if( !isIcon )
    {
        wxASSERT_MSG( wxDynamicCast(&bitmap, wxBitmap),
                      wxT("not an icon and not a bitmap?") );

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
    wxGDIImage *image = ConvertImage( bitmap );

    // Note that m_isIcon must be set before calling MSWCreateControl() so that
    // it creates the control with the correct style, as returned by
    // MSWGetStyle(), which uses m_isIcon to determine whether to use SS_ICON
    // or SS_BITMAP.
    m_isIcon = image->IsKindOf( wxCLASSINFO(wxIcon) );

    // create the native control
    if ( !MSWCreateControl(wxT("STATIC"), wxEmptyString, pos, size) )
    {
        // control creation failed
        return false;
    }

    // no need to delete the new image
    SetImageNoCopy(image);

    // GetBestSize will work properly now, so set the best size if needed
    SetInitialSize(size);

    // if the application has no manifest and so the old comctl32.dll is
    // used, the images with alpha channel are not correctly drawn so we need
    // to draw them ourselves and it's easier to just always do it rather than
    // check if we have an image with alpha or not
    if ( wxTheApp->GetComCtl32Version() < 600 )
    {
        Bind(wxEVT_PAINT, &wxStaticBitmap::DoPaintManually, this);
    }

    return true;
}

WXDWORD wxStaticBitmap::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // what kind of control are we?
    msStyle |= m_isIcon ? SS_ICON : SS_BITMAP;

    // we use SS_CENTERIMAGE to prevent the control from resizing the bitmap to
    // fit to its size -- this is unexpected and doesn't happen in other ports
    //
    // and SS_NOTIFY is necessary to receive mouse events
    msStyle |= SS_CENTERIMAGE | SS_NOTIFY;

    return msStyle;
}

bool wxStaticBitmap::ImageIsOk() const
{
    return m_image && m_image->IsOk();
}

wxIcon wxStaticBitmap::GetIcon() const
{
    wxCHECK_MSG( m_image, wxIcon(), wxT("no image in wxStaticBitmap") );

    // we can't ask for an icon if all we have is a bitmap
    wxCHECK_MSG( m_isIcon, wxIcon(), wxT("no icon in this wxStaticBitmap") );

    return *(wxIcon *)m_image;
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    if ( m_isIcon )
    {
        // don't fail because we might have replaced the bitmap with icon
        // ourselves internally in ConvertImage() to keep the transparency but
        // the user code doesn't know about it so it still can use GetBitmap()
        // to retrieve the bitmap
        return wxBitmap(GetIcon());
    }
    else // we have a bitmap
    {
        wxCHECK_MSG( m_image, wxBitmap(), wxT("no image in wxStaticBitmap") );

        return *(wxBitmap *)m_image;
    }
}

void wxStaticBitmap::Init()
{
    m_isIcon = true;
    m_image = NULL;
    m_currentHandle = 0;
    m_ownsCurrentHandle = false;
}

void wxStaticBitmap::DeleteCurrentHandleIfNeeded()
{
    if ( m_ownsCurrentHandle )
    {
        ::DeleteObject(m_currentHandle);
        m_ownsCurrentHandle = false;
    }
}

void wxStaticBitmap::Free()
{
    MSWReplaceImageHandle(0);

    DeleteCurrentHandleIfNeeded();

    wxDELETE(m_image);
}

wxSize wxStaticBitmap::DoGetBestClientSize() const
{
    wxSize size;
    if ( ImageIsOk() )
    {
        size = m_image->GetSize();
    }
    else // No image yet
    {
        // this is completely arbitrary
        size.x =
        size.y = 16;
    }

    return size;
}

void wxStaticBitmap::WXHandleSize(wxSizeEvent& event)
{
    // Invalidate everything when our size changes as the image position (it's
    // drawn centred in the window client area) changes.
    Refresh();

    event.Skip();
}

void wxStaticBitmap::DoPaintManually(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    const wxSize size(GetSize());
    const wxBitmap bmp(GetBitmap());

    // Clear the background: notice that we're supposed to be transparent, so
    // use the parent background colour if we don't have our own instead of
    // falling back to the default
    const wxWindow *win = UseBgCol() ? this : GetParent();
    dc.SetBrush(win->GetBackgroundColour());
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    // Draw the image in the middle
    dc.DrawBitmap(bmp,
                  (size.GetWidth() - bmp.GetWidth()) / 2,
                  (size.GetHeight() - bmp.GetHeight()) / 2,
                  true /* use mask */);
}

void wxStaticBitmap::SetImage( const wxGDIImage* image )
{
    wxGDIImage* convertedImage = ConvertImage( *image );
    SetImageNoCopy( convertedImage );
}

void wxStaticBitmap::MSWReplaceImageHandle(WXLPARAM handle)
{
    HGDIOBJ oldHandle = (HGDIOBJ)::SendMessage(GetHwnd(), STM_SETIMAGE,
                  m_isIcon ? IMAGE_ICON : IMAGE_BITMAP, (LPARAM)handle);
    // detect if this is still the handle we passed before or
    // if the static-control made a copy of the bitmap!
    if (oldHandle != 0 && oldHandle != (HGDIOBJ) m_currentHandle)
    {
        // the static control made a copy and we are responsible for deleting it
        ::DeleteObject((HGDIOBJ) oldHandle);
    }
}

void wxStaticBitmap::SetImageNoCopy( wxGDIImage* image)
{
    wxSize sizeOld;
    if ( m_image )
        sizeOld = m_image->GetSize();

    wxSize sizeNew;
    if ( image )
        sizeNew = image->GetSize();

    const bool wasIcon = m_isIcon;

    Free();

    m_isIcon = image->IsKindOf( wxCLASSINFO(wxIcon) );
    // the image has already been copied
    m_image = image;

    // Normally we just use the handle of provided image but in some cases we
    // create our own temporary bitmap, so the actual handle may end up being
    // different from the original one.
    const HANDLE handleOrig = (HANDLE)m_image->GetHandle();
    HANDLE handle = handleOrig;

#if wxUSE_WXDIB
    if ( !m_isIcon )
    {
        // wxBitmap normally stores alpha in pre-multiplied format but
        // apparently STM_SETIMAGE message handler does pre-multiplication
        // internally so we need to undo the pre-multiplication here for a
        // while (this is similar to what we do in ImageList::Add()).
        const wxBitmap& bmp = static_cast<wxBitmap&>(*image);
        if ( bmp.HasAlpha() )
        {
            // For bitmap with alpha channel create temporary DIB with
            // not-premultiplied alpha values.
            handle = wxDIB(bmp.ConvertToImage(),
                           wxDIB::PixelFormat_NotPreMultiplied).Detach();
        }
    }
#endif // wxUSE_WXDIB

    if ( m_isIcon != wasIcon )
    {
        wxMSWWinStyleUpdater(GetHwnd())
            .TurnOff(SS_BITMAP | SS_ICON)
            .TurnOn(m_isIcon ? SS_ICON : SS_BITMAP);
    }

    MSWReplaceImageHandle((WXLPARAM)handle);

    m_currentHandle = (WXHANDLE)handle;
    m_ownsCurrentHandle = handle != handleOrig;

    if ( sizeNew != sizeOld )
    {
        InvalidateBestSize();
        SetSize(GetBestSize());
    }

    RECT rect = wxGetWindowRect(GetHwnd());
    ::InvalidateRect(GetHwndOf(GetParent()), &rect, TRUE);
}

#endif // wxUSE_STATBMP
