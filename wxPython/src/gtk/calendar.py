# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _calendar

import misc
import core
wx = core 
CAL_SUNDAY_FIRST = _calendar.CAL_SUNDAY_FIRST
CAL_MONDAY_FIRST = _calendar.CAL_MONDAY_FIRST
CAL_SHOW_HOLIDAYS = _calendar.CAL_SHOW_HOLIDAYS
CAL_NO_YEAR_CHANGE = _calendar.CAL_NO_YEAR_CHANGE
CAL_NO_MONTH_CHANGE = _calendar.CAL_NO_MONTH_CHANGE
CAL_SEQUENTIAL_MONTH_SELECTION = _calendar.CAL_SEQUENTIAL_MONTH_SELECTION
CAL_SHOW_SURROUNDING_WEEKS = _calendar.CAL_SHOW_SURROUNDING_WEEKS
CAL_HITTEST_NOWHERE = _calendar.CAL_HITTEST_NOWHERE
CAL_HITTEST_HEADER = _calendar.CAL_HITTEST_HEADER
CAL_HITTEST_DAY = _calendar.CAL_HITTEST_DAY
CAL_HITTEST_INCMONTH = _calendar.CAL_HITTEST_INCMONTH
CAL_HITTEST_DECMONTH = _calendar.CAL_HITTEST_DECMONTH
CAL_HITTEST_SURROUNDING_WEEK = _calendar.CAL_HITTEST_SURROUNDING_WEEK
CAL_BORDER_NONE = _calendar.CAL_BORDER_NONE
CAL_BORDER_SQUARE = _calendar.CAL_BORDER_SQUARE
CAL_BORDER_ROUND = _calendar.CAL_BORDER_ROUND
class CalendarDateAttr(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalendarDateAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxColour colText, wxColour colBack=wxNullColour, wxColour colBorder=wxNullColour, 
    wxFont font=wxNullFont, 
    wxCalendarDateBorder border=CAL_BORDER_NONE) -> CalendarDateAttr"""
        newobj = _calendar.new_CalendarDateAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxColour colText)"""
        return _calendar.CalendarDateAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour colBack)"""
        return _calendar.CalendarDateAttr_SetBackgroundColour(*args, **kwargs)

    def SetBorderColour(*args, **kwargs):
        """SetBorderColour(wxColour col)"""
        return _calendar.CalendarDateAttr_SetBorderColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont font)"""
        return _calendar.CalendarDateAttr_SetFont(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """SetBorder(wxCalendarDateBorder border)"""
        return _calendar.CalendarDateAttr_SetBorder(*args, **kwargs)

    def SetHoliday(*args, **kwargs):
        """SetHoliday(bool holiday)"""
        return _calendar.CalendarDateAttr_SetHoliday(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour() -> bool"""
        return _calendar.CalendarDateAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour() -> bool"""
        return _calendar.CalendarDateAttr_HasBackgroundColour(*args, **kwargs)

    def HasBorderColour(*args, **kwargs):
        """HasBorderColour() -> bool"""
        return _calendar.CalendarDateAttr_HasBorderColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont() -> bool"""
        return _calendar.CalendarDateAttr_HasFont(*args, **kwargs)

    def HasBorder(*args, **kwargs):
        """HasBorder() -> bool"""
        return _calendar.CalendarDateAttr_HasBorder(*args, **kwargs)

    def IsHoliday(*args, **kwargs):
        """IsHoliday() -> bool"""
        return _calendar.CalendarDateAttr_IsHoliday(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour() -> wxColour"""
        return _calendar.CalendarDateAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour() -> wxColour"""
        return _calendar.CalendarDateAttr_GetBackgroundColour(*args, **kwargs)

    def GetBorderColour(*args, **kwargs):
        """GetBorderColour() -> wxColour"""
        return _calendar.CalendarDateAttr_GetBorderColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont() -> wxFont"""
        return _calendar.CalendarDateAttr_GetFont(*args, **kwargs)

    def GetBorder(*args, **kwargs):
        """GetBorder() -> wxCalendarDateBorder"""
        return _calendar.CalendarDateAttr_GetBorder(*args, **kwargs)


class CalendarDateAttrPtr(CalendarDateAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CalendarDateAttr
_calendar.CalendarDateAttr_swigregister(CalendarDateAttrPtr)

def CalendarDateAttrBorder(*args, **kwargs):
    """CalendarDateAttrBorder(wxCalendarDateBorder border, wxColour colBorder=wxNullColour) -> CalendarDateAttr"""
    val = _calendar.new_CalendarDateAttrBorder(*args, **kwargs)
    val.thisown = 1
    return val

class CalendarEvent(core.CommandEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalendarEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(CalendarCtrl cal, wxEventType type) -> CalendarEvent"""
        newobj = _calendar.new_CalendarEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDate(*args, **kwargs):
        """GetDate() -> DateTime"""
        return _calendar.CalendarEvent_GetDate(*args, **kwargs)

    def GetWeekDay(*args, **kwargs):
        """GetWeekDay() -> wxDateTime::WeekDay"""
        return _calendar.CalendarEvent_GetWeekDay(*args, **kwargs)


class CalendarEventPtr(CalendarEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CalendarEvent
_calendar.CalendarEvent_swigregister(CalendarEventPtr)

wxEVT_CALENDAR_DOUBLECLICKED = _calendar.wxEVT_CALENDAR_DOUBLECLICKED
wxEVT_CALENDAR_SEL_CHANGED = _calendar.wxEVT_CALENDAR_SEL_CHANGED
wxEVT_CALENDAR_DAY_CHANGED = _calendar.wxEVT_CALENDAR_DAY_CHANGED
wxEVT_CALENDAR_MONTH_CHANGED = _calendar.wxEVT_CALENDAR_MONTH_CHANGED
wxEVT_CALENDAR_YEAR_CHANGED = _calendar.wxEVT_CALENDAR_YEAR_CHANGED
wxEVT_CALENDAR_WEEKDAY_CLICKED = _calendar.wxEVT_CALENDAR_WEEKDAY_CLICKED
EVT_CALENDAR =                 wx.PyEventBinder( wxEVT_CALENDAR_DOUBLECLICKED, 1)
EVT_CALENDAR_SEL_CHANGED =     wx.PyEventBinder( wxEVT_CALENDAR_SEL_CHANGED, 1)
EVT_CALENDAR_DAY =             wx.PyEventBinder( wxEVT_CALENDAR_DAY_CHANGED, 1)
EVT_CALENDAR_MONTH =           wx.PyEventBinder( wxEVT_CALENDAR_MONTH_CHANGED, 1)
EVT_CALENDAR_YEAR =            wx.PyEventBinder( wxEVT_CALENDAR_YEAR_CHANGED, 1)
EVT_CALENDAR_WEEKDAY_CLICKED = wx.PyEventBinder( wxEVT_CALENDAR_WEEKDAY_CLICKED, 1)

class CalendarCtrl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalendarCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, DateTime date=wxDefaultDateTime, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=wxCAL_SHOW_HOLIDAYS|wxWANTS_CHARS, 
    wxString name=wxPyCalendarNameStr) -> CalendarCtrl"""
        newobj = _calendar.new_CalendarCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, DateTime date=wxDefaultDateTime, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=wxCAL_SHOW_HOLIDAYS|wxWANTS_CHARS, 
    wxString name=wxPyCalendarNameStr) -> bool"""
        return _calendar.CalendarCtrl_Create(*args, **kwargs)

    def SetDate(*args, **kwargs):
        """SetDate(DateTime date)"""
        return _calendar.CalendarCtrl_SetDate(*args, **kwargs)

    def GetDate(*args, **kwargs):
        """GetDate() -> DateTime"""
        return _calendar.CalendarCtrl_GetDate(*args, **kwargs)

    def SetLowerDateLimit(*args, **kwargs):
        """SetLowerDateLimit(DateTime date=wxDefaultDateTime) -> bool"""
        return _calendar.CalendarCtrl_SetLowerDateLimit(*args, **kwargs)

    def GetLowerDateLimit(*args, **kwargs):
        """GetLowerDateLimit() -> DateTime"""
        return _calendar.CalendarCtrl_GetLowerDateLimit(*args, **kwargs)

    def SetUpperDateLimit(*args, **kwargs):
        """SetUpperDateLimit(DateTime date=wxDefaultDateTime) -> bool"""
        return _calendar.CalendarCtrl_SetUpperDateLimit(*args, **kwargs)

    def GetUpperDateLimit(*args, **kwargs):
        """GetUpperDateLimit() -> DateTime"""
        return _calendar.CalendarCtrl_GetUpperDateLimit(*args, **kwargs)

    def SetDateRange(*args, **kwargs):
        """SetDateRange(DateTime lowerdate=wxDefaultDateTime, DateTime upperdate=wxDefaultDateTime) -> bool"""
        return _calendar.CalendarCtrl_SetDateRange(*args, **kwargs)

    def EnableYearChange(*args, **kwargs):
        """EnableYearChange(bool enable=True)"""
        return _calendar.CalendarCtrl_EnableYearChange(*args, **kwargs)

    def EnableMonthChange(*args, **kwargs):
        """EnableMonthChange(bool enable=True)"""
        return _calendar.CalendarCtrl_EnableMonthChange(*args, **kwargs)

    def EnableHolidayDisplay(*args, **kwargs):
        """EnableHolidayDisplay(bool display=True)"""
        return _calendar.CalendarCtrl_EnableHolidayDisplay(*args, **kwargs)

    def SetHeaderColours(*args, **kwargs):
        """SetHeaderColours(wxColour colFg, wxColour colBg)"""
        return _calendar.CalendarCtrl_SetHeaderColours(*args, **kwargs)

    def GetHeaderColourFg(*args, **kwargs):
        """GetHeaderColourFg() -> wxColour"""
        return _calendar.CalendarCtrl_GetHeaderColourFg(*args, **kwargs)

    def GetHeaderColourBg(*args, **kwargs):
        """GetHeaderColourBg() -> wxColour"""
        return _calendar.CalendarCtrl_GetHeaderColourBg(*args, **kwargs)

    def SetHighlightColours(*args, **kwargs):
        """SetHighlightColours(wxColour colFg, wxColour colBg)"""
        return _calendar.CalendarCtrl_SetHighlightColours(*args, **kwargs)

    def GetHighlightColourFg(*args, **kwargs):
        """GetHighlightColourFg() -> wxColour"""
        return _calendar.CalendarCtrl_GetHighlightColourFg(*args, **kwargs)

    def GetHighlightColourBg(*args, **kwargs):
        """GetHighlightColourBg() -> wxColour"""
        return _calendar.CalendarCtrl_GetHighlightColourBg(*args, **kwargs)

    def SetHolidayColours(*args, **kwargs):
        """SetHolidayColours(wxColour colFg, wxColour colBg)"""
        return _calendar.CalendarCtrl_SetHolidayColours(*args, **kwargs)

    def GetHolidayColourFg(*args, **kwargs):
        """GetHolidayColourFg() -> wxColour"""
        return _calendar.CalendarCtrl_GetHolidayColourFg(*args, **kwargs)

    def GetHolidayColourBg(*args, **kwargs):
        """GetHolidayColourBg() -> wxColour"""
        return _calendar.CalendarCtrl_GetHolidayColourBg(*args, **kwargs)

    def GetAttr(*args, **kwargs):
        """GetAttr(size_t day) -> CalendarDateAttr"""
        return _calendar.CalendarCtrl_GetAttr(*args, **kwargs)

    def SetAttr(*args, **kwargs):
        """SetAttr(size_t day, CalendarDateAttr attr)"""
        return _calendar.CalendarCtrl_SetAttr(*args, **kwargs)

    def SetHoliday(*args, **kwargs):
        """SetHoliday(size_t day)"""
        return _calendar.CalendarCtrl_SetHoliday(*args, **kwargs)

    def ResetAttr(*args, **kwargs):
        """ResetAttr(size_t day)"""
        return _calendar.CalendarCtrl_ResetAttr(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point pos, DateTime date=None, wxDateTime::WeekDay wd=None) -> wxCalendarHitTestResult"""
        return _calendar.CalendarCtrl_HitTest(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(bool enable=True) -> bool"""
        return _calendar.CalendarCtrl_Enable(*args, **kwargs)

    def Show(*args, **kwargs):
        """Show(bool show=True) -> bool"""
        return _calendar.CalendarCtrl_Show(*args, **kwargs)


class CalendarCtrlPtr(CalendarCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CalendarCtrl
_calendar.CalendarCtrl_swigregister(CalendarCtrlPtr)

def PreCalendarCtrl(*args, **kwargs):
    """PreCalendarCtrl() -> CalendarCtrl"""
    val = _calendar.new_PreCalendarCtrl(*args, **kwargs)
    val.thisown = 1
    return val


