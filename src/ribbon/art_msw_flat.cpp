///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_msw_flat.cpp
// Purpose:     Flat MSW style art provider for ribbon interface
// Author:      wxWidgets team
// Created:     2026-02-27
// Copyright:   (c) wxWidgets team
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
#include "wx/dcmemory.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

wxRibbonMSWFlatArtProvider::wxRibbonMSWFlatArtProvider()
    : wxRibbonMSWArtProvider()
{
}

wxRibbonMSWFlatArtProvider::~wxRibbonMSWFlatArtProvider()
{
}

wxRibbonArtProvider* wxRibbonMSWFlatArtProvider::Clone() const
{
    wxRibbonMSWFlatArtProvider *copy = new wxRibbonMSWFlatArtProvider;
    CloneTo(copy);
    return copy;
}

void wxRibbonMSWFlatArtProvider::DrawTab(
                 wxDC& dc,
                 wxWindow* WXUNUSED(wnd),
                 const wxRibbonPageTabInfo& tab)
{
    if ( tab.rect.height <= 2 )
        return;

    if ( tab.active || tab.hovered || tab.highlight )
    {
        if ( tab.active )
        {
            wxRect background(tab.rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 2;

            if ( background.width > 0 && background.height > 0 )
            {
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.SetBrush(wxBrush(m_tab_active_background_colour));
                dc.DrawRectangle(background.x, background.y,
                    background.width, background.height);
            }
        }
        else if ( tab.hovered )
        {
            wxRect background(tab.rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 3;

            if ( background.width > 0 && background.height > 0 )
            {
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.SetBrush(wxBrush(m_tab_hover_background_colour));
                dc.DrawRectangle(background.x, background.y,
                    background.width, background.height);
            }
        }
        else if ( tab.highlight )
        {
            wxRect background(tab.rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 3;

            if ( background.width > 0 && background.height > 0 )
            {
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.SetBrush(wxBrush(m_tab_highlight_colour));
                dc.DrawRectangle(background.x, background.y,
                    background.width, background.height);
            }
        }

        wxPoint border_points[6];
        border_points[0] = wxPoint(1, tab.rect.height - 2);
        border_points[1] = wxPoint(1, 3);
        border_points[2] = wxPoint(3, 1);
        border_points[3] = wxPoint(tab.rect.width - 4, 1);
        border_points[4] = wxPoint(tab.rect.width - 2, 3);
        border_points[5] = wxPoint(tab.rect.width - 2, tab.rect.height - 1);

        dc.SetPen(m_tab_border_pen);
        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points,
            tab.rect.x, tab.rect.y);

        if ( tab.active )
        {
            // Give the tab a curved outward border at the bottom
            dc.DrawPoint(tab.rect.x, tab.rect.y + tab.rect.height - 2);
            dc.DrawPoint(tab.rect.x + tab.rect.width - 1,
                tab.rect.y + tab.rect.height - 2);

            dc.SetPen(wxPen(m_tab_active_background_colour));

            dc.DrawPoint(tab.rect.x + 1, tab.rect.y + tab.rect.height - 2);
            dc.DrawPoint(tab.rect.x + tab.rect.width - 2,
                tab.rect.y + tab.rect.height - 2);
            dc.DrawPoint(tab.rect.x + 1, tab.rect.y + tab.rect.height - 1);
            dc.DrawPoint(tab.rect.x, tab.rect.y + tab.rect.height - 1);
            dc.DrawPoint(tab.rect.x + tab.rect.width - 2,
                tab.rect.y + tab.rect.height - 1);
            dc.DrawPoint(tab.rect.x + tab.rect.width - 1,
                tab.rect.y + tab.rect.height - 1);
        }
    }

    if ( tab.page == nullptr )
        return;

    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
    {
        wxBitmap icon = tab.page->GetIcon();
        if ( icon.IsOk() )
        {
        int x = tab.rect.x + 4;
        if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) == 0 )
            x = tab.rect.x + (tab.rect.width - icon.GetLogicalWidth()) / 2;
        dc.DrawBitmap(icon, x, tab.rect.y + 1 + (tab.rect.height - 1 -
            icon.GetLogicalHeight()) / 2, true);
        }
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
    {
        wxString label = tab.page->GetLabel();
        if ( !label.empty() )
        {
            dc.SetFont(m_tab_label_font);

            if ( tab.active )
            {
                dc.SetTextForeground(m_tab_active_label_colour);
            }
            else if ( tab.hovered )
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
            if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
            {
                const wxBitmap& icon = tab.page->GetIcon();
                if ( icon.IsOk() )
                {
                    const int iconWidth = icon.GetLogicalWidth();
                    x += 3 + iconWidth;
                    width -= 3 + iconWidth;
                }
            }
            int y = tab.rect.y + (tab.rect.height - text_height) / 2;

            if ( width <= text_width )
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

void wxRibbonMSWFlatArtProvider::ReallyDrawTabSeparator(wxWindow* wnd,
    const wxRect& rect, double visibility)
{
    if ( !m_cached_tab_separator.IsOk()
        || m_cached_tab_separator.GetLogicalSize() != rect.GetSize() )
    {
        m_cached_tab_separator = wxBitmap(rect.GetSize());
    }

    wxMemoryDC dc(m_cached_tab_separator);
    DrawTabCtrlBackground(dc, wnd, rect);

    wxCoord x = rect.x + rect.width / 2;

    unsigned char r = (unsigned char)(m_tab_separator_colour.Red() * visibility
        + m_tab_ctrl_background_brush.GetColour().Red() * (1.0 - visibility)
        + 0.5);
    unsigned char g = (unsigned char)(m_tab_separator_colour.Green() * visibility
        + m_tab_ctrl_background_brush.GetColour().Green() * (1.0 - visibility)
        + 0.5);
    unsigned char b = (unsigned char)(m_tab_separator_colour.Blue() * visibility
        + m_tab_ctrl_background_brush.GetColour().Blue() * (1.0 - visibility)
        + 0.5);

    dc.SetPen(wxPen(wxColour(r, g, b)));
    dc.DrawLine(x, rect.y, x, rect.y + rect.height - 1);

    m_cached_tab_separator_visibility = visibility;
}

void wxRibbonMSWFlatArtProvider::DrawPartialPageBackground(wxDC& dc,
        wxWindow* wnd, const wxRect& r, wxRibbonPage* page,
        wxPoint offset, bool hovered)
{
    wxRect background;

    if ( wnd->GetSizer() && wnd->GetParent() != page )
    {
        background = wnd->GetParent()->GetSize();
        offset = wxPoint(0, 0);
    }
    else
    {
        background = page->GetSize();
        page->AdjustRectToIncludeScrollButtons(&background);
        background.height -= 2;
    }
    wxRect paint_rect(r);
    paint_rect.x += offset.x;
    paint_rect.y += offset.y;

    // Clamp vertically to the background but allow full width, as expanded
    // panels can be wider than the page.
    wxRect rect(paint_rect);
    if ( rect.y < background.y )
    {
        rect.height -= (background.y - rect.y);
        rect.y = background.y;
    }
    if ( rect.GetBottom() > background.GetBottom() )
        rect.SetBottom(background.GetBottom());
    rect.x -= offset.x;
    rect.y -= offset.y;

    if ( rect.width > 0 && rect.height > 0 )
    {
        wxColour bg_colour;
        if ( hovered )
            bg_colour = m_page_hover_background_colour;
        else
            bg_colour = m_page_background_colour;

        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(bg_colour));
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
}

void wxRibbonMSWFlatArtProvider::DrawPageBackground(
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

        if ( background.width > 0 && background.height > 0 )
        {
            dc.SetBrush(wxBrush(m_page_background_colour));
            dc.DrawRectangle(background.x, background.y,
                background.width, background.height);
        }
    }

    {
        wxPoint border_points[8];
        border_points[0] = wxPoint(2, 0);
        border_points[1] = wxPoint(1, 1);
        border_points[2] = wxPoint(1, rect.height - 4);
        border_points[3] = wxPoint(3, rect.height - 2);
        border_points[4] = wxPoint(rect.width - 4, rect.height - 2);
        border_points[5] = wxPoint(rect.width - 2, rect.height - 4);
        border_points[6] = wxPoint(rect.width - 2, 1);
        border_points[7] = wxPoint(rect.width - 4, -1);

        dc.SetPen(m_page_border_pen);
        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points,
            rect.x, rect.y);
    }
}

void wxRibbonMSWFlatArtProvider::DrawScrollButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect_,
                        long style)
{
    wxRect rect(rect_);

    if ( (style & wxRIBBON_SCROLL_BTN_FOR_MASK) == wxRIBBON_SCROLL_BTN_FOR_PAGE )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_tab_ctrl_background_brush);
        dc.DrawRectangle(rect);
        dc.SetClippingRegion(rect);
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            rect.x++;
            wxFALLTHROUGH;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            rect.y--;
            rect.width--;
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            rect.x++;
            rect.y--;
            rect.width -= 2;
            rect.height++;
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            rect.x++;
            rect.width -= 2;
            rect.height--;
            break;
        }
    }

    {
        wxRect background(rect);
        background.x++;
        background.y++;
        background.width -= 2;
        background.height -= 2;

        if ( background.width > 0 && background.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(m_page_background_colour));
            dc.DrawRectangle(background.x, background.y,
                background.width, background.height);
        }
    }

    {
        wxPoint border_points[7];
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            border_points[0] = wxPoint(2, 0);
            border_points[1] = wxPoint(rect.width - 1, 0);
            border_points[2] = wxPoint(rect.width - 1, rect.height - 1);
            border_points[3] = wxPoint(2, rect.height - 1);
            border_points[4] = wxPoint(0, rect.height - 3);
            border_points[5] = wxPoint(0, 2);
            break;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            border_points[0] = wxPoint(0, 0);
            border_points[1] = wxPoint(rect.width - 3, 0);
            border_points[2] = wxPoint(rect.width - 1, 2);
            border_points[3] = wxPoint(rect.width - 1, rect.height - 3);
            border_points[4] = wxPoint(rect.width - 3, rect.height - 1);
            border_points[5] = wxPoint(0, rect.height - 1);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            border_points[0] = wxPoint(2, 0);
            border_points[1] = wxPoint(rect.width - 3, 0);
            border_points[2] = wxPoint(rect.width - 1, 2);
            border_points[3] = wxPoint(rect.width - 1, rect.height - 1);
            border_points[4] = wxPoint(0, rect.height - 1);
            border_points[5] = wxPoint(0, 2);
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            border_points[0] = wxPoint(0, 0);
            border_points[1] = wxPoint(rect.width - 1, 0);
            border_points[2] = wxPoint(rect.width - 1, rect.height - 3);
            border_points[3] = wxPoint(rect.width - 3, rect.height - 1);
            border_points[4] = wxPoint(2, rect.height - 1);
            border_points[5] = wxPoint(0, rect.height - 3);
            break;
        }
        border_points[6] = border_points[0];

        dc.SetPen(m_page_border_pen);
        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points,
            rect.x, rect.y);
    }

    {
        // NB: Code for handling hovered/active state is temporary
        wxPoint arrow_points[3];
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            arrow_points[0] = wxPoint(rect.width / 2 - 2, rect.height / 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrow_points[0].y += 1;
            arrow_points[1] = arrow_points[0] + wxPoint(3, -3);
            arrow_points[2] = arrow_points[0] + wxPoint(3,  3);
            break;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            arrow_points[0] = wxPoint(rect.width / 2 + 2, rect.height / 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrow_points[0].y += 1;
            arrow_points[1] = arrow_points[0] - wxPoint(3,  3);
            arrow_points[2] = arrow_points[0] - wxPoint(3, -3);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            arrow_points[0] = wxPoint(rect.width / 2, rect.height / 2 - 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrow_points[0].y += 1;
            arrow_points[1] = arrow_points[0] + wxPoint( 3, 3);
            arrow_points[2] = arrow_points[0] + wxPoint(-3, 3);
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            arrow_points[0] = wxPoint(rect.width / 2, rect.height / 2 + 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrow_points[0].y += 1;
            arrow_points[1] = arrow_points[0] - wxPoint( 3, 3);
            arrow_points[2] = arrow_points[0] - wxPoint(-3, 3);
            break;
        }

        dc.SetPen(*wxTRANSPARENT_PEN);
        wxBrush brush(style & wxRIBBON_SCROLL_BTN_HOVERED
            ? m_tab_active_background_colour : m_tab_label_colour);
        dc.SetBrush(brush);
        dc.DrawPolygon(sizeof(arrow_points)/sizeof(wxPoint), arrow_points,
            rect.x, rect.y);
    }
}

void wxRibbonMSWFlatArtProvider::DrawGalleryButton(wxDC& dc,
                                            wxRect rect,
                                            wxRibbonGalleryButtonState state,
                                            wxBitmap* bitmaps)
{
    wxBitmap btn_bitmap;
    wxColour btn_colour;
    switch ( state )
    {
    case wxRIBBON_GALLERY_BUTTON_NORMAL:
        btn_colour = m_gallery_button_background_colour;
        btn_bitmap = bitmaps[0];
        break;
    case wxRIBBON_GALLERY_BUTTON_HOVERED:
        btn_colour = m_gallery_button_hover_background_colour;
        btn_bitmap = bitmaps[1];
        break;
    case wxRIBBON_GALLERY_BUTTON_ACTIVE:
        btn_colour = m_gallery_button_active_background_colour;
        btn_bitmap = bitmaps[2];
        break;
    case wxRIBBON_GALLERY_BUTTON_DISABLED:
        btn_colour = m_gallery_button_disabled_background_colour;
        btn_bitmap = bitmaps[3];
        break;
    }

    rect.x++;
    rect.y++;
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        rect.width--;
        rect.height -= 2;
    }
    else
    {
        rect.width -= 2;
        rect.height--;
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(btn_colour));
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    dc.DrawBitmap(btn_bitmap, rect.x + rect.width / 2 - 2,
        rect.y + (rect.height / 2) - 2, true);
}

void wxRibbonMSWFlatArtProvider::DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item)
{
    if ( wnd->GetHoveredItem() != item && wnd->GetActiveItem() != item &&
        wnd->GetSelection() != item )
        return;

    dc.SetPen(m_gallery_item_border_pen);
    dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width - 1, rect.y);
    dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
    dc.DrawLine(rect.x + 1, rect.y + rect.height - 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);
    dc.DrawLine(rect.x + rect.width - 1, rect.y + 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);

    wxColour bg_colour;

    if ( wnd->GetActiveItem() == item || wnd->GetSelection() == item )
        bg_colour = m_gallery_button_active_background_colour;
    else
        bg_colour = m_gallery_button_hover_background_colour;

    wxRect inner(rect);
    inner.x += 1;
    inner.width -= 2;
    inner.y += 1;
    inner.height -= 2;
    if ( inner.width > 0 && inner.height > 0 )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(bg_colour));
        dc.DrawRectangle(inner.x, inner.y, inner.width, inner.height);
    }
}

