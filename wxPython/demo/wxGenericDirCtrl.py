from wxPython.wx import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        txt1 = wxStaticText(self, -1, "style=0")
        dir1 = wxGenericDirCtrl(self, -1, size=(200,225), style=0)

        txt2 = wxStaticText(self, -1, "wxDIRCTRL_DIR_ONLY")
        dir2 = wxGenericDirCtrl(self, -1, size=(200,225), style=wxDIRCTRL_DIR_ONLY)

        txt3 = wxStaticText(self, -1, "wxDIRCTRL_SHOW_FILTERS")
        dir3 = wxGenericDirCtrl(self, -1, size=(200,225), style=wxDIRCTRL_SHOW_FILTERS,
                                filter="All files (*.*)|*.*|Python files (*.py)|*.py")

        sz = wxFlexGridSizer(cols=3, hgap=5, vgap=5)
        sz.Add(35, 35)  # some space above
        sz.Add(35, 35)
        sz.Add(35, 35)

        sz.Add(txt1)
        sz.Add(txt2)
        sz.Add(txt3)

        sz.Add(dir1, 0, wxEXPAND)
        sz.Add(dir2, 0, wxEXPAND)
        sz.Add(dir3, 0, wxEXPAND)

        sz.Add(35,35)  # some space below

        sz.AddGrowableRow(2)
        sz.AddGrowableCol(0)
        sz.AddGrowableCol(1)
        sz.AddGrowableCol(2)
        self.SetSizer(sz)
        self.SetAutoLayout(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------





overview = """\
This control can be used to place a directory listing (with optional files)
on an arbitrary window.
"""
