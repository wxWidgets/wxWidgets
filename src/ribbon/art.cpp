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
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"

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
    m_button_bar_label_colour = m_tab_label_colour;
    m_panel_minimised_label_colour = m_tab_label_colour;
    m_tab_separator_colour = wxColour(183, 212, 249);
    m_tab_separator_gradient_colour = wxColour(116, 153, 203);
    m_tab_active_background_colour = wxColour(242, 248, 244);
    m_tab_active_background_gradient_colour = wxColour(223, 233, 245);
    m_tab_hover_background_colour = wxColour(212, 228, 249);
    m_tab_hover_background_gradient_colour = wxColour(225, 210, 165);
    m_tab_hover_background_top_colour = wxColour(196, 221, 254);
    m_tab_hover_background_top_gradient_colour = wxColour(221, 235, 254);
    m_panel_label_colour = wxColour(62, 106, 170);
    m_panel_hover_label_colour = m_panel_label_colour;
    m_panel_active_background_colour = wxColour(118, 153, 200);
    m_panel_active_background_gradient_colour = wxColour(179, 212, 253);
    m_panel_active_background_top_colour = wxColour(174, 194, 221);
    m_panel_active_background_top_gradient_colour = wxColour(136, 167, 208);
    m_page_background_colour = wxColour(199, 216, 237);
    m_page_background_gradient_colour = wxColour(227, 244, 255);
    m_page_background_top_colour = wxColour(223, 233, 245);
    m_page_background_top_gradient_colour = wxColour(209, 223, 240);
    m_page_hover_background_colour = wxColour(220, 234, 251);
    m_page_hover_background_gradient_colour = wxColour(221, 232, 247);
    m_page_hover_background_top_colour = wxColour(225, 239, 253);
    m_page_hover_background_top_gradient_colour = wxColour(232, 240, 252);
    m_button_bar_hover_background_colour = wxColour(255, 215, 80);
    m_button_bar_hover_background_gradient_colour = wxColour(255, 231, 153);
    m_button_bar_hover_background_top_colour = wxColour(255, 252, 217);
    m_button_bar_hover_background_top_gradient_colour = wxColour(255, 231, 147);
    m_gallery_button_background_colour = wxColour(202, 221, 246);
    m_gallery_button_background_gradient_colour = wxColour(207, 224, 247);
    m_gallery_button_hover_background_colour = wxColour(255, 216, 93);
    m_gallery_button_hover_background_gradient_colour = wxColour(253, 230, 150);
    m_gallery_button_active_background_colour = wxColour(240, 122, 33);
    m_gallery_button_active_background_gradient_colour = wxColour(253, 159, 77);
    m_gallery_button_disabled_background_colour = wxColour(224, 228, 232);
    m_gallery_button_disabled_background_gradient_colour = wxColour(232, 235, 239);
    m_gallery_button_face_colour = wxColour(103, 140, 189);
    m_gallery_button_hover_face_colour = wxColour(86, 125, 177);
    m_gallery_button_active_face_colour = m_gallery_button_hover_face_colour;
    m_gallery_button_disabled_face_colour = wxColour(183, 183, 183);

    m_tab_ctrl_background_brush = wxBrush(wxColour(191, 219, 255));
    m_panel_label_background_brush = wxBrush(wxColour(193, 216, 241));
    m_panel_hover_label_background_brush = wxBrush(wxColour(200, 224, 255));
    m_gallery_hover_background_brush = wxBrush(wxColour(236, 243, 251));
    m_gallery_button_background_top_brush = wxBrush(wxColour(220, 235, 254));
    m_gallery_button_hover_background_top_brush = wxBrush(wxColour(255, 249, 216));    
    m_gallery_button_active_background_top_brush = wxBrush(wxColour(254, 169, 90));
    m_gallery_button_disabled_background_top_brush = wxBrush(wxColour(240, 243, 246));

    m_tab_label_font = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE);
    m_button_bar_label_font = m_tab_label_font;
    m_panel_label_font = m_tab_label_font;

    m_tab_border_pen = wxPen(wxColour(153, 187, 232));
    m_button_bar_hover_border_pen = wxPen(wxColour(194, 170, 121));
    m_panel_border_pen = wxPen(wxColour(197, 210, 223));
    m_panel_border_gradient_pen = wxPen(wxColour(158, 191, 219));
    m_panel_minimised_border_pen = wxPen(wxColour(165, 191, 213));
    m_panel_minimised_border_gradient_pen = wxPen(wxColour(148, 185, 213));
    m_page_border_pen = wxPen(wxColour(141, 178, 227));
    m_gallery_border_pen = wxPen(wxColour(185, 208, 237));

    m_cached_tab_separator_visibility = -10.0; // valid visibilities are in range [0, 1]
    m_tab_separation_size = 3;
    m_page_border_left = 2;
    m_page_border_top = 1;
    m_page_border_right = 2;
    m_page_border_bottom = 3;
    m_panel_x_separation_size = 1;
    m_panel_y_separation_size = 1;
    m_gallery_bitmap_padding_left_size = 4;
    m_gallery_bitmap_padding_right_size = 4;
    m_gallery_bitmap_padding_top_size = 4;
    m_gallery_bitmap_padding_bottom_size = 4;
}

wxRibbonMSWArtProvider::~wxRibbonMSWArtProvider()
{
}

