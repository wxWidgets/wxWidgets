#!/usr/bin/env python

from wxPython.wx import *
from wxScrolledWindow import MyCanvas

#----------------------------------------------------------------------

class MyParentFrame(wxMDIParentFrame):
    ID_WINDOW_TOP    = 5100
    ID_WINDOW_LEFT1  = 5101
    ID_WINDOW_LEFT2  = 5102
    ID_WINDOW_BOTTOM = 5103

    def __init__(self):
        wxMDIParentFrame.__init__(self, None, -1, "MDI Parent", size=(600,400),
                                  style = wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL)

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


        EVT_SASH_DRAGGED_RANGE(self,
                               self.ID_WINDOW_TOP, self.ID_WINDOW_BOTTOM,
                               self.OnSashDrag)
        EVT_SIZE(self, self.OnSize)


        # Create some layout windows
        # A window like a toolbar
        win = wxSashLayoutWindow(self, self.ID_WINDOW_TOP, style = wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize((1000, 30))
        win.SetOrientation(wxLAYOUT_HORIZONTAL)
        win.SetAlignment(wxLAYOUT_TOP)
        win.SetBackgroundColour(wxColour(255, 0, 0))
        win.SetSashVisible(wxSASH_BOTTOM, true)

        self.topWindow = win


        # A window like a statusbar
        win = wxSashLayoutWindow(self, self.ID_WINDOW_BOTTOM, style = wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize((1000, 30))
        win.SetOrientation(wxLAYOUT_HORIZONTAL)
        win.SetAlignment(wxLAYOUT_BOTTOM)
        win.SetBackgroundColour(wxColour(0, 0, 255))
        win.SetSashVisible(wxSASH_TOP, true)

        self.bottomWindow = win


        # A window to the left of the client window
        win =  wxSashLayoutWindow(self, self.ID_WINDOW_LEFT1, style = wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize((120, 1000))
        win.SetOrientation(wxLAYOUT_VERTICAL)
        win.SetAlignment(wxLAYOUT_LEFT)
        win.SetBackgroundColour(wxColour(0, 255, 0))
        win.SetSashVisible(wxSASH_RIGHT, TRUE)
        win.SetExtraBorderSize(10)
        textWindow = wxTextCtrl(win, -1, "", wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE|wxSUNKEN_BORDER)
        textWindow.SetValue("A sub window")

        self.leftWindow1 = win


        # Another window to the left of the client window
        win = wxSashLayoutWindow(self, self.ID_WINDOW_LEFT2, style = wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize((120, 1000))
        win.SetOrientation(wxLAYOUT_VERTICAL)
        win.SetAlignment(wxLAYOUT_LEFT)
        win.SetBackgroundColour(wxColour(0, 255, 255))
        win.SetSashVisible(wxSASH_RIGHT, TRUE)

        self.leftWindow2 = win


    def OnSashDrag(self, event):
        if event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE:
            return

        eID = event.GetId()
        if eID == self.ID_WINDOW_TOP:
            self.topWindow.SetDefaultSize(wxSize(1000, event.GetDragRect().height))

        elif eID == self.ID_WINDOW_LEFT1:
            self.leftWindow1.SetDefaultSize(wxSize(event.GetDragRect().width, 1000))


        elif eID == self.ID_WINDOW_LEFT2:
            self.leftWindow2.SetDefaultSize(wxSize(event.GetDragRect().width, 1000))

        elif eID == self.ID_WINDOW_BOTTOM:
            self.bottomWindow.SetDefaultSize(wxSize(1000, event.GetDragRect().height))

        wxLayoutAlgorithm().LayoutMDIFrame(self)
        self.GetClientWindow().Refresh()


    def OnSize(self, event):
        wxLayoutAlgorithm().LayoutMDIFrame(self)


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



