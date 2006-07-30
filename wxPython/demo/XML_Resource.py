
from wxPython.wx import *
from wxPython.xrc import *

#----------------------------------------------------------------------

RESFILE = "data/resource_wdr.xrc"

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        # make the components
        label = wxStaticText(self, -1, "The lower panel was built from this XML:")
        label.SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD))

        text = wxTextCtrl(self, -1, open(RESFILE).read(),
                          style=wxTE_READONLY|wxTE_MULTILINE)
        text.SetInsertionPoint(0)

        line = wxStaticLine(self, -1)

        res = wxXmlResource(RESFILE)
        panel = res.LoadPanel(self, "MyPanel")

        # and do the layout
        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(label, 0, wxEXPAND|wxTOP|wxLEFT, 5)
        sizer.Add(text, 1, wxEXPAND|wxALL, 5)
        sizer.Add(line, 0, wxEXPAND)
        sizer.Add(panel, 1, wxEXPAND|wxALL, 5)

        self.SetSizer(sizer)
        self.SetAutoLayout(true)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """
"""
