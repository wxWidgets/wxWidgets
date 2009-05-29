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
	// TODO: 'Intelligent' colour picking rather than hardcoded defaults
	m_tab_label_colour = wxColour(21, 66, 139);
	m_tab_separator_colour = wxColour(183, 212, 249);
	m_tab_separator_gradient_colour = wxColour(116, 153, 203);
	m_tab_active_background_colour = wxColour(242, 248, 244);
	m_tab_active_background_gradient_colour = wxColour(223, 233, 245);
	m_tab_hover_background_colour = wxColour(212, 228, 249);
	m_tab_hover_background_gradient_colour = wxColour(225, 210, 165);
	m_tab_hover_background_top_colour = wxColour(196, 221, 254);
	m_tab_hover_background_top_gradient_colour = wxColour(221, 235, 254);
	m_page_background_colour = wxColour(199, 216, 237);
	m_page_background_gradient_colour = wxColour(227, 244, 255);
	m_page_background_top_colour = wxColour(223, 233, 245);
	m_page_background_top_gradient_colour = wxColour(209, 223, 240);
	m_tab_ctrl_background_brush = wxBrush(wxColour(191, 219, 255));
	m_tab_label_font = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
	m_tab_border_pen = wxPen(wxColour(153, 187, 232));
	m_page_border_pen = wxPen(wxColour(141, 178, 227));
	m_tab_separation_size = 3;
}

wxRibbonDefaultArtProvider::~wxRibbonDefaultArtProvider()
{
}

