
from wxPython.wx import *

import string
import images

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wxStaticText(self, -1, "This is a wxStaticBitmap.", wxPoint(45, 15))

        bmp = images.getTest2Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        wxStaticBitmap(self, -1, bmp, wxPoint(80, 50),
                       wxSize(bmp.GetWidth(), bmp.GetHeight()))

        bmp = images.getRobinBitmap()
        wxStaticBitmap(self, -1, bmp, (80, 150))

        wxStaticText(self, -1, "Hey, if Ousterhout can do it, so can I.",
                     (200, 175))


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------










overview = """\
"""
