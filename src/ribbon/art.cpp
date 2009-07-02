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

wxRibbonArtProvider::wxRibbonArtProvider() {}
wxRibbonArtProvider::~wxRibbonArtProvider() {}

wxRibbonMSWArtProvider::wxRibbonMSWArtProvider()
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
    m_panel_label_colour = wxColour(62, 106, 170);
    m_page_background_colour = wxColour(199, 216, 237);
    m_page_background_gradient_colour = wxColour(227, 244, 255);
    m_page_background_top_colour = wxColour(223, 233, 245);
    m_page_background_top_gradient_colour = wxColour(209, 223, 240);
    m_tab_ctrl_background_brush = wxBrush(wxColour(191, 219, 255));
    m_panel_label_background_brush = wxBrush(wxColour(193, 216, 241));
    m_tab_label_font = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
    m_panel_label_font = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
    m_tab_border_pen = wxPen(wxColour(153, 187, 232));
    m_panel_border_pen = wxPen(wxColour(197, 210, 223));
    m_panel_border_gradient_pen = wxPen(wxColour(158, 191, 219));
    m_page_border_pen = wxPen(wxColour(141, 178, 227));
    m_cached_tab_separator_visibility = -10.0; // valid visibilities are in range [0, 1]
    m_tab_separation_size = 3;
    m_page_border_left = 2;
    m_page_border_top = 1;
    m_page_border_right = 2;
    m_page_border_bottom = 3;
    m_panel_x_separation_size = 1;
    m_panel_y_separation_size = 1;
}

wxRibbonMSWArtProvider::~wxRibbonMSWArtProvider()
{
}

wxRibbonArtProvider* wxRibbonMSWArtProvider::Clone()
{
    wxRibbonMSWArtProvider *copy = new wxRibbonMSWArtProvider;

    copy->m_tab_label_colour = m_tab_label_colour;
    copy->m_tab_separator_colour = m_tab_separator_colour;
    copy->m_tab_separator_gradient_colour = m_tab_separator_gradient_colour;
    copy->m_tab_active_background_colour = m_tab_hover_background_colour;
    copy->m_tab_active_background_gradient_colour = m_tab_hover_background_gradient_colour;
    copy->m_tab_hover_background_colour = m_tab_hover_background_colour;
    copy->m_tab_hover_background_gradient_colour = m_tab_hover_background_gradient_colour;
    copy->m_tab_hover_background_top_colour = m_tab_hover_background_top_colour;
    copy->m_tab_hover_background_top_gradient_colour = m_tab_hover_background_top_gradient_colour;
    copy->m_panel_label_colour = m_panel_label_colour;
    copy->m_page_background_colour = m_page_background_colour;
    copy->m_page_background_gradient_colour = m_page_background_gradient_colour;
    copy->m_page_background_top_colour = m_page_background_top_colour;
    copy->m_page_background_top_gradient_colour = m_page_background_top_gradient_colour;
    copy->m_tab_ctrl_background_brush = m_tab_ctrl_background_brush;
    copy->m_panel_label_background_brush = m_panel_label_background_brush;
    copy->m_tab_label_font = m_tab_label_font;
    copy->m_panel_label_font = m_panel_label_font;
    copy->m_page_border_pen = m_page_border_pen;
    copy->m_panel_border_pen = m_panel_border_pen;
    copy->m_panel_border_gradient_pen = m_panel_border_gradient_pen;
    copy->m_tab_border_pen = m_tab_border_pen;
    copy->m_flags = m_flags;
    copy->m_tab_separation_size = m_tab_separation_size;
    copy->m_page_border_left = m_page_border_left;
    copy->m_page_border_top = m_page_border_top;
    copy->m_page_border_right = m_page_border_right;
    copy->m_page_border_bottom = m_page_border_bottom;
    copy->m_panel_x_separation_size = m_panel_x_separation_size;
    copy->m_panel_y_separation_size = m_panel_y_separation_size;

    return copy;
}

long wxRibbonMSWArtProvider::GetFlags()
{
    return m_flags;
}

