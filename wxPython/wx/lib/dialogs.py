#----------------------------------------------------------------------
# Name:        wx.lib.dialogs
# Purpose:     ScrolledMessageDialog, MultipleChoiceDialog and
#              function wrappers for the common dialogs by Kevin Altis.
#
# Author:      Various
#
# Created:     3-January-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/01/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for 2.5 compatability.
#
# 12/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxScrolledMessageDialog -> ScrolledMessageDialog
# o wxMultipleChoiceDialog -> MultipleChoiceDialog
#

import  wx
import  layoutf

#----------------------------------------------------------------------

class ScrolledMessageDialog(wx.Dialog):
    def __init__(self, parent, msg, caption,
                 pos=wx.DefaultPosition, size=(500,300),
                 style=wx.DEFAULT_DIALOG_STYLE):
        wx.Dialog.__init__(self, parent, -1, caption, pos, size, style)
        x, y = pos
        if x == -1 and y == -1:
            self.CenterOnScreen(wx.BOTH)

        self.text = text = wx.TextCtrl(self, -1, msg, 
                                       style=wx.TE_MULTILINE | wx.TE_READONLY)

        ok = wx.Button(self, wx.ID_OK, "OK")
        ok.SetDefault()
        lc = layoutf.Layoutf('t=t5#1;b=t5#2;l=l5#1;r=r5#1', (self,ok)) 
        text.SetConstraints(lc)

        lc = layoutf.Layoutf('b=b5#1;x%w50#1;w!80;h*', (self,))
        ok.SetConstraints(lc)
        self.SetAutoLayout(1)
        self.Layout()


class MultipleChoiceDialog(wx.Dialog):
    def __init__(self, parent, msg, title, lst, pos = wx.DefaultPosition,
                 size = (200,200), style = wx.DEFAULT_DIALOG_STYLE):
        wx.Dialog.__init__(self, parent, -1, title, pos, size, style)
        
        x, y = pos
        if x == -1 and y == -1:
            self.CenterOnScreen(wx.BOTH)

        stat = wx.StaticText(self, -1, msg)
        self.lbox = wx.ListBox(self, 100, wx.DefaultPosition, wx.DefaultSize, 
                               lst, wx.LB_MULTIPLE)

        ok = wx.Button(self, wx.ID_OK, "OK")
        ok.SetDefault()
        cancel = wx.Button(self, wx.ID_CANCEL, "Cancel")
        
        dlgsizer = wx.BoxSizer(wx.VERTICAL)
        dlgsizer.Add(stat, 0, wx.ALL, 4)
        dlgsizer.Add(self.lbox, 1, wx.EXPAND | wx.ALL, 4)
        
        btnsizer = wx.StdDialogButtonSizer()
        btnsizer.AddButton(ok)
        btnsizer.AddButton(cancel)
        btnsizer.Realize()
        
        dlgsizer.Add(btnsizer, 0, wx.ALL | wx.ALIGN_RIGHT, 4)
        
        self.SetSizer(dlgsizer)
        
        self.lst = lst
        self.Layout()

    def GetValue(self):
        return self.lbox.GetSelections()

    def GetValueString(self):
        sel = self.lbox.GetSelections()
        val = [ self.lst[i] for i in sel ]
        return tuple(val)


#----------------------------------------------------------------------
"""
function wrappers for wxPython system dialogs
Author: Kevin Altis
Date:   2003-1-2
Rev:    3

This is the third refactor of the PythonCard dialog.py module
for inclusion in the main wxPython distribution. There are a number of
design decisions and subsequent code refactoring to be done, so I'm
releasing this just to get some feedback.

rev 3:
- result dictionary replaced by DialogResults class instance
- should message arg be replaced with msg? most wxWindows dialogs
  seem to use the abbreviation?

rev 2:
- All dialog classes have been replaced by function wrappers
- Changed arg lists to more closely match wxWindows docs and wxPython.lib.dialogs
- changed 'returned' value to the actual button id the user clicked on
- added a returnedString value for the string version of the return value
- reworked colorDialog and fontDialog so you can pass in just a color or font
    for the most common usage case
- probably need to use colour instead of color to match the English English
    spelling in wxWindows (sigh)
- I still think we could lose the parent arg and just always use None
"""

class DialogResults:
    def __init__(self, returned):
        self.returned = returned
        self.accepted = returned in (wx.ID_OK, wx.ID_YES)
        self.returnedString = returnedString(returned)

    def __repr__(self):
        return str(self.__dict__)

def returnedString(ret):
    if ret == wx.ID_OK:
        return "Ok"
    elif ret == wx.ID_CANCEL:
        return "Cancel"
    elif ret == wx.ID_YES:
        return "Yes"
    elif ret == wx.ID_NO:
        return "No"


