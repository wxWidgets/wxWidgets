## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.rcsizer

__doc__ =  wx.lib.rcsizer.__doc__

RowColSizer = wx.lib.rcsizer.RowColSizer
