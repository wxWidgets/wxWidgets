
import wx
import wx.lib.colourdb

import images


#----------------------------------------------------------------------

class TestWindow(wx.ScrolledWindow):
    def __init__(self, parent):
        wx.ScrolledWindow.__init__(self, parent, -1)

        # Populate our color list
        self.clrList = wx.lib.colourdb.getColourInfoList()

        # Just for style points, we'll use this as a background image.
        self.bg_bmp = images.getGridBGBitmap()

        # This could also be done by getting the window's default font;
        # either way, we need to have a font loaded for later on.
        #self.SetBackgroundColour("WHITE")
        self.font = wx.Font(10, wx.SWISS, wx.NORMAL, wx.NORMAL)

        # Create drawing area and set its font
        dc = wx.ClientDC(self)
        dc.SetFont(self.font)

        # Using GetFullTextExtent(), we calculate a basic 'building block'
        # that will be used to draw a depiction of the color list. We're
        # using 'Wy' as the model becuase 'W' is a wide character and 'y' 
        # has a descender. This constitutes a 'worst case' scenario, which means
        # that no matter what we draw later, text-wise, we'll have room for it
        w,h,d,e = dc.GetFullTextExtent("Wy") 

        # Height plus descender
        self.textHeight = h + d

        # Pad a little bit
        self.lineHeight = self.textHeight + 5

        # ... and this is the basic width.
        self.cellWidth = w

        # jmg 11/8/03: why 24?
        numCells = 24
        
        # 'prep' our scroll bars.
        self.SetScrollbars(
            self.cellWidth, self.lineHeight, numCells, len(self.clrList) + 2
            )
            
        # Event handlers - moved here so events won't fire before init is 
        # finished.
        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)


    # tile the background bitmap loaded in __init__()
    def TileBackground(self, dc):
        sz = self.GetClientSize()
        w = self.bg_bmp.GetWidth()
        h = self.bg_bmp.GetHeight()

        # adjust for scrolled position
        spx, spy = self.GetScrollPixelsPerUnit()
        vsx, vsy = self.GetViewStart()
        dx,  dy  = (spx * vsx) % w, (spy * vsy) % h

        x = -dx

        while x < sz.width:
            y = -dy
            while y < sz.height:
                dc.DrawBitmap(self.bg_bmp, x, y)
                y = y + h

            x = x + w

    # Redraw the background over a 'damaged' area.
    def OnEraseBackground(self, evt):
        dc = evt.GetDC()

        if not dc:
            dc = wx.ClientDC(self)
            rect = self.GetUpdateRegion().GetBox()
            dc.SetClippingRect(rect)

        self.TileBackground(dc)


    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        self.PrepareDC(dc)
        self.Draw(dc, self.GetUpdateRegion(), self.GetViewStart())


    def Draw(self, dc, rgn=None, vs=None):
        dc.SetTextForeground("BLACK")
        dc.SetPen(wx.Pen("BLACK", 1, wx.SOLID))
        dc.SetFont(self.font)
        colours = self.clrList
        numColours = len(colours)

        if rgn:
            # determine the subset of the color list that has been exposed 
            # and needs drawn. This is based on all the precalculation we
            # did in __init__()
            rect = rgn.GetBox()
            pixStart = vs[1]*self.lineHeight + rect.y
            pixStop  = pixStart + rect.height
            start = pixStart / self.lineHeight - 1
            stop = pixStop / self.lineHeight
        else:
            start = 0
            stop = numColours

        for line in range(max(0,start), min(stop,numColours)):
            clr = colours[line][0]
            y = (line+1) * self.lineHeight + 2

            dc.DrawText(clr, self.cellWidth, y)

            brush = wx.Brush(clr, wx.SOLID)
            dc.SetBrush(brush)
            dc.DrawRectangle(10 * self.cellWidth, y,
                             6 * self.cellWidth, self.textHeight)
            
            dc.DrawText(str(tuple(colours[line][1:])),
                        18 * self.cellWidth, y)


# On wxGTK there needs to be a panel under wx.ScrolledWindows if they are
# going to be in a wxNotebook. And, in this demo, we are.
class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        self.win = TestWindow(self)
        self.Bind(wx.EVT_SIZE, self.OnSize)


    def OnSize(self, evt):
        self.win.SetSize(evt.GetSize())



#----------------------------------------------------------------------


def runTest(frame, nb, log):
    # This loads a whole bunch of new color names and values
    # into TheColourDatabase
    #
    # Note 11/24/03 - jg - I moved this into runTest() because
    # there must be a wx.App existing before this function
    # can be called - this is a change from 2.4 -> 2.5.
    wx.lib.colourdb.updateColourDB()

    win = TestPanel(nb)

    return win

#----------------------------------------------------------------------

overview = """
<html>
<body>
<B><font size=+2>ColourDB</font></b>

<p>wxWindows maintains a database of standard RGB colours for a predefined 
set of named colours (such as "BLACK'', "LIGHT GREY''). The application 
may add to this set if desired by using Append. There is only one instance 
of this class: <b>TheColourDatabase</b>.

<p>The <code>colourdb</code> library is a lightweight API that pre-defines
a multitude of colors for you to use 'out of the box', and this demo serves
to show you these colors (it also serves as a handy reference).

<p>A secondary benefit of this demo is the use of the <b>ScrolledWindow</b> class
and the use of various *DC() classes, including background tiling and the use of
font data to generate a "building block" type of construct for repetitive use.

<p>
<B><font size=+2>Important note</font></b>

<p>
With implementation of V2.5 and later, it is required to have a wx.App already
initialized before <b><code>wx.updateColourDB()</code></b> can be called. 
Trying to do otherwise will cause an exception to be raised.
</body>
</html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

