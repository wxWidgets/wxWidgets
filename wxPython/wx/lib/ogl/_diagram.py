# -*- coding: iso-8859-1 -*-
#----------------------------------------------------------------------------
# Name:         diagram.py
# Purpose:      Diagram class
#
# Author:       Pierre Hjälm (from C++ original by Julian Smart)
#
# Created:      2004-05-08
# RCS-ID:       $Id$
# Copyright:    (c) 2004 Pierre Hjälm - 1998 Julian Smart
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import wx

DEFAULT_MOUSE_TOLERANCE = 3



class Diagram(object):
    """Encapsulates an entire diagram, with methods for drawing. A diagram has
    an associated ShapeCanvas.

    Derived from:
      Object
    """
    def __init__(self):
        self._diagramCanvas = None
        self._quickEditMode = False
        self._snapToGrid = True
        self._gridSpacing = 5.0
        self._shapeList = []
        self._mouseTolerance = DEFAULT_MOUSE_TOLERANCE

    def Redraw(self, dc):
        """Draw the shapes in the diagram on the specified device context."""
        if self._shapeList:
            if self.GetCanvas():
                self.GetCanvas().SetCursor(wx.HOURGLASS_CURSOR)
            for object in self._shapeList:
                object.Draw(dc)
            if self.GetCanvas():
                self.GetCanvas().SetCursor(wx.STANDARD_CURSOR)

    def Clear(self, dc):
        """Clear the specified device context."""
        dc.Clear()

    def AddShape(self, object, addAfter = None):
        """Adds a shape to the diagram. If addAfter is not None, the shape
        will be added after addAfter.
        """
        if not object in self._shapeList:
            if addAfter:
                self._shapeList.insert(self._shapeList.index(addAfter) + 1, object)
            else:
                self._shapeList.append(object)

            object.SetCanvas(self.GetCanvas())

    def InsertShape(self, object):
        """Insert a shape at the front of the shape list."""
        self._shapeList.insert(0, object)

    def RemoveShape(self, object):
        """Remove the shape from the diagram (non-recursively) but do not
        delete it.
        """
        if object in self._shapeList:
            self._shapeList.remove(object)
            
    def RemoveAllShapes(self):
        """Remove all shapes from the diagram but do not delete the shapes."""
        self._shapeList = []

    def DeleteAllShapes(self):
        """Remove and delete all shapes in the diagram."""
        for shape in self._shapeList[:]:
            if not shape.GetParent():
                self.RemoveShape(shape)
                
    def ShowAll(self, show):
        """Call Show for each shape in the diagram."""
        for shape in self._shapeList:
            shape.Show(show)

    def DrawOutline(self, dc, x1, y1, x2, y2):
        """Draw an outline rectangle on the current device context."""
        dc.SetPen(wx.Pen(wx.Color(0, 0, 0), 1, wx.DOT))
        dc.SetBrush(wx.TRANSPARENT_BRUSH)

        dc.DrawLines([[x1, y1], [x2, y1], [x2, y2], [x1, y2], [x1, y1]])

    def RecentreAll(self, dc):
        """Make sure all text that should be centred, is centred."""
        for shape in self._shapeList:
            shape.Recentre(dc)

    def SetCanvas(self, canvas):
        """Set the canvas associated with this diagram."""
        self._diagramCanvas = canvas

    def GetCanvas(self):
        """Return the shape canvas associated with this diagram."""
        return self._diagramCanvas
        
    def FindShape(self, id):
        """Return the shape for the given identifier."""
        for shape in self._shapeList:
            if shape.GetId() == id:
                return shape
        return None

    def Snap(self, x, y):
        """'Snaps' the coordinate to the nearest grid position, if
        snap-to-grid is on."""
        if self._snapToGrid:
            return self._gridSpacing * int(x / self._gridSpacing + 0.5), self._gridSpacing * int(y / self._gridSpacing + 0.5)
        return x, y

    def SetGridSpacing(self, spacing): 
        """Sets grid spacing.""" 
        self._gridSpacing = spacing 
 
    def SetSnapToGrid(self, snap): 
        """Sets snap-to-grid mode.""" 
        self._snapToGrid = snap 

    def GetGridSpacing(self):
        """Return the grid spacing."""
        return self._gridSpacing

    def GetSnapToGrid(self):
        """Return snap-to-grid mode."""
        return self._snapToGrid

    def SetQuickEditMode(self, mode):
        """Set quick-edit-mode on of off.

        In this mode, refreshes are minimized, but the diagram may need
        manual refreshing occasionally.
        """
        self._quickEditMode = mode

    def GetQuickEditMode(self):
        """Return quick edit mode."""
        return self._quickEditMode

    def SetMouseTolerance(self, tolerance):
        """Set the tolerance within which a mouse move is ignored.

        The default is 3 pixels.
        """
        self._mouseTolerance = tolerance

    def GetMouseTolerance(self):
        """Return the tolerance within which a mouse move is ignored."""
        return self._mouseTolerance

    def GetShapeList(self):
        """Return the internal shape list."""
        return self._shapeList

    def GetCount(self):
        """Return the number of shapes in the diagram."""
        return len(self._shapeList)
