## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.sheet

__doc__ =  wx.lib.sheet.__doc__

CCellEditor = wx.lib.sheet.CCellEditor
CSheet = wx.lib.sheet.CSheet
CTextCellEditor = wx.lib.sheet.CTextCellEditor
