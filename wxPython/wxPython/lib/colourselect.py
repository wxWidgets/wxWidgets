## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.colourselect

__doc__ =  wx.lib.colourselect.__doc__

ColourSelect = wx.lib.colourselect.ColourSelect
ColourSelectEvent = wx.lib.colourselect.ColourSelectEvent
EVT_COLOURSELECT = wx.lib.colourselect.EVT_COLOURSELECT
wxEVT_COMMAND_COLOURSELECT = wx.lib.colourselect.wxEVT_COMMAND_COLOURSELECT
