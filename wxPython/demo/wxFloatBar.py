from wxPython.wx import *
from wxPython.lib.floatbar import *

import images


class TestFloatBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ToolBar',
                         wxPoint(0,0), wxSize(500, 300))
        self.log = log

        win = wxWindow(self, -1)
        win.SetBackgroundColour(wxNamedColour("WHITE"))
        wxStaticText(win, -1, "Drag the toolbar to float it,\n"
                     "Toggle the last tool to remove\nthe title.", wxPoint(15,15))

        tb = wxFloatBar(self, -1)
        self.SetToolBar(tb)
        tb.SetFloatable(1)
        tb.SetTitle("Floating!")
        self.CreateStatusBar()

        tb.AddSimpleTool(10, images.getNewBitmap(), "New", "Long help for 'New'")
        EVT_TOOL(self, 10, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 10, self.OnToolRClick)

        tb.AddSimpleTool(20, images.getOpenBitmap(), "Open")
        EVT_TOOL(self, 20, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 20, self.OnToolRClick)

        tb.AddSeparator()
        tb.AddSimpleTool(30, images.getCopyBitmap(), "Copy")
        EVT_TOOL(self, 30, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 30, self.OnToolRClick)

        tb.AddSimpleTool(40, images.getPasteBitmap(), "Paste")
        EVT_TOOL(self, 40, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 40, self.OnToolRClick)

        tb.AddSeparator()


        tb.AddTool(60, images.getTog1Bitmap(), images.getTog2Bitmap(),
                   shortHelpString="Toggle with 2 bitmaps", isToggle=true)
        EVT_TOOL(self, 60, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 60, self.OnToolRClick)
        tb.Realize()

        self.tb = tb
        EVT_CLOSE(self, self.OnCloseWindow)


    def OnCloseWindow(self, event):
        self.Destroy()

    def OnToolClick(self, event):
        self.log.WriteText("tool %s clicked\n" % event.GetId())
        if event.GetId() == 60:
            print event.GetExtraLong(), event.Checked(), event.GetInt(), self.tb.GetToolState(60)
            if event.GetExtraLong():
                self.tb.SetTitle("")
            else:
                self.tb.SetTitle("Floating!")

    def OnToolRClick(self, event):
        self.log.WriteText("tool %s right-clicked\n" % event.GetId())
 #   def test(self, event):
 #       self.log.WriteText("Button clicked!")

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestFloatBar(frame, log)
    frame.otherWin = win
    win.Show(true)

#---------------------------------------------------------------------------

overview = """\
wxFloatBar is a subclass of wxToolBar, implemented in Python, which can be detached from its frame.

Drag the toolbar with the mouse to make it float, and drag it back, or close it to make the toolbar return to its original position.

"""

















