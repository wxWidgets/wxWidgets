# -*- coding: iso-8859-1 -*-
# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 20040508 - Pierre Hjälm
#
# o Changed to use the python version of OGL
# o Added TextShape, CompositeShape and CompositeShape with divisions
#
# 20040830 - Pierre Hjälm
#
# o Added DrawnShape
#

import wx
import wx.lib.ogl as ogl

import  images

#----------------------------------------------------------------------

class DrawnShape(ogl.DrawnShape):
    def __init__(self):
        ogl.DrawnShape.__init__(self)

        self.SetDrawnBrush(wx.WHITE_BRUSH)
        self.SetDrawnPen(wx.BLACK_PEN)
        self.DrawArc((0, -10), (30, 0), (-30, 0))

        self.SetDrawnPen(wx.Pen("#ff8030"))
        self.DrawLine((-30, 5), (30, 5))

        self.SetDrawnPen(wx.Pen("#00ee10"))
        self.DrawRoundedRectangle((-20, 10, 40, 10), 5)

        self.SetDrawnPen(wx.Pen("#9090f0"))
        self.DrawEllipse((-30, 25, 60, 20))

        self.SetDrawnTextColour(wx.BLACK)
        self.SetDrawnFont(wx.Font(8, wx.SWISS, wx.NORMAL, wx.NORMAL))
        self.DrawText("DrawText", (-26, 28))

        self.SetDrawnBrush(wx.GREEN_BRUSH)
        self.DrawPolygon([(-100, 5), (-45, 30), (-35, 20), (-30, 5)])

        self.SetDrawnPen(wx.BLACK_PEN)
        self.DrawLines([(30, -45), (40, -45), (40 ,45), (30, 45)])

        # Make sure to call CalculateSize when all drawing is done
        self.CalculateSize()
        
#----------------------------------------------------------------------

class DiamondShape(ogl.PolygonShape):
    def __init__(self, w=0.0, h=0.0):
        ogl.PolygonShape.__init__(self)
        if w == 0.0:
            w = 60.0
        if h == 0.0:
            h = 60.0

        points = [ (0.0,    -h/2.0),
                   (w/2.0,  0.0),
                   (0.0,    h/2.0),
                   (-w/2.0, 0.0),
                   ]

        self.Create(points)


#----------------------------------------------------------------------

class RoundedRectangleShape(ogl.RectangleShape):
    def __init__(self, w=0.0, h=0.0):
        ogl.RectangleShape.__init__(self, w, h)
        self.SetCornerRadius(-0.3)


#----------------------------------------------------------------------

class CompositeDivisionShape(ogl.CompositeShape):
    def __init__(self, canvas):
        ogl.CompositeShape.__init__(self)

        self.SetCanvas(canvas)

        # create a division in the composite
        self.MakeContainer()

        # add a shape to the original division
        shape2 = ogl.RectangleShape(40, 60)
        self.GetDivisions()[0].AddChild(shape2)

        # now divide the division so we get 2
        self.GetDivisions()[0].Divide(wx.HORIZONTAL)

        # and add a shape to the second division (and move it to the
        # centre of the division)
        shape3 = ogl.CircleShape(40)
        shape3.SetBrush(wx.CYAN_BRUSH)
        self.GetDivisions()[1].AddChild(shape3)
        shape3.SetX(self.GetDivisions()[1].GetX())

        for division in self.GetDivisions():
            division.SetSensitivityFilter(0)
        
#----------------------------------------------------------------------

class CompositeShape(ogl.CompositeShape):
    def __init__(self, canvas):
        ogl.CompositeShape.__init__(self)

        self.SetCanvas(canvas)

        constraining_shape = ogl.RectangleShape(120, 100)
        constrained_shape1 = ogl.CircleShape(50)
        constrained_shape2 = ogl.RectangleShape(80, 20)

        constraining_shape.SetBrush(wx.BLUE_BRUSH)
        constrained_shape2.SetBrush(wx.RED_BRUSH)
        
        self.AddChild(constraining_shape)
        self.AddChild(constrained_shape1)
        self.AddChild(constrained_shape2)

        constraint = ogl.Constraint(ogl.CONSTRAINT_MIDALIGNED_BOTTOM, constraining_shape, [constrained_shape1, constrained_shape2])
        self.AddConstraint(constraint)
        self.Recompute()

        # If we don't do this, the shapes will be able to move on their
        # own, instead of moving the composite
        constraining_shape.SetDraggable(False)
        constrained_shape1.SetDraggable(False)
        constrained_shape2.SetDraggable(False)

        # If we don't do this the shape will take all left-clicks for itself
        constraining_shape.SetSensitivityFilter(0)

        
#----------------------------------------------------------------------

