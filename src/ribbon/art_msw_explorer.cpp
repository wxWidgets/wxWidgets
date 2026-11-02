///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_msw_explorer.cpp
// Purpose:     Windows Explorer style art provider for ribbon interface
// Author:      Juan Antonio Ortega
// Created:     2026-10-29
// Copyright:   (c) 2026 Juan Antonio Ortega
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/ribbon/art_internal.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"

#ifndef WX_PRECOMP
#include "wx/dc.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#elif defined(__WXOSX__)
#include "wx/osx/private.h"
#endif

#include "wx/private/colour.h"

namespace
{

// Size of the panel extension button, in DIPs.
const int PANEL_EXT_BUTTON_SIZE = 13;

// Width of the dropdown part of the toolbar tools, in DIPs.
const int TOOL_DROPDOWN_WIDTH = 8;

} // anonymous namespace

wxRibbonMSWExplorerArtProvider::wxRibbonMSWExplorerArtProvider(int style)
    : wxRibbonAUIArtProvider(),
      m_ribbon_style(style)
{
    wxColour primary, secondary, tertiary;
    GetDefaultExplorerColourScheme(primary, secondary, tertiary);
    SetColourScheme(primary, secondary, tertiary);

    // Unlike the AUI provider, the active tab label is not bold, which also
    // ensures that the tabs width is computed using the font they are drawn
    // with.
    m_tab_active_label_font = m_tab_label_font;

    // Leave room for the separators drawn between the tabs when they don't
    // have enough space, see DrawTabSeparator().
    m_tab_separation_size = 1;
}

wxRibbonMSWExplorerArtProvider::~wxRibbonMSWExplorerArtProvider()
{
}

/* static */
void wxRibbonMSWExplorerArtProvider::GetDefaultExplorerColourScheme(
                         wxColour& primary,
                         wxColour& secondary,
                         wxColour& tertiary)
{
    if ( wxSystemSettings::GetAppearance().IsDark() )
    {
        primary = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        secondary = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
    }
    else
    {
        primary = wxRibbonHSLColour(210.0f, 1.0f / 9.0f, 246.0f / 255.0f).ToRGB();
        secondary = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    }

    tertiary = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
}

void wxRibbonMSWExplorerArtProvider::UpdateColoursFromSystem()
{
    wxColour primary, secondary, tertiary;
    GetDefaultExplorerColourScheme(primary, secondary, tertiary);
    SetColourScheme(primary, secondary, tertiary);
}

wxRibbonArtProvider* wxRibbonMSWExplorerArtProvider::Clone() const
{
    wxRibbonMSWExplorerArtProvider *copy =
        new wxRibbonMSWExplorerArtProvider(m_ribbon_style);
    CloneTo(copy);

    copy->m_tab_hover_border_pen = m_tab_hover_border_pen;
    copy->m_tab_active_border_pen = m_tab_active_border_pen;
    copy->m_button_bar_active_border_pen = m_button_bar_active_border_pen;
    copy->m_minimised_panel_background_brush = m_minimised_panel_background_brush;
    copy->m_gallery_button_background_brush = m_gallery_button_background_brush;

    return copy;
}

void wxRibbonMSWExplorerArtProvider::SetFont(int id, const wxFont& font)
{
    wxRibbonAUIArtProvider::SetFont(id, font);
    if ( id == wxRIBBON_ART_TAB_LABEL_FONT )
    {
        // Undo the bold font set by the base class.
        m_tab_active_label_font = m_tab_label_font;
    }
}

