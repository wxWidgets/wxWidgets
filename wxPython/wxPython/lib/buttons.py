## This file reverse renames symbols in the wx package to give
## them their wx prefix again, for backwards compatibility.

import wx.lib.buttons


wxGenButtonEvent = wx.lib.buttons.GenButtonEvent
wxGenButton = wx.lib.buttons.GenButton
wxGenBitmapButton = wx.lib.buttons.GenBitmapButton
wxGenBitmapTextButton = wx.lib.buttons.GenBitmapTextButton
wxGenToggleButton = wx.lib.buttons.GenToggleButton
wxGenBitmapToggleButton = wx.lib.buttons.GenBitmapToggleButton
wxGenBitmapTextToggleButton = wx.lib.buttons.GenBitmapTextToggleButton
