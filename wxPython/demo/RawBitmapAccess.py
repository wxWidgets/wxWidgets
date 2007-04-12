import wx

# use the numpy code instead of the raw access code for comparison
USE_NUMPY = False

# time the execution of making a bitmap?
TIMEIT = False

# how big to make the bitmaps
DIM = 100

#----------------------------------------------------------------------
# attempt to import a numeric module if requested to

if USE_NUMPY:
    try:
        import numpy
        def makeByteArray(shape):
            return numpy.empty(shape, numpy.uint8)
        numtype = 'numpy'
    except ImportError:
        try:
            import numarray
            def makeByteArray(shape):
                arr =  numarray.array(shape=shape, typecode='u1')
                arr[:] = 0
                return arr
            numtype = 'numarray'
        except ImportError:
            USE_NUMPY = False


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

        if TIMEIT:
            import timeit
            timeit.s = self # Put self in timeit's global namespace as
                            # 's' so it can be found in the code
                            # snippets being tested.
            if not USE_NUMPY:
                t = timeit.Timer("bmp = s.MakeBitmap(10, 20, 30)")
            else:
                t = timeit.Timer("bmp = s.MakeBitmap2(10, 20, 30)")
            log.write("Timing...\n")
            num = 100
            tm = t.timeit(num)
            log.write("%d passes in %f seconds ==  %f seconds per pass " % 
                  (num, tm, tm/num))
            
        if not USE_NUMPY:
            log.write("using raw access\n")
            self.redBmp   = self.MakeBitmap(178,  34,  34)
            self.greenBmp = self.MakeBitmap( 35, 142,  35)
            self.blueBmp  = self.MakeBitmap(  0,   0, 139)
        else:
            log.write("using %s\n" % numtype)
            self.redBmp   = self.MakeBitmap2(178,  34,  34)
            self.greenBmp = self.MakeBitmap2( 35, 142,  35)
            self.blueBmp  = self.MakeBitmap2(  0,   0, 139)


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc.DrawBitmap(self.redBmp,    50,  50, True)
        dc.DrawBitmap(self.greenBmp, 110, 110, True)
        dc.DrawBitmap(self.blueBmp,  170,  50, True)


    def MakeBitmap(self, red, green, blue, alpha=128):
        # Create the bitmap that we will stuff pixel values into using
        # the raw bitmap access classes.
        bmp = wx.EmptyBitmap(DIM, DIM, 32)

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

        # This block of code is another way to do the same as above,
        # but with the accessor interface instead of the Python
        # iterator.  It is a bit faster than the above because it
        # avoids the iterator/generator magic, but it is not nearly as
        # 'clean' looking ;-)
        #pixels = pixelData.GetPixels()
        #for y in xrange(DIM):
        #    for x in xrange(DIM):
        #        pixels.Set(red, green, blue, alpha)
        #        pixels.nextPixel()
        #    pixels.MoveTo(pixelData, 0, y)


        # Next we'll use the pixel accessor to set the border pixels
        # to be fully opaque
        pixels = pixelData.GetPixels()
        for x in xrange(DIM):
            pixels.MoveTo(pixelData, x, 0)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
            pixels.MoveTo(pixelData, x, DIM-1)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
        for y in xrange(DIM):
            pixels.MoveTo(pixelData, 0, y)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
            pixels.MoveTo(pixelData, DIM-1, y)
            pixels.Set(red, green, blue, wx.ALPHA_OPAQUE)
            
        return bmp


    def MakeBitmap2(self, red, green, blue, alpha=128):
        # Make an array of bytes that is DIM*DIM in size, with enough
        # slots for each pixel to have a RGB and A value
        arr = makeByteArray( (DIM,DIM, 4) )

        # just some indexes to keep track of which byte is which
        R, G, B, A = range(4)

        # initialize all pixel values to the values passed in
        arr[:,:,R] = red
        arr[:,:,G] = green
        arr[:,:,B] = blue
        arr[:,:,A] = alpha

        # Set the alpha for the border pixels to be fully opaque
        arr[0,     0:DIM, A] = wx.ALPHA_OPAQUE  # first row
        arr[DIM-1, 0:DIM, A] = wx.ALPHA_OPAQUE  # last row
        arr[0:DIM, 0,     A] = wx.ALPHA_OPAQUE  # first col
        arr[0:DIM, DIM-1, A] = wx.ALPHA_OPAQUE  # last col

        # finally, use the array to create a bitmap
        bmp = wx.BitmapFromBufferRGBA(DIM, DIM, arr)
        return bmp
    
           
        
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

<p>Unfortunately, although these classes are convienient ways to access
and update the contents of a wx.Bitmap, we lose most of the efficiency
of the C++ classes by requiring one or more Python-to-C++ transitions
for each pixel.  In fact it can be <b>much</b> slower than the other
ways of creating a bitmap from scratch, especially now that
wx.BitmapFromBuffer exists and can save the time needed to copy from a
wx.Image.

<p>To see this difference for yourself this module has been
instrumented to allow you to experiment with using either the raw
access or numpy/numarray, and also to time how long it takes to create
100 bitmaps like you see on the screen.  Simply edit this module in
the \"Demo Code\" tab and set TIMEIT to True and then watch
the log window when the sample is reloaded.  To try numpy or numarray
(if you have them installed) then set USE_NUMPY to True as well, and
watch the log window again. On my machines there is about <b>an
order of magnitude</b> difference between the raw access functions
and using a numarray.array with wx.BitmapFromBufferRGBA!  Almost
another order of magnitude improvement can be gained with using the
new numpy module!

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

