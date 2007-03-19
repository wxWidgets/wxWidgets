
import  wx
import  images

#----------------------------------------------------------------------

class DragShape:
    def __init__(self, bmp):
        self.bmp = bmp
        self.pos = (0,0)
        self.shown = True
        self.text = None
        self.fullscreen = False

    def HitTest(self, pt):
        rect = self.GetRect()
        return rect.InsideXY(pt.x, pt.y)

    def GetRect(self):
        return wx.Rect(self.pos[0], self.pos[1],
                      self.bmp.GetWidth(), self.bmp.GetHeight())

    def Draw(self, dc, op = wx.COPY):
        if self.bmp.Ok():
            memDC = wx.MemoryDC()
            memDC.SelectObject(self.bmp)

            dc.Blit(self.pos[0], self.pos[1],
                    self.bmp.GetWidth(), self.bmp.GetHeight(),
                    memDC, 0, 0, op, True)

            return True
        else:
            return False



#----------------------------------------------------------------------

class DragCanvas(wx.ScrolledWindow):
    def __init__(self, parent, ID):
        wx.ScrolledWindow.__init__(self, parent, ID)
        self.shapes = []
        self.dragImage = None
        self.dragShape = None
        self.hiliteShape = None

        self.SetCursor(wx.StockCursor(wx.CURSOR_ARROW))
        self.bg_bmp = images.getBackgroundBitmap()
        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)

        # Make a shape from an image and mask.  This one will demo
        # dragging outside the window
        bmp = images.getTestStarBitmap()
        #bmp = wx.Bitmap('bitmaps/toucan.png')
        shape = DragShape(bmp)
        shape.pos = (5, 5)
        shape.fullscreen = True
        self.shapes.append(shape)

        bmp = images.getTheKidBitmap()
        shape = DragShape(bmp)
        shape.pos = (200, 5)
        self.shapes.append(shape)

        # Make a shape from some text
        text = "Some Text"
        bg_colour = wx.Colour(57, 115, 57)  # matches the bg image
        font = wx.Font(15, wx.ROMAN, wx.NORMAL, wx.BOLD)
        textExtent = self.GetFullTextExtent(text, font)

        # create a bitmap the same size as our text
        bmp = wx.EmptyBitmap(textExtent[0], textExtent[1])

        # 'draw' the text onto the bitmap
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wx.Brush(bg_colour, wx.SOLID))
        dc.Clear()
        dc.SetTextForeground(wx.RED)
        dc.SetFont(font)
        dc.DrawText(text, 0, 0)
        dc.SelectObject(wx.NullBitmap)
        mask = wx.Mask(bmp, bg_colour)
        bmp.SetMask(mask)
        shape = DragShape(bmp)
        shape.pos = (5, 100)
        shape.text = "Some dragging text"
        self.shapes.append(shape)


        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_MOTION, self.OnMotion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.OnLeaveWindow)

    
    # We're not doing anything here, but you might have reason to.
    # for example, if you were dragging something, you might elect to
    # 'drop it' when the cursor left the window.
    def OnLeaveWindow(self, evt):
        pass


    # tile the background bitmap
    def TileBackground(self, dc):
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


    # Go through our list of shapes and draw them in whatever place they are.
    def DrawShapes(self, dc):
        for shape in self.shapes:
            if shape.shown:
                shape.Draw(dc)

    # This is actually a sophisticated 'hit test', but in this
    # case we're also determining which shape, if any, was 'hit'.
    def FindShape(self, pt):
        for shape in self.shapes:
            if shape.HitTest(pt):
                return shape
        return None

    # Remove a shape from the display
    def EraseShape(self, shape, dc):
        r = shape.GetRect()
        dc.SetClippingRect(r)
        self.TileBackground(dc)
        self.DrawShapes(dc)
        dc.DestroyClippingRegion()

    # Clears the background, then redraws it. If the DC is passed, then
    # we only do so in the area so designated. Otherwise, it's the whole thing.
    def OnEraseBackground(self, evt):
        dc = evt.GetDC()

        if not dc:
            dc = wx.ClientDC(self)
            rect = self.GetUpdateRegion().GetBox()
            dc.SetClippingRect(rect)
        self.TileBackground(dc)

    # Fired whenever a paint event occurs
    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        self.PrepareDC(dc)
        self.DrawShapes(dc)

    # Left mouse button is down.
    def OnLeftDown(self, evt):
        # Did the mouse go down on one of our shapes?
        shape = self.FindShape(evt.GetPosition())

        # If a shape was 'hit', then set that as the shape we're going to
        # drag around. Get our start position. Dragging has not yet started.
        # That will happen once the mouse moves, OR the mouse is released.
        if shape:
            self.dragShape = shape
            self.dragStartPos = evt.GetPosition()

    # Left mouse button up.
    def OnLeftUp(self, evt):
        if not self.dragImage or not self.dragShape:
            self.dragImage = None
            self.dragShape = None
            return

        # Hide the image, end dragging, and nuke out the drag image.
        self.dragImage.Hide()
        self.dragImage.EndDrag()
        self.dragImage = None

        if self.hiliteShape:
            self.RefreshRect(self.hiliteShape.GetRect())
            self.hiliteShape = None

        # reposition and draw the shape

        # Note by jmg 11/28/03 
        # Here's the original:
        #
        # self.dragShape.pos = self.dragShape.pos + evt.GetPosition() - self.dragStartPos
        #
        # So if there are any problems associated with this, use that as
        # a starting place in your investigation. I've tried to simulate the
        # wx.Point __add__ method here -- it won't work for tuples as we
        # have now from the various methods
        #
        # There must be a better way to do this :-)
        #
        
        self.dragShape.pos = (
            self.dragShape.pos[0] + evt.GetPosition()[0] - self.dragStartPos[0],
            self.dragShape.pos[1] + evt.GetPosition()[1] - self.dragStartPos[1]
            )
            
        self.dragShape.shown = True
        self.RefreshRect(self.dragShape.GetRect())
        self.dragShape = None


    # The mouse is moving
    def OnMotion(self, evt):
        # Ignore mouse movement if we're not dragging.
        if not self.dragShape or not evt.Dragging() or not evt.LeftIsDown():
            return

        # if we have a shape, but haven't started dragging yet
        if self.dragShape and not self.dragImage:

            # only start the drag after having moved a couple pixels
            tolerance = 2
            pt = evt.GetPosition()
            dx = abs(pt.x - self.dragStartPos.x)
            dy = abs(pt.y - self.dragStartPos.y)
            if dx <= tolerance and dy <= tolerance:
                return

            # erase the shape since it will be drawn independently now
            dc = wx.ClientDC(self)
            self.dragShape.shown = False
            self.EraseShape(self.dragShape, dc)


            if self.dragShape.text:
                self.dragImage = wx.DragString(self.dragShape.text,
                                              wx.StockCursor(wx.CURSOR_HAND))
            else:
                self.dragImage = wx.DragImage(self.dragShape.bmp,
                                             wx.StockCursor(wx.CURSOR_HAND))

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
                dc = wx.ClientDC(self)
                self.hiliteShape.Draw(dc)
                self.hiliteShape = None

            if hiliteNew:
                dc = wx.ClientDC(self)
                self.hiliteShape = onShape
                self.hiliteShape.Draw(dc, wx.INVERT)

            # now move it and show it again if needed
            self.dragImage.Move(evt.GetPosition())
            if unhiliteOld or hiliteNew:
                self.dragImage.Show()


