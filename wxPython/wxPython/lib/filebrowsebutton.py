## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.filebrowsebutton

__doc__ =  wx.lib.filebrowsebutton.__doc__

DirBrowseButton = wx.lib.filebrowsebutton.DirBrowseButton
FileBrowseButton = wx.lib.filebrowsebutton.FileBrowseButton
FileBrowseButtonWithHistory = wx.lib.filebrowsebutton.FileBrowseButtonWithHistory