void wxRibbonMSWExplorerArtProvider::SetColourScheme(
                         const wxColour& primary,
                         const wxColour& secondary,
                         const wxColour& tertiary)
{
    wxRibbonHSLColour primary_hsl(primary);
    wxRibbonHSLColour secondary_hsl(secondary);
    wxRibbonHSLColour tertiary_hsl(tertiary);

    // TODO: Remove next line once this provider stops piggybacking MSW
    wxRibbonMSWArtProvider::SetColourScheme(primary, secondary, tertiary);

    const auto LikePrimary = [primary_hsl](float luminance)
        {
            return wxRibbonShiftLuminance(primary_hsl, luminance).ToRGB();
        };
    const auto LikeSecondary = [secondary_hsl](float luminance)
        {
            return wxRibbonShiftLuminance(secondary_hsl, luminance).ToRGB();
        };

    const auto MixColours = [](const wxColour& c1, const wxColour& c2, float ratio)
        {
            wxColour result;
            result.Set(
                static_cast<unsigned char>(c1.Red() * ratio + c2.Red() * (1 - ratio)),
                static_cast<unsigned char>(c1.Green() * ratio + c2.Green() * (1 - ratio)),
                static_cast<unsigned char>(c1.Blue() * ratio + c2.Blue() * (1 - ratio))
            );
            return result;
        };

    m_background_brush = LikePrimary(1.05f);

    wxColour text_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
    wxRibbonHSLColour text_hsl(text_colour);
    wxColour label_colour;

#ifdef __WXOSX__
    label_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
#else
    label_colour = wxSystemSettings::SelectLightDark(
                            text_hsl.Lighter(0.1f).ToRGB(),
                            text_colour);
#endif

    m_tab_label_colour = label_colour;

    wxRibbonHSLColour border_hsl = primary_hsl;
    border_hsl.saturation *= 0.25f;
    border_hsl.luminance -= 27.0f / 255.0f;
    wxColour border_colour = wxSystemSettings::SelectLightDark(border_hsl.ToRGB(), LikeSecondary(1.45378f));

    // These colours must be set before computing m_tab_highlight_colour below.
    m_tab_hover_background_top_colour = primary_hsl.ToRGB();
    m_tab_hover_background_brush = m_tab_hover_background_top_colour;
    m_tab_active_background_colour = primary;

    if (m_ribbon_style & wxRIBBON_EXPLORER_COLORED_TABS_BACKGROUND)
    {
        m_tab_ctrl_background_colour = secondary;
        if (secondary_hsl.luminance < 0.5f)
            m_tab_label_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        m_tab_active_border_pen = m_background_brush.GetColour();
        m_tab_active_label_colour = secondary;
        m_tab_hover_label_colour = m_tab_label_colour;
        m_tab_highlight_colour = secondary_hsl.Lighter(0.1f).ToRGB();
    }
    else
    {
        m_tab_ctrl_background_colour = wxSystemSettings::SelectLightDark(
            wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW),
            wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)
        );
        m_tab_active_border_pen = border_colour;
        m_tab_active_label_colour = m_tab_label_colour;
        m_tab_hover_label_colour = m_ribbon_style & wxRIBBON_EXPLORER_BORDERLESS_HOVERED_TABS ? secondary : m_tab_label_colour;
        m_tab_highlight_colour = wxColour((m_tab_active_background_colour.Red() + m_tab_hover_background_top_colour.Red())/2,
                         (m_tab_active_background_colour.Green() + m_tab_hover_background_top_colour.Green())/2,
                         (m_tab_active_background_colour.Blue()  + m_tab_hover_background_top_colour.Blue())/2);
    }
    m_tab_border_pen = border_colour;
    m_page_border_pen = m_tab_border_pen;
    m_panel_border_pen = border_colour;

    if (m_ribbon_style & wxRIBBON_EXPLORER_COLORED_TABS_BACKGROUND)
    {
        wxRibbonHSLColour tab_hover_background_hsl;
        tab_hover_background_hsl.hue = secondary_hsl.hue;
        tab_hover_background_hsl.saturation = secondary_hsl.saturation * 1.35f;
        tab_hover_background_hsl.luminance = secondary_hsl.luminance * 1.34f;
        m_tab_hover_border_pen = tab_hover_background_hsl.ToRGB();
        m_tab_hover_background_colour = tab_hover_background_hsl.ToRGB();
    }
    else if (m_ribbon_style & wxRIBBON_EXPLORER_BORDERLESS_HOVERED_TABS)
    {
        m_tab_hover_border_pen = *wxTRANSPARENT_PEN;
        m_tab_hover_background_colour = m_tab_ctrl_background_colour;
    }
    else
    {
        wxRibbonHSLColour tab_hover_border_hsl;
        tab_hover_border_hsl.hue = 180;
        tab_hover_border_hsl.saturation = 1.0f / 39.0f;
        tab_hover_border_hsl.luminance = 235.5f / 255.0f;
        m_tab_hover_border_pen = wxSystemSettings::SelectLightDark(tab_hover_border_hsl.ToRGB(), LikeSecondary(1.46218f));

        wxRibbonHSLColour tab_hover_background_hsl;
        tab_hover_background_hsl.hue = 240;
        tab_hover_background_hsl.saturation = 1.0f;
        tab_hover_background_hsl.luminance = 254.0f / 255.0f;
        m_tab_hover_background_colour = wxSystemSettings::SelectLightDark(tab_hover_background_hsl.ToRGB(), LikeSecondary(1.35294f));
    }

    m_panel_label_colour = wxSystemSettings::SelectLightDark(
        text_hsl.Lighter(0.35f).ToRGB(),
        text_colour);
    m_panel_minimised_label_colour = m_panel_label_colour;
    m_panel_hover_label_colour = tertiary_hsl.ToRGB();
    m_page_hover_background_colour = LikePrimary(1.5f);
    m_page_hover_background_gradient_colour = LikePrimary(0.9f);
    m_panel_label_background_colour = LikePrimary(0.85f);
    m_panel_label_background_gradient_colour = LikePrimary(0.97f);
    m_panel_hover_label_background_gradient_colour = secondary_hsl.ToRGB();
    m_panel_hover_label_background_colour = secondary_hsl.AdjustLuminance(0.2f).ToRGB();
    m_button_bar_active_border_pen = wxSystemSettings::SelectLightDark(MixColours(secondary, primary, 0.55f), LikeSecondary(1.1f));
    m_button_bar_hover_border_pen = wxSystemSettings::SelectLightDark(MixColours(secondary, primary, 0.3f), LikeSecondary(1.40336f));

    if (m_ribbon_style & wxRIBBON_EXPLORER_BORDERLESS_BUTTONS)
    {
        m_button_bar_hover_background_brush = m_button_bar_hover_border_pen.GetColour();
        m_button_bar_active_background_brush = m_button_bar_active_border_pen.GetColour();
    }
    else
    {
        m_button_bar_hover_background_brush = wxSystemSettings::SelectLightDark(MixColours(secondary, primary, 0.05f), LikeSecondary(1.40336f));
        m_button_bar_active_background_brush = wxSystemSettings::SelectLightDark(MixColours(secondary, primary, 0.15f), LikeSecondary(1.1f));
    }

    m_panel_hover_button_background_brush = m_button_bar_hover_background_brush;
    m_panel_hover_button_border_pen = m_button_bar_hover_border_pen;
    m_ribbon_toggle_pen = m_button_bar_hover_border_pen;
    m_ribbon_toggle_brush = m_button_bar_hover_background_brush;

    wxRibbonHSLColour minimised_panel_background_hsl = m_button_bar_hover_background_brush.GetColour();
    m_minimised_panel_background_brush = minimised_panel_background_hsl.Lighter(1.1f).ToRGB();

    m_button_bar_label_colour = label_colour;
    m_button_bar_active_label_colour = label_colour;
