
from wxPython.wx import *

#----------------------------------------------------------------------

logicList = [
    ('wxAND', wxAND),
    ('wxAND_INVERT', wxAND_INVERT),
    ('wxAND_REVERSE', wxAND_REVERSE),
    ('wxCLEAR', wxCLEAR),
    ('wxCOPY', wxCOPY),
    ('wxEQUIV', wxEQUIV),
    ('wxINVERT', wxINVERT),
    ('wxNAND', wxNAND),

    # this one causes an assert on wxGTK, and doesn't seem to
    # do much on MSW anyway, so I'll just take it out....
    #('wxNOR', wxNOR),

    ('wxNO_OP', wxNO_OP),
    ('wxOR', wxOR),
    ('wxOR_INVERT', wxOR_INVERT),
    ('wxOR_REVERSE', wxOR_REVERSE),
    ('wxSET', wxSET),
    ('wxSRC_INVERT', wxSRC_INVERT),
    ('wxXOR', wxXOR),
]

import images

class TestMaskWindow(wxScrolledWindow):
    def __init__(self, parent):
        wxScrolledWindow.__init__(self, parent, -1)
        self.SetBackgroundColour(wxColour(0,128,0))

        # A reference bitmap that we won't mask
        self.bmp_nomask  = images.getTestStar2Bitmap()

        # One that we will
        self.bmp_withmask  = images.getTestStar2Bitmap()

        # this mask comes from a monochrome bitmap
        self.bmp_themask = images.getTestMaskBitmap()
        self.bmp_themask.SetDepth(1)
        mask = wxMask(self.bmp_themask)

        # set the mask on our bitmap
        self.bmp_withmask.SetMask(mask)

        # Now we'll create a mask in a bit of an easier way, by picking a
        # colour in the image that is to be the transparent colour.
        self.bmp_withcolourmask  = images.getTestStar2Bitmap()
        mask = wxMaskColour(self.bmp_withcolourmask, wxWHITE)
        self.bmp_withcolourmask.SetMask(mask)

        self.SetScrollbars(20, 20, 700/20, 460/20)

        EVT_PAINT(self, self.OnPaint)


    def OnPaint (self, e):
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        dc.SetTextForeground(wxWHITE)

        # make an interesting background...
        dc.SetPen(wxMEDIUM_GREY_PEN)
        for i in range(100):
            dc.DrawLine(0,i*10,i*10,0)

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
        mdc = wxMemoryDC()
        i = 0
        for text, code in logicList:
            x,y = 120+150*(i%4), 20+100*(i/4)
            dc.DrawText(text, x, y-20)
            mdc.SelectObject(self.bmp_withcolourmask)
            dc.Blit(x,y, cx,cy, mdc, 0,0, code, true)
            i = i + 1




#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestMaskWindow(nb)
    return win

#----------------------------------------------------------------------



overview = """\
"""
