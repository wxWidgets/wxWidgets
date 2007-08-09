## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.colourchooser.pypalette

__doc__ =  wx.lib.colourchooser.pypalette.__doc__

PyPalette = wx.lib.colourchooser.pypalette.PyPalette
getBitmap = wx.lib.colourchooser.pypalette.getBitmap
getData = wx.lib.colourchooser.pypalette.getData
getImage = wx.lib.colourchooser.pypalette.getImage
