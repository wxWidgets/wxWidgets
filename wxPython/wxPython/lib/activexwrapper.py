## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.activexwrapper

__doc__ =  wx.lib.activexwrapper.__doc__

MakeActiveXClass = wx.lib.activexwrapper.MakeActiveXClass
axw_Cleanup = wx.lib.activexwrapper.axw_Cleanup
axw_OEB = wx.lib.activexwrapper.axw_OEB
axw_OnSize = wx.lib.activexwrapper.axw_OnSize
axw__getattr__ = wx.lib.activexwrapper.axw__getattr__
axw__init__ = wx.lib.activexwrapper.axw__init__
