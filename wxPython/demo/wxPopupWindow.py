from wxPython.wx import *

#---------------------------------------------------------------------------

class TestPopup(wxPopupWindow):
    def __init__(self, parent, style):
        wxPopupWindow.__init__(self, parent, style)

        EVT_LEFT_DOWN(self, self.OnMouseLeftDown)
        EVT_MOTION(self, self.OnMouseMotion)
        EVT_LEFT_UP(self, self.OnMouseLeftUp)

    def OnMouseLeftDown(self, evt):
        self.ldPos = evt.GetPosition()
        self.CaptureMouse()

    def OnMouseMotion(self, evt):
        if evt.Dragging():
            wPos = self.GetPosition()
            dPos = evt.GetPosition()
            self.Move((wPos.x + (dPos.x - self.ldPos.x),
                       wPos.y + (dPos.y - self.ldPos.y)))
            print self.GetPosition()

    def OnMouseLeftUp(self, evt):
        self.ReleaseMouse()
        pass



class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        b = wxButton(self, -1, "Show popup window", (25, 50))
        EVT_BUTTON(self, b.GetId(), self.OnShowPopup)


    def OnShowPopup(self, evt):
        win = TestPopup(self, wxSIMPLE_BORDER)
        #win.SetPosition((200, 200))
        #win.SetSize((150, 150))
        win.Position((200, 200), (150, 150))
        win.SetBackgroundColour("CADET BLUE")
        win.Show(true)



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
"""
