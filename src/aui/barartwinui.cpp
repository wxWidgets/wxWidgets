/////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/barartwinui.cpp
// Purpose:     Implementation of wxAuiWinUIToolBarArt
// Author:      wxWidgets development team
// Created:     2026-08-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_AUI

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/window.h"
#endif

#include "wx/aui/barartwinui.h"

// For the wxAUI_BUTTON_STATE_XXX flags shared with the dock art.
#include "wx/aui/framemanager.h"

// For wxWinUIWindowUsesBackdrop().
#include "wx/winui/winui.h"

#include "wx/dcgraph.h"
#include "wx/graphics.h"

#include <memory>

namespace
{

// Fluent draws its layers as translucent black or white over whatever is
// behind them. A wxDC has no alpha, so the same result is obtained by
// blending here: the tool bar background is what is behind a tool.
wxColour wxAuiWinUIBlend(const wxColour& back, const wxColour& over,
                         double alpha)
{
    const auto mix = [alpha](unsigned char a, unsigned char b)
    {
        return static_cast<unsigned char>(a + (b - a) * alpha + 0.5);
    };

    return wxColour(mix(back.Red(), over.Red()),
                    mix(back.Green(), over.Green()),
                    mix(back.Blue(), over.Blue()));
}

// Corner radius of a command bar tool, in DIPs.
constexpr int wxAuiWinUICornerRadius = 4;

// Length of the accent indicator of a toggled tool, as a fraction of the
// shorter side of the tool.
constexpr double wxAuiWinUIIndicatorFraction = 0.55;

} // anonymous namespace

wxAuiWinUIToolBarArt::wxAuiWinUIToolBarArt()
{
    // A command bar has no gripper and its tools are square-ish; the generic
    // sizes are Win32 ones. These are DIPs, like the values they replace.
    m_separatorSize = 1;
    m_gripperSize = 8;
    m_overflowSize = 24;
    m_dropdownSize = 14;

    wxAuiWinUIToolBarArt::UpdateColoursFromSystem();
}

wxAuiToolBarArt* wxAuiWinUIToolBarArt::Clone()
{
    return new wxAuiWinUIToolBarArt(*this);
}

void wxAuiWinUIToolBarArt::UpdateColoursFromSystem()
{
    wxAuiGenericToolBarArt::UpdateColoursFromSystem();

    m_dark = wxSystemSettings::GetAppearance().IsDark();

    // What Fluent calls the subtle fill layers: the same neutral over the
    // background, at the opacities the platform uses for a command bar.
    const wxColour over = m_dark ? *wxWHITE : *wxBLACK;
    m_hoverColour = wxAuiWinUIBlend(m_baseColour, over, m_dark ? 0.08 : 0.06);
    m_pressedColour = wxAuiWinUIBlend(m_baseColour, over, m_dark ? 0.04 : 0.03);
    m_checkedColour = wxAuiWinUIBlend(m_baseColour, over, m_dark ? 0.11 : 0.09);
    m_dividerColour = wxAuiWinUIBlend(m_baseColour, over, m_dark ? 0.16 : 0.13);

    m_accentColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
}

void wxAuiWinUIToolBarArt::DrawBackground(wxDC& dc,
                                          wxWindow* wnd,
                                          const wxRect& rect)
{
    // A command bar inside a window with the system backdrop must let the
    // material through, and the port's contract for that is to paint the
    // pixels black: DWM substitutes Mica for them. Filling with the surface
    // colour instead would punch an opaque rectangle through the backdrop.
    //
    // Otherwise: flat, like the surface a command bar sits on. The gradient
    // of the generic provider and the Rebar theme of the MSW one are both
    // foreign to this look.
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxWinUIWindowUsesBackdrop(wnd) ? *wxBLACK
                                                       : m_baseColour));
    dc.DrawRectangle(rect);
}

void wxAuiWinUIToolBarArt::DrawPlainBackground(wxDC& dc,
                                               wxWindow* wnd,
                                               const wxRect& rect)
{
    DrawBackground(dc, wnd, rect);
}

wxColour wxAuiWinUIToolBarArt::GetLayerColour(int state, bool sticky) const
{
    if ( state & wxAUI_BUTTON_STATE_DISABLED )
        return wxColour();

    if ( state & wxAUI_BUTTON_STATE_PRESSED )
        return m_pressedColour;

    if ( (state & wxAUI_BUTTON_STATE_HOVER) || sticky )
        return (state & wxAUI_BUTTON_STATE_CHECKED) ? m_checkedColour
                                                    : m_hoverColour;

    if ( state & wxAUI_BUTTON_STATE_CHECKED )
        return m_checkedColour;

    // The rest state of a command bar tool is the bar itself.
    return wxColour();
}

