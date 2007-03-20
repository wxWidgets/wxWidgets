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
INACTIVE_SELECT_BRUSH = wx.Brush("LIGHT BLUE", wx.SOLID)

NORMALFONT = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
SLANTFONT = wx.Font(NORMALFONT.GetPointSize(), NORMALFONT.GetFamily(), wx.SLANT, NORMALFONT.GetWeight())
BOLDFONT = wx.Font(NORMALFONT.GetPointSize(), NORMALFONT.GetFamily(), NORMALFONT.GetStyle(), wx.BOLD)

DEFAULT_BACKGROUND_COLOR = wx.Colour(0xEE, 0xEE, 0xEE)
HEADER_BRUSH = wx.Brush(wx.Colour(0xDB, 0xEB, 0xFF), wx.SOLID)
BODY_BRUSH = wx.Brush(wx.WHITE, wx.SOLID)


PARKING_VERTICAL = 1
PARKING_HORIZONTAL = 2
PARKING_OFFSET = 30    # space between shapes

FORCE_REDRAW_METHOD = "ForceRedraw"

def GetRawModel(model):
    if hasattr(model, "GetRawModel"):
        rawModel = model.GetRawModel()
    else:
        rawModel = model
    return rawModel


def GetLabel(model):
    model = GetRawModel(model)
    if hasattr(model, "__xmlname__"):
        label = model.__xmlname__
        try:
            if (len(label) > 0):
                label = label[0].upper() + label[1:]
            if (hasattr(model, "complexType")):
                label += ': %s/%s' % (model.complexType.name, model.name)
            else:
                if model.name:
                    label += ': %s' % model.name
                elif model.ref:
                    label += ': %s' % model.ref
        except AttributeError:
            pass
    else:
        label = str(model)
    return label


