/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/datectrl.h
// Purpose:     wxWinUI wxDatePickerCtrl declaration (WinUI CalendarDatePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_DATECTRL_H_
#define _WX_WINUI_DATECTRL_H_

#include <memory>

class wxWinUIDatePickerImpl;

class WXDLLIMPEXP_CORE wxDatePickerCtrl : public wxDatePickerCtrlBase
{
public:
    wxDatePickerCtrl();
    wxDatePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxDatePickerCtrlNameStr);
    ~wxDatePickerCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& dt = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDatePickerCtrlNameStr);

    void SetValue(const wxDateTime& dt) override;
    wxDateTime GetValue() const override;

    void SetRange(const wxDateTime& dt1, const wxDateTime& dt2) override;
    bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const override;

protected:
    wxSize DoGetBestSize() const override;

    void ApplyToPeer();
    void OnPeerDateChanged();

    std::unique_ptr<wxWinUIDatePickerImpl> m_winui;
    wxDateTime m_value;
    wxDateTime m_rangeMin;
    wxDateTime m_rangeMax;
    bool m_updating = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDatePickerCtrl);
};

#endif // _WX_WINUI_DATECTRL_H_
