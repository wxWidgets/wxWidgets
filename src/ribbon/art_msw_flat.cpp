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
    if ( tab.m_rect.height <= 2 )
        return;

    if ( tab.m_active || tab.m_hovered || tab.m_highlight )
    {
        if ( tab.m_active )
        {
            wxRect background(tab.m_rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 2;

            if ( background.width > 0 && background.height > 0 )
            {
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.SetBrush(wxBrush(m_tabActiveBackgroundColour));
                dc.DrawRectangle(background.x, background.y,
                    background.width, background.height);
            }
        }
        else if ( tab.m_hovered )
        {
            wxRect background(tab.m_rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 3;

            if ( background.width > 0 && background.height > 0 )
            {
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.SetBrush(wxBrush(m_tabHoverBackgroundColour));
                dc.DrawRectangle(background.x, background.y,
                    background.width, background.height);
            }
        }
        else if ( tab.m_highlight )
        {
            wxRect background(tab.m_rect);

            background.x += 2;
            background.y += 2;
            background.width -= 4;
            background.height -= 3;

            if ( background.width > 0 && background.height > 0 )
            {
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.SetBrush(wxBrush(m_tabHighlightColour));
                dc.DrawRectangle(background.x, background.y,
                    background.width, background.height);
            }
        }

        wxPoint borderPoints[6];
        borderPoints[0] = wxPoint(1, tab.m_rect.height - 2);
        borderPoints[1] = wxPoint(1, 3);
        borderPoints[2] = wxPoint(3, 1);
        borderPoints[3] = wxPoint(tab.m_rect.width - 4, 1);
        borderPoints[4] = wxPoint(tab.m_rect.width - 2, 3);
        borderPoints[5] = wxPoint(tab.m_rect.width - 2, tab.m_rect.height - 1);

        dc.SetPen(m_tabBorderPen);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints,
            tab.m_rect.x, tab.m_rect.y);

        if ( tab.m_active )
        {
            // Give the tab a curved outward border at the bottom
            dc.DrawPoint(tab.m_rect.x, tab.m_rect.y + tab.m_rect.height - 2);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 1,
                tab.m_rect.y + tab.m_rect.height - 2);

            dc.SetPen(wxPen(m_tabActiveBackgroundColour));

            dc.DrawPoint(tab.m_rect.x + 1, tab.m_rect.y + tab.m_rect.height - 2);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 2,
                tab.m_rect.y + tab.m_rect.height - 2);
            dc.DrawPoint(tab.m_rect.x + 1, tab.m_rect.y + tab.m_rect.height - 1);
            dc.DrawPoint(tab.m_rect.x, tab.m_rect.y + tab.m_rect.height - 1);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 2,
                tab.m_rect.y + tab.m_rect.height - 1);
            dc.DrawPoint(tab.m_rect.x + tab.m_rect.width - 1,
                tab.m_rect.y + tab.m_rect.height - 1);
        }
    }

    if ( tab.m_page == nullptr )
        return;

    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
    {
        wxBitmap icon = tab.m_page->GetIcon();
        if ( icon.IsOk() )
        {
        int x = tab.m_rect.x + 4;
        if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) == 0 )
            x = tab.m_rect.x + (tab.m_rect.width - icon.GetLogicalWidth()) / 2;
        dc.DrawBitmap(icon, x, tab.m_rect.y + 1 + (tab.m_rect.height - 1 -
            icon.GetLogicalHeight()) / 2, true);
        }
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
    {
        wxString label = tab.m_page->GetLabel();
        if ( !label.empty() )
        {
            dc.SetFont(m_tabLabelFont);

            if ( tab.m_active )
            {
                dc.SetTextForeground(m_tabActiveLabelColour);
            }
            else if ( tab.m_hovered )
            {
                dc.SetTextForeground(m_tabHoverLabelColour);
            }
            else
            {
                dc.SetTextForeground(m_tabLabelColour);
            }

            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

            int textHeight;
            int textWidth;
            dc.GetTextExtent(label, &textWidth, &textHeight);
            int width = tab.m_rect.width - 5;
            int x = tab.m_rect.x + 3;
            if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
            {
                const wxBitmap& icon = tab.m_page->GetIcon();
                if ( icon.IsOk() )
                {
                    const int iconWidth = icon.GetLogicalWidth();
                    x += 3 + iconWidth;
                    width -= 3 + iconWidth;
                }
            }
            int y = tab.m_rect.y + (tab.m_rect.height - textHeight) / 2;

            if ( width <= textWidth )
            {
                wxDCClipper clip(dc, x, tab.m_rect.y, width, tab.m_rect.height);
                dc.DrawText(label, x, y);
            }
            else
            {
                dc.DrawText(label, x + (width - textWidth) / 2 + 1, y);
            }
        }
    }
}