void wxRibbonMSWArtProvider::SetFlags(long flags)
{
    m_flags = flags;
}

int wxRibbonMSWArtProvider::GetMetric(int id)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_SEPARATION_SIZE: return m_tab_separation_size;
        case wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE: return m_page_border_left;
        case wxRIBBON_ART_PAGE_BORDER_TOP_SIZE: return m_page_border_top;
        case wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE: return m_page_border_right;
        case wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE: return m_page_border_bottom;
        case wxRIBBON_ART_PANEL_X_SEPARATION_SIZE: return m_panel_x_separation_size;
        case wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE: return m_panel_y_separation_size;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return 0;
}

void wxRibbonMSWArtProvider::SetMetric(int id, int new_val)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_SEPARATION_SIZE: m_tab_separation_size = new_val;
        case wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE: m_page_border_left = new_val;
        case wxRIBBON_ART_PAGE_BORDER_TOP_SIZE: m_page_border_top = new_val;
        case wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE: m_page_border_right = new_val;
        case wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE: m_page_border_bottom = new_val;
        case wxRIBBON_ART_PANEL_X_SEPARATION_SIZE: m_panel_x_separation_size = new_val;
        case wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE: m_panel_y_separation_size = new_val;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

void wxRibbonMSWArtProvider::SetFont(int id, const wxFont& font)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_LABEL_FONT: m_tab_label_font = font;
        case wxRIBBON_ART_PANEL_LABEL_FONT: m_panel_label_font = font;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }
}

wxFont wxRibbonMSWArtProvider::GetFont(int id)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_LABEL_FONT: return m_tab_label_font;
        case wxRIBBON_ART_PANEL_LABEL_FONT: return m_panel_label_font;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return wxNullFont;
}

wxColour wxRibbonMSWArtProvider::GetColour(int id)
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
        case wxRIBBON_ART_PANEL_BORDER_COLOUR: return m_panel_border_pen.GetColour();
        case wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR: return m_panel_border_gradient_pen.GetColour();
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR: return m_panel_label_background_brush.GetColour();
        case wxRIBBON_ART_PANEL_LABEL_COLOUR: return m_panel_label_colour;
        case wxRIBBON_ART_PAGE_BORDER_COLOUR: return m_page_border_pen.GetColour();
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR: return m_page_background_top_colour;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR: return m_page_background_top_gradient_colour;
        case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR: return  m_page_background_colour;
        case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR: return m_page_background_gradient_colour;
        default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
    }

    return wxColour();
}

void wxRibbonMSWArtProvider::SetColour(int id, const wxColor& colour)
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
        case wxRIBBON_ART_PANEL_BORDER_COLOUR: m_panel_border_pen.SetColour(colour);
        case wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR: m_panel_border_gradient_pen.SetColour(colour);
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR: m_panel_label_background_brush.SetColour(colour);
        case wxRIBBON_ART_PANEL_LABEL_COLOUR: m_panel_label_colour = colour;
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

static void DrawParallelGradientLines(wxDC& dc,
                                    int nlines,
                                    const wxPoint* line_origins,
                                    int stepx,
                                    int stepy,
                                    int numsteps,
                                    int offset_x,
                                    int offset_y,
                                    const wxColour& start_colour,
                                    const wxColour& end_colour)
{
    int rd, gd, bd;
    rd = end_colour.Red() - start_colour.Red();
    gd = end_colour.Green() - start_colour.Green();
    bd = end_colour.Blue() - start_colour.Blue();

    for (int step = 0; step < numsteps; ++step)
    {
        int r,g,b;

        r = start_colour.Red() + (((step*rd*100)/numsteps)/100);
        g = start_colour.Green() + (((step*gd*100)/numsteps)/100);
        b = start_colour.Blue() + (((step*bd*100)/numsteps)/100);

        wxPen p(wxColour((unsigned char)r,
                        (unsigned char)g,
                        (unsigned char)b));
        dc.SetPen(p);

        for(int n = 0; n < nlines; ++n)
        {
            dc.DrawLine(offset_x + line_origins[n].x, offset_y + line_origins[n].y,
                        offset_x + line_origins[n].x + stepx, offset_y + line_origins[n].y + stepy);
        }

        offset_x += stepx;
        offset_y += stepy;
    }
}


