/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/checkbox.h
// Purpose:     wxWinUI wxCheckBox declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CHECKBOX_H_
#define _WX_WINUI_CHECKBOX_H_

#include <memory>

class wxWinUICheckBoxImpl;

class WXDLLIMPEXP_CORE wxCheckBox : public wxCheckBoxBase
{
public:
    wxCheckBox();
    wxCheckBox(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxCheckBoxNameStr));
    ~wxCheckBox() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

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
    bool MSWOnEffectiveLayoutDirectionChanged() override;
    void DoEnable(bool enable) override;
    wxSize DoGetBestClientSize() const override;
    void DoSet3StateValue(wxCheckBoxState value) override;
    wxCheckBoxState DoGet3StateValue() const override;

private:
    void SendCheckBoxEvent();
    bool UpdateWinUIContent(bool forceRender = true);
    bool UpdateWinUIAppearance(bool forceRender = true);
    std::unique_ptr<wxWinUICheckBoxImpl> m_winui;
    wxCheckBoxState m_state = wxCHK_UNCHECKED;
#if wxUSE_TOOLTIPS
    wxString m_tooltipText;
#endif // wxUSE_TOOLTIPS

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCheckBox);
};

#endif // _WX_WINUI_CHECKBOX_H_
