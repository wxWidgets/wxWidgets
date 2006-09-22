# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
Classes for an interactive Calendar control.
"""

import _calendar
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _misc
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
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
    """
    A set of customization attributes for a calendar date, which can be
    used to control the look of the Calendar object.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            Colour colBorder=wxNullColour, Font font=wxNullFont, 
            int border=CAL_BORDER_NONE) -> CalendarDateAttr

        Create a CalendarDateAttr.
        """
        _calendar.CalendarDateAttr_swiginit(self,_calendar.new_CalendarDateAttr(*args, **kwargs))
    __swig_destroy__ = _calendar.delete_CalendarDateAttr
    __del__ = lambda self : None;
    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _calendar.CalendarDateAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _calendar.CalendarDateAttr_SetBackgroundColour(*args, **kwargs)

    def SetBorderColour(*args, **kwargs):
        """SetBorderColour(self, Colour col)"""
        return _calendar.CalendarDateAttr_SetBorderColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font font)"""
        return _calendar.CalendarDateAttr_SetFont(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """SetBorder(self, int border)"""
        return _calendar.CalendarDateAttr_SetBorder(*args, **kwargs)

    def SetHoliday(*args, **kwargs):
        """SetHoliday(self, bool holiday)"""
        return _calendar.CalendarDateAttr_SetHoliday(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour(self) -> bool"""
        return _calendar.CalendarDateAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour(self) -> bool"""
        return _calendar.CalendarDateAttr_HasBackgroundColour(*args, **kwargs)

    def HasBorderColour(*args, **kwargs):
        """HasBorderColour(self) -> bool"""
        return _calendar.CalendarDateAttr_HasBorderColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont(self) -> bool"""
        return _calendar.CalendarDateAttr_HasFont(*args, **kwargs)

    def HasBorder(*args, **kwargs):
        """HasBorder(self) -> bool"""
        return _calendar.CalendarDateAttr_HasBorder(*args, **kwargs)

    def IsHoliday(*args, **kwargs):
        """IsHoliday(self) -> bool"""
        return _calendar.CalendarDateAttr_IsHoliday(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _calendar.CalendarDateAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _calendar.CalendarDateAttr_GetBackgroundColour(*args, **kwargs)

    def GetBorderColour(*args, **kwargs):
        """GetBorderColour(self) -> Colour"""
        return _calendar.CalendarDateAttr_GetBorderColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _calendar.CalendarDateAttr_GetFont(*args, **kwargs)

    def GetBorder(*args, **kwargs):
        """GetBorder(self) -> int"""
        return _calendar.CalendarDateAttr_GetBorder(*args, **kwargs)

    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    Border = property(GetBorder,SetBorder,doc="See `GetBorder` and `SetBorder`") 
    BorderColour = property(GetBorderColour,SetBorderColour,doc="See `GetBorderColour` and `SetBorderColour`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
_calendar.CalendarDateAttr_swigregister(CalendarDateAttr)

class CalendarEvent(_core.DateEvent):
    """Proxy of C++ CalendarEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, CalendarCtrl cal, EventType type) -> CalendarEvent"""
        _calendar.CalendarEvent_swiginit(self,_calendar.new_CalendarEvent(*args, **kwargs))
    def SetWeekDay(*args, **kwargs):
        """SetWeekDay(self, int wd)"""
        return _calendar.CalendarEvent_SetWeekDay(*args, **kwargs)

    def GetWeekDay(*args, **kwargs):
        """GetWeekDay(self) -> int"""
        return _calendar.CalendarEvent_GetWeekDay(*args, **kwargs)

    def PySetDate(self, date):
        """takes datetime.datetime or datetime.date object"""
        self.SetDate(_pydate2wxdate(date))

    def PyGetDate(self):
        """returns datetime.date object"""
        return _wxdate2pydate(self.GetDate())

    WeekDay = property(GetWeekDay,SetWeekDay,doc="See `GetWeekDay` and `SetWeekDay`") 
_calendar.CalendarEvent_swigregister(CalendarEvent)

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

class CalendarCtrl(_core.Control):
    """
    The calendar control allows the user to pick a date interactively.

    The CalendarCtrl displays a window containing several parts: the
    control to pick the month and the year at the top (either or both of
    them may be disabled) and a month area below them which shows all the
    days in the month. The user can move the current selection using the
    keyboard and select the date (generating EVT_CALENDAR event) by
    pressing <Return> or double clicking it.

    It has advanced possibilities for the customization of its
    display. All global settings (such as colours and fonts used) can, of
    course, be changed. But also, the display style for each day in the
    month can be set independently using CalendarDateAttr class.

    An item without custom attributes is drawn with the default colours
    and font and without border, but setting custom attributes with
    `SetAttr` allows to modify its appearance. Just create a custom
    attribute object and set it for the day you want to be displayed
    specially A day may be marked as being a holiday, (even if it is not
    recognized as one by `wx.DateTime`) by using the SetHoliday method.

    As the attributes are specified for each day, they may change when the
    month is changed, so you will often want to update them in an
    EVT_CALENDAR_MONTH event handler.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, DateTime date=DefaultDateTime, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCAL_SHOW_HOLIDAYS|wxWANTS_CHARS, 
            String name=CalendarNameStr) -> CalendarCtrl

        Create and show a calendar control.
        """
        _calendar.CalendarCtrl_swiginit(self,_calendar.new_CalendarCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, DateTime date=DefaultDateTime, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCAL_SHOW_HOLIDAYS|wxWANTS_CHARS, 
            String name=CalendarNameStr) -> bool

        Acutally create the GUI portion of the CalendarCtrl for 2-phase
        creation.
        """
        return _calendar.CalendarCtrl_Create(*args, **kwargs)

    def SetDate(*args, **kwargs):
        """
        SetDate(self, DateTime date)

        Sets the current date.
        """
        return _calendar.CalendarCtrl_SetDate(*args, **kwargs)

    def GetDate(*args, **kwargs):
        """
        GetDate(self) -> DateTime

        Gets the currently selected date.
        """
        return _calendar.CalendarCtrl_GetDate(*args, **kwargs)

    def SetLowerDateLimit(*args, **kwargs):
        """
        SetLowerDateLimit(self, DateTime date=DefaultDateTime) -> bool

        set the range in which selection can occur
        """
        return _calendar.CalendarCtrl_SetLowerDateLimit(*args, **kwargs)

    def SetUpperDateLimit(*args, **kwargs):
        """
        SetUpperDateLimit(self, DateTime date=DefaultDateTime) -> bool

        set the range in which selection can occur
        """
        return _calendar.CalendarCtrl_SetUpperDateLimit(*args, **kwargs)

    def GetLowerDateLimit(*args, **kwargs):
        """
        GetLowerDateLimit(self) -> DateTime

        get the range in which selection can occur
        """
        return _calendar.CalendarCtrl_GetLowerDateLimit(*args, **kwargs)

    def GetUpperDateLimit(*args, **kwargs):
        """
        GetUpperDateLimit(self) -> DateTime

        get the range in which selection can occur
        """
        return _calendar.CalendarCtrl_GetUpperDateLimit(*args, **kwargs)

    def SetDateRange(*args, **kwargs):
        """
        SetDateRange(self, DateTime lowerdate=DefaultDateTime, DateTime upperdate=DefaultDateTime) -> bool

        set the range in which selection can occur
        """
        return _calendar.CalendarCtrl_SetDateRange(*args, **kwargs)

    def EnableYearChange(*args, **kwargs):
        """
        EnableYearChange(self, bool enable=True)

        This function should be used instead of changing CAL_NO_YEAR_CHANGE
        style bit directly. It allows or disallows the user to change the year
        interactively.
        """
        return _calendar.CalendarCtrl_EnableYearChange(*args, **kwargs)

    def EnableMonthChange(*args, **kwargs):
        """
        EnableMonthChange(self, bool enable=True)

        This function should be used instead of changing CAL_NO_MONTH_CHANGE
        style bit. It allows or disallows the user to change the month
        interactively. Note that if the month can not be changed, the year can
        not be changed either.
        """
        return _calendar.CalendarCtrl_EnableMonthChange(*args, **kwargs)

    def EnableHolidayDisplay(*args, **kwargs):
        """
        EnableHolidayDisplay(self, bool display=True)

        This function should be used instead of changing CAL_SHOW_HOLIDAYS
        style bit directly. It enables or disables the special highlighting of
        the holidays.
        """
        return _calendar.CalendarCtrl_EnableHolidayDisplay(*args, **kwargs)

    def SetHeaderColours(*args, **kwargs):
        """
        SetHeaderColours(self, Colour colFg, Colour colBg)

        Header colours are used for painting the weekdays at the top.
        """
        return _calendar.CalendarCtrl_SetHeaderColours(*args, **kwargs)

    def GetHeaderColourFg(*args, **kwargs):
        """
        GetHeaderColourFg(self) -> Colour

        Header colours are used for painting the weekdays at the top.
        """
        return _calendar.CalendarCtrl_GetHeaderColourFg(*args, **kwargs)

    def GetHeaderColourBg(*args, **kwargs):
        """
        GetHeaderColourBg(self) -> Colour

        Header colours are used for painting the weekdays at the top.
        """
        return _calendar.CalendarCtrl_GetHeaderColourBg(*args, **kwargs)

    def SetHighlightColours(*args, **kwargs):
        """
        SetHighlightColours(self, Colour colFg, Colour colBg)

        Highlight colour is used for the currently selected date.
        """
        return _calendar.CalendarCtrl_SetHighlightColours(*args, **kwargs)

    def GetHighlightColourFg(*args, **kwargs):
        """
        GetHighlightColourFg(self) -> Colour

        Highlight colour is used for the currently selected date.
        """
        return _calendar.CalendarCtrl_GetHighlightColourFg(*args, **kwargs)

    def GetHighlightColourBg(*args, **kwargs):
        """
        GetHighlightColourBg(self) -> Colour

        Highlight colour is used for the currently selected date.
        """
        return _calendar.CalendarCtrl_GetHighlightColourBg(*args, **kwargs)

    def SetHolidayColours(*args, **kwargs):
        """
        SetHolidayColours(self, Colour colFg, Colour colBg)

        Holiday colour is used for the holidays (if CAL_SHOW_HOLIDAYS style is
        used).
        """
        return _calendar.CalendarCtrl_SetHolidayColours(*args, **kwargs)

    def GetHolidayColourFg(*args, **kwargs):
        """
        GetHolidayColourFg(self) -> Colour

        Holiday colour is used for the holidays (if CAL_SHOW_HOLIDAYS style is
        used).
        """
        return _calendar.CalendarCtrl_GetHolidayColourFg(*args, **kwargs)

    def GetHolidayColourBg(*args, **kwargs):
        """
        GetHolidayColourBg(self) -> Colour

        Holiday colour is used for the holidays (if CAL_SHOW_HOLIDAYS style is
        used).
        """
        return _calendar.CalendarCtrl_GetHolidayColourBg(*args, **kwargs)

    def GetAttr(*args, **kwargs):
        """
        GetAttr(self, size_t day) -> CalendarDateAttr

        Returns the attribute for the given date (should be in the range
        1...31).  The returned value may be None
        """
        return _calendar.CalendarCtrl_GetAttr(*args, **kwargs)

    def SetAttr(*args, **kwargs):
        """
        SetAttr(self, size_t day, CalendarDateAttr attr)

        Associates the attribute with the specified date (in the range
        1...31).  If the attribute passed is None, the items attribute is
        cleared.
        """
        return _calendar.CalendarCtrl_SetAttr(*args, **kwargs)

    def SetHoliday(*args, **kwargs):
        """
        SetHoliday(self, size_t day)

        Marks the specified day as being a holiday in the current month.
        """
        return _calendar.CalendarCtrl_SetHoliday(*args, **kwargs)

    def ResetAttr(*args, **kwargs):
        """
        ResetAttr(self, size_t day)

        Clears any attributes associated with the given day (in the range
        1...31).
        """
        return _calendar.CalendarCtrl_ResetAttr(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(Point pos) -> (result, date, weekday)

        Returns 3-tuple with information about the given position on the
        calendar control.  The first value of the tuple is a result code and
        determines the validity of the remaining two values.
        """
        return _calendar.CalendarCtrl_HitTest(*args, **kwargs)

    def GetMonthControl(*args, **kwargs):
        """
        GetMonthControl(self) -> Control

        Get the currently shown control for month.
        """
        return _calendar.CalendarCtrl_GetMonthControl(*args, **kwargs)

    def GetYearControl(*args, **kwargs):
        """
        GetYearControl(self) -> Control

        Get the currently shown control for year.
        """
        return _calendar.CalendarCtrl_GetYearControl(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _calendar.CalendarCtrl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    def PySetDate(self, date):
        """takes datetime.datetime or datetime.date object"""
        self.SetDate(_pydate2wxdate(date))

    def PyGetDate(self):
        """returns datetime.date object"""
        return _wxdate2pydate(self.GetDate())

    def PySetLowerDateLimit(self, date):
        """takes datetime.datetime or datetime.date object"""
        self.SetLowerDateLimit(_pydate2wxdate(date))

    def PySetUpperDateLimit(self, date):
        """takes datetime.datetime or datetime.date object"""
        self.SetUpperDateLimit(_pydate2wxdate(date))

    def PySetDateRange(self, lowerdate, upperdate):
        """takes datetime.datetime or datetime.date objects"""
        self.PySetLowerDateLimit(lowerdate)
        self.PySetUpperDateLimit(upperdate)

    def PyGetLowerDateLimit(self):
        """returns datetime.date object"""
        return _wxdate2pydate(self.GetLowerDateLimit())

    def PyGetUpperDateLimit(self):
        """returns datetime.date object"""
        return _wxdate2pydate(self.GetUpperDateLimit())

    Date = property(GetDate,SetDate,doc="See `GetDate` and `SetDate`") 
    HeaderColourBg = property(GetHeaderColourBg,doc="See `GetHeaderColourBg`") 
    HeaderColourFg = property(GetHeaderColourFg,doc="See `GetHeaderColourFg`") 
    HighlightColourBg = property(GetHighlightColourBg,doc="See `GetHighlightColourBg`") 
    HighlightColourFg = property(GetHighlightColourFg,doc="See `GetHighlightColourFg`") 
    HolidayColourBg = property(GetHolidayColourBg,doc="See `GetHolidayColourBg`") 
    HolidayColourFg = property(GetHolidayColourFg,doc="See `GetHolidayColourFg`") 
    LowerDateLimit = property(GetLowerDateLimit,SetLowerDateLimit,doc="See `GetLowerDateLimit` and `SetLowerDateLimit`") 
    MonthControl = property(GetMonthControl,doc="See `GetMonthControl`") 
    UpperDateLimit = property(GetUpperDateLimit,SetUpperDateLimit,doc="See `GetUpperDateLimit` and `SetUpperDateLimit`") 
    YearControl = property(GetYearControl,doc="See `GetYearControl`") 
_calendar.CalendarCtrl_swigregister(CalendarCtrl)
cvar = _calendar.cvar
CalendarNameStr = cvar.CalendarNameStr

def PreCalendarCtrl(*args, **kwargs):
    """
    PreCalendarCtrl() -> CalendarCtrl

    Precreate a CalendarCtrl for 2-phase creation.
    """
    val = _calendar.new_PreCalendarCtrl(*args, **kwargs)
    return val

def CalendarCtrl_GetClassDefaultAttributes(*args, **kwargs):
  """
    CalendarCtrl_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _calendar.CalendarCtrl_GetClassDefaultAttributes(*args, **kwargs)

def _pydate2wxdate(date):
    import datetime
    assert isinstance(date, (datetime.datetime, datetime.date))
    tt = date.timetuple()
    dmy = (tt[2], tt[1]-1, tt[0])
    return wx.DateTimeFromDMY(*dmy)

def _wxdate2pydate(date):
    import datetime
    assert isinstance(date, wx.DateTime)
    if date.IsValid():
        ymd = map(int, date.FormatISODate().split('-'))
        return datetime.date(*ymd)
    else:
        return None



