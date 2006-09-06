import wx
import wx.lib.dragscroller
    
#-------------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = DragScrollerExample(nb, -1)
    return win

class DragScrollerExample(wx.ScrolledWindow):
    def __init__(self, parent, id=-1):
        wx.ScrolledWindow.__init__(self, parent, id)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightUp)

        self.SetScrollbars(1, 1, 2000, 2000, 0, 0)

        self.scroller = wx.lib.dragscroller.DragScroller(self)
        
    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.DoPrepareDC(dc)
        
        pen = wx.Pen(wx.BLACK, 5)
        dc.SetPen(pen)

        for y in range(10):
            for x in range(10):
                dc.DrawCircle(x*400+20, y*400+20, 200)
        
        dc.DrawText('Right click and drag in the direction you want to scroll.',
                    20, 20)
        dc.DrawText('The distance from the start of the drag determines the speed.',                
                    20, 50)
        
    def OnRightDown(self, event):
        self.scroller.Start(event.GetPosition())

    def OnRightUp(self, event):
        self.scroller.Stop()


#-------------------------------------------------------------------------------

overview = """<html><body>
<h2>DragScroller</h2>
<p>
A helper class that adds scrolling to a wx.ScrolledWindow in the direction
of the drag.
</body></html>
"""
  
if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
