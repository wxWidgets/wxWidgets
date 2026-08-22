/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/stattext.h
// Purpose:     wxWinUI wxStaticText declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_STATTEXT_H_
#define _WX_WINUI_STATTEXT_H_

#include <memory>

class wxWinUIStaticTextImpl;
struct wxWinUIAppearanceSnapshot;

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxASCII_STR(wxStaticTextNameStr));
    ~wxStaticText() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticTextNameStr));

    void SetLabel(const wxString& label) override;
    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

    // Deterministic observations of the projected XAML peer.  They expose no
    // mutation path and are intentionally kept WinUI-specific.
    bool WinUIGetAppearanceForTesting(
        wxWinUIAppearanceSnapshot *snapshot) const;
    wxString WinUIGetVisibleLabelForTesting() const;
    wxString WinUIGetRenderedTextForTesting() const;
    int WinUIGetTextTrimmingForTesting() const;
    bool WinUIHasLocalBoldInlineForTesting() const;
    bool WinUIHasLocalUnderlineInlineForTesting() const;

protected:
#if wxUSE_MARKUP
    bool DoSetLabelMarkup(const wxString& markup) override;
#endif
    wxSize DoGetBestClientSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags) override;
    wxString WXGetVisibleLabel() const override;
    void WXSetVisibleLabel(const wxString& str) override;

private:
    bool UsesManualEllipsization() const;
    bool ApplyWinUIAppearance();
    bool UpdateWinUIContent();

    std::unique_ptr<wxWinUIStaticTextImpl> m_winui;
    wxString m_visibleLabel;
    wxString m_markup;          // non-empty when the label is set as markup

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticText);
};

#endif // _WX_WINUI_STATTEXT_H_
