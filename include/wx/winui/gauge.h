/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/gauge.h
// Purpose:     wxWinUI wxGauge declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_GAUGE_H_
#define _WX_WINUI_GAUGE_H_

#include <memory>

class wxWinUIGaugeImpl;

class WXDLLIMPEXP_CORE wxGauge : public wxGaugeBase
{
public:
    wxGauge();
    wxGauge(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxGaugeNameStr));
    ~wxGauge() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxGaugeNameStr));

    void SetRange(int range) override;
    int GetRange() const override;

    void SetValue(int pos) override;
    int GetValue() const override;

    void Pulse() override;

protected:
    wxSize DoGetBestSize() const override;

private:
    void ApplyToPeer();

    std::unique_ptr<wxWinUIGaugeImpl> m_winui;
    bool m_indeterminate = false;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGauge);
};

#endif // _WX_WINUI_GAUGE_H_
