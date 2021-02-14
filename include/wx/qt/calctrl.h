/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/calctrl.h
// Purpose:     wxCalendarCtrl control implementation for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CALCTRL_H_
#define _WX_QT_CALCTRL_H_

#include "wx/calctrl.h"
class QCalendarWidget;

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

    virtual ~wxCalendarCtrl();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxASCII_STR(wxCalendarNameStr));

    virtual bool SetDate(const wxDateTime& date) wxOVERRIDE;
    virtual wxDateTime GetDate() const wxOVERRIDE;

    virtual bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                              const wxDateTime& upperdate = wxDefaultDateTime) wxOVERRIDE;
    virtual bool GetDateRange(wxDateTime *lowerdate, wxDateTime *upperdate) const wxOVERRIDE;

    virtual bool EnableMonthChange(bool enable = true) wxOVERRIDE;
    virtual void Mark(size_t day, bool mark) wxOVERRIDE;

    // holidays colours
    virtual void SetHoliday(size_t day) wxOVERRIDE;
    virtual void SetHolidayColours(const wxColour& colFg, const wxColour& colBg) wxOVERRIDE;
    virtual const wxColour& GetHolidayColourFg() const wxOVERRIDE { return m_colHolidayFg; }
    virtual const wxColour& GetHolidayColourBg() const wxOVERRIDE { return m_colHolidayBg; }

    // header colours
    virtual void SetHeaderColours(const wxColour& colFg, const wxColour& colBg) wxOVERRIDE;
    virtual const wxColour& GetHeaderColourFg() const wxOVERRIDE { return m_colHeaderFg; }
    virtual const wxColour& GetHeaderColourBg() const wxOVERRIDE { return m_colHeaderBg; }

    // day attributes
    virtual wxCalendarDateAttr *GetAttr(size_t day) const wxOVERRIDE;
    virtual void SetAttr(size_t day, wxCalendarDateAttr *attr) wxOVERRIDE;
    virtual void ResetAttr(size_t day) wxOVERRIDE { SetAttr(day, NULL); }


    virtual void SetWindowStyleFlag(long style) wxOVERRIDE;

    using wxCalendarCtrlBase::GenerateAllChangeEvents;

    virtual QWidget *GetHandle() const wxOVERRIDE;

protected:
    virtual void RefreshHolidays() wxOVERRIDE;

private:
    void Init();
    void UpdateStyle();

    QCalendarWidget *m_qtCalendar;
    wxColour m_colHeaderFg,
             m_colHeaderBg,
             m_colHolidayFg,
             m_colHolidayBg;

    wxCalendarDateAttr *m_attrs[31];


    wxDECLARE_DYNAMIC_CLASS(wxCalendarCtrl);
};

#endif // _WX_QT_CALCTRL_H_
