#---------------------------------------------------------------------------
# Name:        wxPython.lib.mixins.rubberband
# Purpose:     A mixin class for doing "RubberBand"-ing on a window.
#
# Author:      Robb Shecter and members of wxPython-users
#
# Created:     11-September-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by db-X Corporation
# Licence:     wxWindows license
#---------------------------------------------------------------------------
# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Tested, but there is an anomaly between first use and subsequent uses.
#   First use is odd, subsequent uses seem to be OK. Init error?
#   -- No, the first time it uses an aspect ratio, but after the reset it doesn't.
#

"""
A mixin class for doing "RubberBand"-ing on a window.
"""

import  wx

#
# Some miscellaneous mathematical and geometrical functions
#

def isNegative(aNumber):
    """
    x < 0:   1
    else:    0
    """
    return aNumber < 0


def normalizeBox(box):
    """
    Convert any negative measurements in the current
    box to positive, and adjust the origin.
    """
    x, y, w, h = box
    if w < 0:
        x += (w+1)
        w *= -1
    if h < 0:
        y += (h+1)
        h *= -1
    return (x, y, w, h)


def boxToExtent(box):
    """
    Convert a box specification to an extent specification.
    I put this into a seperate function after I realized that
    I had been implementing it wrong in several places.
    """
    b = normalizeBox(box)
    return (b[0], b[1], b[0]+b[2]-1, b[1]+b[3]-1)


def pointInBox(x, y, box):
    """
    Return True if the given point is contained in the box.
    """
    e = boxToExtent(box)
    return x >= e[0] and x <= e[2] and y >= e[1] and y <= e[3]


def pointOnBox(x, y, box, thickness=1):
    """
    Return True if the point is on the outside edge
    of the box.  The thickness defines how thick the
    edge should be.  This is necessary for HCI reasons:
    For example, it's normally very difficult for a user
    to manuever the mouse onto a one pixel border.
    """
    outerBox = box
    innerBox = (box[0]+thickness, box[1]+thickness, box[2]-(thickness*2), box[3]-(thickness*2))
    return pointInBox(x, y, outerBox) and not pointInBox(x, y, innerBox)


def getCursorPosition(x, y, box, thickness=1):
    """
    Return a position number in the range 0 .. 7 to indicate
    where on the box border the point is.  The layout is:

              0    1    2
              7         3
              6    5    4
    """
    x0, y0, x1, y1 = boxToExtent(box)
    w, h  = box[2], box[3]
    delta = thickness - 1
    p     = None

    if pointInBox(x, y, (x0, y0, thickness, thickness)):
        p = 0
    elif pointInBox(x, y, (x1-delta, y0, thickness, thickness)):
        p = 2
    elif pointInBox(x, y, (x1-delta, y1-delta, thickness, thickness)):
        p = 4
    elif pointInBox(x, y, (x0, y1-delta, thickness, thickness)):
        p = 6
    elif pointInBox(x, y, (x0+thickness, y0, w-(thickness*2), thickness)):
        p = 1
    elif pointInBox(x, y, (x1-delta, y0+thickness, thickness, h-(thickness*2))):
        p = 3
    elif pointInBox(x, y, (x0+thickness, y1-delta, w-(thickness*2), thickness)):
        p = 5
    elif pointInBox(x, y, (x0, y0+thickness, thickness, h-(thickness*2))):
        p = 7

    return p




