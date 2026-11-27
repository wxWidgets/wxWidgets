///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_winui.cpp
// Purpose:     Metrics and remaining drawing of the ribbon shown by WinUI
// Author:      wxWidgets development team
// Created:     2026-11-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// Under wxWinUI the containers of a ribbon -- the strip behind the groups of
// a page, the label and the divider of a group -- are XAML content of their
// windows (see ribbonchromewinui.h), and its commands are WinUI controls.
// What is left to an art provider is to say how much room all that takes, so
// that the layout of wxRibbon, which stays what it is, gives each window the
// room its XAML content needs; and to draw the few things which are not XAML
// yet: the tabs and the small buttons of the bar.
//
// The metrics are those of the Ribbon of the Windows Community Toolkit
// (Labs), RibbonStyle.xaml: a group has 12 pixels of padding on each side and
// 4 above and below, its label is 12 pixels high text under the content,
// groups are separated by a one pixel divider, and nothing is boxed.

#include "wx/wxprec.h"

#if wxUSE_RIBBON && defined(__WXWINUI__)

#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/settings.h"
#endif

#include "wx/fontenum.h"

#include "../winui/private.h"

#include <winrt/Windows.UI.ViewManagement.h>

namespace
{

// Metrics, in DIPs.
const int PANEL_PADDING_X = 12;
const int PANEL_PADDING_Y = 4;
const int PANEL_LABEL_ROW = 20;     // 4 of margin and a line of 12 pixel text
const int PANEL_DIVIDER = 1;
const int PANEL_EXT_BUTTON = 16;
// The height of a SelectorBar, which shows the tabs: the mark of the
// selected one is at its very bottom, and is cut off in anything less.
const int TAB_STRIP_HEIGHT = 48;
const int TAB_PADDING_X = 12;

struct Palette
{
    wxColour textPrimary,
             textSecondary,
             accent,
             solidFill,
             solidStroke;
};

wxColour wxRibbonWinUIAccent(bool dark)
{
    try
    {
        using namespace winrt::Windows::UI::ViewManagement;

        // AccentFillColorDefault: the second lighter shade of the accent
        // colour in the dark theme, the first darker one in the light theme.
        const auto colour = UISettings().GetColorValue(
            dark ? UIColorType::AccentLight2 : UIColorType::AccentDark1);
        return wxColour(colour.R, colour.G, colour.B);
    }
    catch ( const winrt::hresult_error& )
    {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    }
}

// The colours are looked up at each drawing: the theme can change at any
// time, and there is little to draw.
Palette wxRibbonWinUIPalette()
{
    const bool dark = wxSystemSettings::GetAppearance().IsDark();

    Palette palette;
    if ( dark )
    {
        palette.textPrimary = wxColour(0xff, 0xff, 0xff);
        palette.textSecondary = wxColour(0xc8, 0xc8, 0xc8);
        palette.solidFill = wxColour(0x2d, 0x2d, 0x2d);
        palette.solidStroke = wxColour(0x3a, 0x3a, 0x3a);
    }
    else
    {
        palette.textPrimary = wxColour(0x1a, 0x1a, 0x1a);
        palette.textSecondary = wxColour(0x5f, 0x5f, 0x5f);
        palette.solidFill = wxColour(0xff, 0xff, 0xff);
        palette.solidStroke = wxColour(0xe5, 0xe5, 0xe5);
    }
    palette.accent = wxRibbonWinUIAccent(dark);
    return palette;
}

// A glyph of the icon font of Windows, centred in a rectangle.
void wxRibbonWinUIDrawGlyph(wxDC& dc, wxWindow *wnd, const wxRect& rect,
                            wxChar glyph, const wxColour& colour,
                            int sizeDIP = 10)
{
    static const wxString s_face =
        wxFontEnumerator::IsValidFacename("Segoe Fluent Icons")
            ? wxString("Segoe Fluent Icons")
            : wxString("Segoe MDL2 Assets");

    dc.SetFont(wxFontInfo(wxSize(0, wnd->FromDIP(sizeDIP))).FaceName(s_face));
    dc.SetTextForeground(colour);
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    const wxString text(glyph);
    const wxSize extent = dc.GetTextExtent(text);
    dc.DrawText(text,
                rect.x + (rect.width - extent.x) / 2,
                rect.y + (rect.height - extent.y) / 2);
}

} // anonymous namespace

