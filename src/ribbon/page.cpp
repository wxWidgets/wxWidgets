///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/page.cpp
// Purpose:     Container for ribbon-bar-style interface panels
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/page.h"

#if wxUSE_RIBBON

#include "wx/ribbon/bar.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonPage, wxControl)

BEGIN_EVENT_TABLE(wxRibbonPage, wxControl)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxRibbonPage, wxControl)

wxRibbonPage::wxRibbonPage()
{
	m_scroll_amount = 0;
	m_scroll_buttons_visible = false;
}

wxRibbonPage::wxRibbonPage(wxRibbonBar* parent,
				   wxWindowID id,
				   const wxString& label,
				   const wxBitmap& icon,
				   const wxPoint& pos,
				   const wxSize& size,
				   long style) : wxControl(parent, id, pos, size, style)
{
	CommonInit(label, icon);
}

wxRibbonPage::~wxRibbonPage()
{
}

bool wxRibbonPage::Create(wxRibbonBar* parent,
				wxWindowID id,
				const wxString& label,
				const wxBitmap& icon,
				const wxPoint& pos,
				const wxSize& size,
				long style)
{
	if(!wxControl::Create(parent, id, pos, size, style))
		return false;

	CommonInit(label, icon);

	return true;
}

void wxRibbonPage::CommonInit(const wxString& label, const wxBitmap& icon)
{
	WX_INIT_CONTROL_CONTAINER();
    SetName(label);

	SetLabel(label);
	m_icon = icon;
	m_scroll_amount = 0;
	m_scroll_buttons_visible = false;
}

#endif // wxUSE_RIBBON