#ifdef __WXOSX__
    m_button_bar_label_disabled_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT);
#else
    m_button_bar_label_disabled_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
#endif
    m_gallery_border_pen = m_tab_border_pen;
    m_gallery_item_border_pen = m_button_bar_hover_border_pen;
    m_gallery_hover_background_brush = LikePrimary(1.2f);
    m_gallery_button_background_brush = m_page_hover_background_colour;
    m_gallery_button_hover_background_brush = m_button_bar_hover_background_brush;
    m_gallery_button_active_background_brush = m_button_bar_active_background_brush;
    m_gallery_button_disabled_background_brush = primary_hsl.Desaturated(0.15f).ToRGB();
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR, wxSystemSettings::SelectLightDark(secondary_hsl.Desaturated(0.53f).ToRGB(), secondary_hsl.Lighter(0.17f).ToRGB()));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR, wxColour(128, 128, 128));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR, wxSystemSettings::SelectLightDark(secondary_hsl.Desaturated(0.53f).ToRGB(), secondary_hsl.Lighter(0.17f).ToRGB()));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR, wxSystemSettings::SelectLightDark(secondary_hsl.Desaturated(0.53f).ToRGB(), secondary_hsl.Lighter(0.17f).ToRGB()));
    m_toolbar_border_pen = m_tab_border_pen;
    SetColour(wxRIBBON_ART_TOOLBAR_FACE_COLOUR, m_button_bar_label_colour);
    m_tool_background_colour = m_page_hover_background_colour;
    m_tool_background_gradient_colour = m_page_hover_background_gradient_colour;
    m_toolbar_hover_borden_pen = m_button_bar_hover_border_pen;
    m_tool_hover_background_brush = m_button_bar_hover_background_brush;
    m_tool_active_background_brush = m_button_bar_active_background_brush;
}

void wxRibbonMSWExplorerArtProvider::DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    wxRect gradient_rect(rect);
    gradient_rect.height--;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(m_tab_ctrl_background_colour));
    dc.DrawRectangle(gradient_rect);
    dc.SetPen(m_tab_border_pen);
    dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());
}

