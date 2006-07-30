
from wxPython.wx import *


#---------------------------------------------------------------------------

class MySplitter(wxSplitterWindow):
    def __init__(self, parent, ID, log):
        wxSplitterWindow.__init__(self, parent, ID)
        self.log = log
        EVT_SPLITTER_SASH_POS_CHANGED(self, self.GetId(), self.OnSashChanged)

    def OnSashChanged(self, evt):
        self.log.WriteText("sash changed to " + str(evt.GetSashPosition()))

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    splitter = MySplitter(nb, -1, log)

    p1 = wxWindow(splitter, -1)
    p1.SetBackgroundColour(wxRED)
    wxStaticText(p1, -1, "Panel One", wxPoint(5,5)).SetBackgroundColour(wxRED)

    p2 = wxWindow(splitter, -1)
    p2.SetBackgroundColour(wxBLUE)
    wxStaticText(p2, -1, "Panel Two", wxPoint(5,5)).SetBackgroundColour(wxBLUE)

    splitter.SetMinimumPaneSize(20)
    splitter.SplitVertically(p1, p2)
    splitter.SetSashPosition(100)

    return splitter


#---------------------------------------------------------------------------















overview = """\
This class manages up to two subwindows. The current view can be split into two programmatically (perhaps from a menu command), and unsplit either programmatically or via the wxSplitterWindow user interface.

wxSplitterWindow()
-----------------------------------

Default constructor.

wxSplitterWindow(wxWindow* parent, wxWindowID id, int x, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style=wxSP_3D, const wxString& name = "splitterWindow")

Constructor for creating the window.

Parameters
-------------------

parent = The parent of the splitter window.

id = The window identifier.

pos = The window position.

size = The window size.

style = The window style. See wxSplitterWindow.

name = The window name.
"""
