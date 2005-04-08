#----------------------------------------------------------------------------
# Name:         AbstractEditor.py
# Purpose:      Non-text editor for DataModel and Process
#
# Author:       Peter Yared, Morgan Hua
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------


import wx
import wx.lib.docview
import wx.lib.ogl as ogl
import PropertyService
_ = wx.GetTranslation


SELECT_BRUSH = wx.Brush("BLUE", wx.SOLID)
SHAPE_BRUSH = wx.Brush("WHEAT", wx.SOLID)
LINE_BRUSH = wx.BLACK_BRUSH


def GetRawModel(model):
    if hasattr(model, "GetRawModel"):
        rawModel = model.GetRawModel()
    else:
        rawModel = model
    return rawModel


class CanvasView(wx.lib.docview.View):


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------


    def __init__(self, brush = SHAPE_BRUSH):
        wx.lib.docview.View.__init__(self)
        self._brush = brush
        self._canvas = None
        self._pt1 = None
        self._pt2 = None
        self._needEraseLasso = False
        self._propShape = None


    def OnCreate(self, doc, flags):
        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags)
        frame.Show()
        sizer = wx.BoxSizer()
        self._CreateCanvas(frame)
        sizer.Add(self._canvas, 1, wx.EXPAND, 0)
        frame.SetSizer(sizer)
        frame.Layout()        
        self.Activate()
        return True


    def OnActivateView(self, activate, activeView, deactiveView):
        if activate and self._canvas:
            # In MDI mode just calling set focus doesn't work and in SDI mode using CallAfter causes an endless loop
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
                self._canvas.SetFocus()
            else:
                wx.CallAfter(self._canvas.SetFocus)


    def OnClose(self, deleteWindow = True):
        statusC = wx.GetApp().CloseChildDocuments(self.GetDocument())
        statusP = wx.lib.docview.View.OnClose(self, deleteWindow = deleteWindow)
        if hasattr(self, "ClearOutline"):
            wx.CallAfter(self.ClearOutline)  # need CallAfter because when closing the document, it is Activated and then Close, so need to match OnActivateView's CallAfter
        if not (statusC and statusP):
            return False
        self.Activate(False)
        if deleteWindow and self.GetFrame():
            self.GetFrame().Destroy()
        return True


    def _CreateCanvas(self, parent):
        self._canvas = ogl.ShapeCanvas(parent)
        wx.EVT_LEFT_DOWN(self._canvas, self.OnLeftClick)
        wx.EVT_LEFT_UP(self._canvas, self.OnLeftUp)
        wx.EVT_MOTION(self._canvas, self.OnLeftDrag)
        wx.EVT_LEFT_DCLICK(self._canvas, self.OnLeftDoubleClick)
        wx.EVT_KEY_DOWN(self._canvas, self.OnKeyPressed)

        maxWidth = 2000
        maxHeight = 16000
        self._canvas.SetScrollbars(20, 20, maxWidth / 20, maxHeight / 20)
        
        self._canvas.SetBackgroundColour(wx.WHITE)
        self._diagram = ogl.Diagram()
        self._canvas.SetDiagram(self._diagram)
        self._diagram.SetCanvas(self._canvas)


    def OnKeyPressed(self, event):
        key = event.KeyCode()
        if key == wx.WXK_DELETE:
            self.OnClear(event)
        else:
            event.Skip()


    def OnLeftClick(self, event):
        self.EraseRubberBand()

        dc = wx.ClientDC(self._canvas)
        self._canvas.PrepareDC(dc)

        # keep track of mouse down for group select
        self._pt1 = event.GetLogicalPosition(dc)  # this takes into account scrollbar offset
        self._pt2 = None

        shape = self._canvas.FindShape(self._pt1[0], self._pt1[1])[0]
        if shape:
            self.BringToFront(shape)

            self._pt1 = None
            event.Skip()  # pass on event to shape handler to take care of selection

            return
        elif event.ControlDown() or event.ShiftDown():    # extend select, don't deselect
            pass
        else:
            # click on empty part of canvas, deselect everything
            needRefresh = False
            for shape in self._diagram.GetShapeList():
                if hasattr(shape, "GetModel"):
                    if shape.Selected():
                        needRefresh = True
                        shape.Select(False, dc)
            if needRefresh:
                self._canvas.Redraw(dc)

            self.SetPropertyModel(None)

        if len(self.GetSelection()) == 0:
            self.SetPropertyShape(None)



    def OnLeftDoubleClick(self, event):
        propertyService = wx.GetApp().GetService(PropertyService.PropertyService)
        if propertyService:
            propertyService.ShowWindow()


    def OnLeftDrag(self, event):
        # draw lasso for group select
        if self._pt1 and event.LeftIsDown():   # we are in middle of lasso selection
            self.EraseRubberBand()

            dc = wx.ClientDC(self._canvas)
            self._canvas.PrepareDC(dc)
            self._pt2 = event.GetLogicalPosition(dc)  # this takes into account scrollbar offset
            self.DrawRubberBand()
        else:
            event.Skip()


    def OnLeftUp(self, event):
        # do group select
        if self._needEraseLasso:
            self.EraseRubberBand()

            dc = wx.ClientDC(self._canvas)
            self._canvas.PrepareDC(dc)
            x1, y1 = self._pt1
            x2, y2 = event.GetLogicalPosition(dc)  # this takes into account scrollbar offset

            tol = self._diagram.GetMouseTolerance()
            if abs(x1 - x2) > tol or abs(y1 - y2) > tol:
                # make sure x1 < x2 and y1 < y2 to make comparison test easier
                if x1 > x2:
                    temp = x1
                    x1 = x2
                    x2 = temp
                if y1 > y2:
                    temp = y1
                    y1 = y2
                    y2 = temp

                for shape in self._diagram.GetShapeList():
                    if not shape.GetParent() and hasattr(shape, "GetModel"):  # if part of a composite, don't select it
                        x, y = shape.GetX(), shape.GetY()
                        width, height = shape.GetBoundingBoxMax()
                        selected = x1 < x - width/2 and x2 > x + width/2 and y1 < y - height/2 and y2 > y + height/2
                        if event.ControlDown() or event.ShiftDown():    # extend select, don't deselect
                            if selected:
                                shape.Select(selected, dc)
                        else:   # select items in lasso and deselect items out of lasso
                            shape.Select(selected, dc)
                self._canvas.Redraw(dc)
            else:
                event.Skip()
        else:
            event.Skip()


    def EraseRubberBand(self):
        if self._needEraseLasso:
            self._needEraseLasso = False

            dc = wx.ClientDC(self._canvas)
            self._canvas.PrepareDC(dc)
            dc.SetLogicalFunction(wx.XOR)
            pen = wx.Pen(wx.Colour(200, 200, 200), 1, wx.SHORT_DASH)
            dc.SetPen(pen)
            brush = wx.Brush(wx.Colour(255, 255, 255), wx.TRANSPARENT)
            dc.SetBrush(brush)
            dc.ResetBoundingBox()
            dc.BeginDrawing()

            x1, y1 = self._pt1
            x2, y2 = self._pt2

            # make sure x1 < x2 and y1 < y2
            # this will make (x1, y1) = upper left corner
            if x1 > x2:
                temp = x1
                x1 = x2
                x2 = temp
            if y1 > y2:
                temp = y1
                y1 = y2
                y2 = temp

            # erase previous outline
            dc.SetClippingRegion(x1, y1, x2 - x1, y2 - y1)
            dc.DrawRectangle(x1, y1, x2 - x1, y2 - y1)
            dc.EndDrawing()


    def DrawRubberBand(self):
        self._needEraseLasso = True

        dc = wx.ClientDC(self._canvas)
        self._canvas.PrepareDC(dc)
        dc.SetLogicalFunction(wx.XOR)
        pen = wx.Pen(wx.Colour(200, 200, 200), 1, wx.SHORT_DASH)
        dc.SetPen(pen)
        brush = wx.Brush(wx.Colour(255, 255, 255), wx.TRANSPARENT)
        dc.SetBrush(brush)
        dc.ResetBoundingBox()
        dc.BeginDrawing()

        x1, y1 = self._pt1
        x2, y2 = self._pt2

        # make sure x1 < x2 and y1 < y2
        # this will make (x1, y1) = upper left corner
        if x1 > x2:
            temp = x1
            x1 = x2
            x2 = temp
        if y1 > y2:
            temp = y1
            y1 = y2
            y2 = temp

        # draw outline
        dc.SetClippingRegion(x1, y1, x2 - x1, y2 - y1)
        dc.DrawRectangle(x1, y1, x2 - x1, y2 - y1)
        dc.EndDrawing()


    def FindParkingSpot(self, width, height):
        """ given a width and height, find a upper left corner where shape can be parked without overlapping other shape """
        offset = 30  # space between shapes
        x = offset
        y = offset
        maxX = 700  # max distance to the right where we'll place tables
        noParkingSpot = True

        while noParkingSpot:
            point = self.isSpotOccupied(x, y, width, height)
            if point:
                x = point[0] + offset
                if x > maxX:
                    x = offset
                    y = point[1] + offset
            else:
                noParkingSpot = False

        return x, y


    def isSpotOccupied(self, x, y, width, height):
        """ returns None if at x,y,width,height no object occupies that rectangle,
            otherwise returns lower right corner of object that occupies given x,y position
        """
        x2 = x + width
        y2 = y + height

        for shape in self._diagram.GetShapeList():
            if isinstance(shape, ogl.RectangleShape) or isinstance(shape, ogl.EllipseShape):
                if shape.GetParent() and isinstance(shape.GetParent(), ogl.CompositeShape):
                    # skip, part of a composite shape
                    continue

                if hasattr(shape, "GetModel"):
                    other_x, other_y, other_width, other_height = shape.GetModel().getEditorBounds()
                    other_x2 = other_x + other_width
                    other_y2 = other_y + other_height
                else:
                    # shapes x,y are at the center of the shape, need to transform to upper left coordinate
                    other_width, other_height = shape.GetBoundingBoxMax()
                    other_x = shape.GetX() - other_width/2
                    other_y = shape.GetY() - other_height/2

                other_x2 = other_x + other_width
                other_y2 = other_y + other_height
                # intersection check
                if ((other_x2 < other_x or other_x2 > x) and
                    (other_y2 < other_y or other_y2 > y) and
                    (x2 < x or x2 > other_x) and
                    (y2 < y or y2 > other_y)):
                    return (other_x2, other_y2)
        return None


    #----------------------------------------------------------------------------
    # Canvas methods
    #----------------------------------------------------------------------------

    def AddShape(self, shape, x = None, y = None, pen = None, brush = None, text = None, eventHandler = None):
        if isinstance(shape, ogl.CompositeShape):
            dc = wx.ClientDC(self._canvas)
            self._canvas.PrepareDC(dc)
            shape.Move(dc, x, y)
        else:
            shape.SetDraggable(True, True)
        shape.SetCanvas(self._canvas)

        if x:
            shape.SetX(x)
        if y:
            shape.SetY(y)
        shape.SetCentreResize(False)
        if pen:
            shape.SetPen(pen)
        if brush:
            shape.SetBrush(brush)
        if text:
            shape.AddText(text)
        shape.SetShadowMode(ogl.SHADOW_RIGHT)
        self._diagram.AddShape(shape)
        shape.Show(True)
        if not eventHandler:
            eventHandler = EditorCanvasShapeEvtHandler(self)
        eventHandler.SetShape(shape)
        eventHandler.SetPreviousHandler(shape.GetEventHandler())
        shape.SetEventHandler(eventHandler)
        return shape


    def RemoveShape(self, model = None, shape = None):
        if not model and not shape:
            return

        if not shape:
            shape = self.GetShape(model)

        if shape:
            shape.Select(False)
            self._diagram.RemoveShape(shape)
            if isinstance(shape, ogl.CompositeShape):
                shape.RemoveFromCanvas(self._canvas)


    def UpdateShape(self, model):
        for shape in self._diagram.GetShapeList():
            if hasattr(shape, "GetModel") and shape.GetModel() == model:
                x, y, w, h = model.getEditorBounds()
                newX = x + w / 2
                newY = y + h / 2
                changed = False
                if isinstance(shape, ogl.CompositeShape):
                    if shape.GetX() != newX or shape.GetY() != newY:
                        dc = wx.ClientDC(self._canvas)
                        self._canvas.PrepareDC(dc)
                        shape.SetSize(w, h, True)  # wxBug: SetSize must be before Move because links won't go to the right place
                        shape.Move(dc, newX, newY)  # wxBug: Move must be before SetSize because links won't go to the right place
                        changed = True
                else:
                    oldw, oldh = shape.GetBoundingBoxMax()
                    oldx = shape.GetX()
                    oldy = shape.GetY()
                    if oldw != w or oldh != h or oldx != newX or oldy != newY:
                        shape.SetSize(w, h)
                        shape.SetX(newX)
                        shape.SetY(newY)
                        changed = True
                if changed:
                    shape.ResetControlPoints()
                    self._canvas.Refresh()
                break


    def GetShape(self, model):
        for shape in self._diagram.GetShapeList():
            if hasattr(shape, "GetModel") and shape.GetModel() == model:
                return shape
        return None


    def GetSelection(self):
        return filter(lambda shape: shape.Selected(), self._diagram.GetShapeList())


    def SetSelection(self, models, extendSelect = False):
        dc = wx.ClientDC(self._canvas)
        self._canvas.PrepareDC(dc)
        update = False
        if not isinstance(models, type([])) and not isinstance(models, type(())):
            models = [models]
        for shape in self._diagram.GetShapeList():
            if hasattr(shape, "GetModel"):
                if shape.Selected() and not shape.GetModel() in models:  # was selected, but not in new list, so deselect, unless extend select
                    if not extendSelect:
                        shape.Select(False, dc)
                        update = True
                elif not shape.Selected() and shape.GetModel() in models: # was not selected and in new list, so select
                    shape.Select(True, dc)
                    update = True
                elif extendSelect and shape.Selected() and shape.GetModel() in models: # was selected, but extend select means to deselect
                    shape.Select(False, dc)
                    update = True
        if update:
            self._canvas.Redraw(dc)


    def BringToFront(self, shape):
        if shape.GetParent() and isinstance(shape.GetParent(), ogl.CompositeShape):
            self._diagram.RemoveShape(shape.GetParent())
            self._diagram.AddShape(shape.GetParent())
        else:
            self._diagram.RemoveShape(shape)
            self._diagram.AddShape(shape)


    def SendToBack(self, shape):
        if shape.GetParent() and isinstance(shape.GetParent(), ogl.CompositeShape):
            self._diagram.RemoveShape(shape.GetParent())
            self._diagram.InsertShape(shape.GetParent())
        else:
            self._diagram.RemoveShape(shape)
            self._diagram.InsertShape(shape)


    def ScrollVisible(self, shape):
        xUnit, yUnit = shape._canvas.GetScrollPixelsPerUnit()
        scrollX, scrollY = self._canvas.GetViewStart()  # in scroll units
        scrollW, scrollH = self._canvas.GetSize()  # in pixels
        w, h = shape.GetBoundingBoxMax() # in pixels
        x = shape.GetX() - w/2  # convert to upper left coordinate from center
        y = shape.GetY() - h/2  # convert to upper left coordinate from center

        if x >= scrollX*xUnit and x <= scrollX*xUnit + scrollW:  # don't scroll if already visible
            x = -1
        else:
            x = x/xUnit

        if y >= scrollY*yUnit and y <= scrollY*yUnit + scrollH:  # don't scroll if already visible
            y = -1
        else:
            y = y/yUnit

        self._canvas.Scroll(x, y)  # in scroll units


    def SetPropertyShape(self, shape):
        # no need to highlight if no PropertyService is running
        propertyService = wx.GetApp().GetService(PropertyService.PropertyService)
        if not propertyService:
            return

        if shape == self._propShape:
            return

        if hasattr(shape, "GetPropertyShape"):
            shape = shape.GetPropertyShape()

        dc = wx.ClientDC(self._canvas)
        self._canvas.PrepareDC(dc)
        dc.BeginDrawing()

        # erase old selection if it still exists
        if self._propShape and self._propShape in self._diagram.GetShapeList():
            self._propShape.SetBrush(self._brush)
            if (self._propShape._textColourName in ["BLACK", "WHITE"]):  # Would use GetTextColour() but it is broken
                self._propShape.SetTextColour("BLACK", 0)
            self._propShape.Draw(dc)

        # set new selection
        self._propShape = shape

        # draw new selection
        if self._propShape and self._propShape in self._diagram.GetShapeList():
            self._propShape.SetBrush(SELECT_BRUSH)
            if (self._propShape._textColourName in ["BLACK", "WHITE"]):  # Would use GetTextColour() but it is broken
                self._propShape.SetTextColour("WHITE", 0)
            self._propShape.Draw(dc)

        dc.EndDrawing()


    #----------------------------------------------------------------------------
    # Property Service methods
    #----------------------------------------------------------------------------

    def GetPropertyModel(self):
        if hasattr(self, "_propModel"):
            return self._propModel
        return None
        

    def SetPropertyModel(self, model):
        # no need to set the model if no PropertyService is running
        propertyService = wx.GetApp().GetService(PropertyService.PropertyService)
        if not propertyService:
            return
            
        if hasattr(self, "_propModel") and model == self._propModel:
            return
            
        self._propModel = model
        propertyService.LoadProperties(self._propModel, self.GetDocument())


