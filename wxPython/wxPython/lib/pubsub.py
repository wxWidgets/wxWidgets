## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.pubsub

__doc__ =  wx.lib.pubsub.__doc__

Message = wx.lib.pubsub.Message
Topic = wx.lib.pubsub.Topic