void wxRibbonMSWFlatArtProvider::ReallyDrawTabSeparator(wxWindow* wnd,
    const wxRect& rect, double visibility)
{
    if ( !m_cachedTabSeparator.IsOk()
        || m_cachedTabSeparator.GetLogicalSize() != rect.GetSize() )
    {
        m_cachedTabSeparator = wxBitmap(rect.GetSize());
    }

    wxMemoryDC dc(m_cachedTabSeparator);
    DrawTabCtrlBackground(dc, wnd, rect);

    wxCoord x = rect.x + rect.width / 2;

    unsigned char r = (unsigned char)(m_tabSeparatorColour.Red() * visibility
        + m_tabCtrlBackgroundBrush.GetColour().Red() * (1.0 - visibility)
        + 0.5);
    unsigned char g = (unsigned char)(m_tabSeparatorColour.Green() * visibility
        + m_tabCtrlBackgroundBrush.GetColour().Green() * (1.0 - visibility)
        + 0.5);
    unsigned char b = (unsigned char)(m_tabSeparatorColour.Blue() * visibility
        + m_tabCtrlBackgroundBrush.GetColour().Blue() * (1.0 - visibility)
        + 0.5);

    dc.SetPen(wxPen(wxColour(r, g, b)));
    dc.DrawLine(x, rect.y, x, rect.y + rect.height - 1);

    m_cachedTabSeparatorVisibility = visibility;
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
    wxRect paintRect(r);
    paintRect.x += offset.x;
    paintRect.y += offset.y;

    // Clamp vertically to the background but allow full width, as expanded
    // panels can be wider than the page.
    wxRect rect(paintRect);
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
        wxColour bgColour;
        if ( hovered )
            bgColour = m_pageHoverBackgroundColour;
        else
            bgColour = m_pageBackgroundColour;

        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(bgColour));
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
}