#----------------------------------------------------------------------

def runTest(frame, nb, log):

    win = wx.Panel(nb, -1)
    canvas = DragCanvas(win, -1)

    def onSize(evt, panel=win, canvas=canvas): 
        canvas.SetSize(panel.GetSize())

    win.Bind(wx.EVT_SIZE, onSize)
    return win

#----------------------------------------------------------------------



overview = """\
DragImage is used when you wish to drag an object on the screen, and a simple 
cursor is not enough.

On Windows, the WIN32 API is used to do achieve smooth dragging. On other 
platforms, <code>GenericDragImage</code> is used. Applications may also prefer to use 
<code>GenericDragImage</code> on Windows, too.

<b>wxPython note</b>: wxPython uses <code>GenericDragImage</code> on all 
platforms, but uses the <code>DragImage</code> name.

To use this class, when you wish to start dragging an image, create a 
<code>DragImage</code> object and store it somewhere you can access it as the 
drag progresses. Call BeginDrag to start, and EndDrag to stop the drag. To move 
the image, initially call Show and then Move. If you wish to update the screen 
contents during the drag (for example, highlight an item as in the example), first 
call Hide, update the screen, call Move, and then call Show.

You can drag within one window, or you can use full-screen dragging either across 
the whole screen, or just restricted to one area of the screen to save resources. 
If you want the user to drag between two windows, then you will need to use 
full-screen dragging.
 
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

