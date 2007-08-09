# doodle.py

"""
This module contains the DoodleWindow class which is a window that you
can do simple drawings upon.
"""


import wx                  # This module uses the new wx namespace

#----------------------------------------------------------------------

class DoodleWindow(wx.Window):
    menuColours = { 100 : 'Black',
                    101 : 'Yellow',
                    102 : 'Red',
                    103 : 'Green',
                    104 : 'Blue',
                    105 : 'Purple',
                    106 : 'Brown',
                    107 : 'Aquamarine',
                    108 : 'Forest Green',
                    109 : 'Light Blue',
                    110 : 'Goldenrod',
                    111 : 'Cyan',
                    112 : 'Orange',
                    113 : 'Navy',
                    114 : 'Dark Grey',
                    115 : 'Light Grey',
                    }
    maxThickness = 16


    def __init__(self, parent, ID):
        wx.Window.__init__(self, parent, ID, style=wx.NO_FULL_REPAINT_ON_RESIZE)
        self.SetBackgroundColour("WHITE")
        self.listeners = []
        self.thickness = 1
        self.SetColour("Black")
        self.lines = []
        self.pos = wx.Point(0,0)
        self.MakeMenu()

        self.InitBuffer()

        self.SetCursor(wx.StockCursor(wx.CURSOR_PENCIL))

        # hook some mouse events
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.Bind(wx.EVT_MOTION, self.OnMotion)

        # the window resize event and idle events for managing the buffer
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_IDLE, self.OnIdle)

        # and the refresh event
        self.Bind(wx.EVT_PAINT, self.OnPaint)

        # When the window is destroyed, clean up resources.
        self.Bind(wx.EVT_WINDOW_DESTROY, self.Cleanup)


    def Cleanup(self, evt):
        if hasattr(self, "menu"):
            self.menu.Destroy()
            del self.menu


    def InitBuffer(self):
        """Initialize the bitmap used for buffering the display."""
        size = self.GetClientSize()
        self.buffer = wx.EmptyBitmap(max(1,size.width), max(1,size.height))
        dc = wx.BufferedDC(None, self.buffer)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        self.DrawLines(dc)
        self.reInitBuffer = False


    def SetColour(self, colour):
        """Set a new colour and make a matching pen"""
        self.colour = colour
        self.pen = wx.Pen(self.colour, self.thickness, wx.SOLID)
        self.Notify()


    def SetThickness(self, num):
        """Set a new line thickness and make a matching pen"""
        self.thickness = num
        self.pen = wx.Pen(self.colour, self.thickness, wx.SOLID)
        self.Notify()


    def GetLinesData(self):
        return self.lines[:]


    def SetLinesData(self, lines):
        self.lines = lines[:]
        self.InitBuffer()
        self.Refresh()


    def MakeMenu(self):
        """Make a menu that can be popped up later"""
        menu = wx.Menu()
        keys = self.menuColours.keys()
        keys.sort()
        for k in keys:
            text = self.menuColours[k]
            menu.Append(k, text, kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU_RANGE, self.OnMenuSetColour, id=100, id2=200)
        self.Bind(wx.EVT_UPDATE_UI_RANGE, self.OnCheckMenuColours, id=100, id2=200)
        menu.Break()

        for x in range(1, self.maxThickness+1):
            menu.Append(x, str(x), kind=wx.ITEM_CHECK)

        self.Bind(wx.EVT_MENU_RANGE, self.OnMenuSetThickness, id=1, id2=self.maxThickness)
        self.Bind(wx.EVT_UPDATE_UI_RANGE, self.OnCheckMenuThickness, id=1, id2=self.maxThickness)
        self.menu = menu


    # These two event handlers are called before the menu is displayed
    # to determine which items should be checked.
    def OnCheckMenuColours(self, event):
        text = self.menuColours[event.GetId()]
        if text == self.colour:
            event.Check(True)
            event.SetText(text.upper())
        else:
            event.Check(False)
            event.SetText(text)

    def OnCheckMenuThickness(self, event):
        if event.GetId() == self.thickness:
            event.Check(True)
        else:
            event.Check(False)


    def OnLeftDown(self, event):
        """called when the left mouse button is pressed"""
        self.curLine = []
        self.pos = event.GetPosition()
        self.CaptureMouse()


    def OnLeftUp(self, event):
        """called when the left mouse button is released"""
        if self.HasCapture():
            self.lines.append( (self.colour, self.thickness, self.curLine) )
            self.curLine = []
            self.ReleaseMouse()


    def OnRightUp(self, event):
        """called when the right mouse button is released, will popup the menu"""
        pt = event.GetPosition()
        self.PopupMenu(self.menu, pt)



    def OnMotion(self, event):
        """
        Called when the mouse is in motion.  If the left button is
        dragging then draw a line from the last event position to the
        current one.  Save the coordinants for redraws.
        """
        if event.Dragging() and event.LeftIsDown():
            dc = wx.BufferedDC(wx.ClientDC(self), self.buffer)
            dc.BeginDrawing()
            dc.SetPen(self.pen)
            pos = event.GetPosition()
            coords = (self.pos.x, self.pos.y, pos.x, pos.y)
            self.curLine.append(coords)
            dc.DrawLine(*coords)
            self.pos = pos
            dc.EndDrawing()


    def OnSize(self, event):
        """
        Called when the window is resized.  We set a flag so the idle
        handler will resize the buffer.
        """
        self.reInitBuffer = True


    def OnIdle(self, event):
        """
        If the size was changed then resize the bitmap used for double
        buffering to match the window size.  We do it in Idle time so
        there is only one refresh after resizing is done, not lots while
        it is happening.
        """
        if self.reInitBuffer:
            self.InitBuffer()
            self.Refresh(False)


    def OnPaint(self, event):
        """
        Called when the window is exposed.
        """
        # Create a buffered paint DC.  It will create the real
        # wx.PaintDC and then blit the bitmap to it when dc is
        # deleted.  Since we don't need to draw anything else
        # here that's all there is to it.
        dc = wx.BufferedPaintDC(self, self.buffer)


    def DrawLines(self, dc):
        """
        Redraws all the lines that have been drawn already.
        """
        dc.BeginDrawing()
        for colour, thickness, line in self.lines:
            pen = wx.Pen(colour, thickness, wx.SOLID)
            dc.SetPen(pen)
            for coords in line:
                dc.DrawLine(*coords)
        dc.EndDrawing()


    # Event handlers for the popup menu, uses the event ID to determine
    # the colour or the thickness to set.
    def OnMenuSetColour(self, event):
        self.SetColour(self.menuColours[event.GetId()])

    def OnMenuSetThickness(self, event):
        self.SetThickness(event.GetId())


    # Observer pattern.  Listeners are registered and then notified
    # whenever doodle settings change.
    def AddListener(self, listener):
        self.listeners.append(listener)

    def Notify(self):
        for other in self.listeners:
            other.Update(self.colour, self.thickness)


#----------------------------------------------------------------------

class DoodleFrame(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, -1, "Doodle Frame", size=(800,600),
                         style=wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE)
        doodle = DoodleWindow(self, -1)

#----------------------------------------------------------------------

if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = DoodleFrame(None)
    frame.Show(True)
    app.MainLoop()

