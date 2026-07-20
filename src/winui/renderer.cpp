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
    #include "wx/window.h"
#endif

#include "wx/renderer.h"

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

// Install the Fluent renderer; called once from wxWinUI3Initialize().
void wxWinUIInstallRenderer()
{
    // wxRendererNative takes ownership of the pointer and deletes it during
    // the library cleanup, so it must be heap-allocated.
    wxRendererNative::Set(new wxRendererWinUI);
}

#endif // wxUSE_WINUI3
