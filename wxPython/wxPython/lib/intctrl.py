## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.intctrl

__doc__ =  wx.lib.intctrl.__doc__

EVT_INT = wx.lib.intctrl.EVT_INT
wxIntCtrl = wx.lib.intctrl.IntCtrl
wxIntUpdatedEvent = wx.lib.intctrl.IntUpdatedEvent
wxIntValidator = wx.lib.intctrl.IntValidator
