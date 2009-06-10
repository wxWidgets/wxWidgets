///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/panel.cpp
// Purpose:     Ribbon-style container for a group of related tools / controls
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

#include "wx/ribbon/art.h"
#include "wx/ribbon/panel.h"
#include "wx/dcbuffer.h"

#if wxUSE_RIBBON

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonPanel, wxRibbonControl)

BEGIN_EVENT_TABLE(wxRibbonPanel, wxRibbonControl)
  EVT_ERASE_BACKGROUND(wxRibbonPanel::OnEraseBackground)
  EVT_PAINT(wxRibbonPanel::OnPaint)
END_EVENT_TABLE()

wxRibbonPanel::wxRibbonPanel()
{
}

wxRibbonPanel::wxRibbonPanel(wxWindow* parent,
				  wxWindowID id,
				  const wxString& label,
				  const wxBitmap& minimised_icon,
				  const wxPoint& pos,
				  const wxSize& size,
				  long style)
	: wxRibbonControl(parent, id, pos, size, style | wxTRANSPARENT_WINDOW)
{
	CommonInit(label, minimised_icon, style | wxTRANSPARENT_WINDOW);
}

wxRibbonPanel::~wxRibbonPanel()
{
}

bool wxRibbonPanel::Create(wxWindow* parent,
				wxWindowID id,
				const wxString& label,
				const wxBitmap& icon,
				const wxPoint& pos,
				const wxSize& size,
				long style)
{
	style |= wxTRANSPARENT_WINDOW;

	if(!wxRibbonControl::Create(parent, id, pos, size, style))
	{
		return false;
	}

	CommonInit(label, icon, style);

	return true;
}

void wxRibbonPanel::SetArtProvider(wxRibbonArtProvider* art)
{
	m_art = art;
}

void wxRibbonPanel::CommonInit(const wxString& label, const wxBitmap& icon, long style)
{
	SetName(label);
	SetLabel(label);

	m_flags = style;
	m_minimised_icon = icon;

	if(m_art == NULL)
	{
		wxRibbonControl* parent = wxDynamicCast(GetParent(), wxRibbonControl);
		if(parent != NULL)
		{
			m_art = parent->GetArtProvider();
		}
	}

	// Temporary
	SetMinSize(wxSize(100, 87));
	SetSize(GetMinSize());

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void wxRibbonPanel::OnEraseBackground(wxEraseEvent& evt)
{
	wxDC* dc = evt.GetDC();
	if(dc != NULL)
	{
		m_art->DrawPanelBackground(*dc, this, GetSize());
	}
	else
	{
		wxClientDC cdc(this);
		m_art->DrawPanelBackground(cdc, this, GetSize());
	}
}

void wxRibbonPanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	// No foreground painting done by the panel itself, but a paint DC
	// must be created anyway.
	wxPaintDC dc(this);
}

#endif // wxUSE_RIBBON
