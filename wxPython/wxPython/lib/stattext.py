## This file reverse renames symbols in the wx package to give
## them their wx prefix again, for backwards compatibility.

import wx.lib.stattext

wxGenStaticText = wx.lib.stattext.GenStaticText
