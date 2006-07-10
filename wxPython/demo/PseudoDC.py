
import wx
import images
import random

#---------------------------------------------------------------------------

W = 1000
H = 1000
SW = 150
SH = 150
SHAPE_COUNT = 100
MOVING_COUNT = 10

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
        
        # use timer to move one of the objects around
        self.timer = wx.Timer()
        self.timer.SetOwner(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer)
        self.timer.Start(50)
        self.movingids = random.sample(self.objids, MOVING_COUNT)
        self.velocitydict = {}
        for id in self.movingids:
            vx = random.randint(1,5) * random.choice([-1,1])
            vy = random.randint(1,5) * random.choice([-1,1])
            self.velocitydict[id] = (vx,vy)

    def OnTimer(self, event):
        # get the current position
        xv, yv = self.GetViewStart()
        dx, dy = self.GetScrollPixelsPerUnit()
        x, y   = (xv * dx, yv * dy)
        w, h   = self.GetClientSizeTuple()
        clip = wx.Rect(x,y,w,h)
        refreshed = False
        for id in self.movingids:
            r = self.pdc.GetIdBounds(id)
            # get new object position
            (vx,vy) = self.velocitydict[id]
            x = r.x + vx
            y = r.y + vy
            # check for bounce
            if x < 0: 
                x = -x
                vx = -vx
            if x >= W: 
                x = W - (x - W)
                vx = -vx
            if y < 0: 
                y = -y
                vy = -vy
            if y >= H:
                y = H - (y - H)
                vy = -vy
            self.velocitydict[id] = (vx,vy)
            # get change
            dx = x - r.x
            dy = y - r.y
            # translate the object
            self.pdc.TranslateId(id, dx, dy)
            # redraw
            r.x -= 20
            if dx < 0:
                r.x = x
            r.y -= 20
            if dy < 0:
                r.y = y
            r.width += abs(dx) + 40
            r.height += abs(dy) + 40
            if r.Intersects(clip):
                r.x -= clip.x
                r.y -= clip.y
                refreshed = True
                self.RefreshRect(r, False)

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
        # we need to clear the dc BEFORE calling PrepareDC
        bg = wx.Brush(self.GetBackgroundColour())
        dc.SetBackground(bg)
        dc.Clear()
        # use PrepateDC to set position correctly
        self.PrepareDC(dc)
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
                dc.SetPen(self.RandomPen())
                dc.DrawPoint(x,y)
                dc.SetIdBounds(id,wx.Rect(x,y,1,1))
            elif choice in (2,3):
                x1 = random.randint(0, W-SW)
                y1 = random.randint(0, H-SH)
                x2 = random.randint(x1, x1+SW)
                y2 = random.randint(y1, y1+SH)
                dc.SetPen(self.RandomPen())
                dc.DrawLine(x1,y1,x2,y2)
                dc.SetIdBounds(id,wx.Rect(x1,y1,x2-x1,y2-y1))
            elif choice in (4,5):
                w = random.randint(10, SW)
                h = random.randint(10, SH)
                x = random.randint(0, W - w)
                y = random.randint(0, H - h)
                dc.SetPen(self.RandomPen())
                dc.SetBrush(self.RandomBrush())
                dc.DrawRectangle(x,y,w,h)
                dc.SetIdBounds(id,wx.Rect(x,y,w,h))
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
                dc.SetTextForeground(self.RandomColor())
                dc.SetTextBackground(self.RandomColor())
                dc.DrawText(word, x, y)
                dc.SetIdBounds(id,wx.Rect(x,y,w,h))
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
                dc.SetPen(self.RandomPen())
                dc.SetBrush(self.RandomBrush())
                dc.DrawPolygon(poly, x,y)
                dc.SetIdBounds(id,wx.Rect(minx+x,miny+y,maxx-minx+x,maxy-miny+y))
                self.objids.append(id)
            elif choice == 8:
                w,h = self.bmp.GetSize()
                x = random.randint(0, W-w)
                y = random.randint(0, H-h)
                dc.DrawBitmap(self.bmp,x,y,True)
                dc.SetIdBounds(id,wx.Rect(x,y,w,h))
                self.objids.append(id)
        dc.EndDrawing()

    def ShutdownDemo(self):
        self.timer.Stop()
        del self.timer

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyCanvas(nb, wx.ID_ANY, log)
    return win

#---------------------------------------------------------------------------



overview = """
<html>
<body>
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

