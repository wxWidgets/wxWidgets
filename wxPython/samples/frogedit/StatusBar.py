import os.path
import wx

class CustomStatusBar(wx.StatusBar):
    def __init__(self, parent):
        wx.StatusBar.__init__(self, parent, -1)
        self.SetFieldsCount(3)

    def setFileName(self, fn):
        path, fileName = os.path.split(fn)
        self.SetStatusText(fileName, 0)

    def setRowCol(self, row, col):
        self.SetStatusText("%d,%d" % (row,col), 1)

    def setDirty(self, dirty):
        if dirty:
            self.SetStatusText("...", 2)
        else:
            self.SetStatusText(" ", 2)