class RubberBand:
    """
    A stretchable border which is drawn on top of an
    image to define an area.
    """
    def __init__(self, drawingSurface, aspectRatio=None):
        self.__THICKNESS     = 5
        self.drawingSurface  = drawingSurface
        self.aspectRatio     = aspectRatio
        self.hasLetUp        = 0
        self.currentlyMoving = None
        self.currentBox      = None
        self.__enabled       = 1
        self.__currentCursor = None

        drawingSurface.Bind(wx.EVT_MOUSE_EVENTS, self.__handleMouseEvents)
        drawingSurface.Bind(wx.EVT_PAINT, self.__handleOnPaint)

    def __setEnabled(self, enabled):
        self.__enabled = enabled

    def __isEnabled(self):
        return self.__enabled

    def __handleOnPaint(self, event):
        #print 'paint'
        event.Skip()

    def __isMovingCursor(self):
        """
        Return True if the current cursor is one used to
        mean moving the rubberband.
        """
        return self.__currentCursor == wx.CURSOR_HAND

    def __isSizingCursor(self):
        """
        Return True if the current cursor is one of the ones
        I may use to signify sizing.
        """
        sizingCursors = [wx.CURSOR_SIZENESW,
                         wx.CURSOR_SIZENS,
                         wx.CURSOR_SIZENWSE,
                         wx.CURSOR_SIZEWE,
                         wx.CURSOR_SIZING,
                         wx.CURSOR_CROSS]
        try:
            sizingCursors.index(self.__currentCursor)
            return 1
        except ValueError:
            return 0


    def __handleMouseEvents(self, event):
        """
        React according to the new event.  This is the main
        entry point into the class.  This method contains the
        logic for the class's behavior.
        """
        if not self.enabled:
            return

        x, y = event.GetPosition()

        # First make sure we have started a box.
        if self.currentBox == None and not event.LeftDown():
            # No box started yet.  Set cursor to the initial kind.
            self.__setCursor(wx.CURSOR_CROSS)
            return

        if event.LeftDown():
            if self.currentBox == None:
                # No RB Box, so start a new one.
                self.currentBox = (x, y, 0, 0)
                self.hasLetUp   = 0
            elif self.__isSizingCursor():
                # Starting a sizing operation.  Change the origin.
                position = getCursorPosition(x, y, self.currentBox, thickness=self.__THICKNESS)
                self.currentBox = self.__denormalizeBox(position, self.currentBox)

        elif event.Dragging() and event.LeftIsDown():
            # Use the cursor type to determine operation
            if self.__isMovingCursor():
                if self.currentlyMoving or pointInBox(x, y, self.currentBox):
                    if not self.currentlyMoving:
                        self.currentlyMoving = (x - self.currentBox[0], y - self.currentBox[1])
                    self.__moveTo(x - self.currentlyMoving[0], y - self.currentlyMoving[1])
            elif self.__isSizingCursor():
                self.__resizeBox(x, y)

        elif event.LeftUp():
            self.hasLetUp = 1
            self.currentlyMoving = None
            self.__normalizeBox()

        elif event.Moving() and not event.Dragging():
            # Simple mouse movement event
            self.__mouseMoved(x,y)

    def __denormalizeBox(self, position, box):
        x, y, w, h = box
        b = box
        if position == 2 or position == 3:
            b = (x, y + (h-1), w, h * -1)
        elif position == 0 or position == 1 or position == 7:
            b = (x + (w-1), y + (h-1), w * -1, h * -1)
        elif position == 6:
            b = (x + (w-1), y, w * -1, h)
        return b

    def __resizeBox(self, x, y):
        """
        Resize and repaint the box based on the given mouse
        coordinates.
        """
        # Implement the correct behavior for dragging a side
        # of the box:  Only change one dimension.
        if not self.aspectRatio:
            if self.__currentCursor == wx.CURSOR_SIZENS:
                x = None
            elif self.__currentCursor == wx.CURSOR_SIZEWE:
                y = None

        x0,y0,w0,h0 = self.currentBox
        currentExtent = boxToExtent(self.currentBox)
        if x == None:
            if w0 < 1:
                w0 += 1
            else:
                w0 -= 1
            x = x0 + w0
        if y == None:
            if h0 < 1:
                h0 += 1
            else:
                h0 -= 1
            y = y0 + h0
        x1,y1 = x, y
        w, h = abs(x1-x0)+1, abs(y1-y0)+1
        if self.aspectRatio:
            w = max(w, int(h * self.aspectRatio))
            h = int(w / self.aspectRatio)
        w *= [1,-1][isNegative(x1-x0)]
        h *= [1,-1][isNegative(y1-y0)]
        newbox = (x0, y0, w, h)
        self.__drawAndErase(boxToDraw=normalizeBox(newbox), boxToErase=normalizeBox(self.currentBox))
        self.currentBox = (x0, y0, w, h)

    def __normalizeBox(self):
        """
        Convert any negative measurements in the current
        box to positive, and adjust the origin.
        """
        self.currentBox = normalizeBox(self.currentBox)

    def __mouseMoved(self, x, y):
        """
        Called when the mouse moved without any buttons pressed
        or dragging being done.
        """
        # Are we on the bounding box?
        if pointOnBox(x, y, self.currentBox, thickness=self.__THICKNESS):
            position = getCursorPosition(x, y, self.currentBox, thickness=self.__THICKNESS)
            cursor   = [
                wx.CURSOR_SIZENWSE,
                wx.CURSOR_SIZENS,
                wx.CURSOR_SIZENESW,
                wx.CURSOR_SIZEWE,
                wx.CURSOR_SIZENWSE,
                wx.CURSOR_SIZENS,
                wx.CURSOR_SIZENESW,
                wx.CURSOR_SIZEWE
                ] [position]
            self.__setCursor(cursor)
        elif pointInBox(x, y, self.currentBox):
            self.__setCursor(wx.CURSOR_HAND)
        else:
            self.__setCursor()

    def __setCursor(self, id=None):
        """
        Set the mouse cursor to the given id.
        """
        if self.__currentCursor != id:  # Avoid redundant calls
            if id:
                self.drawingSurface.SetCursor(wx.StockCursor(id))
            else:
                self.drawingSurface.SetCursor(wx.NullCursor)
            self.__currentCursor = id

    def __moveCenterTo(self, x, y):
        """
        Move the rubber band so that its center is at (x,y).
        """
        x0, y0, w, h = self.currentBox
        x2, y2 = x - (w/2), y - (h/2)
        self.__moveTo(x2, y2)

    def __moveTo(self, x, y):
        """
        Move the rubber band so that its origin is at (x,y).
        """
        newbox = (x, y, self.currentBox[2], self.currentBox[3])
        self.__drawAndErase(boxToDraw=newbox, boxToErase=self.currentBox)
        self.currentBox = newbox

    def __drawAndErase(self, boxToDraw, boxToErase=None):
        """
        Draw one box shape and possibly erase another.
        """
        dc = wx.ClientDC(self.drawingSurface)
        dc.BeginDrawing()
        dc.SetPen(wx.Pen(wx.WHITE, 1, wx.DOT))
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.SetLogicalFunction(wx.XOR)
        if boxToErase:
            r = wx.Rect(*boxToErase)
            dc.DrawRectangleRect(r)

        r = wx.Rect(*boxToDraw)
        dc.DrawRectangleRect(r)
        dc.EndDrawing()

    def __dumpMouseEvent(self, event):
        print 'Moving:          ',event.Moving()
        print 'Dragging:        ',event.Dragging()
        print 'LeftDown:        ',event.LeftDown()
        print 'LeftisDown:      ',event.LeftIsDown()
        print 'LeftUp:          ',event.LeftUp()
        print 'Position:        ',event.GetPosition()
        print 'x,y:             ',event.GetX(),event.GetY()
        print


    #
    # The public API:
    #

    def reset(self, aspectRatio=None):
        """
        Clear the existing rubberband
        """
        self.currentBox   = None
        self.aspectRatio  = aspectRatio
        self.drawingSurface.Refresh()

    def getCurrentExtent(self):
        """
        Return (x0, y0, x1, y1) or None if
        no drawing has yet been done.
        """
        if not self.currentBox:
            extent = None
        else:
            extent = boxToExtent(self.currentBox)
        return extent

    enabled = property(__isEnabled, __setEnabled, None, 'True if I am responding to mouse events')



if __name__ == '__main__':
    app   = wx.PySimpleApp()
    frame = wx.Frame(None, -1, title='RubberBand Test', size=(300,300))

    # Add a panel that the rubberband will work on.
    panel = wx.Panel(frame, -1)
    panel.SetBackgroundColour(wx.BLUE)

    # Create the rubberband
    frame.rubberBand = RubberBand(drawingSurface=panel)
    frame.rubberBand.reset(aspectRatio=0.5)

    # Add a button that creates a new rubberband
    def __newRubberBand(event):
        frame.rubberBand.reset()
    button = wx.Button(frame, 100, 'Reset Rubberband')
    frame.Bind(wx.EVT_BUTTON, __newRubberBand, button)

    # Layout the frame
    sizer = wx.BoxSizer(wx.VERTICAL)
    sizer.Add(panel,  1, wx.EXPAND | wx.ALL, 5)
    sizer.Add(button, 0, wx.ALIGN_CENTER | wx.ALL, 5)
    frame.SetAutoLayout(1)
    frame.SetSizer(sizer)
    frame.Show(1)
    app.MainLoop()
