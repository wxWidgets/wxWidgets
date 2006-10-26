
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

        txt = """\
If this build of wxPython includes the new wx.GCDC class (which
provides the wx.DC API on top of the new wx.GraphicsContext class)
then these squares should be transparent.
"""
        wx.StaticText(self, -1, txt, (20, 20))
        

    def OnPaint(self, evt):
        pdc = wx.PaintDC(self)
        try:
            dc = wx.GCDC(pdc)
        except:
            dc = pdc
        rect = wx.Rect(0,0, 100, 100)
        for RGB, pos in [((178,  34,  34), ( 50,  90)),
                         (( 35, 142,  35), (110, 150)),
                         ((  0,   0, 139), (170,  90))
                         ]:
            r, g, b = RGB
            penclr   = wx.Colour(r, g, b, wx.ALPHA_OPAQUE)
            brushclr = wx.Colour(r, g, b, 128)   # half transparent
            dc.SetPen(wx.Pen(penclr))
            dc.SetBrush(wx.Brush(brushclr))
            rect.SetPosition(pos)
            dc.DrawRoundedRectangleRect(rect, 8)
            
        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Alpha Drawing</center></h2>

The wx.GCDC class is a class that implemented the wx.DC API using the
new wx.GraphicsContext class, and so it supports anti-aliased drawing
using pens and brushes, that can optionally also be drawn using an
alpha transparency.  (On the Mac all the DC classes are using this new
implementation.)  This is accomplished by enabling the wx.Colour class
to have a fourth component for the alpha value, where 0 is fully
transparent, and 255 is fully opaque.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

