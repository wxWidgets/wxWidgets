
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestToolBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ToolBar',
                         wxPoint(0,0), wxSize(500, 300))
        self.log = log

        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        tb = self.CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER) #|wxTB_FLAT)
        #tb = wxToolBar(self, -1, wxDefaultPosition, wxDefaultSize,
        #               wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT)
        #self.SetToolBar(tb)

        self.CreateStatusBar()

        tb.AddSimpleTool(10, wxBitmap('bitmaps/new.bmp',   wxBITMAP_TYPE_BMP),
                         "New", "Long help for 'New'")
        EVT_TOOL(self, 10, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 10, self.OnToolRClick)

        tb.AddSimpleTool(20, wxBitmap('bitmaps/open.bmp',  wxBITMAP_TYPE_BMP), "Open")
        EVT_TOOL(self, 20, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 20, self.OnToolRClick)

        tb.AddSeparator()
        tb.AddSimpleTool(30, wxBitmap('bitmaps/copy.bmp',  wxBITMAP_TYPE_BMP), "Copy")
        EVT_TOOL(self, 30, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 30, self.OnToolRClick)

        tb.AddSimpleTool(40, wxBitmap('bitmaps/paste.bmp', wxBITMAP_TYPE_BMP), "Paste")
        EVT_TOOL(self, 40, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 40, self.OnToolRClick)

        tb.AddSeparator()

        tool = tb.AddTool(50, wxBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                          shortHelpString="Toggle this", toggle=true)
        EVT_TOOL(self, 50, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 50, self.OnToolRClick)

        tb.AddTool(60, wxBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                   wxBitmap('bitmaps/tog2.bmp', wxBITMAP_TYPE_BMP),
                   shortHelpString="Toggle with 2 bitmaps", toggle=true)
        EVT_TOOL(self, 60, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 60, self.OnToolRClick)


        tb.AddSeparator()
        tb.AddControl(wxComboBox(tb, -1, "", LIST=["", "This", "is a", "wxComboBox"],
                                 size=(150,-1), style=wxCB_DROPDOWN))

        tb.Realize()


    def OnCloseWindow(self, event):
        self.Destroy()

    def OnToolClick(self, event):
        self.log.WriteText("tool %s clicked\n" % event.GetId())

    def OnToolRClick(self, event):
        self.log.WriteText("tool %s right-clicked\n" % event.GetId())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestToolBar(frame, log)
    frame.otherWin = win
    win.Show(true)

#---------------------------------------------------------------------------
















overview = """\
The name wxToolBar is defined to be a synonym for one of the following classes:

wxToolBar95 The native Windows 95 toolbar. Used on Windows 95, NT 4 and above.

wxToolBarMSW A Windows implementation. Used on 16-bit Windows.

wxToolBarGTK The GTK toolbar.

wxToolBarSimple A simple implementation, with scrolling. Used on platforms with no native toolbar control, or where scrolling is required.

wxToolBar()
-----------------------

Default constructor.

wxToolBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTB_HORIZONTAL | wxNO_BORDER, const wxString& name = wxPanelNameStr)

Constructs a toolbar.

Parameters
-------------------

parent = Pointer to a parent window.

id = Window identifier. If -1, will automatically create an identifier.

pos = Window position. wxDefaultPosition is (-1, -1) which indicates that wxWindows should generate a default position for the window. If using the wxWindow class directly, supply an actual position.

size = Window size. wxDefaultSize is (-1, -1) which indicates that wxWindows should generate a default size for the window.

style = Window style. See wxToolBar for details.

name = Window name.
"""
