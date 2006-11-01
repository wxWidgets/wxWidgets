import wx
import math
import random

class RadarGraph(wx.Window):
    """
    A simple radar graph that plots a collection of values in the
    range of 0-100 onto a polar coordinate system designed to easily
    show outliers, etc.  You might use this kind of graph to monitor
    some sort of resource allocation metrics, and a quick glance at
    the graph can tell you when conditions are good (within some
    accepted tolerance level) or approaching critical levels (total
    resource consumption).
    """
    def __init__(self, parent, title, labels):
        wx.Window.__init__(self, parent)
        self.title = title
        self.labels = labels
        self.data = [0.0] * len(labels)
        self.titleFont = wx.Font(14, wx.SWISS, wx.NORMAL, wx.BOLD)
        self.labelFont = wx.Font(10, wx.SWISS, wx.NORMAL, wx.NORMAL)

        self.InitBuffer()

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def OnSize(self, evt):
        # When the window size changes we need a new buffer.
        self.InitBuffer()


    def OnPaint(self, evt):
        # This automatically Blits self.buffer to a wx.PaintDC when
        # the dc is destroyed, and so nothing else needs done.
        dc = wx.BufferedPaintDC(self, self.buffer)


    def InitBuffer(self):
        # Create the buffer bitmap to be the same size as the window,
        # then draw our graph to it.  Since we use wx.BufferedDC
        # whatever is drawn to the buffer is also drawn to the window.
        w, h = self.GetClientSize()        
        self.buffer = wx.EmptyBitmap(w, h)
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawGraph(dc)
        

    def GetData(self):
        return self.data

    def SetData(self, newData):
        assert len(newData) == len(self.data)
        self.data = newData[:]
        
        # The data has changed, so update the buffer and the window
        dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
        self.DrawGraph(dc)

    
    def PolarToCartesian(self, radius, angle, cx, cy):
        x = radius * math.cos(math.radians(angle))
        y = radius * math.sin(math.radians(angle))
        return (cx+x, cy-y)


    def DrawGraph(self, dc):
        spacer = 10
        scaledmax = 150.0

        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        dw, dh = dc.GetSize()

        # Find out where to draw the title and do it
        dc.SetFont(self.titleFont)
        tw, th = dc.GetTextExtent(self.title)
        dc.DrawText(self.title, (dw-tw)/2, spacer)

        # find the center of the space below the title
        th = th + 2*spacer
        cx = dw/2
        cy = (dh-th)/2 + th

        # calculate a scale factor to use for drawing the graph based
        # on the minimum available width or height
        mindim = min(cx, (dh-th)/2)
        scale = mindim/scaledmax

        # draw the graph axis and "bulls-eye" with rings at scaled 25,
        # 50, 75 and 100 positions
        dc.SetPen(wx.Pen("black", 1))
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawCircle(cx,cy, 25*scale)
        dc.DrawCircle(cx,cy, 50*scale)
        dc.DrawCircle(cx,cy, 75*scale)
        dc.DrawCircle(cx,cy, 100*scale)

        dc.SetPen(wx.Pen("black", 2))
        dc.DrawLine(cx-110*scale, cy, cx+110*scale, cy)
        dc.DrawLine(cx, cy-110*scale, cx, cy+110*scale)

        # Now find the coordinates for each data point, draw the
        # labels, and find the max data point
        dc.SetFont(self.labelFont)
        maxval = 0
        angle = 0
        polypoints = []
        for i, label in enumerate(self.labels):
            val = self.data[i]
            point = self.PolarToCartesian(val*scale, angle, cx, cy)
            polypoints.append(point)
            x, y = self.PolarToCartesian(125*scale, angle, cx,cy)
            dc.DrawText(label, x, y)
            if val > maxval:
                maxval = val
            angle = angle + 360/len(self.labels)
            
        # Set the brush color based on the max value (green is good,
        # red is bad)
        c = "forest green"
        if maxval > 70:
            c = "yellow"
        if maxval > 95:
            c = "red"

        # Finally, draw the plot data as a filled polygon
        dc.SetBrush(wx.Brush(c))
        dc.SetPen(wx.Pen("navy", 3))
        dc.DrawPolygon(polypoints)
        

        
class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Double Buffered Drawing",
                          size=(480,480))
        self.plot = RadarGraph(self, "Sample 'Radar' Plot",
                          ["A", "B", "C", "D", "E", "F", "G", "H"])

        # Set some random initial data values
        data = []
        for d in self.plot.GetData():
            data.append(random.randint(0, 75))
        self.plot.SetData(data)

        # Create a timer to update the data values
        self.Bind(wx.EVT_TIMER, self.OnTimeout)
        self.timer = wx.Timer(self)
        self.timer.Start(500)


    def OnTimeout(self, evt):
        # simulate the positive or negative growth of each data value
        data = []
        for d in self.plot.GetData():
            val = d + random.uniform(-5, 5)
            if val < 0:
                val = 0
            if val > 110:
                val = 110
            data.append(val)
        self.plot.SetData(data)

        
app = wx.PySimpleApp()
frm = TestFrame()
frm.Show()
app.MainLoop()
