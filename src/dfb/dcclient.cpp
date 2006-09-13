/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/dcclient.cpp
// Purpose:     wxWindowDC, wxClientDC and wxPaintDC
// Author:      Vaclav Slavik
// Created:     2006-08-10
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
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

#include "wx/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/dfb/private.h"

#define TRACE_PAINT  _T("paint")

// ===========================================================================
// implementation
// ===========================================================================

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC(wxWindow *win)
{
    InitForWin(win, NULL);
}

void wxWindowDC::InitForWin(wxWindow *win, const wxRect *rect)
{
    m_win = win;

    wxCHECK_RET( win, _T("invalid window") );

    // obtain the surface used for painting:
    wxPoint origin;
    wxIDirectFBSurfacePtr surface;

    if ( !win->IsVisible() )
    {
        // we're painting on invisible window: the changes won't have any
        // effect, as the window will be repainted anyhow when it is shown, but
        // we still need a valid DC so that e.g. text extents can be measured,
        // so let's create a dummy surface that has the same format as the real
        // one would have and let the code paint on it:
        wxLogTrace(TRACE_PAINT, _T("%p ('%s'): creating dummy DC surface"),
                   win, win->GetName().c_str());
        wxSize size(rect ? rect->GetSize() : win->GetSize());
        surface = win->GetDfbSurface()->CreateCompatible(size);
    }
    else
    {
        wxRect rectOrig(rect ? *rect : wxRect(win->GetSize()));

        // compute painting rectangle after clipping if we're in PaintWindow
        // code, otherwise paint on the entire window:
        wxRect r(rectOrig);
        if ( win->GetTLW()->IsPainting() )
            r.Intersect(win->GetUpdateRegion().AsRect());

        wxCHECK_RET( !r.IsEmpty(), _T("invalid painting rectangle") );

        // if the DC was clipped thanks to rectPaint, we must adjust the origin
        // accordingly; but we do *not* adjust for 'rect', because
        // rect.GetPosition() has coordinates (0,0) in the DC:
        origin.x = rectOrig.x - r.x;
        origin.y = rectOrig.y - r.y;

        wxLogTrace(TRACE_PAINT,
                   _T("%p ('%s'): creating DC for area [%i,%i,%i,%i], clipped to [%i,%i,%i,%i], origin [%i,%i]"),
                   win, win->GetName().c_str(),
                   rectOrig.x, rectOrig.y, rectOrig.GetRight(), rectOrig.GetBottom(),
                   r.x, r.y, r.GetRight(), r.GetBottom(),
                   origin.x, origin.y);

        DFBRectangle dfbrect = { r.x, r.y, r.width, r.height };
        surface = win->GetDfbSurface()->GetSubSurface(&dfbrect);
    }

    if ( !surface )
        return;

    Init(surface);
    SetFont(win->GetFont());

    // offset coordinates to account for subsurface's origin coordinates:
    SetDeviceOrigin(origin.x, origin.y);
}

wxWindowDC::~wxWindowDC()
{
    wxIDirectFBSurfacePtr surface(GetDirectFBSurface());
    if ( !surface || !m_win )
        return;

    // painting on hidden window has no effect on TLW's surface, don't
    // waste time flipping the dummy surface:
    if ( !m_win->IsVisible() )
        return;

    // if no painting was done on the DC, we don't have to flip the surface:
    if ( !m_isBBoxValid )
        return;

    if ( !m_win->GetTLW()->IsPainting() )
    {
        // FIXME: flip only modified parts of the surface
        surface->FlipToFront();
    }
    // else: don't flip the surface, wxTLW will do it when it finishes
    //       painting of its invalidated areas
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC(wxWindow *win)
{
    wxCHECK_RET( win, _T("invalid window") );

    wxRect rect = win->GetClientRect();
    InitForWin(win, &rect);
}

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)
