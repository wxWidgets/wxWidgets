#!/usr/bin/env python

import wx

import ScrolledWindow

#----------------------------------------------------------------------
# There are better ways to do IDs, but this demo requires that the window
# IDs be in a specific range. There are better ways to do that, too, but
# this will do for purposes of this demo.

ID_Menu_New         = 5004
ID_Menu_Exit        = 5005

ID_WINDOW_TOP       = 5000
ID_WINDOW_LEFT1     = 5001
ID_WINDOW_LEFT2     = 5002
ID_WINDOW_BOTTOM    = 5003

#----------------------------------------------------------------------

class MyParentFrame(wx.MDIParentFrame):
    def __init__(self):
        wx.MDIParentFrame.__init__(
            self, None, -1, "MDI Parent", size=(600,400),
            style = wx.DEFAULT_FRAME_STYLE | wx.HSCROLL | wx.VSCROLL
            )

        self.winCount = 0
        menu = wx.Menu()
        menu.Append(ID_Menu_New, "&New Window")
        menu.AppendSeparator()
        menu.Append(ID_Menu_Exit, "E&xit")

        menubar = wx.MenuBar()
        menubar.Append(menu, "&File")
        self.SetMenuBar(menubar)

        #self.CreateStatusBar()

        self.Bind(wx.EVT_MENU, self.OnNewWindow, id=ID_Menu_New)
        self.Bind(wx.EVT_MENU, self.OnExit, id=ID_Menu_Exit)

        self.Bind(
            wx.EVT_SASH_DRAGGED_RANGE, self.OnSashDrag, id=ID_WINDOW_TOP, 
            id2=ID_WINDOW_BOTTOM
            )

        self.Bind(wx.EVT_SIZE, self.OnSize)


        # Create some layout windows
        # A window like a toolbar
        win = wx.SashLayoutWindow(self, ID_WINDOW_TOP, style=wx.NO_BORDER|wx.SW_3D)
        win.SetDefaultSize((1000, 30))
        win.SetOrientation(wx.LAYOUT_HORIZONTAL)
        win.SetAlignment(wx.LAYOUT_TOP)
        win.SetBackgroundColour(wx.Colour(255, 0, 0))
        win.SetSashVisible(wx.SASH_BOTTOM, True)

        self.topWindow = win


        # A window like a statusbar
        win = wx.SashLayoutWindow(self, ID_WINDOW_BOTTOM, style=wx.NO_BORDER|wx.SW_3D)
        win.SetDefaultSize((1000, 30))
        win.SetOrientation(wx.LAYOUT_HORIZONTAL)
        win.SetAlignment(wx.LAYOUT_BOTTOM)
        win.SetBackgroundColour(wx.Colour(0, 0, 255))
        win.SetSashVisible(wx.SASH_TOP, True)

        self.bottomWindow = win


        # A window to the left of the client window
        win =  wx.SashLayoutWindow(self, ID_WINDOW_LEFT1, style=wx.NO_BORDER|wx.SW_3D)
        win.SetDefaultSize((120, 1000))
        win.SetOrientation(wx.LAYOUT_VERTICAL)
        win.SetAlignment(wx.LAYOUT_LEFT)
        win.SetBackgroundColour(wx.Colour(0, 255, 0))
        win.SetSashVisible(wx.SASH_RIGHT, True)
        win.SetExtraBorderSize(10)
        textWindow = wx.TextCtrl(win, -1, "", style=wx.TE_MULTILINE|wx.SUNKEN_BORDER)
        textWindow.SetValue("A sub window")

        self.leftWindow1 = win


        # Another window to the left of the client window
        win = wx.SashLayoutWindow(self, ID_WINDOW_LEFT2, style=wx.NO_BORDER|wx.SW_3D)
        win.SetDefaultSize((120, 1000))
        win.SetOrientation(wx.LAYOUT_VERTICAL)
        win.SetAlignment(wx.LAYOUT_LEFT)
        win.SetBackgroundColour(wx.Colour(0, 255, 255))
        win.SetSashVisible(wx.SASH_RIGHT, True)

        self.leftWindow2 = win


    def OnSashDrag(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return

        eID = event.GetId()
        
        if eID == ID_WINDOW_TOP:
            self.topWindow.SetDefaultSize((1000, event.GetDragRect().height))

        elif eID == ID_WINDOW_LEFT1:
            self.leftWindow1.SetDefaultSize((event.GetDragRect().width, 1000))

        elif eID == ID_WINDOW_LEFT2:
            self.leftWindow2.SetDefaultSize((event.GetDragRect().width, 1000))

        elif eID == ID_WINDOW_BOTTOM:
            self.bottomWindow.SetDefaultSize((1000, event.GetDragRect().height))

        wx.LayoutAlgorithm().LayoutMDIFrame(self)
        self.GetClientWindow().Refresh()


    def OnSize(self, event):
        wx.LayoutAlgorithm().LayoutMDIFrame(self)


    def OnExit(self, evt):
        self.Close(True)


    def OnNewWindow(self, evt):
        self.winCount = self.winCount + 1
        win = wx.MDIChildFrame(self, -1, "Child Window: %d" % self.winCount)
        canvas = ScrolledWindow.MyCanvas(win)
        win.Show(True)


#----------------------------------------------------------------------

if __name__ == '__main__':
    class MyApp(wx.App):
        def OnInit(self):
            wx.InitAllImageHandlers()
            frame = MyParentFrame()
            frame.Show(True)
            self.SetTopWindow(frame)
            return True

    app = MyApp(False)
    app.MainLoop()



