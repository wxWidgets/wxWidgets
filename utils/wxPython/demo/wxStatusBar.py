
from wxPython.wx import *

import time

#---------------------------------------------------------------------------

class CustomStatusBar(wxStatusBar):
    def __init__(self, parent, log):
        wxStatusBar.__init__(self, parent, -1)
        self.SetFieldsCount(3)
        self.log = log

        self.SetStatusText("A Custom StatusBar...", 0)

        self.cb = wxCheckBox(self, 1001, "toggle clock")
        EVT_CHECKBOX(self, 1001, self.OnToggleClock)
        self.cb.SetValue(true)

        # figure out how tall to make it.
        dc = wxClientDC(self)
        dc.SetFont(self.GetFont())
        (w,h) = dc.GetTextExtent('X')
        h = int(h * 1.8)
        self.SetSize(wxSize(100, h))

        # start our timer
        self.timer = wxPyTimer(self.Notify)
        self.timer.Start(1000)
        self.Notify()


    # Time-out handler
    def Notify(self):
        t = time.localtime(time.time())
        st = time.strftime("%d-%b-%Y   %I:%M:%S", t)
        self.SetStatusText(st, 2)
        self.log.WriteText("tick...\n")

    # the checkbox was clicked
    def OnToggleClock(self, event):
        if self.cb.GetValue():
            self.timer.Start(1000)
            self.Notify()
        else:
            self.timer.Stop()


    # reposition the checkbox
    def OnSize(self, event):
        rect = self.GetFieldRect(1)
        self.cb.SetPosition(wxPoint(rect.x+2, rect.y+2))
        self.cb.SetSize(wxSize(rect.width-4, rect.height-4))



class TestCustomStatusBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test Custom StatusBar',
                         wxPoint(0,0), wxSize(500, 300))
        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        self.sb = CustomStatusBar(self, log)
        self.SetStatusBar(self.sb)

    def OnCloseWindow(self, event):
        self.sb.timer.Stop()
        del self.sb.timer
        self.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestCustomStatusBar(frame, log)
    frame.otherWin = win
    win.Show(true)

#---------------------------------------------------------------------------









overview = """\
A status bar is a narrow window that can be placed along the bottom of a frame to give small amounts of status information. It can contain one or more fields, one or more of which can be variable length according to the size of the window.

wxStatusBar()
----------------------------

Default constructor.

wxStatusBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "statusBar")

Constructor, creating the window.

Parameters
-------------------

parent = The window parent, usually a frame.

id = The window identifier. It may take a value of -1 to indicate a default value.

pos = The window position. A value of (-1, -1) indicates a default position, chosen by either the windowing system or wxWindows, depending on platform.

size = The window size. A value of (-1, -1) indicates a default size, chosen by either the windowing system or wxWindows, depending on platform.

style = The window style. See wxStatusBar.

name = The name of the window. This parameter is used to associate a name with the item, allowing the application user to set Motif resource values for individual windows.
"""
