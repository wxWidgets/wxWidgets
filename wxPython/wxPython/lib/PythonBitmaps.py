## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.PythonBitmaps

__doc__ =  wx.lib.PythonBitmaps.__doc__

PythonBitmaps = wx.lib.PythonBitmaps.PythonBitmaps
getPythonPoweredBitmap = wx.lib.PythonBitmaps.getPythonPoweredBitmap
getPythonPoweredData = wx.lib.PythonBitmaps.getPythonPoweredData
getPythonPoweredImage = wx.lib.PythonBitmaps.getPythonPoweredImage
getwxPythonBitmap = wx.lib.PythonBitmaps.getwxPythonBitmap
getwxPythonData = wx.lib.PythonBitmaps.getwxPythonData
getwxPythonImage = wx.lib.PythonBitmaps.getwxPythonImage
