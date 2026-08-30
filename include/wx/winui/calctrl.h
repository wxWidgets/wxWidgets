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

#include <cstdint>
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

    wxCalendarHitTestResult
    HitTest(const wxPoint& pos,
            wxDateTime *date = nullptr,
            wxDateTime::WeekDay *wd = nullptr) override;

    bool EnableMonthChange(bool enable = true) override;
    void EnableHolidayDisplay(bool display = true) override;
    void Mark(size_t day, bool mark) override;
    void SetHoliday(size_t day) override;
    void SetWindowStyleFlag(long style) override;
    void SetMinSize(const wxSize& minSize) override;

protected:
    wxSize DoGetBestSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags) override;

    bool ApplyInitialSizeTransaction(const wxSize& size,
                                     std::uint64_t expectedRevision);
    void OnLayoutLoaded();
    bool ApplyToPeer();
    bool ApplyStateToPeer(const wxDateTime& date,
                          const wxDateTime& lowerdate,
                          const wxDateTime& upperdate,
                          bool monthChangeEnabled);
    void OnPeerSelectionChanged();
    void HandleVisualThemeChanged();
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void ResetHolidayAttrs() override;
    void RefreshHolidays() override;

    std::unique_ptr<wxWinUICalendarImpl> m_winui;
    wxDateTime m_date;
    wxDateTime m_lowerDate;
    wxDateTime m_upperDate;
    std::uint32_t m_marks = 0;
    std::uint32_t m_holidays = 0;

private:
    friend class wxWinUICalendarTestAccess;
    friend class wxWinUICalendarImpl;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCalendarCtrl);
};

#endif // _WX_WINUI_CALCTRL_H_
