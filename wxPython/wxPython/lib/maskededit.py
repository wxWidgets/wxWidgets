## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.maskededit

__doc__ =  wx.lib.maskededit.__doc__

Field = wx.lib.maskededit.Field
test = wx.lib.maskededit.test
test2 = wx.lib.maskededit.test2
wxIpAddrCtrl = wx.lib.maskededit.wxIpAddrCtrl
wxMaskedComboBox = wx.lib.maskededit.wxMaskedComboBox
wxMaskedComboBoxSelectEvent = wx.lib.maskededit.wxMaskedComboBoxSelectEvent
wxMaskedEditMixin = wx.lib.maskededit.wxMaskedEditMixin
wxMaskedTextCtrl = wx.lib.maskededit.wxMaskedTextCtrl