void wxRibbonMSWFlatArtProvider::DrawPanelBorder(wxDC& dc, const wxRect& rect,
                                             wxPen& primary_colour,
                                             wxPen& secondary_colour)
{
    wxUnusedVar(secondary_colour);

    const wxPoint border_points[] = {
        {2, 0},
        {rect.width - 3, 0},
        {rect.width - 1, 2},
        {rect.width - 1, rect.height - 3},
        {rect.width - 3, rect.height - 1},
        {2, rect.height - 1},
        {0, rect.height - 3},
        {0, 2},
        {2, 0},
    };

    dc.SetPen(primary_colour);
    dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points,
        rect.x, rect.y);
}

void wxRibbonMSWFlatArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);

    if ( wnd->GetExpandedPanel() != nullptr )
    {
        wxRect client_rect(true_rect);
        client_rect.x++;
        client_rect.width -= 2;
        client_rect.y++;
        client_rect.height = (true_rect.y + true_rect.height) - client_rect.y;

        if ( client_rect.width > 0 && client_rect.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(m_panel_active_background_colour));
            dc.DrawRectangle(client_rect.x, client_rect.y,
                client_rect.width, client_rect.height);
        }
    }
    else if ( wnd->IsHovered() )
    {
        wxRect client_rect(true_rect);
        client_rect.x++;
        client_rect.width -= 2;
        client_rect.y++;
        client_rect.height -= 2;
        if ( client_rect.width > 0 && client_rect.height > 0 )
            DrawPartialPageBackground(dc, wnd, client_rect, true);
    }

    wxRect preview;
    DrawMinimisedPanelCommon(dc, wnd, true_rect, &preview);

    dc.SetBrush(m_panel_hover_label_background_brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(preview.x + 1, preview.y + preview.height - 8,
        preview.width - 2, 7);

    {
        wxRect full_rect(preview);
        full_rect.x += 1;
        full_rect.y += 1;
        full_rect.width -= 2;
        full_rect.height -= 9;

        if ( full_rect.width > 0 && full_rect.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(m_page_hover_background_colour));
            dc.DrawRectangle(full_rect.x, full_rect.y,
                full_rect.width, full_rect.height);
        }
    }

    if ( bitmap.IsOk() )
    {
        dc.DrawBitmap(bitmap, preview.x + (preview.width -
            bitmap.GetLogicalWidth()) / 2,
            preview.y + (preview.height - 7 -
            bitmap.GetLogicalHeight()) / 2, true);
    }

    if ( !wnd->IsHovered() )
        DrawPanelBorder(dc, preview, m_panel_border_pen,
            m_panel_border_gradient_pen);
    else
        DrawPanelBorder(dc, preview, m_panel_hover_border_pen,
            m_panel_hover_border_gradient_pen);

    DrawPanelBorder(dc, true_rect, m_panel_minimised_border_pen,
        m_panel_minimised_border_gradient_pen);
}

void wxRibbonMSWFlatArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small)
{
    if ( kind == wxRIBBON_BUTTON_TOGGLE )
    {
        kind = wxRIBBON_BUTTON_NORMAL;
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED )
            state ^= wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
    }

    if ( state & (wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK |
        wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK) )
    {
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
            dc.SetPen(m_button_bar_active_border_pen);
        else
            dc.SetPen(m_button_bar_hover_border_pen);

        wxRect bg_rect(rect);
        bg_rect.x++;
        bg_rect.y++;
        bg_rect.width -= 2;
        bg_rect.height -= 2;

        if ( kind == wxRIBBON_BUTTON_HYBRID )
        {
            switch ( state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK )
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmap_large.GetLogicalHeight() + 4;
                    wxRect partial_bg(rect);
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        partial_bg.SetBottom(iYBorder - 1);
                    }
                    else
                    {
                        partial_bg.height -= (iYBorder - partial_bg.y + 1);
                        partial_bg.y = iYBorder + 1;
                    }
                    dc.DrawLine(rect.x, iYBorder, rect.x + rect.width,
                        iYBorder);
                    bg_rect.Intersect(partial_bg);
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
                {
                    int iArrowWidth = 9;
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
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
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                break;
            }
        }

        wxColour bg_colour;
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
            bg_colour = m_button_bar_active_background_colour;
        else
            bg_colour = m_button_bar_hover_background_colour;

        if ( bg_rect.width > 0 && bg_rect.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(bg_colour));
            dc.DrawRectangle(bg_rect.x, bg_rect.y,
                bg_rect.width, bg_rect.height);
        }

        const wxPoint border_points[] = {
            {2, 0},
            {rect.width - 3, 0},
            {rect.width - 1, 2},
            {rect.width - 1, rect.height - 3},
            {rect.width - 3, rect.height - 1},
            {2, rect.height - 1},
            {0, rect.height - 3},
            {0, 2},
            {2, 0},
        };

        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
            dc.SetPen(m_button_bar_active_border_pen);
        else
            dc.SetPen(m_button_bar_hover_border_pen);

        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points,
            rect.x, rect.y);
    }

    dc.SetFont(m_button_bar_label_font);
    dc.SetTextForeground(state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED
                            ? m_button_bar_label_disabled_colour
                            : m_button_bar_label_colour);
    DrawButtonBarButtonForeground(dc, rect, kind, state, label, bitmap_large,
        bitmap_small);
}

