import wx

METHOD = 1
SCALE  = 0.5

class TestPanel(wx.Panel):
    """
    Shows some ways to scale text down
    """
    def __init__(self, *args, **kw):
        wx.Panel.__init__(self, *args, **kw)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def OnPaint(self, evt):
        pdc = wx.PaintDC(self)
        font = wx.FFont(28, wx.SWISS)
        pdc.SetFont(font)
        txt = "Here is some text"
        
        # draw the original unscaled text
        pdc.DrawText(txt, 10, 10)

        if METHOD == 0:
            # Create a new font that is scaled
            points = font.GetPointSize() * SCALE
            font.SetPointSize(points)
            pdc.SetFont(font)
            pdc.DrawText(txt, 10, 100)

        if METHOD == 1:
            # Draw to a bitmap and scale that
            w,h = pdc.GetTextExtent(txt)
            bmp = wx.EmptyBitmap(w, h)
            mdc = wx.MemoryDC(bmp)
            mdc.SetBackground(wx.Brush(self.GetBackgroundColour()))
            mdc.Clear()
            mdc.SetFont(font)
            mdc.DrawText(txt, 0,0)
            del mdc

            image = bmp.ConvertToImage()
            image = image.Scale(w*SCALE, h*SCALE, wx.IMAGE_QUALITY_HIGH)
            bmp = wx.BitmapFromImage(image)
            bmp.SetMaskColour(self.GetBackgroundColour())
            
            pdc.DrawBitmap(bmp, 10,100)
            

        elif METHOD == 2:
            # Just scale the DC and draw the text again
            pdc.SetUserScale(SCALE, SCALE)
            pdc.DrawText(txt, 10/SCALE, 100/SCALE)
            

        elif METHOD == 3:
            # Use a GraphicsContext for scaling.  Since it uses a
            # floating point coordinate system, it can be more
            # accurate.
            gc = wx.GraphicsContext.Create(pdc)
            gc.Scale(SCALE, SCALE)
            gc.SetFont(font)
            gc.DrawText(txt, 10/SCALE, 100/SCALE)
            


app = wx.App()
frm = wx.Frame(None, title="test_scaleText")
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()

