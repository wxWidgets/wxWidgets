## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.popupctl

__doc__ =  wx.lib.popupctl.__doc__

PopButton = wx.lib.popupctl.PopButton
wxPopupControl = wx.lib.popupctl.wxPopupControl
wxPopupCtrl = wx.lib.popupctl.wxPopupCtrl
wxPopupDialog = wx.lib.popupctl.wxPopupDialog