void wxRibbonMSWFlatArtProvider::DrawPageBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_tabCtrlBackgroundBrush);

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
            dc.SetBrush(wxBrush(m_pageBackgroundColour));
            dc.DrawRectangle(background.x, background.y,
                background.width, background.height);
        }
    }

    {
        wxPoint borderPoints[8];
        borderPoints[0] = wxPoint(2, 0);
        borderPoints[1] = wxPoint(1, 1);
        borderPoints[2] = wxPoint(1, rect.height - 4);
        borderPoints[3] = wxPoint(3, rect.height - 2);
        borderPoints[4] = wxPoint(rect.width - 4, rect.height - 2);
        borderPoints[5] = wxPoint(rect.width - 2, rect.height - 4);
        borderPoints[6] = wxPoint(rect.width - 2, 1);
        borderPoints[7] = wxPoint(rect.width - 4, -1);

        dc.SetPen(m_pageBorderPen);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints,
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
        dc.SetBrush(m_tabCtrlBackgroundBrush);
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
            dc.SetBrush(wxBrush(m_pageBackgroundColour));
            dc.DrawRectangle(background.x, background.y,
                background.width, background.height);
        }
    }

    {
        wxPoint borderPoints[7];
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            borderPoints[0] = wxPoint(2, 0);
            borderPoints[1] = wxPoint(rect.width - 1, 0);
            borderPoints[2] = wxPoint(rect.width - 1, rect.height - 1);
            borderPoints[3] = wxPoint(2, rect.height - 1);
            borderPoints[4] = wxPoint(0, rect.height - 3);
            borderPoints[5] = wxPoint(0, 2);
            break;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            borderPoints[0] = wxPoint(0, 0);
            borderPoints[1] = wxPoint(rect.width - 3, 0);
            borderPoints[2] = wxPoint(rect.width - 1, 2);
            borderPoints[3] = wxPoint(rect.width - 1, rect.height - 3);
            borderPoints[4] = wxPoint(rect.width - 3, rect.height - 1);
            borderPoints[5] = wxPoint(0, rect.height - 1);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            borderPoints[0] = wxPoint(2, 0);
            borderPoints[1] = wxPoint(rect.width - 3, 0);
            borderPoints[2] = wxPoint(rect.width - 1, 2);
            borderPoints[3] = wxPoint(rect.width - 1, rect.height - 1);
            borderPoints[4] = wxPoint(0, rect.height - 1);
            borderPoints[5] = wxPoint(0, 2);
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            borderPoints[0] = wxPoint(0, 0);
            borderPoints[1] = wxPoint(rect.width - 1, 0);
            borderPoints[2] = wxPoint(rect.width - 1, rect.height - 3);
            borderPoints[3] = wxPoint(rect.width - 3, rect.height - 1);
            borderPoints[4] = wxPoint(2, rect.height - 1);
            borderPoints[5] = wxPoint(0, rect.height - 3);
            break;
        }
        borderPoints[6] = borderPoints[0];

        dc.SetPen(m_pageBorderPen);
        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints,
            rect.x, rect.y);
    }

    {
        // NB: Code for handling hovered/active state is temporary
        wxPoint arrowPoints[3];
        switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_LEFT:
            arrowPoints[0] = wxPoint(rect.width / 2 - 2, rect.height / 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] + wxPoint(3, -3);
            arrowPoints[2] = arrowPoints[0] + wxPoint(3,  3);
            break;
        case wxRIBBON_SCROLL_BTN_RIGHT:
            arrowPoints[0] = wxPoint(rect.width / 2 + 2, rect.height / 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] - wxPoint(3,  3);
            arrowPoints[2] = arrowPoints[0] - wxPoint(3, -3);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
            arrowPoints[0] = wxPoint(rect.width / 2, rect.height / 2 - 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] + wxPoint( 3, 3);
            arrowPoints[2] = arrowPoints[0] + wxPoint(-3, 3);
            break;
        case wxRIBBON_SCROLL_BTN_DOWN:
            arrowPoints[0] = wxPoint(rect.width / 2, rect.height / 2 + 2);
            if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
                arrowPoints[0].y += 1;
            arrowPoints[1] = arrowPoints[0] - wxPoint( 3, 3);
            arrowPoints[2] = arrowPoints[0] - wxPoint(-3, 3);
            break;
        }

        dc.SetPen(*wxTRANSPARENT_PEN);
        wxBrush brush(style & wxRIBBON_SCROLL_BTN_HOVERED
            ? m_tabActiveBackgroundColour : m_tabLabelColour);
        dc.SetBrush(brush);
        dc.DrawPolygon(sizeof(arrowPoints)/sizeof(wxPoint), arrowPoints,
            rect.x, rect.y);
    }
}

void wxRibbonMSWFlatArtProvider::DrawGalleryButton(wxDC& dc,
                                            wxRect rect,
                                            wxRibbonGalleryButtonState state,
                                            wxBitmapBundle* bundles,
                                            wxWindow* wnd)
{
    wxBitmap btnBitmap;
    wxColour btnColour;
    switch ( state )
    {
    case wxRIBBON_GALLERY_BUTTON_NORMAL:
        btnColour = m_galleryButtonBackgroundColour;
        btnBitmap = bundles[0].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_HOVERED:
        btnColour = m_galleryButtonHoverBackgroundColour;
        btnBitmap = bundles[1].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_ACTIVE:
        btnColour = m_galleryButtonActiveBackgroundColour;
        btnBitmap = bundles[2].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_DISABLED:
        btnColour = m_galleryButtonDisabledBackgroundColour;
        btnBitmap = bundles[3].GetBitmapFor(wnd);
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
    dc.SetBrush(wxBrush(btnColour));
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    dc.DrawBitmap(btnBitmap, rect.x + rect.width / 2 - 2,
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

    dc.SetPen(m_galleryItemBorderPen);
    dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width - 1, rect.y);
    dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
    dc.DrawLine(rect.x + 1, rect.y + rect.height - 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);
    dc.DrawLine(rect.x + rect.width - 1, rect.y + 1, rect.x + rect.width - 1,
        rect.y + rect.height - 1);

    wxColour bgColour;

    if ( wnd->GetActiveItem() == item || wnd->GetSelection() == item )
        bgColour = m_galleryButtonActiveBackgroundColour;
    else
        bgColour = m_galleryButtonHoverBackgroundColour;

    wxRect inner(rect);
    inner.x += 1;
    inner.width -= 2;
    inner.y += 1;
    inner.height -= 2;
    if ( inner.width > 0 && inner.height > 0 )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(bgColour));
        dc.DrawRectangle(inner.x, inner.y, inner.width, inner.height);
    }
}

