
from wxPython.wx import *
from wxPython.ogl import *

#----------------------------------------------------------------------
# This creates some pens and brushes that the OGL library uses.

wxOGLInitialize()

#----------------------------------------------------------------------

class DiamondShape(wxPolygonShape):
    def __init__(self, w=0.0, h=0.0):
        wxPolygonShape.__init__(self)
        if w == 0.0:
            w = 60.0
        if h == 0.0:
            h = 60.0

        ## Either wxRealPoints or 2-tuples of floats  works.

        #points = [ wxRealPoint(0.0,    -h/2.0),
        #          wxRealPoint(w/2.0,  0.0),
        #          wxRealPoint(0.0,    h/2.0),
        #          wxRealPoint(-w/2.0, 0.0),
        #          ]
        points = [ (0.0,    -h/2.0),
                   (w/2.0,  0.0),
                   (0.0,    h/2.0),
                   (-w/2.0, 0.0),
                   ]

        self.Create(points)


#----------------------------------------------------------------------

class RoundedRectangleShape(wxRectangleShape):
    def __init__(self, w=0.0, h=0.0):
        wxRectangleShape.__init__(self, w, h)
        self.SetCornerRadius(-0.3)


#----------------------------------------------------------------------

class MyEvtHandler(wxShapeEvtHandler):
    def __init__(self, log, frame):
        wxShapeEvtHandler.__init__(self)
        self.log = log
        self.statbarFrame = frame


    def UpdateStatusBar(self, shape):
        x,y = shape.GetX(), shape.GetY()
        width, height = shape.GetBoundingBoxMax()
        self.statbarFrame.SetStatusText("Pos: (%d,%d)  Size: (%d, %d)" %
                                        (x, y, width, height))


    def OnLeftClick(self, x, y, keys = 0, attachment = 0):
        shape = self.GetShape()
        canvas = shape.GetCanvas()
        dc = wxClientDC(canvas)
        canvas.PrepareDC(dc)

        if shape.Selected():
            shape.Select(false, dc)
            canvas.Redraw(dc)
        else:
            redraw = false
            shapeList = canvas.GetDiagram().GetShapeList()
            toUnselect = []
            for s in shapeList:
                if s.Selected():
                    # If we unselect it now then some of the objects in
                    # shapeList will become invalid (the control points are
                    # shapes too!) and bad things will happen...
                    toUnselect.append(s)

            shape.Select(true, dc)

            if toUnselect:
                for s in toUnselect:
                    s.Select(false, dc)
                canvas.Redraw(dc)

        self.UpdateStatusBar(shape)


    def OnEndDragLeft(self, x, y, keys = 0, attachment = 0):
        shape = self.GetShape()
        self.base_OnEndDragLeft(x, y, keys, attachment)
        if not shape.Selected():
            self.OnLeftClick(x, y, keys, attachment)
        self.UpdateStatusBar(shape)


    def OnSize(self, x, y):
        self.base_OnSize(x, y)
        self.UpdateStatusBar(self.GetShape())


#    def OnMovePost(self, dc, x, y, oldX, oldY, display):
#        self.base_OnMovePost(dc, x, y, oldX, oldY, display)
#        self.UpdateStatusBar(self.GetShape())


    def OnRightClick(self, *dontcare):
        self.log.WriteText("%s\n" % self.GetShape())


#----------------------------------------------------------------------

class TestWindow(wxShapeCanvas):
    def __init__(self, parent, log, frame):
        wxShapeCanvas.__init__(self, parent)

        self.log = log
        self.frame = frame
        self.SetBackgroundColour(wxWHITE)
        self.diagram = wxDiagram()
        self.SetDiagram(self.diagram)
        self.diagram.SetCanvas(self)
        self.shapes = []

        rRectBrush = wxBrush(wxNamedColour("MEDIUM TURQUOISE"), wxSOLID)

        self.MyAddShape(wxCircleShape(80), 100, 100, wxPen(wxBLUE, 3), wxGREEN_BRUSH, "Circle")
        self.MyAddShape(wxRectangleShape(85, 50), 305, 60, wxBLACK_PEN, wxLIGHT_GREY_BRUSH, "Rectangle")
        self.MyAddShape(DiamondShape(90, 90), 345, 235, wxPen(wxBLUE, 3, wxDOT), wxRED_BRUSH, "Polygon")
        self.MyAddShape(RoundedRectangleShape(95,70), 140, 255, wxPen(wxRED, 1), rRectBrush, "Rounded Rect")

        dc = wxClientDC(self)
        self.PrepareDC(dc)
        for x in range(len(self.shapes)):
            fromShape = self.shapes[x]
            if x+1 == len(self.shapes):
                toShape = self.shapes[0]
            else:
                toShape = self.shapes[x+1]
            line = wxLineShape()
            line.SetCanvas(self)
            line.SetPen(wxBLACK_PEN)
            line.SetBrush(wxBLACK_BRUSH)
            line.AddArrow(ARROW_ARROW)
            line.MakeLineControlPoints(2)
            fromShape.AddLine(line, toShape)
            self.diagram.AddShape(line)
            line.Show(true)

            # for some reason, the shapes have to be moved for the line to show up...
            fromShape.Move(dc, fromShape.GetX(), fromShape.GetY())



    def MyAddShape(self, shape, x, y, pen, brush, text):
        shape.SetDraggable(true)
        shape.SetCanvas(self)
        shape.SetX(x)
        shape.SetY(y)
        shape.SetPen(pen)
        shape.SetBrush(brush)
        shape.AddText(text)
        #shape.SetShadowMode(SHADOW_RIGHT)
        self.diagram.AddShape(shape)
        shape.Show(true)

        evthandler = MyEvtHandler(self.log, self.frame)
        evthandler.SetShape(shape)
        evthandler.SetPreviousHandler(shape.GetEventHandler())
        shape.SetEventHandler(evthandler)

        self.shapes.append(shape)


    def __del__(self):
        for shape in self.diagram.GetShapeList():
            if shape.GetParent() == None:
                shape.SetCanvas(None)
                shape.Destroy()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestWindow(nb, log, frame)
    return win

#----------------------------------------------------------------------

class __Cleanup:
    cleanup = wxOGLCleanUp
    def __del__(self):
        self.cleanup()

# when this module gets cleaned up then wxOGLCleanUp() will get called
__cu = __Cleanup()








overview = """\
The Object Graphics Library is a library supporting the creation and
manipulation of simple and complex graphic images on a canvas.

"""


