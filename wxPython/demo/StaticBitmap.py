
import  wx
import  images


USE_GENERIC = 0

if USE_GENERIC:
    from wx.lib.stattext import GenStaticText as StaticText
    from wx.lib.statbmp  import GenStaticBitmap as StaticBitmap
else:
    StaticText = wx.StaticText
    StaticBitmap = wx.StaticBitmap


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        ##self.SetBackgroundColour("sky blue")

        StaticText(self, -1, "This is a wx.StaticBitmap.", (45, 15))

        bmp = images.getTest2Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        StaticBitmap(self, -1, bmp, (80, 50), (bmp.GetWidth(), bmp.GetHeight()))

        bmp = images.getRobinBitmap()
        StaticBitmap(self, -1, bmp, (80, 150))

        StaticText(self, -1, "Hey, if Ousterhout can do it, so can I.", (200, 175))


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------

overview = """\
A StaticBitmap control displays a bitmap.

The bitmap to be displayed should have a small number of colours, such as 16, 
to avoid palette problems.

A bitmap can be derived from most image formats using the wx.Image class.

"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