wxRibbonArtProvider* wxRibbonMSWArtProvider::Clone()
{
    wxRibbonMSWArtProvider *copy = new wxRibbonMSWArtProvider;

    copy->m_button_bar_label_colour = m_button_bar_label_colour;
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
    copy->m_panel_hover_label_colour = m_panel_hover_label_colour;
    copy->m_panel_minimised_label_colour = m_panel_minimised_label_colour;
    copy->m_panel_active_background_colour = m_panel_active_background_colour;
    copy->m_panel_active_background_gradient_colour = m_panel_active_background_gradient_colour;
    copy->m_panel_active_background_top_colour = m_panel_active_background_top_colour;
    copy->m_panel_active_background_top_gradient_colour = m_panel_active_background_top_gradient_colour;
    copy->m_page_background_colour = m_page_background_colour;
    copy->m_page_background_gradient_colour = m_page_background_gradient_colour;
    copy->m_page_background_top_colour = m_page_background_top_colour;
    copy->m_page_background_top_gradient_colour = m_page_background_top_gradient_colour;
    copy->m_page_hover_background_colour = m_page_hover_background_colour;
    copy->m_page_hover_background_gradient_colour = m_page_hover_background_gradient_colour;
    copy->m_page_hover_background_top_colour = m_page_hover_background_top_colour;
    copy->m_page_hover_background_top_gradient_colour = m_page_hover_background_top_gradient_colour;
    copy->m_button_bar_hover_background_colour = m_button_bar_hover_background_colour;
    copy->m_button_bar_hover_background_gradient_colour = m_button_bar_hover_background_gradient_colour;
    copy->m_button_bar_hover_background_top_colour = m_button_bar_hover_background_top_colour;
    copy->m_button_bar_hover_background_top_gradient_colour = m_button_bar_hover_background_top_gradient_colour;
    copy->m_gallery_button_background_colour = m_gallery_button_background_colour;
    copy->m_gallery_button_background_gradient_colour = m_gallery_button_background_gradient_colour;    
    copy->m_gallery_button_hover_background_colour = m_gallery_button_hover_background_colour;
    copy->m_gallery_button_hover_background_gradient_colour = m_gallery_button_hover_background_gradient_colour;
    copy->m_gallery_button_active_background_colour = m_gallery_button_active_background_colour;
    copy->m_gallery_button_active_background_gradient_colour = m_gallery_button_active_background_gradient_colour;
    copy->m_gallery_button_disabled_background_colour = m_gallery_button_disabled_background_colour;
    copy->m_gallery_button_disabled_background_gradient_colour = m_gallery_button_disabled_background_gradient_colour;
    copy->m_gallery_button_face_colour = m_gallery_button_face_colour;
    copy->m_gallery_button_hover_face_colour = m_gallery_button_hover_face_colour;
    copy->m_gallery_button_active_face_colour = m_gallery_button_active_face_colour;
    copy->m_gallery_button_disabled_face_colour = m_gallery_button_disabled_face_colour;

    copy->m_tab_ctrl_background_brush = m_tab_ctrl_background_brush;
    copy->m_panel_label_background_brush = m_panel_label_background_brush;
    copy->m_panel_hover_label_background_brush = m_panel_hover_label_background_brush;
    copy->m_gallery_hover_background_brush = m_gallery_hover_background_brush;
    copy->m_gallery_button_background_top_brush = m_gallery_button_background_top_brush;
    copy->m_gallery_button_hover_background_top_brush = m_gallery_button_hover_background_top_brush;
    copy->m_gallery_button_active_background_top_brush = m_gallery_button_active_background_top_brush;
    copy->m_gallery_button_disabled_background_top_brush = m_gallery_button_disabled_background_top_brush;

    copy->m_tab_label_font = m_tab_label_font;
    copy->m_button_bar_label_font = m_button_bar_label_font;
    copy->m_panel_label_font = m_panel_label_font;

    copy->m_page_border_pen = m_page_border_pen;
    copy->m_panel_border_pen = m_panel_border_pen;
    copy->m_panel_border_gradient_pen = m_panel_border_gradient_pen;
    copy->m_panel_minimised_border_pen = m_panel_minimised_border_pen;
    copy->m_panel_minimised_border_gradient_pen = m_panel_minimised_border_gradient_pen;
    copy->m_tab_border_pen = m_tab_border_pen;
    copy->m_gallery_border_pen = m_gallery_border_pen;
    copy->m_button_bar_hover_border_pen = m_button_bar_hover_border_pen;

    copy->m_flags = m_flags;
    copy->m_tab_separation_size = m_tab_separation_size;
    copy->m_page_border_left = m_page_border_left;
    copy->m_page_border_top = m_page_border_top;
    copy->m_page_border_right = m_page_border_right;
    copy->m_page_border_bottom = m_page_border_bottom;
    copy->m_panel_x_separation_size = m_panel_x_separation_size;
    copy->m_panel_y_separation_size = m_panel_y_separation_size;
    copy->m_gallery_bitmap_padding_left_size = m_gallery_bitmap_padding_left_size;
    copy->m_gallery_bitmap_padding_right_size = m_gallery_bitmap_padding_right_size;
    copy->m_gallery_bitmap_padding_top_size = m_gallery_bitmap_padding_top_size;
    copy->m_gallery_bitmap_padding_bottom_size = m_gallery_bitmap_padding_bottom_size;

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
        case wxRIBBON_ART_TAB_SEPARATION_SIZE:
            return m_tab_separation_size;
        case wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE:
            return m_page_border_left;
        case wxRIBBON_ART_PAGE_BORDER_TOP_SIZE:
            return m_page_border_top;
        case wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE:
            return m_page_border_right;
        case wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE:
            return m_page_border_bottom;
        case wxRIBBON_ART_PANEL_X_SEPARATION_SIZE:
            return m_panel_x_separation_size;
        case wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE:
            return m_panel_y_separation_size;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE:
            return m_gallery_bitmap_padding_left_size;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE:
            return m_gallery_bitmap_padding_right_size;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE:
            return m_gallery_bitmap_padding_top_size;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE:
            return m_gallery_bitmap_padding_bottom_size;
        default:
            wxFAIL_MSG(wxT("Invalid Metric Ordinal"));
            break;
    }

    return 0;
}

void wxRibbonMSWArtProvider::SetMetric(int id, int new_val)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_SEPARATION_SIZE:
            m_tab_separation_size = new_val;
            break;
        case wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE:
            m_page_border_left = new_val;
            break;
        case wxRIBBON_ART_PAGE_BORDER_TOP_SIZE:
            m_page_border_top = new_val;
            break;
        case wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE:
            m_page_border_right = new_val;
            break;
        case wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE:
            m_page_border_bottom = new_val;
            break;
        case wxRIBBON_ART_PANEL_X_SEPARATION_SIZE:
            m_panel_x_separation_size = new_val;
            break;
        case wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE:
            m_panel_y_separation_size = new_val;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE:
            m_gallery_bitmap_padding_left_size = new_val;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE:
            m_gallery_bitmap_padding_right_size = new_val;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE:
            m_gallery_bitmap_padding_top_size = new_val;
            break;
        case wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE:
            m_gallery_bitmap_padding_bottom_size = new_val;
            break;
        default:
            wxFAIL_MSG(wxT("Invalid Metric Ordinal"));
            break;
    }
}

void wxRibbonMSWArtProvider::SetFont(int id, const wxFont& font)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_LABEL_FONT:
            m_tab_label_font = font;
            break;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_FONT:
            m_button_bar_label_font = font;
            break;
        case wxRIBBON_ART_PANEL_LABEL_FONT:
            m_panel_label_font = font;
            break;
        default:
            wxFAIL_MSG(wxT("Invalid Metric Ordinal"));
            break;
    }
}