void wxRibbonMSWFlatArtProvider::DrawTabSeparator(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility)
{
    if ( visibility <= 0.0 )
    {
        return;
    }
    if ( visibility > 1.0 )
    {
        visibility = 1.0;
    }

    if ( !m_cached_tab_separator.IsOk() || m_cached_tab_separator.GetLogicalSize() != rect.GetSize() || visibility != m_cached_tab_separator_visibility )
    {
        wxRect size(rect.GetSize());
        ReallyDrawTabSeparator(wnd, size, visibility);
    }
    dc.DrawBitmap(m_cached_tab_separator, rect.x, rect.y, false);
}

void wxRibbonMSWFlatArtProvider::DrawPartialPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        bool allow_hovered)
{
    // Assume the window is a child of a ribbon page, and also check for a
    // hovered panel somewhere between the window and the page, as it causes
    // the background to change.
    wxPoint offset(wnd->GetPosition());
    wxRibbonPage* page = nullptr;
    wxWindow* parent = wnd->GetParent();
    wxRibbonPanel* panel = wxDynamicCast(wnd, wxRibbonPanel);
    bool hovered = false;

    if ( panel != nullptr )
    {
        hovered = allow_hovered && panel->IsHovered();
        if ( panel->GetExpandedDummy() != nullptr )
        {
            offset = panel->GetExpandedDummy()->GetPosition();
            parent = panel->GetExpandedDummy()->GetParent();
        }
    }
    for ( ; parent; parent = parent->GetParent() )
    {
        if ( panel == nullptr )
        {
            panel = wxDynamicCast(parent, wxRibbonPanel);
            if ( panel != nullptr )
            {
                hovered = allow_hovered && panel->IsHovered();
                if ( panel->GetExpandedDummy() != nullptr )
                {
                    parent = panel->GetExpandedDummy();
                }
            }
        }
        page = wxDynamicCast(parent, wxRibbonPage);
        if ( page != nullptr )
        {
            break;
        }
        offset += parent->GetPosition();
    }
    if ( page != nullptr )
    {
        DrawPartialPageBackground(dc, wnd, rect, page, offset, hovered);
        return;
    }

    // No page found - fallback to painting with a stock brush
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonMSWFlatArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);
    bool has_ext_button = wnd->HasExtButton();

    int label_height;
    {
        dc.SetFont(m_panel_label_font);
        dc.SetPen(*wxTRANSPARENT_PEN);
        if ( wnd->IsHovered() )
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
        wxString label = wnd->GetLabel();
        bool clip_label = false;
        wxSize label_size(dc.GetTextExtent(label));

        label_rect.SetX(label_rect.GetX() + 1);
        label_rect.SetWidth(label_rect.GetWidth() - 2);
        label_rect.SetHeight(label_size.GetHeight() + 2);
        label_rect.SetY(true_rect.GetBottom() - label_rect.GetHeight());
        label_height = label_rect.GetHeight();

        wxRect label_bg_rect = label_rect;

        if ( has_ext_button )
            label_rect.SetWidth(label_rect.GetWidth() - 13);

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

        dc.DrawRectangle(label_bg_rect);
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
            if ( wnd->IsExtButtonHovered() )
            {
                dc.SetPen(m_panel_hover_button_border_pen);
                dc.SetBrush(m_panel_hover_button_background_brush);
                dc.DrawRoundedRectangle(label_rect.GetRight(), label_rect.GetBottom() - 13, 13, 13, 1.0);
                dc.DrawBitmap(m_panel_extension_bitmap[1], label_rect.GetRight() + 3, label_rect.GetBottom() - 10, true);
            }
            else
                dc.DrawBitmap(m_panel_extension_bitmap[0], label_rect.GetRight() + 3, label_rect.GetBottom() - 10, true);
        }
    }

    if ( wnd->IsHovered() )
    {
        wxRect client_rect(true_rect);
        client_rect.x++;
        client_rect.width -= 2;
        client_rect.y++;
        client_rect.height -= 2 + label_height;
        if ( client_rect.width > 0 && client_rect.height > 0 )
            DrawPartialPageBackground(dc, wnd, client_rect, true);
    }

    if ( !wnd->IsHovered() )
        DrawPanelBorder(dc, true_rect, m_panel_border_pen, m_panel_border_gradient_pen);
    else
        DrawPanelBorder(dc, true_rect, m_panel_hover_border_pen, m_panel_hover_border_gradient_pen);
}

