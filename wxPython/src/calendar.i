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
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

#include <wx/calctrl.h>
%}

//----------------------------------------------------------------------

%import misc.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__docfilter__ }

%include _calendar_rename.i

//---------------------------------------------------------------------------

enum {
    wxCAL_SUNDAY_FIRST,
    wxCAL_MONDAY_FIRST,
    wxCAL_SHOW_HOLIDAYS,
    wxCAL_NO_YEAR_CHANGE,
    wxCAL_NO_MONTH_CHANGE,
    wxCAL_SEQUENTIAL_MONTH_SELECTION,
    wxCAL_SHOW_SURROUNDING_WEEKS,
};


enum wxCalendarHitTestResult
{
    wxCAL_HITTEST_NOWHERE,      // outside of anything
    wxCAL_HITTEST_HEADER,       // on the header (weekdays)
    wxCAL_HITTEST_DAY,          // on a day in the calendar
    wxCAL_HITTEST_INCMONTH,
    wxCAL_HITTEST_DECMONTH,
    wxCAL_HITTEST_SURROUNDING_WEEK
};

// border types for a date
enum wxCalendarDateBorder
{
    wxCAL_BORDER_NONE,          // no border (default)
    wxCAL_BORDER_SQUARE,        // a rectangular border
    wxCAL_BORDER_ROUND          // a round border
};

//---------------------------------------------------------------------------

