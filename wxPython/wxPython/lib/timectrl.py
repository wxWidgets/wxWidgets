## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.masked.timectrl

__doc__ =  wx.lib.masked.timectrl.__doc__

EVT_TIMEUPDATE = wx.lib.masked.timectrl.EVT_TIMEUPDATE
TimeUpdatedEvent = wx.lib.masked.timectrl.TimeUpdatedEvent
wxTimeCtrl = wx.lib.masked.timectrl.TimeCtrl