void wxRibbonMSWExplorerArtProvider::DrawTab(wxDC& dc,
                 wxWindow* WXUNUSED(wnd),
                 const wxRibbonPageTabInfo& tab)
{
    if (tab.rect.height <= 2)
        return;

    if (tab.active || tab.hovered || tab.highlight)
    {
        wxColour fill = m_tab_active_background_colour;
        wxPen border_pen = m_tab_border_pen;
        if (tab.active)
        {
            fill = m_tab_active_background_colour;
            border_pen = m_tab_active_border_pen;
        }
        else if (tab.hovered)
        {
            fill = m_tab_hover_background_colour;
            border_pen = m_tab_hover_border_pen;
        }
        else if (tab.highlight)
        {
            fill = m_tab_highlight_colour;
        }

        wxRect background(tab.rect);
        background.x += 1;
        background.y += 1;
        background.width -= 2;
        background.height -= 1;

        dc.SetPen(border_pen);
        dc.SetBrush(wxBrush(fill));
        dc.DrawRectangle(background);

        if (tab.active || tab.hovered)
        {
            dc.SetPen(tab.active ? wxPen(m_tab_active_background_colour) : m_tab_border_pen);
            int offsetX = tab.active ? 2 : 1;
            dc.DrawLine(tab.rect.x + offsetX, tab.rect.y + tab.rect.height - 1,
                        tab.rect.x + tab.rect.width - offsetX, tab.rect.y + tab.rect.height - 1);
        }
    }

    if (m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
    {
        wxBitmap icon = tab.page->GetIcon();
        if (icon.IsOk())
        {
            int x = tab.rect.x + 4;
            if ((m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) == 0)
                x = tab.rect.x + (tab.rect.width - icon.GetLogicalWidth()) / 2;
            dc.DrawBitmap(icon, x, tab.rect.y + 1 + (tab.rect.height - 1 -
                icon.GetLogicalHeight()) / 2, true);
        }
    }
    if (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
    {
        wxString label = tab.page->GetLabel();
        if (!label.IsEmpty())
        {
            if (m_ribbon_style & wxRIBBON_EXPLORER_UPPERCASE_TABS)
            {
                label.MakeUpper();
            }
            dc.SetFont(m_tab_label_font);

            if (tab.active)
            {
                dc.SetTextForeground(m_tab_active_label_colour);
            }
            else if (tab.hovered)
            {
                dc.SetTextForeground(m_tab_hover_label_colour);
            }
            else
            {
                dc.SetTextForeground(m_tab_label_colour);
            }

            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

            int text_height;
            int text_width;
            dc.GetTextExtent(label, &text_width, &text_height);
            int width = tab.rect.width - 5;
            int x = tab.rect.x + 3;
            if (m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
            {
                x += 3 + tab.page->GetIcon().GetLogicalWidth();
                width -= 3 + tab.page->GetIcon().GetLogicalWidth();
            }
            int y = tab.rect.y + (tab.rect.height - text_height) / 2;

            if (width <= text_width)
            {
                wxDCClipper clip(dc, x, tab.rect.y, width, tab.rect.height);
                dc.DrawText(label, x, y);
            }
            else
            {
                dc.DrawText(label, x + (width - text_width) / 2 + 1, y);
            }
        }
    }
}

void wxRibbonMSWExplorerArtProvider::GetBarTabWidth(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* small_begin_need_separator,
                        int* small_must_have_separator,
                        int* minimum)
{
    wxString aux_label = label;
    if (m_ribbon_style & wxRIBBON_EXPLORER_UPPERCASE_TABS)
    {
        aux_label.MakeUpper();
    }

    int ideal_width = 0;
    int minimum_width = 0;
    wxRibbonAUIArtProvider::GetBarTabWidth(dc, wnd, aux_label, bitmap,
        &ideal_width, nullptr, nullptr, &minimum_width);

    // As in the native ribbon, start showing the separators as soon as the
    // tabs become narrower than their ideal width and show them completely
    // once most of the padding around the label has gone.
    if(ideal != nullptr)
        *ideal = ideal_width;
    if(small_begin_need_separator != nullptr)
        *small_begin_need_separator = ideal_width;
    if(small_must_have_separator != nullptr)
        *small_must_have_separator = wxMax(minimum_width, ideal_width - 12);
    if(minimum != nullptr)
        *minimum = minimum_width;
}

void wxRibbonMSWExplorerArtProvider::DrawTabSeparator(
                    wxDC& dc,
                    wxWindow* wnd,
                    const wxRect& rect,
                    double visibility)
{
    if(visibility <= 0.0 || rect.width <= 0)
        return;
    if(visibility > 1.0)
        visibility = 1.0;

    // Fade the separator in by blending it with the tabs background.
    const wxColour& bg = m_tab_ctrl_background_colour;
    const wxColour& fg = m_tab_border_pen.GetColour();
    const auto Blend = [visibility](unsigned char c1, unsigned char c2)
        {
            return static_cast<unsigned char>(c1 * visibility + c2 * (1.0 - visibility) + 0.5);
        };
    const wxColour colour(Blend(fg.Red(), bg.Red()),
                          Blend(fg.Green(), bg.Green()),
                          Blend(fg.Blue(), bg.Blue()));

    // The separator doesn't extend to the top and bottom borders of the tab.
    const int margin = wnd ? wnd->FromDIP(4) : 4;
    const int x = rect.x + rect.width / 2;

    dc.SetPen(colour);
    dc.DrawLine(x, rect.y + margin, x, rect.GetBottom() - margin + 1);
}

void wxRibbonMSWExplorerArtProvider::DrawPageBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_background_brush);
    dc.DrawRectangle(rect.x + 1, rect.y, rect.width - 2, rect.height - 1);

    dc.SetPen(m_page_border_pen);
    dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height);
    dc.DrawLine(rect.GetRight(), rect.y, rect.GetRight(), rect.y +rect.height);
    dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());
}

wxSize wxRibbonMSWExplorerArtProvider::GetScrollButtonMinimumSize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        wxWindow* wnd,
                        long WXUNUSED(style))
{
    const wxSize size(11, 11);
    return wnd ? wnd->FromDIP(size) : size;
}