class DividedShape(ogl.DividedShape):
    def __init__(self, width, height, canvas):
        ogl.DividedShape.__init__(self, width, height)

        region1 = ogl.ShapeRegion()
        region1.SetText('DividedShape')
        region1.SetProportions(0.0, 0.2)
        region1.SetFormatMode(ogl.FORMAT_CENTRE_HORIZ)
        self.AddRegion(region1)

        region2 = ogl.ShapeRegion()
        region2.SetText('This is Region number two.')
        region2.SetProportions(0.0, 0.3)
        region2.SetFormatMode(ogl.FORMAT_CENTRE_HORIZ|ogl.FORMAT_CENTRE_VERT)
        self.AddRegion(region2)

        region3 = ogl.ShapeRegion()
        region3.SetText('Region 3\nwith embedded\nline breaks')
        region3.SetProportions(0.0, 0.5)
        region3.SetFormatMode(ogl.FORMAT_NONE)
        self.AddRegion(region3)

        self.SetRegionSizes()
        self.ReformatRegions(canvas)


    def ReformatRegions(self, canvas=None):
        rnum = 0

        if canvas is None:
            canvas = self.GetCanvas()

        dc = wx.ClientDC(canvas)  # used for measuring

        for region in self.GetRegions():
            text = region.GetText()
            self.FormatText(dc, text, rnum)
            rnum += 1


    def OnSizingEndDragLeft(self, pt, x, y, keys, attch):
        print "***", self
        ogl.DividedShape.OnSizingEndDragLeft(self, pt, x, y, keys, attch)
        self.SetRegionSizes()
        self.ReformatRegions()
        self.GetCanvas().Refresh()


#----------------------------------------------------------------------

class MyEvtHandler(ogl.ShapeEvtHandler):
    def __init__(self, log, frame):
        ogl.ShapeEvtHandler.__init__(self)
        self.log = log
        self.statbarFrame = frame

    def UpdateStatusBar(self, shape):
        x, y = shape.GetX(), shape.GetY()
        width, height = shape.GetBoundingBoxMax()
        self.statbarFrame.SetStatusText("Pos: (%d, %d)  Size: (%d, %d)" %
                                        (x, y, width, height))


    def OnLeftClick(self, x, y, keys=0, attachment=0):
        shape = self.GetShape()
        canvas = shape.GetCanvas()
        dc = wx.ClientDC(canvas)
        canvas.PrepareDC(dc)

        if shape.Selected():
            shape.Select(False, dc)
            canvas.Redraw(dc)
        else:
            redraw = False
            shapeList = canvas.GetDiagram().GetShapeList()
            toUnselect = []

            for s in shapeList:
                if s.Selected():
                    # If we unselect it now then some of the objects in
                    # shapeList will become invalid (the control points are
                    # shapes too!) and bad things will happen...
                    toUnselect.append(s)

            shape.Select(True, dc)

            if toUnselect:
                for s in toUnselect:
                    s.Select(False, dc)

                canvas.Redraw(dc)

        self.UpdateStatusBar(shape)


    def OnEndDragLeft(self, x, y, keys=0, attachment=0):
        shape = self.GetShape()
        ogl.ShapeEvtHandler.OnEndDragLeft(self, x, y, keys, attachment)

        if not shape.Selected():
            self.OnLeftClick(x, y, keys, attachment)

        self.UpdateStatusBar(shape)


    def OnSizingEndDragLeft(self, pt, x, y, keys, attch):
        ogl.ShapeEvtHandler.OnSizingEndDragLeft(self, pt, x, y, keys, attch)
        self.UpdateStatusBar(self.GetShape())


    def OnMovePost(self, dc, x, y, oldX, oldY, display):
        ogl.ShapeEvtHandler.OnMovePost(self, dc, x, y, oldX, oldY, display)
        self.UpdateStatusBar(self.GetShape())


    def OnRightClick(self, *dontcare):
        self.log.WriteText("%s\n" % self.GetShape())


#----------------------------------------------------------------------

