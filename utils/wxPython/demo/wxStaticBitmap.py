
from wxPython.wx import *

import string

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wxStaticText(self, -1, "This is a wxStaticBitmap.", wxPoint(45, 15))

        bmp = wxBitmap('bitmaps/test2.bmp', wxBITMAP_TYPE_BMP)
        wxStaticBitmap(self, -1, bmp, wxPoint(80, 50),
                       wxSize(bmp.GetWidth(), bmp.GetHeight()))




#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------










overview = """\
"""
