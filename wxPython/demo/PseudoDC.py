
import wx
import images
import random

#---------------------------------------------------------------------------

W = 2000
H = 2000
SW = 150
SH = 150
SHAPE_COUNT = 2500
hitradius = 5

#---------------------------------------------------------------------------

colours = [
    "BLACK",
    "BLUE",
    "BLUE VIOLET",
    "BROWN",
    "CYAN",
    "DARK GREY",
    "DARK GREEN",
    "GOLD",
    "GREY",
    "GREEN",
    "MAGENTA",
    "NAVY",
    "PINK",
    "RED",
    "SKY BLUE",
    "VIOLET",
    "YELLOW",
    ]



class MyCanvas(wx.ScrolledWindow):
    def __init__(self, parent, id, log, size = wx.DefaultSize):
        wx.ScrolledWindow.__init__(self, parent, id, (0, 0), size=size, style=wx.SUNKEN_BORDER)

        self.lines = []
        self.maxWidth  = W
        self.maxHeight = H
        self.x = self.y = 0
        self.curLine = []
        self.drawing = False

        self.SetBackgroundColour("WHITE")
        bmp = images.getTest2Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        self.bmp = bmp

        self.SetVirtualSize((self.maxWidth, self.maxHeight))
        self.SetScrollRate(20,20)
        
        # create a PseudoDC to record our drawing
        self.pdc = wx.PseudoDC()
        self.pen_cache = {}
        self.brush_cache = {}
        self.DoDrawing(self.pdc)
        log.write('Created PseudoDC draw list with %d operations!'%self.pdc.GetLen())

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, lambda x:None)
        self.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouse)
        
        # vars for handling mouse clicks
        self.dragid = -1
        self.lastpos = (0,0)
    
    def ConvertEventCoords(self, event):
        xView, yView = self.GetViewStart()
        xDelta, yDelta = self.GetScrollPixelsPerUnit()
        return (event.GetX() + (xView * xDelta),
            event.GetY() + (yView * yDelta))

    def OffsetRect(self, r):
        xView, yView = self.GetViewStart()
        xDelta, yDelta = self.GetScrollPixelsPerUnit()
        r.OffsetXY(-(xView*xDelta),-(yView*yDelta))

    def OnMouse(self, event):
        global hitradius
        if event.LeftDown():
            x,y = self.ConvertEventCoords(event)
            #l = self.pdc.FindObjectsByBBox(x, y)
            l = self.pdc.FindObjects(x, y, hitradius)
            for id in l:
                if not self.pdc.GetIdGreyedOut(id):
                    self.dragid = id
                    self.lastpos = (event.GetX(),event.GetY())
                    break
        elif event.RightDown():
            x,y = self.ConvertEventCoords(event)
            #l = self.pdc.FindObjectsByBBox(x, y)
            l = self.pdc.FindObjects(x, y, hitradius)
            if l:
                self.pdc.SetIdGreyedOut(l[0], not self.pdc.GetIdGreyedOut(l[0]))
                r = self.pdc.GetIdBounds(l[0])
                r.Inflate(4,4)
                self.OffsetRect(r)
                self.RefreshRect(r, False)
        elif event.Dragging() or event.LeftUp():
            if self.dragid != -1:
                x,y = self.lastpos
                dx = event.GetX() - x
                dy = event.GetY() - y
                r = self.pdc.GetIdBounds(self.dragid)
                self.pdc.TranslateId(self.dragid, dx, dy)
                r2 = self.pdc.GetIdBounds(self.dragid)
                r = r.Union(r2)
                r.Inflate(4,4)
                self.OffsetRect(r)
                self.RefreshRect(r, False)
                self.lastpos = (event.GetX(),event.GetY())
            if event.LeftUp():
                self.dragid = -1

    def RandomPen(self):
        c = random.choice(colours)
        t = random.randint(1, 4)
        if not self.pen_cache.has_key( (c, t) ):
            self.pen_cache[(c, t)] = wx.Pen(c, t)
        return self.pen_cache[(c, t)]


    def RandomBrush(self):
        c = random.choice(colours)
        if not self.brush_cache.has_key(c):
            self.brush_cache[c] = wx.Brush(c)

        return self.brush_cache[c]

    def RandomColor(self):
        return random.choice(colours)


    def OnPaint(self, event):
        # Create a buffered paint DC.  It will create the real
        # wx.PaintDC and then blit the bitmap to it when dc is
        # deleted.  
        dc = wx.BufferedPaintDC(self)
        # use PrepateDC to set position correctly
        self.PrepareDC(dc)
        # we need to clear the dc BEFORE calling PrepareDC
        bg = wx.Brush(self.GetBackgroundColour())
        dc.SetBackground(bg)
        dc.Clear()
        # create a clipping rect from our position and size
        # and the Update Region
        xv, yv = self.GetViewStart()
        dx, dy = self.GetScrollPixelsPerUnit()
        x, y   = (xv * dx, yv * dy)
        rgn = self.GetUpdateRegion()
        rgn.Offset(x,y)
        r = rgn.GetBox()
        # draw to the dc using the calculated clipping rect
        self.pdc.DrawToDCClipped(dc,r)

    def DoDrawing(self, dc):
        random.seed()
        self.objids = []
        self.boundsdict = {}
        dc.BeginDrawing()
        for i in range(SHAPE_COUNT):
            id = wx.NewId()
            dc.SetId(id)
            choice = random.randint(0,8)
            if choice in (0,1):
                x = random.randint(0, W)
                y = random.randint(0, H)
                pen = self.RandomPen()
                dc.SetPen(pen)
                dc.DrawPoint(x,y)
                r = wx.Rect(x,y,1,1)
                r.Inflate(pen.GetWidth(),pen.GetWidth())
                dc.SetIdBounds(id,r)
            elif choice in (2,3):
                x1 = random.randint(0, W-SW)
                y1 = random.randint(0, H-SH)
                x2 = random.randint(x1, x1+SW)
                y2 = random.randint(y1, y1+SH)
                pen = self.RandomPen()
                dc.SetPen(pen)
                dc.DrawLine(x1,y1,x2,y2)
                r = wx.Rect(x1,y1,x2-x1,y2-y1)
                r.Inflate(pen.GetWidth(),pen.GetWidth())
                dc.SetIdBounds(id,r)
            elif choice in (4,5):
                w = random.randint(10, SW)
                h = random.randint(10, SH)
                x = random.randint(0, W - w)
                y = random.randint(0, H - h)
                pen = self.RandomPen()
                dc.SetPen(pen)
                dc.SetBrush(self.RandomBrush())
                dc.DrawRectangle(x,y,w,h)
                r = wx.Rect(x,y,w,h)
                r.Inflate(pen.GetWidth(),pen.GetWidth())
                dc.SetIdBounds(id,r)
                self.objids.append(id)
            elif choice == 6:
                Np = 8 # number of characters in text
                word = []
                for i in range(Np):
                    c = chr( random.randint(48, 122) )
                    word.append( c )
                word = "".join(word)
                w,h = self.GetFullTextExtent(word)[0:2]
                x = random.randint(0, W-w)
                y = random.randint(0, H-h)
                dc.SetFont(self.GetFont())
                dc.SetTextForeground(self.RandomColor())
                dc.SetTextBackground(self.RandomColor())
                dc.DrawText(word, x, y)
                r = wx.Rect(x,y,w,h)
                r.Inflate(2,2)
                dc.SetIdBounds(id, r)
                self.objids.append(id)
            elif choice == 7:
                Np = 8 # number of points per polygon
                poly = []
                minx = SW
                miny = SH
                maxx = 0
                maxy = 0
                for i in range(Np):
                    x = random.randint(0, SW)
                    y = random.randint(0, SH)
                    if x < minx: minx = x
                    if x > maxx: maxx = x
                    if y < miny: miny = y
                    if y > maxy: maxy = y
                    poly.append(wx.Point(x,y))
                x = random.randint(0, W-SW)
                y = random.randint(0, H-SH)
                pen = self.RandomPen()
                dc.SetPen(pen)
                dc.SetBrush(self.RandomBrush())
                dc.DrawPolygon(poly, x,y)
                r = wx.Rect(minx+x,miny+y,maxx-minx,maxy-miny)
                r.Inflate(pen.GetWidth(),pen.GetWidth())
                dc.SetIdBounds(id,r)
                self.objids.append(id)
            elif choice == 8:
                w,h = self.bmp.GetSize()
                x = random.randint(0, W-w)
                y = random.randint(0, H-h)
                dc.DrawBitmap(self.bmp,x,y,True)
                dc.SetIdBounds(id,wx.Rect(x,y,w,h))
                self.objids.append(id)
        dc.EndDrawing()

