
from wxPython.wx import *

import time

#---------------------------------------------------------------------------

class CustomStatusBar(wxStatusBar):
    def __init__(self, parent, log):
        wxStatusBar.__init__(self, parent, -1)
        self.SetFieldsCount(3)
        self.log = log
        self.sizeChanged = false
        EVT_SIZE(self, self.OnSize)
        EVT_IDLE(self, self.OnIdle)

        self.SetStatusText("A Custom StatusBar...", 0)

        self.cb = wxCheckBox(self, 1001, "toggle clock")
        EVT_CHECKBOX(self, 1001, self.OnToggleClock)
        self.cb.SetValue(true)

        # set the initial position of the checkbox
        self.Reposition()

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


    def OnSize(self, evt):
        self.Reposition()  # for normal size events

        # Set a flag so the idle time handler will also do the repositioning.
        # It is done this way to get around a buglet where GetFieldRect is not
        # accurate during the EVT_SIZE resulting from a frame maximize.
        self.sizeChanged = true


    def OnIdle(self, evt):
        if self.sizeChanged:
            self.Reposition()


    # reposition the checkbox
    def Reposition(self):
        rect = self.GetFieldRect(1)
        self.cb.SetPosition(wxPoint(rect.x+2, rect.y+2))
        self.cb.SetSize(wxSize(rect.width-4, rect.height-4))
        self.sizeChanged = false



class TestCustomStatusBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test Custom StatusBar',
                         wxPoint(0,0), wxSize(500, 300))
        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        self.sb = CustomStatusBar(self, log)
        self.SetStatusBar(self.sb)
        EVT_CLOSE(self, self.OnCloseWindow)

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
"""
