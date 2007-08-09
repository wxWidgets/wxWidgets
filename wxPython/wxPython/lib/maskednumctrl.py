## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.masked.numctrl

__doc__ =  wx.lib.masked.numctrl.__doc__

EVT_MASKEDNUM = wx.lib.masked.numctrl.EVT_NUM
wxMaskedNumCtrl = wx.lib.masked.numctrl.NumCtrl
wxMaskedNumNumberUpdatedEvent = wx.lib.masked.numctrl.NumberUpdatedEvent