class ControlPanel(wx.Panel):
    def __init__(self, parent, id, pos=wx.DefaultPosition,
            size=wx.DefaultSize, style = wx.TAB_TRAVERSAL):
        wx.Panel.__init__(self,parent,id,pos,size,style)
        lbl = wx.StaticText(self, wx.ID_ANY, "Hit Test Radius: ")
        lbl2 = wx.StaticText(self, wx.ID_ANY, "Left Click to drag, Right Click to enable/disable")
        sc = wx.SpinCtrl(self, wx.ID_ANY, "5")
        sc.SetRange(0,100)
        global hitradius
        sc.SetValue(hitradius)
        self.sc = sc
        sz = wx.BoxSizer(wx.HORIZONTAL)
        sz.Add(lbl,0,wx.EXPAND)
        sz.Add(sc,0)
        sz.Add(lbl2,0,wx.LEFT,5)
        sz.Add((10,10),1,wx.EXPAND)
        self.SetSizerAndFit(sz)
        sc.Bind(wx.EVT_SPINCTRL,self.OnChange)
        sc.Bind(wx.EVT_TEXT,self.OnChange)

    def OnChange(self, event):
        global hitradius
        hitradius = self.sc.GetValue()
    

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    pnl = wx.Panel(nb, wx.ID_ANY,size=(200,30))
    pnl2 = ControlPanel(pnl,wx.ID_ANY)
    win = MyCanvas(pnl, wx.ID_ANY, log)
    sz = wx.BoxSizer(wx.VERTICAL)
    sz.Add(pnl2,0,wx.EXPAND|wx.ALL,5)
    sz.Add(win,1,wx.EXPAND)
    pnl.SetSizerAndFit(sz)
    return pnl

#---------------------------------------------------------------------------



overview = """
<html>
<body>
<h2>wx.PseudoDC</h2>

The wx.PseudoDC class provides a way to record operations on a DC and then
play them back later.  The PseudoDC can be passed to a drawing routine as
if it were a real DC.  All Drawing methods are supported except Blit but
GetXXX methods are not supported and none of the drawing methods return
a value. The PseudoDC records the drawing to an operation
list.  The operations can be played back to a real DC using:<pre>
DrawToDC(dc)
</pre>
The operations can be tagged with an id in order to associated them with a
specific object.  To do this use:<pre>
    SetId(id)
</pre>
Every operation after this will be associated with id until SetId is called
again.  The PseudoDC also supports object level clipping.  To enable this use:<pre>
    SetIdBounds(id,rect)
</pre>
for each object that should be clipped.  Then use:<pre>
    DrawToDCClipped(dc, clippingRect)
</pre>
To draw the PseudoDC to a real dc. This is useful for large scrolled windows 
where many objects are offscreen.

Objects can be moved around without re-drawing using:<pre>
    TranslateId(id, dx, dy)
</pre>

To re-draw an object use:<pre>
    ClearId(id)
    SetId(id)
</pre>
and then re-draw the object.
</body>
</html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

