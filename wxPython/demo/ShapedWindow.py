
from wxPython.wx import *
import images

#----------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        self.log = log
        wxFrame.__init__(self, parent, -1, "Shaped Window",
                         style = wxFRAME_SHAPED )
        self.hasShape = False
        self.delta = wxPoint(0,0)

        EVT_LEFT_DCLICK(self, self.OnDoubleClick)
        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_MOTION(self, self.OnMouseMove)
        EVT_RIGHT_UP(self, self.OnExit)
        EVT_PAINT(self, self.OnPaint)

        self.bmp = images.getTuxBitmap()
        w, h = self.bmp.GetWidth(), self.bmp.GetHeight()
        self.SetClientSize( (w, h) )

        if wxPlatform != "__WXMAC__":
            # wxMac clips the tooltip to the window shape, YUCK!!!
            self.SetToolTipString("Right-click to close the window\n"
                                  "Double-click the image to set/unset the window shape")

        if wxPlatform == "__WXGTK__":
            # wxGTK requires that the window be created before you can
            # set its shape, so delay the call to SetWindowShape until
            # this event.
            EVT_WINDOW_CREATE(self, self.SetWindowShape)
        else:
            # On wxMSW and wxMac the window has already been created, so go for it.
            self.SetWindowShape()

        dc = wxClientDC(self)
        dc.DrawBitmap(self.bmp, 0,0, True)


    def SetWindowShape(self, *evt):
        # Use the bitmap's mask to determine the region
        r = wxRegionFromBitmap(self.bmp)
        self.hasShape = self.SetShape(r)


    def OnDoubleClick(self, evt):
        if self.hasShape:
            self.SetShape(wxRegion())
            self.hasShape = False
        else:
            self.SetWindowShape()


    def OnPaint(self, evt):
        dc = wxPaintDC(self)
        dc.DrawBitmap(self.bmp, 0,0, True)

    def OnExit(self, evt):
        self.Close()


    def OnLeftDown(self, evt):
        self.CaptureMouse()
        pos = self.ClientToScreen(evt.GetPosition())
        origin = self.GetPosition()
        dx = pos.x - origin.x
        dy = pos.y - origin.y
        self.delta = wxPoint(dx, dy)


    def OnLeftUp(self, evt):
        if self.HasCapture():
            self.ReleaseMouse()


    def OnMouseMove(self, evt):
        if evt.Dragging() and evt.LeftIsDown():
            pos = self.ClientToScreen(evt.GetPosition())
            fp = (pos.x - self.delta.x, pos.y - self.delta.y)
            self.Move(fp)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestFrame(nb, log)
    frame.otherWin = win
    win.Show(True)


#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Shaped Window</center></h2>

Top level windows now have a SetShape method that lets you set a
non-rectangular shape for the window using a wxRegion.  All pixels
outside of the region will not be drawn and the window will not be
sensitive to the mouse in those areas either.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

