## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.masked.ctrl

__doc__ =  wx.lib.masked.ctrl.__doc__

MASKEDTEXT  = wx.lib.masked.ctrl.TEXT
MASKEDCOMBO = wx.lib.masked.ctrl.COMBO
IPADDR      = wx.lib.masked.ctrl.IPADDR
TIME        = wx.lib.masked.ctrl.TIME
NUMBER      = wx.lib.masked.ctrl.NUMBER

class controlTypes:
    MASKEDTEXT  = wx.lib.masked.ctrl.TEXT
    MASKEDCOMBO = wx.lib.masked.ctrl.COMBO
    IPADDR      = wx.lib.masked.ctrl.IPADDR
    TIME        = wx.lib.masked.ctrl.TIME
    NUMBER      = wx.lib.masked.ctrl.NUMBER

wxMaskedCtrl = wx.lib.masked.Ctrl
