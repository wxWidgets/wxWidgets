
from wxPython.wx import *
from wxPython.lib.rcsizer import RowColSizer


#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        sizer = RowColSizer()
        text = "This sizer lays out it's items by row and column "\
               "that are specified explicitly when the item is \n"\
               "added to the sizer.  Grid cells with nothing in "\
               "them are supported and column- or row-spanning is \n"\
               "handled as well.  Growable rows and columns are "\
               "specified just like the wxFlexGridSizer."

        sizer.Add(wxStaticText(self, -1, text), row=1, col=1, colspan=5)

        sizer.Add(wxTextCtrl(self, -1, "(3,1)"), flag=wxEXPAND, row=3, col=1)
        sizer.Add(wxTextCtrl(self, -1, "(3,2)"), row=3, col=2)
        sizer.Add(wxTextCtrl(self, -1, "(3,3)"), row=3, col=3)
        sizer.Add(wxTextCtrl(self, -1, "(3,4)"), row=3, col=4)
        sizer.Add(wxTextCtrl(self, -1, "(4,2) span:(2,2)"), flag=wxEXPAND,
                  row=4, col=2, rowspan=2, colspan=2)
        sizer.Add(wxTextCtrl(self, -1, "(6,4)"), row=6, col=4)
        sizer.Add(wxTextCtrl(self, -1, "(7,2)"), row=7, col=2)
        sizer.Add(wxTextCtrl(self, -1, "(8,3)"), row=8, col=3)
        sizer.Add(wxTextCtrl(self, -1, "(10,1) colspan: 4"), flag=wxEXPAND, pos=(10,1), colspan=4)
        sizer.Add(wxTextCtrl(self, -1, "(3,5) rowspan: 8, growable col", style=wxTE_MULTILINE),
                  flag=wxEXPAND, pos=(3,5), size=(8,1))

        box = wxBoxSizer(wxVERTICAL)
        box.Add(wxButton(self, -1, "A vertical box"), flag=wxEXPAND)
        box.Add(wxButton(self, -1, "sizer put in the"), flag=wxEXPAND)
        box.Add(wxButton(self, -1, "RowColSizer at (12,1)"), flag=wxEXPAND)
        sizer.Add(box, pos=(12,1))

        sizer.Add(wxTextCtrl(self, -1, "(12,2) align bottom"), flag=wxALIGN_BOTTOM, pos=(12,2))
        sizer.Add(wxTextCtrl(self, -1, "(12,3) align center"), flag=wxALIGN_CENTER_VERTICAL, pos=(12,3))
        sizer.Add(wxTextCtrl(self, -1, "(12,4)"),pos=(12,4))
        sizer.Add(wxTextCtrl(self, -1, "(12,5) full border"), flag=wxEXPAND|wxALL, border=15, pos=(12,5))

        sizer.AddGrowableCol(5)
        sizer.AddGrowableRow(9)

        sizer.AddSpacer(10,10, pos=(1,6))
        sizer.AddSpacer(10,10, pos=(13,1))

        self.SetSizer(sizer)
        self.SetAutoLayout(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win


#----------------------------------------------------------------------


import wxPython.lib.rcsizer
overview = wxPython.lib.rcsizer.__doc__



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

