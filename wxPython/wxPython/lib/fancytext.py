## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.fancytext

__doc__ = wx.lib.fancytext.__doc__

DCRenderer = wx.lib.fancytext.DCRenderer
GetExtent = wx.lib.fancytext.GetExtent
GetFullExtent = wx.lib.fancytext.GetFullExtent
RenderToBitmap = wx.lib.fancytext.RenderToBitmap
RenderToDC = wx.lib.fancytext.RenderToDC
RenderToRenderer = wx.lib.fancytext.RenderToRenderer
Renderer = wx.lib.fancytext.Renderer
SizeRenderer = wx.lib.fancytext.SizeRenderer
StaticFancyText = wx.lib.fancytext.StaticFancyText
_addGreek = wx.lib.fancytext._addGreek
getExtent = wx.lib.fancytext.getExtent
iceil = wx.lib.fancytext.iceil
iround = wx.lib.fancytext.iround
renderToBitmap = wx.lib.fancytext.renderToBitmap
renderToDC = wx.lib.fancytext.renderToDC
test = wx.lib.fancytext.test
