# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx
import  images

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        self.count = 0

        wx.StaticText(self, -1, "This is a wxStaticBitmap.", (45, 15))

        bmp = images.getTest2Bitmap()
        mask = wx.MaskColour(bmp, wx.BLUE)
        bmp.SetMask(mask)
        wx.StaticBitmap(self, -1, bmp, (80, 50), (bmp.GetWidth(), bmp.GetHeight()))

        bmp = images.getRobinBitmap()
        wx.StaticBitmap(self, -1, bmp, (80, 150))

        wx.StaticText(self, -1, "Hey, if Ousterhout can do it, so can I.", (200, 175))


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------

overview = """\
A static bitmap control displays a bitmap.

The bitmap to be displayed should have a small number of colours, such as 16, 
to avoid palette problems.

A bitmap can be derived from most image formats using the wxImage class.

"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
