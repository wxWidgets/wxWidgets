
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestSashWindow(wxPanel):
    ID_WINDOW_TOP    = 5100
    ID_WINDOW_LEFT1  = 5101
    ID_WINDOW_LEFT2  = 5102
    ID_WINDOW_BOTTOM = 5103


    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log

        # will occupy the space not used by the Layout Algorithm
        self.remainingSpace = wxPanel(self, -1, style=wxSUNKEN_BORDER)

        EVT_SASH_DRAGGED_RANGE(self, self.ID_WINDOW_TOP,
                               self.ID_WINDOW_BOTTOM, self.OnSashDrag)
        EVT_SIZE(self, self.OnSize)


        # Create some layout windows
        # A window like a toolbar
        win = wxSashLayoutWindow(self, self.ID_WINDOW_TOP, wxDefaultPosition,
                                 wxSize(200, 30), wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(1000, 30))
        win.SetOrientation(wxLAYOUT_HORIZONTAL)
        win.SetAlignment(wxLAYOUT_TOP)
        win.SetBackgroundColour(wxColour(255, 0, 0))
        win.SetSashVisible(wxSASH_BOTTOM, true)

        self.topWindow = win


        # A window like a statusbar
        win = wxSashLayoutWindow(self, self.ID_WINDOW_BOTTOM,
                                 wxDefaultPosition, wxSize(200, 30),
                                 wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(1000, 30))
        win.SetOrientation(wxLAYOUT_HORIZONTAL)
        win.SetAlignment(wxLAYOUT_BOTTOM)
        win.SetBackgroundColour(wxColour(0, 0, 255))
        win.SetSashVisible(wxSASH_TOP, true)

        self.bottomWindow = win


        # A window to the left of the client window
        win =  wxSashLayoutWindow(self, self.ID_WINDOW_LEFT1,
                                  wxDefaultPosition, wxSize(200, 30),
                                  wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(120, 1000))
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
        win = wxSashLayoutWindow(self, self.ID_WINDOW_LEFT2,
                                 wxDefaultPosition, wxSize(200, 30),
                                 wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(120, 1000))
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

        wxLayoutAlgorithm().LayoutWindow(self, self.remainingSpace)
        self.remainingSpace.Refresh()

    def OnSize(self, event):
        wxLayoutAlgorithm().LayoutWindow(self, self.remainingSpace)

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestSashWindow(nb, log)
    return win

#---------------------------------------------------------------------------
















overview = """\
"""