void wxRibbonMSWExplorerArtProvider::DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style)
{
    wxRect true_rect(rect);
    wxPoint arrow_points[3];

    // Half of the arrow base length, which is also its height, limited to fit
    // inside the button.
    int arrow = wnd ? wnd->FromDIP(5) : 5;
    arrow = wxMin(arrow, (wxMin(rect.width, rect.height) - 1) / 2);

    if((style & wxRIBBON_SCROLL_BTN_FOR_MASK) == wxRIBBON_SCROLL_BTN_FOR_TABS)
    {
        true_rect.y += 2;
        true_rect.height -= 2;
        dc.SetPen(m_tab_border_pen);
    }
    else
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_background_brush);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
        dc.SetPen(m_page_border_pen);
    }

    switch(style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK)
    {
    case wxRIBBON_SCROLL_BTN_LEFT:
        dc.DrawLine(true_rect.GetRight(), true_rect.y, true_rect.GetRight(),
            true_rect.y + true_rect.height);
        arrow_points[0] = wxPoint(rect.width / 2 - arrow / 2, rect.height / 2);
        arrow_points[1] = arrow_points[0] + wxPoint(arrow, -arrow);
        arrow_points[2] = arrow_points[0] + wxPoint(arrow,  arrow);
        break;
    case wxRIBBON_SCROLL_BTN_RIGHT:
        dc.DrawLine(true_rect.x, true_rect.y, true_rect.x,
            true_rect.y + true_rect.height);
        arrow_points[0] = wxPoint(rect.width / 2 + (arrow + 1) / 2, rect.height / 2);
        arrow_points[1] = arrow_points[0] - wxPoint(arrow, -arrow);
        arrow_points[2] = arrow_points[0] - wxPoint(arrow,  arrow);
        break;
    case wxRIBBON_SCROLL_BTN_DOWN:
        dc.DrawLine(true_rect.x, true_rect.y, true_rect.x + true_rect.width,
            true_rect.y);
        arrow_points[0] = wxPoint(rect.width / 2, rect.height / 2 + (arrow + 1) / 2);
        arrow_points[1] = arrow_points[0] - wxPoint( arrow, arrow);
        arrow_points[2] = arrow_points[0] - wxPoint(-arrow, arrow);
        break;
    case wxRIBBON_SCROLL_BTN_UP:
        dc.DrawLine(true_rect.x, true_rect.GetBottom(),
            true_rect.x + true_rect.width, true_rect.GetBottom());
        arrow_points[0] = wxPoint(rect.width / 2, rect.height / 2 - arrow / 2);
        arrow_points[1] = arrow_points[0] + wxPoint( arrow, arrow);
        arrow_points[2] = arrow_points[0] + wxPoint(-arrow, arrow);
        break;
    default:
        return;
    }

    int x = rect.x;
    int y = rect.y;
    if(style & wxRIBBON_SCROLL_BTN_ACTIVE)
    {
        ++x;
        ++y;
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(m_tab_label_colour));
    dc.DrawPolygon(WXSIZEOF(arrow_points), arrow_points, x, y);
}

wxSize wxRibbonMSWExplorerArtProvider::GetPanelSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize client_size,
                        wxPoint* client_offset)
{
    dc.SetFont(m_panel_label_font);
    wxSize label_size = dc.GetTextExtent(wnd->GetLabel());

    client_size.IncBy(0, label_size.GetHeight());
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
        client_size.IncBy(wnd->FromDIP(wxSize(4, 8)));
    else
        client_size.IncBy(wnd->FromDIP(wxSize(10, 6)));

    if(client_offset != nullptr)
    {
        if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
            *client_offset = wnd->FromDIP(wxPoint(2, 3));
        else
            *client_offset = wnd->FromDIP(wxPoint(3, 2));
    }

    return client_size;
}

wxSize wxRibbonMSWExplorerArtProvider::GetPanelClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* client_offset)
{
    dc.SetFont(m_panel_label_font);
    wxSize label_size = dc.GetTextExtent(wnd->GetLabel());

    size.DecBy(0, label_size.GetHeight());
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
        size.DecBy(wnd->FromDIP(wxSize(4, 8)));
    else
        size.DecBy(wnd->FromDIP(wxSize(10, 6)));

    if(client_offset != nullptr)
    {
        if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
            *client_offset = wnd->FromDIP(wxPoint(2, 3));
        else
            *client_offset = wnd->FromDIP(wxPoint(3, 2));
    }
    if (size.x < 0) size.x = 0;
    if (size.y < 0) size.y = 0;

    return size;
}

wxRect wxRibbonMSWExplorerArtProvider::GetPanelExtButtonArea(wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxRect rect)
{
    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);

    // Use the same font as DrawPanelBackground() to get the same label height.
    dc.SetFont(m_panel_label_font);

    wxRect label_rect(true_rect);
    wxString label = wnd->GetLabel();
    wxSize label_size(dc.GetTextExtent(label));

    label_rect.SetX(label_rect.GetX() + 1);
    label_rect.SetWidth(label_rect.GetWidth() - 2);
    label_rect.SetHeight(label_size.GetHeight() + 2);
    label_rect.SetY(true_rect.GetBottom() - label_rect.GetHeight());

    const int button_size = wnd->FromDIP(PANEL_EXT_BUTTON_SIZE);
    return wxRect(label_rect.GetRight() - button_size,
                  label_rect.GetBottom() - button_size,
                  button_size, button_size);
}

void wxRibbonMSWExplorerArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);

    if (wnd != nullptr && wnd->GetExpandedDummy() != nullptr)
    {
        dc.SetPen(m_panel_border_pen);
        dc.SetBrush(m_background_brush);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    else
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_background_brush);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

        dc.SetPen(m_panel_border_pen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawLine(true_rect.GetRight(), true_rect.y + 2, true_rect.GetRight(), true_rect.GetBottom() - 2);
    }

    bool has_ext_button = wnd->HasExtButton();
    const int ext_button_size = wnd->FromDIP(PANEL_EXT_BUTTON_SIZE);

    {
        dc.SetFont(m_panel_label_font);
        dc.SetTextForeground(m_panel_label_colour);

        wxRect label_rect(true_rect);
        wxString label = wnd->GetLabel();
        bool clip_label = false;
        wxSize label_size(dc.GetTextExtent(label));

        label_rect.SetX(label_rect.GetX() + 1);
        label_rect.SetWidth(label_rect.GetWidth() - 2);
        label_rect.SetHeight(label_size.GetHeight() + 2);
        label_rect.SetY(true_rect.GetBottom() - label_rect.GetHeight());

        if ( has_ext_button )
            label_rect.SetWidth(label_rect.GetWidth() - ext_button_size);

        if ( label_size.GetWidth() > label_rect.GetWidth() )
        {
            // Test if there is enough length for 3 letters and ...
            wxString new_label = label.Mid(0, 3) + "...";
            label_size = dc.GetTextExtent(new_label);
            if ( label_size.GetWidth() > label_rect.GetWidth() )
            {
                // Not enough room for three characters and ...
                // Display the entire label and just crop it
                clip_label = true;
            }
            else
            {
                // Room for some characters and ...
                // Display as many characters as possible and append ...
                for ( int len = static_cast<int>(label.Len()) - 1; len >= 3; --len )
                {
                    new_label = label.Mid(0, len) + "...";
                    label_size = dc.GetTextExtent(new_label);
                    if ( label_size.GetWidth() <= label_rect.GetWidth() )
                    {
                        label = new_label;
                        break;
                    }
                }
            }
        }

        if ( clip_label )
        {
            wxDCClipper clip(dc, label_rect);
            dc.DrawText(label, label_rect.x, label_rect.y +
                (label_rect.GetHeight() - label_size.GetHeight()) / 2);
        }
        else
        {
            dc.DrawText(label, label_rect.x +
                (label_rect.GetWidth() - label_size.GetWidth()) / 2,
                label_rect.y +
                (label_rect.GetHeight() - label_size.GetHeight()) / 2);
        }

        if ( has_ext_button )
        {
            // This must be the same rectangle as returned by
            // GetPanelExtButtonArea().
            const wxRect ext_rect(label_rect.GetRight(),
                                  label_rect.GetBottom() - ext_button_size,
                                  ext_button_size, ext_button_size);

            const bool hovered = wnd->IsExtButtonHovered();
            if ( hovered )
            {
                dc.SetPen(m_panel_hover_button_border_pen);
                dc.SetBrush(m_panel_hover_button_background_brush);
                dc.DrawRectangle(ext_rect);
            }

            const wxBitmap ext_bitmap =
                m_panel_extension_bundle[hovered ? 1 : 0].GetBitmapFor(wnd);
            dc.DrawBitmap(ext_bitmap,
                ext_rect.x + (ext_rect.width - ext_bitmap.GetLogicalWidth()) / 2,
                ext_rect.y + (ext_rect.height - ext_bitmap.GetLogicalHeight()) / 2,
                true);
        }
    }
}

void wxRibbonMSWExplorerArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_background_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);

    dc.SetPen(m_button_bar_hover_border_pen);

    if (wnd->GetExpandedPanel())
    {
        dc.SetBrush(m_button_bar_active_background_brush);
    }
    else if (wnd->IsHovered())
    {
        dc.SetBrush(m_minimised_panel_background_brush);
    }
    else
    {
        dc.SetBrush(m_button_bar_hover_background_brush);
    }
    dc.DrawRectangle(true_rect.x, true_rect.y + 2, true_rect.width, true_rect.height - 4);
    true_rect.Deflate(1);

    wxRect preview;
    DrawMinimisedPanelCommon(dc, wnd, true_rect, &preview);

    dc.SetPen(m_button_bar_hover_border_pen);
    dc.SetBrush(m_minimised_panel_background_brush);
    dc.DrawRectangle(preview.x, preview.y, preview.width, preview.height);
    preview.Deflate(1);

    if(bitmap.IsOk())
    {
        dc.DrawBitmap(bitmap, preview.x + (preview.width - bitmap.GetLogicalWidth()) / 2,
            preview.y + (preview.height - bitmap.GetLogicalHeight()) / 2, true);
    }
}

