
import  random
import  time

import  wx

#----------------------------------------------------------------------

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

#----------------------------------------------------------------------

def makeRandomPoints(num, w, h):
    pnts = []

    for i in range(num):
        x = random.randint(0, w)
        y = random.randint(0, h)
        pnts.append( (x,y) )

    return pnts


def makeRandomLines(num, w, h):
    pnts = []

    for i in range(num):
        x1 = random.randint(0, w)
        y1 = random.randint(0, h)
        x2 = random.randint(0, w)
        y2 = random.randint(0, h)
        pnts.append( (x1,y1, x2,y2) )

    return pnts


def makeRandomRectangles(num, W, H):
    rects = []

    for i in range(num):
        w = random.randint(10, W/2)
        h = random.randint(10, H/2)
        x = random.randint(0, W - w)
        y = random.randint(0, H - h)
        rects.append( (x, y, w, h) )

    return rects


def makeRandomText(num):
    Np = 8 # number of characters in text
    text = []

    for i in range(num):
        word = []

        for i in range(Np):
            c = chr( random.randint(48, 122) )
            word.append( c )

        text.append( "".join(word) )

    return text


def makeRandomPolygons(num, W, H):
    Np = 8 # number of points per polygon
    polys = []

    for i in range(num):
        poly = []

        for i in range(Np):
            x = random.randint(0, W)
            y = random.randint(0, H)
            poly.append( (x,y) )

        polys.append( poly )

    return polys


def makeRandomPens(num, cache):
    pens = []

    for i in range(num):
        c = random.choice(colours)
        t = random.randint(1, 4)

        if not cache.has_key( (c, t) ):
            cache[(c, t)] = wx.Pen(c, t)

        pens.append( cache[(c, t)] )

    return pens


def makeRandomBrushes(num, cache):
    brushes = []

    for i in range(num):
        c = random.choice(colours)

        if not cache.has_key(c):
            cache[c] = wx.Brush(c)

        brushes.append( cache[c] )

    return brushes


def makeRandomColors(num):
    colors = []

    for i in range(num):
        c = random.choice(colours)
        colors.append(wx.NamedColour(c))
    return colors



pencache = {}
brushcache = {}
points = None
lines = None
rectangles = None
polygons = None
text = None
pens = None
brushes = None
colors1 = None
colors2 = None


def Init(w, h, n):
    global pencache
    global brushcache
    global points
    global lines
    global rectangles
    global polygons
    global text
    global pens
    global brushes
    global colors1
    global colors2

    # make some lists of random shapes
    points = makeRandomPoints(n, w, h)

    try:
        import Numeric
        Apoints = Numeric.array(points)
    except:
        pass

    lines = makeRandomLines(n, w, h)
    rectangles = makeRandomRectangles(n, w, h)
    polygons = makeRandomPolygons(n, w, h)
    text = makeRandomText(n)

    # make some random pens and brushes
    pens  = makeRandomPens(n, pencache)
    brushes = makeRandomBrushes(n, brushcache)
    # make some random color lists
    colors1 = makeRandomColors(n)
    colors2 = makeRandomColors(n)



def TestPoints(dc,log):
    dc.BeginDrawing()
    start = time.time()
    dc.SetPen(wx.Pen("BLACK", 4))

    dc.DrawPointList(points)
    dc.DrawPointList(points, wx.Pen("RED", 2))
    dc.DrawPointList(points, pens)

    dc.EndDrawing()
    log.write("DrawTime: %s seconds with DrawPointList\n" % (time.time() - start))


def TestArrayPoints(dc,log):
    try:
        import Numeric

        dc.BeginDrawing()
        start = time.time()
        dc.SetPen(wx.Pen("BLACK", 1))

        for i in range(1):
            dc.DrawPointList(Apoints)

        #dc.DrawPointList(Apoints, wx.Pen("RED", 2))
        #dc.DrawPointList(Apoints, pens)
        dc.EndDrawing()
        log.write("DrawTime: %s seconds with DrawPointList an Numpy Array\n" % (time.time() - start))
    except ImportError:
        log.write("Couldn't import Numeric")
        pass


def TestLines(dc,log):
    dc.BeginDrawing()
    start = time.time()

    dc.SetPen(wx.Pen("BLACK", 2))
    dc.DrawLineList(lines)
    dc.DrawLineList(lines, wx.Pen("RED", 2))
    dc.DrawLineList(lines, pens)

    dc.EndDrawing()
    log.write("DrawTime: %s seconds with DrawLineList\n" % (time.time() - start))


def TestRectangles(dc,log):
    dc.BeginDrawing()
    start = time.time()

    dc.SetPen( wx.Pen("BLACK",1) )
    dc.SetBrush( wx.Brush("RED") )

    dc.DrawRectangleList(rectangles)
    dc.DrawRectangleList(rectangles,pens)
    dc.DrawRectangleList(rectangles,pens[0],brushes)
    dc.DrawRectangleList(rectangles,pens,brushes[0])
    dc.DrawRectangleList(rectangles,None,brushes)
##    for i in range(10):
##        #dc.DrawRectangleList(rectangles,pens,brushes)
##        dc.DrawRectangleList(rectangles)

    dc.EndDrawing()
    log.write("DrawTime: %s seconds with DrawRectanglesList\n" % (time.time() - start))


