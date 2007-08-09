## This file reverse renames symbols in the wx package to give
## them their wx prefix again, for backwards compatibility.

import wx.lib.stattext

__doc__ =  wx.lib.stattext.__doc__

wxGenStaticText = wx.lib.stattext.GenStaticText