void wxRibbonMSWExplorerArtProvider::DrawPartialPanelBackground(wxDC& dc,
        wxWindow* WXUNUSED(wnd), const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_background_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonMSWExplorerArtProvider::DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);

    if(wnd->IsHovered())
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_gallery_hover_background_brush);
        if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
        {
            dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 2,
                rect.height - 16);
        }
        else
        {
            dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 16,
                rect.height - 2);
        }
    }

    dc.SetPen(m_gallery_border_pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    DrawGalleryBackgroundCommon(dc, wnd, rect);
}

void wxRibbonMSWExplorerArtProvider::DrawGalleryButton(wxDC& dc, wxRect rect,
        wxRibbonGalleryButtonState state, wxBitmapBundle* bundles, wxWindow* wnd)
{
    int extra_height = 0;
    int extra_width = 0;
    wxRect reduced_rect(rect);
    reduced_rect.Deflate(1);
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        reduced_rect.width++;
        extra_width = 1;
    }
    else
    {
        reduced_rect.height++;
        extra_height = 1;
    }

    wxBitmap btn_bitmap;
    switch(state)
    {
    case wxRIBBON_GALLERY_BUTTON_NORMAL:
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_gallery_button_background_brush);
        dc.DrawRectangle(reduced_rect.x, reduced_rect.y, reduced_rect.width,
            reduced_rect.height);
        btn_bitmap = bundles[0].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_HOVERED:
        dc.SetPen(m_gallery_item_border_pen);
        dc.SetBrush(m_gallery_button_hover_background_brush);
        dc.DrawRectangle(rect.x, rect.y, rect.width + extra_width,
            rect.height + extra_height);
        btn_bitmap = bundles[1].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_ACTIVE:
        dc.SetPen(m_gallery_item_border_pen);
        dc.SetBrush(m_gallery_button_active_background_brush);
        dc.DrawRectangle(rect.x, rect.y, rect.width + extra_width,
            rect.height + extra_height);
        btn_bitmap = bundles[2].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_DISABLED:
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_gallery_button_disabled_background_brush);
        dc.DrawRectangle(reduced_rect.x, reduced_rect.y, reduced_rect.width,
            reduced_rect.height);
        btn_bitmap = bundles[3].GetBitmapFor(wnd);
        break;
    }

    dc.DrawBitmap(btn_bitmap,
        reduced_rect.x + (reduced_rect.width - btn_bitmap.GetLogicalWidth()) / 2,
        rect.y + (rect.height - btn_bitmap.GetLogicalHeight()) / 2, true);
}

void wxRibbonMSWExplorerArtProvider::DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item)
{
    if(wnd->GetHoveredItem() != item && wnd->GetActiveItem() != item &&
        wnd->GetSelection() != item)
        return;

    dc.SetPen(m_gallery_item_border_pen);
    if(wnd->GetActiveItem() == item || wnd->GetSelection() == item)
        dc.SetBrush(m_gallery_button_active_background_brush);
    else
        dc.SetBrush(m_gallery_button_hover_background_brush);

    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonMSWExplorerArtProvider::DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);
}

void wxRibbonMSWExplorerArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small)
{
    if(kind == wxRIBBON_BUTTON_TOGGLE)
    {
        kind = wxRIBBON_BUTTON_NORMAL;
        if(state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED)
            state ^= wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
    }

    if(state & (wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK
        | wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK))
    {
        dc.SetPen(m_button_bar_hover_border_pen);

        wxRect bg_rect(rect);
        bg_rect.Deflate(1);

        if(kind == wxRIBBON_BUTTON_HYBRID)
        {
            switch(state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmap_large.GetLogicalHeight() + 4;
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
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                {
                    int iArrowWidth = 9;
                    if(state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED)
                    {
                        bg_rect.width -= iArrowWidth;
                        dc.DrawLine(bg_rect.x + bg_rect.width,
                            rect.y, bg_rect.x + bg_rect.width,
                            rect.y + rect.height);
                    }
                    else
                    {
                        --iArrowWidth;
                        bg_rect.x += bg_rect.width - iArrowWidth;
                        bg_rect.width = iArrowWidth;
                        dc.DrawLine(bg_rect.x - 1, rect.y,
                            bg_rect.x - 1, rect.y + rect.height);
                    }
                }
                break;
            }
        }

        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

        dc.SetPen(*wxTRANSPARENT_PEN);
        if(state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK)
            dc.SetBrush(m_button_bar_active_background_brush);
        else
            dc.SetBrush(m_button_bar_hover_background_brush);
        dc.DrawRectangle(bg_rect.x, bg_rect.y, bg_rect.width, bg_rect.height);
    }

    dc.SetFont(m_button_bar_label_font);
    dc.SetTextForeground(GetButtonBarLabelColour(state));
    DrawButtonBarButtonForeground(dc, rect, kind, state, label, bitmap_large,
        bitmap_small);
}

void wxRibbonMSWExplorerArtProvider::DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);
}

void wxRibbonMSWExplorerArtProvider::DrawToolGroupBackground(
                    wxDC& dc,
                    wxWindow* wnd,
                    const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);
}

