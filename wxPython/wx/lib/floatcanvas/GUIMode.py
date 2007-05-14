"""

Module that holds the GUI modes used by FloatCanvas


Note that this can only be imported after a wx.App() has been created.

This approach was inpired by Christian Blouin, who also wrote the initial
version of the code.

"""

import wx
## fixme: events should live in their own module, so all of FloatCanvas
##        wouldn't have to be imported here.
import FloatCanvas, Resources
import numpy as N

## create all the Cursors, so they don't need to be created each time.
if "wxMac" in wx.PlatformInfo: # use 16X16 cursors for wxMac
    HandCursor = wx.CursorFromImage(Resources.getHand16Image())
    GrabHandCursor = wx.CursorFromImage(Resources.getGrabHand16Image())

    img = Resources.getMagPlus16Image()
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_X, 6)
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_Y, 6)
    MagPlusCursor = wx.CursorFromImage(img)

    img = Resources.getMagMinus16Image()
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_X, 6)
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_Y, 6)
    MagMinusCursor = wx.CursorFromImage(img)
else: # use 24X24 cursors for GTK and Windows
    HandCursor = wx.CursorFromImage(Resources.getHandImage())
    GrabHandCursor = wx.CursorFromImage(Resources.getGrabHandImage())

    img = Resources.getMagPlusImage()
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_X, 9)
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_Y, 9)
    MagPlusCursor = wx.CursorFromImage(img)

    img = Resources.getMagMinusImage()
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_X, 9)
    img.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_Y, 9)
    MagMinusCursor = wx.CursorFromImage(img)


class GUIBase:
    """
    Basic Mouse mode and baseclass for other GUImode.

    This one does nothing with any event

    """
    def __init__(self, parent):
        self.parent = parent
    
    Cursor = wx.NullCursor

    # Handlers
    def OnLeftDown(self, event):
        pass
    def OnLeftUp(self, event):
        pass
    def OnLeftDouble(self, event):
        pass
    def OnRightDown(self, event):
        pass
    def OnRightUp(self, event):
        pass
    def OnRightDouble(self, event):
        pass
    def OnMiddleDown(self, event):
        pass
    def OnMiddleUp(self, event):
        pass
    def OnMiddleDouble(self, event):
        pass
    def OnWheel(self, event):
        pass
    def OnMove(self, event):
        pass
            
    def UpdateScreen(self):
        """
        Update gets called if the screen has been repainted in the middle of a zoom in
        so the Rubber Band Box can get updated
        """
        pass

class GUIMouse(GUIBase):
    """

    Mouse mode checks for a hit test, and if nothing is hit,
    raises a FloatCanvas mouse event for each event.

    """

    Cursor = wx.NullCursor

    # Handlers
    def OnLeftDown(self, event):
        EventType = FloatCanvas.EVT_FC_LEFT_DOWN
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnLeftUp(self, event):
        EventType = FloatCanvas.EVT_FC_LEFT_UP
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnLeftDouble(self, event):
        EventType = FloatCanvas.EVT_FC_LEFT_DCLICK
        if not self.parent.HitTest(event, EventType):
                self.parent._RaiseMouseEvent(event, EventType)

    def OnMiddleDown(self, event):
        EventType = FloatCanvas.EVT_FC_MIDDLE_DOWN
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnMiddleUp(self, event):
        EventType = FloatCanvas.EVT_FC_MIDDLE_UP
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnMiddleDouble(self, event):
        EventType = FloatCanvas.EVT_FC_MIDDLE_DCLICK
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnRightDown(self, event):
        EventType = FloatCanvas.EVT_FC_RIGHT_DOWN
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnRightUp(self, event):
        EventType = FloatCanvas.EVT_FC_RIGHT_UP
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnRightDouble(self, event):
        EventType = FloatCanvas.EVT_FC_RIGHT_DCLICK
        if not self.parent.HitTest(event, EventType):
            self.parent._RaiseMouseEvent(event, EventType)

    def OnWheel(self, event):
        EventType = FloatCanvas.EVT_FC_MOUSEWHEEL
        self.parent._RaiseMouseEvent(event, EventType)

    def OnMove(self, event):
        ## The Move event always gets raised, even if there is a hit-test
        self.parent.MouseOverTest(event)
        self.parent._RaiseMouseEvent(event,FloatCanvas.EVT_FC_MOTION)


