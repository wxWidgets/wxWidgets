## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.calendar

__doc__ =  wx.lib.calendar.__doc__

wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED = wx.lib.calendar.wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED
EVT_CALENDAR = wx.lib.calendar.EVT_CALENDAR


Date = wx.lib.calendar.Date
FillDate = wx.lib.calendar.FillDate
FormatDay = wx.lib.calendar.FormatDay
FromJulian = wx.lib.calendar.FromJulian
TodayDay = wx.lib.calendar.TodayDay
dayOfWeek = wx.lib.calendar.dayOfWeek
daysPerMonth = wx.lib.calendar.daysPerMonth
isleap = wx.lib.calendar.isleap
julianDay = wx.lib.calendar.julianDay
leapdays = wx.lib.calendar.leapdays
now = wx.lib.calendar.now
Month = wx.lib.calendar.Month
mdays = wx.lib.calendar.mdays
day_name = wx.lib.calendar.day_name
day_abbr = wx.lib.calendar.day_abbr


CalDraw = wx.lib.calendar.CalDraw
CalenDlg = wx.lib.calendar.CalenDlg
GetMonthList = wx.lib.calendar.GetMonthList
PrtCalDraw = wx.lib.calendar.PrtCalDraw
wxCalendar = wx.lib.calendar.Calendar

wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED = wx.lib.calendar.wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED
EVT_CALENDAR = wx.lib.calendar.EVT_CALENDAR
