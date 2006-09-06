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
    : wxDC(win ? win->GetDfbSurface() : NULL)
{
    InitForWin(win);
}

void wxWindowDC::InitForWin(wxWindow *win)
{
    wxCHECK_RET( win, _T("invalid window") );

    SetFont(win->GetFont());
}

//-----------------------------------------------------------------------------
// base class for wxClientDC and wxPaintDC
//-----------------------------------------------------------------------------

wxClientDCBase::wxClientDCBase(wxWindow *win)
{
    wxCHECK_RET( win, _T("invalid window") );

    wxRect rect = win->GetClientRect();
    DFBRectangle dfbrect = { rect.x, rect.y, rect.width, rect.height };

    wxIDirectFBSurfacePtr subsurf(
            win->GetDfbSurface()->GetSubSurface(&dfbrect));
    if ( !subsurf )
        return;

    Init(subsurf);
    InitForWin(win);

    // offset coordinates to account for subsurface's origin coordinates:
    SetDeviceOrigin(rect.x, rect.y);
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::~wxClientDC()
{
    // flip to surface so that the changes become visible
    wxIDirectFBSurfacePtr surface(GetDirectFBSurface());
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
