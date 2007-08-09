## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.ErrorDialogs

__doc__ =  wx.lib.ErrorDialogs.__doc__

_createhtmlmail = wx.lib.ErrorDialogs._createhtmlmail
_sendmail = wx.lib.ErrorDialogs._sendmail
_startmailerwithhtml = wx.lib.ErrorDialogs._startmailerwithhtml
_writehtmlmessage = wx.lib.ErrorDialogs._writehtmlmessage
wxPyDestroyErrorDialogIfPresent = wx.lib.ErrorDialogs.wxPyDestroyErrorDialogIfPresent
wxPyFatalError = wx.lib.ErrorDialogs.wxPyFatalError
wxPyFatalErrorDialog = wx.lib.ErrorDialogs.wxPyFatalErrorDialog
wxPyFatalErrorDialogWithTraceback = wx.lib.ErrorDialogs.wxPyFatalErrorDialogWithTraceback
wxPyFatalOrNonFatalError = wx.lib.ErrorDialogs.wxPyFatalOrNonFatalError
wxPyNewErrorDialog = wx.lib.ErrorDialogs.wxPyNewErrorDialog
wxPyNonFatalError = wx.lib.ErrorDialogs.wxPyNonFatalError
wxPyNonFatalErrorDialog = wx.lib.ErrorDialogs.wxPyNonFatalErrorDialog
wxPyNonFatalErrorDialogWithTraceback = wx.lib.ErrorDialogs.wxPyNonFatalErrorDialogWithTraceback
wxPyNonWindowingError = wx.lib.ErrorDialogs.wxPyNonWindowingError
wxPyNonWindowingErrorHandler = wx.lib.ErrorDialogs.wxPyNonWindowingErrorHandler
wxPyResizeHTMLWindowToDispelScrollbar = wx.lib.ErrorDialogs.wxPyResizeHTMLWindowToDispelScrollbar
wxPythonRExec = wx.lib.ErrorDialogs.wxPythonRExec