wxRibbonWinUIArtProvider::wxRibbonWinUIArtProvider()
{
    // The text of a tab is the body text of WinUI, 14 pixels; the default
    // font of the port is its caption text, 12 pixels.
    m_tab_label_font = *wxNORMAL_FONT;
    m_tab_label_font.SetFractionalPointSize(
        m_tab_label_font.GetFractionalPointSize() * 14. / 12.);
    m_panel_label_font = *wxNORMAL_FONT;

    // The strip of a page has a line above and below it and 4 pixels of
    // padding inside; groups touch, their dividers are their own.
    m_page_border_left = 0;
    m_page_border_right = 0;
    m_page_border_top = 5;
    m_page_border_bottom = 5;
    m_panel_x_separation_size = 0;
    m_panel_y_separation_size = 0;
    m_tab_separation_size = 4;
}

wxRibbonArtProvider *wxRibbonWinUIArtProvider::Clone() const
{
    wxRibbonWinUIArtProvider * const copy = new wxRibbonWinUIArtProvider;
    CloneTo(copy);
    return copy;
}

// ----------------------------------------------------------------------------
// what XAML shows: nothing to draw
// ----------------------------------------------------------------------------

void wxRibbonWinUIArtProvider::DrawTabCtrlBackground(wxDC& WXUNUSED(dc),
                                                     wxWindow *WXUNUSED(wnd),
                                                     const wxRect& WXUNUSED(rect))
{
    // The tabs lie on the material of the window.
}

void wxRibbonWinUIArtProvider::DrawTabSeparator(wxDC& WXUNUSED(dc),
                                                wxWindow *WXUNUSED(wnd),
                                                const wxRect& WXUNUSED(rect),
                                                double WXUNUSED(visibility))
{
}

void wxRibbonWinUIArtProvider::DrawPageBackground(wxDC& WXUNUSED(dc),
                                                  wxWindow *WXUNUSED(wnd),
                                                  const wxRect& WXUNUSED(rect))
{
}

void wxRibbonWinUIArtProvider::DrawPanelBackground(wxDC& WXUNUSED(dc),
                                                   wxRibbonPanel *WXUNUSED(wnd),
                                                   const wxRect& WXUNUSED(rect))
{
}

void wxRibbonWinUIArtProvider::DrawMinimisedPanel(wxDC& WXUNUSED(dc),
                                                  wxRibbonPanel *WXUNUSED(wnd),
                                                  const wxRect& WXUNUSED(rect),
                                                  wxBitmap& WXUNUSED(bitmap))
{
}

void wxRibbonWinUIArtProvider::DrawGalleryBackground(wxDC& WXUNUSED(dc),
                                                     wxRibbonGallery *WXUNUSED(wnd),
                                                     const wxRect& WXUNUSED(rect))
{
}

void wxRibbonWinUIArtProvider::DrawButtonBarBackground(wxDC& WXUNUSED(dc),
                                                       wxWindow *WXUNUSED(wnd),
                                                       const wxRect& WXUNUSED(rect))
{
}

void wxRibbonWinUIArtProvider::DrawToolBarBackground(wxDC& WXUNUSED(dc),
                                                     wxWindow *WXUNUSED(wnd),
                                                     const wxRect& WXUNUSED(rect))
{
}

void wxRibbonWinUIArtProvider::DrawToolGroupBackground(wxDC& WXUNUSED(dc),
                                                       wxWindow *WXUNUSED(wnd),
                                                       const wxRect& WXUNUSED(rect))
{
}

// ----------------------------------------------------------------------------
// what is still drawn
// ----------------------------------------------------------------------------

void wxRibbonWinUIArtProvider::DrawTab(wxDC& dc, wxWindow *wnd,
                                       const wxRibbonPageTabInfo& tab)
{
    if ( tab.rect.height <= 2 || !tab.page )
        return;

    const Palette palette = wxRibbonWinUIPalette();

    // The item of a selector bar: its text, lit when it is selected or
    // under the pointer, and the accent mark under the selected one.
    wxBitmap icon;
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
        icon = tab.page->GetIcon();
    wxString label;
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
        label = tab.page->GetLabel();

    dc.SetFont(m_tab_label_font);
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.SetTextForeground(tab.active || tab.hovered || tab.highlight
                            ? palette.textPrimary
                            : palette.textSecondary);

    const wxSize text = dc.GetTextExtent(label);
    const int gap = icon.IsOk() && !label.empty() ? wnd->FromDIP(6) : 0;
    const int iconWidth = icon.IsOk() ? icon.GetLogicalWidth() : 0;
    int x = tab.rect.x + (tab.rect.width - iconWidth - gap - text.x) / 2;
    if ( x < tab.rect.x + wnd->FromDIP(4) )
        x = tab.rect.x + wnd->FromDIP(4);

    if ( icon.IsOk() )
    {
        dc.DrawBitmap(icon, x,
                      tab.rect.y +
                          (tab.rect.height - icon.GetLogicalHeight()) / 2,
                      true);
        x += iconWidth + gap;
    }
    if ( !label.empty() )
        dc.DrawText(label, x, tab.rect.y + (tab.rect.height - text.y) / 2);

    if ( tab.active )
    {
        const int width = wnd->FromDIP(16);
        const int height = wnd->FromDIP(3);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(palette.accent));
        dc.DrawRoundedRectangle(
            tab.rect.x + (tab.rect.width - width) / 2,
            tab.rect.GetBottom() - height - wnd->FromDIP(1),
            width, height, height / 2.);
    }
}

