from wxPython.wx import *

import os.path

class CustomStatusBar(wxStatusBar):
    def __init__(self, parent):
        wxStatusBar.__init__(self, parent, -1)
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

