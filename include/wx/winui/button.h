/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/button.h
// Purpose:     wxWinUI wxButton declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_BUTTON_H_
#define _WX_WINUI_BUTTON_H_

#include <memory>

class wxWinUIButtonImpl;

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxButtonNameStr));
    ~wxButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxButtonNameStr));

    void SetLabel(const wxString& label) override;
    void Command(wxCommandEvent& event) override;

protected:
    bool SendClickEvent();
    wxSize DoGetBestSize() const override;

private:
    void UpdateWinUIContent();

    std::unique_ptr<wxWinUIButtonImpl> m_winui;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxButton);
};

#endif // _WX_WINUI_BUTTON_H_
