/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
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
    EVT_DPI_CHANGED(wxStaticBitmap::WXHandleDPIChanged)
wxEND_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxStaticBitmap
// ---------------------------------------------------------------------------

bool wxStaticBitmap::DoCreate(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(wxT("STATIC"), wxEmptyString, pos, size) )
    {
        // control creation failed
        return false;
    }

    DoUpdateImage(wxSize(), m_icon.IsOk());

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
    msStyle |= m_icon.IsOk() ? SS_ICON : SS_BITMAP;

    // we use SS_CENTERIMAGE to prevent the control from resizing the bitmap to
    // fit to its size -- this is unexpected and doesn't happen in other ports
    //
    // and SS_NOTIFY is necessary to receive mouse events
    msStyle |= SS_CENTERIMAGE | SS_NOTIFY;

    return msStyle;
}

wxSize wxStaticBitmap::GetImageSize() const
{
    return m_icon.IsOk() ? m_icon.GetSize()
                         : m_bitmapBundle.GetPreferredBitmapSizeFor(this);
}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{
    const wxSize sizeOld = GetImageSize();
    const bool wasIcon = m_icon.IsOk();

    m_icon = icon;
    m_bitmapBundle = wxBitmapBundle();

    DoUpdateImage(sizeOld, wasIcon);
}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    const wxSize sizeOld = GetImageSize();
    const bool wasIcon = m_icon.IsOk();

    m_icon = wxIcon();
    m_bitmapBundle = bitmap;

    DoUpdateImage(sizeOld, wasIcon);
}

wxIcon wxStaticBitmap::GetIcon() const
{
    wxIcon icon = m_icon;
    if ( !icon.IsOk() && m_bitmapBundle.IsOk() )
        icon.CopyFromBitmap(m_bitmapBundle.GetBitmapFor(this));

    return icon;
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    wxBitmap bitmap = m_bitmapBundle.GetBitmapFor(this);
    if ( !bitmap.IsOk() && m_icon.IsOk() )
        bitmap.CopyFromIcon(m_icon);

    return bitmap;
}

void wxStaticBitmap::Init()
{
    m_currentHandle = 0;
    m_ownsCurrentHandle = false;
}

void wxStaticBitmap::Free()
{
    m_bitmap.UnRef();

    MSWReplaceImageHandle(0);

    if ( m_ownsCurrentHandle )
    {
        ::DeleteObject(m_currentHandle);
        m_ownsCurrentHandle = false;
    }
}

wxSize wxStaticBitmap::DoGetBestClientSize() const
{
    wxSize size = GetImageSize();
    if ( size == wxDefaultSize )
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

void wxStaticBitmap::WXHandleDPIChanged(wxDPIChangedEvent& event)
{
    // Icons only exist in a single resolution, so don't bother updating in
    // this case.
    if ( !m_icon.IsOk() && m_bitmapBundle.IsOk() )
        DoUpdateImage(wxSize(), false /* not using an icon */);

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

void wxStaticBitmap::MSWReplaceImageHandle(WXHANDLE handle)
{
    HGDIOBJ oldHandle = (HGDIOBJ)::SendMessage(GetHwnd(), STM_SETIMAGE,
                  m_icon.IsOk() ? IMAGE_ICON : IMAGE_BITMAP, (LPARAM)handle);
    // detect if this is still the handle we passed before or
    // if the static-control made a copy of the bitmap!
    if (oldHandle != 0 && oldHandle != (HGDIOBJ) m_currentHandle)
    {
        // the static control made a copy and we are responsible for deleting it
        ::DeleteObject((HGDIOBJ) oldHandle);
    }
}

void wxStaticBitmap::DoUpdateImage(const wxSize& sizeOld, bool wasIcon)
{
    const wxSize sizeNew = GetImageSize();

    Free();

    // For the icons we just use its HICON directly, but for bitmaps we create
    // our own temporary bitmap and need to delete its handle manually later.
    if ( !m_icon.IsOk() )
    {
        wxBitmap bitmap = m_bitmapBundle.GetBitmapFor(this);

#if wxUSE_WXDIB
        // wxBitmap normally stores alpha in pre-multiplied format but
        // apparently STM_SETIMAGE message handler does pre-multiplication
        // internally so we need to undo the pre-multiplication here for a
        // while (this is similar to what we do in ImageList::Add()).
        if ( bitmap.HasAlpha() )
        {
            // For bitmap with alpha channel create temporary DIB with
            // not-premultiplied alpha values.
            m_currentHandle = wxDIB(bitmap.ConvertToImage(),
                                    wxDIB::PixelFormat_NotPreMultiplied)
                .Detach();
            m_ownsCurrentHandle = true;
        }
        else if ( bitmap.GetMask() )
        {
            // The native control doesn't know anything about the bitmap mask,
            // so we need to use an icon in this case.
            //
            // Alternatively, we could convert mask to alpha and it's not
            // really clear what is better, but we used to use icons for
            // bitmaps with masks before, so let's keep doing it until we find
            // a good reason not to.
            m_icon.CopyFromBitmap(bitmap);
        }
        else
#endif // wxUSE_WXDIB
        {
            // Just use the HBITMAP as is, but also make a copy of the bitmap
            // to ensure that HBITMAP remains valid for as long as we need it
            m_bitmap = bitmap;
            m_currentHandle = bitmap.GetHandle();
        }
    }

    const bool isIcon = m_icon.IsOk();
    if ( isIcon )
    {
        m_currentHandle = m_icon.GetHandle();
    }

    if ( isIcon != wasIcon )
    {
        wxMSWWinStyleUpdater(GetHwnd())
            .TurnOff(SS_BITMAP | SS_ICON)
            .TurnOn(isIcon ? SS_ICON : SS_BITMAP);
    }

    MSWReplaceImageHandle(m_currentHandle);

    if ( sizeNew != sizeOld )
    {
        InvalidateBestSize();
        SetSize(GetBestSize());
    }

    RECT rect = wxGetWindowRect(GetHwnd());
    ::InvalidateRect(GetHwndOf(GetParent()), &rect, TRUE);
}

#endif // wxUSE_STATBMP