void wxRibbonWinUIArtProvider::DrawScrollButton(wxDC& dc, wxWindow *wnd,
                                                const wxRect& rect, long style)
{
    const Palette palette = wxRibbonWinUIPalette();

    // Over what it scrolls: opaque, like the scroll buttons of the ribbon of
    // the Toolkit.
    dc.SetPen(wxPen(palette.solidStroke));
    dc.SetBrush(wxBrush(palette.solidFill));
    dc.DrawRoundedRectangle(rect, wnd->FromDIP(4));

    wxChar glyph = 0;
    switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
    {
        case wxRIBBON_SCROLL_BTN_LEFT:  glyph = 0xEDD9; break;
        case wxRIBBON_SCROLL_BTN_RIGHT: glyph = 0xEDDA; break;
        case wxRIBBON_SCROLL_BTN_UP:    glyph = 0xEDDB; break;
        case wxRIBBON_SCROLL_BTN_DOWN:  glyph = 0xEDDC; break;
    }

    const bool lit = (style & (wxRIBBON_SCROLL_BTN_HOVERED |
                               wxRIBBON_SCROLL_BTN_ACTIVE)) != 0;
    wxRibbonWinUIDrawGlyph(dc, wnd, rect, glyph,
                           lit ? palette.textPrimary : palette.textSecondary,
                           8);
}

void wxRibbonWinUIArtProvider::DrawToggleButton(wxDC& dc, wxRibbonBar *wnd,
                                                const wxRect& rect,
                                                wxRibbonDisplayMode mode)
{
    const Palette palette = wxRibbonWinUIPalette();

    wxChar glyph = 0xE70E;                  // chevron up: fold the ribbon
    if ( mode == wxRIBBON_BAR_MINIMIZED )
        glyph = 0xE70D;                     // chevron down: unfold it
    else if ( mode == wxRIBBON_BAR_EXPANDED )
        glyph = 0xE718;                     // pin: keep it unfolded

    wxRibbonWinUIDrawGlyph(dc, wnd, rect, glyph,
                           wnd->IsToggleButtonHovered()
                               ? palette.textPrimary
                               : palette.textSecondary);
}

void wxRibbonWinUIArtProvider::DrawHelpButton(wxDC& dc, wxRibbonBar *wnd,
                                              const wxRect& rect)
{
    const Palette palette = wxRibbonWinUIPalette();

    wxRibbonWinUIDrawGlyph(dc, wnd, rect, 0xE897,
                           wnd->IsHelpButtonHovered()
                               ? palette.textPrimary
                               : palette.textSecondary,
                           12);
}

// ----------------------------------------------------------------------------
// metrics
// ----------------------------------------------------------------------------

void wxRibbonWinUIArtProvider::GetBarTabWidth(wxReadOnlyDC& dc, wxWindow *wnd,
                                              const wxString& label,
                                              const wxBitmap& bitmap,
                                              int *ideal,
                                              int *small_begin_need_separator,
                                              int *small_must_have_separator,
                                              int *minimum)
{
    int width = 0;
    int min = 0;
    if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) && !label.empty() )
    {
        dc.SetFont(m_tab_label_font);
        width += dc.GetTextExtent(label).GetWidth();
        min += wxMin(wnd->FromDIP(25), width);
        if ( bitmap.IsOk() )
        {
            width += wnd->FromDIP(6);
            min += wnd->FromDIP(2);
        }
    }
    if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS) && bitmap.IsOk() )
    {
        width += bitmap.GetLogicalWidth();
        min += bitmap.GetLogicalWidth();
    }

    const int padding = wnd->FromDIP(TAB_PADDING_X);
    if ( ideal )
        *ideal = width + 2 * padding;
    if ( small_begin_need_separator )
        *small_begin_need_separator = width + padding;
    if ( small_must_have_separator )
        *small_must_have_separator = width + padding / 2;
    if ( minimum )
        *minimum = min;
}