void wxRibbonMSWFlatArtProvider::DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect);

    if ( wnd->IsHovered() )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_gallery_hover_background_brush);
        if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
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
    // Outline
    dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width - 1, rect.y);
    dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
    dc.DrawLine(rect.x + 1, rect.y + rect.height - 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);
    dc.DrawLine(rect.x + rect.width - 1, rect.y + 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);

    DrawGalleryBackgroundCommon(dc, wnd, rect);
}

void wxRibbonMSWFlatArtProvider::DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, true);
}

void wxRibbonMSWFlatArtProvider::DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect);
}

void
wxRibbonMSWFlatArtProvider::DrawToggleButton(wxDC& dc,
                                         wxRibbonBar* wnd,
                                         const wxRect& rect,
                                         wxRibbonDisplayMode mode)
{
    int bindex = 0;
    DrawPartialPageBackground(dc, wnd, rect, false);

    dc.DestroyClippingRegion();
    dc.SetClippingRegion(rect);

    if ( wnd->IsToggleButtonHovered() )
    {
        dc.SetPen(m_ribbon_toggle_pen);
        dc.SetBrush(m_ribbon_toggle_brush);
        dc.DrawRoundedRectangle(rect.GetX(), rect.GetY(), 20, 20, 1.0);
        bindex = 1;
    }
    switch ( mode )
    {
        case wxRIBBON_BAR_PINNED:
            dc.DrawBitmap(m_ribbon_toggle_up_bitmap[bindex], rect.GetX()+7, rect.GetY()+6, true);
            break;
        case wxRIBBON_BAR_MINIMIZED:
            dc.DrawBitmap(m_ribbon_toggle_down_bitmap[bindex], rect.GetX()+7, rect.GetY()+6, true);
            break;
        case wxRIBBON_BAR_EXPANDED:
            dc.DrawBitmap(m_ribbon_toggle_pin_bitmap[bindex], rect.GetX()+4, rect.GetY()+5, true);
            break;
    }
}

