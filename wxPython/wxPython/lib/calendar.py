## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.calendar

__doc__ =  wx.lib.calendar.__doc__

CalDraw = wx.lib.calendar.CalDraw
CalenDlg = wx.lib.calendar.CalenDlg
GetMonthList = wx.lib.calendar.GetMonthList
PrtCalDraw = wx.lib.calendar.PrtCalDraw
wxCalendar = wx.lib.calendar.wxCalendar