wxRibbonArtProvider* wxRibbonDefaultArtProvider::Clone()
{
	wxRibbonDefaultArtProvider *copy = new wxRibbonDefaultArtProvider;

	copy->m_tab_label_colour = m_tab_label_colour;
	copy->m_tab_separator_colour = m_tab_separator_colour;
	copy->m_tab_separator_gradient_colour = m_tab_separator_gradient_colour;
	copy->m_tab_active_background_colour = m_tab_hover_background_colour;
	copy->m_tab_active_background_gradient_colour = m_tab_hover_background_gradient_colour;
	copy->m_tab_hover_background_colour = m_tab_hover_background_colour;
	copy->m_tab_hover_background_gradient_colour = m_tab_hover_background_gradient_colour;
	copy->m_tab_hover_background_top_colour = m_tab_hover_background_top_colour;
	copy->m_tab_hover_background_top_gradient_colour = m_tab_hover_background_top_gradient_colour;
	copy->m_page_background_colour = m_page_background_colour;
	copy->m_page_background_gradient_colour = m_page_background_gradient_colour;
	copy->m_page_background_top_colour = m_page_background_top_colour;
	copy->m_page_background_top_gradient_colour = m_page_background_top_gradient_colour;
	copy->m_tab_ctrl_background_brush = m_tab_ctrl_background_brush;
	copy->m_tab_label_font = m_tab_label_font;
	copy->m_page_border_pen = m_page_border_pen;
	copy->m_tab_border_pen = m_tab_border_pen;
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
		case wxRIBBON_ART_TAB_SEPARATOR_COLOUR: return m_tab_separator_colour;
		case wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR: return m_tab_separator_gradient_colour;
		case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR: return m_tab_active_background_colour;
		case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR: return m_tab_active_background_gradient_colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR: return m_tab_hover_background_top_colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR: return m_tab_hover_background_top_gradient_colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR: return m_tab_hover_background_colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR: return m_tab_hover_background_gradient_colour;
		case wxRIBBON_ART_TAB_BORDER_COLOUR: return m_tab_border_pen.GetColour();
		case wxRIBBON_ART_PAGE_BORDER_COLOUR: return m_page_border_pen.GetColour();
		case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR: return m_page_background_top_colour;
		case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR: return m_page_background_top_gradient_colour;
		case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR: return  m_page_background_colour;
		case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR: return m_page_background_gradient_colour;
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
		case wxRIBBON_ART_TAB_SEPARATOR_COLOUR: m_tab_separator_colour = colour;
		case wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR: m_tab_separator_gradient_colour = colour;
		case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR: m_tab_active_background_colour = colour;
		case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR: m_tab_active_background_gradient_colour = colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR: m_tab_hover_background_top_colour = colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR: m_tab_hover_background_top_gradient_colour = colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR: m_tab_hover_background_colour = colour;
		case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR: m_tab_hover_background_gradient_colour = colour;
		case wxRIBBON_ART_TAB_BORDER_COLOUR: m_tab_border_pen.SetColour(colour);
		case wxRIBBON_ART_PAGE_BORDER_COLOUR: m_page_border_pen.SetColour(colour);
		case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR: m_page_background_top_colour = colour;
		case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR: m_page_background_top_gradient_colour = colour;
		case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:  m_page_background_colour = colour;
		case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR: m_page_background_gradient_colour = colour;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

// Originally from wxAUI's dockart.cpp
static void DrawVerticalGradientRectangle(wxDC& dc,
                                  const wxRect& rect,
                                  const wxColour& start_colour,
                                  const wxColour& end_colour)
{
    int rd, gd, bd, high = 0;
    rd = end_colour.Red() - start_colour.Red();
    gd = end_colour.Green() - start_colour.Green();
    bd = end_colour.Blue() - start_colour.Blue();

	if(rd == 0 && gd == 0 && bd == 0)
	{
		wxBrush b(start_colour);
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(b);
		dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
		return;
	}

    high = rect.GetHeight() - 1;

    for (int i = 0; i <= high; ++i)
    {
        int r,g,b;

        r = start_colour.Red() + (high <= 0 ? 0 : (((i*rd*100)/high)/100));
        g = start_colour.Green() + (high <= 0 ? 0 : (((i*gd*100)/high)/100));
        b = start_colour.Blue() + (high <= 0 ? 0 : (((i*bd*100)/high)/100));

        wxPen p(wxColour((unsigned char)r,
                        (unsigned char)g,
                        (unsigned char)b));
        dc.SetPen(p);

        dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
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

	dc.SetPen(m_page_border_pen);
	dc.DrawLine(rect.x + 3, rect.y + rect.height - 1, rect.x + rect.width - 3, rect.y + rect.height - 1);
}

void wxRibbonDefaultArtProvider::DrawTab(
				 wxDC& dc,
				 wxWindow* wnd,
				 const wxRibbonPageTabInfo& tab)
{
	if(tab.active || tab.hovered)
	{
		if(tab.active)
		{
			wxRect background(tab.rect);

			background.x += 2;
			background.y += 2;
			background.width -= 4;
			background.height -= 2;

			DrawVerticalGradientRectangle(dc, background, m_tab_active_background_colour, m_tab_active_background_gradient_colour);

			// TODO: active and hovered
		}
		else if(tab.hovered)
		{
			wxRect background(tab.rect);

			background.x += 2;
			background.y += 2;
			background.width -= 4;
			background.height -= 3;
			int h = background.height;
			background.height /= 2;
			DrawVerticalGradientRectangle(dc, background, m_tab_hover_background_top_colour, m_tab_hover_background_top_gradient_colour);

			background.y += background.height;
			background.height = h - background.height;
			DrawVerticalGradientRectangle(dc, background, m_tab_hover_background_colour, m_tab_hover_background_gradient_colour);
		}

		wxPoint border_points[6];
		border_points[0] = wxPoint(1, tab.rect.height - 2);
		border_points[1] = wxPoint(1, 3);
		border_points[2] = wxPoint(3, 1);
		border_points[3] = wxPoint(tab.rect.width - 4, 1);
		border_points[4] = wxPoint(tab.rect.width - 2, 3);
		border_points[5] = wxPoint(tab.rect.width - 2, tab.rect.height - 1);

		dc.SetPen(m_tab_border_pen);
		dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points, tab.rect.x, tab.rect.y);

		if(tab.active)
		{
			// Give the tab a curved outward border at the bottom
			dc.DrawPoint(tab.rect.x, tab.rect.y + tab.rect.height - 2);
			dc.DrawPoint(tab.rect.x + tab.rect.width - 1, tab.rect.y + tab.rect.height - 2);

			wxPen p(m_tab_active_background_gradient_colour);
			dc.SetPen(p);

			// Technically the first two points are the wrong colour, but they're near enough
			dc.DrawPoint(tab.rect.x + 1, tab.rect.y + tab.rect.height - 2);
			dc.DrawPoint(tab.rect.x + tab.rect.width - 2, tab.rect.y + tab.rect.height - 2);
			dc.DrawPoint(tab.rect.x + 1, tab.rect.y + tab.rect.height - 1);
			dc.DrawPoint(tab.rect.x, tab.rect.y + tab.rect.height - 1);
			dc.DrawPoint(tab.rect.x + tab.rect.width - 2, tab.rect.y + tab.rect.height - 1);
			dc.DrawPoint(tab.rect.x + tab.rect.width - 1, tab.rect.y + tab.rect.height - 1);
		}
	}

	if(m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
	{
		wxString label = tab.page->GetLabel();
		if(!label.IsEmpty())
		{
			dc.SetFont(m_tab_label_font);
			dc.SetTextForeground(m_tab_label_colour);
			dc.SetBackgroundMode(wxTRANSPARENT);

			int text_width = dc.GetTextExtent(label).GetWidth();
			int width = tab.rect.width - 5;
			int x = tab.rect.x + 3;
			int y = tab.rect.y + 2;

			if(width <= text_width)
			{
				dc.SetClippingRegion(x, tab.rect.y, width, tab.rect.height);
				dc.DrawText(label, x, y);
			}
			else
			{
				dc.DrawText(label, x + (width - text_width) / 2 + 1, y);
			}
		}
	}
}

void wxRibbonDefaultArtProvider::DrawTabSeparator(
						wxDC& dc,
						wxWindow* wnd,
						const wxRect& rect,
						double visibility)
{
	if(visibility <= 0.0)
	{
		return;
	}
	if(visibility > 1.0)
	{
		visibility = 1.0;
	}
	wxCoord x = rect.x + rect.width / 2;
	double h = (double)(rect.height - 1);

	double r1 = m_tab_ctrl_background_brush.GetColour().Red() * (1.0 - visibility) + 0.5;
	double g1 = m_tab_ctrl_background_brush.GetColour().Green() * (1.0 - visibility) + 0.5;
	double b1 = m_tab_ctrl_background_brush.GetColour().Blue() * (1.0 - visibility) + 0.5;
	double r2 = m_tab_separator_colour.Red();
	double g2 = m_tab_separator_colour.Green();
	double b2 = m_tab_separator_colour.Blue();
	double r3 = m_tab_separator_gradient_colour.Red();
	double g3 = m_tab_separator_gradient_colour.Green();
	double b3 = m_tab_separator_gradient_colour.Blue();

	for(int i = 0; i < rect.height - 1; ++i)
	{
		double p = ((double)i)/h;

		double r = (p * r3 + (1.0 - p) * r2) * visibility + r1;
		double g = (p * g3 + (1.0 - p) * g2) * visibility + g1;
		double b = (p * b3 + (1.0 - p) * b2) * visibility + b1;

		wxPen P(wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b));
		dc.SetPen(P);
		dc.DrawPoint(x, rect.y + i);
	}
}

void wxRibbonDefaultArtProvider::DrawPageBackground(
						wxDC& dc,
						wxWindow* wnd,
						const wxRect& rect)
{
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(m_tab_ctrl_background_brush);

	{
		wxRect edge(rect);

		edge.width = 2;
	    dc.DrawRectangle(edge.x, edge.y, edge.width, edge.height);

		edge.x += rect.width - 2;
		dc.DrawRectangle(edge.x, edge.y, edge.width, edge.height);

		edge = rect;
		edge.height = 2;
		edge.y += (rect.height - edge.height);
		dc.DrawRectangle(edge.x, edge.y, edge.width, edge.height);
	}

	{
		wxRect background(rect);
		background.x += 2;
		background.width -= 4;
		background.height -= 2;

		background.height /= 5;
		DrawVerticalGradientRectangle(dc, background, m_page_background_top_colour, m_page_background_top_gradient_colour);

		background.y += background.height;
		background.height = rect.height - 2 - background.height;
		DrawVerticalGradientRectangle(dc, background, m_page_background_colour, m_page_background_gradient_colour);
	}

	{
		wxPoint border_points[8];
		border_points[0] = wxPoint(3, -1);
		border_points[1] = wxPoint(1, 1);
		border_points[2] = wxPoint(1, rect.height - 4);
		border_points[3] = wxPoint(3, rect.height - 2);
		border_points[4] = wxPoint(rect.width - 4, rect.height - 2);
		border_points[5] = wxPoint(rect.width - 2, rect.height - 4);
		border_points[6] = wxPoint(rect.width - 2, 1);
		border_points[7] = wxPoint(rect.width - 3, -1);

		dc.SetPen(m_page_border_pen);
		dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points, rect.x, rect.y);
	}
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
		min += wxMin(25, width); // enough for a few chars
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