class GUIMove(GUIBase):

    Cursor = HandCursor
    GrabCursor = GrabHandCursor
    def __init__(self, parent):
        GUIBase.__init__(self, parent)
        self.StartMove = None
        self.PrevMoveXY = None

    def OnLeftDown(self, event):
        self.parent.SetCursor(self.GrabCursor)
        self.parent.CaptureMouse()
        self.StartMove = N.array( event.GetPosition() )
        self.PrevMoveXY = (0,0)

    def OnLeftUp(self, event):
        if self.StartMove is not None:
            StartMove = self.StartMove
            EndMove = N.array(event.GetPosition())
            DiffMove = StartMove-EndMove
            if N.sum(DiffMove**2) > 16:
                self.parent.MoveImage(DiffMove, 'Pixel')
            self.StartMove = None
        self.parent.SetCursor(self.Cursor)

    def OnMove(self, event):
        # Allways raise the Move event.
        self.parent._RaiseMouseEvent(event,FloatCanvas.EVT_FC_MOTION)
        if event.Dragging() and event.LeftIsDown() and not self.StartMove is None:
            xy1 = N.array( event.GetPosition() )
            wh = self.parent.PanelSize
            xy_tl = xy1 - self.StartMove
            dc = wx.ClientDC(self.parent)
            dc.BeginDrawing()
            x1,y1 = self.PrevMoveXY
            x2,y2 = xy_tl
            w,h = self.parent.PanelSize
            ##fixme: This sure could be cleaner!
            if x2 > x1 and y2 > y1:
                xa = xb = x1
                ya = yb = y1
                wa = w
                ha = y2 - y1
                wb = x2-  x1
                hb = h
            elif x2 > x1 and y2 <= y1:
                xa = x1
                ya = y1
                wa = x2 - x1
                ha = h
                xb = x1
                yb = y2 + h
                wb = w
                hb = y1 - y2
            elif x2 <= x1 and y2 > y1:
                xa = x1
                ya = y1
                wa = w
                ha = y2 - y1
                xb = x2 + w
                yb = y1
                wb = x1 - x2
                hb = h - y2 + y1
            elif x2 <= x1 and y2 <= y1:
                xa = x2 + w
                ya = y1
                wa = x1 - x2
                ha = h
                xb = x1
                yb = y2 + h
                wb = w
                hb = y1 - y2

            dc.SetPen(wx.TRANSPARENT_PEN)
            dc.SetBrush(self.parent.BackgroundBrush)
            dc.DrawRectangle(xa, ya, wa, ha)
            dc.DrawRectangle(xb, yb, wb, hb)
            self.PrevMoveXY = xy_tl
            if self.parent._ForeDrawList:
                dc.DrawBitmapPoint(self.parent._ForegroundBuffer,xy_tl)
            else:
                dc.DrawBitmapPoint(self.parent._Buffer,xy_tl)
            dc.EndDrawing()

    def OnWheel(self, event):
        """
           By default, zoom in/out by a 0.1 factor per Wheel event.
        """
        if event.GetWheelRotation() < 0:
            self.parent.Zoom(0.9)
        else:
            self.parent.Zoom(1.1)

