/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/dcmemory.h"
#include "wx/msw/dcmemory.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/log.h"
#endif

#include "wx/display.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxMemoryDCImpl
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxMemoryDCImpl, wxMSWDCImpl);

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner )
        : wxMSWDCImpl( owner )
{
    CreateCompatible(nullptr);
    Init();
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
        : wxMSWDCImpl( owner )
{
    CreateCompatible(nullptr);
    Init();
    DoSelect(bitmap);
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc )
        : wxMSWDCImpl( owner )
{
    wxCHECK_RET( dc, wxT("null dc in wxMemoryDC ctor") );

    CreateCompatible(dc);

    Init();
}

void wxMemoryDCImpl::Init()
{
    if ( m_ok )
    {
        SetBrush(*wxWHITE_BRUSH);
        SetPen(*wxBLACK_PEN);
        SetFont(*wxNORMAL_FONT);

        // the background mode is only used for text background and is set in
        // DrawText() to OPAQUE as required, otherwise always TRANSPARENT
        ::SetBkMode( GetHdc(), TRANSPARENT );
    }
}

bool wxMemoryDCImpl::CreateCompatible(wxDC *dc)
{
    wxDCImpl *impl = dc ? dc->GetImpl() : nullptr ;
    wxMSWDCImpl *msw_impl = wxDynamicCast( impl, wxMSWDCImpl );
    if ( dc && !msw_impl)
    {
        m_ok = false;
        return false;
    }

    m_hDC = (WXHDC)::CreateCompatibleDC(dc ? GetHdcOf(*msw_impl) : nullptr);

    // as we created the DC, we must delete it in the dtor
    m_bOwnsDC = true;

    m_ok = m_hDC != 0;

    return m_ok;
}

void wxMemoryDCImpl::DoSelect( const wxBitmap& bitmap )
{
    // select old bitmap out of the device context
    if ( m_oldBitmap )
    {
        ::SelectObject(GetHdc(), (HBITMAP) m_oldBitmap);
        if ( m_selectedBitmap.IsOk() )
        {
            m_selectedBitmap.SetSelectedInto(nullptr);
            m_selectedBitmap = wxNullBitmap;
        }
    }

    // check for whether the bitmap is already selected into a device context
    wxASSERT_MSG( !bitmap.GetSelectedInto() ||
                  (bitmap.GetSelectedInto() == GetOwner()),
                  wxT("Bitmap is selected in another wxMemoryDC, delete the first wxMemoryDC or use SelectObject(nullptr)") );

    m_selectedBitmap = bitmap;
    WXHBITMAP hBmp = m_selectedBitmap.GetHBITMAP();
    if ( !hBmp )
        return;

    m_selectedBitmap.SetSelectedInto(GetOwner());
    hBmp = (WXHBITMAP)::SelectObject(GetHdc(), (HBITMAP)hBmp);

    if ( !hBmp )
    {
        wxLogLastError(wxT("SelectObject(memDC, bitmap)"));

        wxFAIL_MSG(wxT("Couldn't select a bitmap into wxMemoryDC"));
    }
    else if ( !m_oldBitmap )
    {
        m_oldBitmap = hBmp;
    }

    // Remember content scale factor used by the bitmap: we don't use it
    // ourselves, but this can be needed later for creating fonts of the
    // correct size.
    m_contentScaleFactor = bitmap.GetScaleFactor();

    // The font may need to be adjusted for the new scale factor.
    SetFont(GetFont());
}

wxSize wxMemoryDCImpl::GetPPI() const
{
    return wxDisplay::GetStdPPI() * GetContentScaleFactor();
}

void wxMemoryDCImpl::SetFont(const wxFont& font)
{
    // We need to adjust the font size by the ratio between the scale factor we
    // use and the default/global scale factor used when creating fonts.
    wxFont scaledFont = font;
    if ( scaledFont.IsOk() )
        scaledFont.WXAdjustToPPI(GetPPI());
    wxMSWDCImpl::SetFont(scaledFont);
}

void wxMemoryDCImpl::DoGetSize(int *width, int *height) const
{
    if ( m_selectedBitmap.IsOk() )
    {
        *width = m_selectedBitmap.GetWidth();
        *height = m_selectedBitmap.GetHeight();
    }
    else
    {
        *width = 0;
        *height = 0;
    }
}

// the rest of this file deals with drawing rectangles workaround, disabled by
// default

#define wxUSE_MEMORY_DC_DRAW_RECTANGLE 0

#if wxUSE_MEMORY_DC_DRAW_RECTANGLE

// For some reason, drawing a rectangle on a memory DC has problems.
// Use this substitute if we can.
static void wxDrawRectangle(wxDC& dc, wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxBrush brush(dc.GetBrush());
    wxPen pen(dc.GetPen());
    if (brush.IsOk() && brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
    {
        HBRUSH hBrush = (HBRUSH) brush.GetResourceHandle() ;
        if (hBrush)
        {
            RECT rect;
            rect.left = x; rect.top = y;
            rect.right = x + width - 1;
            rect.bottom = y + height - 1;
            ::FillRect((HDC) dc.GetHDC(), &rect, hBrush);
        }
    }
    width --; height --;
    if (pen.IsOk() && pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
    {
        dc.DrawLine(x, y, x + width, y);
        dc.DrawLine(x, y, x, y + height);
        dc.DrawLine(x, y+height, x+width, y + height);
        dc.DrawLine(x+width, y+height, x+width, y);
    }
}

#endif // wxUSE_MEMORY_DC_DRAW_RECTANGLE

void wxMemoryDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    // Set this to 1 to work around an apparent video driver bug
    // (visible with e.g. 70x70 rectangle on a memory DC; see Drawing sample)
#if wxUSE_MEMORY_DC_DRAW_RECTANGLE
    if (m_brush.IsOk() && m_pen.IsOk() &&
        (m_brush.GetStyle() == wxBRUSHSTYLE_SOLID || m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT) &&
        (m_pen.GetStyle() == wxPENSTYLE_SOLID || m_pen.GetStyle() == wxPENSTYLE_TRANSPARENT) &&
        (GetLogicalFunction() == wxCOPY))
    {
        wxDrawRectangle(* this, x, y, width, height);
    }
    else
#endif // wxUSE_MEMORY_DC_DRAW_RECTANGLE
    {
        wxMSWDCImpl::DoDrawRectangle(x, y, width, height);
    }
}
