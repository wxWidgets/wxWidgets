## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.ErrorDialogs_wdr

__doc__ =  wx.lib.ErrorDialogs_wdr.__doc__

populate_wxPyFatalError = wx.lib.ErrorDialogs_wdr.populate_wxPyFatalError
populate_wxPyFatalErrorDialog = wx.lib.ErrorDialogs_wdr.populate_wxPyFatalErrorDialog
populate_wxPyFatalErrorDialogWithTraceback = wx.lib.ErrorDialogs_wdr.populate_wxPyFatalErrorDialogWithTraceback
populate_wxPyNonFatalError = wx.lib.ErrorDialogs_wdr.populate_wxPyNonFatalError
populate_wxPyNonFatalErrorDialog = wx.lib.ErrorDialogs_wdr.populate_wxPyNonFatalErrorDialog
populate_wxPyNonFatalErrorDialogWithTraceback = wx.lib.ErrorDialogs_wdr.populate_wxPyNonFatalErrorDialogWithTraceback
