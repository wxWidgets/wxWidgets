"""

Part of the floatcanvas.Utilities package.

This module contains assorted GUI-related utilities that can be used
with FloatCanvas

So far, they are:

RubberBandBox: used to draw a RubberBand Box on the screen

"""
import wx
from floatcanvas import FloatCanvas

class RubberBandBox:
    """
    Class to provide a rubber band box that can be drawn on a Window

    """

    def __init__(self, Canvas, CallBack, Tol=5):

        """
        To initialize:
        
        RubberBandBox(Canvas, CallBack)

        Canvas:  the FloatCanvas you want the Rubber band box to be used on

        CallBack: is the method you want called when the mouse is
                  released. That method will be called, passing in a rect
                  parameter, where rect is: (Point, WH) of the rect in
                  world coords.

        Tol: The tolerance for the smallest rectangle allowed. defaults
             to 5. In pixels

        Methods:
        
        Enable() : Enables the Rubber Band Box (Binds the events)
        
        Disable() : Enables the Rubber Band Box (Unbinds the events)

        Attributes:

        CallBack: The callback function, if it's replaced you need to
                  call Enable() again.
                  
        """

        self.Canvas = Canvas
        self.CallBack = CallBack
        self.Tol = Tol
        
        self.Drawing = False
        self.RBRect = None
        self.StartPointWorld = None

        return None

    def Enable(self):
        """
        Called when you want the rubber band box to be enabled

        """

        # bind events:
        self.Canvas.Bind(FloatCanvas.EVT_MOTION, self.OnMove ) 
        self.Canvas.Bind(FloatCanvas.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Canvas.Bind(FloatCanvas.EVT_LEFT_UP, self.OnLeftUp ) 

    def Disable(self):
        """
        Called when you don't want the rubber band box to be enabled

        """

        # unbind events:
        self.Canvas.Unbind(FloatCanvas.EVT_MOTION)
        self.Canvas.Unbind(FloatCanvas.EVT_LEFT_DOWN)
        self.Canvas.Unbind(FloatCanvas.EVT_LEFT_UP)

    def OnMove(self, event):
        if self.Drawing:
            x, y = self.StartPoint
            Cornerx, Cornery = event.GetPosition()
            w, h = ( Cornerx - x, Cornery - y)
            if abs(w) > self.Tol and abs(h) > self.Tol:
                # draw the RB box
                dc = wx.ClientDC(self.Canvas)
                dc.SetPen(wx.Pen('WHITE', 2, wx.SHORT_DASH))
                dc.SetBrush(wx.TRANSPARENT_BRUSH)
                dc.SetLogicalFunction(wx.XOR)
                if self.RBRect:
                    dc.DrawRectangle(*self.RBRect)
                self.RBRect = (x, y, w, h )
                dc.DrawRectangle(*self.RBRect)
        event.Skip() # skip so that other events can catch these

    def OnLeftDown(self, event):
        # Start drawing
        self.Drawing = True
        self.StartPoint = event.GetPosition()
        self.StartPointWorld = event.Coords
    
    def OnLeftUp(self, event):
        # Stop Drawing
        if self.Drawing:
            self.Drawing = False
            if self.RBRect:
                WH = event.Coords - self.StartPointWorld
                wx.CallAfter(self.CallBack, (self.StartPointWorld, WH))
        self.RBRect = None
        self.StartPointWorld = None
