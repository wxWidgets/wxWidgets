
import  wx

#----------------------------------------------------------------------

logicList = [
    ('wx.AND', wx.AND),
    ('wx.AND_INVERT', wx.AND_INVERT),
    ('wx.AND_REVERSE', wx.AND_REVERSE),
    ('wx.CLEAR', wx.CLEAR),
    ('wx.COPY', wx.COPY),
    ('wx.EQUIV', wx.EQUIV),
    ('wx.INVERT', wx.INVERT),
    ('wx.NAND', wx.NAND),

    # this one causes an assert on wxGTK, and doesn't seem to
    # do much on MSW anyway, so I'll just take it out....
    #('wxNOR', wxNOR),

    ('wx.NO_OP', wx.NO_OP),
    ('wx.OR', wx.OR),
    ('wx.OR_INVERT', wx.OR_INVERT),
    ('wx.OR_REVERSE', wx.OR_REVERSE),
    ('wx.SET', wx.SET),
    ('wx.SRC_INVERT', wx.SRC_INVERT),
    ('wx.XOR', wx.XOR),
]

if 'mac-cg' in wx.PlatformInfo:
    # that's all, folks!
    logicList = [
        ('wx.COPY', wx.COPY),
    ]    

import images

class TestMaskWindow(wx.ScrolledWindow):
    def __init__(self, parent):
        wx.ScrolledWindow.__init__(self, parent, -1)
        self.SetBackgroundColour(wx.Colour(0,128,0))

        # A reference bitmap that we won't mask
        self.bmp_nomask  = images.getTestStar2Bitmap()

        # One that we will
        self.bmp_withmask  = images.getTestStar2Bitmap()

        # this mask comes from a monochrome bitmap
        self.bmp_themask = images.getTestMaskBitmap()
        mask = wx.Mask(self.bmp_themask)

        # set the mask on our bitmap
        self.bmp_withmask.SetMask(mask)

        # Now we'll create a mask in a bit of an easier way, by picking a
        # colour in the image that is to be the transparent colour.
        self.bmp_withcolourmask  = images.getTestStar2Bitmap()
        mask = wx.Mask(self.bmp_withcolourmask, wx.WHITE)
        self.bmp_withcolourmask.SetMask(mask)

        self.SetScrollbars(20, 20, 700/20, 460/20)

        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def OnPaint (self, e):
        self.SetBackgroundColour(wx.Colour(0,128,0))
        dc = wx.PaintDC(self)
        self.PrepareDC(dc)
        dc.SetTextForeground(wx.WHITE)

        # make an interesting background...
        dc.SetPen(wx.MEDIUM_GREY_PEN)
        for i in range(100):
            dc.DrawLine(0,i*10, i*10,0)

        # draw raw image, mask, and masked images
        dc.DrawText('original image', 0,0)
        dc.DrawBitmap(self.bmp_nomask, 0,20, 0)
        dc.DrawText('with colour mask', 0,100)
        dc.DrawBitmap(self.bmp_withcolourmask, 0,120, 1)
        dc.DrawText('the mask image', 0,200)
        dc.DrawBitmap(self.bmp_themask, 0,220, 0)
        dc.DrawText('masked image', 0,300)
        dc.DrawBitmap(self.bmp_withmask, 0,320, 1)

        cx,cy = self.bmp_themask.GetWidth(), self.bmp_themask.GetHeight()

        # draw array of assorted blit operations
        mdc = wx.MemoryDC()
        i = 0

        for text, code in logicList:
            x,y = 120+150*(i%4), 20+100*(i/4)
            dc.DrawText(text, x, y-20)
            mdc.SelectObject(self.bmp_withcolourmask)
            dc.Blit(x,y, cx,cy, mdc, 0,0, code, True)
            i = i + 1


# On wxGTK there needs to be a panel under wx.ScrolledWindows if they are
# going to be in a wxNotebook...
class TestPanel(wx.Panel):
    def __init__(self, parent, ID):
        wx.Panel.__init__(self, parent, ID)
        self.win = TestMaskWindow(self)
        self.Bind(wx.EVT_SIZE, self.OnSize)

    def OnSize(self, evt):
        self.win.SetSize(evt.GetSize())


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, -1)
    return win

#----------------------------------------------------------------------


overview = """\
This class encapsulates a monochrome mask bitmap, where the masked area is black 
and the unmasked area is white. When associated with a bitmap and drawn in a device 
context, the unmasked area of the bitmap will be drawn, and the masked area will 
not be drawn.

This example shows not only how to create a Mask, but the effects of the Device
Context (dc) <code>Blit()</code> method's logic codes.
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