void wxRibbonMSWArtProvider::DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_tab_ctrl_background_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    dc.SetPen(m_page_border_pen);
    if(rect.width > 6)
    {
        dc.DrawLine(rect.x + 3, rect.y + rect.height - 1, rect.x + rect.width - 3, rect.y + rect.height - 1);
    }
    else
    {
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
    }
}

void wxRibbonMSWArtProvider::DrawTab(
                 wxDC& dc,
                 wxWindow* WXUNUSED(wnd),
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

            int text_height;
            int text_width;
            dc.GetTextExtent(label, &text_width, &text_height);
            int width = tab.rect.width - 5;
            int x = tab.rect.x + 3;
            int y = tab.rect.y + (tab.rect.height - text_height) / 2;

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

void wxRibbonMSWArtProvider::DrawTabSeparator(
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

    // The tab separator is relatively expensive to draw (for its size), and is
    // usually drawn multiple times sequentially (in different positions), so it
    // makes sense to draw it once and cache it.
    if(!m_cached_tab_separator.IsOk() || m_cached_tab_separator.GetSize() != rect.GetSize() || visibility != m_cached_tab_separator_visibility)
    {
        wxRect size(rect.GetSize());
        ReallyDrawTabSeparator(wnd, size, visibility);
    }
    dc.DrawBitmap(m_cached_tab_separator, rect.x, rect.y, false);
}

void wxRibbonMSWArtProvider::ReallyDrawTabSeparator(wxWindow* wnd, const wxRect& rect, double visibility)
{
    if(!m_cached_tab_separator.IsOk() || m_cached_tab_separator.GetSize() != rect.GetSize())
    {
        m_cached_tab_separator = wxBitmap(rect.GetSize());
    }

    wxMemoryDC dc(m_cached_tab_separator);
    DrawTabCtrlBackground(dc, wnd, rect);

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

    m_cached_tab_separator_visibility = visibility;
}

void wxRibbonMSWArtProvider::DrawPageBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
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

void wxRibbonMSWArtProvider::DrawScrollButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect_,
                        long style)
{
    switch(style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK)
    {
    case wxRIBBON_SCROLL_BTN_LEFT:
    case wxRIBBON_SCROLL_BTN_RIGHT:
        break;
    case wxRIBBON_SCROLL_BTN_UP:
    case wxRIBBON_SCROLL_BTN_DOWN:
        // TODO
        return;
    }

    wxRect rect(rect_);

    if((style & wxRIBBON_SCROLL_BTN_FOR_MASK) == wxRIBBON_SCROLL_BTN_FOR_PAGE)
    {
        // Page scroll buttons do not have the luxury of rendering on top of anything
        // else, and their size includes some padding, hence the background painting
        // and size adjustment.
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_tab_ctrl_background_brush);
        dc.DrawRectangle(rect);
        dc.SetClippingRegion(rect);
        rect.y--;
        rect.width--;
        if((style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK) == wxRIBBON_SCROLL_BTN_LEFT)
        {
            rect.x++;
        }
    }

    {
        wxRect background(rect);
        background.x++;
        background.y++;
        background.width -= 2;
        background.height -= 2;

        background.height /= 5;
        DrawVerticalGradientRectangle(dc, background, m_page_background_top_colour, m_page_background_top_gradient_colour);

        background.y += background.height;
        background.height = rect.height - 2 - background.height;
        DrawVerticalGradientRectangle(dc, background, m_page_background_colour, m_page_background_gradient_colour);
    }

    {
        wxPoint border_points[7];
        if((style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK) == wxRIBBON_SCROLL_BTN_LEFT)
        {
            border_points[0] = wxPoint(2, 0);
            border_points[1] = wxPoint(rect.width - 1, 0);
            border_points[2] = wxPoint(rect.width - 1, rect.height - 1);
            border_points[3] = wxPoint(2, rect.height - 1);
            border_points[4] = wxPoint(0, rect.height - 3);
            border_points[5] = wxPoint(0, 2);
        }
        else
        {
            border_points[0] = wxPoint(0, 0);
            border_points[1] = wxPoint(rect.width - 3, 0);
            border_points[2] = wxPoint(rect.width - 1, 2);
            border_points[3] = wxPoint(rect.width - 1, rect.height - 3);
            border_points[4] = wxPoint(rect.width - 3, rect.height - 1);
            border_points[5] = wxPoint(0, rect.height - 1);
        }
        border_points[6] = border_points[0];

        dc.SetPen(m_page_border_pen);
        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points, rect.x, rect.y);
    }

    {
        // NB: Code for handling hovered/active state is temporary
        wxPoint arrow_points[3];
        if((style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK) == wxRIBBON_SCROLL_BTN_LEFT)
        {
            arrow_points[0] = wxPoint(rect.width / 2 - 2, rect.height / 2);
            if(style & wxRIBBON_SCROLL_BTN_ACTIVE)
                arrow_points[0].y += 1;
            arrow_points[1] = arrow_points[0] + wxPoint(3, -3);
            arrow_points[2] = arrow_points[0] + wxPoint(3,  3);
        }
        else
        {
            arrow_points[0] = wxPoint(rect.width / 2 + 2, rect.height / 2);
            if(style & wxRIBBON_SCROLL_BTN_ACTIVE)
                arrow_points[0].y += 1;
            arrow_points[1] = arrow_points[0] - wxPoint(3,  3);
            arrow_points[2] = arrow_points[0] - wxPoint(3, -3);
        }

        dc.SetPen(*wxTRANSPARENT_PEN);
        wxBrush B(style & wxRIBBON_SCROLL_BTN_HOVERED ? m_tab_active_background_colour : m_tab_label_colour);
        dc.SetBrush(B);
        dc.DrawPolygon(sizeof(arrow_points)/sizeof(wxPoint), arrow_points, rect.x, rect.y);
    }
}

void wxRibbonMSWArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    wxRect true_rect(rect);
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        true_rect.y += 1;
        true_rect.height -= 2;
    }
    else
    {
        true_rect.x += 1;
        true_rect.width -= 2;
    }

    {
        dc.SetBrush(m_panel_label_background_brush);
        dc.SetFont(m_panel_label_font);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetTextForeground(m_panel_label_colour);

        wxRect label_rect(true_rect);
        wxSize label_size(dc.GetTextExtent(wnd->GetLabel()));

        label_rect.SetHeight(label_size.GetHeight() + 2);
        label_rect.SetY(true_rect.GetBottom() - label_rect.GetHeight());

        dc.DrawRectangle(label_rect.GetX(), label_rect.GetY(), label_rect.GetWidth(), label_rect.GetHeight());
        dc.DrawText(wnd->GetLabel(), label_rect.x + (label_rect.GetWidth() - label_size.GetWidth()) / 2, label_rect.y + (label_rect.GetHeight() - label_size.GetHeight()) / 2);
    }

    {
        wxPoint border_points[9];
        border_points[0] = wxPoint(2, 0);
        border_points[1] = wxPoint(true_rect.width - 3, 0);
        border_points[2] = wxPoint(true_rect.width - 1, 2);
        border_points[3] = wxPoint(true_rect.width - 1, true_rect.height - 3);
        border_points[4] = wxPoint(true_rect.width - 3, true_rect.height - 1);
        border_points[5] = wxPoint(2, true_rect.height - 1);
        border_points[6] = wxPoint(0, true_rect.height - 3);
        border_points[7] = wxPoint(0, 2);

        if(m_panel_border_pen.GetColour() == m_panel_border_gradient_pen.GetColour())
        {
            border_points[8] = border_points[0];
            dc.SetPen(m_panel_border_pen);
            dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points, true_rect.x, true_rect.y);
        }
        else
        {
            dc.SetPen(m_panel_border_pen);
            dc.DrawLines(3, border_points, true_rect.x, true_rect.y);
#define SingleLine(start, finish) \
            dc.DrawLine(start.x + true_rect.x, start.y + true_rect.y, finish.x + true_rect.x, finish.y + true_rect.y)
            SingleLine(border_points[0], border_points[7]);

            dc.SetPen(m_panel_border_gradient_pen);
            dc.DrawLines(3, border_points + 4, true_rect.x, true_rect.y);
            SingleLine(border_points[4], border_points[3]);
#undef SingleLine

            border_points[6] = border_points[2];
            DrawParallelGradientLines(dc, 2, border_points + 6, 0, 1, border_points[3].y - border_points[2].y + 1,
                true_rect.x, true_rect.y, m_panel_border_pen.GetColour(), m_panel_border_gradient_pen.GetColour());
        }
    }
}

