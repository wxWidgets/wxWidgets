
from wxPython.wx import *

import images

#----------------------------------------------------------------------

class DragShape:
    def __init__(self, bmp):
        self.bmp = bmp
        self.pos = wxPoint(0,0)
        self.shown = True
        self.text = None
        self.fullscreen = False


    def HitTest(self, pt):
        rect = self.GetRect()
        return rect.InsideXY(pt.x, pt.y)


    def GetRect(self):
        return wxRect(self.pos.x, self.pos.y,
                      self.bmp.GetWidth(), self.bmp.GetHeight())


    def Draw(self, dc, op = wxCOPY):
        if self.bmp.Ok():
            memDC = wxMemoryDC()
            memDC.SelectObject(self.bmp)

            dc.Blit(self.pos.x, self.pos.y,
                    self.bmp.GetWidth(), self.bmp.GetHeight(),
                    memDC, 0, 0, op, True)

            return True
        else:
            return False



#----------------------------------------------------------------------

class DragCanvas(wxScrolledWindow):
    def __init__(self, parent, ID):
        wxScrolledWindow.__init__(self, parent, ID)
        self.shapes = []
        self.dragImage = None
        self.dragShape = None
        self.hiliteShape = None

        self.SetCursor(wxStockCursor(wxCURSOR_ARROW))
        self.bg_bmp = images.getBackgroundBitmap()


        # Make a shape from an image and mask.  This one will demo
        # dragging outside the window
        bmp = images.getTestStarBitmap()
        shape = DragShape(bmp)
        shape.pos = wxPoint(5, 5)
        shape.fullscreen = True
        self.shapes.append(shape)


        # Make a shape from some text
        text = "Some Text"
        bg_colour = wxColour(57, 115, 57)  # matches the bg image
        font = wxFont(15, wxROMAN, wxNORMAL, wxBOLD)
        textExtent = self.GetFullTextExtent(text, font)
        bmp = wxEmptyBitmap(textExtent[0], textExtent[1])
        dc = wxMemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wxBrush(bg_colour, wxSOLID))
        dc.Clear()
        dc.SetTextForeground(wxRED)
        dc.SetFont(font)
        dc.DrawText(text, 0, 0)
        dc.SelectObject(wxNullBitmap)
        mask = wxMaskColour(bmp, bg_colour)
        bmp.SetMask(mask)
        shape = DragShape(bmp)
        shape.pos = wxPoint(5, 100)
        shape.text = "Some dragging text"
        self.shapes.append(shape)


        # Make some shapes from some playing card images.
        x = 200
        for card in ['_01c_', '_12h_', '_13d_', '_10s_']:
            bmpFunc = getattr(images, "get%sBitmap" % card)
            bmp = bmpFunc()
            shape = DragShape(bmp)
            shape.pos = wxPoint(x, 5)
            self.shapes.append(shape)
            x = x + 80


        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
        EVT_PAINT(self, self.OnPaint)
        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_MOTION(self, self.OnMotion)
        EVT_LEAVE_WINDOW(self, self.OnLeaveWindow)


    def OnLeaveWindow(self, evt):
        pass


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
            rect = self.GetUpdateRegion().GetBox()
            dc.SetClippingRegion(rect.x, rect.y, rect.width, rect.height)
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

        dc = wxClientDC(self)
        if self.hiliteShape:
            self.hiliteShape.Draw(dc)
            self.hiliteShape = None

        # reposition and draw the shape
        self.dragShape.pos = self.dragShape.pos + evt.GetPosition() - self.dragStartPos
        self.dragShape.shown = True
        self.dragShape.Draw(dc)
        self.dragShape = None


    def OnMotion(self, evt):
        if not self.dragShape or not evt.Dragging() or not evt.LeftIsDown():
            return

        # if we have a shape, but havn't started dragging yet
        if self.dragShape and not self.dragImage:

            # only start the drag after having moved a couple pixels
            tolerance = 2
            pt = evt.GetPosition()
            dx = abs(pt.x - self.dragStartPos.x)
            dy = abs(pt.y - self.dragStartPos.y)
            if dx <= tolerance and dy <= tolerance:
                return

            # erase the shape since it will be drawn independently now
            dc = wxClientDC(self)
            self.dragShape.shown = False
            self.EraseShape(self.dragShape, dc)


            if self.dragShape.text:
                self.dragImage = wxDragString(self.dragShape.text,
                                              wxStockCursor(wxCURSOR_HAND))
            else:
                self.dragImage = wxDragImage(self.dragShape.bmp,
                                             wxStockCursor(wxCURSOR_HAND))

            hotspot = self.dragStartPos - self.dragShape.pos
            self.dragImage.BeginDrag(hotspot, self, self.dragShape.fullscreen)

            self.dragImage.Move(pt)
            self.dragImage.Show()


        # if we have shape and image then move it, posibly highlighting another shape.
        elif self.dragShape and self.dragImage:
            onShape = self.FindShape(evt.GetPosition())
            unhiliteOld = False
            hiliteNew = False

            # figure out what to hilite and what to unhilite
            if self.hiliteShape:
                if onShape is None or self.hiliteShape is not onShape:
                    unhiliteOld = True

            if onShape and onShape is not self.hiliteShape and onShape.shown:
                hiliteNew = True

            # if needed, hide the drag image so we can update the window
            if unhiliteOld or hiliteNew:
                self.dragImage.Hide()

            if unhiliteOld:
                dc = wxClientDC(self)
                self.hiliteShape.Draw(dc)
                self.hiliteShape = None

            if hiliteNew:
                dc = wxClientDC(self)
                self.hiliteShape = onShape
                self.hiliteShape.Draw(dc, wxINVERT)

            # now move it and show it again if needed
            self.dragImage.Move(evt.GetPosition())
            if unhiliteOld or hiliteNew:
                self.dragImage.Show()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = wxPanel(nb, -1)
    canvas = DragCanvas(win, -1)
    def onSize(evt, panel=win, canvas=canvas): canvas.SetSize(panel.GetSize())
    EVT_SIZE(win, onSize)
    return win

#----------------------------------------------------------------------



overview = """\
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

