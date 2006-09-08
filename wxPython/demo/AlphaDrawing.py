
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        
        wx.StaticText(self, -1,
                      "On the Mac these squares should be transparent,\n"
                      "if the CoreGrahics option is turned on.",
                      (20, 20))
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        rect = wx.Rect(0,0, 100, 100)
        for RGB, pos in [((178,  34,  34), ( 50,  70)),
                         (( 35, 142,  35), (110, 130)),
                         ((  0,   0, 139), (170,  70))
                         ]:
            r, g, b = RGB
            penclr   = wx.Colour(r, g, b, wx.ALPHA_OPAQUE)
            brushclr = wx.Colour(r, g, b, 128)   # half transparent
            dc.SetPen(wx.Pen(penclr))
            dc.SetBrush(wx.Brush(brushclr))
            rect.SetPosition(pos)
            dc.DrawRectangleRect(rect)
            
        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Alpha Drawing</center></h2>

The wx.DC on Mac now supports alpha transparent drawing using pens and
brushes.  This is accomplished by enabling the wx.Colour class to have
a fourth component for the alpha value, where 0 is fully transparent,
and 255 is fully opaque.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