DocStr(wxCalendarDateAttr,
"A set of customization attributes for a calendar date, which can be used to
control the look of the Calendar object.");

class wxCalendarDateAttr
{
public:
    DocStr(wxCalendarDateAttr,
            "Create a CalendarDateAttr.");
    wxCalendarDateAttr(const wxColour& colText = wxNullColour,
                       const wxColour& colBack = wxNullColour,
                       const wxColour& colBorder = wxNullColour,
                       const wxFont& font = wxNullFont,
                       wxCalendarDateBorder border = wxCAL_BORDER_NONE);

    
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

    wxColour GetTextColour() const;
    wxColour GetBackgroundColour() const;
    wxColour GetBorderColour() const;
    wxFont GetFont() const;
    wxCalendarDateBorder GetBorder() const;
};

//---------------------------------------------------------------------------

class wxCalendarCtrl;

class wxCalendarEvent : public wxCommandEvent
{
public:
    wxCalendarEvent(wxCalendarCtrl *cal, wxEventType type);

    const wxDateTime GetDate() const;
    void SetDate(const wxDateTime &date);
    void SetWeekDay(const wxDateTime::WeekDay wd);
    wxDateTime::WeekDay GetWeekDay() const;

};


%constant wxEventType wxEVT_CALENDAR_DOUBLECLICKED;
%constant wxEventType wxEVT_CALENDAR_SEL_CHANGED;
%constant wxEventType wxEVT_CALENDAR_DAY_CHANGED;
%constant wxEventType wxEVT_CALENDAR_MONTH_CHANGED;
%constant wxEventType wxEVT_CALENDAR_YEAR_CHANGED;
%constant wxEventType wxEVT_CALENDAR_WEEKDAY_CLICKED;


%pythoncode {
EVT_CALENDAR =                 wx.PyEventBinder( wxEVT_CALENDAR_DOUBLECLICKED, 1)
EVT_CALENDAR_SEL_CHANGED =     wx.PyEventBinder( wxEVT_CALENDAR_SEL_CHANGED, 1)
EVT_CALENDAR_DAY =             wx.PyEventBinder( wxEVT_CALENDAR_DAY_CHANGED, 1)
EVT_CALENDAR_MONTH =           wx.PyEventBinder( wxEVT_CALENDAR_MONTH_CHANGED, 1)
EVT_CALENDAR_YEAR =            wx.PyEventBinder( wxEVT_CALENDAR_YEAR_CHANGED, 1)
EVT_CALENDAR_WEEKDAY_CLICKED = wx.PyEventBinder( wxEVT_CALENDAR_WEEKDAY_CLICKED, 1)
}


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(CalendarNameStr);


DocStr(wxCalendarCtrl,
       "The calendar control allows the user to pick a date interactively.");

RefDoc(wxCalendarCtrl,

"The CalendarCtrl displays a window containing several parts: the control to
pick the month and the year at the top (either or both of them may be
disabled) and a month area below them which shows all the days in the
month. The user can move the current selection using the keyboard and select
the date (generating EVT_CALENDAR event) by pressing <Return> or double
clicking it.

It has advanced possibilities for the customization of its display. All global
settings (such as colours and fonts used) can, of course, be changed. But
also, the display style for each day in the month can be set independently
using CalendarDateAttr class.

An item without custom attributes is drawn with the default colours and font
and without border, but setting custom attributes with SetAttr allows to
modify its appearance. Just create a custom attribute object and set it for
the day you want to be displayed specially A day may be marked as being a
holiday, (even if it is not recognized as one by wx.DateTime) by using the
SetHoliday method.

As the attributes are specified for each day, they may change when the month
is changed, so you will often want to update them in an EVT_CALENDAR_MONTH
event handler.

 Styles
    CAL_SUNDAY_FIRST:	        Show Sunday as the first day in the week
    CAL_MONDAY_FIRST:	        Show Monday as the first day in the week
    CAL_SHOW_HOLIDAYS:	        Highlight holidays in the calendar
    CAL_NO_YEAR_CHANGE:	        Disable the year changing
    CAL_NO_MONTH_CHANGE:	Disable the month (and, implicitly, the year) changing
    CAL_SHOW_SURROUNDING_WEEKS:	Show the neighbouring weeks in the previous and next months
    CAL_SEQUENTIAL_MONTH_SELECTION:	Use alternative, more compact, style for the month and year selection controls.

The default calendar style is wxCAL_SHOW_HOLIDAYS.

 Events
    EVT_CALENDAR:       	A day was double clicked in the calendar.
    EVT_CALENDAR_SEL_CHANGED:	The selected date changed.
    EVT_CALENDAR_DAY:    	The selected day changed.
    EVT_CALENDAR_MONTH: 	The selected month changed.
    EVT_CALENDAR_YEAR:   	The selected year changed.
    EVT_CALENDAR_WEEKDAY_CLICKED: 	User clicked on the week day header

Note that changing the selected date will result in either of
EVT_CALENDAR_DAY, MONTH or YEAR events and an EVT_CALENDAR_SEL_CHANGED event.
    
");       

       
class wxCalendarCtrl : public wxControl
{
public:
    %pythonAppend wxCalendarCtrl      "self._setOORInfo(self)"
    %pythonAppend wxCalendarCtrl()    ""

    DocStr(wxCalendarCtrl, "Create and show a calendar control.");
    RefDoc(wxCalendarCtrl, ""); // turn it off for the ctors
    
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id=-1,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS | wxWANTS_CHARS,
                   const wxString& name = wxPyCalendarNameStr);


    DocStr(wxCalendarCtrl(), "Precreate a CalendarCtrl for 2-phase creation.");
    %name(PreCalendarCtrl)wxCalendarCtrl();

    DocStr(Create, "Acutally create the GUI portion of the CalendarCtrl for 2-phase creation.");
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS | wxWANTS_CHARS,
                const wxString& name = wxPyCalendarNameStr);


    DocDeclStr(
        void, SetDate(const wxDateTime& date),
        "Sets the current date.");

    DocDeclStr(
        const wxDateTime&, GetDate() const,
        "Gets the currently selected date.");
    


    DocDeclStr(
        bool, SetLowerDateLimit(const wxDateTime& date = wxDefaultDateTime),
        "set the range in which selection can occur");
    
    DocDeclStr(
        bool, SetUpperDateLimit(const wxDateTime& date = wxDefaultDateTime),
        "set the range in which selection can occur");
    
    DocDeclStr(
        const wxDateTime&, GetLowerDateLimit() const,
        "get the range in which selection can occur");
    
    DocDeclStr(
        const wxDateTime&, GetUpperDateLimit() const,
        "get the range in which selection can occur");

    DocDeclStr(
        bool, SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                           const wxDateTime& upperdate = wxDefaultDateTime),
        "set the range in which selection can occur");


    

    DocDeclStr(
        void, EnableYearChange(bool enable = True),
        "This function should be used instead of changing CAL_NO_YEAR_CHANGE\n"
        "style bit directly. It allows or disallows the user to change the year\n"
        "interactively.");
    
    DocDeclStr(
        void, EnableMonthChange(bool enable = True),
        "This function should be used instead of changing CAL_NO_MONTH_CHANGE style\n"
        "bit. It allows or disallows the user to change the month interactively. Note\n"
        "that if the month can not be changed, the year can not be changed either.");

    DocDeclStr(
        void, EnableHolidayDisplay(bool display = True),
        "This function should be used instead of changing CAL_SHOW_HOLIDAYS style\n"
        "bit directly. It enables or disables the special highlighting of the holidays.");


    
    DocDeclStr(
        void, SetHeaderColours(const wxColour& colFg, const wxColour& colBg),
        "header colours are used for painting the weekdays at the top");

    DocDeclStr(
        wxColour, GetHeaderColourFg() const,
        "header colours are used for painting the weekdays at the top");
    
    DocDeclStr(
        wxColour, GetHeaderColourBg() const,
        "header colours are used for painting the weekdays at the top");


    
    DocDeclStr(
        void, SetHighlightColours(const wxColour& colFg, const wxColour& colBg),
        "highlight colour is used for the currently selected date");
    
    DocDeclStr(
        wxColour, GetHighlightColourFg() const,
        "highlight colour is used for the currently selected date");
    
    DocDeclStr(
        wxColour, GetHighlightColourBg() const,
        "highlight colour is used for the currently selected date");

    
     
    DocDeclStr(
        void, SetHolidayColours(const wxColour& colFg, const wxColour& colBg),
        "holiday colour is used for the holidays (if CAL_SHOW_HOLIDAYS style is used)");
    
    DocDeclStr(
        wxColour, GetHolidayColourFg() const,
        "holiday colour is used for the holidays (if CAL_SHOW_HOLIDAYS style is used)");
    
    DocDeclStr(
        wxColour, GetHolidayColourBg() const,
        "holiday colour is used for the holidays (if CAL_SHOW_HOLIDAYS style is used)");


    
    DocDeclStr(
        wxCalendarDateAttr*, GetAttr(size_t day) const,
        "Returns the attribute for the given date (should be in the range 1...31).\n"
        "The returned value may be None");
    
    DocDeclStr(
        void, SetAttr(size_t day, wxCalendarDateAttr *attr),
        "Associates the attribute with the specified date (in the range 1...31).\n"
        "If the attribute passed is None, the items attribute is cleared.");

    DocDeclStr(
        void, SetHoliday(size_t day),
        "Marks the specified day as being a holiday in the current month.");

    DocDeclStr(
        void, ResetAttr(size_t day),
        "Clears any attributes associated with the given day (in the range 1...31).");
    


    DocAStr(HitTest,
            "HitTest(Point pos) -> (result, date, weekday)",
"Returns 3-tuple with information about the given position on the calendar
control.  The first value of the tuple is a result code and determines the
validity of the remaining two values.  The result codes are:

    CAL_HITTEST_NOWHERE:    hit outside of anything
    CAL_HITTEST_HEADER:     hit on the header, weekday is valid
    CAL_HITTEST_DAY:        hit on a day in the calendar, date is set.
");           
    %extend {
        PyObject* HitTest(const wxPoint& pos) {
            wxDateTime* date = new wxDateTime;
            wxDateTime::WeekDay wd;
            wxCalendarHitTestResult result = self->HitTest(pos, date, &wd);
            bool blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(3);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(result));
            PyTuple_SET_ITEM(tup, 1, wxPyConstructObject(date, wxT("wxDateTime"), 1));
            PyTuple_SET_ITEM(tup, 2, PyInt_FromLong(wd));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }

    DocDeclStr(
        wxControl*, GetMonthControl() const,
        "get the currently shown control for month");
    
    DocDeclStr(
        wxControl*, GetYearControl() const,
        "get the currently shown control for year");
};


//---------------------------------------------------------------------------

%init %{
%}

//---------------------------------------------------------------------------

