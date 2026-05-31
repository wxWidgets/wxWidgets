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

protected:
    wxSize DoGetBestClientSize() const override;
    void DoSet3StateValue(wxCheckBoxState value) override;
    wxCheckBoxState DoGet3StateValue() const override;

private:
    void SendCheckBoxEvent();
    void UpdateWinUIContent();

    std::unique_ptr<wxWinUICheckBoxImpl> m_winui;
    wxCheckBoxState m_state;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCheckBox);
};

#endif // _WX_WINUI_CHECKBOX_H_
