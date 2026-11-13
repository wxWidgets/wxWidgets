/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/dcscreen.cpp
// Purpose:     wxScreenDCImpl implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"

#include "wx/retro68/dcscreen.h"
#include "wx/retro68/private/qd.h"

// ----------------------------------------------------------------------------
// wxScreenDCImpl
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxRetro68DCImpl);

wxScreenDCImpl::wxScreenDCImpl(wxScreenDC *owner)
              : wxRetro68DCImpl(owner)
{
    // Use the Window Manager port which covers the entire screen.
    GrafPtr port;
    if ( wxMacHasColorQD() )
    {
        CGrafPtr cport;
        GetCWMgrPort(&cport);
        port = reinterpret_cast<GrafPtr>(cport);
    }
    else
    {
        GetWMgrPort(&port);
    }

    const wxRect screen = wxRectFromMac(wxMacGetScreenBounds());

    InitForPort(port, wxPoint(), screen);

    m_size = screen.GetSize();
}
