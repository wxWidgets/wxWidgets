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

// Keep the documented generic implementation available independently of the
// native WinUI peer. In particular, code explicitly selecting the generic
// control retains its label-only hit testing and painting semantics.
#include "wx/generic/hyperlink.h"

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
    void SetHoverColour(const wxColour& colour) override;

    wxColour GetNormalColour() const override;
    void SetNormalColour(const wxColour& colour) override;

    wxColour GetVisitedColour() const override { return m_visitedColour; }
    void SetVisitedColour(const wxColour& colour) override;

    wxString GetURL() const override { return m_url; }
    void SetURL(const wxString& url) override;

    void SetVisited(bool visited = true) override;
    bool GetVisited() const override { return m_visited; }

    void SetLabel(const wxString& label) override;
    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

    wxVisualAttributes GetDefaultAttributes() const override;
    static wxVisualAttributes
    GetClassDefaultAttributes(
        wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);


protected:
    wxSize DoGetBestSize() const override;

    bool UpdateWinUIContent();

    std::unique_ptr<wxWinUIHyperlinkImpl> m_winui;
    wxString m_url;
    bool m_visited = false;
    wxColour m_hoverColour;
    wxColour m_normalColour;
    wxColour m_visitedColour;
    bool m_hasCustomHoverColour = false;
    bool m_hasCustomNormalColour = false;
    bool m_hasCustomVisitedColour = false;
    bool m_pointerOver = false;

private:
    friend class wxWinUIHyperlinkTestAccess;

    void BumpWinUIModelRevision();
    void HandleClick();
    void HandleContextRequested();
    void HandleThemeChanged();
    void SetPointerOver(bool pointerOver);
    bool CopyURLToClipboard();
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxHyperlinkCtrl);
};

#endif // _WX_WINUI_HYPERLINK_H_
