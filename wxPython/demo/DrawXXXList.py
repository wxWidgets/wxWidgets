
from wxPython.wx import *
import whrandom, time

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
        x = whrandom.randint(0, w)
        y = whrandom.randint(0, h)
        pnts.append( (x,y) )
    return pnts


def makeRandomLines(num, w, h):
    pnts = []
    for i in range(num):
        x1 = whrandom.randint(0, w)
        y1 = whrandom.randint(0, h)
        x2 = whrandom.randint(0, w)
        y2 = whrandom.randint(0, h)
        pnts.append( (x1,y1, x2,y2) )
    return pnts


def makeRandomPens(num, cache):
    pens = []
    for i in range(num):
        c = whrandom.choice(colours)
        t = whrandom.randint(1, 4)
        if not cache.has_key( (c, t) ):
            cache[(c, t)] = wxPen(c, t)
        pens.append( cache[(c, t)] )
    return pens


class TestPanel(wxPanel):
    def __init__(self, parent, size, log):
        wxPanel.__init__(self, parent, -1, size=size)
        self.log = log
        self.SetBackgroundColour(wxWHITE)

        w = size.width
        h = size.height
        pencache = {}

        # make some lists of random points
        self.pnts1 = makeRandomPoints(1000, w, h)
        self.pnts2 = makeRandomPoints(1000, w, h)
        self.pnts3 = makeRandomPoints(1000, w, h)
        self.pens1  = makeRandomPens(1000, pencache)

        # and now some lines
        self.lines1 = makeRandomLines(500, w, h)
        self.lines2 = makeRandomLines(500, w, h)
        self.lines3 = makeRandomLines(500, w, h)
        self.pens2  = makeRandomPens(500, pencache)

        EVT_PAINT(self, self.OnPaint)


    def OnPaint(self, evt):
        dc = wxPaintDC(self)
        dc.BeginDrawing()
        start = time.time()

        dc.SetPen(wxPen("BLACK", 1))
        dc.DrawPointList(self.pnts1)
        dc.DrawPointList(self.pnts2, wxPen("RED", 2))
        dc.DrawPointList(self.pnts3, self.pens1)

        dc.SetPen(wxPen("BLACK", 1))
        dc.DrawLineList(self.lines1)
        dc.DrawLineList(self.lines2, wxPen("RED", 2))
        dc.DrawLineList(self.lines3, self.pens2)

        dc.EndDrawing()
        self.log.write("DrawTime: %s seconds\n" % (time.time() - start))
        self.log.write("GetBoundingBox: %s\n" % (dc.GetBoundingBox(), ))

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    w = nb.GetClientSize().width
    h = nb.GetClientSize().height
    if w < 300: w = 300
    if h < 300: h = 300
    win = wxPanel(nb, -1)
    tp = TestPanel(win, wxSize(w, h), log)
    def OnPanelSize(evt, tp=tp):
        tp.SetSize(evt.GetSize())
    EVT_SIZE(win, OnPanelSize)
    return win

#----------------------------------------------------------------------


overview = """\

Some methods have been added to wxDC to help with optimization of
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
     (x1,y1, x2,y2) andd pens is either None, a single pen or a list
     of pens.

"""
