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
    def __init__(self, *args, **kwargs):
        newobj = _calendar.new_CalendarDateAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetTextColour(*args, **kwargs): return _calendar.CalendarDateAttr_SetTextColour(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _calendar.CalendarDateAttr_SetBackgroundColour(*args, **kwargs)
    def SetBorderColour(*args, **kwargs): return _calendar.CalendarDateAttr_SetBorderColour(*args, **kwargs)
    def SetFont(*args, **kwargs): return _calendar.CalendarDateAttr_SetFont(*args, **kwargs)
    def SetBorder(*args, **kwargs): return _calendar.CalendarDateAttr_SetBorder(*args, **kwargs)
    def SetHoliday(*args, **kwargs): return _calendar.CalendarDateAttr_SetHoliday(*args, **kwargs)
    def HasTextColour(*args, **kwargs): return _calendar.CalendarDateAttr_HasTextColour(*args, **kwargs)
    def HasBackgroundColour(*args, **kwargs): return _calendar.CalendarDateAttr_HasBackgroundColour(*args, **kwargs)
    def HasBorderColour(*args, **kwargs): return _calendar.CalendarDateAttr_HasBorderColour(*args, **kwargs)
    def HasFont(*args, **kwargs): return _calendar.CalendarDateAttr_HasFont(*args, **kwargs)
    def HasBorder(*args, **kwargs): return _calendar.CalendarDateAttr_HasBorder(*args, **kwargs)
    def IsHoliday(*args, **kwargs): return _calendar.CalendarDateAttr_IsHoliday(*args, **kwargs)
    def GetTextColour(*args, **kwargs): return _calendar.CalendarDateAttr_GetTextColour(*args, **kwargs)
    def GetBackgroundColour(*args, **kwargs): return _calendar.CalendarDateAttr_GetBackgroundColour(*args, **kwargs)
    def GetBorderColour(*args, **kwargs): return _calendar.CalendarDateAttr_GetBorderColour(*args, **kwargs)
    def GetFont(*args, **kwargs): return _calendar.CalendarDateAttr_GetFont(*args, **kwargs)
    def GetBorder(*args, **kwargs): return _calendar.CalendarDateAttr_GetBorder(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalendarDateAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class CalendarDateAttrPtr(CalendarDateAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CalendarDateAttr
_calendar.CalendarDateAttr_swigregister(CalendarDateAttrPtr)

def CalendarDateAttrBorder(*args, **kwargs):
    val = _calendar.new_CalendarDateAttrBorder(*args, **kwargs)
    val.thisown = 1
    return val

class CalendarEvent(core.CommandEvent):
    def __init__(self, *args, **kwargs):
        newobj = _calendar.new_CalendarEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDate(*args, **kwargs): return _calendar.CalendarEvent_GetDate(*args, **kwargs)
    def GetWeekDay(*args, **kwargs): return _calendar.CalendarEvent_GetWeekDay(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalendarEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

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
    def __init__(self, *args, **kwargs):
        newobj = _calendar.new_CalendarCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _calendar.CalendarCtrl_Create(*args, **kwargs)
    def SetDate(*args, **kwargs): return _calendar.CalendarCtrl_SetDate(*args, **kwargs)
    def GetDate(*args, **kwargs): return _calendar.CalendarCtrl_GetDate(*args, **kwargs)
    def SetLowerDateLimit(*args, **kwargs): return _calendar.CalendarCtrl_SetLowerDateLimit(*args, **kwargs)
    def GetLowerDateLimit(*args, **kwargs): return _calendar.CalendarCtrl_GetLowerDateLimit(*args, **kwargs)
    def SetUpperDateLimit(*args, **kwargs): return _calendar.CalendarCtrl_SetUpperDateLimit(*args, **kwargs)
    def GetUpperDateLimit(*args, **kwargs): return _calendar.CalendarCtrl_GetUpperDateLimit(*args, **kwargs)
    def SetDateRange(*args, **kwargs): return _calendar.CalendarCtrl_SetDateRange(*args, **kwargs)
    def EnableYearChange(*args, **kwargs): return _calendar.CalendarCtrl_EnableYearChange(*args, **kwargs)
    def EnableMonthChange(*args, **kwargs): return _calendar.CalendarCtrl_EnableMonthChange(*args, **kwargs)
    def EnableHolidayDisplay(*args, **kwargs): return _calendar.CalendarCtrl_EnableHolidayDisplay(*args, **kwargs)
    def SetHeaderColours(*args, **kwargs): return _calendar.CalendarCtrl_SetHeaderColours(*args, **kwargs)
    def GetHeaderColourFg(*args, **kwargs): return _calendar.CalendarCtrl_GetHeaderColourFg(*args, **kwargs)
    def GetHeaderColourBg(*args, **kwargs): return _calendar.CalendarCtrl_GetHeaderColourBg(*args, **kwargs)
    def SetHighlightColours(*args, **kwargs): return _calendar.CalendarCtrl_SetHighlightColours(*args, **kwargs)
    def GetHighlightColourFg(*args, **kwargs): return _calendar.CalendarCtrl_GetHighlightColourFg(*args, **kwargs)
    def GetHighlightColourBg(*args, **kwargs): return _calendar.CalendarCtrl_GetHighlightColourBg(*args, **kwargs)
    def SetHolidayColours(*args, **kwargs): return _calendar.CalendarCtrl_SetHolidayColours(*args, **kwargs)
    def GetHolidayColourFg(*args, **kwargs): return _calendar.CalendarCtrl_GetHolidayColourFg(*args, **kwargs)
    def GetHolidayColourBg(*args, **kwargs): return _calendar.CalendarCtrl_GetHolidayColourBg(*args, **kwargs)
    def GetAttr(*args, **kwargs): return _calendar.CalendarCtrl_GetAttr(*args, **kwargs)
    def SetAttr(*args, **kwargs): return _calendar.CalendarCtrl_SetAttr(*args, **kwargs)
    def SetHoliday(*args, **kwargs): return _calendar.CalendarCtrl_SetHoliday(*args, **kwargs)
    def ResetAttr(*args, **kwargs): return _calendar.CalendarCtrl_ResetAttr(*args, **kwargs)
    def HitTest(*args, **kwargs): return _calendar.CalendarCtrl_HitTest(*args, **kwargs)
    def Enable(*args, **kwargs): return _calendar.CalendarCtrl_Enable(*args, **kwargs)
    def Show(*args, **kwargs): return _calendar.CalendarCtrl_Show(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalendarCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class CalendarCtrlPtr(CalendarCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CalendarCtrl
_calendar.CalendarCtrl_swigregister(CalendarCtrlPtr)

def PreCalendarCtrl(*args, **kwargs):
    val = _calendar.new_PreCalendarCtrl(*args, **kwargs)
    val.thisown = 1
    
    return val


