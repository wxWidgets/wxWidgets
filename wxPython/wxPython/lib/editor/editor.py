## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.editor.editor

__doc__ =  wx.lib.editor.editor.__doc__

ForceBetween = wx.lib.editor.editor.ForceBetween
LineSplitter = wx.lib.editor.editor.LineSplitter
LineTrimmer = wx.lib.editor.editor.LineTrimmer
Scroller = wx.lib.editor.editor.Scroller
wxEditor = wx.lib.editor.editor.Editor