class TestWindow(ogl.ShapeCanvas):
    def __init__(self, parent, log, frame):
        ogl.ShapeCanvas.__init__(self, parent)

        maxWidth  = 1000
        maxHeight = 1000
        self.SetScrollbars(20, 20, maxWidth/20, maxHeight/20)

        self.log = log
        self.frame = frame
        self.SetBackgroundColour("LIGHT BLUE") #wx.WHITE)
        self.diagram = ogl.Diagram()
        self.SetDiagram(self.diagram)
        self.diagram.SetCanvas(self)
        self.shapes = []
        self.save_gdi = []

        rRectBrush = wx.Brush("MEDIUM TURQUOISE", wx.SOLID)
        dsBrush = wx.Brush("WHEAT", wx.SOLID)

        self.MyAddShape(
            CompositeDivisionShape(self), 
            270, 310, wx.BLACK_PEN, wx.BLUE_BRUSH, "Division"
            )
        
        self.MyAddShape(
            CompositeShape(self), 
            100, 260, wx.BLACK_PEN, wx.RED_BRUSH, "Composite"
            )
        
        self.MyAddShape(
            ogl.CircleShape(80), 
            75, 110, wx.Pen(wx.BLUE, 3), wx.GREEN_BRUSH, "Circle"
            )
            
        self.MyAddShape(
            ogl.TextShape(120, 45), 
            160, 35, wx.GREEN_PEN, wx.LIGHT_GREY_BRUSH, "OGL is now a\npure Python lib!"
            )

        self.MyAddShape(
            ogl.RectangleShape(85, 50), 
            305, 60, wx.BLACK_PEN, wx.LIGHT_GREY_BRUSH, "Rectangle"
            )

        self.MyAddShape(
            DrawnShape(),
            500, 80, wx.BLACK_PEN, wx.BLACK_BRUSH, "DrawnShape"
            )

        ds = self.MyAddShape(
            DividedShape(140, 150, self), 
            520, 265, wx.BLACK_PEN, dsBrush, ''
            )

        self.MyAddShape(
            DiamondShape(90, 90), 
            355, 260, wx.Pen(wx.BLUE, 3, wx.DOT), wx.RED_BRUSH, "Polygon"
            )
            
        self.MyAddShape(
            RoundedRectangleShape(95, 70), 
            345, 145, wx.Pen(wx.RED, 2), rRectBrush, "Rounded Rect"
            )

        bmp = images.getTest2Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)

        s = ogl.BitmapShape()
        s.SetBitmap(bmp)
        self.MyAddShape(s, 225, 130, None, None, "Bitmap")

        dc = wx.ClientDC(self)
        self.PrepareDC(dc)

        for x in range(len(self.shapes)):
            fromShape = self.shapes[x]
            if x+1 == len(self.shapes):
                toShape = self.shapes[0]
            else:
                toShape = self.shapes[x+1]

            line = ogl.LineShape()
            line.SetCanvas(self)
            line.SetPen(wx.BLACK_PEN)
            line.SetBrush(wx.BLACK_BRUSH)
            line.AddArrow(ogl.ARROW_ARROW)
            line.MakeLineControlPoints(2)
            fromShape.AddLine(line, toShape)
            self.diagram.AddShape(line)
            line.Show(True)


    def MyAddShape(self, shape, x, y, pen, brush, text):
        # Composites have to be moved for all children to get in place
        if isinstance(shape, ogl.CompositeShape):
            dc = wx.ClientDC(self)
            self.PrepareDC(dc)
            shape.Move(dc, x, y)
        else:
            shape.SetDraggable(True, True)
        shape.SetCanvas(self)
        shape.SetX(x)
        shape.SetY(y)
        if pen:    shape.SetPen(pen)
        if brush:  shape.SetBrush(brush)
        if text:
            for line in text.split('\n'):
                shape.AddText(line)
        #shape.SetShadowMode(ogl.SHADOW_RIGHT)
        self.diagram.AddShape(shape)
        shape.Show(True)

        evthandler = MyEvtHandler(self.log, self.frame)
        evthandler.SetShape(shape)
        evthandler.SetPreviousHandler(shape.GetEventHandler())
        shape.SetEventHandler(evthandler)

        self.shapes.append(shape)
        return shape


    def OnBeginDragLeft(self, x, y, keys):
        self.log.write("OnBeginDragLeft: %s, %s, %s\n" % (x, y, keys))

    def OnEndDragLeft(self, x, y, keys):
        self.log.write("OnEndDragLeft: %s, %s, %s\n" % (x, y, keys))


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    # This creates some pens and brushes that the OGL library uses.
    # It should be called after the app object has been created, but
    # before OGL is used.
    ogl.OGLInitialize()

    win = TestWindow(nb, log, frame)
    return win
    
#----------------------------------------------------------------------


overview = """<html><body>
<h2>Object Graphics Library</h2>

The Object Graphics Library is a library supporting the creation and
manipulation of simple and complex graphic images on a canvas.

<p>The OGL library was originally written in C++ and provided to
wxPython via an extension module wrapper as is most of the rest of
wxPython.  The code has now been ported to Python (with many thanks to
Pierre Hjälm!) in order to make it be more easily maintainable and
less likely to get rusty because nobody cares about the C++ lib any
more.

<p>The Python version should be mostly drop-in compatible with the
wrapped C++ version, except for the location of the package
(wx.lib.ogl instead of wx.ogl) and that the base class methods are
called the normal Python way (superclass.Method(self, ...)) instead of the
hacky way that had to be done to support overloaded methods with the
old SWIG (self.base_Method(...))


"""

if __name__ == '__main__':
    import sys, os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

