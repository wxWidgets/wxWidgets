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

protected:
#if wxUSE_MARKUP
    bool DoSetLabelMarkup(const wxString& markup) override;
#endif
    wxSize DoGetBestClientSize() const override;
    wxString WXGetVisibleLabel() const override;
    void WXSetVisibleLabel(const wxString& str) override;

private:
    void UpdateWinUIContent();

    std::unique_ptr<wxWinUIStaticTextImpl> m_winui;
    wxString m_visibleLabel;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticText);
};

#endif // _WX_WINUI_STATTEXT_H_
