import wx
import os
print "PID:", os.getpid()

count = 0

class ResultsFrame(wx.Frame):
    def __init__(self, parent, bmp):
        global count
        count += 1
        wx.Frame.__init__(self, parent, title=str(count), size=(100,100))
        p = wx.Panel(self)
        sb = wx.StaticBitmap(p, -1, bmp, (20,20))
        self.Show()
        


class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        b = wx.Button(self, -1, "Test", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)

    def RunTest(self, bdc, bg, fg):
        # draw to the buffered dc
        bdc.SetBackground(wx.Brush(bg))
        bdc.Clear()
        bdc.SetPen(wx.Pen(fg, 2))
        bdc.DrawLine(0,0, 30,30)
        bdc.DrawLine(0,30, 30,0)

        # now bilt it to a bitmap
        bmp = wx.EmptyBitmap(30,30)
        mdc = wx.MemoryDC()
        mdc.SelectObject(bmp)
        mdc.Blit(0,0, 30,30, bdc, 0,0)
        del mdc

        # display the results
        ResultsFrame(self, bmp)
        


    def OnButton(self, evt):
       
        # 1. test a buffered dc not using a buffer bitmap
        bdc = wx.BufferedDC(wx.ClientDC(self), self.GetSize())
        self.RunTest(bdc, "yellow", "red")
        del bdc
        
        # 2. now do one that does have a buffer bitmap
        buf = wx.EmptyBitmap(100,100)
        bdc = wx.BufferedDC(wx.ClientDC(self), buf)
        self.RunTest(bdc, "red", "blue")
        del bdc
        
        # 3. now one without a real DC
        buf = wx.EmptyBitmap(100,100)
        bdc = wx.BufferedDC(None, buf)
        self.RunTest(bdc, "purple", "yellow")
        del bdc

        # 4. finally test a real unbuffered DC
        dc = wx.ClientDC(self)
        self.RunTest(dc, "red", "white")


        # blit from the last buffer back to the main window
        buf.SetMaskColour("yellow")
        mdc = wx.MemoryDC()
        mdc.SelectObject(buf)
        dc.Blit(100,100,30,30, mdc, 0,0, useMask=True)

        

app = wx.App(0)
frm = wx.Frame(None)
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()


        
        
            
