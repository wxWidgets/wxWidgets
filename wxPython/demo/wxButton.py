
from wxPython.wx import *

import images

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1,
                         style=wxNO_FULL_REPAINT_ON_RESIZE)
        self.log = log

        b = wxButton(self, 10, "Hello", wxPoint(20, 20))
        EVT_BUTTON(self, 10, self.OnClick)
        b.SetBackgroundColour(wxBLUE)
        b.SetForegroundColour(wxWHITE)
        b.SetDefault()

        b = wxButton(self, 20, "HELLO AGAIN!", wxPoint(20, 60), wxSize(120, 45))
        EVT_BUTTON(self, 20, self.OnClick)
        b.SetToolTipString("This is a Hello button...")

        if 0:  # a test case for catching wxPyAssertionError

            #wxGetApp().SetAssertMode(wxPYAPP_ASSERT_SUPPRESS)
            #wxGetApp().SetAssertMode(wxPYAPP_ASSERT_EXCEPTION)
            #wxGetApp().SetAssertMode(wxPYAPP_ASSERT_DIALOG)
            #wxGetApp().SetAssertMode(wxPYAPP_ASSERT_EXCEPTION | wxPYAPP_ASSERT_DIALOG)

            try:
                bmp = wxBitmap("nosuchfile.bmp", wxBITMAP_TYPE_BMP)
                mask = wxMaskColour(bmp, wxBLUE)
            except wxPyAssertionError:
                self.log.write("Caught wxPyAssertionError!  I will fix the problem.\n")
                bmp = images.getTest2Bitmap()
                mask = wxMaskColour(bmp, wxBLUE)
        else:
            bmp = images.getTest2Bitmap()
            mask = wxMaskColour(bmp, wxBLUE)

        bmp.SetMask(mask)
        wxBitmapButton(self, 30, bmp, wxPoint(160, 20),
                       wxSize(bmp.GetWidth()+10, bmp.GetHeight()+10))
        EVT_BUTTON(self, 30, self.OnClick)


    def OnClick(self, event):
        self.log.write("Click! (%d)\n" % event.GetId())
        ##wxLogDebug("debug message")


## wxLog_SetLogLevel(wxLOG_Message)  # ignore everything above wxLOG_Message

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """<html><body>
<h2>wxButton</h2>

A button is a control that contains a text string or a bitmap and cab be
placed on nearly any kind of window.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

