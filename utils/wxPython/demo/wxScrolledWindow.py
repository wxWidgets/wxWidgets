
from wxPython.wx import *

#---------------------------------------------------------------------------

class MyCanvas(wxScrolledWindow):
    def __init__(self, parent):
        wxScrolledWindow.__init__(self, parent, -1, wxPoint(0, 0), wxPyDefaultSize, wxSUNKEN_BORDER)

        self.lines = []

        self.SetBackgroundColour(wxNamedColor("WHITE"))
        self.Connect(-1, -1, wxEVT_LEFT_DOWN, self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_LEFT_UP,   self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_MOTION,    self.OnLeftButtonEvent)

        self.SetCursor(wxStockCursor(wxCURSOR_PENCIL))
        bmp = wxBitmap('bitmaps/test2.bmp', wxBITMAP_TYPE_BMP)
        self.bmp = bmp

        self.SetScrollbars(20, 20, 50, 50)


    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        self.DoDrawing(dc)


    def DoDrawing(self, dc):
        dc.BeginDrawing()
        #dc.Clear()
        pen1 = wxPen(wxNamedColour('RED'))
        dc.SetPen(pen1)
        dc.DrawRectangle(5, 5, 50, 50)

        dc.SetBrush(wxLIGHT_GREY_BRUSH)
        dc.SetPen(wxPen(wxNamedColour('BLUE'), 4))
        dc.DrawRectangle(15, 15, 50, 50)

        font = wxFont(14, wxSWISS, wxNORMAL, wxNORMAL)
        dc.SetFont(font)
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

        dc.DrawBitmap(self.bmp, 200, 20)
        dc.SetTextForeground(wxColour(0, 0xFF, 0x80))
        dc.DrawText("a bitmap", 200, 80)

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
        xView, yView = self.ViewStart()
        xDelta, yDelta = self.GetScrollPixelsPerUnit()
        return (event.GetX() + (xView * xDelta),
                event.GetY() + (yView * yDelta))

    def OnLeftButtonEvent(self, event):
        if event.LeftDown():
            self.SetXY(event)
            self.curLine = []

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

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyCanvas(nb)
    return win

#---------------------------------------------------------------------------













overview = """\
"""