void wxRibbonMSWFlatArtProvider::DrawPanelBorder(wxDC& dc, const wxRect& rect,
                                             wxPen& primaryColour,
                                             wxPen& secondaryColour)
{
    wxUnusedVar(secondaryColour);

    const wxPoint borderPoints[] = {
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

    dc.SetPen(primaryColour);
    dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints,
        rect.x, rect.y);
}

void wxRibbonMSWFlatArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);

    if ( wnd->GetExpandedPanel() != nullptr )
    {
        wxRect clientRect(trueRect);
        clientRect.x++;
        clientRect.width -= 2;
        clientRect.y++;
        clientRect.height = (trueRect.y + trueRect.height) - clientRect.y;

        if ( clientRect.width > 0 && clientRect.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(m_panelActiveBackgroundColour));
            dc.DrawRectangle(clientRect.x, clientRect.y,
                clientRect.width, clientRect.height);
        }
    }
    else if ( wnd->IsHovered() )
    {
        wxRect clientRect(trueRect);
        clientRect.x++;
        clientRect.width -= 2;
        clientRect.y++;
        clientRect.height -= 2;
        if ( clientRect.width > 0 && clientRect.height > 0 )
            DrawPartialPageBackground(dc, wnd, clientRect, true);
    }

    wxRect preview;
    DrawMinimisedPanelCommon(dc, wnd, trueRect, &preview);

    dc.SetBrush(m_panelHoverLabelBackgroundBrush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(preview.x + 1, preview.y + preview.height - 8,
        preview.width - 2, 7);

    {
        wxRect fullRect(preview);
        fullRect.x += 1;
        fullRect.y += 1;
        fullRect.width -= 2;
        fullRect.height -= 9;

        if ( fullRect.width > 0 && fullRect.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(m_pageHoverBackgroundColour));
            dc.DrawRectangle(fullRect.x, fullRect.y,
                fullRect.width, fullRect.height);
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
        DrawPanelBorder(dc, preview, m_panelBorderPen,
            m_panelBorderGradientPen);
    else
        DrawPanelBorder(dc, preview, m_panelHoverBorderPen,
            m_panelHoverBorderGradientPen);

    DrawPanelBorder(dc, trueRect, m_panelMinimisedBorderPen,
        m_panelMinimisedBorderGradientPen);
}

void wxRibbonMSWFlatArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall)
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
            dc.SetPen(m_buttonBarActiveBorderPen);
        else
            dc.SetPen(m_buttonBarHoverBorderPen);

        wxRect bgRect(rect);
        bgRect.x++;
        bgRect.y++;
        bgRect.width -= 2;
        bgRect.height -= 2;

        if ( kind == wxRIBBON_BUTTON_HYBRID )
        {
            switch ( state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK )
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmapLarge.GetLogicalHeight() + 4;
                    wxRect partialBg(rect);
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        partialBg.SetBottom(iYBorder - 1);
                    }
                    else
                    {
                        partialBg.height -= (iYBorder - partialBg.y + 1);
                        partialBg.y = iYBorder + 1;
                    }
                    dc.DrawLine(rect.x, iYBorder, rect.x + rect.width,
                        iYBorder);
                    bgRect.Intersect(partialBg);
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                {
                    int iArrowWidth = 9;
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        bgRect.width -= iArrowWidth;
                        dc.DrawLine(bgRect.x + bgRect.width,
                            rect.y, bgRect.x + bgRect.width,
                            rect.y + rect.height);
                    }
                    else
                    {
                        --iArrowWidth;
                        bgRect.x += bgRect.width - iArrowWidth;
                        bgRect.width = iArrowWidth;
                        dc.DrawLine(bgRect.x - 1, rect.y,
                            bgRect.x - 1, rect.y + rect.height);
                    }
                }
                break;
            }
        }

        wxColour bgColour;
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
            bgColour = m_buttonBarActiveBackgroundColour;
        else
            bgColour = m_buttonBarHoverBackgroundColour;

        if ( bgRect.width > 0 && bgRect.height > 0 )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(bgColour));
            dc.DrawRectangle(bgRect.x, bgRect.y,
                bgRect.width, bgRect.height);
        }

        const wxPoint borderPoints[] = {
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
            dc.SetPen(m_buttonBarActiveBorderPen);
        else
            dc.SetPen(m_buttonBarHoverBorderPen);

        dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints,
            rect.x, rect.y);
    }

    dc.SetFont(m_buttonBarLabelFont);
    dc.SetTextForeground(state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED
                            ? m_buttonBarLabelDisabledColour
                            : m_buttonBarLabelColour);
    DrawButtonBarButtonForeground(dc, rect, kind, state, label, bitmapLarge,
        bitmapSmall);
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

    if ( !m_cachedTabSeparator.IsOk() || m_cachedTabSeparator.GetLogicalSize() != rect.GetSize() || visibility != m_cachedTabSeparatorVisibility )
    {
        wxRect size(rect.GetSize());
        ReallyDrawTabSeparator(wnd, size, visibility);
    }
    dc.DrawBitmap(m_cachedTabSeparator, rect.x, rect.y, false);
}

