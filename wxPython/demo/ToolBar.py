# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx
import  images

#---------------------------------------------------------------------------

class TestToolBar(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, 'Test ToolBar', size=(500, 300))
        self.log = log
        self.timer = None
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        wx.Window(self, -1).SetBackgroundColour(wx.NamedColour("WHITE"))

        # Use the wxFrame internals to create the toolbar and associate it all
        # in one tidy method call.
        tb = self.CreateToolBar( wx.TB_HORIZONTAL
                                 | wx.NO_BORDER
                                 | wx.TB_FLAT
                                 | wx.TB_TEXT
                                 )

        # Here's a 'simple' toolbar example, and how to bind it using SetToolBar()
        #tb = wx.ToolBarSimple(self, -1, wx.DefaultPosition, wx.DefaultSize,
        #               wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT)
        #self.SetToolBar(tb)
        # But we're doing it a different way here.

        log.write("Default toolbar tool size: %s\n" % tb.GetToolBitmapSize())

        self.CreateStatusBar()

        tb.AddSimpleTool(10, images.getNewBitmap(), "New", "Long help for 'New'")
        #tb.AddLabelTool(10, "New", images.getNewBitmap(), shortHelp="New", longHelp="Long help for 'New'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=10)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=10)

        tb.AddSimpleTool(20, images.getOpenBitmap(), "Open", "Long help for 'Open'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=20)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=20)

        tb.AddSeparator()
        tb.AddSimpleTool(30, images.getCopyBitmap(), "Copy", "Long help for 'Copy'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=30)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=30)

        tb.AddSimpleTool(40, images.getPasteBitmap(), "Paste", "Long help for 'Paste'")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=40)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick, id=40)

        tb.AddSeparator()

        tool = tb.AddCheckTool(50, images.getTog1Bitmap(),
                               shortHelp="Toggle this")
        self.Bind(wx.EVT_TOOL, self.OnToolClick, id=50)

##         tb.AddCheckTool(60, images.getTog1Bitmap(), images.getTog2Bitmap(),
##                         shortHelp="Toggle with 2 bitmaps")
##         self.Bind(EVT_TOOL, self.OnToolClick, id=60)

        self.Bind(wx.EVT_TOOL_ENTER, self.OnToolEnter)
        self.Bind(wx.EVT_TOOL_RCLICKED, self.OnToolRClick) # Match all
        self.Bind(wx.EVT_TIMER, self.OnClearSB)

        tb.AddSeparator()
        cbID = wx.NewId()

        tb.AddControl(
            wx.ComboBox(
                tb, cbID, "", choices=["", "This", "is a", "wxComboBox"],
                size=(150,-1), style=wx.CB_DROPDOWN
                ))
                
        self.Bind(wx.EVT_COMBOBOX, self.OnCombo, id=cbID)
        tb.AddControl(wx.TextCtrl(tb, -1, "Toolbar controls!!", size=(150, -1)))

        # Final thing to do for a toolbar is call the Realize() method. This
        # causes it to render (more or less, that is).
        tb.Realize()


    def OnToolClick(self, event):
        self.log.WriteText("tool %s clicked\n" % event.GetId())
        tb = self.GetToolBar()
        tb.EnableTool(10, not tb.GetToolEnabled(10))

    def OnToolRClick(self, event):
        self.log.WriteText("tool %s right-clicked\n" % event.GetId())

    def OnCombo(self, event):
        self.log.WriteText("combobox item selected: %s\n" % event.GetString())

    def OnToolEnter(self, event):
        self.log.WriteText('OnToolEnter: %s, %s\n' % (event.GetId(), event.GetInt()))

        if self.timer is None:
            self.timer = wx.Timer(self)

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

