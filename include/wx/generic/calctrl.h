///////////////////////////////////////////////////////////////////////////////
// Name:        generic/calctrl.h
// Purpose:     generic implementation of date-picker control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "calctrl.h"
#endif

#ifndef _WX_GENERIC_CALCTRL_H
#define _WX_GENERIC_CALCTRL_H

#include "wx/control.h"         // the base class

#include "wx/spinctrl.h"        // for wxSpinEvent

class WXDLLEXPORT wxComboBox;
class WXDLLEXPORT wxStaticText;

#define wxCalendarNameStr _T("CalendarCtrl")

// ----------------------------------------------------------------------------
// wxCalendarCtrl: a control allowing the user to pick a date interactively
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCalendarCtrl : public wxControl
{
friend class wxMonthComboBox;
friend class wxYearSpinCtrl;

public:
    // construction
    wxCalendarCtrl() { Init(); }
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const wxString& name = wxCalendarNameStr)
        : wxControl(parent, id, pos, size,
                    style | wxWANTS_CHARS, wxDefaultValidator, name)
    {
        Init();

        (void)Create(parent, id, date, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxCalendarNameStr);

    virtual ~wxCalendarCtrl();

    // set/get the current date
    // ------------------------

    void SetDate(const wxDateTime& date);
    const wxDateTime& GetDate() const { return m_date; }

    // calendar mode
    // -------------

    // some calendar styles can't be changed after the control creation by
    // just using SetWindowStyle() and Refresh() and the functions below
    // should be used instead for them

    // corresponds to wxCAL_NO_YEAR_CHANGE bit
    void EnableYearChange(bool enable = TRUE);

    // corresponds to wxCAL_NO_MONTH_CHANGE bit
    void EnableMonthChange(bool enable = TRUE);

    // corresponds to wxCAL_SHOW_HOLIDAYS bit
    void EnableHolidayDisplay(bool display = TRUE);

    // customization
    // -------------

    // header colours are used for painting the weekdays at the top
    void SetHeaderColours(const wxColour& colFg, const wxColour& colBg)
    {
        m_colHeaderFg = colFg;
        m_colHeaderBg = colBg;
    }

    const wxColour& GetHeaderColourFg() const { return m_colHeaderFg; }
    const wxColour& GetHeaderColourBg() const { return m_colHeaderBg; }

    // highlight colour is used for the currently selected date
    void SetHighlightColours(const wxColour& colFg, const wxColour& colBg)
    {
        m_colHighlightFg = colFg;
        m_colHighlightBg = colBg;
    }

    const wxColour& GetHighlightColourFg() const { return m_colHighlightFg; }
    const wxColour& GetHighlightColourBg() const { return m_colHighlightBg; }

    // holiday colour is used for the holidays (if style & wxCAL_SHOW_HOLIDAYS)
    void SetHolidayColours(const wxColour& colFg, const wxColour& colBg)
    {
        m_colHolidayFg = colFg;
        m_colHolidayBg = colBg;
    }

    const wxColour& GetHolidayColourFg() const { return m_colHolidayFg; }
    const wxColour& GetHolidayColourBg() const { return m_colHolidayBg; }

    // an item without custom attributes is drawn with the default colours and
    // font and without border, setting custom attributes allows to modify this
    //
    // the day parameter should be in 1..31 range, for days 29, 30, 31 the
    // corresponding attribute is just unused if there is no such day in the
    // current month

    wxCalendarDateAttr *GetAttr(size_t day) const
    {
        wxCHECK_MSG( day > 0 && day < 32, NULL, _T("invalid day") );

        return m_attrs[day - 1];
    }

    void SetAttr(size_t day, wxCalendarDateAttr *attr)
    {
        wxCHECK_RET( day > 0 && day < 32, _T("invalid day") );

        delete m_attrs[day - 1];
        m_attrs[day - 1] = attr;
    }

    void SetHoliday(size_t day);

    void ResetAttr(size_t day) { SetAttr(day, (wxCalendarDateAttr *)NULL); }

    // returns one of wxCAL_HITTEST_XXX constants and fills either date or wd
    // with the corresponding value (none for NOWHERE, the date for DAY and wd
    // for HEADER)
    wxCalendarHitTestResult HitTest(const wxPoint& pos,
                                    wxDateTime *date = NULL,
                                    wxDateTime::WeekDay *wd = NULL);

    // implementation only from now on
    // -------------------------------

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);

private:
    // common part of all ctors
    void Init();

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnClick(wxMouseEvent& event);
    void OnDClick(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMonthChange(wxCommandEvent& event);
    void OnYearChange(wxSpinEvent& event);
    void OnCalMonthChange(wxCalendarEvent& event);

    // override some base class virtuals
    virtual wxSize DoGetBestSize() const;
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags);
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // (re)calc m_widthCol and m_heightRow
    void RecalcGeometry();

    // set the date and send the notification
    void SetDateAndNotify(const wxDateTime& date);

    // get the week (row, in range 1..6) for the given date
    size_t GetWeek(const wxDateTime& date) const;

    // get the date from which we start drawing days
    wxDateTime GetStartDate() const;

    // is this date shown?
    bool IsDateShown(const wxDateTime& date) const;

    // redraw the given date
    void RefreshDate(const wxDateTime& date);

    // change the date inside the same month/year
    void ChangeDay(const wxDateTime& date);

    // set the attributes for the holidays if needed
    void SetHolidayAttrs();

    // reset all holidays
    void ResetHolidayAttrs();

    // generate the given calendar event(s)
    void GenerateEvent(wxEventType type)
    {
        wxCalendarEvent event(this, type);
        (void)GetEventHandler()->ProcessEvent(event);
    }

    void GenerateEvents(wxEventType type1, wxEventType type2)
    {
        GenerateEvent(type1);
        GenerateEvent(type2);
    }

    // do we allow changing the month/year?
    bool AllowMonthChange() const
    {
        return (GetWindowStyle() & wxCAL_NO_MONTH_CHANGE)
                != wxCAL_NO_MONTH_CHANGE;
    }
    bool AllowYearChange() const
    {
        return !(GetWindowStyle() & wxCAL_NO_YEAR_CHANGE);
    }

    // show the correct controls
    void ShowCurrentControls();

    // get the currently shown control for month/year
    wxControl *GetMonthControl() const;
    wxControl *GetYearControl() const;

    // the subcontrols
    wxStaticText *m_staticMonth;
    wxComboBox *m_comboMonth;

    wxStaticText *m_staticYear;
    wxSpinCtrl *m_spinYear;

    // the current selection
    wxDateTime m_date;

    // default attributes
    wxColour m_colHighlightFg,
             m_colHighlightBg,
             m_colHolidayFg,
             m_colHolidayBg,
             m_colHeaderFg,
             m_colHeaderBg;

    // the attributes for each of the month days
    wxCalendarDateAttr *m_attrs[31];

    // the width and height of one column/row in the calendar
    wxCoord m_widthCol,
            m_heightRow;

    // the week day names
    wxString m_weekdays[7];

    DECLARE_DYNAMIC_CLASS(wxCalendarCtrl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GENERIC_CALCTRL_H
