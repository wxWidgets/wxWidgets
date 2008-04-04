/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/calctrl.h
// Purpose:     wxCalendarCtrl control implementation for MSW
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
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
                   const wxString& name = wxCalendarNameStr)
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
                const wxString& name = wxCalendarNameStr);

    virtual bool SetDate(const wxDateTime& date);
    virtual wxDateTime GetDate() const;

    virtual bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                              const wxDateTime& upperdate = wxDefaultDateTime);
    virtual bool GetDateRange(wxDateTime *lowerdate, wxDateTime *upperdate) const;

    virtual bool EnableMonthChange(bool enable = true);

    virtual void Mark(size_t day, bool mark);

    virtual wxCalendarHitTestResult HitTest(const wxPoint& pos,
                                            wxDateTime *date = NULL,
                                            wxDateTime::WeekDay *wd = NULL);

    virtual void SetWindowStyleFlag(long style);

protected:
    virtual wxSize wxCalendarCtrl::DoGetBestSize() const;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    void MSWOnClick(wxMouseEvent& event);
    void MSWOnDoubleClick(wxMouseEvent& event);

private:
    void Init() { m_marks = 0; }

    // bring the control in sync with m_marks
    void UpdateMarks();

    // set first day of week in the control to correspond to our
    // wxCAL_MONDAY_FIRST flag
    void UpdateFirstDayOfWeek();


    // current date, we need to store it instead of simply retrieving it from
    // the control as needed in order to be able to generate the correct events
    // from MSWOnNotify()
    wxDateTime m_date;

    // bit field containing the state (marked or not) of all days in the month
    wxUint32 m_marks;


    DECLARE_DYNAMIC_CLASS(wxCalendarCtrl)
    DECLARE_NO_COPY_CLASS(wxCalendarCtrl)
};

#endif // _WX_MSW_CALCTRL_H_
