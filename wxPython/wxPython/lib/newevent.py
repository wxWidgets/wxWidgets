## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.newevent

__doc__ =  wx.lib.newevent.__doc__

NewCommandEvent = wx.lib.newevent.NewCommandEvent
NewEvent = wx.lib.newevent.NewEvent
_test = wx.lib.newevent._test