int wxRibbonWinUIArtProvider::GetTabCtrlHeight(wxReadOnlyDC& dc, wxWindow *wnd,
                                               const wxRibbonPageTabInfoArray& pages)
{
    // The base class knows when the tabs are not shown at all: a bar with a
    // single page has none unless wxRIBBON_BAR_ALWAYS_SHOW_TABS asks for
    // them, and it then returns 2, the pixels of its border. With the tabs
    // go the buttons which are at their end (collapse, help).
    const int height = wxRibbonMSWArtProvider::GetTabCtrlHeight(dc, wnd, pages);
    if ( height <= 2 )
        return height;

    return wxMax(height, wnd->FromDIP(TAB_STRIP_HEIGHT));
}

wxSize wxRibbonWinUIArtProvider::GetScrollButtonMinimumSize(wxReadOnlyDC& WXUNUSED(dc),
                                                            wxWindow *wnd,
                                                            long WXUNUSED(style))
{
    return wnd->FromDIP(wxSize(16, 16));
}

wxSize wxRibbonWinUIArtProvider::GetPanelSize(wxReadOnlyDC& WXUNUSED(dc),
                                              const wxRibbonPanel *wnd,
                                              wxSize client_size,
                                              wxPoint *client_offset)
{
    // The divider is at the trailing edge of the group, or under it when
    // the groups are stacked.
    const bool stacked = (m_flags & wxRIBBON_BAR_FLOW_VERTICAL) != 0;
    client_size.IncBy(
        wnd->FromDIP(2 * PANEL_PADDING_X + (stacked ? 0 : PANEL_DIVIDER)),
        wnd->FromDIP(2 * PANEL_PADDING_Y + PANEL_LABEL_ROW +
                     (stacked ? PANEL_DIVIDER : 0)));
    if ( client_offset )
        *client_offset = wnd->FromDIP(wxPoint(PANEL_PADDING_X, PANEL_PADDING_Y));
    return client_size;
}

wxSize wxRibbonWinUIArtProvider::GetPanelClientSize(wxReadOnlyDC& WXUNUSED(dc),
                                                    const wxRibbonPanel *wnd,
                                                    wxSize size,
                                                    wxPoint *client_offset)
{
    const bool stacked = (m_flags & wxRIBBON_BAR_FLOW_VERTICAL) != 0;
    size.DecBy(
        wnd->FromDIP(2 * PANEL_PADDING_X + (stacked ? 0 : PANEL_DIVIDER)),
        wnd->FromDIP(2 * PANEL_PADDING_Y + PANEL_LABEL_ROW +
                     (stacked ? PANEL_DIVIDER : 0)));
    if ( size.x < 0 )
        size.x = 0;
    if ( size.y < 0 )
        size.y = 0;
    if ( client_offset )
        *client_offset = wnd->FromDIP(wxPoint(PANEL_PADDING_X, PANEL_PADDING_Y));
    return size;
}

wxRect wxRibbonWinUIArtProvider::GetPanelExtButtonArea(wxReadOnlyDC& WXUNUSED(dc),
                                                       const wxRibbonPanel *wnd,
                                                       wxRect rect)
{
    // In the row of the label, at its trailing end.
    const int side = wnd->FromDIP(PANEL_EXT_BUTTON);
    const int row = wnd->FromDIP(PANEL_LABEL_ROW);
    return wxRect(rect.GetRight() + 1 - wnd->FromDIP(PANEL_DIVIDER + 4) - side,
                  rect.GetBottom() + 1 - wnd->FromDIP(PANEL_PADDING_Y) - row +
                      (row - side) / 2 + wnd->FromDIP(2),
                  side, side);
}

wxSize wxRibbonWinUIArtProvider::GetMinimisedPanelMinimumSize(
                                    wxReadOnlyDC& dc,
                                    const wxRibbonPanel *wnd,
                                    wxSize *desired_bitmap_size,
                                    wxDirection *expanded_panel_direction)
{
    // A folded group is a button holding the icon of the group and a
    // chevron, over the label of the group.
    if ( desired_bitmap_size )
        *desired_bitmap_size = wnd->FromDIP(wxSize(16, 16));
    if ( expanded_panel_direction )
    {
        *expanded_panel_direction =
            (m_flags & wxRIBBON_BAR_FLOW_VERTICAL) ? wxEAST : wxSOUTH;
    }

    dc.SetFont(m_panel_label_font);
    const wxSize label = dc.GetTextExtent(wnd->GetLabel());
    const int width = wxMin(wxMax(label.x, wnd->FromDIP(40)), wnd->FromDIP(100));
    // High enough for the button: an icon, a chevron and its padding.
    return wxSize(width + wnd->FromDIP(2 * PANEL_PADDING_X + PANEL_DIVIDER),
                  wnd->FromDIP(48 + 2 * PANEL_PADDING_Y + PANEL_LABEL_ROW +
                               PANEL_DIVIDER));
}

#endif // wxUSE_RIBBON && __WXWINUI__
