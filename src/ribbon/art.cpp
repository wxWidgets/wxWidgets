///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art.cpp
// Purpose:     Art providers for ribbon-bar-style interface
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

#if wxUSE_RIBBON

#include "wx/ribbon/bar.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

wxRibbonDefaultArtProvider::wxRibbonDefaultArtProvider()
{
	m_tab_label_colour = wxColour(21, 66, 139);
	m_tab_ctrl_background_brush = wxBrush(wxColour(191, 219, 255));
	m_tab_label_font = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
	m_tab_separation_size = 3;
}

wxRibbonDefaultArtProvider::~wxRibbonDefaultArtProvider()
{
}

wxRibbonArtProvider* wxRibbonDefaultArtProvider::Clone()
{
	wxRibbonDefaultArtProvider *copy = new wxRibbonDefaultArtProvider;

	copy->m_tab_label_colour = m_tab_label_colour;
	copy->m_tab_ctrl_background_brush = m_tab_ctrl_background_brush;
	copy->m_tab_label_font = m_tab_label_font;
	copy->m_flags = m_flags;
	copy->m_tab_separation_size = m_tab_separation_size;

	return copy;
}

void wxRibbonDefaultArtProvider::SetFlags(long flags)
{
	m_flags = flags;
}

int wxRibbonDefaultArtProvider::GetMetric(int id)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_SEPARATION_SIZE: return m_tab_separation_size;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}

	return 0;
}

void wxRibbonDefaultArtProvider::SetMetric(int id, int new_val)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_SEPARATION_SIZE: m_tab_separation_size = new_val;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

void wxRibbonDefaultArtProvider::SetFont(int id, const wxFont& font)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_LABEL_FONT: m_tab_label_font = font;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

wxFont wxRibbonDefaultArtProvider::GetFont(int id)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_LABEL_FONT: return m_tab_label_font;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}

	return wxNullFont;
}

wxColour wxRibbonDefaultArtProvider::GetColour(int id)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR: return m_tab_ctrl_background_brush.GetColour();
		case wxRIBBON_ART_TAB_LABEL_COLOUR: return m_tab_label_colour;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}

	return wxColour();
}

void wxRibbonDefaultArtProvider::SetColour(int id, const wxColor& colour)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR: m_tab_ctrl_background_brush.SetColour(colour);
		case wxRIBBON_ART_TAB_LABEL_COLOUR: m_tab_label_colour = colour;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

void wxRibbonDefaultArtProvider::DrawTabCtrlBackground(
						wxDC& dc,
						wxWindow* wnd,
						const wxRect& rect)
{
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(m_tab_ctrl_background_brush);
	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonDefaultArtProvider::DrawTab(
				 wxDC& dc,
				 wxWindow* wnd,
				 const wxRibbonPageTabInfo& tab)
{
	// Temporary code to make tab regions extremely visible
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(wxColour(255, 0, 0)));
	dc.DrawRectangle(tab.rect.x, tab.rect.y, tab.rect.width, tab.rect.height);

	if(m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
	{
		wxString label = tab.page->GetLabel();
		if(!label.IsEmpty())
		{
			dc.SetFont(m_tab_label_font);
			dc.SetTextForeground(m_tab_label_colour);
			dc.SetBackgroundMode(wxTRANSPARENT);

			int text_width = dc.GetTextExtent(label).GetWidth();
			int width = tab.rect.width - 4;
			int x = tab.rect.x + 2;
			int y = tab.rect.y + 2;

			if(width <= text_width)
			{
				dc.DrawText(label, x, y);
			}
			else
			{
				dc.DrawText(label, x + (width - text_width) / 2, y);
			}
		}
	}
}

void wxRibbonDefaultArtProvider::DrawPageBackground(
						wxDC& dc,
						wxWindow* wnd,
						const wxRect& rect)
{
}

void wxRibbonDefaultArtProvider::GetBarTabWidth(
						wxDC& dc,
						wxWindow* wnd,
						const wxString& label,
						const wxBitmap& bitmap,
						int* ideal,
						int* small_begin_need_separator,
						int* small_must_have_separator,
						int* minimum)
{
	int width = 0;
	int min = 0;
	if(!label.IsEmpty())
	{
		dc.SetFont(m_tab_label_font);
		width += dc.GetTextExtent(label).GetWidth();
		min += 25; // enough for a few chars
		if(width < min)
		{
			min = width;
		}
		if(bitmap.IsOk())
		{
			// gap between label and bitmap
			width += 4;
			min += 2;
		}
	}
	if(bitmap.IsOk())
	{
		width += bitmap.GetWidth();
		min += bitmap.GetWidth();
	}

	if(ideal != NULL)
	{
		*ideal = width + 30;
	}
	if(small_begin_need_separator != NULL)
	{
		*small_begin_need_separator = width + 20;
	}
	if(small_must_have_separator != NULL)
	{
		*small_must_have_separator = width + 10;
	}
	if(minimum != NULL)
	{
		*minimum = min;
	}
}

#endif // wxUSE_RIBBON
