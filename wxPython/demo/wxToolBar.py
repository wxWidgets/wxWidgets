
from wxPython.wx import *

import images

#---------------------------------------------------------------------------

class TestToolBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ToolBar', size=(500, 300))
        self.log = log
        self.timer = None
        EVT_CLOSE(self, self.OnCloseWindow)

        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        tb = self.CreateToolBar( wxTB_HORIZONTAL
                                 | wxNO_BORDER
                                 | wxTB_FLAT
                                 | wxTB_TEXT
                                 )
        #tb = wxToolBarSimple(self, -1, wxDefaultPosition, wxDefaultSize,
        #               wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT)
        #self.SetToolBar(tb)

        self.CreateStatusBar()

        tb.AddSimpleTool(10, images.getNewBitmap(), "New", "Long help for 'New'")
        #tb.AddLabelTool(10, "New", images.getNewBitmap(), shortHelp="New", longHelp="Long help for 'New'")
        EVT_TOOL(self, 10, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 10, self.OnToolRClick)

        tb.AddSimpleTool(20, images.getOpenBitmap(), "Open", "Long help for 'Open'")
        EVT_TOOL(self, 20, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 20, self.OnToolRClick)

        tb.AddSeparator()
        tb.AddSimpleTool(30, images.getCopyBitmap(), "Copy", "Long help for 'Copy'")
        EVT_TOOL(self, 30, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 30, self.OnToolRClick)

        tb.AddSimpleTool(40, images.getPasteBitmap(), "Paste", "Long help for 'Paste'")
        EVT_TOOL(self, 40, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 40, self.OnToolRClick)

        tb.AddSeparator()

        tool = tb.AddCheckTool(50, images.getTog1Bitmap(),
                               shortHelp="Toggle this")
        EVT_TOOL(self, 50, self.OnToolClick)

##         tb.AddCheckTool(60, images.getTog1Bitmap(), images.getTog2Bitmap(),
##                         shortHelp="Toggle with 2 bitmaps")
##         EVT_TOOL(self, 60, self.OnToolClick)

        EVT_TOOL_ENTER(self, -1, self.OnToolEnter)
        EVT_TOOL_RCLICKED(self, -1, self.OnToolRClick)  # Match all
        EVT_TIMER(self, -1, self.OnClearSB)

        if wxPlatform != "__WXMAC__":
            tb.AddSeparator()
            cbID = wxNewId()
            tb.AddControl(wxComboBox(tb, cbID, "", choices=["", "This", "is a", "wxComboBox"],
                                     size=(150,-1), style=wxCB_DROPDOWN))
            EVT_COMBOBOX(self, cbID, self.OnCombo)

        tb.Realize()


    def OnToolClick(self, event):
        self.log.WriteText("tool %s clicked\n" % event.GetId())

    def OnToolRClick(self, event):
        self.log.WriteText("tool %s right-clicked\n" % event.GetId())

    def OnCombo(self, event):
        self.log.WriteText("combobox item selected: %s\n" % event.GetString())

    def OnToolEnter(self, event):
        self.log.WriteText('OnToolEnter: %s, %s\n' % (event.GetId(), event.GetInt()))
        if self.timer is None:
            self.timer = wxTimer(self)
        if self.timer.IsRunning():
            self.timer.Stop()
        self.timer.Start(2000)
        event.Skip()


    def OnClearSB(self, event):  # called for the timer event handler
        self.SetStatusText("")
        self.timer.Stop()
        self.timer = None


    def OnCloseWindow(self, event):
        if self.timer is not None:
            self.timer.Stop()
            self.timer = None
        self.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestToolBar(frame, log)
    frame.otherWin = win
    win.Show(True)

#---------------------------------------------------------------------------






overview = """\

"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

