
from wxPython.wx import *
from wxPython.lib.filebrowsebutton import FileBrowseButton


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = wxPanel(nb, -1)
    fbb = FileBrowseButton(win, -1, wxPoint(20,20), wxSize(350, -1))
    return win



#----------------------------------------------------------------------



overview = FileBrowseButton.__doc__
