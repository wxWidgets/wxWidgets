/////////////////////////////////////////////////////////////////////////////
// Name:        calendar.i
// Purpose:     SWIG definitions for the wxCalendarCtrl
//
// Author:      Robin Dunn
//
// Created:     23-May-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module calendar


%{
#include "export.h"
#include <wx/calctrl.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import controls.i
%import events.i
%import utils.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

enum {
    wxCAL_SUNDAY_FIRST,
    wxCAL_MONDAY_FIRST,
    wxCAL_SHOW_HOLIDAYS,
    wxCAL_NO_YEAR_CHANGE,
    wxCAL_NO_MONTH_CHANGE,
};


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

//---------------------------------------------------------------------------


class wxCalendarDateAttr
{
public:
    // ctors
    wxCalendarDateAttr(const wxColour& colText,
                       const wxColour& colBack = wxNullColour,
                       const wxColour& colBorder = wxNullColour,
                       const wxFont& font = wxNullFont,
                       wxCalendarDateBorder border = wxCAL_BORDER_NONE);

    %name(wxCalendarDateAttrBorder)
        wxCalendarDateAttr(wxCalendarDateBorder border,
                           const wxColour& colBorder = wxNullColour);

    // setters
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetBorderColour(const wxColour& col);
    void SetFont(const wxFont& font);
    void SetBorder(wxCalendarDateBorder border);
    void SetHoliday(bool holiday);

    // accessors
    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasBorderColour() const;
    bool HasFont() const;
    bool HasBorder() const;

    bool IsHoliday() const;

    const wxColour& GetTextColour() const;
    const wxColour& GetBackgroundColour() const;
    const wxColour& GetBorderColour() const;
    const wxFont& GetFont() const;
    wxCalendarDateBorder GetBorder() const;
};

//---------------------------------------------------------------------------

class wxCalendarCtrl;

class wxCalendarEvent : public wxCommandEvent
{
public:
    wxCalendarEvent(wxCalendarCtrl *cal, wxEventType type);

    const wxDateTime& GetDate() const;
    wxDateTime::WeekDay GetWeekDay() const;

};

enum {
    wxEVT_CALENDAR_DOUBLECLICKED,
    wxEVT_CALENDAR_SEL_CHANGED,
    wxEVT_CALENDAR_DAY_CHANGED,
    wxEVT_CALENDAR_MONTH_CHANGED,
    wxEVT_CALENDAR_YEAR_CHANGED,
    wxEVT_CALENDAR_WEEKDAY_CLICKED,
};

%pragma(python) code = "
def EVT_CALENDAR(win, id, fn):
    win.Connect(id, -1, wxEVT_CALENDAR_DOUBLECLICKED, fn)

def EVT_CALENDAR_SEL_CHANGED(win, id, fn):
    win.Connect(id, -1, wxEVT_CALENDAR_SEL_CHANGED, fn)

def EVT_CALENDAR_DAY(win, id, fn):
    win.Connect(id, -1, wxEVT_CALENDAR_DAY_CHANGED, fn)

def EVT_CALENDAR_MONTH(win, id, fn):
    win.Connect(id, -1, wxEVT_CALENDAR_MONTH_CHANGED, fn)

def EVT_CALENDAR_YEAR(win, id, fn):
    win.Connect(id, -1, wxEVT_CALENDAR_YEAR_CHANGED, fn)

def EVT_CALENDAR_WEEKDAY_CLICKED(win, id, fn):
    win.Connect(id, -1, wxEVT_CALENDAR_WEEKDAY_CLICKED, fn)

"

//---------------------------------------------------------------------------

class wxCalendarCtrl : public wxControl
{
public:
    // construction
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const char* name = "calendar");

    // set/get the current date
    // ------------------------

    void SetDate(const wxDateTime& date);
    const wxDateTime& GetDate() const;

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
    void SetHeaderColours(const wxColour& colFg, const wxColour& colBg);
    const wxColour& GetHeaderColourFg() const;
    const wxColour& GetHeaderColourBg() const;

    // highlight colour is used for the currently selected date
    void SetHighlightColours(const wxColour& colFg, const wxColour& colBg);
    const wxColour& GetHighlightColourFg() const;
    const wxColour& GetHighlightColourBg() const;

    // holiday colour is used for the holidays (if style & wxCAL_SHOW_HOLIDAYS)
    void SetHolidayColours(const wxColour& colFg, const wxColour& colBg);
    const wxColour& GetHolidayColourFg() const;
    const wxColour& GetHolidayColourBg() const;

    // an item without custom attributes is drawn with the default colours and
    // font and without border, setting custom attributes allows to modify this
    //
    // the day parameter should be in 1..31 range, for days 29, 30, 31 the
    // corresponding attribute is just unused if there is no such day in the
    // current month

    wxCalendarDateAttr *GetAttr(size_t day) const;
    void SetAttr(size_t day, wxCalendarDateAttr *attr);

    void SetHoliday(size_t day);

    void ResetAttr(size_t day);

    // returns one of wxCAL_HITTEST_XXX constants and fills either date or wd
    // with the corresponding value (none for NOWHERE, the date for DAY and wd
    // for HEADER)
    wxCalendarHitTestResult HitTest(const wxPoint& pos,
                                    wxDateTime *date = NULL,
                                    wxDateTime::WeekDay *wd = NULL);

};


//---------------------------------------------------------------------------

%init %{
    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();
%}

//---------------------------------------------------------------------------

%pragma(python) include="_calextras.py";

//---------------------------------------------------------------------------

