
from wxPython.wx import *
from wxPython.lib import colourdb

import images


# This loads a whole bunch of new color names and values
# into wxTheColourDatabase

colourdb.updateColourDB()

#----------------------------------------------------------------------

class TestWindow(wxScrolledWindow):
    def __init__(self, parent):
        wxScrolledWindow.__init__(self, parent, -1)

        self.clrList = colourdb.getColourList()
        #self.clrList.sort()
        self.bg_bmp = images.getGridBGBitmap()

        EVT_PAINT(self, self.OnPaint)
        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
        #self.SetBackgroundColour("WHITE")

        self.font = wxFont(10, wxSWISS, wxNORMAL, wxNORMAL)
        dc = wxClientDC(self)
        dc.SetFont(self.font)

        w,h,d,e = dc.GetFullTextExtent("Wy") # a wide character and one that decends
        self.textHeight = h + d
        self.lineHeight = self.textHeight + 5
        self.cellWidth = w

        numCells = 24
        self.SetScrollbars(self.cellWidth, self.lineHeight, numCells, len(self.clrList) + 2)


    def TileBackground(self, dc):
        # tile the background bitmap
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


    def OnEraseBackground(self, evt):
        dc = evt.GetDC()
        if not dc:
            dc = wxClientDC(self)
            rect = self.GetUpdateRegion().GetBox()
            dc.SetClippingRegion(rect.x, rect.y, rect.width, rect.height)
        self.TileBackground(dc)


    def OnPaint(self, evt):
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        self.Draw(dc, self.GetUpdateRegion(), self.GetViewStart())


    def Draw(self, dc, rgn=None, vs=None):
        dc.BeginDrawing()
        dc.SetTextForeground("BLACK")
        dc.SetPen(wxPen("BLACK", 1, wxSOLID))
        dc.SetFont(self.font)
        colours = self.clrList
        numColours = len(colours)

        if rgn:
            # determine the subset that has been exposed and needs drawn
            rect = rgn.GetBox()
            pixStart = vs[1]*self.lineHeight + rect.y
            pixStop  = pixStart + rect.height
            start = pixStart / self.lineHeight - 1
            stop = pixStop / self.lineHeight
        else:
            start = 0
            stop = numColours

        for line in range(max(0,start), min(stop,numColours)):
            clr = colours[line]
            y = (line+1) * self.lineHeight + 2
            dc.DrawText(clr, self.cellWidth, y)

            brush = wxBrush(clr, wxSOLID)
            dc.SetBrush(brush)
            dc.DrawRectangle(12 * self.cellWidth, y, 6 * self.cellWidth, self.textHeight)

        dc.EndDrawing()


# On wxGTK there needs to be a panel under wxScrolledWindows if they are
# going to be in a wxNotebook...
class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)
        self.win = TestWindow(self)
        EVT_SIZE(self, self.OnSize)

    def OnSize(self, evt):
        self.win.SetSize(evt.GetSize())



#----------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win

#----------------------------------------------------------------------

overview = """
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

