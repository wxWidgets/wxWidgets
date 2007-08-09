## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.evtmgr

__doc__ =  wx.lib.evtmgr.__doc__

EventAdapter = wx.lib.evtmgr.EventAdapter
EventMacroInfo = wx.lib.evtmgr.EventMacroInfo
EventManager = wx.lib.evtmgr.EventManager
FakeWindow = wx.lib.evtmgr.FakeWindow
MessageAdapter = wx.lib.evtmgr.MessageAdapter
eventManager  = wx.lib.evtmgr.eventManager
