
# simple text editor
#
# Copyright 2001 Adam Feuer and Steve Howell
#
# License: Python

import re
import wx

from wx.lib.editor import Editor

#---------------------------------------------------------------------

class FrogEditor(Editor):
    def __init__(self, parent, id,
                 pos=wx.DefaultPosition, size=wx.DefaultSize, style=0, statusBar=None):
        self.StatusBar = statusBar
        Editor.__init__(self, parent, id, pos, size, style)
        self.parent = parent

    ##------------------------------------

    def TouchBuffer(self):
        Editor.TouchBuffer(self)
        self.StatusBar.setDirty(1)

    def UnTouchBuffer(self):
        Editor.UnTouchBuffer(self)
        self.StatusBar.setDirty(0)


    #--------- utility function -------------

    # override our base class method
    def DrawCursor(self, dc = None):
        Editor.DrawCursor(self,dc)
        self.StatusBar.setRowCol(self.cy,self.cx)

    def lastLine(self):
        lastline = self.sy + self.sh - 1
        return min(lastline, self.LinesInFile() - 1)

    def rawLines(self):
        return [l.text for l in self.text]

    def save(self):
        if self.page:
            self.ds.store(self.page,self.rawLines())

    def SetRawText(self, rawtext=""):
        self.rawText= rawtext
        self.SetText(self.RenderText())

    def RenderText(self):
        return(self.rawText)

    #---------- logging -------------

    def SetStatus(self, log):
        self.log = log
        self.status = []

    def PrintSeparator(self, event):
        self.Print("..........................")

    def Print(self, data):
        self.status.append(data)
        if data[-1:] == '\n':
            data = data[:-1]
        wx.LogMessage(data)

    #--------- wxEditor keyboard overrides

    def SetControlFuncs(self, action):
        Editor.SetControlFuncs(self, action)
        action['-'] = self.PrintSeparator

    def SetAltFuncs(self, action):
        Editor.SetAltFuncs(self, action)
        action['x'] = self.Exit

    #----------- commands -----------

    def OnCloseWindow(self, event):
        # xxx - We don't fully understand how exit logic works.
        # This event is actually called by our parent frame.
        pass

    def Exit(self,event):
        self.parent.Close(None)

