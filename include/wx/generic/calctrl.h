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

#include "wx/datetime.h"        // for m_date
#include "wx/combobox.h"        // for m_comboMonth
#include "wx/spinctrl.h"        // for m_spinYear

#define wxCalendarNameStr _T("CalendarCtrl")

// ----------------------------------------------------------------------------
// wxCalendarCtrl: a control allowing the user to pick a date interactively
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCalendarCtrl : public wxControl
{
public:
    // construction
    wxCalendarCtrl() { Init(); }
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxCalendarNameStr)
        : wxControl(parent, id, pos, size, style, wxDefaultValidator, name)
    {
        Init();

        (void)Create(parent, id, date, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxCalendarNameStr);

    // set/get the current date
    void SetDate(const wxDateTime& date);
    const wxDateTime& GetDate() const { return m_date; }

    // returns TRUE if the given point is on a day and fills date with its
    // value
    bool HitTest(const wxPoint& pos, wxDateTime *date);

private:
    // common part of all ctors
    void Init();

    // override some base class virtuals
    virtual wxSize DoGetBestSize() const;
    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags);
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // (re)calc m_widthCol and m_heightRow
    void RecalcGeometry();

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnClick(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMonthChange(wxCommandEvent& event);
    void OnYearChange(wxSpinEvent& event);

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

    // generate a calendar event
    void GenerateEvent(wxEventType type);

    // the subcontrols
    wxComboBox *m_comboMonth;
    wxSpinCtrl *m_spinYear;

    wxDateTime m_date;

    // the width and height of one column/row in the calendar
    wxCoord m_widthCol,
            m_heightRow;

    // the week day names
    wxString m_weekdays[7];

    DECLARE_DYNAMIC_CLASS(wxCalendarCtrl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GENERIC_CALCTRL_H
