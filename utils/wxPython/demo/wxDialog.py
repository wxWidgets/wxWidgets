
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = wxDialog(frame, -1, "This is a wxDialog", wxDefaultPosition, wxSize(350, 200))

    wxStaticText(win, -1, "This is a wxDialog", wxPoint(20, 20))
    wxButton(win, wxID_OK,     " OK ", wxPoint(75, 120), wxDefaultSize).SetDefault()
    wxButton(win, wxID_CANCEL, " Cancel ", wxPoint(150, 120), wxDefaultSize)

    val = win.ShowModal()
    if val == wxID_OK:
        log.WriteText("You pressed OK\n")
    else:
        log.WriteText("You pressed Cancel\n")



#---------------------------------------------------------------------------










overview = """\
"""
