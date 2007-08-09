## This file imports items from the wx package into the wxPython package for
## backwards compatibility.  Some names will also have a 'wx' added on if
## that is how they used to be named in the old wxPython package.

import wx.lib.dialogs

__doc__ =  wx.lib.dialogs.__doc__

DialogResults = wx.lib.dialogs.DialogResults
alertDialog = wx.lib.dialogs.alertDialog
colorDialog = wx.lib.dialogs.colorDialog
colourDialog = wx.lib.dialogs.colourDialog
dirDialog = wx.lib.dialogs.dirDialog
directoryDialog = wx.lib.dialogs.directoryDialog
fileDialog = wx.lib.dialogs.fileDialog
findDialog = wx.lib.dialogs.findDialog
fontDialog = wx.lib.dialogs.fontDialog
messageDialog = wx.lib.dialogs.messageDialog
multipleChoiceDialog = wx.lib.dialogs.multipleChoiceDialog
openFileDialog = wx.lib.dialogs.openFileDialog
returnedString = wx.lib.dialogs.returnedString
saveFileDialog = wx.lib.dialogs.saveFileDialog
scrolledMessageDialog = wx.lib.dialogs.scrolledMessageDialog
singleChoiceDialog = wx.lib.dialogs.singleChoiceDialog
textEntryDialog = wx.lib.dialogs.textEntryDialog
wxMultipleChoiceDialog = wx.lib.dialogs.MultipleChoiceDialog
wxScrolledMessageDialog = wx.lib.dialogs.ScrolledMessageDialog