## findDialog was created before wxPython got a Find/Replace dialog
## but it may be instructive as to how a function wrapper can
## be added for your own custom dialogs
## this dialog is always modal, while wxFindReplaceDialog is
## modeless and so doesn't lend itself to a function wrapper
def findDialog(parent=None, searchText='', wholeWordsOnly=0, caseSensitive=0):
    dlg = wx.Dialog(parent, -1, "Find", wx.DefaultPosition, (380, 120))

    wx.StaticText(dlg, -1, 'Find what:', (7, 10))
    wSearchText = wx.TextCtrl(dlg, -1, searchText, (80, 7), (195, -1))
    wSearchText.SetValue(searchText)
    wx.Button(dlg, wx.ID_OK, "Find Next", (285, 5), wx.DefaultSize).SetDefault()
    wx.Button(dlg, wx.ID_CANCEL, "Cancel", (285, 35), wx.DefaultSize)

    wWholeWord = wx.CheckBox(dlg, -1, 'Match whole word only',
                            (7, 35), wx.DefaultSize, wx.NO_BORDER)

    if wholeWordsOnly:
        wWholeWord.SetValue(1)

    wCase = wx.CheckBox(dlg, -1, 'Match case', (7, 55), wx.DefaultSize, wx.NO_BORDER)

    if caseSensitive:
        wCase.SetValue(1)

    wSearchText.SetSelection(0, len(wSearchText.GetValue()))
    wSearchText.SetFocus()

    result = DialogResults(dlg.ShowModal())
    result.searchText = wSearchText.GetValue()
    result.wholeWordsOnly = wWholeWord.GetValue()
    result.caseSensitive = wCase.GetValue()
    dlg.Destroy()
    return result


def colorDialog(parent=None, colorData=None, color=None):
    if colorData:
        dialog = wx.ColourDialog(parent, colorData)
    else:
        dialog = wx.ColourDialog(parent)
        dialog.GetColourData().SetChooseFull(1)

    if color is not None:
        dialog.GetColourData().SetColour(color)

    result = DialogResults(dialog.ShowModal())
    result.colorData = dialog.GetColourData()
    result.color = result.colorData.GetColour().Get()
    dialog.Destroy()
    return result


## it is easier to just duplicate the code than
## try and replace color with colour in the result
def colourDialog(parent=None, colourData=None, colour=None):
    if colourData:
        dialog = wx.ColourDialog(parent, colourData)
    else:
        dialog = wx.ColourDialog(parent)
        dialog.GetColourData().SetChooseFull(1)

    if colour is not None:
        dialog.GetColourData().SetColour(color)

    result = DialogResults(dialog.ShowModal())
    result.colourData = dialog.GetColourData()
    result.colour = result.colourData.GetColour().Get()
    dialog.Destroy()
    return result


def fontDialog(parent=None, fontData=None, font=None):
    if fontData is None:
        fontData = wx.FontData()
        fontData.SetColour(wx.BLACK)
        fontData.SetInitialFont(wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT))

    if font is not None:
        fontData.SetInitialFont(font)

    dialog = wx.FontDialog(parent, fontData)
    result = DialogResults(dialog.ShowModal())

    if result.accepted:
        fontData = dialog.GetFontData()
        result.fontData = fontData
        result.color = fontData.GetColour().Get()
        result.colour = result.color
        result.font = fontData.GetChosenFont()
    else:
        result.color = None
        result.colour = None
        result.font = None

    dialog.Destroy()
    return result


def textEntryDialog(parent=None, message='', title='', defaultText='',
                    style=wx.OK | wx.CANCEL):
    dialog = wx.TextEntryDialog(parent, message, title, defaultText, style)
    result = DialogResults(dialog.ShowModal())
    result.text = dialog.GetValue()
    dialog.Destroy()
    return result


def messageDialog(parent=None, message='', title='Message box',
                  aStyle = wx.OK | wx.CANCEL | wx.CENTRE,
                  pos=wx.DefaultPosition):
    dialog = wx.MessageDialog(parent, message, title, aStyle, pos)
    result = DialogResults(dialog.ShowModal())
    dialog.Destroy()
    return result


## KEA: alerts are common, so I'm providing a class rather than
## requiring the user code to set up the right icons and buttons
## the with messageDialog function
def alertDialog(parent=None, message='', title='Alert', pos=wx.DefaultPosition):
    return messageDialog(parent, message, title, wx.ICON_EXCLAMATION | wx.OK, pos)


def scrolledMessageDialog(parent=None, message='', title='', pos=wx.DefaultPosition,
                          size=(500,300)):

    dialog = ScrolledMessageDialog(parent, message, title, pos, size)
    result = DialogResults(dialog.ShowModal())
    dialog.Destroy()
    return result


def fileDialog(parent=None, title='Open', directory='', filename='', wildcard='*.*',
               style=wx.OPEN | wx.MULTIPLE):

    dialog = wx.FileDialog(parent, title, directory, filename, wildcard, style)
    result = DialogResults(dialog.ShowModal())
    if result.accepted:
        result.paths = dialog.GetPaths()
    else:
        result.paths = None
    dialog.Destroy()
    return result


## openFileDialog and saveFileDialog are convenience functions
## they represent the most common usages of the fileDialog
## with the most common style options
def openFileDialog(parent=None, title='Open', directory='', filename='',
                   wildcard='All Files (*.*)|*.*',
                   style=wx.OPEN | wx.MULTIPLE):
    return fileDialog(parent, title, directory, filename, wildcard, style)