void wxRibbonMSWFlatArtProvider::DrawPartialPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        bool allowHovered)
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
        hovered = allowHovered && panel->IsHovered();
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
                hovered = allowHovered && panel->IsHovered();
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

    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);
    bool hasExtButton = wnd->HasExtButton();

    int labelHeight;
    {
        dc.SetFont(m_panelLabelFont);
        dc.SetPen(*wxTRANSPARENT_PEN);
        if ( wnd->IsHovered() )
        {
            dc.SetBrush(m_panelHoverLabelBackgroundBrush);
            dc.SetTextForeground(m_panelHoverLabelColour);
        }
        else
        {
            dc.SetBrush(m_panelLabelBackgroundBrush);
            dc.SetTextForeground(m_panelLabelColour);
        }

        wxRect labelRect(trueRect);
        wxString label = wnd->GetLabel();
        bool clipLabel = false;
        wxSize labelSize(dc.GetTextExtent(label));

        labelRect.SetX(labelRect.GetX() + 1);
        labelRect.SetWidth(labelRect.GetWidth() - 2);
        labelRect.SetHeight(labelSize.GetHeight() + 2);
        labelRect.SetY(trueRect.GetBottom() - labelRect.GetHeight());
        labelHeight = labelRect.GetHeight();

        wxRect labelBgRect = labelRect;

        if ( hasExtButton )
            labelRect.SetWidth(labelRect.GetWidth() - 13);

        if ( labelSize.GetWidth() > labelRect.GetWidth() )
        {
            // Test if there is enough length for 3 letters and ...
            wxString newLabel = label.Mid(0, 3) + "...";
            labelSize = dc.GetTextExtent(newLabel);
            if ( labelSize.GetWidth() > labelRect.GetWidth() )
            {
                // Not enough room for three characters and ...
                // Display the entire label and just crop it
                clipLabel = true;
            }
            else
            {
                // Room for some characters and ...
                // Display as many characters as possible and append ...
                for ( int len = static_cast<int>(label.Len()) - 1; len >= 3; --len )
                {
                    newLabel = label.Mid(0, len) + "...";
                    labelSize = dc.GetTextExtent(newLabel);
                    if ( labelSize.GetWidth() <= labelRect.GetWidth() )
                    {
                        label = newLabel;
                        break;
                    }
                }
            }
        }

        dc.DrawRectangle(labelBgRect);
        if ( clipLabel )
        {
            wxDCClipper clip(dc, labelRect);
            dc.DrawText(label, labelRect.x, labelRect.y +
                (labelRect.GetHeight() - labelSize.GetHeight()) / 2);
        }
        else
        {
            dc.DrawText(label, labelRect.x +
                (labelRect.GetWidth() - labelSize.GetWidth()) / 2,
                labelRect.y +
                (labelRect.GetHeight() - labelSize.GetHeight()) / 2);
        }

        if ( hasExtButton )
        {
            if ( wnd->IsExtButtonHovered() )
            {
                dc.SetPen(m_panelHoverButtonBorderPen);
                dc.SetBrush(m_panelHoverButtonBackgroundBrush);
                dc.DrawRoundedRectangle(labelRect.GetRight(), labelRect.GetBottom() - 13, 13, 13, 1.0);
                dc.DrawBitmap(m_panelExtensionBundle[1].GetBitmapFor(wnd), labelRect.GetRight() + 3, labelRect.GetBottom() - 10, true);
            }
            else
                dc.DrawBitmap(m_panelExtensionBundle[0].GetBitmapFor(wnd), labelRect.GetRight() + 3, labelRect.GetBottom() - 10, true);
        }
    }

    if ( wnd->IsHovered() )
    {
        wxRect clientRect(trueRect);
        clientRect.x++;
        clientRect.width -= 2;
        clientRect.y++;
        clientRect.height -= 2 + labelHeight;
        if ( clientRect.width > 0 && clientRect.height > 0 )
            DrawPartialPageBackground(dc, wnd, clientRect, true);
    }

    if ( !wnd->IsHovered() )
        DrawPanelBorder(dc, trueRect, m_panelBorderPen, m_panelBorderGradientPen);
    else
        DrawPanelBorder(dc, trueRect, m_panelHoverBorderPen, m_panelHoverBorderGradientPen);
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
        dc.SetBrush(m_galleryHoverBackgroundBrush);
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

    dc.SetPen(m_galleryBorderPen);
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
        dc.SetPen(m_ribbonTogglePen);
        dc.SetBrush(m_ribbonToggleBrush);
        dc.DrawRoundedRectangle(rect.GetX(), rect.GetY(), 20, 20, 1.0);
        bindex = 1;
    }
    switch ( mode )
    {
        case wxRIBBON_BAR_PINNED:
            dc.DrawBitmap(m_ribbonToggleUpBundle[bindex].GetBitmapFor(wnd), rect.GetX()+7, rect.GetY()+6, true);
            break;
        case wxRIBBON_BAR_MINIMIZED:
            dc.DrawBitmap(m_ribbonToggleDownBundle[bindex].GetBitmapFor(wnd), rect.GetX()+7, rect.GetY()+6, true);
            break;
        case wxRIBBON_BAR_EXPANDED:
            dc.DrawBitmap(m_ribbonTogglePinBundle[bindex].GetBitmapFor(wnd), rect.GetX()+4, rect.GetY()+5, true);
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
        dc.SetPen(m_ribbonTogglePen);
        dc.SetBrush(m_ribbonToggleBrush);
        dc.DrawRoundedRectangle(rect.GetX(), rect.GetY(), 20, 20, 1.0);
        dc.DrawBitmap(m_ribbonBarHelpButtonBundle[1].GetBitmapFor(wnd), rect.GetX()+4, rect.GetY()+5, true);
    }
    else
    {
        dc.DrawBitmap(m_ribbonBarHelpButtonBundle[0].GetBitmapFor(wnd), rect.GetX()+4, rect.GetY()+5, true);
    }
}