void wxAuiWinUIToolBarArt::DrawToolLayer(wxDC& dc,
                                         wxWindow* wnd,
                                         const wxRect& rect,
                                         int state,
                                         bool sticky)
{
    const wxColour layer = GetLayerColour(state, sticky);
    const bool checked = (state & wxAUI_BUTTON_STATE_CHECKED) &&
                         !(state & wxAUI_BUTTON_STATE_DISABLED);
    if ( !layer.IsOk() && !checked )
        return;

    const int radius = wnd->FromDIP(wxAuiWinUICornerRadius);

    // Rounded corners are only worth their name when they are antialiased,
    // which a plain wxDC on this platform is not. The context is created
    // only for the tool actually being highlighted -- the rest state above
    // returns before this point -- so a bar full of tools does not pay for
    // it. If no context can be created the shape is still drawn, just with
    // hard edges, rather than not drawn at all.
    std::unique_ptr<wxGraphicsContext> gc(
        wxGraphicsContext::CreateFromUnknownDC(dc));
    if ( gc )
        gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);

    if ( layer.IsOk() )
    {
        if ( gc )
        {
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->SetBrush(wxBrush(layer));
            gc->DrawRoundedRectangle(rect.x, rect.y,
                                     rect.width, rect.height, radius);
        }
        else
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(layer));
            dc.DrawRoundedRectangle(rect, radius);
        }
    }

    if ( !checked )
        return;

    // A toggled tool gets the accent indicator of an AppBarToggleButton, not
    // an accent-filled background: the tools of a real application carry
    // colour of their own and would be unreadable over the accent colour.
    const bool vertical = (m_flags & wxAUI_TB_VERTICAL) != 0;
    const int thickness = wxMax(wnd->FromDIP(3), 2);
    const int length = wxMax(
        static_cast<int>((vertical ? rect.height : rect.width) *
                            wxAuiWinUIIndicatorFraction),
        thickness * 2);

    wxRect indicator;
    if ( vertical )
    {
        // A vertical bar puts the indicator on the leading edge.
        indicator = wxRect(rect.x + wnd->FromDIP(1),
                           rect.y + (rect.height - length) / 2,
                           thickness,
                           length);
    }
    else
    {
        indicator = wxRect(rect.x + (rect.width - length) / 2,
                           rect.y + rect.height - thickness - wnd->FromDIP(1),
                           length,
                           thickness);
    }

    if ( gc )
    {
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->SetBrush(wxBrush(m_accentColour));
        gc->DrawRoundedRectangle(indicator.x, indicator.y,
                                 indicator.width, indicator.height,
                                 thickness / 2.0);
    }
    else
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_accentColour));
        dc.DrawRectangle(indicator);
    }
}

void wxAuiWinUIToolBarArt::DrawButton(wxDC& dc,
                                      wxWindow* wnd,
                                      const wxAuiToolBarItem& item,
                                      const wxRect& rect)
{
    DrawToolLayer(dc, wnd, rect, item.GetState(), item.IsSticky());

    // The generic provider owns the bitmap and label layout, including the
    // text orientations and the rotated bitmaps; only the chrome above is
    // ours. Its own state drawing is a no-op once the state layer has been
    // drawn, so pass an item whose state carries no highlight.
    wxAuiToolBarItem plain(item);
    plain.SetState(item.GetState() &
                       (wxAUI_BUTTON_STATE_DISABLED |
                        wxAUI_BUTTON_STATE_HIDDEN));
    plain.SetSticky(false);
    wxAuiGenericToolBarArt::DrawButton(dc, wnd, plain, rect);
}

void wxAuiWinUIToolBarArt::DrawDropDownButton(wxDC& dc,
                                              wxWindow* wnd,
                                              const wxAuiToolBarItem& item,
                                              const wxRect& rect)
{
    DrawToolLayer(dc, wnd, rect, item.GetState(), item.IsSticky());

    wxAuiToolBarItem plain(item);
    plain.SetState(item.GetState() &
                       (wxAUI_BUTTON_STATE_DISABLED |
                        wxAUI_BUTTON_STATE_HIDDEN));
    plain.SetSticky(false);
    wxAuiGenericToolBarArt::DrawDropDownButton(dc, wnd, plain, rect);
}

void wxAuiWinUIToolBarArt::DrawSeparator(wxDC& dc,
                                         wxWindow* wnd,
                                         const wxRect& rect)
{
    // One hairline in the divider colour, inset from both ends: a command bar
    // separator does not run the full height of the bar.
    const bool horizontal = rect.height > rect.width;
    const int inset = wnd->FromDIP(6);

    dc.SetPen(wxPen(m_dividerColour));

    if ( horizontal )
    {
        const int x = rect.x + rect.width / 2;
        dc.DrawLine(x, rect.y + inset, x, rect.y + rect.height - inset);
    }
    else
    {
        const int y = rect.y + rect.height / 2;
        dc.DrawLine(rect.x + inset, y, rect.x + rect.width - inset, y);
    }
}

void wxAuiWinUIToolBarArt::DrawGripper(wxDC& dc,
                                       wxWindow* wnd,
                                       const wxRect& rect)
{
    // The dotted Win32 gripper has no Fluent counterpart; a single hairline
    // reads as a handle without imitating another platform.
    const int inset = wnd->FromDIP(5);

    dc.SetPen(wxPen(m_dividerColour));

    if ( m_flags & wxAUI_TB_VERTICAL )
    {
        const int y = rect.y + rect.height / 2;
        dc.DrawLine(rect.x + inset, y, rect.x + rect.width - inset, y);
    }
    else
    {
        const int x = rect.x + rect.width / 2;
        dc.DrawLine(x, rect.y + inset, x, rect.y + rect.height - inset);
    }
}

void wxAuiWinUIToolBarArt::DrawOverflowButton(wxDC& dc,
                                              wxWindow* wnd,
                                              const wxRect& rect,
                                              int state)
{
    DrawToolLayer(dc, wnd, rect, state, false);

    // Only the chevron is left to the generic provider: its own hover and
    // pressed backgrounds would be drawn over the layer above.
    wxAuiGenericToolBarArt::DrawOverflowButton(dc, wnd, rect, 0);
}

wxSize wxAuiWinUIToolBarArt::GetToolSize(wxReadOnlyDC& dc,
                                         wxWindow* wnd,
                                         const wxAuiToolBarItem& item)
{
    wxSize size = wxAuiGenericToolBarArt::GetToolSize(dc, wnd, item);

    // A command bar tool is a 32 DIP touch target, and a rounded layer needs
    // room around the icon not to clip it.
    const wxSize minimum = wnd->FromDIP(wxSize(32, 32));
    size.IncTo(wxSize(minimum.x, minimum.y));
    return size;
}

#endif // wxUSE_AUI
