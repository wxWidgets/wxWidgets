## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.timectrl

__doc__ =  wx.lib.timectrl.__doc__

EVT_TIMEUPDATE = wx.lib.timectrl.EVT_TIMEUPDATE
TimeUpdatedEvent = wx.lib.timectrl.TimeUpdatedEvent
wxTimeCtrl = wx.lib.timectrl.wxTimeCtrl
