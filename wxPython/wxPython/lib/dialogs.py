#----------------------------------------------------------------------
# Name:        wxPython.lib.dialogs
# Purpose:     wxScrolledMessageDialog, wxMultipleChoiceDialog and
#              function wrappers for the common dialogs by Kevin Altis.
#
# Author:      Various
#
# Created:     3-January-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

from wxPython import wx
from layoutf import Layoutf

#----------------------------------------------------------------------

class wxScrolledMessageDialog(wx.wxDialog):
    def __init__(self, parent, msg, caption, pos = wx.wxDefaultPosition, size = (500,300)):
        wx.wxDialog.__init__(self, parent, -1, caption, pos, size)
        x, y = pos
        if x == -1 and y == -1:
            self.CenterOnScreen(wx.wxBOTH)
        text = wx.wxTextCtrl(self, -1, msg, wx.wxDefaultPosition,
                             wx.wxDefaultSize,
                             wx.wxTE_MULTILINE | wx.wxTE_READONLY)
        ok = wx.wxButton(self, wx.wxID_OK, "OK")
        text.SetConstraints(Layoutf('t=t5#1;b=t5#2;l=l5#1;r=r5#1', (self,ok)))
        ok.SetConstraints(Layoutf('b=b5#1;x%w50#1;w!80;h!25', (self,)))
        self.SetAutoLayout(1)
        self.Layout()


class wxMultipleChoiceDialog(wx.wxDialog):
    def __init__(self, parent, msg, title, lst, pos = wx.wxDefaultPosition,
                 size = (200,200), style = wx.wxDEFAULT_DIALOG_STYLE):
        wx.wxDialog.__init__(self, parent, -1, title, pos, size, style)
        x, y = pos
        if x == -1 and y == -1:
            self.CenterOnScreen(wx.wxBOTH)
        dc = wx.wxClientDC(self)
        height = 0
        for line in msg.splitlines():
            height = height + dc.GetTextExtent(line)[1] + 2
        stat = wx.wxStaticText(self, -1, msg)
        self.lbox = wx.wxListBox(self, 100, wx.wxDefaultPosition,
                                 wx.wxDefaultSize, lst, wx.wxLB_MULTIPLE)
        ok = wx.wxButton(self, wx.wxID_OK, "OK")
        cancel = wx.wxButton(self, wx.wxID_CANCEL, "Cancel")
        stat.SetConstraints(Layoutf('t=t10#1;l=l5#1;r=r5#1;h!%d' % (height,),
                                   (self,)))
        self.lbox.SetConstraints(Layoutf('t=b10#2;l=l5#1;r=r5#1;b=t5#3',
                                 (self, stat, ok)))
        ok.SetConstraints(Layoutf('b=b5#1;x%w25#1;w!80;h!25', (self,)))
        cancel.SetConstraints(Layoutf('b=b5#1;x%w75#1;w!80;h!25', (self,)))
        self.SetAutoLayout(1)
        self.lst = lst
        self.Layout()

    def GetValue(self):
        return self.lbox.GetSelections()

    def GetValueString(self):
        sel = self.lbox.GetSelections()
        val = []
        for i in sel:
            val.append(self.lst[i])
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
        self.accepted = returned in (wx.wxID_OK, wx.wxID_YES)
        self.returnedString = returnedString(returned)

    def __repr__(self):
        return str(self.__dict__)

def returnedString(ret):
    if ret == wx.wxID_OK:
        return "Ok"
    elif ret == wx.wxID_CANCEL:
        return "Cancel"
    elif ret == wx.wxID_YES:
        return "Yes"
    elif ret == wx.wxID_NO:
        return "No"


# findDialog was created before wxPython got a Find/Replace dialog
# but it may be instructive as to how a function wrapper can
# be added for your own custom dialogs
# this dialog is always modal, while wxFindReplaceDialog is
# modeless and so doesn't lend itself to a function wrapper
def findDialog(parent=None, searchText='', wholeWordsOnly=0, caseSensitive=0):
    dlg = wx.wxDialog(parent, -1, "Find", wx.wxDefaultPosition, wx.wxSize(370, 120))

    wx.wxStaticText(dlg, -1, 'Find what:', wx.wxPoint(7, 10))
    wSearchText = wx.wxTextCtrl(dlg, -1, searchText,
                                wx.wxPoint(70, 7), wx.wxSize(195, -1))
    wSearchText.SetValue(searchText)
    wx.wxButton(dlg, wx.wxID_OK, "Find Next", wx.wxPoint(280, 5), wx.wxDefaultSize).SetDefault()
    wx.wxButton(dlg, wx.wxID_CANCEL, "Cancel", wx.wxPoint(280, 35), wx.wxDefaultSize)
    wWholeWord = wx.wxCheckBox(dlg, -1, 'Match whole word only',
                            wx.wxPoint(7, 35), wx.wxDefaultSize, wx.wxNO_BORDER)
    if wholeWordsOnly:
        wWholeWord.SetValue(1)
    wCase = wx.wxCheckBox(dlg, -1, 'Match case',
                        wx.wxPoint(7, 55), wx.wxDefaultSize, wx.wxNO_BORDER)
    if caseSensitive:
        wCase.SetValue(1)
    wSearchText.SetSelection(0, len(wSearchText.GetValue()))
    wSearchText.SetFocus()

    result = DialogResults(dlg.ShowModal())
    result.text = wSearchText.GetValue()
    result.wholeword = wWholeWord.GetValue()
    result.casesensitive = wCase.GetValue()
    dlg.Destroy()
    return result


