## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.multisash

__doc__ =  wx.lib.multisash.__doc__

EmptyChild = wx.lib.multisash.EmptyChild
MultiClient = wx.lib.multisash.MultiClient
MultiCloser = wx.lib.multisash.MultiCloser
MultiCreator = wx.lib.multisash.MultiCreator
MultiSizer = wx.lib.multisash.MultiSizer
wxMultiSash = wx.lib.multisash.MultiSash
wxMultiSplit = wx.lib.multisash.MultiSplit
wxMultiViewLeaf = wx.lib.multisash.MultiViewLeaf
