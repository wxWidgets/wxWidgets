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

// A status bar drawn with a WinUI island: each field is a TextBlock, separated
// by a thin divider, sitting on the window's Mica backdrop for a modern look.
class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    wxStatusBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    virtual ~wxStatusBar();

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

protected:
    void DoUpdateStatusText(int number) override;
    wxSize DoGetBestSize() const override;

private:
    // (re)create the field text blocks + dividers in the island
    void RebuildContent();
    // absolute field widths in pixels for the current client width
    wxArrayInt GetAbsWidths() const;

    std::unique_ptr<wxWinUIStatusBarImpl> m_winui;
    int m_borderX = 2;
    int m_borderY = 2;
    int m_minHeight = 0;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBar);
};

#endif // wxUSE_STATUSBAR

#endif // _WX_WINUI_STATUSBAR_H_
