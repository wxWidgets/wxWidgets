## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.floatcanvas

__doc__ =  wx.lib.floatcanvas.__doc__

Circle = wx.lib.floatcanvas.Circle
Dot = wx.lib.floatcanvas.Dot
Ellipse = wx.lib.floatcanvas.Ellipse
FloatCanvas = wx.lib.floatcanvas.FloatCanvas
GetHandBitmap = wx.lib.floatcanvas.GetHandBitmap
GetHandData = wx.lib.floatcanvas.GetHandData
GetMinusBitmap = wx.lib.floatcanvas.GetMinusBitmap
GetMinusData = wx.lib.floatcanvas.GetMinusData
GetPlusBitmap = wx.lib.floatcanvas.GetPlusBitmap
GetPlusData = wx.lib.floatcanvas.GetPlusData
Line = wx.lib.floatcanvas.Line
LineSet = wx.lib.floatcanvas.LineSet
PointSet = wx.lib.floatcanvas.PointSet
Polygon = wx.lib.floatcanvas.Polygon
PolygonSet = wx.lib.floatcanvas.PolygonSet
Rectangle = wx.lib.floatcanvas.Rectangle
Text = wx.lib.floatcanvas.Text
draw_object = wx.lib.floatcanvas.draw_object