class CanvasView(wx.lib.docview.View):


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------


    def __init__(self, brush=SHAPE_BRUSH, background=DEFAULT_BACKGROUND_COLOR):
        wx.lib.docview.View.__init__(self)
        self._brush = brush
        self._backgroundColor = background
        self._canvas = None
        self._pt1 = None
        self._pt2 = None
        self._needEraseLasso = False
        self._propShape = None
        self._maxWidth = 2000
        self._maxHeight = 16000
        self._valetParking = False


    def OnDraw(self, dc):
        """ for Print Preview and Print """
        dc.BeginDrawing()
        self._canvas.Redraw(dc)
        dc.EndDrawing()
        

    def OnCreate(self, doc, flags):
        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags)
        frame.Show()
        sizer = wx.BoxSizer()
        self._CreateCanvas(frame)
        sizer.Add(self._canvas, 1, wx.EXPAND, 0)
        frame.SetSizer(sizer)
        frame.Layout()        
        self.Activate()
        wx.EVT_RIGHT_DOWN(self._canvas, self.OnRightClick)
        return True
        

    def OnActivateView(self, activate, activeView, deactiveView):
        if activate and self._canvas:
            # In MDI mode just calling set focus doesn't work and in SDI mode using CallAfter causes an endless loop
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
                self.SetFocus()
            else:
                wx.CallAfter(self.SetFocus)


    def SetFocus(self):
        if self._canvas:
            self._canvas.SetFocus()


    def OnFocus(self, event):
        self.FocusColorPropertyShape(True)
        event.Skip()


    def FocusOnClick(self, event):
        self.SetFocus()
        event.Skip()


    def OnKillFocus(self, event):
        self.FocusColorPropertyShape(False)
        event.Skip()


    def HasFocus(self):
        winWithFocus = wx.Window.FindFocus()
        if not winWithFocus:
            return False
        while winWithFocus:
            if winWithFocus == self._canvas:
                return True
            winWithFocus = winWithFocus.GetParent()
        return False


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
        
        # need this otherwise mouse clicks don't set focus to this view
        wx.EVT_LEFT_DOWN(self._canvas, self.FocusOnClick)
        wx.EVT_LEFT_DCLICK(self._canvas, self.FocusOnClick)
        wx.EVT_RIGHT_DOWN(self._canvas, self.FocusOnClick)
        wx.EVT_RIGHT_DCLICK(self._canvas, self.FocusOnClick)
        wx.EVT_MIDDLE_DOWN(self._canvas, self.FocusOnClick)
        wx.EVT_MIDDLE_DCLICK(self._canvas, self.FocusOnClick)
        
        wx.EVT_KILL_FOCUS(self._canvas, self.OnKillFocus)
        wx.EVT_SET_FOCUS(self._canvas, self.OnFocus)

        self._canvas.SetScrollbars(20, 20, self._maxWidth / 20, self._maxHeight / 20)
        
        self._canvas.SetBackgroundColour(self._backgroundColor)
        self._diagram = ogl.Diagram()
        self._canvas.SetDiagram(self._diagram)
        self._diagram.SetCanvas(self._canvas)
        self._canvas.SetFont(NORMALFONT)


    def OnClear(self, event):
        """ Deletion of selected objects from view.
        *Must Override*
        """
        self.SetPropertyModel(None)
                  

    def SetLastRightClick(self, x, y):
        self._lastRightClick = (x,y)
        

    def GetLastRightClick(self):
        if hasattr(self, "_lastRightClick"):
            return self._lastRightClick
        return (-1,-1)
        

    def OnKeyPressed(self, event):
        key = event.KeyCode()
        if key == wx.WXK_DELETE:
            self.OnClear(event)
        else:
            event.Skip()


    def OnRightClick(self, event):
        """ force selection underneath right click position. """
        self.Activate()
        self._canvas.SetFocus()

        dc = wx.ClientDC(self._canvas)
        self._canvas.PrepareDC(dc)
        x, y = event.GetLogicalPosition(dc)  # this takes into account scrollbar offset
        self.SetLastRightClick(x, y)
        shape = self._canvas.FindShape(x, y)[0]
        
        model = None
        if not shape:
            self.SetSelection(None)
            self.SetPropertyShape(None)
        elif hasattr(shape, "GetModel"):
            self.BringToFront(shape)
            self.SetPropertyShape(shape)
            self.SetSelection(shape)
            shape.Select(True, dc)
            model = shape.GetModel()
        elif shape.GetParent() and isinstance(shape.GetParent(), ogl.CompositeShape):  # ComplexTypeHeader for ComplexTypeShape
            self.BringToFront(shape)
            self.SetPropertyShape(shape.GetParent())
            self.SetSelection(shape.GetParent())
            shape.GetParent().Select(True, dc)
            model = shape.GetParent().GetModel()

        self.SetPropertyModel(model)
        
        return (shape, model)


    def OnLeftClick(self, event):
        self.Activate()
        self._canvas.SetFocus()
        
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
            forceRedrawShapes = []
            needRefresh = False
            for shape in self._diagram.GetShapeList():
                if hasattr(shape, "GetModel"):
                    if shape.Selected():
                        needRefresh = True
                        shape.Select(False, dc)
                        if hasattr(shape, FORCE_REDRAW_METHOD):
                            forceRedrawShapes.append(shape)
            if needRefresh:
                self._canvas.Redraw(dc)

            self.SetPropertyModel(None)

        if len(self.GetSelection()) == 0:
            self.SetPropertyShape(None)

        for shape in forceRedrawShapes:
            shape.ForceRedraw()

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


    def SetValetParking(self, enable=True):
        """ If valet parking is enabled, remember last parking spot and try for a spot near it """
        self._valetParking = enable
        if enable:
            self._valetPosition = None
        

    def FindParkingSpot(self, width, height, parking=PARKING_HORIZONTAL, x=PARKING_OFFSET, y=PARKING_OFFSET):
        """
            Given a width and height, find a upper left corner where shape can be parked without overlapping other shape
        """
        if self._valetParking and self._valetPosition:
            x, y = self._valetPosition
        
        max = 700  # max distance to the right where we'll place tables
        noParkingSpot = True

        while noParkingSpot:
            point = self.isSpotOccupied(x, y, width, height)
            if point:
                if parking == PARKING_HORIZONTAL:
                    x = point[0] + PARKING_OFFSET
                    if x > max:
                        x = PARKING_OFFSET
                        y = point[1] + PARKING_OFFSET
                else:  # parking == PARKING_VERTICAL:
                    y = point[1] + PARKING_OFFSET
                    if y > max:
                        y = PARKING_OFFSET
                        x = point[0] + PARKING_OFFSET
            else:
                noParkingSpot = False

        if self._valetParking:
            self._valetPosition = (x, y)
            
        return x, y


    def isSpotOccupied(self, x, y, width, height):
        """ returns None if at x,y,width,height no object occupies that rectangle,
            otherwise returns lower right corner of object that occupies given x,y position
        """
        x2 = x + width
        y2 = y + height

        for shape in self._diagram.GetShapeList():
            if isinstance(shape, ogl.RectangleShape) or isinstance(shape, ogl.EllipseShape) or isinstance(shape, ogl.PolygonShape):
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

    def AddShape(self, shape, x = None, y = None, pen = None, brush = None, text = None, eventHandler = None, shown=True):
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
        shape.SetShadowMode(ogl.SHADOW_NONE)
        self._diagram.AddShape(shape)
        shape.Show(shown)
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
            for line in shape.GetLines():
                shape.RemoveLine(line)
                self._diagram.RemoveShape(line)
                line.Delete()
            for obj in self._diagram.GetShapeList():
                for line in obj.GetLines():
                    if self.IsShapeContained(shape, line.GetTo()) or self.IsShapeContained(shape, line.GetFrom()):
                        obj.RemoveLine(line)
                        self._diagram.RemoveShape(line)
                        line.Delete()
                    if line == shape:
                        obj.RemoveLine(line)
                        self._diagram.RemoveShape(line)
                        line.Delete()
                    
            if self._canvas:
                shape.RemoveFromCanvas(self._canvas)
            self._diagram.RemoveShape(shape)
            shape.Delete()


    def IsShapeContained(self, parent, shape):
        if parent == shape:
            return True
        elif shape.GetParent():
            return self.IsShapeContained(parent, shape.GetParent())
            
        return False


    def UpdateShape(self, model):
        for shape in self._diagram.GetShapeList():
            if hasattr(shape, "GetModel") and shape.GetModel() == model:
                oldw, oldh = shape.GetBoundingBoxMax()
                oldx = shape.GetX()
                oldy = shape.GetY()

                x, y, w, h = model.getEditorBounds()
                newX = x + w / 2
                newY = y + h / 2
                
                if oldw != w or oldh != h or oldx != newX or oldy != newY:
                    dc = wx.ClientDC(self._canvas)
                    self._canvas.PrepareDC(dc)
                    shape.SetSize(w, h, True)   # wxBug: SetSize must be before Move because links won't go to the right place
                    shape.Move(dc, newX, newY)  # wxBug: Move must be after SetSize because links won't go to the right place
                    shape.ResetControlPoints()
                    self._canvas.Refresh()
                    
                break


    def GetShape(self, model):
        for shape in self._diagram.GetShapeList():
            if hasattr(shape, "GetModel") and shape.GetModel() == model:
                return shape
        return None


    def GetShapeCount(self):
        return self._diagram.GetCount()


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
        if not shape:
            return
            
        xUnit, yUnit = self._canvas.GetScrollPixelsPerUnit()
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
            if hasattr(self._propShape, "DEFAULT_BRUSH"):
                self._propShape.SetBrush(self._propShape.DEFAULT_BRUSH)
            else:
                self._propShape.SetBrush(self._brush)
            if (self._propShape._textColourName in ["BLACK", "WHITE"]):  # Would use GetTextColour() but it is broken
                self._propShape.SetTextColour("BLACK", 0)
            self._propShape.Draw(dc)

        # set new selection
        self._propShape = shape

        # draw new selection
        if self._propShape and self._propShape in self._diagram.GetShapeList():
            if self.HasFocus():
                self._propShape.SetBrush(SELECT_BRUSH)
            else:
                self._propShape.SetBrush(INACTIVE_SELECT_BRUSH)
            if (self._propShape._textColourName in ["BLACK", "WHITE"]):  # Would use GetTextColour() but it is broken
                self._propShape.SetTextColour("WHITE", 0)
            self._propShape.Draw(dc)

        dc.EndDrawing()


    def FocusColorPropertyShape(self, gotFocus=False):
        # no need to change highlight if no PropertyService is running
        propertyService = wx.GetApp().GetService(PropertyService.PropertyService)
        if not propertyService:
            return

        if not self._propShape:
            return

        dc = wx.ClientDC(self._canvas)
        self._canvas.PrepareDC(dc)
        dc.BeginDrawing()

        # draw deactivated selection
        if self._propShape and self._propShape in self._diagram.GetShapeList():
            if gotFocus:
                self._propShape.SetBrush(SELECT_BRUSH)
            else:
                self._propShape.SetBrush(INACTIVE_SELECT_BRUSH)
            if (self._propShape._textColourName in ["BLACK", "WHITE"]):  # Would use GetTextColour() but it is broken
                self._propShape.SetTextColour("WHITE", 0)
            self._propShape.Draw(dc)

            if hasattr(self._propShape, FORCE_REDRAW_METHOD):
                self._propShape.ForceRedraw()

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

        if model:
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


    def OnMovePre(self, dc, x, y, oldX, oldY, display):
        """ Prevent objects from being dragged outside of viewable area """
        if (x < 0) or (y < 0) or (x > self._view._maxWidth) or (y > self._view._maxHeight):
            return False

        return ogl.ShapeEvtHandler.OnMovePre(self, dc, x, y, oldX, oldY, display)


    def OnMovePost(self, dc, x, y, oldX, oldY, display):
        """ Update the model's record of where the shape should be.  Also enable redo/undo.  """
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

