
from wxPython.wx import *
from wxPython.lib.buttons import wxGenButton, wxGenBitmapButton, \
                                 wxGenToggleButton, wxGenBitmapToggleButton

import images
#----------------------------------------------------------------------


class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        b = wxButton(self, -1, "A real button", (10,10))
        b.SetDefault()
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b = wxButton(self, -1, "non-default", (140, 10))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        #wxTextCtrl(self, -1, "", (10,40))

        b = wxGenButton(self, -1, 'Hello', (10,65))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b = wxGenButton(self, -1, 'disabled', (140,65))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b.Enable(false)

        b = wxGenButton(self, -1, 'bigger', (250,50))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b.SetFont(wxFont(20, wxSWISS, wxNORMAL, wxBOLD, false))
        b.SetBezelWidth(5)
        b.SetBestSize()
        b.SetBackgroundColour(wxNamedColour("Navy"))
        b.SetForegroundColour(wxWHITE)
        #b.SetUseFocusIndicator(false)
        b.SetToolTipString("This is a BIG button...")

        bmp = images.getTest2Bitmap()
        b = wxGenBitmapButton(self, -1, bmp, (10, 130))
        EVT_BUTTON(self, b.GetId(), self.OnButton)


        b = wxGenBitmapButton(self, -1, None, (140, 130))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        bmp = images.getBulb1Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetBestSize()

        b = wxGenToggleButton(self, -1, "Toggle Button", (10, 230))
        EVT_BUTTON(self, b.GetId(), self.OnToggleButton)


        b = wxGenBitmapToggleButton(self, -1, None, (140, 230))
        EVT_BUTTON(self, b.GetId(), self.OnToggleButton)
        bmp = images.getBulb1Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetToggle(true)
        b.SetBestSize()


    def OnButton(self, event):
        self.log.WriteText("Button Clicked: %d\n" % event.GetId())

    def OnToggleButton(self, event):
        msg = (event.GetIsDown() and "on") or "off"
        self.log.WriteText("Button %d Toggled: %s\n" % (event.GetId(), msg))



#----------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------


import wxPython.lib.buttons
overview = wxPython.lib.buttons.__doc__
