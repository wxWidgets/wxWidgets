## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.mixins.rubberband

__doc__ =  wx.lib.mixins.rubberband.__doc__

RubberBand = wx.lib.mixins.rubberband.RubberBand
boxToExtent = wx.lib.mixins.rubberband.boxToExtent
getCursorPosition = wx.lib.mixins.rubberband.getCursorPosition
isNegative = wx.lib.mixins.rubberband.isNegative
normalizeBox = wx.lib.mixins.rubberband.normalizeBox
pointInBox = wx.lib.mixins.rubberband.pointInBox
pointOnBox = wx.lib.mixins.rubberband.pointOnBox
