/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/renderer.cpp
// Purpose:     Fluent-styled wxRendererNative used by the winui toolkit
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// The generic controls which have no WinUI peer (wxListCtrl, wxDataViewCtrl,
// wxGrid, ...) draw their chrome through wxRendererNative.  Install a
// renderer drawing Fluent-looking headers, selections and expanders so that
// the whole generic family fits in with the real WinUI controls without
// having to reimplement each of them on a XAML peer.

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/utils.h"
    #include "wx/window.h"
#endif

#include "wx/renderer.h"
#include "wx/msw/private/metrics.h"

#include "private.h"

namespace
{

// Fluent-ish hover/divider tints: opaque blends of the base colour (a plain
// wxDC ignores the alpha channel of brush colours).
wxColour wxWinUISubtleFill(const wxColour& base)
{
    return base.ChangeLightness(wxWinUIIsDarkTheme() ? 115 : 95);
}

wxColour wxWinUIDividerColour(const wxColour& base)
{
    return base.ChangeLightness(wxWinUIIsDarkTheme() ? 135 : 80);
}

bool wxWinUIWindowUsesBackdrop(wxWindow *win)
{
    for ( HWND hwnd = win ? GetHwndOf(win) : nullptr; hwnd; )
    {
        if ( ::GetPropW(hwnd, L"wxWinUIBackdropTransparent") )
            return true;

        if ( !(::GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD) )
            break;
        hwnd = ::GetParent(hwnd);
    }

    return false;
}

wxColour wxWinUISplitterSurfaceColour(wxWindow *win)
{
    // Accessibility colours take precedence over the backdrop marker: High
    // Contrast intentionally disables decorative material assumptions.
    if ( wxMSWImpl::IsHighContrast() )
        return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

    if ( wxWinUIWindowUsesBackdrop(win) )
    {
        // Native children use black as the composition colour key while the
        // XAML backdrop is active (see wxWinUIControlHost). Keep this named
        // protocol value here instead of treating it as a theme colour.
        static const wxColour s_backdropColourKey(0, 0, 0);
        return s_backdropColourKey;
    }

    const wxColour background = win->GetBackgroundColour();
    return background.IsOk()
        ? background
        : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
}

class wxRendererWinUI : public wxDelegateRendererNative
{
public:
    // Delegate everything we don't override to the classic MSW renderer.
    wxRendererWinUI() : wxDelegateRendererNative(wxRendererNative::GetDefault())
    {
    }

    int DrawHeaderButton(wxWindow *win,
                         wxDC& dc,
                         const wxRect& rect,
                         int flags = 0,
                         wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                         wxHeaderButtonParams* params = nullptr) override
    {
        // Flat header: plain background, hover tint, hairline bottom divider.
        const wxColour bg = win->GetBackgroundColour();
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(bg));
        dc.DrawRectangle(rect);

        if ( flags & (wxCONTROL_CURRENT | wxCONTROL_PRESSED) )
        {
            dc.SetBrush(wxBrush(wxWinUISubtleFill(bg)));
            wxRect fill = rect;
            fill.Deflate(win->FromDIP(2));
            dc.DrawRoundedRectangle(fill, win->FromDIP(4));
        }

        dc.SetPen(wxPen(wxWinUIDividerColour(bg)));
        dc.DrawLine(rect.GetLeft(), rect.GetBottom(),
                    rect.GetRight() + 1, rect.GetBottom());

        return DrawHeaderButtonContents(win, dc, rect, flags, sortArrow, params);
    }

    int GetHeaderButtonHeight(wxWindow *win) override
    {
        return win->FromDIP(32);
    }

