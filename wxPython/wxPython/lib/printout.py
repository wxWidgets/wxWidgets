## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.printout

__doc__ =  wx.lib.printout.__doc__

PrintBase = wx.lib.printout.PrintBase
PrintGrid = wx.lib.printout.PrintGrid
PrintTable = wx.lib.printout.PrintTable
PrintTableDraw = wx.lib.printout.PrintTableDraw
SetPrintout = wx.lib.printout.SetPrintout
