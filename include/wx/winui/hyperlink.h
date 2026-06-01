/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/hyperlink.h
// Purpose:     wxWinUI wxHyperlinkCtrl declaration (WinUI HyperlinkButton)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HYPERLINK_H_
#define _WX_WINUI_HYPERLINK_H_

#include <memory>

class wxWinUIHyperlinkImpl;

class WXDLLIMPEXP_CORE wxHyperlinkCtrl : public wxHyperlinkCtrlBase
{
public:
    wxHyperlinkCtrl();
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label,
                    const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));
    ~wxHyperlinkCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));

    wxColour GetHoverColour() const override { return m_hoverColour; }
    void SetHoverColour(const wxColour& colour) override { m_hoverColour = colour; }

    wxColour GetNormalColour() const override { return m_normalColour; }
    void SetNormalColour(const wxColour& colour) override { m_normalColour = colour; }

    wxColour GetVisitedColour() const override { return m_visitedColour; }
    void SetVisitedColour(const wxColour& colour) override { m_visitedColour = colour; }

    wxString GetURL() const override { return m_url; }
    void SetURL(const wxString& url) override { m_url = url; }

    void SetVisited(bool visited = true) override { m_visited = visited; }
    bool GetVisited() const override { return m_visited; }

    void SetLabel(const wxString& label) override;

protected:
    wxSize DoGetBestSize() const override;

    void UpdateWinUIContent();

    std::unique_ptr<wxWinUIHyperlinkImpl> m_winui;
    wxString m_url;
    bool m_visited = false;
    wxColour m_hoverColour;
    wxColour m_normalColour;
    wxColour m_visitedColour;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxHyperlinkCtrl);
};

#endif // _WX_WINUI_HYPERLINK_H_
