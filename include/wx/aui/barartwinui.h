/////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/barartwinui.h
// Purpose:     Declaration of wxAuiWinUIToolBarArt
// Author:      wxWidgets development team
// Created:     2026-08-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUI_BARART_WINUI_H_
#define _WX_AUI_BARART_WINUI_H_

#include "wx/aui/auibar.h"

#if wxUSE_AUI

// ----------------------------------------------------------------------------
// wxAuiWinUIToolBarArt: Fluent chrome for wxAuiToolBar
// ----------------------------------------------------------------------------

// wxAuiToolBar is drawn by its art provider, never by a native control, so
// under this port it would otherwise keep the uxtheme Rebar/Toolbar look of
// wxAuiMSWToolBarArt -- Windows 7 chrome inside a WinUI window.
//
// This provider draws the same elements the way the platform draws a command
// bar today: nothing at all in the rest state, a rounded subtle layer on
// hover and press, and an accent indicator for a toggled tool rather than a
// filled background, so that the application's own icon colours survive.
// Everything is derived from wxSystemSettings, which follows the WinUI theme,
// so a theme switch is picked up by UpdateColoursFromSystem() like anywhere
// else.
class WXDLLIMPEXP_AUI wxAuiWinUIToolBarArt : public wxAuiGenericToolBarArt
{
public:
    wxAuiWinUIToolBarArt();

    wxNODISCARD virtual wxAuiToolBarArt* Clone() override;

    virtual void DrawBackground(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect) override;

    virtual void DrawPlainBackground(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect) override;

    virtual void DrawButton(
                wxDC& dc,
                wxWindow* wnd,
                const wxAuiToolBarItem& item,
                const wxRect& rect) override;

    virtual void DrawDropDownButton(
                wxDC& dc,
                wxWindow* wnd,
                const wxAuiToolBarItem& item,
                const wxRect& rect) override;

    virtual void DrawSeparator(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect) override;

    virtual void DrawGripper(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect) override;

    virtual void DrawOverflowButton(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect,
                int state) override;

    virtual wxSize GetToolSize(
                wxReadOnlyDC& dc,
                wxWindow* wnd,
                const wxAuiToolBarItem& item) override;

    virtual void UpdateColoursFromSystem() override;

protected:
    // The layer drawn behind a tool for the given item state, or an invalid
    // colour when the state draws no layer at all (the rest state does not).
    wxColour GetLayerColour(int state, bool sticky) const;

    // Draws the rounded layer and, for a checked tool, the accent indicator.
    // Does nothing for a state which has no layer.
    void DrawToolLayer(wxDC& dc,
                       wxWindow* wnd,
                       const wxRect& rect,
                       int state,
                       bool sticky);

    // Colours resolved from the current theme by UpdateColoursFromSystem().
    wxColour m_hoverColour;
    wxColour m_pressedColour;
    wxColour m_checkedColour;
    wxColour m_accentColour;
    wxColour m_dividerColour;
    bool m_dark = false;
};

#endif // wxUSE_AUI

#endif // _WX_AUI_BARART_WINUI_H_
