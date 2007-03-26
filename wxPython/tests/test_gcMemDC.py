"""
Tests using a memory dc (or a buffered dc) as the target of a
wx.GraphicsContext.
"""

import wx
#import os; print "PID:", os.getpid(); raw_input("Press Enter...")


class TestPanel(wx.Panel):
    def __init__(self, *args, **kw):
        wx.Panel.__init__(self, *args, **kw)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)

    def OnSize(self, evt):
        self.Refresh()

    def OnPaint(self, evt):
        #dc = wx.PaintDC(self)
        dc = wx.BufferedPaintDC(self)
        gcdc = wx.GCDC(dc)

        sz = self.GetSize()
        gcdc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        gcdc.Clear()
        gcdc.DrawLine(0, 0, sz.width, sz.height)
        gcdc.DrawLine(sz.width, 0, 0, sz.height)


app = wx.App(False)
frm = wx.Frame(None, title="GC/MemoryDC")
pnl = TestPanel(frm)
frm.Show()
app.MainLoop()
