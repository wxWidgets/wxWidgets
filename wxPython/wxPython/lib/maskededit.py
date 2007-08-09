## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.
import wx.lib.masked
import wx.lib.masked.maskededit

__doc__ =  wx.lib.masked.maskededit.__doc__

from wx.lib.masked.maskededit import *

wxMaskedEditMixin = wx.lib.masked.MaskedEditMixin
wxMaskedTextCtrl = wx.lib.masked.TextCtrl
wxMaskedComboBox = wx.lib.masked.ComboBox
wxMaskedComboBoxSelectEvent = wx.lib.masked.MaskedComboBoxSelectEvent
wxIpAddrCtrl = wx.lib.masked.IpAddrCtrl
