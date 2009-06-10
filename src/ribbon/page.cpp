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

#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonPage, wxRibbonControl)

BEGIN_EVENT_TABLE(wxRibbonPage, wxRibbonControl)
  EVT_ERASE_BACKGROUND(wxRibbonPage::OnEraseBackground)
  EVT_PAINT(wxRibbonPage::OnPaint)
END_EVENT_TABLE()

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
				   long style) : wxRibbonControl(parent, id, pos, size, style)
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
	if(!wxRibbonControl::Create(parent, id, pos, size, style))
		return false;

	CommonInit(label, icon);

	return true;
}

void wxRibbonPage::CommonInit(const wxString& label, const wxBitmap& icon)
{
    SetName(label);

	SetLabel(label);
	m_icon = icon;
	m_scroll_amount = 0;
	m_scroll_buttons_visible = false;

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	wxDynamicCast(GetParent(), wxRibbonBar)->AddPage(this);
}

void wxRibbonPage::SetArtProvider(wxRibbonArtProvider* art)
{
	m_art = art;
	for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
		wxRibbonControl* ribbon_child = wxDynamicCast(child, wxRibbonControl);
		if(ribbon_child)
		{
			ribbon_child->SetArtProvider(art);
		}
	}
}

void wxRibbonPage::OnEraseBackground(wxEraseEvent& evt)
{
	wxDC* dc = evt.GetDC();
	if(dc != NULL)
	{
		m_art->DrawPageBackground(*dc, this, GetClientSize());
	}
	else
	{
		wxClientDC cdc(this);
		m_art->DrawPageBackground(cdc, this, GetClientSize());
	}
}

void wxRibbonPage::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	// No foreground painting done by the page itself, but a paint DC
	// must be created anyway.
	wxPaintDC dc(this);
}

wxOrientation wxRibbonPage::GetMajorAxis() const
{
	if(m_art && (m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL))
	{
		return wxVERTICAL;
	}
	else
	{
		return wxHORIZONTAL;
	}
}

bool wxRibbonPage::Layout()
{
	wxPoint origin(m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE), m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE));

	if(GetMajorAxis() == wxHORIZONTAL)
	{
		int gap = m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE);
		int height = GetSize().GetHeight() - origin.y - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			int w, h;
			child->GetSize(&w, &h);
			child->SetSize(origin.x, origin.y, w, height);
			origin.x += w + gap;
		}
	}
	else
	{
		int gap = m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE);
		int width = GetSize().GetWidth() - origin.x - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			int w, h;
			child->GetSize(&w, &h);
			child->SetSize(origin.x, origin.y, width, h);
			origin.y += h + gap;
		}
	}

	return true;
}

wxSize wxRibbonPage::GetMinSize() const
{
	wxSize min(wxDefaultCoord, wxDefaultCoord);

	for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
		wxSize child_min(child->GetMinSize());

		min.x = wxMax(min.x, child_min.x);
		min.y = wxMax(min.y, child_min.y);
	}

	if(GetMajorAxis() == wxHORIZONTAL)
	{
		min.x = wxDefaultCoord;
		if(min.y != wxDefaultCoord)
		{
			min.y += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
		}
	}
	else
	{
		if(min.x != wxDefaultCoord)
		{
			min.x += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
		}
		min.y = wxDefaultCoord;
	}

	return min;
}

wxSize wxRibbonPage::DoGetBestSize() const
{
	wxSize best(0, 0);
	size_t count = 0;

	if(GetMajorAxis() == wxHORIZONTAL)
	{
		best.y = wxDefaultCoord;

		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			wxSize child_best(child->GetBestSize());

			if(child_best.x != wxDefaultCoord)
			{
				best.IncBy(child_best.x, 0);
			}
			best.y = wxMax(best.y, child_best.y);

			++count;
		}

		if(count > 1)
		{
			best.IncBy((count - 1) * m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE), 0);
		}
	}
	else
	{
		best.x = wxDefaultCoord;

		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			wxSize child_best(child->GetBestSize());

			best.x = wxMax(best.x, child_best.x);
			if(child_best.y != wxDefaultCoord)
			{
				best.IncBy(0, child_best.y);
			}

			++count;
		}

		if(count > 1)
		{
			best.IncBy(0, (count - 1) * m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE));
		}
	}

	if(best.x != wxDefaultCoord)
	{
		best.x += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
	}
	if(best.y != wxDefaultCoord)
	{
		best.y += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
	}
	return best;
}

#endif // wxUSE_RIBBON
