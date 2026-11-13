/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/dcclient.cpp
// Purpose:     wxWindowDCImpl, wxClientDCImpl and wxPaintDCImpl
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/nonownedwnd.h"
#endif

#include "wx/retro68/dcclient.h"
#include "wx/retro68/private/qd.h"

extern GWorldPtr wxRetro68GetScratchPort();

//-----------------------------------------------------------------------------
// wxWindowDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxWindowDCImpl, wxRetro68DCImpl);

wxWindowDCImpl::wxWindowDCImpl(wxDC *owner, wxWindow *win)
              : wxRetro68DCImpl(owner)
{
    InitForWin(win, nullptr);
}

void wxWindowDCImpl::InitForWin(wxWindow *win, const wxRect *rect)
{
    wxCHECK_RET( win, "invalid window" );

    m_window = win;

    // The area of the window covered by this DC, in window coordinates.
    const wxRect rectDC(rect ? *rect : wxRect(win->GetSize()));

    wxNonOwnedWindow* const tlw = win->GetTLW();
    GrafPtr port = tlw ? tlw->GetPort() : nullptr;

    const wxPoint portPos = win->GetPortPosition();

    // Compute the clipping rectangle in the port coordinates: we can only
    // draw on the visible part of the window.
    wxRect clip;
    if ( port && win->IsShownOnScreen() )
    {
        clip = rectDC;
        clip.Intersect(win->GetUncoveredRect());
        clip.Offset(portPos);
    }

    if ( !port )
    {
        // Still create a valid DC which can be used for measuring text, but
        // without drawing anywhere.
        port = reinterpret_cast<GrafPtr>(wxRetro68GetScratchPort());
        m_device = port;
        clip = wxRect();
    }

    InitForPort(port, portPos + rectDC.GetPosition(), clip);

    m_size = rectDC.GetSize();

    SetFont(win->GetFont());
}

//-----------------------------------------------------------------------------
// wxClientDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxClientDCImpl, wxWindowDCImpl);

wxClientDCImpl::wxClientDCImpl(wxDC *owner, wxWindow *win)
              : wxWindowDCImpl(owner)
{
    wxCHECK_RET( win, "invalid window" );

    const wxRect rect = win->GetClientRect();
    InitForWin(win, &rect);
}

//-----------------------------------------------------------------------------
// wxPaintDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPaintDCImpl, wxClientDCImpl);