def colorDialog(parent=None, colorData=None, color=None):
    if colorData:
        dialog = wx.wxColourDialog(parent, colorData)
    else:
        dialog = wx.wxColourDialog(parent)
        dialog.GetColourData().SetChooseFull(1)
    if color is not None:
        dialog.GetColourData().SetColour(color)
    result = DialogResults(dialog.ShowModal())
    result.colorData = dialog.GetColourData()
    result.color = result.colorData.GetColour().Get()
    dialog.Destroy()
    return result

# it is easier to just duplicate the code than
# try and replace color with colour in the result
def colourDialog(parent=None, colourData=None, colour=None):
    if colourData:
        dialog = wx.wxColourDialog(parent, colourData)
    else:
        dialog = wx.wxColourDialog(parent)
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
        fontData = wx.wxFontData()
    if font is not None:
        aFontData.SetInitialFont(font)
    dialog = wx.wxFontDialog(parent, fontData)
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


def textEntryDialog(parent=None, message='', title='', defaultText='', style=wx.wxOK | wx.wxCANCEL):
    dialog = wx.wxTextEntryDialog(parent, message, title, defaultText, style)
    result = DialogResults(dialog.ShowModal())
    result.text = dialog.GetValue()
    dialog.Destroy()
    return result


def messageDialog(parent=None, message='', title='Message box',
                  aStyle = wx.wxOK | wx.wxCANCEL | wx.wxCENTRE,
                  pos=wx.wxDefaultPosition):
    dialog = wx.wxMessageDialog(parent, message, title, aStyle, pos)
    result = DialogResults(dialog.ShowModal())
    dialog.Destroy()
    return result


# KEA alerts are common, so I'm providing a class rather than
# requiring the user code to set up the right icons and buttons
# the with messageDialog function
def alertDialog(parent=None, message='', title='Alert', pos=wx.wxDefaultPosition):
    return messageDialog(parent, message, title, wx.wxICON_EXCLAMATION | wx.wxOK, pos)


def scrolledMessageDialog(parent=None, message='', title='', pos=wx.wxDefaultPosition, size=(500,300)):
    dialog = wxScrolledMessageDialog(parent, message, title, pos, size)
    result = DialogResults(dialog.ShowModal())
    dialog.Destroy()
    return result


def fileDialog(parent=None, title='Open', directory='', filename='', wildcard='*.*',
                style=wx.wxOPEN | wx.wxMULTIPLE):
    dialog = wx.wxFileDialog(parent, title, directory, filename, wildcard, style)
    result = DialogResults(dialog.ShowModal())
    if result.accepted:
        result.paths = dialog.GetPaths()
    else:
        result.paths = None
    dialog.Destroy()
    return result


# openFileDialog and saveFileDialog are convenience functions
# they represent the most common usages of the fileDialog
# with the most common style options
def openFileDialog(parent=None, title='Open', directory='', filename='',
                    wildcard='All Files (*.*)|*.*',
                    style=wx.wxOPEN | wx.wxMULTIPLE):
    return fileDialog(parent, title, directory, filename, wildcard, style)


def saveFileDialog(parent=None, title='Save', directory='', filename='',
                    wildcard='All Files (*.*)|*.*',
                    style=wx.wxSAVE | wx.wxHIDE_READONLY | wx.wxOVERWRITE_PROMPT):
    return fileDialog(parent, title, directory, filename, wildcard, style)


def dirDialog(parent=None, message='Choose a directory', path='', style=0,
                pos=wx.wxDefaultPosition, size=wx.wxDefaultSize):
    dialog = wx.wxDirDialog(parent, message, path, style, pos, size)
    result = DialogResults(dialog.ShowModal())
    if result.accepted:
        result.path = dialog.GetPath()
    else:
        result.path = None
    dialog.Destroy()
    return result

directoryDialog = dirDialog


def singleChoiceDialog(parent=None, message='', title='', lst=[],
                  style=wx.wxOK | wx.wxCANCEL | wx.wxCENTRE):
    dialog = wx.wxSingleChoiceDialog(parent,
                                    message,
                                    title,
                                    lst,
                                    style)
    result = DialogResults(dialog.ShowModal())
    result.selection = dialog.GetStringSelection()
    dialog.Destroy()
    return result


def multipleChoiceDialog(parent=None, message='', title='', lst=[], pos=wx.wxDefaultPosition, size=(200,200)):
    dialog = wxMultipleChoiceDialog(parent, message, title, lst, pos, size)
    result = DialogResults(dialog.ShowModal())
    result.selection = dialog.GetValueString()
    dialog.Destroy()
    return result


if __name__ == '__main__':
    class MyApp(wx.wxApp):

        def OnInit(self):
            frame = wx.wxFrame(wx.NULL, -1, "Dialogs", size=(400, 200))
            panel = wx.wxPanel(frame, -1)
            self.panel = panel

            frame.Show(1)

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
            self.nameList = wx.wxListBox(panel, -1, (0, 0), (130, 180), dialogNames, style=wx.wxLB_SINGLE)
            wx.EVT_LISTBOX(panel, self.nameList.GetId(), self.OnNameListSelected)

            tstyle = wx.wxTE_RICH2 | wx.wxTE_PROCESS_TAB | wx.wxTE_MULTILINE
            self.text1 = wx.wxTextCtrl(panel, -1, pos=(150, 0), size=(200, 180), style=tstyle)

            self.SetTopWindow(frame)

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
                          'A Message Box', wx.wxOK | wx.wxICON_INFORMATION)
                          #wx.wxYES_NO | wx.wxNO_DEFAULT | wx.wxCANCEL | wx.wxICON_INFORMATION)
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

    app = MyApp(0)
    app.MainLoop()
