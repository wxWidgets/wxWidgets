
import wx
import array

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.width, self.height = 120,120
        
        self.MakeBitmapRGB(self.width, self.height)
        self.MakeBitmapRGBA(self.width, self.height)
        self.MakeBitmapRGBpA(self.width, self.height)

    def GetRGB(self, x, y, bpp):
        # calculate some colour values for this sample based on x,y position
        r = g = b = 0
        if y < self.height/3:                           r = 255
        if y >= self.height/3 and y <= 2*self.height/3: g = 255
        if y > 2*self.height/3:                         b = 255

        if bpp == 4:
            a = int(x * 255.0 / self.width)
            return r, g, b, a
        else:
            return r, g, b


    def MakeBitmapRGB(self, width, height):
        # Make a bitmap using an array of RGB bytes
        bpp = 3  # bytes per pixel
        bytes = array.array('B', [0] * width*height*bpp)

        def offset(x, y):
            # return the offset into the bytes array for the start of
            # the x,y pixel
            return y*width*bpp + x*bpp

        for y in xrange(height):
            for x in xrange(width):
                r,g,b = self.GetRGB(x, y, bpp)
                bytes[offset(x, y) + 0] = r
                bytes[offset(x, y) + 1] = g
                bytes[offset(x, y) + 2] = b
                ##print (x, y), offset(x, y), (r, g, b)

        self.rgbBmp = wx.BitmapFromBuffer(width, height, bytes)
        


    def MakeBitmapRGBA(self, width, height):
        # Make a bitmap using an array of RGBA bytes
        bpp = 4  # bytes per pixel
        bytes = array.array('B', [0] * width*height*bpp)

        def offset(x, y):
            # return the offset into the bytes array for the start of
            # the x,y pixel
            return y*width*bpp + x*bpp

        for y in xrange(height):
            for x in xrange(width):
                r,g,b,a = self.GetRGB(x, y, bpp)
                bytes[offset(x, y) + 0] = r
                bytes[offset(x, y) + 1] = g
                bytes[offset(x, y) + 2] = b
                bytes[offset(x, y) + 3] = a
                ##print (x, y), offset(x, y), (r, g, b, a)

        self.rgbaBmp = wx.BitmapFromBufferRGBA(width, height, bytes)


    def MakeBitmapRGBpA(self, width, height):
        # Make a bitmap using an array of RGB bytes plus a separate
        # buffer for the alpha channel
        bpp = 3  # bytes per pixel
        bytes = array.array('B', [0] * width*height*bpp)

        def offset(x, y):
            # return the offset into the bytes array for the start of
            # the x,y pixel
            return y*width*bpp + x*bpp

        for y in xrange(height):
            for x in xrange(width):
                r,g,b = self.GetRGB(x, y, bpp)
                bytes[offset(x, y) + 0] = r
                bytes[offset(x, y) + 1] = g
                bytes[offset(x, y) + 2] = b
                ##print (x, y), offset(x, y), (r, g, b)

        # just use an alpha buffer with a constant alpha value for all
        # pixels for this example, it could just as easily have
        # varying alpha values like the other sample.
        alpha = array.array('B', [128]*width*height)
        self.rgbaBmp2 = wx.BitmapFromBuffer(width, height, bytes, alpha)


    def DrawBitmapAndMessage(self, dc, bmp, msg, x_, y_):
        x, y = x_, y_

        # draw some text to help show the alpha
        dc.SetFont(self.GetFont())
        while y < y_ + self.height + 2*dc.GetCharHeight():
            dc.DrawText(msg, x,y)
            y += dc.GetCharHeight() + 5

        # draw the bitmap over the text
        dc.DrawBitmap(bmp, x+15,y_+15, True)

        
    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        self.DrawBitmapAndMessage(dc, self.rgbBmp,  "No alpha channel in this image", 30,35)
        self.DrawBitmapAndMessage(dc, self.rgbaBmp, "This image has some alpha", 325,35)
        self.DrawBitmapAndMessage(dc, self.rgbaBmp2,"This one made with RGB+A", 180,220)


        
        
        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>BitmapFromBuffer</center></h2>

Two new wx.Bitmap factory functions allow you to create a wx.Bitmap
directly from a data buffer.  The the buffer can be any Python object
that implements the buffer interface, or is convertable to a buffer,
such as a string or an array.  The new functions are: <ul>

<li><b>wx.BitmapFromBuffer</b>(width, height, dataBuffer, alphaBuffer=None):
Creates the bitmap from a buffer of RGB bytes, optionally with a separate
buffer of alpha bytes.

<li><b>wx.BitmapFromBufferRGBA</b>(width, height, dataBuffer): Creates
the bitmap from a buffer containing RGBA bytes.

</ul>



</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

