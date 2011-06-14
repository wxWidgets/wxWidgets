/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_frame_n.mm
// Purpose:     Special frame class for use on iPhone or
//              iPhone 'emulator'
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#include "wx/mobile/native/frame.h"
#include "wx/mobile/native/keyboard.h"

IMPLEMENT_DYNAMIC_CLASS( wxMoFrame, wxWindow )

BEGIN_EVENT_TABLE( wxMoFrame, wxWindow )
    EVT_SIZE(wxMoFrame::OnSize)
    EVT_IDLE(wxMoFrame::OnIdle)
END_EVENT_TABLE()

wxMoFrame::wxMoFrame(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

wxMoFrame::~wxMoFrame()
{
}

void wxMoFrame::Init()
{
    // FIXME stub
}

bool wxMoFrame::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    // FIXME stub

    return true;    
}

void wxMoFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

// Do layout
void wxMoFrame::DoLayout()
{
    // FIXME stub
}

// test whether this window makes part of the frame
// (menubar, toolbar and statusbar are excluded from automatic layout)
bool wxMoFrame::IsOneOfBars(const wxWindow *WXUNUSED(win)) const
{
	// FIXME stub
    // TODO
    return false;
}

void wxMoFrame::SendSizeEvent()
{
    // FIXME stub
}

void wxMoFrame::OnIdle(wxIdleEvent& event)
{
    // FIXME stub
}
