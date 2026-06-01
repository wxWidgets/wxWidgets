/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/timectrl.h
// Purpose:     wxWinUI wxTimePickerCtrl declaration (WinUI TimePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TIMECTRL_H_
#define _WX_WINUI_TIMECTRL_H_

#include <memory>

class wxWinUITimePickerImpl;

class WXDLLIMPEXP_CORE wxTimePickerCtrl : public wxTimePickerCtrlBase
{
public:
    wxTimePickerCtrl();
    wxTimePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTP_DEFAULT,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxTimePickerCtrlNameStr);
    ~wxTimePickerCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& dt = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTP_DEFAULT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTimePickerCtrlNameStr);

    void SetValue(const wxDateTime& dt) override;
    wxDateTime GetValue() const override;

protected:
    wxSize DoGetBestSize() const override;

    void ApplyToPeer();
    void OnPeerTimeChanged();

    std::unique_ptr<wxWinUITimePickerImpl> m_winui;
    wxDateTime m_value;
    bool m_updating = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTimePickerCtrl);
};

#endif // _WX_WINUI_TIMECTRL_H_
