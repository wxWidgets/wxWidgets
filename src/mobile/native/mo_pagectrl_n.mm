/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_pagectrl_n.mm
// Purpose:     wxMoPageCtrl class
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

#include "wx/mobile/native/pagectrl.h"
#include "wx/mobile/native/utils.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoPageCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoPageCtrl, wxControl)
    EVT_PAINT(wxMoPageCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxMoPageCtrl::OnEraseBackground)    
    EVT_SIZE(wxMoPageCtrl::OnSize)
    EVT_MOUSE_EVENTS(wxMoPageCtrl::OnMouseEvent)
END_EVENT_TABLE()

bool wxMoPageCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    // FIXME stub
}

wxMoPageCtrl::~wxMoPageCtrl()
{
}

void wxMoPageCtrl::Init()
{
    // FIXME stub
}

wxSize wxMoPageCtrl::DoGetBestSize() const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}

void wxMoPageCtrl::SetPageCount(int pageCount)
{
    // FIXME stub
}

void wxMoPageCtrl::SetCurrentPage(int page)
{
    // FIXME stub
}

wxSize wxMoPageCtrl::GetSizeForPageCount(int pageCount) const
{
	// FIXME stub
	
    wxSize sz(1, 1);
    return sz;
}

bool wxMoPageCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoPageCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoPageCtrl::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoPageCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

void wxMoPageCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// FIXME stub
}

int wxMoPageCtrl::HitTest(const wxPoint& pos)
{
    // FIXME stub

    return -1;
}

void wxMoPageCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxMoPageCtrl::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

void wxMoPageCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoPageCtrl::SendClickEvent()
{
    // FIXME stub
}
