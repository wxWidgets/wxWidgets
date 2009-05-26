///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/bar.cpp
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

#include "wx/ribbon/bar.h"

#if wxUSE_RIBBON

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonBar, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxRibbonBarEvent, wxNotifyEvent)

BEGIN_EVENT_TABLE(wxRibbonBar, wxControl)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxRibbonBar, wxControl)

wxRibbonBar::wxRibbonBar()
{
	m_flags = 0;
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;
}

wxRibbonBar::wxRibbonBar(wxWindow* parent,
							 wxWindowID id,
							 const wxPoint& pos,
							 const wxSize& size,
							 long style) : wxControl(parent, id, pos, size, style)
{
	CommonInit(style);
}

wxRibbonBar::~wxRibbonBar()
{
}

bool wxRibbonBar::Create(wxWindow* parent,
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

void wxRibbonBar::CommonInit(long style)
{
	WX_INIT_CONTROL_CONTAINER();
    SetName(wxT("wxRibbonBar"));

	m_flags = style;
	m_tab_height = 20; // initial guess
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;
}

#endif // wxUSE_RIBBON
