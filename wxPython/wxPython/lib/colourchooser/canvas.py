"""
wxPyColourChooser
Copyright (C) 2002 Michael Gilfix <mgilfix@eecs.tufts.edu>

This file is part of wxPyColourChooser.

This version of wxPyColourChooser is open source; you can redistribute it
and/or modify it under the licensed terms.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
"""

from wxPython.wx import *

class BitmapBuffer(wxMemoryDC):
    """A screen buffer class.

    This class implements a screen output buffer. Data is meant to
    be drawn in the buffer class and then blitted directly to the
    output device, or on-screen window.
    """
    def __init__(self, width, height, colour):
        """Initialize the empty buffer object."""
        wxMemoryDC.__init__(self)

        self.width = width
        self.height = height
        self.colour = colour

        self.bitmap = wxEmptyBitmap(self.width, self.height)
        self.SelectObject(self.bitmap)

        # Initialize the buffer to the background colour
        self.SetBackground(wxBrush(self.colour, wxSOLID))
        self.Clear()

        # Make each logical unit of the buffer equal to 1 pixel
        self.SetMapMode(wxMM_TEXT)

    def GetBitmap(self):
        """Returns the internal bitmap for direct drawing."""
        return self.bitmap

class Canvas(wxWindow):
    """A canvas class for arbitrary drawing.

    The Canvas class implements a window that allows for drawing
    arbitrary graphics. It implements a double buffer scheme and
    blits the off-screen buffer to the window during paint calls
    by the windowing system for speed.

    Some other methods for determining the canvas colour and size
    are also provided.
    """
    def __init__(self, parent, id,
                 pos=wxDefaultPosition,
                 size=wxDefaultSize,
                 style=wxSIMPLE_BORDER):
        """Creates a canvas instance and initializes the off-screen
        buffer. Also sets the handler for rendering the canvas
        automatically via size and paint calls from the windowing
        system."""
        wxWindow.__init__(self, parent, id, pos, size, style)

        # Perform an intial sizing
        self.ReDraw()

        # Register event handlers
        EVT_SIZE(self, self.onSize)
        EVT_PAINT(self, self.onPaint)

    def MakeNewBuffer(self):
        size = self.GetSizeTuple()
        self.buffer = BitmapBuffer(size[0], size[1],
                                   self.GetBackgroundColour())

    def onSize(self, event):
        """Perform actual redraw to off-screen buffer only when the
        size of the canvas has changed. This saves a lot of computation
        since the same image can be re-used, provided the canvas size
        hasn't changed."""
        self.MakeNewBuffer()
        self.DrawBuffer()
        self.Refresh()

    def ReDraw(self):
        """Explicitly tells the canvas to redraw it's contents."""
        self.onSize(None)

    def Refresh(self):
        """Re-draws the buffer contents on-screen."""
        dc = wxClientDC(self)
        self.Blit(dc)

    def onPaint(self, event):
        """Renders the off-screen buffer on-screen."""
        dc = wxPaintDC(self)
        self.Blit(dc)

    def Blit(self, dc):
        """Performs the blit of the buffer contents on-screen."""
        width, height = self.buffer.GetSize()
        dc.BeginDrawing()
        dc.Blit(0, 0, width, height, self.buffer, 0, 0)
        dc.EndDrawing()

    def GetBoundingRect(self):
        """Returns a tuple that contains the co-ordinates of the
        top-left and bottom-right corners of the canvas."""
        x, y = self.GetPositionTuple()
        w, h = self.GetSize()
        return(x, y + h, x + w, y)

    def DrawBuffer(self):
        """Actual drawing function for drawing into the off-screen
        buffer. To be overrideen in the implementing class. Do nothing
        by default."""
        pass
