# This file was created automatically by SWIG.
import calendarc

from misc import *

from windows import *

from gdi import *

from clip_dnd import *

from controls import *

from events import *

from utils import *
import wx

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


class wxCalendarDateAttrPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetTextColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_SetTextColour,(self,) + _args, _kwargs)
        return val
    def SetBackgroundColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_SetBackgroundColour,(self,) + _args, _kwargs)
        return val
    def SetBorderColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_SetBorderColour,(self,) + _args, _kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_SetFont,(self,) + _args, _kwargs)
        return val
    def SetBorder(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_SetBorder,(self,) + _args, _kwargs)
        return val
    def SetHoliday(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_SetHoliday,(self,) + _args, _kwargs)
        return val
    def HasTextColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_HasTextColour,(self,) + _args, _kwargs)
        return val
    def HasBackgroundColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_HasBackgroundColour,(self,) + _args, _kwargs)
        return val
    def HasBorderColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_HasBorderColour,(self,) + _args, _kwargs)
        return val
    def HasFont(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_HasFont,(self,) + _args, _kwargs)
        return val
    def HasBorder(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_HasBorder,(self,) + _args, _kwargs)
        return val
    def IsHoliday(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_IsHoliday,(self,) + _args, _kwargs)
        return val
    def GetTextColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_GetTextColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetBackgroundColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_GetBackgroundColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetBorderColour(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_GetBorderColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetBorder(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarDateAttr_GetBorder,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCalendarDateAttr instance at %s>" % (self.this,)
class wxCalendarDateAttr(wxCalendarDateAttrPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(calendarc.new_wxCalendarDateAttr,_args,_kwargs)
        self.thisown = 1



def wxCalendarDateAttrBorder(*_args,**_kwargs):
    val = wxCalendarDateAttrPtr(apply(calendarc.new_wxCalendarDateAttrBorder,_args,_kwargs))
    val.thisown = 1
    return val


class wxCalendarEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetDate(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarEvent_GetDate,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def GetWeekDay(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarEvent_GetWeekDay,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCalendarEvent instance at %s>" % (self.this,)
class wxCalendarEvent(wxCalendarEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(calendarc.new_wxCalendarEvent,_args,_kwargs)
        self.thisown = 1




class wxCalendarCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetDate(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_SetDate,(self,) + _args, _kwargs)
        return val
    def GetDate(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetDate,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def EnableYearChange(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_EnableYearChange,(self,) + _args, _kwargs)
        return val
    def EnableMonthChange(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_EnableMonthChange,(self,) + _args, _kwargs)
        return val
    def EnableHolidayDisplay(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_EnableHolidayDisplay,(self,) + _args, _kwargs)
        return val
    def SetHeaderColours(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_SetHeaderColours,(self,) + _args, _kwargs)
        return val
    def GetHeaderColourFg(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetHeaderColourFg,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetHeaderColourBg(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetHeaderColourBg,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetHighlightColours(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_SetHighlightColours,(self,) + _args, _kwargs)
        return val
    def GetHighlightColourFg(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetHighlightColourFg,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetHighlightColourBg(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetHighlightColourBg,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetHolidayColours(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_SetHolidayColours,(self,) + _args, _kwargs)
        return val
    def GetHolidayColourFg(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetHolidayColourFg,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetHolidayColourBg(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetHolidayColourBg,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetAttr(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_GetAttr,(self,) + _args, _kwargs)
        if val: val = wxCalendarDateAttrPtr(val) 
        return val
    def SetAttr(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_SetAttr,(self,) + _args, _kwargs)
        return val
    def SetHoliday(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_SetHoliday,(self,) + _args, _kwargs)
        return val
    def ResetAttr(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_ResetAttr,(self,) + _args, _kwargs)
        return val
    def HitTest(self, *_args, **_kwargs):
        val = apply(calendarc.wxCalendarCtrl_HitTest,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCalendarCtrl instance at %s>" % (self.this,)
class wxCalendarCtrl(wxCalendarCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(calendarc.new_wxCalendarCtrl,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxCAL_SUNDAY_FIRST = calendarc.wxCAL_SUNDAY_FIRST
wxCAL_MONDAY_FIRST = calendarc.wxCAL_MONDAY_FIRST
wxCAL_SHOW_HOLIDAYS = calendarc.wxCAL_SHOW_HOLIDAYS
wxCAL_NO_YEAR_CHANGE = calendarc.wxCAL_NO_YEAR_CHANGE
wxCAL_NO_MONTH_CHANGE = calendarc.wxCAL_NO_MONTH_CHANGE
wxCAL_HITTEST_NOWHERE = calendarc.wxCAL_HITTEST_NOWHERE
wxCAL_HITTEST_HEADER = calendarc.wxCAL_HITTEST_HEADER
wxCAL_HITTEST_DAY = calendarc.wxCAL_HITTEST_DAY
wxCAL_BORDER_NONE = calendarc.wxCAL_BORDER_NONE
wxCAL_BORDER_SQUARE = calendarc.wxCAL_BORDER_SQUARE
wxCAL_BORDER_ROUND = calendarc.wxCAL_BORDER_ROUND
wxEVT_CALENDAR_DOUBLECLICKED = calendarc.wxEVT_CALENDAR_DOUBLECLICKED
wxEVT_CALENDAR_SEL_CHANGED = calendarc.wxEVT_CALENDAR_SEL_CHANGED
wxEVT_CALENDAR_DAY_CHANGED = calendarc.wxEVT_CALENDAR_DAY_CHANGED
wxEVT_CALENDAR_MONTH_CHANGED = calendarc.wxEVT_CALENDAR_MONTH_CHANGED
wxEVT_CALENDAR_YEAR_CHANGED = calendarc.wxEVT_CALENDAR_YEAR_CHANGED
wxEVT_CALENDAR_WEEKDAY_CLICKED = calendarc.wxEVT_CALENDAR_WEEKDAY_CLICKED


#-------------- USER INCLUDE -----------------------


# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxCalendarEventPtr                  = wxCalendarEventPtr
