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
    wxCHECK_RET( win, _T("invalid window") );

    // FIXME: this should be made to work: we need to detect that the window
    //        is not visible and in that case, a) ignore any drawing actions
    //        and b) provide dummy surface that can still be used to get
    //        information (e.g. text extents):
    for ( wxWindow *w = win; w; w = w->GetParent() )
    {
        // painting on hidden TLW when non-TLW windows are shown is OK,
        // DirectFB manages that:
        if ( w->IsTopLevel() )
            break;

        wxASSERT_MSG( w->IsShown(),
                      _T("painting on hidden window not implemented yet") );
    }

    // check if the rectangle covers full window and so is not needed:
    if ( rect && *rect == wxRect(win->GetSize()) )
        rect = NULL;

    // obtain the surface used for painting:
    wxIDirectFBSurfacePtr surface;
    if ( !rect )
    {
        wxCHECK_RET( win->GetSize().x > 0 && win->GetSize().y > 0,
                     _T("window has invalid size") );

        surface = win->GetDfbSurface();
    }
    else
    {
        wxCHECK_RET( !rect || !rect->IsEmpty(), _T("invalid rectangle") );

        DFBRectangle dfbrect = { rect->x, rect->y, rect->width, rect->height };
        surface = win->GetDfbSurface()->GetSubSurface(&dfbrect);
    }

    if ( !surface )
        return;

    Init(surface);
    SetFont(win->GetFont());

    // offset coordinates to account for subsurface's origin coordinates:
    if ( rect )
        SetDeviceOrigin(rect->x, rect->y);
}

//-----------------------------------------------------------------------------
// base class for wxClientDC and wxPaintDC
//-----------------------------------------------------------------------------

wxClientDCBase::wxClientDCBase(wxWindow *win)
{
    wxCHECK_RET( win, _T("invalid window") );

    wxRect rect = win->GetClientRect();
    InitForWin(win, &rect);
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::~wxClientDC()
{
    // flip to surface so that the changes become visible
    wxIDirectFBSurfacePtr surface(GetDirectFBSurface());

    // FIXME: do this only if the surface was modified (as opposed to e.g.
    //        used only to obtain text metrics)
    if ( surface )
        surface->Flip(NULL, DSFLIP_NONE);
}

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)

#warning "wxPaintDC ctor must respect m_updateRegion"

wxPaintDC::~wxPaintDC()
{
    // NB: do *not* flip the surface: wxPaintDC is used with EVT_PAINT and the
    //     surface will be flipped for the entire TLW once all children are
    //     repainted
}