wxFont wxRibbonMSWArtProvider::GetFont(int id)
{
    switch(id)
    {
        case wxRIBBON_ART_TAB_LABEL_FONT:
            return m_tab_label_font;
        case wxRIBBON_ART_BUTTON_BAR_LABEL_FONT:
            return m_button_bar_label_font;
        case wxRIBBON_ART_PANEL_LABEL_FONT:
            return m_panel_label_font;
        default:
            wxFAIL_MSG(wxT("Invalid Metric Ordinal"));
            break;
    }

    return wxNullFont;
}

wxColour wxRibbonMSWArtProvider::GetColour(int id)
{
    switch(id)
    {
        case wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR:
            return m_button_bar_label_colour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BORDER_COLOUR:
            return m_button_bar_hover_border_pen.GetColour();
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_COLOUR:
            return m_button_bar_hover_background_top_colour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_button_bar_hover_background_top_gradient_colour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR:
            return m_button_bar_hover_background_colour;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_button_bar_hover_background_gradient_colour;
        case wxRIBBON_ART_GALLERY_BORDER_COLOUR:
            return m_gallery_border_pen.GetColour();
        case wxRIBBON_ART_GALLERY_HOVER_BACKGROUND_COLOUR:
            return m_gallery_hover_background_brush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_COLOUR:
            return m_gallery_button_background_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_GRADIENT_COLOUR:
            return m_gallery_button_background_gradient_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_TOP_COLOUR:
            return m_gallery_button_background_top_brush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR:
            return m_gallery_button_face_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR:
            return m_gallery_button_hover_background_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_gallery_button_hover_background_gradient_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_TOP_COLOUR:
            return m_gallery_button_hover_background_top_brush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR:
            return m_gallery_button_face_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR:
            return m_gallery_button_active_background_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_gallery_button_active_background_gradient_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_TOP_COLOUR:
            return m_gallery_button_background_top_brush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR:
            return m_gallery_button_active_face_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR:
            return m_gallery_button_disabled_background_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR:
            return m_gallery_button_disabled_background_gradient_colour;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_TOP_COLOUR:
            return m_gallery_button_disabled_background_top_brush.GetColour();
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR:
            return m_gallery_button_disabled_face_colour;
        case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR:
            return m_tab_ctrl_background_brush.GetColour();
        case wxRIBBON_ART_TAB_LABEL_COLOUR:
            return m_tab_label_colour;
        case wxRIBBON_ART_TAB_SEPARATOR_COLOUR:
            return m_tab_separator_colour;
        case wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR:
            return m_tab_separator_gradient_colour;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR:
            return m_tab_active_background_colour;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_tab_active_background_gradient_colour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR:
            return m_tab_hover_background_top_colour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_tab_hover_background_top_gradient_colour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR:
            return m_tab_hover_background_colour;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_tab_hover_background_gradient_colour;
        case wxRIBBON_ART_TAB_BORDER_COLOUR:
            return m_tab_border_pen.GetColour();
        case wxRIBBON_ART_PANEL_BORDER_COLOUR:
            return m_panel_border_pen.GetColour();
        case wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR:
            return m_panel_border_gradient_pen.GetColour();
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR:
            return m_panel_minimised_border_pen.GetColour();
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR:
            return m_panel_minimised_border_gradient_pen.GetColour();
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR:
            return m_panel_label_background_brush.GetColour();
        case wxRIBBON_ART_PANEL_LABEL_COLOUR:
            return m_panel_label_colour;
        case wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR:
            return m_panel_minimised_label_colour;
        case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR:
            return m_panel_hover_label_background_brush.GetColour();
        case wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR:
            return m_panel_hover_label_colour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR:
            return m_panel_active_background_top_colour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_panel_active_background_top_gradient_colour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR:
            return m_panel_active_background_colour;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            return m_panel_active_background_gradient_colour;
        case wxRIBBON_ART_PAGE_BORDER_COLOUR:
            return m_page_border_pen.GetColour();
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR:
            return m_page_background_top_colour;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_page_background_top_gradient_colour;
        case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:
            return m_page_background_colour;
        case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR:
            return m_page_background_gradient_colour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR:
            return m_page_hover_background_top_colour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            return m_page_hover_background_top_gradient_colour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR:
            return m_page_hover_background_colour;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR:
            return m_page_hover_background_gradient_colour;
        default:
            wxFAIL_MSG(wxT("Invalid Metric Ordinal"));
            break;
    }

    return wxColour();
}

void wxRibbonMSWArtProvider::SetColour(int id, const wxColor& colour)
{
    switch(id)
    {
        case wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR:
            m_button_bar_label_colour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BORDER_COLOUR:
            m_button_bar_hover_border_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_COLOUR:
            m_button_bar_hover_background_top_colour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_button_bar_hover_background_top_gradient_colour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR:
            m_button_bar_hover_background_colour = colour;
            break;
        case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_button_bar_hover_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BORDER_COLOUR:
            m_gallery_border_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_HOVER_BACKGROUND_COLOUR:
            m_gallery_hover_background_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_COLOUR:
            m_gallery_button_background_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_GRADIENT_COLOUR:
            m_gallery_button_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_TOP_COLOUR:
            m_gallery_button_background_top_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR:
            m_gallery_button_face_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR:
            m_gallery_button_hover_background_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_gallery_button_hover_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_TOP_COLOUR:
            m_gallery_button_hover_background_top_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR:
            m_gallery_button_hover_face_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR:
            m_gallery_button_active_background_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_gallery_button_active_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_TOP_COLOUR:
            m_gallery_button_background_top_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR:
            m_gallery_button_active_face_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR:
            m_gallery_button_disabled_background_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR:
            m_gallery_button_disabled_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_TOP_COLOUR:
            m_gallery_button_disabled_background_top_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR:
            m_gallery_button_disabled_face_colour = colour;
            break;
        case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR:
            m_tab_ctrl_background_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_TAB_LABEL_COLOUR:
            m_tab_label_colour = colour;
            break;
        case wxRIBBON_ART_TAB_SEPARATOR_COLOUR:
            m_tab_separator_colour = colour;
            break;
        case wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR:
            m_tab_separator_gradient_colour = colour;
            break;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR:
            m_tab_active_background_colour = colour;
            break;
        case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_tab_active_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR:
            m_tab_hover_background_top_colour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_tab_hover_background_top_gradient_colour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR:
            m_tab_hover_background_colour = colour;
            break;
        case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_tab_hover_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_TAB_BORDER_COLOUR:
            m_tab_border_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_BORDER_COLOUR:
            m_panel_border_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR:
            m_panel_border_gradient_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR:
            m_panel_minimised_border_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR:
            m_panel_minimised_border_gradient_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR:
            m_panel_label_background_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_LABEL_COLOUR:
            m_panel_label_colour = colour;
            break;
        case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR:
            m_panel_hover_label_background_brush.SetColour(colour);
            break;
        case wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR:
            m_panel_hover_label_colour = colour;
            break;
        case wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR:
            m_panel_minimised_label_colour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR:
            m_panel_active_background_top_colour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_panel_active_background_top_gradient_colour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR:
            m_panel_active_background_colour = colour;
            break;
        case wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
            m_panel_active_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_BORDER_COLOUR:
            m_page_border_pen.SetColour(colour);
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR:
            m_page_background_top_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_page_background_top_gradient_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:
            m_page_background_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR:
            m_page_background_gradient_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR:
            m_page_hover_background_top_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR:
            m_page_hover_background_top_gradient_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR:
            m_page_hover_background_colour = colour;
            break;
        case wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR:
            m_page_hover_background_gradient_colour = colour;
            break;
        default:
            wxFAIL_MSG(wxT("Invalid Metric Ordinal"));
            break;
    }
}