void wxRibbonMSWExplorerArtProvider::DrawTool(
            wxDC& dc,
            wxWindow* wnd,
            const wxRect& rect,
            const wxBitmap& bitmap,
            wxRibbonButtonKind kind,
            long state)
{
    if(kind == wxRIBBON_BUTTON_TOGGLE)
    {
        if(state & wxRIBBON_TOOLBAR_TOOL_TOGGLED)
            state ^= wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
    }

    // This must be the same width as used by GetToolSize().
    const int dropdown_width = wnd->FromDIP(TOOL_DROPDOWN_WIDTH);

    wxRect bg_rect(rect);
    bg_rect.Deflate(1);
    if((state & wxRIBBON_TOOLBAR_TOOL_LAST) == 0)
        bg_rect.width++;
    bool is_custom_bg = (state & (wxRIBBON_TOOLBAR_TOOL_HOVER_MASK |
        wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)) != 0;
    bool is_split_hybrid = kind == wxRIBBON_BUTTON_HYBRID && is_custom_bg;

    // Background
    if(is_custom_bg)
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        if ( kind == wxRIBBON_BUTTON_HYBRID )
        {
            wxRect buttonRect(bg_rect);
            buttonRect.width -= dropdown_width; // button without the dropdown arrow area

            wxRect dropdownRect(bg_rect);
            dropdownRect.x += dropdownRect.width - dropdown_width;
            dropdownRect.width = dropdown_width;

            if (state & wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE)
            {
                // dropdown arrow area
                dc.SetBrush(m_tool_active_background_brush);
                dc.DrawRectangle(dropdownRect);
            }
            else if (state & wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED)
            {
                // dropdown arrow area
                dc.SetBrush(m_tool_hover_background_brush);
                dc.DrawRectangle(dropdownRect);
            }

            if (state & wxRIBBON_TOOLBAR_TOOL_NORMAL_ACTIVE)
            {
                // button without the dropdown arrow area
                dc.SetBrush(m_tool_active_background_brush);
                dc.DrawRectangle(buttonRect);
            }
            else if (state & wxRIBBON_TOOLBAR_TOOL_NORMAL_HOVERED)
            {
                // button without the dropdown arrow area
                dc.SetBrush(m_tool_hover_background_brush);
                dc.DrawRectangle(buttonRect);
            }
        }
        else
        {
            if (state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)
                dc.SetBrush(m_tool_active_background_brush);
            else
                dc.SetBrush(m_tool_hover_background_brush);
            dc.DrawRectangle(bg_rect);
        }
    }

    // Border
    if(is_custom_bg)
        dc.SetPen(m_toolbar_hover_borden_pen);
    else
        dc.SetPen(m_toolbar_border_pen);

    if(is_custom_bg)
    {
        wxRect border_rect(bg_rect);
        border_rect.Inflate(1);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(border_rect.x, border_rect.y, border_rect.width,
            border_rect.height);
    }

    // Foreground
    int avail_width = bg_rect.GetWidth();
    if(kind & wxRIBBON_BUTTON_DROPDOWN)
    {
        avail_width -= dropdown_width;
        if(is_split_hybrid)
        {
            dc.DrawLine(rect.x + avail_width + 1, rect.y,
                rect.x + avail_width + 1, rect.y + rect.height);
        }
        const wxBitmap drop_bitmap = m_toolbar_drop_bundle.GetBitmapFor(wnd);
        dc.DrawBitmap(drop_bitmap,
            bg_rect.x + avail_width + (dropdown_width - drop_bitmap.GetLogicalWidth() + 1) / 2,
            bg_rect.y + (bg_rect.height - drop_bitmap.GetLogicalHeight()) / 2, true);
    }
    if ( bitmap.IsOk() )
        dc.DrawBitmap(bitmap, bg_rect.x + (avail_width - bitmap.GetLogicalWidth()) / 2,
            bg_rect.y + (bg_rect.height - bitmap.GetLogicalHeight()) / 2, true);
}

wxSize wxRibbonMSWExplorerArtProvider::GetToolSize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        wxWindow* wnd,
                        wxSize bitmap_size,
                        wxRibbonButtonKind kind,
                        bool WXUNUSED(is_first),
                        bool is_last,
                        wxRect* dropdown_region)
{
    wxSize size(bitmap_size);
    size.IncBy(wnd->FromDIP(wxSize(7, 6)));
    if(is_last)
        size.IncBy(1, 0);
    if(kind & wxRIBBON_BUTTON_DROPDOWN)
    {
        const int dropdown_width = wnd->FromDIP(TOOL_DROPDOWN_WIDTH);
        size.IncBy(dropdown_width, 0);
        if(dropdown_region)
        {
            if(kind == wxRIBBON_BUTTON_DROPDOWN)
                *dropdown_region = size;
            else
                *dropdown_region = wxRect(size.GetWidth() - dropdown_width, 0,
                                          dropdown_width, size.GetHeight());
        }
    }
    else
    {
        if(dropdown_region)
            *dropdown_region = wxRect(0, 0, 0, 0);
    }
    return size;
}

#endif // wxUSE_RIBBON