    void DrawItemSelectionRect(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0) override
    {
        wxColour fill;
        if ( flags & wxCONTROL_SELECTED )
        {
            fill = wxSystemSettings::GetColour(
                (flags & wxCONTROL_FOCUSED) ? wxSYS_COLOUR_HIGHLIGHT
                                            : wxSYS_COLOUR_BTNSHADOW);
        }
        else if ( flags & wxCONTROL_CURRENT )
        {
            fill = wxWinUISubtleFill(win->GetBackgroundColour());
        }
        else
        {
            return;
        }

        // Rounded selection pill, as used by the WinUI list controls.
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(fill));
        wxRect pill = rect;
        pill.Deflate(win->FromDIP(1), 0);
        dc.DrawRoundedRectangle(pill, win->FromDIP(4));
    }

    void DrawFocusRect(wxWindow *win,
                       wxDC& dc,
                       const wxRect& rect,
                       int WXUNUSED(flags) = 0) override
    {
        // Rounded outline instead of the classic dotted rectangle.
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
        dc.DrawRoundedRectangle(rect, win->FromDIP(4));
    }

    // A Fluent surface has no 3-D edges: draw nothing for the border and a
    // flat, theme-coloured sash.  Otherwise wxSP_3D splitters show the classic
    // light highlight line, which reads as a bright scar on a dark theme.
    void DrawSplitterBorder(wxWindow *WXUNUSED(win),
                            wxDC& WXUNUSED(dc),
                            const wxRect& WXUNUSED(rect),
                            int WXUNUSED(flags) = 0) override
    {
    }

    void DrawSplitterSash(wxWindow *win,
                          wxDC& dc,
                          const wxSize& size,
                          wxCoord position,
                          wxOrientation orient,
                          int WXUNUSED(flags) = 0) override
    {
        // Paint explicitly: WM_PAINT is allowed without a preceding erase.
        // Leaving this rectangle untouched was the source of the persistent
        // white scar after resize. The same policy also remains meaningful
        // when High Contrast disables Mica.
        const wxCoord width = win->FromDIP(6);
        const wxRect sash = orient == wxVERTICAL
            ? wxRect(position, 0, width, size.y)
            : wxRect(0, position, size.x, width);

        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(wxWinUISplitterSurfaceColour(win)));
        dc.DrawRectangle(sash);

        // High Contrast removes the material cue, so a surface-coloured sash
        // alone can disappear into its panes. Use system colours only and
        // retain a visible one-DIP separator in every HC palette.
        if ( wxMSWImpl::IsHighContrast() )
        {
            const wxCoord markerDIP = win->FromDIP(1);
            const wxCoord markerWidth = markerDIP > 1 ? markerDIP : 1;
            const wxRect marker = orient == wxVERTICAL
                ? wxRect(sash.x + (sash.width - markerWidth) / 2,
                         sash.y, markerWidth, sash.height)
                : wxRect(sash.x,
                         sash.y + (sash.height - markerWidth) / 2,
                         sash.width, markerWidth);
            dc.SetBrush(wxBrush(
                wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
            dc.DrawRectangle(marker);
        }
    }

    wxSplitterRenderParams GetSplitterParams(const wxWindow *win) override
    {
        // No 3-D border, whatever the window style asks for.
        return wxSplitterRenderParams(win->FromDIP(6), 0, false);
    }

    void DrawTreeItemButton(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags = 0) override
    {
        // A Fluent chevron: '>' when collapsed, 'v' when expanded.
        const int size = win->FromDIP(3);
        const wxPoint c = wxPoint(rect.x + rect.width/2, rect.y + rect.height/2);

        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT),
                        win->FromDIP(1)));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        if ( flags & wxCONTROL_EXPANDED )
        {
            dc.DrawLine(c.x - size, c.y - size/2, c.x, c.y + size - size/2);
            dc.DrawLine(c.x, c.y + size - size/2, c.x + size, c.y - size/2);
        }
        else
        {
            dc.DrawLine(c.x - size/2, c.y - size, c.x + size - size/2, c.y);
            dc.DrawLine(c.x + size - size/2, c.y, c.x - size/2, c.y + size);
        }
    }
};

} // anonymous namespace

// Choose the toolkit renderer once, independently of XAML runtime epochs.
void wxWinUIInstallRenderer()
{
    static bool configured = false;
    if ( configured )
        return;

    // Resolve a traits-provided renderer, or one explicitly installed before
    // initialization. An application renderer always takes precedence.
    if ( &wxRendererNative::Get() == &wxRendererNative::GetDefault() )
    {
        // Get() returned the unowned default: there is no displaced renderer
        // to delete. The global renderer holder owns the new instance, using
        // the ordinary wxRendererNative::Set() ownership contract.
        wxRendererNative::Set(new wxRendererWinUI);
    }

    // Even Set(nullptr) is an application choice after this point. Repeated
    // runtime initialization must never overwrite it or a newer renderer.
    configured = true;
}

#endif // wxUSE_WINUI3
