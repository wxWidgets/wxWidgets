///////////////////////////////////////////////////////////////////////////////
// Name:        wx/calctrl.h
// Purpose:     date-picker control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CALCTRL_H
#define _WX_CALCTRL_H

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// return values for the HitTest() method
enum wxCalendarHitTestResult
{
    wxCAL_HITTEST_NOWHERE,      // outside of anything
    wxCAL_HITTEST_HEADER,       // on the header (weekdays)
    wxCAL_HITTEST_DAY           // on a day in the calendar
};

// ----------------------------------------------------------------------------
// wxCalendarCtrl
// ----------------------------------------------------------------------------

// so far we only have a generic version, so keep it simple
#include "wx/generic/calctrl.h"

// ----------------------------------------------------------------------------
// wxCalendarCtrl events
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCalendarEvent : public wxCommandEvent
{
friend class wxCalendarCtrl;
public:
    wxCalendarEvent() { Init(); }
    wxCalendarEvent(wxCalendarCtrl *cal, wxEventType type);

    const wxDateTime& GetDate() const { return m_date; }
    wxDateTime::WeekDay GetWeekDay() const { return m_wday; }

protected:
    void Init();

private:
    wxDateTime m_date;
    wxDateTime::WeekDay m_wday;
};

#define EVT_CALENDAR(id, fn) { wxEVT_CALENDAR_DOUBLECLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_SEL_CHANGED(id, fn) { wxEVT_CALENDAR_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_DAY(id, fn) { wxEVT_CALENDAR_DAY_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_MONTH(id, fn) { wxEVT_CALENDAR_MONTH_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_YEAR(id, fn) { wxEVT_CALENDAR_YEAR_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_WEEKDAY_CLICKED(id, fn) { wxEVT_CALENDAR_WEEKDAY_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) & fn, (wxObject *) NULL },

#endif // _WX_CALCTRL_H
