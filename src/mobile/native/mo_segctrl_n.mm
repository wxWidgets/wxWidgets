/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_segctrl_g.h
// Purpose:     wxMoSegmentedCtrl class
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

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/mobile/native/segctrl.h"

#include "wx/imaglist.h"
#include "wx/dcbuffer.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoSegmentedCtrl, wxMoTabCtrl)

BEGIN_EVENT_TABLE(wxMoSegmentedCtrl, wxMoTabCtrl)
    EVT_PAINT(wxMoSegmentedCtrl::OnPaint)
END_EVENT_TABLE()

wxMoSegmentedCtrl::wxMoSegmentedCtrl()
{
    // FIXME stub
}

bool wxMoSegmentedCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    // FIXME stub
    
    return true;
}

void wxMoSegmentedCtrl::Init()
{
    // FIXME stub
}

// wxEVT_COMMAND_TAB_SEL_CHANGED;
// wxEVT_COMMAND_TAB_SEL_CHANGING;

bool wxMoSegmentedCtrl::SendCommand(wxEventType eventType, int selection)
{
    // FIXME stub
    
    return true;
}

void wxMoSegmentedCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// FIXME stub
}

void wxMoSegmentedCtrl::OnInsertItem(wxMoBarButton* button)
{
    // FIXME stub
}

wxSize wxMoSegmentedCtrl::DoGetBestSize() const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}