void wxRibbonMSWFlatArtProvider::DrawTool(
                wxDC& dc,
                wxWindow* wnd,
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

    wxRect bgRect(rect);
    bgRect.Deflate(1);
    if ( (state & wxRIBBON_TOOLBAR_TOOL_LAST) == 0 )
        bgRect.width++;
    bool isSplitHybrid = (kind == wxRIBBON_BUTTON_HYBRID && (state &
        (wxRIBBON_TOOLBAR_TOOL_HOVER_MASK | wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)));

    // Background - single solid fill
    wxColour bgColour = m_toolBackgroundColour;
    if ( state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK )
        bgColour = m_toolActiveBackgroundColour;
    else if ( state & wxRIBBON_TOOLBAR_TOOL_HOVER_MASK )
        bgColour = m_toolHoverBackgroundColour;

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(bgColour));
    dc.DrawRectangle(bgRect.x, bgRect.y, bgRect.width, bgRect.height);

    if ( isSplitHybrid )
    {
        wxRect nonrect(bgRect);
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
        wxBrush B(m_toolHoverBackgroundTopColour);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(B);
        dc.DrawRectangle(nonrect.x, nonrect.y, nonrect.width, nonrect.height);
    }

    // Border
    dc.SetPen(m_toolbarBorderPen);
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
    int availWidth = bgRect.GetWidth();
    if ( kind & wxRIBBON_BUTTON_DROPDOWN )
    {
        availWidth -= 8;
        if ( isSplitHybrid )
        {
            dc.DrawLine(rect.x + availWidth + 1, rect.y,
                rect.x + availWidth + 1, rect.y + rect.height);
        }
        dc.DrawBitmap(m_toolbarDropBundle.GetBitmapFor(wnd), bgRect.x + availWidth + 2,
            bgRect.y + (bgRect.height / 2) - 2, true);
    }
    dc.DrawBitmap(bitmap, bgRect.x + (availWidth - bitmap.GetLogicalWidth()) / 2,
        bgRect.y + (bgRect.height - bitmap.GetLogicalHeight()) / 2, true);
}

#endif // wxUSE_RIBBON
