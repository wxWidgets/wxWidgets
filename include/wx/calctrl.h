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

/*
   TODO

   1. implement multiple selections for date ranges
   2. background bitmap for the calendar?
 */

#ifndef _WX_CALCTRL_H_
#define _WX_CALCTRL_H_

#include "wx/defs.h"

#if wxUSE_CALENDARCTRL

#include "wx/datetime.h"

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

// border types for a date
enum wxCalendarDateBorder
{
    wxCAL_BORDER_NONE,          // no border (default)
    wxCAL_BORDER_SQUARE,        // a rectangular border
    wxCAL_BORDER_ROUND          // a round border
};

// ----------------------------------------------------------------------------
// wxCalendarDateAttr: custom attributes for a calendar date
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCalendarDateAttr
{
#if !defined(__VISAGECPP__)
protected:
    // This has to be before the use of Init(), for MSVC++ 1.5
    // But dorks up Visualage!
    void Init(wxCalendarDateBorder border = wxCAL_BORDER_NONE)
    {
        m_border = border;
        m_holiday = FALSE;
    }
#endif
public:
    // ctors
    wxCalendarDateAttr() { Init(); }
    wxCalendarDateAttr(const wxColour& colText,
                       const wxColour& colBack = wxNullColour,
                       const wxColour& colBorder = wxNullColour,
                       const wxFont& font = wxNullFont,
                       wxCalendarDateBorder border = wxCAL_BORDER_NONE)
        : m_colText(colText), m_colBack(colBack),
          m_colBorder(colBorder), m_font(font)
    {
        Init(border);
    }
    wxCalendarDateAttr(wxCalendarDateBorder border,
                       const wxColour& colBorder = wxNullColour)
        : m_colBorder(colBorder)
    {
        Init(border);
    }

    // setters
    void SetTextColour(const wxColour& colText) { m_colText = colText; }
    void SetBackgroundColour(const wxColour& colBack) { m_colBack = colBack; }
    void SetBorderColour(const wxColour& col) { m_colBorder = col; }
    void SetFont(const wxFont& font) { m_font = font; }
    void SetBorder(wxCalendarDateBorder border) { m_border = border; }
    void SetHoliday(bool holiday) { m_holiday = holiday; }

    // accessors
    bool HasTextColour() const { return m_colText.Ok(); }
    bool HasBackgroundColour() const { return m_colBack.Ok(); }
    bool HasBorderColour() const { return m_colBorder.Ok(); }
    bool HasFont() const { return m_font.Ok(); }
    bool HasBorder() const { return m_border != wxCAL_BORDER_NONE; }

    bool IsHoliday() const { return m_holiday; }

    const wxColour& GetTextColour() const { return m_colText; }
    const wxColour& GetBackgroundColour() const { return m_colBack; }
    const wxColour& GetBorderColour() const { return m_colBorder; }
    const wxFont& GetFont() const { return m_font; }
    wxCalendarDateBorder GetBorder() const { return m_border; }
#if defined(__VISAGECPP__)
protected:
    // This has to be here for VisualAge
    void Init(wxCalendarDateBorder border = wxCAL_BORDER_NONE)
    {
        m_border = border;
        m_holiday = FALSE;
    }
#endif
private:
    wxColour m_colText,
             m_colBack,
             m_colBorder;
    wxFont   m_font;
    wxCalendarDateBorder m_border;
    bool m_holiday;
};

// ----------------------------------------------------------------------------
// wxCalendarCtrl events
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCalendarCtrl;

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

    DECLARE_DYNAMIC_CLASS(wxCalendarEvent)
};

// ----------------------------------------------------------------------------
// wxCalendarCtrl
// ----------------------------------------------------------------------------

// so far we only have a generic version, so keep it simple
#include "wx/generic/calctrl.h"

// ----------------------------------------------------------------------------
// calendar events macros
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::*wxCalendarEventFunction)(wxCalendarEvent&);

#define EVT_CALENDAR(id, fn) { wxEVT_CALENDAR_DOUBLECLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxCalendarEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_SEL_CHANGED(id, fn) { wxEVT_CALENDAR_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxCalendarEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_DAY(id, fn) { wxEVT_CALENDAR_DAY_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxCalendarEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_MONTH(id, fn) { wxEVT_CALENDAR_MONTH_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxCalendarEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_YEAR(id, fn) { wxEVT_CALENDAR_YEAR_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxCalendarEventFunction) & fn, (wxObject *) NULL },
#define EVT_CALENDAR_WEEKDAY_CLICKED(id, fn) { wxEVT_CALENDAR_WEEKDAY_CLICKED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxCalendarEventFunction) & fn, (wxObject *) NULL },

#endif // wxUSE_CALENDARCTRL

#endif // _WX_CALCTRL_H_