static void DrawVerticalGradientRectangle(wxDC& dc,
                                  const wxRect& rect,
                                  const wxColour& start_colour,
                                  const wxColour& end_colour)
{
    dc.GradientFillLinear(rect, start_colour, end_colour, wxSOUTH);
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

static wxColour InterpolateColour(const wxColour& start_colour,
                                const wxColour& end_colour,
                                int position,
                                int start_position,
                                int end_position)
{
    if(position <= start_position)
    {
        return start_colour;
    }
    if(position >= end_position)
    {
        return end_colour;
    }
    position -= start_position;
    end_position -= start_position;
    int r = end_colour.Red() - start_colour.Red();
    int g = end_colour.Green() - start_colour.Green();
    int b = end_colour.Blue() - start_colour.Blue();
    r = start_colour.Red()   + (((r * position * 100) / end_position) / 100);
    g = start_colour.Green() + (((g * position * 100) / end_position) / 100);
    b = start_colour.Blue()  + (((b * position * 100) / end_position) / 100);
    return wxColour(r, g, b);
}

void wxRibbonMSWArtProvider::DrawPartialPageBackground(wxDC& dc,
        wxWindow* WXUNUSED(wnd), const wxRect& rect, wxRibbonPage* page,
        wxPoint offset, bool hovered)
{
    wxRect background(page->GetSize());
    page->AdjustRectToIncludeScrollButtons(&background);
    background.height -= 2;
    // Page background isn't dependant upon the width of the page
    // (at least not the part of it intended to be painted by this
    // function). Set to wider than the page itself for when externally
    // expanded panels need a background - the expanded panel can be wider
    // than the bar.
    background.x = 0;
    background.width = INT_MAX;

    // upper_rect, lower_rect, paint_rect are all in page co-ordinates
    wxRect upper_rect(background);
    upper_rect.height /= 5;

    wxRect lower_rect(background);
    lower_rect.y += upper_rect.height;
    lower_rect.height -= upper_rect.height;

    wxRect paint_rect(rect);
    paint_rect.x += offset.x;
    paint_rect.y += offset.y;

    wxColour bg_top, bg_top_grad, bg_btm, bg_btm_grad;
    if(hovered)
    {
        bg_top = m_page_hover_background_top_colour;
        bg_top_grad = m_page_hover_background_top_gradient_colour;
        bg_btm = m_page_hover_background_colour;
        bg_btm_grad = m_page_hover_background_gradient_colour;
    }
    else
    {
        bg_top = m_page_background_top_colour;
        bg_top_grad = m_page_background_top_gradient_colour;
        bg_btm = m_page_background_colour;
        bg_btm_grad = m_page_background_gradient_colour;
    }

    if(paint_rect.Intersects(upper_rect))
    {
        wxRect rect(upper_rect);
        rect.Intersect(paint_rect);
        rect.x -= offset.x;
        rect.y -= offset.y;
        wxColour starting_colour(InterpolateColour(bg_top, bg_top_grad,
            paint_rect.y, upper_rect.y, upper_rect.y + upper_rect.height));
        wxColour ending_colour(InterpolateColour(bg_top, bg_top_grad,
            paint_rect.y + paint_rect.height, upper_rect.y,
            upper_rect.y + upper_rect.height));
        DrawVerticalGradientRectangle(dc, rect, starting_colour, ending_colour);
    }

    if(paint_rect.Intersects(lower_rect))
    {
        wxRect rect(lower_rect);
        rect.Intersect(paint_rect);
        rect.x -= offset.x;
        rect.y -= offset.y;
        wxColour starting_colour(InterpolateColour(bg_btm, bg_btm_grad,
            paint_rect.y, lower_rect.y, lower_rect.y + lower_rect.height));
        wxColour ending_colour(InterpolateColour(bg_btm, bg_btm_grad,
            paint_rect.y + paint_rect.height,
            lower_rect.y, lower_rect.y + lower_rect.height));
        DrawVerticalGradientRectangle(dc, rect, starting_colour, ending_colour);
    }
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

void wxRibbonMSWArtProvider::DrawDropdownArrow(wxDC& dc, int x, int y, const wxColour& colour)
{
    wxPoint arrow_points[3];
    wxBrush brush(colour);
    arrow_points[0] = wxPoint(1, 2);
    arrow_points[1] = arrow_points[0] + wxPoint(-3, -3);
    arrow_points[2] = arrow_points[0] + wxPoint( 3, -3);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(brush);
    dc.DrawPolygon(sizeof(arrow_points)/sizeof(wxPoint), arrow_points, x, y);
}

void wxRibbonMSWArtProvider::RemovePanelPadding(wxRect* rect)
{
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        rect->y += 1;
        rect->height -= 2;
    }
    else
    {
        rect->x += 1;
        rect->width -= 2;
    }
}

void wxRibbonMSWArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);

    int label_height;
    {
        dc.SetFont(m_panel_label_font);
        dc.SetPen(*wxTRANSPARENT_PEN);
        if(wnd->IsHovered())
        {
            dc.SetBrush(m_panel_hover_label_background_brush);
            dc.SetTextForeground(m_panel_hover_label_colour);
        }
        else
        {
            dc.SetBrush(m_panel_label_background_brush);
            dc.SetTextForeground(m_panel_label_colour);
        }

        wxRect label_rect(true_rect);
        wxSize label_size(dc.GetTextExtent(wnd->GetLabel()));

        label_rect.SetHeight(label_size.GetHeight() + 2);
        label_rect.SetY(true_rect.GetBottom() - label_rect.GetHeight());
        label_height = label_rect.GetHeight();

        dc.DrawRectangle(label_rect.GetX(), label_rect.GetY(), label_rect.GetWidth(), label_rect.GetHeight());
        dc.DrawText(wnd->GetLabel(), label_rect.x + (label_rect.GetWidth() - label_size.GetWidth()) / 2, label_rect.y + (label_rect.GetHeight() - label_size.GetHeight()) / 2);
    }

    if(wnd->IsHovered())
    {
        wxRect client_rect(true_rect);
        client_rect.x++;
        client_rect.width -= 2;
        client_rect.y++;
        client_rect.height -= 2 + label_height;
        DrawPartialPageBackground(dc, wnd, client_rect, true);
    }

    DrawPanelBorder(dc, true_rect, m_panel_border_pen, m_panel_border_gradient_pen);
}

