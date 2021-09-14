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
#ifdef wxOVERLAY_NO_EXTERNAL_DC
#include "wx/dcgraph.h"
#else // !wxOVERLAY_NO_EXTERNAL_DC
#include "wx/dcmemory.h"
#endif // wxOVERLAY_NO_EXTERNAL_DC
#include "wx/dcscreen.h"
#include "wx/scrolwin.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxOverlay
// ----------------------------------------------------------------------------

wxOverlay::wxOverlay()
{
    m_impl = wxOverlayImpl::Create();
    m_inDrawing = false;
}

wxOverlay::~wxOverlay()
{
    delete m_impl;
}

bool wxOverlay::IsOk()
{
    return m_impl->IsOk();
}

void wxOverlay::Init( wxDC* dc, int x , int y , int width , int height )
{
    m_impl->Init(dc, x, y, width, height);
}

void wxOverlay::Init( wxWindow* win, bool fullscreen )
{
#ifdef wxOVERLAY_NO_EXTERNAL_DC
    m_impl->Init(win, fullscreen);
#else // !wxOVERLAY_NO_EXTERNAL_DC
    wxUnusedVar(win);
    wxUnusedVar(fullscreen);
#endif // wxOVERLAY_NO_EXTERNAL_DC
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
// wxDCOverlay
// ----------------------------------------------------------------------------

wxDCOverlay::wxDCOverlay(wxOverlay &overlay, wxDC *dc, int x , int y , int width , int height) :
    m_overlay(overlay), m_ownsDC(false)
{
    Init(dc, x, y, width, height);
}

wxDCOverlay::wxDCOverlay(wxOverlay &overlay, wxDC *dc) :
    m_overlay(overlay), m_ownsDC(false)
{
    Init(dc);
}

wxDCOverlay::wxDCOverlay(wxOverlay& overlay, wxWindow* win,
                         int x, int y, int width, int height) :
    m_overlay(overlay), m_ownsDC(true)
{
    Init(win, false /*fullscreen*/, wxRect(x, y, width, height));
}

wxDCOverlay::wxDCOverlay(wxOverlay& overlay, wxWindow* win, bool fullscreen) :
    m_overlay(overlay), m_ownsDC(true)
{
    Init(win, fullscreen, wxRect());
}

wxDCOverlay::~wxDCOverlay()
{
    m_overlay.EndDrawing(m_dc);

    if ( m_ownsDC && m_dc )
        delete m_dc;
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

void wxDCOverlay::Init(wxWindow* win, bool fullscreen, const wxRect& rect)
{
    wxCHECK_RET( win, wxS("Invalid window pointer") );

#ifdef wxOVERLAY_NO_EXTERNAL_DC
    if ( !m_overlay.IsOk() )
    {
        m_overlay.Init(win, fullscreen);
    }

    const bool isNative = m_overlay.GetImpl()->IsNative();

    if ( m_overlay.IsOk() && isNative )
    {
        wxBitmap& bitmap = m_overlay.GetImpl()->GetBitmap();
        m_memDC.SelectObject(bitmap);

#if wxUSE_GRAPHICS_CONTEXT
        m_dc = new wxGCDC;
        m_dc->SetGraphicsContext(wxGraphicsContext::Create(m_memDC));
#else // !wxUSE_GRAPHICS_CONTEXT
        m_dc = &m_memDC;
        m_ownsDC = false;
#endif // wxUSE_GRAPHICS_CONTEXT
        m_overlay.BeginDrawing(m_dc);

        if ( !rect.IsEmpty() )
            SetUpdateRectangle(rect);
    }
    else
#endif // !wxOVERLAY_NO_EXTERNAL_DC
    {
        wxDC* dc;

        if ( fullscreen )
            dc = new wxScreenDC;
        else
            dc = new wxClientDC(win);

        wxScrolledWindow* const sw = wxDynamicCast(win, wxScrolledWindow);
        if ( sw )
            sw->PrepareDC(*dc);

        if ( rect.IsEmpty() )
            Init(dc);
        else
            Init(dc, rect.x, rect.y, rect.width, rect.height);
    }
}

void wxDCOverlay::Init(wxDC* dc)
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

void wxDCOverlay::Clear()
{
    m_overlay.Clear(m_dc);
}

void wxDCOverlay::SetUpdateRectangle(const wxRect& rect)
{
    m_overlay.GetImpl()->SetUpdateRectangle(rect);
}

// ----------------------------------------------------------------------------
// generic implementation of wxOverlayImpl
// ----------------------------------------------------------------------------

class wxOverlayGenericImpl : public wxOverlayImpl
{
public:
    wxOverlayGenericImpl() : wxOverlayImpl()
    {
        m_window = NULL;
    }

    virtual bool IsNative() const wxOVERRIDE { return false; }

    virtual bool IsOk() wxOVERRIDE
    {
        return GetBitmap().IsOk();
    }

    virtual void InitFromDC(wxDC* dc, int x, int y, int width, int height) wxOVERRIDE
    {
        m_window = dc->GetWindow();
        m_rect   = wxRect(x, y, width, height);

        wxMemoryDC dcMem ;
        wxBitmap& bmpSaved = GetBitmap();
        bmpSaved.Create( width, height );
        dcMem.SelectObject( bmpSaved );
        dcMem.Blit(0, 0, width, height, dc, x, y);
        dcMem.SelectObject( wxNullBitmap );
    }

    virtual void InitFromWindow(wxWindow* WXUNUSED(win), bool WXUNUSED(fullscreen)) wxOVERRIDE
    {
        // Not constructible from wxWindow.
    }

    virtual void Clear(wxDC* dc) wxOVERRIDE
    {
        wxMemoryDC dcMem ;
        wxBitmap& bmpSaved = GetBitmap();
        dcMem.SelectObject( bmpSaved );
        dc->Blit( m_rect.x, m_rect.y, m_rect.width, m_rect.height, &dcMem, 0, 0 );
        dcMem.SelectObject( wxNullBitmap );
    }

    virtual void Reset() wxOVERRIDE
    {
        GetBitmap() = wxBitmap();
    }

private:
    wxRect    m_rect;
    wxWindow* m_window;
};

void wxOverlay::UseGeneric()
{
#ifdef wxHAS_NATIVE_OVERLAY
    wxASSERT_MSG( !IsOk(), "should only be called for uninitialized overlay" );
    delete m_impl;
    m_impl = new wxOverlayGenericImpl();
#endif // wxHAS_NATIVE_OVERLAY
}

#ifndef wxHAS_NATIVE_OVERLAY
wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayGenericImpl(); }
#endif
