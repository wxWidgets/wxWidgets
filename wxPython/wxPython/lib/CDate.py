## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.CDate

__doc__ =  wx.lib.CDate.__doc__

Date = wx.lib.CDate.Date
FillDate = wx.lib.CDate.FillDate
FormatDay = wx.lib.CDate.FormatDay
FromJulian = wx.lib.CDate.FromJulian
TodayDay = wx.lib.CDate.TodayDay
dayOfWeek = wx.lib.CDate.dayOfWeek
daysPerMonth = wx.lib.CDate.daysPerMonth
isleap = wx.lib.CDate.isleap
julianDay = wx.lib.CDate.julianDay
leapdays = wx.lib.CDate.leapdays
now = wx.lib.CDate.now
