/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/calctrl.h
// Purpose:     wxWinUI wxCalendarCtrl declaration (WinUI CalendarView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CALCTRL_H_
#define _WX_WINUI_CALCTRL_H_

#include <memory>

class wxWinUICalendarImpl;

class WXDLLIMPEXP_CORE wxCalendarCtrl : public wxCalendarCtrlBase
{
public:
    wxCalendarCtrl();
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const wxString& name = wxASCII_STR(wxCalendarNameStr));
    ~wxCalendarCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxASCII_STR(wxCalendarNameStr));

    bool SetDate(const wxDateTime& date) override;
    wxDateTime GetDate() const override;

    bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                      const wxDateTime& upperdate = wxDefaultDateTime) override;
    bool GetDateRange(wxDateTime *lowerdate, wxDateTime *upperdate) const override;

    bool EnableMonthChange(bool enable = true) override;
    void Mark(size_t day, bool mark) override;

protected:
    wxSize DoGetBestSize() const override;

    void ApplyToPeer();
    void OnPeerSelectionChanged();

    std::unique_ptr<wxWinUICalendarImpl> m_winui;
    wxDateTime m_date;
    wxDateTime m_lowerDate;
    wxDateTime m_upperDate;
    bool m_updating = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCalendarCtrl);
};

#endif // _WX_WINUI_CALCTRL_H_
