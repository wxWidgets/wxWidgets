## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.grids

__doc__ =  wx.lib.grids.__doc__

wxFlexGridSizer = wx.lib.grids.PyFlexGridSizer
wxGridSizer = wx.lib.grids.PyGridSizer