def TestEllipses(dc,log):
    dc.BeginDrawing()
    start = time.time()

    dc.SetPen( wx.Pen("BLACK",1) )
    dc.SetBrush( wx.Brush("RED") )

    dc.DrawEllipseList(rectangles)
    dc.DrawEllipseList(rectangles,pens)
    dc.DrawEllipseList(rectangles,pens[0],brushes)
    dc.DrawEllipseList(rectangles,pens,brushes[0])
    dc.DrawEllipseList(rectangles,None,brushes)
    dc.DrawEllipseList(rectangles,pens,brushes)

    dc.EndDrawing()
    log.write("DrawTime: %s seconds with DrawEllipsesList\n" % (time.time() - start))


def TestRectanglesArray(dc,log):
    try:
        import Numeric
        Apoints = Numeric.array(rectangles)

        dc.BeginDrawing()
        start = time.time()
        dc.SetPen(wx.Pen("BLACK", 1))
        dc.DrawRectangleList(rectangles)
        dc.DrawRectangleList(rectangles,pens)
        dc.DrawRectangleList(rectangles,pens[0],brushes)
        dc.DrawRectangleList(rectangles,pens,brushes[0])
        dc.DrawRectangleList(rectangles,None,brushes)
##        for i in range(10):
##            #dc.DrawRectangleList(rectangles,pens,brushes)
##            dc.DrawRectangleList(rectangles)

        dc.EndDrawing()
        log.write("DrawTime: %s seconds with DrawRectangleList and Numpy Array\n" % (time.time() - start))
    except ImportError:
        log.write("Couldn't import Numeric")
        pass


def TestRectanglesLoop(dc,log):
    dc.BeginDrawing()

    start = time.time()
    dc.DrawRectangleList(rectangles,pens,brushes)
    log.write("DrawTime: %s seconds with DrawRectanglesList\n" % (time.time() - start))

    start = time.time()

    for i in range(len(rectangles)):
        dc.SetPen( pens[i] )
        dc.SetBrush( brushes[i]  )
        dc.DrawRectangle(rectangles[i][0],rectangles[i][1],rectangles[i][2],rectangles[i][3])

    dc.EndDrawing()
    log.write("DrawTime: %s seconds with Python loop\n" % (time.time() - start))


def TestPolygons(dc,log):
    dc.BeginDrawing()

    start = time.time()
    dc.SetPen(wx.Pen("BLACK", 1))
    dc.DrawPolygonList(polygons)
    dc.DrawPolygonList(polygons,pens)
    dc.DrawPolygonList(polygons,pens[0],brushes)
    dc.DrawPolygonList(polygons,pens,brushes[0])
    dc.DrawPolygonList(polygons,None,brushes)
    log.write("DrawTime: %s seconds with DrawPolygonList\n" % (time.time() - start))

    dc.EndDrawing()


def TestText(dc,log):
    dc.BeginDrawing()

    start = time.time()

    # NOTE: you need to set BackgroundMode for the background colors to be used.
    dc.SetBackgroundMode(wx.SOLID)
    foreground = colors1
    background = colors2
    dc.DrawTextList(text, points, foreground, background)

    log.write("DrawTime: %s seconds with DrawTextList\n" % (time.time() - start))

    dc.EndDrawing()



class TestNB(wx.Notebook):
    def __init__(self, parent, id, log):
        style = wx.NB_BOTTOM

        if wx.Platform == "__WXMAC__":
            style = 0

        wx.Notebook.__init__(self, parent, id, style=style)
        self.log = log

        # Initialize our various samples and add them to the notebook.
        win = DrawPanel(self, TestEllipses, log)
        self.AddPage(win, 'Ellipses')

        win = DrawPanel(self, TestText, log)
        self.AddPage(win, 'Text')

        win = DrawPanel(self, TestPolygons, log)
        self.AddPage(win, 'Polygons')

        win = DrawPanel(self, TestPoints, log)
        self.AddPage(win, 'Points')

        win = DrawPanel(self, TestLines, log)
        self.AddPage(win, 'Lines')

        win = DrawPanel(self, TestRectangles, log)
        self.AddPage(win, 'Rectangles')

# Class used for all the various sample pages; the mechanics are the same
# for each one with regards to the notebook. The only difference is
# the function we use to draw on it.
class DrawPanel(wx.Panel):
    def __init__(self, parent, drawFun, log):
        wx.Panel.__init__(self, parent, -1)
        self.SetBackgroundColour(wx.WHITE)

        self.log = log
        self.drawFun = drawFun
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        dc.Clear()
        self.drawFun(dc,self.log)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    w = nb.GetClientSize().width
    h = nb.GetClientSize().height
    if w < 600: w = 600
    if h < 400: h = 400
    Init(w, h, 200)
    win = TestNB(nb, -1, log)
    return win

#----------------------------------------------------------------------


overview = """\

Some methods have been added to wx.DC to help with optimization of
drawing routines.  Currently they are:

<pre>
    DrawPointList(sequence, pens=None)
</pre>
    Where sequence is a tuple, list, whatever of 2 element tuples
    (x, y) and pens is either None, a single pen or a list of pens.

<pre>
    DrawLineList(sequence, pens=None)
</pre>
     Where sequence is a tuple, list, whatever of 4 element tuples
     (x1,y1, x2,y2) and pens is either None, a single pen or a list
     of pens.

<pre>
    DrawRectangleList(rectangles, pens=None, brushes=None)
</pre>


<pre>
    DrawEllipseList(ellipses, pens=None, brushes=None)
</pre>


<pre>
    DrawPolygonList(polygons, pens=None, brushes=None)
</pre>


<pre>
    DrawTextList(textList, coords, foregrounds = None, backgrounds = None)
</pre>

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

