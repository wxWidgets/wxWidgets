## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.imagebrowser

__doc__ =  wx.lib.imagebrowser.__doc__

ConvertBMP = wx.lib.imagebrowser.ConvertBMP
FindFiles = wx.lib.imagebrowser.FindFiles
GetSize = wx.lib.imagebrowser.GetSize
ImageDialog = wx.lib.imagebrowser.ImageDialog
ImageView = wx.lib.imagebrowser.ImageView
OnFileDlg = wx.lib.imagebrowser.OnFileDlg