void wxRibbonMSWArtProvider::DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect);

    if(wnd->IsHovered())
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_gallery_hover_background_brush);
        dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 16, rect.height - 2);
    }

    dc.SetPen(m_gallery_border_pen);
    // Outline
    dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width - 1, rect.y);
    dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
    dc.DrawLine(rect.x + 1, rect.y + rect.height - 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);
    dc.DrawLine(rect.x + rect.width - 1, rect.y + 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);

    // Divider between items and buttons
    dc.DrawLine(rect.x + rect.width - 16, rect.y, rect.x + rect.width - 16,
        rect.y + rect.height);

    wxRect up_btn(rect.x + rect.width - 16, rect.y, 16, rect.height / 3);

    wxRect down_btn(up_btn.GetLeft(), up_btn.GetBottom() + 1, up_btn.GetWidth(),
        up_btn.GetHeight());
    dc.DrawLine(down_btn.GetLeft(), down_btn.GetTop(), down_btn.GetRight(),
        down_btn.GetTop());

    wxRect ext_btn(up_btn.GetLeft(), down_btn.GetBottom() + 1, up_btn.GetWidth(),
        rect.height - up_btn.GetHeight() - down_btn.GetHeight() - 1);
    dc.DrawLine(ext_btn.GetLeft(), ext_btn.GetTop(), ext_btn.GetRight(),
        ext_btn.GetTop());

    DrawGalleryButton(dc, up_btn, wxRIBBON_GALLERY_BUTTON_NORMAL);
    DrawGalleryButton(dc, down_btn, wxRIBBON_GALLERY_BUTTON_NORMAL);
    DrawGalleryButton(dc, ext_btn, wxRIBBON_GALLERY_BUTTON_NORMAL);
}

void wxRibbonMSWArtProvider::DrawGalleryButton(wxDC& dc,
                                            wxRect rect,
                                            wxRibbonGalleryButtonState state)
{
    wxBrush btn_top_brush;
    wxColour btn_face_colour;
    wxColour btn_colour;
    wxColour btn_grad_colour;
    switch(state)
    {
    case wxRIBBON_GALLERY_BUTTON_NORMAL:
        btn_top_brush = m_gallery_button_background_top_brush;
        btn_face_colour = m_gallery_button_face_colour;
        btn_colour = m_gallery_button_background_colour;
        btn_grad_colour = m_gallery_button_background_gradient_colour;
        break;
    case wxRIBBON_GALLERY_BUTTON_HOVERED:
        btn_top_brush = m_gallery_button_hover_background_top_brush;
        btn_face_colour = m_gallery_button_hover_face_colour;
        btn_colour = m_gallery_button_hover_background_colour;
        btn_grad_colour = m_gallery_button_hover_background_gradient_colour;
        break;
    case wxRIBBON_GALLERY_BUTTON_ACTIVE:
        btn_top_brush = m_gallery_button_active_background_top_brush;
        btn_face_colour = m_gallery_button_active_face_colour;
        btn_colour = m_gallery_button_active_background_colour;
        btn_grad_colour = m_gallery_button_active_background_gradient_colour;
        break;
    case wxRIBBON_GALLERY_BUTTON_DISABLED:
        btn_top_brush = m_gallery_button_disabled_background_top_brush;
        btn_face_colour = m_gallery_button_disabled_face_colour;
        btn_colour = m_gallery_button_disabled_background_colour;
        btn_grad_colour = m_gallery_button_disabled_background_gradient_colour;
        break;
    }

    rect.x++;
    rect.width -= 2;
    rect.y++;
    rect.height--;

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(btn_top_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height / 2);

    wxRect lower(rect);
    lower.height = (lower.height + 1) / 2;
    lower.y += rect.height - lower.height;
    DrawVerticalGradientRectangle(dc, lower, btn_colour, btn_grad_colour);
}

void wxRibbonMSWArtProvider::DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item)
{
    // TODO
}

void wxRibbonMSWArtProvider::DrawPanelBorder(wxDC& dc, const wxRect& rect,
                                             wxPen& primary_colour,
                                             wxPen& secondary_colour)
{
    wxPoint border_points[9];
    border_points[0] = wxPoint(2, 0);
    border_points[1] = wxPoint(rect.width - 3, 0);
    border_points[2] = wxPoint(rect.width - 1, 2);
    border_points[3] = wxPoint(rect.width - 1, rect.height - 3);
    border_points[4] = wxPoint(rect.width - 3, rect.height - 1);
    border_points[5] = wxPoint(2, rect.height - 1);
    border_points[6] = wxPoint(0, rect.height - 3);
    border_points[7] = wxPoint(0, 2);

    if(primary_colour.GetColour() == secondary_colour.GetColour())
    {
        border_points[8] = border_points[0];
        dc.SetPen(primary_colour);
        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points, rect.x, rect.y);
    }
    else
    {
        dc.SetPen(primary_colour);
        dc.DrawLines(3, border_points, rect.x, rect.y);

#define SingleLine(start, finish) \
        dc.DrawLine(start.x + rect.x, start.y + rect.y, finish.x + rect.x, finish.y + rect.y)

        SingleLine(border_points[0], border_points[7]);
        dc.SetPen(secondary_colour);
        dc.DrawLines(3, border_points + 4, rect.x, rect.y);
        SingleLine(border_points[4], border_points[3]);

#undef SingleLine

        border_points[6] = border_points[2];
        DrawParallelGradientLines(dc, 2, border_points + 6, 0, 1,
            border_points[3].y - border_points[2].y + 1, rect.x, rect.y,
            primary_colour.GetColour(), secondary_colour.GetColour());
    }
}

void wxRibbonMSWArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);

    if(wnd->GetExpandedPanel() != NULL)
    {
        wxRect client_rect(true_rect);
        client_rect.x++;
        client_rect.width -= 2;
        client_rect.y++;
        client_rect.height = (rect.y + rect.height / 5) - client_rect.x;
        DrawVerticalGradientRectangle(dc, client_rect, 
            m_panel_active_background_top_colour,
            m_panel_active_background_top_gradient_colour);

        client_rect.y += client_rect.height;
        client_rect.height = (true_rect.y + true_rect.height) - client_rect.y;
        DrawVerticalGradientRectangle(dc, client_rect, 
            m_panel_active_background_colour,
            m_panel_active_background_gradient_colour);
    }
    else if(wnd->IsHovered())
    {
        wxRect client_rect(true_rect);
        client_rect.x++;
        client_rect.width -= 2;
        client_rect.y++;
        client_rect.height -= 2;
        DrawPartialPageBackground(dc, wnd, client_rect, true);
    }

    int ypos;
    {
        wxRect preview(0, 0, 32, 32);
        preview.x = rect.x + (rect.width - preview.width) / 2;
        preview.y = rect.y + 5;

        dc.SetBrush(m_panel_hover_label_background_brush);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(preview.x + 1, preview.y + preview.height - 8,
            preview.width - 2, 7);

        int mid_pos = rect.y + rect.height / 5 - preview.y;
        if(mid_pos < 0 || mid_pos >= preview.height)
        {
            wxRect full_rect(preview);
            full_rect.x += 1;
            full_rect.y += 1;
            full_rect.width -= 2;
            full_rect.height -= 9;
            if(mid_pos < 0)
            {
                DrawVerticalGradientRectangle(dc, full_rect,
                    m_page_hover_background_colour,
                    m_page_hover_background_gradient_colour);
            }
            else
            {
                DrawVerticalGradientRectangle(dc, full_rect,
                    m_page_hover_background_top_colour,
                    m_page_hover_background_top_gradient_colour);
            }
        }
        else
        {
            wxRect top_rect(preview);
            top_rect.x += 1;
            top_rect.y += 1;
            top_rect.width -= 2;
            top_rect.height = mid_pos;
            DrawVerticalGradientRectangle(dc, top_rect,
                m_page_hover_background_top_colour,
                m_page_hover_background_top_gradient_colour);

            wxRect btm_rect(top_rect);
            btm_rect.y = preview.y + mid_pos;
            btm_rect.height = preview.y + preview.height - 7 - btm_rect.y;
            DrawVerticalGradientRectangle(dc, btm_rect,
                m_page_hover_background_colour,
                m_page_hover_background_gradient_colour);
        }

        if(bitmap.IsOk())
        {
            dc.DrawBitmap(bitmap, preview.x + (preview.width - bitmap.GetWidth()) / 2,
                preview.y + (preview.height - 7 - bitmap.GetHeight()) / 2, true);
        }

        DrawPanelBorder(dc, preview, m_panel_border_pen, m_panel_border_gradient_pen);

        ypos = preview.y + preview.height + 5;
    }

    wxCoord label_width, label_height;
    dc.SetFont(m_panel_label_font);
    dc.GetTextExtent(wnd->GetLabel(), &label_width, &label_height);
    dc.SetTextForeground(m_panel_minimised_label_colour);
    dc.DrawText(wnd->GetLabel(),
                true_rect.x + (true_rect.width - label_width + 1) / 2,
                ypos);
    ypos += label_height;

    {
        wxPoint arrow_points[3];
        arrow_points[0] = wxPoint(true_rect.width / 2, ypos + 5);
        arrow_points[1] = arrow_points[0] + wxPoint(-3, -3);
        arrow_points[2] = arrow_points[0] + wxPoint( 3, -3);

        dc.SetPen(*wxTRANSPARENT_PEN);
        wxBrush B(m_panel_minimised_label_colour);
        dc.SetBrush(B);
        dc.DrawPolygon(sizeof(arrow_points)/sizeof(wxPoint), arrow_points,
            true_rect.x, true_rect.y);
    }

    DrawPanelBorder(dc, true_rect, m_panel_minimised_border_pen,
        m_panel_minimised_border_gradient_pen);
}

void wxRibbonMSWArtProvider::DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, true);
}