class EditorCanvasShapeMixin:

    def GetModel(self):
        return self._model


    def SetModel(self, model):
        self._model = model


class EditorCanvasShapeEvtHandler(ogl.ShapeEvtHandler):

    """ wxBug: Bug in OLG package.  With wxShape.SetShadowMode() turned on, when we set the size,
        the width/height is larger by 6 pixels.  Need to subtract this value from width and height when we
        resize the object.
    """
    SHIFT_KEY = 1
    CONTROL_KEY = 2

    def __init__(self, view):
        ogl.ShapeEvtHandler.__init__(self)
        self._view = view


    def OnLeftClick(self, x, y, keys = 0, attachment = 0):
        shape = self.GetShape()
        if hasattr(shape, "GetModel"):  # Workaround, on drag, we should deselect all other objects and select the clicked on object
            model = shape.GetModel()
        else:
            shape = shape.GetParent()
            if shape:
                model = shape.GetModel()

        self._view.SetSelection(model, keys == self.SHIFT_KEY or keys == self.CONTROL_KEY)
        self._view.SetPropertyShape(shape)
        self._view.SetPropertyModel(model)


    def OnEndDragLeft(self, x, y, keys = 0, attachment = 0):
        ogl.ShapeEvtHandler.OnEndDragLeft(self, x, y, keys, attachment)
        shape = self.GetShape()
        if hasattr(shape, "GetModel"):  # Workaround, on drag, we should deselect all other objects and select the clicked on object
            model = shape.GetModel()
        else:
            parentShape = shape.GetParent()
            if parentShape:
                model = parentShape.GetModel()
        self._view.SetSelection(model, keys == self.SHIFT_KEY or keys == self.CONTROL_KEY)


    def OnMovePost(self, dc, x, y, oldX, oldY, display):
        if x == oldX and y == oldY:
            return
        if not self._view.GetDocument():
            return
        shape = self.GetShape()
        if isinstance(shape, EditorCanvasShapeMixin) and shape.Draggable():
            model = shape.GetModel()
            if hasattr(model, "getEditorBounds") and model.getEditorBounds():
                x, y, w, h = model.getEditorBounds()
                newX = shape.GetX() - shape.GetBoundingBoxMax()[0] / 2
                newY = shape.GetY() - shape.GetBoundingBoxMax()[1] / 2
                newWidth = shape.GetBoundingBoxMax()[0]
                newHeight = shape.GetBoundingBoxMax()[1]
                if shape._shadowMode != ogl.SHADOW_NONE:
                    newWidth -= shape._shadowOffsetX
                    newHeight -= shape._shadowOffsetY
                newbounds = (newX, newY, newWidth, newHeight)
    
                if x != newX or y != newY or w != newWidth or h != newHeight:
                    self._view.GetDocument().GetCommandProcessor().Submit(EditorCanvasUpdateShapeBoundariesCommand(self._view.GetDocument(), model, newbounds))


    def Draw(self, dc):
        pass


class EditorCanvasUpdateShapeBoundariesCommand(wx.lib.docview.Command):


    def __init__(self, canvasDocument, model, newbounds):
        wx.lib.docview.Command.__init__(self, canUndo = True)
        self._canvasDocument = canvasDocument
        self._model = model
        self._oldbounds = model.getEditorBounds()
        self._newbounds = newbounds


    def GetName(self):
        name = self._canvasDocument.GetNameForObject(self._model)
        if not name:
            name = ""
            print "ERROR: AbstractEditor.EditorCanvasUpdateShapeBoundariesCommand.GetName: unable to get name for ", self._model
        return _("Move/Resize %s") % name


    def Do(self):
        return self._canvasDocument.UpdateEditorBoundaries(self._model, self._newbounds)


    def Undo(self):
        return self._canvasDocument.UpdateEditorBoundaries(self._model, self._oldbounds)

