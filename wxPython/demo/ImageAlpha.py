
import wx                  # This module uses the new wx namespace
from Main import opj


#----------------------------------------------------------------------

msg = "This is some text that will appear behind the image..."

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush("WHITE"))
        dc.Clear()

        dc.SetFont(wx.Font(16, wx.SWISS, wx.NORMAL, wx.BOLD, True))
        dc.DrawText("Bitmap alpha blending (on wxMSW and wxMac only)",
                    25,25)
        
        bmp = wx.Bitmap(opj('bitmaps/toucan.png'))
        if "__WXGTK__" in wx.PlatformInfo:
            # try to make up for it a bit...
            bmp.SetMaskColour("black")
            
        dc.DrawBitmap(bmp, 25,100, True)

        dc.SetFont(self.GetFont())
        y = 75
        for line in range(10):
            y += dc.GetCharHeight() + 5
            dc.DrawText(msg, 200, y)
        dc.DrawBitmap(bmp, 250,100, True)
        


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Images with Alpha</center></h2>

wxMSW and wxMac now support alpha channels of supported image
types, and will properly blend that channel when drawing a
bitmap.  It is not supported yet on wxGTK, (if you would like to
change that please submit a patch!)

<p>On wxGTK this demo turns the alpha channel into a 1-bit mask, so
yes, it looks like crap.  Please help us fix it!

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

