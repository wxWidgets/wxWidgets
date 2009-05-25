///////////////////////////////////////////////////////////////////////////////
// Name:        src/mahogany/bar.cpp
// Purpose:     Top-level component of the ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-23
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mahogany/bar.h"

#if wxUSE_MAHOGANY

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxMahoganyBar, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxMahoganyBarEvent, wxNotifyEvent)

BEGIN_EVENT_TABLE(wxMahoganyBar, wxControl)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxMahoganyBar, wxControl)

wxMahoganyBar::wxMahoganyBar()
{
	m_flags = 0;
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;
}

wxMahoganyBar::wxMahoganyBar(wxWindow* parent,
							 wxWindowID id,
							 const wxPoint& pos,
							 const wxSize& size,
							 long style) : wxControl(parent, id, pos, size, style)
{
	CommonInit(style);
}

wxMahoganyBar::~wxMahoganyBar()
{
}

bool wxMahoganyBar::Create(wxWindow* parent,
				wxWindowID id,
				const wxPoint& pos,
				const wxSize& size,
				long style)
{
	if(!wxControl::Create(parent, id, pos, size, style))
		return false;

	CommonInit(style);

	return true;
}

void wxMahoganyBar::CommonInit(long style)
{
	WX_INIT_CONTROL_CONTAINER();
    SetName(wxT("wxMahoganyBar"));

	m_flags = style;
	m_tab_height = 20; // initial guess
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;
}

#endif // wxUSE_MAHOGANY
