
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        
        self.redBmp   = self.MakeBitmap(188, 143, 234)
        self.greenBmp = self.MakeBitmap(35,  142,  35)
        self.blueBmp  = self.MakeBitmap(50,  153, 204)

    def MakeBitmap(self, red, green, blue, alpha=128):
        bmp = wx.EmptyBitmap(100, 100, 32)

        # Create an object that facilitates access to the bitmap's
        # pixel buffer
        pixelData = wx.AlphaPixelData(bmp)
        if not pixelData:
            raise RuntimeError("Failed to gain raw access to bitmap data.")

        # We have two ways to access each pixel, first we'll use an
        # iterator to set every pixel to the colour and alpha values
        # passed in.
        for pixel in pixelData:
            pixel.Set(red, green, blue, alpha)

        # Next we'll use the pixel accessor to draw a border
        pixels = pixelData.GetPixels()
        for x in xrange(100):
            pixels.MoveTo(pixelData, x, 0)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
            pixels.MoveTo(pixelData, x, 99)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
        for y in xrange(100):
            pixels.MoveTo(pixelData, 0, y)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
            pixels.MoveTo(pixelData, 99, y)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
            
        return bmp


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc.DrawBitmap(self.redBmp,    50,  50, True)
        dc.DrawBitmap(self.greenBmp, 110, 110, True)
        dc.DrawBitmap(self.blueBmp,  170,  50, True)
        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Raw Bitmap Access</center></h2>

wx.NativePixelData and wx.AlphaPixelData provide a cross-platform way
to access the platform-specific pixel buffer within a wx.Bitmap.  They
provide both a random access method, and an iterator interface.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

