"""PyAlaCarte is a simple programmer's editor."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

import os
import sys

import editor

try:
    True
except NameError:
    True = 1==1
    False = 1==0

class App(wx.wxApp):
    """PyAlaCarte standalone application."""

    def __init__(self, filename=None):
        self.filename = filename
        wx.wxApp.__init__(self, redirect=False)

    def OnInit(self):
        wx.wxInitAllImageHandlers()
        self.frame = editor.EditorFrame(filename=self.filename)
        self.frame.Show()
        self.SetTopWindow(self.frame)
        return True

def main(filename=None):
    app = App(filename)
    app.MainLoop()

if __name__ == '__main__':
    filename = None
    if len(sys.argv) > 1:
        filename = os.path.realpath(sys.argv[1])
    main(filename)
