/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mp_gauge_n.mm
// Purpose:     wxMoGauge class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/native/utils.h"
#include "wx/mobile/native/gauge.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoGauge, wxControl)

BEGIN_EVENT_TABLE(wxMoGauge, wxControl)
    EVT_PAINT(wxMoGauge::OnPaint)
    EVT_ERASE_BACKGROUND(wxMoGauge::OnEraseBackground)    
    EVT_SIZE(wxMoGauge::OnSize)
END_EVENT_TABLE()

bool wxMoGauge::Create(wxWindow *parent,
                       wxWindowID id,
                       int range,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    // FIXME stub

    return true;
}

void wxMoGauge::Init()
{
}

wxSize wxMoGauge::DoGetBestSize() const
{
	// FIXME stub
	
    wxSize sz(100, 20);
    return sz;
}

// ----------------------------------------------------------------------------
// wxMoGauge setters
// ----------------------------------------------------------------------------

void wxMoGauge::SetRange(int r)
{
    // FIXME stub
}

void wxMoGauge::SetValue(int pos)
{
    // FIXME stub
}

bool wxMoGauge::SetForegroundColour(const wxColour& col)
{
    // FIXME stub

    return true;
}

bool wxMoGauge::SetBackgroundColour(const wxColour& col)
{
    // FIXME stub

    return true;
}

void wxMoGauge::SetIndeterminateMode()
{
	// FIXME stub
    // TODO
}

void wxMoGauge::SetDeterminateMode()
{
	// FIXME stub
    // TODO
}

void wxMoGauge::Pulse()
{
	// FIXME stub
    // TODO
}

void wxMoGauge::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// FIXME stub
}

void wxMoGauge::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxMoGauge::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}
