## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.throbber

__doc__ =  wx.lib.throbber.__doc__

EVT_UPDATE_THROBBER = wx.lib.throbber.EVT_UPDATE_THROBBER
Throbber = wx.lib.throbber.Throbber
UpdateThrobberEvent = wx.lib.throbber.UpdateThrobberEvent
