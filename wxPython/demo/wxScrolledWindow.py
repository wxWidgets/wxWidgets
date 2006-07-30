
from wxPython.wx import *

import images

#---------------------------------------------------------------------------

class MyCanvas(wxScrolledWindow):
    def __init__(self, parent, id = -1, size = wxDefaultSize):
        wxScrolledWindow.__init__(self, parent, id, wxPoint(0, 0), size, wxSUNKEN_BORDER)

        self.lines = []
        self.maxWidth  = 1000
        self.maxHeight = 1000
        self.count = 0

        self.SetBackgroundColour(wxNamedColor("WHITE"))
        EVT_LEFT_DOWN(self, self.OnLeftButtonEvent)
        EVT_LEFT_UP(self,   self.OnLeftButtonEvent)
        EVT_MOTION(self,    self.OnLeftButtonEvent)
        EVT_PAINT(self, self.OnPaint)

        self.SetCursor(wxStockCursor(wxCURSOR_PENCIL))
        bmp = images.getTest2Bitmap()
        mask = wxMaskColour(bmp, wxBLUE)
        bmp.SetMask(mask)
        self.bmp = bmp

        self.SetScrollbars(20, 20, self.maxWidth/20, self.maxHeight/20)


    def getWidth(self):
        return self.maxWidth

    def getHeight(self):
        return self.maxHeight


    def OnPaint(self, event):
        #self.count += 1
        #print self.count, "begin paint...",
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        self.DoDrawing(dc)
        #print "end paint"


    def DoDrawing(self, dc):
        dc.BeginDrawing()
        dc.SetPen(wxPen(wxNamedColour('RED')))
        dc.DrawRectangle(5, 5, 50, 50)

        dc.SetBrush(wxLIGHT_GREY_BRUSH)
        dc.SetPen(wxPen(wxNamedColour('BLUE'), 4))
        dc.DrawRectangle(15, 15, 50, 50)

        dc.SetFont(wxFont(14, wxSWISS, wxNORMAL, wxNORMAL))
        dc.SetTextForeground(wxColour(0xFF, 0x20, 0xFF))
        te = dc.GetTextExtent("Hello World")
        dc.DrawText("Hello World", 60, 65)

        dc.SetPen(wxPen(wxNamedColour('VIOLET'), 4))
        dc.DrawLine(5, 65+te[1], 60+te[0], 65+te[1])

        lst = [(100,110), (150,110), (150,160), (100,160)]
        dc.DrawLines(lst, -60)
        dc.SetPen(wxGREY_PEN)
        dc.DrawPolygon(lst, 75)
        dc.SetPen(wxGREEN_PEN)
        dc.DrawSpline(lst+[(100,100)])

        dc.DrawBitmap(self.bmp, 200, 20, true)
        dc.SetTextForeground(wxColour(0, 0xFF, 0x80))
        dc.DrawText("a bitmap", 200, 85)

        font = wxFont(20, wxSWISS, wxNORMAL, wxNORMAL)
        dc.SetFont(font)
        dc.SetTextForeground(wxBLACK)
        for a in range(0, 360, 45):
            dc.DrawRotatedText("Rotated text...", 300, 300, a)

        dc.SetPen(wxTRANSPARENT_PEN)
        dc.SetBrush(wxBLUE_BRUSH)
        dc.DrawRectangle(50,500,50,50)
        dc.DrawRectangle(100,500,50,50)

        dc.SetPen(wxPen(wxNamedColour('RED')))
        dc.DrawEllipticArc(200, 500, 50, 75, 0, 90)

        y = 20
        for style in [wxDOT, wxLONG_DASH, wxSHORT_DASH, wxDOT_DASH, wxUSER_DASH]:
            pen = wxPen("DARK ORCHID", 1, style)
            if style == wxUSER_DASH:
                pen.SetDashes([1, 2, 3, 4, 5, 6, 7, 8])
                pen.SetColour("RED")
            dc.SetPen(pen)
            dc.DrawLine(300, y, 400, y)
            y = y + 10

        dc.SetBrush(wxNullBrush)
        dc.SetPen(wxPen(wxColour(0xFF, 0x20, 0xFF), 1, wxSOLID))
        dc.DrawRectangle(450, 50, 100, 100)
        old_pen = dc.GetPen()
        new_pen = wxPen("BLACK", 5)
        dc.SetPen(new_pen)
        dc.DrawRectangle(470, 70, 60, 60)
        dc.SetPen(old_pen)
        dc.DrawRectangle(490, 90, 20, 20)


        self.DrawSavedLines(dc)
        dc.EndDrawing()


    def DrawSavedLines(self, dc):
        dc.SetPen(wxPen(wxNamedColour('MEDIUM FOREST GREEN'), 4))
        for line in self.lines:
            for coords in line:
                apply(dc.DrawLine, coords)


    def SetXY(self, event):
        self.x, self.y = self.ConvertEventCoords(event)

    def ConvertEventCoords(self, event):
        xView, yView = self.GetViewStart()
        xDelta, yDelta = self.GetScrollPixelsPerUnit()
        return (event.GetX() + (xView * xDelta),
                event.GetY() + (yView * yDelta))

    def OnLeftButtonEvent(self, event):
        if event.LeftDown():
            self.SetXY(event)
            self.curLine = []
            self.CaptureMouse()

        elif event.Dragging():
            dc = wxClientDC(self)
            self.PrepareDC(dc)
            dc.BeginDrawing()
            dc.SetPen(wxPen(wxNamedColour('MEDIUM FOREST GREEN'), 4))
            coords = (self.x, self.y) + self.ConvertEventCoords(event)
            self.curLine.append(coords)
            apply(dc.DrawLine, coords)
            self.SetXY(event)
            dc.EndDrawing()

        elif event.LeftUp():
            self.lines.append(self.curLine)
            self.curLine = []
            self.ReleaseMouse()


## This is an example of what to do for the EVT_MOUSEWHEEL event,
## but since wxScrolledWindow does this already it's not
## necessary to do it ourselves.

##     wheelScroll = 0
##     def OnWheel(self, evt):
##         delta = evt.GetWheelDelta()
##         rot = evt.GetWheelRotation()
##         linesPer = evt.GetLinesPerAction()
##         ws = self.wheelScroll
##         ws = ws + rot
##         lines = ws / delta
##         ws = ws - lines * delta
##         self.wheelScroll = ws
##         if lines != 0:
##             lines = lines * linesPer
##             vsx, vsy = self.GetViewStart()
##             scrollTo = vsy - lines
##             self.Scroll(-1, scrollTo)

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyCanvas(nb)
    return win

#---------------------------------------------------------------------------













overview = """\
"""
