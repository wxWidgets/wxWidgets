
from wxPython.wx import *
from wxPython.lib.buttons import wxGenButton, wxGenBitmapButton, \
                                 wxGenToggleButton, wxGenBitmapToggleButton

#----------------------------------------------------------------------


class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        b = wxButton(self, -1, "A real button", (10,10))
        b.SetDefault()
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b = wxButton(self, -1, "non-default", (100, 10))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        #wxTextCtrl(self, -1, "", (10,40))

        b = wxGenButton(self, -1, 'Hello', (10,65))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b = wxGenButton(self, -1, 'disabled', (100,65))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b.Enable(false)

        b = wxGenButton(self, -1, 'bigger', (195,50))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        b.SetFont(wxFont(20, wxSWISS, wxNORMAL, wxBOLD, false))
        b.SetBezelWidth(5)
        b.SetBestSize()
        b.SetBackgroundColour(wxNamedColour("Navy"))
        b.SetForegroundColour(wxWHITE)
        #b.SetUseFocusIndicator(false)

        bmp = wxBitmap('bitmaps/test2.bmp', wxBITMAP_TYPE_BMP)
        b = wxGenBitmapButton(self, -1, bmp, (10, 130))
        EVT_BUTTON(self, b.GetId(), self.OnButton)


        b = wxGenBitmapButton(self, -1, None, (100, 130))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        bmp = wxBitmap('bitmaps/lb1.bmp', wxBITMAP_TYPE_BMP)
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = wxBitmap('bitmaps/lb2.bmp', wxBITMAP_TYPE_BMP)
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetBestSize()

        b = wxGenToggleButton(self, -1, "Toggle Button", (10, 230))
        EVT_BUTTON(self, b.GetId(), self.OnToggleButton)


        b = wxGenBitmapToggleButton(self, -1, None, (100, 230))
        EVT_BUTTON(self, b.GetId(), self.OnToggleButton)
        bmp = wxBitmap('bitmaps/lb1.bmp', wxBITMAP_TYPE_BMP)
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = wxBitmap('bitmaps/lb2.bmp', wxBITMAP_TYPE_BMP)
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
