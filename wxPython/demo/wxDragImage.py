
from wxPython.wx import *

#----------------------------------------------------------------------

class DragShape:
    def __init__(self, bmp):
        self.bmp = bmp
        self.pos = wxPoint(0,0)
        self.shown = true
        self.text = None
        self.fullscreen = false


    def HitTest(self, pt):
        rect = self.GetRect()
        return rect.Inside(pt.x, pt.y)


    def GetRect(self):
        return wxRect(self.pos.x, self.pos.y,
                      self.bmp.GetWidth(), self.bmp.GetHeight())


    def Draw(self, dc, op = wxCOPY):
        if self.bmp.Ok():
            memDC = wxMemoryDC()
            memDC.SelectObject(self.bmp)

            dc.Blit(self.pos.x, self.pos.y,
                    self.bmp.GetWidth(), self.bmp.GetHeight(),
                    memDC, 0, 0, op, true)

            return true
        else:
            return false



#----------------------------------------------------------------------

class DragCanvas(wxScrolledWindow):
    def __init__(self, parent, ID):
        wxScrolledWindow.__init__(self, parent, ID)
        self.shapes = []
        self.dragImage = None
        self.dragShape = None

        self.SetCursor(wxStockCursor(wxCURSOR_ARROW))
        self.bg_bmp = wxBitmap('bitmaps/backgrnd.png', wxBITMAP_TYPE_PNG)


        # Make a shape from an image and mask.  This one will demo
        # dragging outside the window
        bmp = wxBitmap('bitmaps/test_image.png', wxBITMAP_TYPE_PNG)
        mask = wxMaskColour(bmp, wxWHITE)
        bmp.SetMask(mask)
        shape = DragShape(bmp)
        shape.pos = wxPoint(5, 5)
        shape.fullscreen = true
        self.shapes.append(shape)


        # Make a shape from some text
        text = "Some Text"
        font = wxFont(15, wxROMAN, wxNORMAL, wxBOLD)
        textExtent = self.GetFullTextExtent(text, font)
        bmp = wxEmptyBitmap(textExtent[0], textExtent[1])
        dc = wxMemoryDC()
        dc.SelectObject(bmp)
        dc.Clear()
        dc.SetTextForeground(wxRED)
        dc.SetFont(font)
        dc.DrawText(text, 0, 0)
        dc.SelectObject(wxNullBitmap)
        del dc
        mask = wxMaskColour(bmp, wxWHITE)
        bmp.SetMask(mask)
        shape = DragShape(bmp)
        shape.pos = wxPoint(5, 100)
        shape.text = "Some dragging text"
        self.shapes.append(shape)


        # Make some shapes from some playing card images.
        x = 200
        for card in ['01c.gif', '10s.gif', '12h.gif', '13d.gif']:
            bmp = wxBitmap('bitmaps/'+card, wxBITMAP_TYPE_GIF)
            shape = DragShape(bmp)
            shape.pos = wxPoint(x, 5)
            self.shapes.append(shape)
            x = x + 80


        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
        EVT_PAINT(self, self.OnPaint)
        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_MOTION(self, self.OnMotion)



    def TileBackground(self, dc):
        # tile the background bitmap
        sz = self.GetClientSize()
        w = self.bg_bmp.GetWidth()
        h = self.bg_bmp.GetHeight()

        x = 0
        while x < sz.width:
            y = 0
            while y < sz.height:
                dc.DrawBitmap(self.bg_bmp, x, y)
                y = y + h
            x = x + w


    def DrawShapes(self, dc):
        for shape in self.shapes:
            if shape.shown:
                shape.Draw(dc)


    def FindShape(self, pt):
        for shape in self.shapes:
            if shape.HitTest(pt):
                return shape
        return None


    def EraseShape(self, shape, dc):
        r = shape.GetRect()
        dc.SetClippingRegion(r.x, r.y, r.width, r.height)
        self.TileBackground(dc)
        self.DrawShapes(dc)
        dc.DestroyClippingRegion()




    def OnEraseBackground(self, evt):
        dc = evt.GetDC()
        if not dc:
            dc = wxClientDC(self)
        self.TileBackground(dc)


    def OnPaint(self, evt):
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        self.DrawShapes(dc)


    def OnLeftDown(self, evt):
        shape = self.FindShape(evt.GetPosition())
        if shape:
            # get ready to start dragging, but wait for the user to
            # move it a bit first
            self.dragShape = shape
            self.dragStartPos = evt.GetPosition()


    def OnLeftUp(self, evt):
        if not self.dragImage or not self.dragShape:
            self.dragImage = None
            self.dragShape = None
            return

        # end the dragging
        self.dragImage.Hide()
        self.dragImage.EndDrag()
        self.dragImage = None

        # reposition and draw the shape
        pt = evt.GetPosition()
        newPos = wxPoint(self.dragShape.pos.x + (pt.x - self.dragStartPos.x),
                         self.dragShape.pos.y + (pt.y - self.dragStartPos.y))

        dc = wxClientDC(self)
        self.dragShape.pos = newPos
        self.dragShape.shown = true
        self.dragShape.Draw(dc)
        self.dragShape = None


    def OnMotion(self, evt):
        if not self.dragShape or not evt.Dragging() or not evt.LeftIsDown():
            return

        # if we have a shape, but havn't started dragging yet
        if self.dragShape and not self.dragImage:

            # only start the drag after having moved a couple pixels
            tolerance = 4
            pt = evt.GetPosition()
            dx = abs(pt.x - self.dragStartPos.x)
            dy = abs(pt.y - self.dragStartPos.y)
            if dx <= tolerance and dy <= tolerance:
                return

            if self.dragShape.text:
                self.dragImage = wxDragString(self.dragShape.text,
                                              wxStockCursor(wxCURSOR_HAND))
            else:
                self.dragImage = wxDragImage(self.dragShape.bmp,
                                             wxStockCursor(wxCURSOR_HAND))

            newPos = wxPoint(self.dragShape.pos.x + (pt.x - self.dragStartPos.x),
                             self.dragShape.pos.y + (pt.y - self.dragStartPos.y))

            if self.dragShape.fullscreen:
                newPos = self.ClientToScreen(newPos)
                self.dragImage.BeginDrag((0,0), self, true)
            else:
                self.dragImage.BeginDrag((0,0), self)


            # erase the shape since it will be drawn independently now
            dc = wxClientDC(self)
            self.dragShape.shown = false
            self.EraseShape(self.dragShape, dc)

            self.dragImage.Move(newPos)
            self.dragImage.Show()


        # if we have shape and image then move it.
        elif self.dragShape and self.dragImage:
            pt = evt.GetPosition()
            newPos = wxPoint(self.dragShape.pos.x + (pt.x - self.dragStartPos.x),
                             self.dragShape.pos.y + (pt.y - self.dragStartPos.y))
            if self.dragShape.fullscreen:
                newPos = self.ClientToScreen(newPos)

            self.dragImage.Move(newPos)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = DragCanvas(nb, -1)
    return win

#----------------------------------------------------------------------



overview = """\
"""
