
import  wx
import  images

#----------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        self.log = log
        wx.Frame.__init__(self, parent, -1, "Shaped Window",
                         style =
                           wx.FRAME_SHAPED
                         | wx.SIMPLE_BORDER
                         | wx.FRAME_NO_TASKBAR
                         | wx.STAY_ON_TOP
                         )

        self.hasShape = False
        self.delta = (0,0)

        self.Bind(wx.EVT_LEFT_DCLICK,   self.OnDoubleClick)
        self.Bind(wx.EVT_LEFT_DOWN,     self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP,       self.OnLeftUp)
        self.Bind(wx.EVT_MOTION,        self.OnMouseMove)
        self.Bind(wx.EVT_RIGHT_UP,      self.OnExit)
        self.Bind(wx.EVT_PAINT,         self.OnPaint)

        self.bmp = images.getTuxBitmap()
        w, h = self.bmp.GetWidth(), self.bmp.GetHeight()
        self.SetClientSize( (w, h) )

        if wx.Platform != "__WXMAC__":
            # wxMac clips the tooltip to the window shape, YUCK!!!
            self.SetToolTipString("Right-click to close the window\n"
                                  "Double-click the image to set/unset the window shape")

        if wx.Platform == "__WXGTK__":
            # wxGTK requires that the window be created before you can
            # set its shape, so delay the call to SetWindowShape until
            # this event.
            self.Bind(wx.EVT_WINDOW_CREATE, self.SetWindowShape)
        else:
            # On wxMSW and wxMac the window has already been created, so go for it.
            self.SetWindowShape()

        dc = wx.ClientDC(self)
        dc.DrawBitmap(self.bmp, 0,0, True)


    def SetWindowShape(self, *evt):
        # Use the bitmap's mask to determine the region
        r = wx.RegionFromBitmap(self.bmp)
        self.hasShape = self.SetShape(r)


    def OnDoubleClick(self, evt):
        if self.hasShape:
            self.SetShape(wx.Region())
            self.hasShape = False
        else:
            self.SetWindowShape()


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc.DrawBitmap(self.bmp, 0,0, True)

    def OnExit(self, evt):
        self.Close()


    def OnLeftDown(self, evt):
        self.CaptureMouse()
        x, y = self.ClientToScreen(evt.GetPosition())
        originx, originy = self.GetPosition()
        dx = x - originx
        dy = y - originy
        self.delta = ((dx, dy))


    def OnLeftUp(self, evt):
        if self.HasCapture():
            self.ReleaseMouse()


    def OnMouseMove(self, evt):
        if evt.Dragging() and evt.LeftIsDown():
            x, y = self.ClientToScreen(evt.GetPosition())
            fp = (x - self.delta[0], y - self.delta[1])
            self.Move(fp)


#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the ShapedWindow sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = TestFrame(self, self.log)
        win.Show(True)

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

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
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

