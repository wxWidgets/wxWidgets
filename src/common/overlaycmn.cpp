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
#ifdef wxOVERLAY_USE_INTERNAL_BUFFER
#include "wx/dcgraph.h"
#else // !wxOVERLAY_USE_INTERNAL_BUFFER
#include "wx/dcmemory.h"
#endif // wxOVERLAY_USE_INTERNAL_BUFFER
#include "wx/dcscreen.h"
#include "wx/scrolwin.h"

// wxGCDC won't be used under wxMSW for two reasons:
// 1) Blitting from wxGCDC is not implemented under wxMSW
// 2) wxGCDC is too slow compared to using wxMemoryDC directly

#ifndef __WXMSW__
    #if wxUSE_GRAPHICS_CONTEXT
        #define wxHAS_OVERLAY_USING_GCDC 1
    #endif // wxUSE_GRAPHICS_CONTEXT
#endif //__WXMSW__

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

bool wxOverlay::IsNative() const
{
    return m_impl->IsNative();
}

bool wxOverlay::IsOk()
{
    return m_impl->IsOk();
}

void wxOverlay::InitFromDC( wxDC* dc, int x , int y , int width , int height )
{
    m_impl->Init(dc, x, y, width, height);
}

void wxOverlay::InitFromWindow( wxWindow* win, wxOverlay::Target target )
{
#ifdef wxOVERLAY_USE_INTERNAL_BUFFER
    m_impl->Init(win, target);
#else // !wxOVERLAY_USE_INTERNAL_BUFFER
    wxUnusedVar(win);
    wxUnusedVar(target);
#endif // wxOVERLAY_USE_INTERNAL_BUFFER
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
    // Notice that this function can be safely called anytime when using generic overlay
    // because BeginDrawing()/EndDrawing() are trivial there.

    wxASSERT_MSG(!m_impl->IsNative() || !m_inDrawing,
                 "cannot reset overlay during drawing");

    m_impl->Reset();
}

// ----------------------------------------------------------------------------
// wxDCOverlay
// ----------------------------------------------------------------------------

wxDCOverlay::wxDCOverlay(wxOverlay &overlay, wxDC *dc, int x , int y , int width , int height) :
    m_overlay(overlay), m_ownsDC(false)
{
    // Initializing the wxOverlay from wxDC is not currently supported under wxMSW and wxGTK3
    // and to preserve compatibility with the old wx versions we fallback to the generic impl.
#if !defined(__WXOSX__) && !defined(__WXDFB__)
    m_overlay.UseGeneric();
#endif

    InitFromDC(dc, x, y, width, height);
}

wxDCOverlay::wxDCOverlay(wxOverlay &overlay, wxDC *dc) :
    m_overlay(overlay), m_ownsDC(false)
{
    // Initializing the wxOverlay from wxDC is not currently supported under wxMSW and wxGTK3
    // and to preserve compatibility with the old wx versions we fallback to the generic impl.
#if !defined(__WXOSX__) && !defined(__WXDFB__)
    m_overlay.UseGeneric();
#endif

    InitFromDC(dc);
}

wxDCOverlay::wxDCOverlay(wxOverlay& overlay, wxWindow* win,
                         int x, int y, int width, int height) :
    m_overlay(overlay), m_dc(NULL), m_ownsDC(true)
{
    InitFromWindow(win, wxOverlay::Overlay_Window, wxRect(x, y, width, height));
}

wxDCOverlay::wxDCOverlay(wxOverlay& overlay, wxWindow* win, wxOverlay::Target target) :
    m_overlay(overlay), m_dc(NULL), m_ownsDC(true)
{
    InitFromWindow(win, target, wxRect());
}

wxDCOverlay::~wxDCOverlay()
{
    m_overlay.EndDrawing(m_dc);

    if ( m_ownsDC && m_dc )
        delete m_dc;
}

void wxDCOverlay::InitFromDC(wxDC *dc, int x , int y , int width , int height )
{
    m_dc = dc ;
    if ( !m_overlay.IsOk() )
    {
        m_overlay.InitFromDC(dc,x,y,width,height);
    }
    m_overlay.BeginDrawing(dc);
}

void wxDCOverlay::InitFromWindow(wxWindow* win, wxOverlay::Target target, const wxRect& rect)
{
    wxCHECK_RET( win, wxS("Invalid window pointer") );

#ifdef wxOVERLAY_USE_INTERNAL_BUFFER
    if ( !m_overlay.IsOk() )
    {
        SetUpdateRectangle(rect);
        m_overlay.InitFromWindow(win, target);
    }

    if ( m_overlay.IsOk() && m_overlay.GetImpl()->IsNative() )
    {
        wxBitmap& bitmap = m_overlay.GetImpl()->GetBitmap();
        m_memDC.SelectObject(bitmap);

#ifdef wxHAS_OVERLAY_USING_GCDC
        m_dc = new wxGCDC;
        m_dc->SetGraphicsContext(wxGraphicsContext::Create(m_memDC));
#else // !wxHAS_OVERLAY_USING_GCDC
        m_dc = &m_memDC;
        m_ownsDC = false;
#endif // wxHAS_OVERLAY_USING_GCDC
        m_overlay.BeginDrawing(m_dc);

        if ( !rect.IsEmpty() )
            SetUpdateRectangle(rect);
    }
    else
#endif // !wxOVERLAY_USE_INTERNAL_BUFFER
    {
        wxDC* dc;

        if ( target == wxOverlay::Overlay_Screen )
            dc = new wxScreenDC;
        else
            dc = new wxClientDC(win);

        wxScrolledWindow* const sw = wxDynamicCast(win, wxScrolledWindow);
        if ( sw )
            sw->PrepareDC(*dc);

        if ( rect.IsEmpty() )
            InitFromDC(dc);
        else
            InitFromDC(dc, rect.x, rect.y, rect.width, rect.height);
    }
}

void wxDCOverlay::InitFromDC(wxDC* dc)
{
    const wxSize size(dc->GetSize());

    const wxCoord logicalLeft = dc->DeviceToLogicalX(0);
    const wxCoord logicalTop = dc->DeviceToLogicalY(0);
    const wxCoord logicalRight = dc->DeviceToLogicalX(size.GetWidth());
    const wxCoord logicalBottom = dc->DeviceToLogicalY(size.GetHeight());

    InitFromDC(dc,
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

    virtual void InitFromWindow(wxWindow* WXUNUSED(win), wxOverlay::Target WXUNUSED(target)) wxOVERRIDE
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

    if ( !m_impl->IsNative() || !m_impl->IsGenericSupported() )
    {
        // Only native overlay can be used (Wayland for ex.)
        return;
    }

    delete m_impl;
    m_impl = new wxOverlayGenericImpl();
#endif // wxHAS_NATIVE_OVERLAY
}

#ifndef wxHAS_NATIVE_OVERLAY
wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayGenericImpl(); }
#endif
