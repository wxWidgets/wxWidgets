/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/statusbar.h
// Purpose:     wxWinUI wxStatusBar declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_STATUSBAR_H_
#define _WX_WINUI_STATUSBAR_H_

#if wxUSE_STATUSBAR

#include <memory>

class wxWinUIStatusBarImpl;
class wxDPIChangedEvent;

// A status bar projected into the shared WinUI island. Fields retain their wx
// pixel geometry while their XAML columns, theme borders and text live in DIPs.
class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    wxStatusBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    ~wxStatusBar() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    // base class overrides
    void SetFieldsCount(int number = 1, const int *widths = nullptr) override;
    void SetStatusWidths(int n, const int widths[]) override;
    void SetStatusStyles(int n, const int styles[]) override;

    bool GetFieldRect(int i, wxRect& rect) const override;
    void SetMinHeight(int height) override;

    int GetBorderX() const override { return m_borderX; }
    int GetBorderY() const override { return m_borderY; }

    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

protected:
    void DoUpdateStatusText(int number) override;
    wxSize DoGetBestSize() const override;

private:
    friend class wxWinUIStatusBarTestAccess;

    // Project the complete status-bar model as one transaction. Text,
    // trimming, UIA, tooltips and appearance deliberately share the same
    // revision so a cross-domain re-entrant mutation can never leave a
    // partially updated peer behind.
    bool RebuildContent();
    wxString GetDisplayedStatusText(int field,
                                    bool *ellipsized,
                                    int *trimming) const;
    void OnSize(wxSizeEvent& event);
    void OnDPIChanged(wxDPIChangedEvent& event);
    // absolute field widths in pixels for the current client width
    wxArrayInt GetAbsWidths() const;
    int GetSizeGripReservedWidth() const;
    int GetMinHeightPixels() const;
    void ApplyMinHeight(double heightDIPs, int heightPixels);

    std::unique_ptr<wxWinUIStatusBarImpl> m_winui;
    int m_borderX = 0;
    int m_borderY = 0;
    // Keep the caller's logical request independent of the monitor on which
    // it was made. -1 means SetMinHeight() has never imposed a minimum.
    double m_minHeightDIPs = -1.0;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBar);
};

#endif // wxUSE_STATUSBAR

#endif // _WX_WINUI_STATUSBAR_H_
