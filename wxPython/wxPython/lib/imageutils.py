## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.imageutils

__doc__ =  wx.lib.imageutils.__doc__

grayOut = wx.lib.imageutils.grayOut
makeGray = wx.lib.imageutils.makeGray
