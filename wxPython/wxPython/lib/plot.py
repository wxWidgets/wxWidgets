## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.plot

__doc__ =  wx.lib.plot.__doc__

PolyPoints = wx.lib.plot.PolyPoints
PolyLine = wx.lib.plot.PolyLine
PolyMarker = wx.lib.plot.PolyMarker
PlotGraphics = wx.lib.plot.PlotGraphics
PlotCanvas = wx.lib.plot.PlotCanvas
PlotPrintout = wx.lib.plot.PlotPrintout
FloatDCWrapper = wx.lib.plot.FloatDCWrapper
_draw1Objects = wx.lib.plot._draw1Objects
_draw2Objects = wx.lib.plot._draw2Objects
_draw3Objects = wx.lib.plot._draw3Objects
_draw4Objects = wx.lib.plot._draw4Objects
_draw5Objects = wx.lib.plot._draw5Objects
__test = wx.lib.plot.__test

