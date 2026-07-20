/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/radiobut.h
// Purpose:     wxWinUI wxRadioButton declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_RADIOBUT_H_
#define _WX_WINUI_RADIOBUT_H_

#include <memory>

class wxWinUIRadioButtonImpl;

class WXDLLIMPEXP_CORE wxRadioButton : public wxRadioButtonBase
{
public:
    wxRadioButton();
    wxRadioButton(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxASCII_STR(wxRadioButtonNameStr));
    ~wxRadioButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioButtonNameStr));

    void SetValue(bool value) override;
    bool GetValue() const override;
    void SetLabel(const wxString& label) override;
    void Command(wxCommandEvent& event) override;

    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;
#if wxUSE_TOOLTIPS
    void DoSetToolTipText(const wxString& tip) override;
    void DoSetToolTip(wxToolTip *tip) override;
#endif // wxUSE_TOOLTIPS

protected:
    void DoEnable(bool enable) override;
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }
    wxSize DoGetBestSize() const override;

private:
    void UpdateWinUIContent();
    void UpdateWinUIAppearance();
    void ApplyToolTip();
    void ClearRadioGroup();
    void SendRadioEvent();

    std::unique_ptr<wxWinUIRadioButtonImpl> m_winui;
    bool m_isChecked;
#if wxUSE_TOOLTIPS
    wxString m_tooltipText;
#endif // wxUSE_TOOLTIPS

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxRadioButton);
};

#endif // _WX_WINUI_RADIOBUT_H_
