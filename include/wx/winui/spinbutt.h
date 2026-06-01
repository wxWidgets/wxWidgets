/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/spinbutt.h
// Purpose:     wxWinUI wxSpinButton declaration (WinUI RepeatButtons)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_SPINBUTT_H_
#define _WX_WINUI_SPINBUTT_H_

#include <memory>

class wxWinUISpinButtonImpl;

class WXDLLIMPEXP_CORE wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton();
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                 const wxString& name = wxSPIN_BUTTON_NAME);
    ~wxSpinButton() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                const wxString& name = wxSPIN_BUTTON_NAME);

    int GetValue() const override;
    void SetValue(int val) override;
    void SetRange(int minVal, int maxVal) override;
    void SetIncrement(int value) override;
    int GetIncrement() const override;

protected:
    wxSize DoGetBestSize() const override;

    // Increment (+1) or decrement (-1) the value by one step, honoring the
    // range and wxSP_WRAP, and raise the spin events.
    void Step(int direction);

    std::unique_ptr<wxWinUISpinButtonImpl> m_winui;
    int m_value = 0;
    int m_increment = 1;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSpinButton);
};

#endif // _WX_WINUI_SPINBUTT_H_