void wxRibbonMSWFlatArtProvider::DrawHelpButton(wxDC& dc,
                                       wxRibbonBar* wnd,
                                       const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    dc.DestroyClippingRegion();
    dc.SetClippingRegion(rect);

    if ( wnd->IsHelpButtonHovered() )
    {
        dc.SetPen(m_ribbon_toggle_pen);
        dc.SetBrush(m_ribbon_toggle_brush);
        dc.DrawRoundedRectangle(rect.GetX(), rect.GetY(), 20, 20, 1.0);
        dc.DrawBitmap(m_ribbon_bar_help_button_bitmap[1], rect.GetX()+4, rect.GetY()+5, true);
    }
    else
    {
        dc.DrawBitmap(m_ribbon_bar_help_button_bitmap[0], rect.GetX()+4, rect.GetY()+5, true);
    }
}

void wxRibbonMSWFlatArtProvider::DrawTool(
                wxDC& dc,
                wxWindow* WXUNUSED(wnd),
                const wxRect& rect,
                const wxBitmap& bitmap,
                wxRibbonButtonKind kind,
                long state)
{
    if ( kind == wxRIBBON_BUTTON_TOGGLE )
    {
        if ( state & wxRIBBON_TOOLBAR_TOOL_TOGGLED )
            state ^= wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
    }

    wxRect bg_rect(rect);
    bg_rect.Deflate(1);
    if ( (state & wxRIBBON_TOOLBAR_TOOL_LAST) == 0 )
        bg_rect.width++;
    bool is_split_hybrid = (kind == wxRIBBON_BUTTON_HYBRID && (state &
        (wxRIBBON_TOOLBAR_TOOL_HOVER_MASK | wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)));

    // Background - single solid fill
    wxColour bg_colour = m_tool_background_colour;
    if ( state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK )
        bg_colour = m_tool_active_background_colour;
    else if ( state & wxRIBBON_TOOLBAR_TOOL_HOVER_MASK )
        bg_colour = m_tool_hover_background_colour;

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(bg_colour));
    dc.DrawRectangle(bg_rect.x, bg_rect.y, bg_rect.width, bg_rect.height);

    if ( is_split_hybrid )
    {
        wxRect nonrect(bg_rect);
        if ( state & (wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED |
            wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE) )
        {
            nonrect.width -= 8;
        }
        else
        {
            nonrect.x += nonrect.width - 8;
            nonrect.width = 8;
        }
        wxBrush B(m_tool_hover_background_top_colour);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(B);
        dc.DrawRectangle(nonrect.x, nonrect.y, nonrect.width, nonrect.height);
    }

    // Border
    dc.SetPen(m_toolbar_border_pen);
    if ( state & wxRIBBON_TOOLBAR_TOOL_FIRST )
    {
        dc.DrawPoint(rect.x + 1, rect.y + 1);
        dc.DrawPoint(rect.x + 1, rect.y + rect.height - 2);
    }
    else
        dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);

    if ( state & wxRIBBON_TOOLBAR_TOOL_LAST )
    {
        dc.DrawPoint(rect.x + rect.width - 2, rect.y + 1);
        dc.DrawPoint(rect.x + rect.width - 2, rect.y + rect.height - 2);
    }

    // Foreground
    int avail_width = bg_rect.GetWidth();
    if ( kind & wxRIBBON_BUTTON_DROPDOWN )
    {
        avail_width -= 8;
        if ( is_split_hybrid )
        {
            dc.DrawLine(rect.x + avail_width + 1, rect.y,
                rect.x + avail_width + 1, rect.y + rect.height);
        }
        dc.DrawBitmap(m_toolbar_drop_bitmap, bg_rect.x + avail_width + 2,
            bg_rect.y + (bg_rect.height / 2) - 2, true);
    }
    dc.DrawBitmap(bitmap, bg_rect.x + (avail_width - bitmap.GetLogicalWidth()) / 2,
        bg_rect.y + (bg_rect.height - bitmap.GetLogicalHeight()) / 2, true);
}

#endif // wxUSE_RIBBON
