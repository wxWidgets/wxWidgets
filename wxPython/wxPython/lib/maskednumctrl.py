## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.maskednumctrl

__doc__ =  wx.lib.maskednumctrl.__doc__

EVT_MASKEDNUM = wx.lib.maskednumctrl.EVT_MASKEDNUM
wxMaskedNumCtrl = wx.lib.maskednumctrl.wxMaskedNumCtrl
wxMaskedNumNumberUpdatedEvent = wx.lib.maskednumctrl.wxMaskedNumNumberUpdatedEvent
