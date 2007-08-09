# This one shows how to draw images on a DC.

import wx
import random
random.seed()

class RandomImagePlacementWindow(wx.Window):
    def __init__(self, parent, image):
        wx.Window.__init__(self, parent)
        self.photo = image.ConvertToBitmap()

        # choose some random positions to draw the image at:
        self.positions = [(10,10)]
        for x in range(50):
            x = random.randint(0, 1000)
            y = random.randint(0, 1000)
            self.positions.append( (x,y) )
            
        # Bind the Paint event
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def OnPaint(self, evt):
        # create and clear the DC
        dc = wx.PaintDC(self)
        brush = wx.Brush("sky blue")
        dc.SetBackground(brush)
        dc.Clear()

        # draw the image in random locations
        for x,y in self.positions:
            dc.DrawBitmap(self.photo, x, y, True)

        
class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Loading Images",
                          size=(640,480))
        img = wx.Image("masked-portrait.png")
        win = RandomImagePlacementWindow(self, img)
        

app = wx.PySimpleApp()
frm = TestFrame()
frm.Show()
app.MainLoop()