class GUIZoomIn(GUIBase):
 
    Cursor = MagPlusCursor
 
    def __init__(self, parent):
        GUIBase.__init__(self, parent)
        self.StartRBBox = None
        self.PrevRBBox = None

    def OnLeftDown(self, event):
        self.StartRBBox = N.array( event.GetPosition() )
        self.PrevRBBox = None
        self.parent.CaptureMouse()

    def OnLeftUp(self, event):
        #if self.parent.HasCapture():
        #        self.parent.ReleaseMouse()
        if event.LeftUp() and not self.StartRBBox is None:
            self.PrevRBBox = None
            EndRBBox = event.GetPosition()
            StartRBBox = self.StartRBBox
            # if mouse has moved less that ten pixels, don't use the box.
            if ( abs(StartRBBox[0] - EndRBBox[0]) > 10
                    and abs(StartRBBox[1] - EndRBBox[1]) > 10 ):
                EndRBBox = self.parent.PixelToWorld(EndRBBox)
                StartRBBox = self.parent.PixelToWorld(StartRBBox)
                BB = N.array(((min(EndRBBox[0],StartRBBox[0]),
                                min(EndRBBox[1],StartRBBox[1])),
                            (max(EndRBBox[0],StartRBBox[0]),
                                max(EndRBBox[1],StartRBBox[1]))),N.float_)
                self.parent.ZoomToBB(BB)
            else:
                Center = self.parent.PixelToWorld(StartRBBox)
                self.parent.Zoom(1.5,Center)
            self.StartRBBox = None

    def OnMove(self, event):
        # Allways raise the Move event.
        self.parent._RaiseMouseEvent(event,FloatCanvas.EVT_FC_MOTION)
        if event.Dragging() and event.LeftIsDown() and not (self.StartRBBox is None):
            xy0 = self.StartRBBox
            xy1 = N.array( event.GetPosition() )
            wh  = abs(xy1 - xy0)
            wh[0] = max(wh[0], int(wh[1]*self.parent.AspectRatio))
            wh[1] = int(wh[0] / self.parent.AspectRatio)
            xy_c = (xy0 + xy1) / 2
            dc = wx.ClientDC(self.parent)
            dc.BeginDrawing()
            dc.SetPen(wx.Pen('WHITE', 2, wx.SHORT_DASH))
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.SetLogicalFunction(wx.XOR)
            if self.PrevRBBox:
                dc.DrawRectanglePointSize(*self.PrevRBBox)
            self.PrevRBBox = ( xy_c - wh/2, wh )
            dc.DrawRectanglePointSize( *self.PrevRBBox )
            dc.EndDrawing()
            
    def UpdateScreen(self):
        """
        Update gets called if the screen has been repainted in the middle of a zoom in
        so the Rubber Band Box can get updated
        """
        if self.PrevRBBox is not None:
            dc = wx.ClientDC(self.parent)
            dc.SetPen(wx.Pen('WHITE', 2, wx.SHORT_DASH))
            dc.SetBrush(wx.TRANSPARENT_BRUSH)
            dc.SetLogicalFunction(wx.XOR)
            dc.DrawRectanglePointSize(*self.PrevRBBox)

    def OnRightDown(self, event):
        self.parent.Zoom(1/1.5, event.GetPosition(), centerCoords="pixel")

    def OnWheel(self, event):
        if event.GetWheelRotation() < 0:
            self.parent.Zoom(0.9)
        else:
            self.parent.Zoom(1.1)

class GUIZoomOut(GUIBase):

    Cursor = MagMinusCursor

    def OnLeftDown(self, event):
        self.parent.Zoom(1/1.5, event.GetPosition(), centerCoords="pixel")

    def OnRightDown(self, event):
        self.parent.Zoom(1.5, event.GetPosition(), centerCoords="pixel")

    def OnWheel(self, event):
        if event.GetWheelRotation() < 0:
            self.parent.Zoom(0.9)
        else:
            self.parent.Zoom(1.1)

    def OnMove(self, event):
        # Allways raise the Move event.
        self.parent._RaiseMouseEvent(event,FloatCanvas.EVT_FC_MOTION)

