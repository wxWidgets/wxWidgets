#!/usr/bin/env python

from wxPython.wx import *
from wxScrolledWindow import MyCanvas

import images
SHOW_BACKGROUND = 1

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

        self.CreateStatusBar()

        EVT_MENU(self, 5000, self.OnNewWindow)
        EVT_MENU(self, 5001, self.OnExit)

        if SHOW_BACKGROUND:
            self.bg_bmp = images.getGridBGBitmap()
            EVT_ERASE_BACKGROUND(self.GetClientWindow(), self.OnEraseBackground)


    def OnExit(self, evt):
        self.Close(True)


    def OnNewWindow(self, evt):
        self.winCount = self.winCount + 1
        win = wxMDIChildFrame(self, -1, "Child Window: %d" % self.winCount)
        canvas = MyCanvas(win)
        win.Show(True)


    def OnEraseBackground(self, evt):
        dc = evt.GetDC()
        if not dc:
            dc = wxClientDC(self.GetClientWindow())

        # tile the background bitmap
        sz = self.GetClientSize()
        w = self.bg_bmp.GetWidth()
        h = self.bg_bmp.GetHeight()
        x = 0
        while x < sz.width:
            y = 0
            while y < sz.height:
                dc.DrawBitmap(self.bg_bmp, x, y)
                y = y + h
            x = x + w


#----------------------------------------------------------------------

if __name__ == '__main__':
    class MyApp(wxApp):
        def OnInit(self):
            wxInitAllImageHandlers()
            frame = MyParentFrame()
            frame.Show(True)
            self.SetTopWindow(frame)
            return True


    app = MyApp(0)
    app.MainLoop()



