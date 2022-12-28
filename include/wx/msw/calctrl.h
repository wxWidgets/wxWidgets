/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/calctrl.h
// Purpose:     wxCalendarCtrl control implementation for MSW
// Author:      Vadim Zeitlin
// Copyright:   (C) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_CALCTRL_H_
#define _WX_MSW_CALCTRL_H_

class WXDLLIMPEXP_ADV wxCalendarCtrl : public wxCalendarCtrlBase
{
public:
    wxCalendarCtrl() { Init(); }
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const wxString& name = wxASCII_STR(wxCalendarNameStr))
    {
        Init();

        Create(parent, id, date, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxASCII_STR(wxCalendarNameStr));

    virtual bool SetDate(const wxDateTime& date) override;
    virtual wxDateTime GetDate() const override;

    virtual bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                              const wxDateTime& upperdate = wxDefaultDateTime) override;
    virtual bool GetDateRange(wxDateTime *lowerdate, wxDateTime *upperdate) const override;

    virtual bool EnableMonthChange(bool enable = true) override;

    virtual void Mark(size_t day, bool mark) override;
    virtual void SetHoliday(size_t day) override;

    virtual wxCalendarHitTestResult HitTest(const wxPoint& pos,
                                            wxDateTime *date = nullptr,
                                            wxDateTime::WeekDay *wd = nullptr) override;

    virtual void SetWindowStyleFlag(long style) override;

protected:
    virtual wxSize DoGetBestSize() const override;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;

    void MSWOnClick(wxMouseEvent& event);
    void MSWOnDoubleClick(wxMouseEvent& event);

private:
    void Init();

    // bring the control in sync with m_marks
    void UpdateMarks();

    // set first day of week in the control to correspond to our
    // wxCAL_MONDAY_FIRST flag
    void UpdateFirstDayOfWeek();

    // reset holiday information
    virtual void ResetHolidayAttrs() override { m_holidays = 0; }

    // redisplay holidays
    virtual void RefreshHolidays() override { UpdateMarks(); }


    // current date, we need to store it instead of simply retrieving it from
    // the control as needed in order to be able to generate the correct events
    // from MSWOnNotify()
    wxDateTime m_date;

    // bit field containing the state (marked or not) of all days in the month
    wxUint32 m_marks;

    // the same but indicating whether a day is a holiday or not
    wxUint32 m_holidays;


    wxDECLARE_DYNAMIC_CLASS(wxCalendarCtrl);
    wxDECLARE_NO_COPY_CLASS(wxCalendarCtrl);
};

#endif // _WX_MSW_CALCTRL_H_
