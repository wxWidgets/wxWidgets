
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestToolBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ToolBar',
                         wxPoint(0,0), wxSize(500, 300))
        self.log = log
        self.timer = None
        EVT_CLOSE(self, self.OnCloseWindow)

        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        tb = self.CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER) #|wxTB_FLAT)
        #tb = wxToolBarSimple(self, -1, wxDefaultPosition, wxDefaultSize,
        #               wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT)
        #self.SetToolBar(tb)

        self.CreateStatusBar()

        tb.AddSimpleTool(10, wxBitmap('bitmaps/new.bmp',   wxBITMAP_TYPE_BMP),
                         "New", "Long help for 'New'")
        EVT_TOOL(self, 10, self.OnToolClick)

        tb.AddSimpleTool(20, wxBitmap('bitmaps/open.bmp',  wxBITMAP_TYPE_BMP),
                         "Open", "Long help for 'Open'")
        EVT_TOOL(self, 20, self.OnToolClick)

        tb.AddSeparator()
        tb.AddSimpleTool(30, wxBitmap('bitmaps/copy.bmp',  wxBITMAP_TYPE_BMP),
                         "Copy", "Long help for 'Copy'")
        EVT_TOOL(self, 30, self.OnToolClick)

        tb.AddSimpleTool(40, wxBitmap('bitmaps/paste.bmp', wxBITMAP_TYPE_BMP),
                         "Paste", "Long help for 'Paste'")
        EVT_TOOL(self, 40, self.OnToolClick)

        tb.AddSeparator()

        tool = tb.AddTool(50, wxBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                          shortHelpString="Toggle this", isToggle=true)
        EVT_TOOL(self, 50, self.OnToolClick)

        tb.AddTool(60, wxBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                   wxBitmap('bitmaps/tog2.bmp', wxBITMAP_TYPE_BMP),
                   shortHelpString="Toggle with 2 bitmaps", isToggle=true)
        EVT_TOOL(self, 60, self.OnToolClick)

        EVT_TOOL_ENTER(self, -1, self.OnToolEnter)
        EVT_TOOL_RCLICKED(self, -1, self.OnToolRClick)  # Match all
        EVT_TIMER(self, -1, self.OnClearSB)

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
    win.Show(true)

#---------------------------------------------------------------------------
















overview = """\

"""
