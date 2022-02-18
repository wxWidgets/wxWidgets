/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/overlaycmn.cpp
// Purpose:     common wxOverlay code
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// Copyright:   (c) wxWidgets team
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


#include "wx/overlay.h"
#include "wx/private/overlay.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxOverlay
// ----------------------------------------------------------------------------

wxOverlay::~wxOverlay()
{
    delete m_impl;
}

bool wxOverlay::IsNative() const
{
    return m_impl->IsNative();
}

bool wxOverlay::IsOk()
{
    return m_impl->IsOk();
}

void wxOverlay::Init( wxDC* dc, int x , int y , int width , int height )
{
    m_impl->Init(dc, x, y, width, height);
}

void wxOverlay::BeginDrawing( wxDC* dc)
{
    m_impl->BeginDrawing(dc);
    m_inDrawing = true ;
}

void wxOverlay::EndDrawing( wxDC* dc)
{
    m_impl->EndDrawing(dc);
    m_inDrawing = false ;
}

void wxOverlay::Clear( wxDC* dc)
{
    m_impl->Clear(dc);
}

void wxOverlay::Reset()
{
    wxASSERT_MSG(m_inDrawing==false,wxT("cannot reset overlay during drawing"));
    m_impl->Reset();
}
// ----------------------------------------------------------------------------

wxOverlay::Impl::~Impl()
{
}

bool wxOverlay::Impl::IsNative() const
{
    return true;
}

// ----------------------------------------------------------------------------
// wxDCOverlay
// ----------------------------------------------------------------------------

wxDCOverlay::wxDCOverlay(wxOverlay &overlay, wxDC *dc, int x , int y , int width , int height) :
    m_overlay(overlay)
{
    Init(dc, x, y, width, height);
}

wxDCOverlay::wxDCOverlay(wxOverlay &overlay, wxDC *dc) :
    m_overlay(overlay)
{
    const wxSize size(dc->GetSize());

    const wxCoord logicalLeft = dc->DeviceToLogicalX(0);
    const wxCoord logicalTop = dc->DeviceToLogicalY(0);
    const wxCoord logicalRight = dc->DeviceToLogicalX(size.GetWidth());
    const wxCoord logicalBottom = dc->DeviceToLogicalY(size.GetHeight());

    Init(dc,
         logicalLeft,
         logicalTop,
         logicalRight - logicalLeft,
         logicalBottom - logicalTop);
}

wxDCOverlay::~wxDCOverlay()
{
    m_overlay.EndDrawing(m_dc);
}

void wxDCOverlay::Init(wxDC *dc, int x , int y , int width , int height )
{
    m_dc = dc ;
    if ( !m_overlay.IsOk() )
    {
        m_overlay.Init(dc,x,y,width,height);
    }
    m_overlay.BeginDrawing(dc);
}

void wxDCOverlay::Clear()
{
    m_overlay.Clear(m_dc);
}

// ----------------------------------------------------------------------------
// generic implementation of wxOverlayImpl
// ----------------------------------------------------------------------------

#ifdef wxHAS_GENERIC_OVERLAY

#include "wx/window.h"

namespace {
class wxOverlayImpl: public wxOverlay::Impl
{
public:
    wxOverlayImpl();
    ~wxOverlayImpl();
    virtual bool IsNative() const wxOVERRIDE;
    virtual bool IsOk() wxOVERRIDE;
    virtual void Init(wxDC* dc, int x, int y, int width, int height) wxOVERRIDE;
    virtual void BeginDrawing(wxDC* dc) wxOVERRIDE;
    virtual void EndDrawing(wxDC* dc) wxOVERRIDE;
    virtual void Clear(wxDC* dc) wxOVERRIDE;
    virtual void Reset() wxOVERRIDE;

    wxBitmap m_bmpSaved;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    wxWindow* m_window;
};
} // namespace

wxOverlayImpl::wxOverlayImpl()
{
     m_window = NULL ;
     m_x = m_y = m_width = m_height = 0 ;
}

wxOverlayImpl::~wxOverlayImpl()
{
}

bool wxOverlayImpl::IsNative() const
{
    return false;
}

bool wxOverlayImpl::IsOk()
{
    return false;
}

void wxOverlayImpl::Init( wxDC* dc, int x , int y , int width , int height )
{
    if (m_bmpSaved.IsOk())
    {
        if (x != m_x || y != m_y || width != m_width || height != m_height)
        {
            if (m_window)
                m_window->Update();

            // Area that needs to be copied from window
            wxRect rect(x, y, width, height);
            wxRegion region(rect);
            rect.Intersect(wxRect(m_x, m_y, m_width, m_height));
            region.Subtract(rect);
            rect = region.GetBox();

            const wxBitmap bmpOld(m_bmpSaved);
            m_bmpSaved.Create(width, height, *dc);
            wxMemoryDC dcMem(m_bmpSaved);

            // Get new area from window
            dcMem.Blit(rect.x - x, rect.y - y, rect.width, rect.height, dc, rect.x, rect.y);

            // Copy old area to new position
            dcMem.DrawBitmap(bmpOld, m_x - x, m_y - y);

            m_x = x;
            m_y = y;
            m_width = width;
            m_height = height;
        }
        return;
    }
    m_window = dc->GetWindow();
    m_bmpSaved.Create(width, height, *dc);
    wxMemoryDC dcMem(m_bmpSaved);
    m_x = x ;
    m_y = y ;
    m_width = width ;
    m_height = height ;
    dcMem.Blit(0, 0, m_width, m_height,
        dc, x, y);
}

void wxOverlayImpl::Clear(wxDC* dc)
{
    dc->DrawBitmap(m_bmpSaved, m_x, m_y);
}

void wxOverlayImpl::Reset()
{
    m_bmpSaved.UnRef();
}

void wxOverlayImpl::BeginDrawing(wxDC* dc)
{
    // Make sure no drawing is done outside of overlay area
    dc->SetClippingRegion(m_x, m_y, m_width, m_height);
}

void wxOverlayImpl::EndDrawing(wxDC* WXUNUSED(dc))
{
}

#ifndef wxHAS_NATIVE_OVERLAY
wxOverlay::Impl* wxOverlay::Create()
{
    return new wxOverlayImpl;
}
#endif

#endif // wxHAS_GENERIC_OVERLAY

wxOverlay::wxOverlay()
{
    m_impl = Create();
#if defined(wxHAS_GENERIC_OVERLAY) && defined(wxHAS_NATIVE_OVERLAY)
    if (m_impl == NULL)
        m_impl = new wxOverlayImpl;
#endif
    m_inDrawing = false;
}