void wxRibbonMSWArtProvider::GetBarTabWidth(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* small_begin_need_separator,
                        int* small_must_have_separator,
                        int* minimum)
{
    int width = 0;
    int min = 0;
    if((m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) && !label.IsEmpty())
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
    if((m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS) && bitmap.IsOk())
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

int wxRibbonMSWArtProvider::GetTabCtrlHeight(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRibbonPageTabInfoArray& pages)
{
    int text_height = 0;
    int icon_height = 0;

    if(m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
    {
        dc.SetFont(m_tab_label_font);
        text_height = dc.GetTextExtent(wxT("ABCDEFXj")).GetHeight() + 10;
    }
    if(m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
    {
        size_t numpages = pages.GetCount();
        for(size_t i = 0; i < numpages; ++i)
        {
            const wxRibbonPageTabInfo& info = pages.Item(i);
            if(info.page->GetIcon().IsOk())
            {
                icon_height = wxMax(icon_height, info.page->GetIcon().GetHeight() + 4);
            }
        }
    }

    return wxMax(text_height, icon_height);
}

wxSize wxRibbonMSWArtProvider::GetScrollButtonMinimumSize(
                        wxDC& WXUNUSED(dc),
                        wxWindow* WXUNUSED(wnd),
                        long WXUNUSED(style))
{
    return wxSize(12, 12);
}

wxSize wxRibbonMSWArtProvider::GetPanelSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize client_size)
{
    dc.SetFont(m_panel_label_font);
    wxSize label_size = dc.GetTextExtent(wnd->GetLabel());

    client_size.IncBy(0, label_size.GetHeight());
    client_size.IncBy(4, 8); // Guesswork at the moment

    return client_size;
}

wxRect wxRibbonMSWArtProvider::GetPageBackgroundRedrawArea(
                        wxDC& WXUNUSED(dc),
                        const wxRibbonPage* WXUNUSED(wnd),
                        wxSize page_old_size,
                        wxSize page_new_size)
{
    wxRect new_rect, old_rect;

    if(page_new_size.GetWidth() != page_old_size.GetWidth())
    {
        if(page_new_size.GetHeight() != page_old_size.GetHeight())
        {
            // Width and height both changed - redraw everything
            return wxRect(page_new_size);
        }
        else
        {
            // Only width changed - redraw right hand side
            const int right_edge_width = 4;

            new_rect = wxRect(page_new_size.GetWidth() - right_edge_width, 0, right_edge_width, page_new_size.GetHeight());
            old_rect = wxRect(page_old_size.GetWidth() - right_edge_width, 0, right_edge_width, page_old_size.GetHeight());
        }
    }
    else
    {
        if(page_new_size.GetHeight() == page_old_size.GetHeight())
        {
            // Nothing changed (should never happen) - redraw nothing
            return wxRect(0, 0, 0, 0);
        }
        else
        {
            // Only height changed - redraw bottom
            const int bottom_edge_height = 4;

            new_rect = wxRect(0, page_new_size.GetHeight() - bottom_edge_height, page_new_size.GetWidth(), bottom_edge_height);
            old_rect = wxRect(0, page_old_size.GetHeight() - bottom_edge_height, page_old_size.GetWidth(), bottom_edge_height);
        }
    }

    new_rect.Union(old_rect);
    new_rect.Intersect(wxRect(page_new_size));
    return new_rect;
}

#endif // wxUSE_RIBBON
