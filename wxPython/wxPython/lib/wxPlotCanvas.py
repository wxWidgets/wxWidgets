## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.wxPlotCanvas

__doc__ =  wx.lib.wxPlotCanvas.__doc__

PolyPoints = wx.lib.wxPlotCanvas.PolyPoints
PolyLine = wx.lib.wxPlotCanvas.PolyLine
PolyMarker = wx.lib.wxPlotCanvas.PolyMarker
PlotGraphics = wx.lib.wxPlotCanvas.PlotGraphics
PlotCanvas = wx.lib.wxPlotCanvas.PlotCanvas
