
from wxPython.wx import *
from wxPython.lib.buttons import *

import images
#----------------------------------------------------------------------


class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        sizer = wxFlexGridSizer(1, 3, 20, 20)
        b = wxButton(self, -1, "A real button")
        b.SetDefault()
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        sizer.Add(b)

        b = wxButton(self, -1, "non-default")
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        sizer.Add(b)
        sizer.Add(10,10)

        b = wxGenButton(self, -1, 'Hello')
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        sizer.Add(b)

        b = wxGenButton(self, -1, 'disabled')
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b.Enable(False)
        sizer.Add(b)

        b = wxGenButton(self, -1, 'bigger')
        EVT_BUTTON(self, b.GetId(), self.OnBiggerButton)
        b.SetFont(wxFont(20, wxSWISS, wxNORMAL, wxBOLD, False))
        b.SetBezelWidth(5)
        ###b.SetBestSize()
        b.SetBackgroundColour("Navy")
        b.SetForegroundColour(wxWHITE)
        b.SetToolTipString("This is a BIG button...")
        sizer.Add(b, flag=wxADJUST_MINSIZE)  # let the sizer set best size

        bmp = images.getTest2Bitmap()
        b = wxGenBitmapButton(self, -1, bmp)
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        sizer.Add(b)

        bmp = images.getTest2Bitmap()
        b = wxGenBitmapButton(self, -1, bmp)
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        sizer.Add(b)
        b.Enable(False)

        b = wxGenBitmapButton(self, -1, None)
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
        sizer.Add(b)

        b = wxGenToggleButton(self, -1, "Toggle Button")
        EVT_BUTTON(self, b.GetId(), self.OnToggleButton)
        sizer.Add(b)

        b = wxGenBitmapToggleButton(self, -1, None)
        EVT_BUTTON(self, b.GetId(), self.OnToggleButton)
        bmp = images.getBulb1Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetToggle(True)
        b.SetBestSize()
        sizer.Add(b)

        b = wxGenBitmapTextButton(self, -1, None, "Bitmapped Text", size = (200, 45))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        bmp = images.getBulb1Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetUseFocusIndicator(False)
        b.SetBestSize()
        sizer.Add(b)

        border = wxBoxSizer(wxVERTICAL)
        border.Add(sizer, 0, wxALL, 25)
        self.SetSizer(border)


    def OnButton(self, event):
        self.log.WriteText("Button Clicked: %d\n" % event.GetId())


    def OnBiggerButton(self, event):
        self.log.WriteText("Bigger Button Clicked: %d\n" % event.GetId())
        b = event.GetEventObject()
        txt = "big " + b.GetLabel()
        b.SetLabel(txt)
        self.GetSizer().Layout()


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



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

