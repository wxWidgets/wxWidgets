## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.infoframe

__doc__ =  wx.lib.infoframe.__doc__

Dummy_wxPyInformationalMessagesFrame = wx.lib.infoframe.Dummy_PyInformationalMessagesFrame
_MyStatusBar = wx.lib.infoframe._MyStatusBar
wxPyInformationalMessagesFrame = wx.lib.infoframe.PyInformationalMessagesFrame
