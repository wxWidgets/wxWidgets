#!/usr/bin/env python

from wxPython.wx import *
from wxScrolledWindow import MyCanvas

#----------------------------------------------------------------------

class MyParentFrame(wxMDIParentFrame):
    def __init__(self):
        wxMDIParentFrame.__init__(self, None, -1, "MDI Parent", size=(600,400))

        self.winCount = 0
        menu = wxMenu()
        menu.Append(5000, "&New Window")
        menu.AppendSeparator()
        menu.Append(5001, "E&xit")

        menubar = wxMenuBar()
        menubar.Append(menu, "&File")
        self.SetMenuBar(menubar)

        #self.CreateStatusBar()

        EVT_MENU(self, 5000, self.OnNewWindow)
        EVT_MENU(self, 5001, self.OnExit)


    def OnExit(self, evt):
        self.Close(true)


    def OnNewWindow(self, evt):
        self.winCount = self.winCount + 1
        win = wxMDIChildFrame(self, -1, "Child Window: %d" % self.winCount)
        canvas = MyCanvas(win)
        win.Show(true)


#----------------------------------------------------------------------

class MyApp(wxApp):
    def OnInit(self):
        frame = MyParentFrame()
        frame.Show(true)
        self.SetTopWindow(frame)
        return true


app = MyApp(0)
app.MainLoop()



