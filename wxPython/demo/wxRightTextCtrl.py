
from wxPython.wx import *
from wxPython.lib.rightalign import wxRightTextCtrl
import wxPython.lib.rightalign


#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        fgs = wxFlexGridSizer(cols=2, vgap=5, hgap=5)
        txt = wxStaticText(self, -1,
                           "These text controls will align their contents to\n"
                           "the right (on wxMSW) when they don't have focus.", style=wxALIGN_RIGHT )
        fgs.Add(txt)
        fgs.Add(wxRightTextCtrl(self, -1, "", size=(75, -1)))

        fgs.Add(10,10)
        fgs.Add(wxRightTextCtrl(self, -1, "123.45", size=(75, -1)))

        fgs.Add(10,10)
        fgs.Add(wxRightTextCtrl(self, -1, "234.56", size=(75, -1)))

        fgs.Add(10,10)
        fgs.Add(wxRightTextCtrl(self, -1, "345.67", size=(75, -1)))

        fgs.Add(10,10)
        fgs.Add(wxRightTextCtrl(self, -1, "456.78", size=(75, -1)))

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(fgs, 0, wxALL, 25)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win

#----------------------------------------------------------------------

overview = wxPython.lib.rightalign.__doc__




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