def saveFileDialog(parent=None, title='Save', directory='', filename='',
                   wildcard='All Files (*.*)|*.*',
                   style=wx.SAVE | wx.OVERWRITE_PROMPT):
    return fileDialog(parent, title, directory, filename, wildcard, style)


def dirDialog(parent=None, message='Choose a directory', path='', style=0,
              pos=wx.DefaultPosition, size=wx.DefaultSize):

    dialog = wx.DirDialog(parent, message, path, style, pos, size)
    result = DialogResults(dialog.ShowModal())
    if result.accepted:
        result.path = dialog.GetPath()
    else:
        result.path = None
    dialog.Destroy()
    return result

directoryDialog = dirDialog


def singleChoiceDialog(parent=None, message='', title='', lst=[], 
                       style=wx.OK | wx.CANCEL | wx.CENTRE):
    dialog = wx.SingleChoiceDialog(parent, message, title, list(lst), style | wx.DEFAULT_DIALOG_STYLE)
    result = DialogResults(dialog.ShowModal())
    result.selection = dialog.GetStringSelection()
    dialog.Destroy()
    return result


def multipleChoiceDialog(parent=None, message='', title='', lst=[],
                         pos=wx.DefaultPosition, size=wx.DefaultSize):

    dialog = wx.MultiChoiceDialog(parent, message, title, lst,
                                  wx.CHOICEDLG_STYLE, pos)
    result = DialogResults(dialog.ShowModal())
    result.selection = tuple([lst[i] for i in dialog.GetSelections()])
    dialog.Destroy()
    return result


if __name__ == '__main__':
    #import os
    #print os.getpid()
    
    class MyApp(wx.App):

        def OnInit(self):
            self.frame = frame = wx.Frame(None, -1, "Dialogs", size=(400, 240))
            panel = wx.Panel(frame, -1)
            self.panel = panel


            dialogNames = [
                'alertDialog',
                'colorDialog',
                'directoryDialog',
                'fileDialog',
                'findDialog',
                'fontDialog',
                'messageDialog',
                'multipleChoiceDialog',
                'openFileDialog',
                'saveFileDialog',
                'scrolledMessageDialog',
                'singleChoiceDialog',
                'textEntryDialog',
            ]

            self.nameList = wx.ListBox(panel, -1,
                                       size=(130, 180),
                                       choices=dialogNames,
                                       style=wx.LB_SINGLE)
            self.Bind(wx.EVT_LISTBOX, self.OnNameListSelected, self.nameList)

            tstyle = wx.TE_RICH2 | wx.TE_PROCESS_TAB | wx.TE_MULTILINE
            self.text1 = wx.TextCtrl(panel, -1, size=(200, 180), style=tstyle)

            sizer = wx.BoxSizer(wx.HORIZONTAL)
            sizer.Add(self.nameList, 0, wx.EXPAND|wx.ALL, 20)
            sizer.Add(self.text1, 1,  wx.EXPAND|wx.ALL, 20)

            panel.SetSizer(sizer)

            self.SetTopWindow(frame)
            frame.Show(1)
            return 1


        def OnNameListSelected(self, evt):
            import pprint
            sel = evt.GetString()
            result = None
            if sel == 'alertDialog':
                result = alertDialog(message='Danger Will Robinson')
            elif sel == 'colorDialog':
                result = colorDialog()
            elif sel == 'directoryDialog':
                result = directoryDialog()
            elif sel == 'fileDialog':
                wildcard = "JPG files (*.jpg;*.jpeg)|*.jpeg;*.JPG;*.JPEG;*.jpg|GIF files (*.gif)|*.GIF;*.gif|All Files (*.*)|*.*"
                result = fileDialog(None, 'Open', '', '', wildcard)
            elif sel == 'findDialog':
                result = findDialog()
            elif sel == 'fontDialog':
                result = fontDialog()
            elif sel == 'messageDialog':
                result = messageDialog(None, 'Hello from Python and wxPython!',
                          'A Message Box', wx.OK | wx.ICON_INFORMATION)
                          #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION)
                #result = messageDialog(None, 'message', 'title')
            elif sel == 'multipleChoiceDialog':
                result = multipleChoiceDialog(None, "message", "title", ['one', 'two', 'three'])
            elif sel == 'openFileDialog':
                result = openFileDialog()
            elif sel == 'saveFileDialog':
                result = saveFileDialog()
            elif sel == 'scrolledMessageDialog':
                msg = "Can't find the file dialog.py"
                try:
                    # read this source file and then display it
                    import sys
                    filename = sys.argv[-1]
                    fp = open(filename)
                    message = fp.read()
                    fp.close()
                except:
                    pass
                result = scrolledMessageDialog(None, message, filename)
            elif sel == 'singleChoiceDialog':
                result = singleChoiceDialog(None, "message", "title", ['one', 'two', 'three'])
            elif sel == 'textEntryDialog':
                result = textEntryDialog(None, "message", "title", "text")

            if result:
                #self.text1.SetValue(pprint.pformat(result.__dict__))
                self.text1.SetValue(str(result))

    app = MyApp(True)
    app.MainLoop()