void wxRibbonMSWArtProvider::DrawPartialPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        bool allow_hovered)
{
    // Assume the window is a child of a ribbon page, and also check for a
    // hovered panel somewhere between the window and the page, as it causes
    // the background to change.
    wxPoint offset(wnd->GetPosition());
    wxRibbonPage* page = NULL;
    wxWindow* parent = wnd->GetParent();
    wxRibbonPanel* panel = NULL;
    bool hovered = false;

    panel = wxDynamicCast(wnd, wxRibbonPanel);
    if(panel != NULL)
    {
        hovered = allow_hovered && panel->IsHovered();
        if(panel->GetExpandedDummy() != NULL)
        {
            offset = panel->GetExpandedDummy()->GetPosition();
            parent = panel->GetExpandedDummy()->GetParent();
        }
    }
    for(; parent; parent = parent->GetParent())
    {
        if(panel == NULL)
        {
            panel = wxDynamicCast(parent, wxRibbonPanel);
            if(panel != NULL)
            {
                hovered = allow_hovered && panel->IsHovered();
                if(panel->GetExpandedDummy() != NULL)
                {
                    parent = panel->GetExpandedDummy();
                }
            }
        }
        page = wxDynamicCast(parent, wxRibbonPage);
        if(page != NULL)
        {
            break;
        }
        offset += parent->GetPosition();
    }
    if(page != NULL)
    {
        DrawPartialPageBackground(dc, wnd, rect, page, offset, hovered);
        return;
    }

    // No page found - fallback to painting with a stock brush
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

static bool CanButtonBarLabelBreakAtPosition(const wxString& label, size_t pos);

void wxRibbonMSWArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonBarButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small)
{
    if(state & wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK)
    {
        dc.SetPen(m_button_bar_hover_border_pen);

        wxRect bg_rect(rect);
        bg_rect.x++;
        bg_rect.y++;
        bg_rect.width -= 2;
        bg_rect.height -= 2;

        wxRect bg_rect_top(bg_rect);
        bg_rect_top.height /= 3;
        bg_rect.y += bg_rect_top.height;
        bg_rect.height -= bg_rect_top.height;

        if(kind == wxRIBBON_BUTTONBAR_BUTTON_HYBRID)
        {
            switch(state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmap_large.GetHeight() + 4;
                    wxRect partial_bg(rect);
                    if(state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED)
                    {
                        partial_bg.SetBottom(iYBorder - 1);
                    }
                    else
                    {
                        partial_bg.height -= (iYBorder - partial_bg.y + 1);
                        partial_bg.y = iYBorder + 1;
                    }
                    dc.DrawLine(rect.x, iYBorder, rect.x + rect.width, iYBorder);
                    bg_rect.Intersect(partial_bg);
                    bg_rect_top.Intersect(partial_bg);
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
                {
                    int iArrowWidth = 9;
                    if(state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED)
                    {
                        bg_rect.width -= iArrowWidth;
                        bg_rect_top.width -= iArrowWidth;
                        dc.DrawLine(bg_rect_top.x + bg_rect_top.width,
                            rect.y, bg_rect_top.x + bg_rect_top.width,
                            rect.y + rect.height);
                    }
                    else
                    {
                        --iArrowWidth;
                        bg_rect.x += bg_rect.width - iArrowWidth;
                        bg_rect_top.x += bg_rect_top.width - iArrowWidth;
                        bg_rect.width = iArrowWidth;
                        bg_rect_top.width = iArrowWidth;
                        dc.DrawLine(bg_rect_top.x - 1, rect.y,
                            bg_rect_top.x - 1, rect.y + rect.height);
                    }
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                break;
            }
        }

        DrawVerticalGradientRectangle(dc, bg_rect_top,
            m_button_bar_hover_background_top_colour,
            m_button_bar_hover_background_top_gradient_colour);

        DrawVerticalGradientRectangle(dc, bg_rect,
            m_button_bar_hover_background_colour,
            m_button_bar_hover_background_gradient_colour);

        wxPoint border_points[9];
        border_points[0] = wxPoint(2, 0);
        border_points[1] = wxPoint(rect.width - 3, 0);
        border_points[2] = wxPoint(rect.width - 1, 2);
        border_points[3] = wxPoint(rect.width - 1, rect.height - 3);
        border_points[4] = wxPoint(rect.width - 3, rect.height - 1);
        border_points[5] = wxPoint(2, rect.height - 1);
        border_points[6] = wxPoint(0, rect.height - 3);
        border_points[7] = wxPoint(0, 2);
        border_points[8] = border_points[0];

        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points,
            rect.x, rect.y);
    }

    dc.SetFont(m_button_bar_label_font);
    dc.SetTextForeground(m_button_bar_label_colour);
    switch(state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
    {
    case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
        {
            const int padding = 2;
            dc.DrawBitmap(bitmap_large,
                rect.x + (rect.width - bitmap_large.GetWidth()) / 2,
                rect.y + padding, true);
            int ypos = rect.y + padding + bitmap_large.GetHeight() + padding;
            int arrow_width = kind == wxRIBBON_BUTTONBAR_BUTTON_NORMAL ? 0 : 8;
            wxCoord label_w, label_h;
            dc.GetTextExtent(label, &label_w, &label_h);
            if(label_w + 2 * padding <= rect.width)
            {
                dc.DrawText(label, rect.x + (rect.width - label_w) / 2, ypos);
                if(arrow_width != 0)
                {
                    DrawDropdownArrow(dc, rect.x + rect.width / 2,
                        ypos + (label_h * 3) / 2,
                        m_button_bar_label_colour);
                }
            }
            else
            {
                size_t breaki = label.Len();
                do
                {
                    --breaki;
                    if(CanButtonBarLabelBreakAtPosition(label, breaki))
                    {
                        wxString label_top = label.Mid(0, breaki);
                        dc.GetTextExtent(label_top, &label_w, &label_h);
                        if(label_w + 2 * padding <= rect.width)
                        {
                            dc.DrawText(label_top,
                                rect.x + (rect.width - label_w) / 2, ypos);
                            ypos += label_h;
                            wxString label_bottom = label.Mid(breaki + 1);
                            dc.GetTextExtent(label_bottom, &label_w, &label_h);
                            label_w += arrow_width;
                            int iX = rect.x + (rect.width - label_w) / 2;
                            dc.DrawText(label_bottom, iX, ypos);
                            if(arrow_width != 0)
                            {
                                DrawDropdownArrow(dc, 
                                    iX + 2 +label_w - arrow_width,
                                    ypos + label_h / 2 + 1,
                                    m_button_bar_label_colour);
                            }
                            break;
                        }
                    }
                } while(breaki > 0);
            }
        }
        break;
    case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
        {
            int x_cursor = rect.x + 2;
            dc.DrawBitmap(bitmap_small, x_cursor,
                    rect.y + (rect.height - bitmap_small.GetHeight())/2, true);
            x_cursor += bitmap_small.GetWidth() + 2;
            wxCoord label_w, label_h;
            dc.GetTextExtent(label, &label_w, &label_h);
            dc.DrawText(label, x_cursor,
                rect.y + (rect.height - label_h) / 2);
            x_cursor += label_w + 3;
            if(kind != wxRIBBON_BUTTONBAR_BUTTON_NORMAL)
            {
                DrawDropdownArrow(dc, x_cursor, rect.y + rect.height / 2,
                    m_button_bar_label_colour);
            }
            break;
        }
    default:
        // TODO
        break;
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
                        wxSize client_size,
                        wxPoint* client_offset)
{
    dc.SetFont(m_panel_label_font);
    wxSize label_size = dc.GetTextExtent(wnd->GetLabel());

    client_size.IncBy(0, label_size.GetHeight());
    client_size.IncBy(6, 8); // Guesswork at the moment

    if(client_offset != NULL)
    {
        *client_offset = wxPoint(3, 2);
    }

    return client_size;
}

wxSize wxRibbonMSWArtProvider::GetPanelClientSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* client_offset)
{
    dc.SetFont(m_panel_label_font);
    wxSize label_size = dc.GetTextExtent(wnd->GetLabel());

    size.DecBy(0, label_size.GetHeight());
    size.DecBy(6, 8); // Guesswork at the moment

    if(client_offset != NULL)
    {
        *client_offset = wxPoint(3, 2);
    }

    return size;
}

wxSize wxRibbonMSWArtProvider::GetGallerySize(
                        wxDC& WXUNUSED(dc),
                        const wxRibbonGallery* WXUNUSED(wnd),
                        wxSize client_size)
{
    client_size.IncBy( 2, 2); // Left / top padding
    client_size.IncBy(16, 2); // Right / bototm padding
    return client_size;
}

wxSize wxRibbonMSWArtProvider::GetGalleryClientSize(
                        wxDC& WXUNUSED(dc),
                        const wxRibbonGallery* WXUNUSED(wnd),
                        wxSize size,
                        wxPoint* client_offset,
                        wxRect* scroll_up_button,
                        wxRect* scroll_down_button,
                        wxRect* extension_button)
{
    wxRect scroll_up;
    scroll_up.x = size.GetWidth() - 15;
    scroll_up.width = 15;
    scroll_up.y = 0;
    scroll_up.height = (size.GetHeight() + 2) / 3;
    wxRect scroll_down;
    scroll_down.x = scroll_up.x;
    scroll_down.width = scroll_up.width;
    scroll_down.y = scroll_up.y + scroll_up.height;
    scroll_down.height = scroll_up.height;
    wxRect extension;
    extension.x = scroll_down.x;
    extension.width = scroll_down.width;
    extension.y = scroll_down.y + scroll_down.height;
    extension.height = size.GetHeight() - scroll_up.height - scroll_down.height;
    
    if(client_offset != NULL)
        *client_offset = wxPoint(2, 2);
    if(scroll_up_button != NULL)
        *scroll_up_button = scroll_up;
    if(scroll_down_button != NULL)
        *scroll_down_button = scroll_down;
    if(extension_button != NULL)
        *extension_button = extension;

    size.DecBy( 2, 2);
    size.DecBy(16, 2);
    return size;
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

static bool CanButtonBarLabelBreakAtPosition(const wxString& label, size_t pos)
{
    return label[pos] == ' ';
}

bool wxRibbonMSWArtProvider::GetButtonBarButtonSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonBarButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxSize bitmap_size_large,
                        wxSize bitmap_size_small,
                        wxSize* button_size,
                        wxRect* normal_region,
                        wxRect* dropdown_region)
{
    const int drop_button_width = 8;

    dc.SetFont(m_button_bar_label_font);
    switch(size)
    {
    case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
        // Small bitmap, no label
        *button_size = bitmap_size_small + wxSize(6, 4);
        switch(kind)
        {
        case wxRIBBON_BUTTONBAR_BUTTON_NORMAL:
            *normal_region = wxRect(*button_size);
            *dropdown_region = wxRect(0, 0, 0, 0);
            break;
        case wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN:
            *button_size += wxSize(drop_button_width, 0);
            *dropdown_region = wxRect(*button_size);
            *normal_region = wxRect(0, 0, 0, 0);
            break;
        case wxRIBBON_BUTTONBAR_BUTTON_HYBRID:
            *normal_region = wxRect(*button_size);
            *dropdown_region = wxRect(button_size->GetWidth(), 0,
                drop_button_width, button_size->GetHeight());
            *button_size += wxSize(drop_button_width, 0);
            break;
        }
        break;
    case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
        // Small bitmap, with label to the right
        {
            GetButtonBarButtonSize(dc, wnd, kind, wxRIBBON_BUTTONBAR_BUTTON_SMALL,
                label, bitmap_size_large, bitmap_size_small, button_size,
                normal_region, dropdown_region);
            int text_size = dc.GetTextExtent(label).GetWidth();
            button_size->SetWidth(button_size->GetWidth() + text_size);
            switch(kind)
            {
            case wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN:
                dropdown_region->SetWidth(dropdown_region->GetWidth() + text_size);
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_HYBRID:
                dropdown_region->SetX(dropdown_region->GetX() + text_size);
                // no break
            case wxRIBBON_BUTTONBAR_BUTTON_NORMAL:
                normal_region->SetWidth(normal_region->GetWidth() + text_size);
                break;
            }
            break;
        }
    case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
        // Large bitmap, with label below (possibly split over 2 lines)
        {
            wxSize icon_size(bitmap_size_large);
            icon_size += wxSize(4, 4);
            wxCoord label_height;
            wxCoord best_width;
            dc.GetTextExtent(label, &best_width, &label_height);
            int best_num_lines = 1;
            int last_line_extra_width = 0;
            if(kind != wxRIBBON_BUTTONBAR_BUTTON_NORMAL)
            {
                last_line_extra_width += 8;
                best_num_lines = 2; // label on top line, button below
            }
            size_t i;
            for(i = 0; i < label.Len(); ++i)
            {
                if(CanButtonBarLabelBreakAtPosition(label, i))
                {
                    int width = wxMax(
                        dc.GetTextExtent(label.Mid(0, i - 1)).GetWidth(),
                        dc.GetTextExtent(label.Mid(i + 1)).GetWidth() + last_line_extra_width);
                    if(width < best_width)
                    {
                        best_width = width;
                        best_num_lines = 2;
                    }
                }
            }
            label_height *= 2; // Assume two lines even when only one is used
                               // (to give all buttons a consistent height)
            icon_size.SetWidth(wxMax(icon_size.GetWidth(), best_width) + 6);
            icon_size.SetHeight(icon_size.GetHeight() + label_height);
            *button_size = icon_size;
            switch(kind)
            {
            case wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN:
                *dropdown_region = wxRect(icon_size);
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_HYBRID:
                *normal_region = wxRect(icon_size);
                normal_region->height -= 2 + label_height;
                dropdown_region->x = 0;
                dropdown_region->y = normal_region->height;
                dropdown_region->width = icon_size.GetWidth();
                dropdown_region->height = icon_size.GetHeight() - normal_region->height;
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_NORMAL:
                *normal_region = wxRect(icon_size);
                break;
            }
            break;
        }
    };
    return true;
}

wxSize wxRibbonMSWArtProvider::GetMinimisedPanelMinimumSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desired_bitmap_size,
                        wxDirection* expanded_panel_direction)
{
    if(desired_bitmap_size != NULL)
    {
        *desired_bitmap_size = wxSize(16, 16);
    }
    if(expanded_panel_direction != NULL)
    {
        *expanded_panel_direction = wxSOUTH;
    }
    wxSize base_size(42, 42);

    dc.SetFont(m_panel_label_font);
    wxSize label_size(dc.GetTextExtent(wnd->GetLabel()));
    label_size.IncBy(2, 2); // Allow for differences between this DC and a paint DC
    label_size.IncBy(6, 0); // Padding
    label_size.y *= 2; // Second line for dropdown button

    return wxSize(wxMax(base_size.x, label_size.x), base_size.y + label_size.y);
}

#endif // wxUSE_RIBBON
