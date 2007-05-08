
from __future__ import division

try:
    import numpy as N
except ImportError:
    raise ImportError("I could not import numpy")

from time import clock
import wx

from Utilities import BBox


## A global variable to hold the Pixels per inch that wxWindows thinks is in use
## This is used for scaling fonts.
## This can't be computed on module __init__, because a wx.App might not have initialized yet.
global FontScale

## Custom Exceptions:

class FloatCanvasError(Exception):
    pass

## Create all the mouse events
EVT_FC_ENTER_WINDOW = wx.NewEventType()
EVT_FC_LEAVE_WINDOW = wx.NewEventType()
EVT_FC_LEFT_DOWN = wx.NewEventType()
EVT_FC_LEFT_UP  = wx.NewEventType()
EVT_FC_LEFT_DCLICK = wx.NewEventType()
EVT_FC_MIDDLE_DOWN = wx.NewEventType()
EVT_FC_MIDDLE_UP = wx.NewEventType()
EVT_FC_MIDDLE_DCLICK = wx.NewEventType()
EVT_FC_RIGHT_DOWN = wx.NewEventType()
EVT_FC_RIGHT_UP = wx.NewEventType()
EVT_FC_RIGHT_DCLICK = wx.NewEventType()
EVT_FC_MOTION = wx.NewEventType()
EVT_FC_MOUSEWHEEL = wx.NewEventType()
## these two are for the hit-test stuff, I never make them real Events
## fixme: could I use the PyEventBinder for the Object events too?
EVT_FC_ENTER_OBJECT = wx.NewEventType()
EVT_FC_LEAVE_OBJECT = wx.NewEventType()

##Create all mouse event binding objects
EVT_LEFT_DOWN = wx.PyEventBinder(EVT_FC_LEFT_DOWN)
EVT_LEFT_UP = wx.PyEventBinder(EVT_FC_LEFT_UP)
EVT_LEFT_DCLICK = wx.PyEventBinder(EVT_FC_LEFT_DCLICK)
EVT_MIDDLE_DOWN = wx.PyEventBinder(EVT_FC_MIDDLE_DOWN)
EVT_MIDDLE_UP = wx.PyEventBinder(EVT_FC_MIDDLE_UP)
EVT_MIDDLE_DCLICK = wx.PyEventBinder(EVT_FC_MIDDLE_DCLICK)
EVT_RIGHT_DOWN = wx.PyEventBinder(EVT_FC_RIGHT_DOWN)
EVT_RIGHT_UP = wx.PyEventBinder(EVT_FC_RIGHT_UP)
EVT_RIGHT_DCLICK = wx.PyEventBinder(EVT_FC_RIGHT_DCLICK)
EVT_MOTION = wx.PyEventBinder(EVT_FC_MOTION)
EVT_ENTER_WINDOW = wx.PyEventBinder(EVT_FC_ENTER_WINDOW)
EVT_LEAVE_WINDOW = wx.PyEventBinder(EVT_FC_LEAVE_WINDOW)
EVT_MOUSEWHEEL = wx.PyEventBinder(EVT_FC_MOUSEWHEEL)

class _MouseEvent(wx.PyCommandEvent):

    """!

    This event class takes a regular wxWindows mouse event as a parameter,
    and wraps it so that there is access to all the original methods. This
    is similar to subclassing, but you can't subclass a wxWindows event

    The goal is to be able to it just like a regular mouse event.

    It adds the method:

    GetCoords() , which returns and (x,y) tuple in world coordinates.

    Another difference is that it is a CommandEvent, which propagates up
    the window hierarchy until it is handled.

    """

    def __init__(self, EventType, NativeEvent, WinID, Coords = None):
        wx.PyCommandEvent.__init__(self)

        self.SetEventType( EventType )
        self._NativeEvent = NativeEvent
        self.Coords = Coords

    def GetCoords(self):
        return self.Coords

    def __getattr__(self, name):
        return getattr(self._NativeEvent, name)

def _cycleidxs(indexcount, maxvalue, step):

    """!
    Utility function used by _colorGenerator

    """
    
    if indexcount == 0:
        yield ()
    else:
        for idx in xrange(0, maxvalue, step):
            for tail in _cycleidxs(indexcount - 1, maxvalue, step):
                yield (idx, ) + tail

def _colorGenerator():

    """!

    Generates a series of unique colors used to do hit-tests with the Hit
    Test bitmap
    """

    depth = wx.GetDisplayDepth()
##    ##there have been problems with 16 bbp displays, to I'm disabling this for now.
##    if depth == 16:
##        print "Warning: There have been problems with hit-testing on 16bbp displays"
##        step = 8
    if depth >= 24:
        step = 1
    else:
        msg= ["ColorGenerator does not work with depth = %s" % depth]
        msg.append("It is required for hit testing -- binding events to mouse")
        msg.append("actions on objects on the Canvas.")
        msg.append("Please set your display to 24bit")
        msg.append("Alternatively, the code could be adapted to 16 bit if that's required")
        raise FloatCanvasError(msg)
    return _cycleidxs(indexcount=3, maxvalue=256, step=step)

class DrawObject:
    """!
    This is the base class for all the objects that can be drawn.

    One must subclass from this (and an assortment of Mixins) to create
    a new DrawObject.

      \note This class contain a series of static dictionaries:

      * BrushList
      * PenList
      * FillStyleList
      * LineStyleList

      Is this still necessary?

    """

    def __init__(self, InForeground  = False, IsVisible = True):
        """! \param InForeground (bool)
             \param IsVisible (Bool)
        """
        self.InForeground = InForeground

        self._Canvas = None

        self.HitColor = None
        self.CallBackFuncs = {}

        ## these are the defaults
        self.HitAble = False
        self.HitLine = True
        self.HitFill = True
        self.MinHitLineWidth = 3
        self.HitLineWidth = 3 ## this gets re-set by the subclasses if necessary

        self.Brush = None
        self.Pen = None

        self.FillStyle = "Solid"

        self.Visible = IsVisible

    # I pre-define all these as class variables to provide an easier
    # interface, and perhaps speed things up by caching all the Pens
    # and Brushes, although that may not help, as I think wx now
    # does that on it's own. Send me a note if you know!

    BrushList = {
            ( None,"Transparent")  : wx.TRANSPARENT_BRUSH,
            ("Blue","Solid")       : wx.BLUE_BRUSH,
            ("Green","Solid")      : wx.GREEN_BRUSH,
            ("White","Solid")      : wx.WHITE_BRUSH,
            ("Black","Solid")      : wx.BLACK_BRUSH,
            ("Grey","Solid")       : wx.GREY_BRUSH,
            ("MediumGrey","Solid") : wx.MEDIUM_GREY_BRUSH,
            ("LightGrey","Solid")  : wx.LIGHT_GREY_BRUSH,
            ("Cyan","Solid")       : wx.CYAN_BRUSH,
            ("Red","Solid")        : wx.RED_BRUSH
                    }
    PenList = {
            (None,"Transparent",1)   : wx.TRANSPARENT_PEN,
            ("Green","Solid",1)      : wx.GREEN_PEN,
            ("White","Solid",1)      : wx.WHITE_PEN,
            ("Black","Solid",1)      : wx.BLACK_PEN,
            ("Grey","Solid",1)       : wx.GREY_PEN,
            ("MediumGrey","Solid",1) : wx.MEDIUM_GREY_PEN,
            ("LightGrey","Solid",1)  : wx.LIGHT_GREY_PEN,
            ("Cyan","Solid",1)       : wx.CYAN_PEN,
            ("Red","Solid",1)        : wx.RED_PEN
            }

    FillStyleList = {
            "Transparent"    : wx.TRANSPARENT,
            "Solid"          : wx.SOLID,
            "BiDiagonalHatch": wx.BDIAGONAL_HATCH,
            "CrossDiagHatch" : wx.CROSSDIAG_HATCH,
            "FDiagonal_Hatch": wx.FDIAGONAL_HATCH,
            "CrossHatch"     : wx.CROSS_HATCH,
            "HorizontalHatch": wx.HORIZONTAL_HATCH,
            "VerticalHatch"  : wx.VERTICAL_HATCH
            }

    LineStyleList = {
            "Solid"      : wx.SOLID,
            "Transparent": wx.TRANSPARENT,
            "Dot"        : wx.DOT,
            "LongDash"   : wx.LONG_DASH,
            "ShortDash"  : wx.SHORT_DASH,
            "DotDash"    : wx.DOT_DASH,
            }

#    def BBFromPoints(self, Points):
#        """!
#        Calculates a Bounding box from a set of points (NX2 array of coordinates)
#        \param Points (array?) 
#        """
#
#        ## fixme: this could be done with array.min() and vstack() in numpy.
#        ##        This could use the Utilities.BBox module now.
#        #return N.array( (N.minimum.reduce(Points),
#        #                 N.maximum.reduce(Points) ),
#        #                )
#        return BBox.fromPoints(Points)

    def Bind(self, Event, CallBackFun):
        self.CallBackFuncs[Event] = CallBackFun
        self.HitAble = True
        self._Canvas.UseHitTest = True
        if self.InForeground and self._Canvas._ForegroundHTBitmap is None:
            self._Canvas.MakeNewForegroundHTBitmap()
        elif self._Canvas._HTBitmap is None:
            self._Canvas.MakeNewHTBitmap()
        if not self.HitColor:
            if not self._Canvas.HitColorGenerator:
                self._Canvas.HitColorGenerator = _colorGenerator()
                self._Canvas.HitColorGenerator.next() # first call to prevent the background color from being used.
            self.HitColor = self._Canvas.HitColorGenerator.next()
            self.SetHitPen(self.HitColor,self.HitLineWidth)
            self.SetHitBrush(self.HitColor)
        # put the object in the hit dict, indexed by it's color
        if not self._Canvas.HitDict:
            self._Canvas.MakeHitDict()
        self._Canvas.HitDict[Event][self.HitColor] = (self) # put the object in the hit dict, indexed by it's color

    def UnBindAll(self):
        ## fixme: this only removes one from each list, there could be more.
        if self._Canvas.HitDict:
            for List in self._Canvas.HitDict.itervalues():
                try:
                   List.remove(self)
                except ValueError:
                    pass
        self.HitAble = False


    def SetBrush(self,FillColor,FillStyle):
        if FillColor is None or FillStyle is None:
            self.Brush = wx.TRANSPARENT_BRUSH
            ##fixme: should I really re-set the style?
            self.FillStyle = "Transparent"
        else:
            self.Brush = self.BrushList.setdefault( (FillColor,FillStyle),  wx.Brush(FillColor,self.FillStyleList[FillStyle] ) )

    def SetPen(self,LineColor,LineStyle,LineWidth):
        if (LineColor is None) or (LineStyle is None):
            self.Pen = wx.TRANSPARENT_PEN
            self.LineStyle = 'Transparent'
        else:
             self.Pen = self.PenList.setdefault( (LineColor,LineStyle,LineWidth),  wx.Pen(LineColor,LineWidth,self.LineStyleList[LineStyle]) )

    def SetHitBrush(self,HitColor):
        if not self.HitFill:
            self.HitBrush = wx.TRANSPARENT_BRUSH
        else:
            self.HitBrush = self.BrushList.setdefault( (HitColor,"solid"),  wx.Brush(HitColor,self.FillStyleList["Solid"] ) )

    def SetHitPen(self,HitColor,LineWidth):
        if not self.HitLine:
            self.HitPen = wx.TRANSPARENT_PEN
        else:
            self.HitPen = self.PenList.setdefault( (HitColor, "solid", self.HitLineWidth),  wx.Pen(HitColor, self.HitLineWidth, self.LineStyleList["Solid"]) )

    ## Just to make sure that they will always be there
    ##   the appropriate ones should be overridden in the subclasses
    def SetColor(self, Color):
        pass
    def SetLineColor(self, LineColor):
        pass
    def SetLineStyle(self, LineStyle):
        pass
    def SetLineWidth(self, LineWidth):
        pass
    def SetFillColor(self, FillColor):
        pass
    def SetFillStyle(self, FillStyle):
        pass

    def PutInBackground(self):
        if self._Canvas and self.InForeground:
            self._Canvas._ForeDrawList.remove(self)
            self._Canvas._DrawList.append(self)
            self._Canvas._BackgroundDirty = True
            self.InForeground = False

    def PutInForeground(self):
        if self._Canvas and (not self.InForeground):
            self._Canvas._ForeDrawList.append(self)
            self._Canvas._DrawList.remove(self)
            self._Canvas._BackgroundDirty = True
            self.InForeground = True

    def Hide(self):
        """! \brief Make an object hidden.
        """
        self.Visible = False

    def Show(self):
        """! \brief Make an object visible on the canvas.
        """
        self.Visible = True

class Group(DrawObject): 
    """
    A group of other FloatCanvas Objects
    
    Not all DrawObject methods may apply here. In particular, you can't Bind events to a group.
    
    Note that if an object is in more than one group, it will get drawn more than once.
    
    """

    def __init__(self, ObjectList=[], InForeground  = False, IsVisible = True):
        self.ObjectList = list(ObjectList)
        DrawObject.__init__(self, InForeground, IsVisible)
        self.CalcBoundingBox()

    def AddObject(self, obj):
        self.ObjectList.append(obj)
        self.BoundingBox.Merge(obj.BoundingBox)

    def AddObjects(self, Objects):
        for o in Objects:
            self.AddObject(o)
            
    def CalcBoundingBox(self):
        if self.ObjectList:
            BB = BBox.asBBox(self.ObjectList[0].BoundingBox)
            for obj in self.ObjectList[1:]:
                BB.Merge(obj.BoundingBox)
        else:
            BB = None
        self.BoundingBox = BB

    def SetColor(self, Color):
        for o in self.ObjectList:
            o.SetColor(Color)
    def SetLineColor(self, Color):
        for o in self.ObjectList:
            o.SetLineColor(Color)
    def SetLineStyle(self, LineStyle):
        for o in self.ObjectList:
            o.SetLineStyle(LineStyle)
    def SetLineWidth(self, LineWidth):
        for o in self.ObjectList:
            o.SetLineWidth(LineWidth)
    def SetFillColor(self, Color):
        for o in self.ObjectList:
            o.SetFillColor(Color)
    def SetFillStyle(self, FillStyle):
        for o in self.ObjectList:
            o.SetFillStyle(FillStyle)
    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel = None, HTdc=None):
        for obj in self.ObjectList:
            obj._Draw(dc, WorldToPixel, ScaleWorldToPixel, HTdc)
            

class ColorOnlyMixin:
    """

    Mixin class for objects that have just one color, rather than a fill
    color and line color

    """

    def SetColor(self, Color):
        self.SetPen(Color,"Solid",1)
        self.SetBrush(Color,"Solid")

    SetFillColor = SetColor # Just to provide a consistant interface

class LineOnlyMixin:
    """

    Mixin class for objects that have just one color, rather than a fill
    color and line color

    """

    def SetLineColor(self, LineColor):
        self.LineColor = LineColor
        self.SetPen(LineColor,self.LineStyle,self.LineWidth)
    SetColor = SetLineColor# so that it will do somethign reasonable
    
    def SetLineStyle(self, LineStyle):
        self.LineStyle = LineStyle
        self.SetPen(self.LineColor,LineStyle,self.LineWidth)

    def SetLineWidth(self, LineWidth):
        self.LineWidth = LineWidth
        self.SetPen(self.LineColor,self.LineStyle,LineWidth)

class LineAndFillMixin(LineOnlyMixin):
    """

    Mixin class for objects that have both a line and a fill color and
    style.

    """
    def SetFillColor(self, FillColor):
        self.FillColor = FillColor
        self.SetBrush(FillColor, self.FillStyle)

    def SetFillStyle(self, FillStyle):
        self.FillStyle = FillStyle
        self.SetBrush(self.FillColor,FillStyle)

    def SetUpDraw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc):
        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
        return ( WorldToPixel(self.XY),
                 ScaleWorldToPixel(self.WH) )

class XYObjectMixin:
    """

    This is a mixin class that provides some methods suitable for use
    with objects that have a single (x,y) coordinate pair.

    """

    def Move(self, Delta ):
        """

        Move(Delta): moves the object by delta, where delta is a
        (dx,dy) pair. Ideally a Numpy array of shape (2,)

        """

        Delta = N.asarray(Delta, N.float)
        self.XY += Delta
        self.BoundingBox += Delta

        if self._Canvas:
            self._Canvas.BoundingBoxDirty = True

    def CalcBoundingBox(self):
        ## This may get overwritten in some subclasses
        self.BoundingBox = N.array( (self.XY, self.XY), N.float )
        self.BoundingBox = BBox.asBBox((self.XY, self.XY))

    def SetPoint(self, xy):
        xy = N.array(xy, N.float)
        xy.shape = (2,)

        self.XY = xy
        self.CalcBoundingBox()

        if self._Canvas:
            self._Canvas.BoundingBoxDirty = True

class PointsObjectMixin:
    """

    This is a mixin class that provides some methods suitable for use
    with objects that have a set of (x,y) coordinate pairs.

    """

    def Move(self, Delta):
        """
        Move(Delta): moves the object by delta, where delta is an (dx,
        dy) pair. Ideally a Numpy array of shape (2,)
        """
        
        Delta = N.asarray(Delta, N.float)
        Delta.shape = (2,)
        self.Points += Delta
        self.BoundingBox += Delta
        if self._Canvas:
            self._Canvas.BoundingBoxDirty = True

    def CalcBoundingBox(self):
        self.BoundingBox = BBox.fromPoints(self.Points)
        if self._Canvas:
            self._Canvas.BoundingBoxDirty = True

    def SetPoints(self, Points, copy = True):
        """
        Sets the coordinates of the points of the object to Points (NX2 array).

        By default, a copy is made, if copy is set to False, a reference
        is used, iff Points is a NumPy array of Floats. This allows you
        to change some or all of the points without making any copies.

        For example:

        Points = Object.Points
        Points += (5,10) # shifts the points 5 in the x dir, and 10 in the y dir.
        Object.SetPoints(Points, False) # Sets the points to the same array as it was

        """
        if copy:
            self.Points = N.array(Points, N.float)
            self.Points.shape = (-1,2) # Make sure it is a NX2 array, even if there is only one point
        else:
            self.Points = N.asarray(Points, N.float)
        self.CalcBoundingBox()


class Polygon(PointsObjectMixin, LineAndFillMixin, DrawObject):

    """

    The Polygon class takes a list of 2-tuples, or a NX2 NumPy array of
    point coordinates.  so that Points[N][0] is the x-coordinate of
    point N and Points[N][1] is the y-coordinate or Points[N,0] is the
    x-coordinate of point N and Points[N,1] is the y-coordinate for
    arrays.

    The other parameters specify various properties of the Polygon, and
    should be self explanatory.

    """
    def __init__(self,
                 Points,
                 LineColor = "Black",
                 LineStyle = "Solid",
                 LineWidth    = 1,
                 FillColor    = None,
                 FillStyle    = "Solid",
                 InForeground = False):
        DrawObject.__init__(self, InForeground)
        self.Points = N.array(Points ,N.float) # this DOES need to make a copy
        self.CalcBoundingBox()

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.HitLineWidth = max(LineWidth,self.MinHitLineWidth)

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel = None, HTdc=None):
        Points = WorldToPixel(self.Points)#.tolist()
        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        dc.DrawPolygon(Points)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawPolygon(Points)

class Line(PointsObjectMixin, LineOnlyMixin, DrawObject,):
    """

    The Line class takes a list of 2-tuples, or a NX2 NumPy Float array
    of point coordinates.

    It will draw a straight line if there are two points, and a polyline
    if there are more than two.

    """
    def __init__(self,Points,
                 LineColor = "Black",
                 LineStyle = "Solid",
                 LineWidth    = 1,
                 InForeground = False):
        DrawObject.__init__(self, InForeground)


        self.Points = N.array(Points,N.float)
        self.CalcBoundingBox()

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth

        self.SetPen(LineColor,LineStyle,LineWidth)

        self.HitLineWidth = max(LineWidth,self.MinHitLineWidth)


    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        Points = WorldToPixel(self.Points)
        dc.SetPen(self.Pen)
        dc.DrawLines(Points)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.DrawLines(Points)

class Spline(Line):
    def __init__(self, *args, **kwargs):
            Line.__init__(self, *args, **kwargs)

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        Points = WorldToPixel(self.Points)
        dc.SetPen(self.Pen)
        dc.DrawSpline(Points)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.DrawSpline(Points)


class Arrow(XYObjectMixin, LineOnlyMixin, DrawObject):
    """

    Arrow(XY, # coords of origin of arrow (x,y)
          Length, # length of arrow in pixels
          theta, # angle of arrow in degrees: zero is straight up
                 # +angle is to the right
          LineColor = "Black",
          LineStyle = "Solid",
          LineWidth    = 1,
          ArrowHeadSize = 4, # size of arrowhead in pixels
          ArrowHeadAngle = 45, # angle of arrow head in degrees
          InForeground = False):

    It will draw an arrow , starting at the point, (X,Y) pointing in
    direction, theta.


    """
    def __init__(self,
                 XY,
                 Length,
                 Direction,
                 LineColor = "Black",
                 LineStyle = "Solid",
                 LineWidth    = 2, # pixels
                 ArrowHeadSize = 8, # pixels
                 ArrowHeadAngle = 30, # degrees
                 InForeground = False):

        DrawObject.__init__(self, InForeground)

        self.XY = N.array(XY, N.float)
        self.XY.shape = (2,) # Make sure it is a length 2 vector
        self.Length = Length
        self.Direction = float(Direction)
        self.ArrowHeadSize = ArrowHeadSize
        self.ArrowHeadAngle = float(ArrowHeadAngle)

        self.CalcArrowPoints()
        self.CalcBoundingBox()

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth

        self.SetPen(LineColor,LineStyle,LineWidth)

        ##fixme: How should the HitTest be drawn?
        self.HitLineWidth = max(LineWidth,self.MinHitLineWidth)

    def SetDirection(self, Direction):
        self.Direction = float(Direction)
        self.CalcArrowPoints()

    def SetLength(self, Length):
        self.Length = Length
        self.CalcArrowPoints()

    def SetLengthDirection(self, Length, Direction):
        self.Direction = float(Direction)
        self.Length = Length
        self.CalcArrowPoints()

##    def CalcArrowPoints(self):
##        L = self.Length
##        S = self.ArrowHeadSize
##        phi = self.ArrowHeadAngle * N.pi / 360
##        theta = (self.Direction-90.0) * N.pi / 180
##        ArrowPoints = N.array( ( (0, L, L - S*N.cos(phi),L, L - S*N.cos(phi) ),
##                               (0, 0, S*N.sin(phi),    0, -S*N.sin(phi)    ) ),
##                             N.float )
##        RotationMatrix = N.array( ( ( N.cos(theta), -N.sin(theta) ),
##                                  ( N.sin(theta), N.cos(theta) ) ),
##                                N.float
##                                )
##        ArrowPoints = N.matrixmultiply(RotationMatrix, ArrowPoints)
##        self.ArrowPoints = N.transpose(ArrowPoints)

    def CalcArrowPoints(self):
        L = self.Length
        S = self.ArrowHeadSize
        phi = self.ArrowHeadAngle * N.pi / 360
        theta = (270 - self.Direction) * N.pi / 180
        AP = N.array( ( (0,0),
                        (0,0),
                        (N.cos(theta - phi), -N.sin(theta - phi) ),
                        (0,0),
                        (N.cos(theta + phi), -N.sin(theta + phi) ),
                        ), N.float )
        AP *= S
        shift = (-L*N.cos(theta), L*N.sin(theta) )
        AP[1:,:] += shift
        self.ArrowPoints = AP

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        dc.SetPen(self.Pen)
        xy = WorldToPixel(self.XY)
        ArrowPoints = xy + self.ArrowPoints
        dc.DrawLines(ArrowPoints)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.DrawLines(ArrowPoints)


class ArrowLine(PointsObjectMixin, LineOnlyMixin, DrawObject):
    """

    ArrowLine(Points, # coords of points
              LineColor = "Black",
              LineStyle = "Solid",
              LineWidth    = 1,
              ArrowHeadSize = 4, # in pixels
              ArrowHeadAngle = 45,
              InForeground = False):

    It will draw a set of arrows from point to point.

    It takes a list of 2-tuples, or a NX2 NumPy Float array
    of point coordinates.


    """

    def __init__(self,
                 Points,
                 LineColor = "Black",
                 LineStyle = "Solid",
                 LineWidth    = 1, # pixels
                 ArrowHeadSize = 8, # pixels
                 ArrowHeadAngle = 30, # degrees
                 InForeground = False):

        DrawObject.__init__(self, InForeground)

        self.Points = N.asarray(Points,N.float)
        self.Points.shape = (-1,2) # Make sure it is a NX2 array, even if there is only one point
        self.ArrowHeadSize = ArrowHeadSize
        self.ArrowHeadAngle = float(ArrowHeadAngle)

        self.CalcArrowPoints()
        self.CalcBoundingBox()

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth

        self.SetPen(LineColor,LineStyle,LineWidth)

        self.HitLineWidth = max(LineWidth,self.MinHitLineWidth)

    def CalcArrowPoints(self):
        S = self.ArrowHeadSize
        phi = self.ArrowHeadAngle * N.pi / 360
        Points = self.Points
        n = Points.shape[0]
        self.ArrowPoints = N.zeros((n-1, 3, 2), N.float)
        for i in xrange(n-1):
            dx, dy = self.Points[i] - self.Points[i+1]
            theta = N.arctan2(dy, dx)
            AP = N.array( (
                            (N.cos(theta - phi), -N.sin(theta-phi)),
                            (0,0),
                            (N.cos(theta + phi), -N.sin(theta + phi))
                            ),
                          N.float )
            self.ArrowPoints[i,:,:] = AP
        self.ArrowPoints *= S

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        Points = WorldToPixel(self.Points)
        ArrowPoints = Points[1:,N.newaxis,:] + self.ArrowPoints
        dc.SetPen(self.Pen)
        dc.DrawLines(Points)
        for arrow in ArrowPoints:
                dc.DrawLines(arrow)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.DrawLines(Points)
            for arrow in ArrowPoints:
                HTdc.DrawLines(arrow)


class PointSet(PointsObjectMixin, ColorOnlyMixin, DrawObject):
    """

    The PointSet class takes a list of 2-tuples, or a NX2 NumPy array of
    point coordinates.

    If Points is a sequence of tuples: Points[N][0] is the x-coordinate of
    point N and Points[N][1] is the y-coordinate.

    If Points is a NumPy array: Points[N,0] is the x-coordinate of point
    N and Points[N,1] is the y-coordinate for arrays.

    Each point will be drawn the same color and Diameter. The Diameter
    is in screen pixels, not world coordinates.

    The hit-test code does not distingish between the points, you will
    only know that one of the points got hit, not which one. You can use
    PointSet.FindClosestPoint(WorldPoint) to find out which one

    In the case of points, the HitLineWidth is used as diameter.

    """
    def __init__(self, Points, Color = "Black", Diameter =  1, InForeground = False):
        DrawObject.__init__(self,InForeground)

        self.Points = N.array(Points,N.float)
        self.Points.shape = (-1,2) # Make sure it is a NX2 array, even if there is only one point
        self.CalcBoundingBox()
        self.Diameter = Diameter

        self.HitLineWidth = min(self.MinHitLineWidth, Diameter)
        self.SetColor(Color)

    def SetDiameter(self,Diameter):
            self.Diameter = Diameter

    def FindClosestPoint(self, XY):
        """

        Returns the index of the closest point to the point, XY, given
        in World coordinates. It's essentially random which you get if
        there are more than one that are the same.

        This can be used to figure out which point got hit in a mouse
        binding callback, for instance. It's a lot faster that using a
        lot of separate points.

        """
        d = self.Points - XY
        return N.argmin(N.hypot(d[:,0],d[:,1]))


    def DrawD2(self, dc, Points):
        # A Little optimization for a diameter2 - point
        dc.DrawPointList(Points)
        dc.DrawPointList(Points + (1,0))
        dc.DrawPointList(Points + (0,1))
        dc.DrawPointList(Points + (1,1))

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        dc.SetPen(self.Pen)
        Points = WorldToPixel(self.Points)
        if self.Diameter <= 1:
            dc.DrawPointList(Points)
        elif self.Diameter <= 2:
            self.DrawD2(dc, Points)
        else:
            dc.SetBrush(self.Brush)
            radius = int(round(self.Diameter/2))
            ##fixme: I really should add a DrawCircleList to wxPython
            if len(Points) > 100:
                xy = Points
                xywh = N.concatenate((xy-radius, N.ones(xy.shape) * self.Diameter ), 1 )
                dc.DrawEllipseList(xywh)
            else:
                for xy in Points:
                    dc.DrawCircle(xy[0],xy[1], radius)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            if self.Diameter <= 1:
                HTdc.DrawPointList(Points)
            elif self.Diameter <= 2:
                self.DrawD2(HTdc, Points)
            else:
                if len(Points) > 100:
                    xy = Points
                    xywh = N.concatenate((xy-radius, N.ones(xy.shape) * self.Diameter ), 1 )
                    HTdc.DrawEllipseList(xywh)
                else:
                    for xy in Points:
                        HTdc.DrawCircle(xy[0],xy[1], radius)

class Point(XYObjectMixin, ColorOnlyMixin, DrawObject):
    """

    The Point class takes a 2-tuple, or a (2,) NumPy array of point
    coordinates.

    The Diameter is in screen points, not world coordinates, So the
    Bounding box is just the point, and doesn't include the Diameter.

    The HitLineWidth is used as diameter for the
    Hit Test.

    """
    def __init__(self, XY, Color = "Black", Diameter =  1, InForeground = False):
        DrawObject.__init__(self, InForeground)

        self.XY = N.array(XY, N.float)
        self.XY.shape = (2,) # Make sure it is a length 2 vector
        self.CalcBoundingBox()
        self.SetColor(Color)
        self.Diameter = Diameter

        self.HitLineWidth = self.MinHitLineWidth

    def SetDiameter(self,Diameter):
            self.Diameter = Diameter


    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        dc.SetPen(self.Pen)
        xy = WorldToPixel(self.XY)
        if self.Diameter <= 1:
            dc.DrawPoint(xy[0], xy[1])
        else:
            dc.SetBrush(self.Brush)
            radius = int(round(self.Diameter/2))
            dc.DrawCircle(xy[0],xy[1], radius)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            if self.Diameter <= 1:
                HTdc.DrawPoint(xy[0], xy[1])
            else:
                HTdc.SetBrush(self.HitBrush)
                HTdc.DrawCircle(xy[0],xy[1], radius)

class SquarePoint(XYObjectMixin, ColorOnlyMixin, DrawObject):
    """

    The SquarePoint class takes a 2-tuple, or a (2,) NumPy array of point
    coordinates. It produces a square dot, centered on Point

    The Size is in screen points, not world coordinates, so the
    Bounding box is just the point, and doesn't include the Size.

    The HitLineWidth is used as diameter for the
    Hit Test.

    """
    def __init__(self, Point, Color = "Black", Size =  4, InForeground = False):
        DrawObject.__init__(self, InForeground)

        self.XY = N.array(Point, N.float)
        self.XY.shape = (2,) # Make sure it is a length 2 vector
        self.CalcBoundingBox()
        self.SetColor(Color)
        self.Size = Size

        self.HitLineWidth = self.MinHitLineWidth

    def SetSize(self,Size):
            self.Size = Size

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        Size = self.Size
        dc.SetPen(self.Pen)
        xc,yc = WorldToPixel(self.XY)

        if self.Size <= 1:
            dc.DrawPoint(xc, yc)
        else:
            x = xc - Size/2.0
            y = yc - Size/2.0
            dc.SetBrush(self.Brush)
            dc.DrawRectangle(x, y, Size, Size)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            if self.Size <= 1:
                HTdc.DrawPoint(xc, xc)
            else:
                HTdc.SetBrush(self.HitBrush)
                HTdc.DrawRectangle(x, y, Size, Size)

class RectEllipse(XYObjectMixin, LineAndFillMixin, DrawObject):
    def __init__(self, XY, WH,
                 LineColor = "Black",
                 LineStyle = "Solid",
                 LineWidth    = 1,
                 FillColor    = None,
                 FillStyle    = "Solid",
                 InForeground = False):

        DrawObject.__init__(self,InForeground)

        self.SetShape(XY, WH)
        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.HitLineWidth = max(LineWidth,self.MinHitLineWidth)

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def SetShape(self, XY, WH):
        self.XY = N.array( XY, N.float)
        self.XY.shape = (2,)
        self.WH = N.array( WH, N.float)
        self.WH.shape = (2,)
        self.CalcBoundingBox()


    def CalcBoundingBox(self):
        # you need this in case Width or Height are negative
        corners = N.array((self.XY, (self.XY + self.WH) ), N.float)
        self.BoundingBox = BBox.fromPoints(corners)
        if self._Canvas:
            self._Canvas.BoundingBoxDirty = True


class Rectangle(RectEllipse):

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        ( XY, WH ) = self.SetUpDraw(dc,
                                    WorldToPixel,
                                    ScaleWorldToPixel,
                                    HTdc)
        dc.DrawRectanglePointSize(XY, WH)
        if HTdc and self.HitAble:
            HTdc.DrawRectanglePointSize(XY, WH)



class Ellipse(RectEllipse):

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        ( XY, WH ) = self.SetUpDraw(dc,
                                    WorldToPixel,
                                    ScaleWorldToPixel,
                                    HTdc)
        dc.DrawEllipsePointSize(XY, WH)
        if HTdc and self.HitAble:
            HTdc.DrawEllipsePointSize(XY, WH)

class Circle(Ellipse):
    ## fixme: this should probably be use the DC.DrawCircle!
    def __init__(self, XY, Diameter, **kwargs):
        self.Center = N.array(XY, N.float)
        Diameter = float(Diameter)
        RectEllipse.__init__(self ,
                             self.Center - Diameter/2.0,
                             (Diameter, Diameter),
                             **kwargs)

    def SetDiameter(self, Diameter):
        Diameter = float(Diameter)
        XY = self.Center - (Diameter/2.0)
        self.SetShape(XY,
                      (Diameter, Diameter)
                      )

class TextObjectMixin(XYObjectMixin):
    """

    A mix in class that holds attributes and methods that are needed by
    the Text objects

    """

    ## I'm caching fonts, because on GTK, getting a new font can take a
    ## while. However, it gets cleared after every full draw as hanging
    ## on to a bunch of large fonts takes a massive amount of memory.

    FontList = {}

    LayoutFontSize = 16 # font size used for calculating layout

    def SetFont(self, Size, Family, Style, Weight, Underlined, FaceName):
        self.Font = self.FontList.setdefault( (Size,
                                               Family,
                                               Style,
                                               Weight,
                                               Underlined,
                                               FaceName),
                                               #wx.FontFromPixelSize((0.45*Size,Size), # this seemed to give a decent height/width ratio on Windows
                                               wx.Font(Size, 
                                                       Family,
                                                       Style,
                                                       Weight,
                                                       Underlined,
                                                       FaceName) )

    def SetColor(self, Color):
        self.Color = Color

    def SetBackgroundColor(self, BackgroundColor):
        self.BackgroundColor = BackgroundColor

    def SetText(self, String):
        """
        Re-sets the text displayed by the object

        In the case of the ScaledTextBox, it will re-do the layout as appropriate

        Note: only tested with the ScaledTextBox

        """

        self.String = String
        self.LayoutText()

    def LayoutText(self):
        """
        A dummy method to re-do the layout of the text.

        A derived object needs to override this if required.

        """
        pass

    ## store the function that shift the coords for drawing text. The
    ## "c" parameter is the correction for world coordinates, rather
    ## than pixel coords as the y axis is reversed
    ## pad is the extra space around the text
    ## if world = 1, the vertical shift is done in y-up coordinates
    ShiftFunDict = {'tl': lambda x, y, w, h, world=0, pad=0: (x + pad,     y + pad - 2*world*pad),
                    'tc': lambda x, y, w, h, world=0, pad=0: (x - w/2,     y + pad - 2*world*pad),
                    'tr': lambda x, y, w, h, world=0, pad=0: (x - w - pad, y + pad - 2*world*pad),
                    'cl': lambda x, y, w, h, world=0, pad=0: (x + pad,     y - h/2 + world*h),
                    'cc': lambda x, y, w, h, world=0, pad=0: (x - w/2,     y - h/2 + world*h),
                    'cr': lambda x, y, w, h, world=0, pad=0: (x - w - pad, y - h/2 + world*h),
                    'bl': lambda x, y, w, h, world=0, pad=0: (x + pad,     y - h + 2*world*h - pad + world*2*pad) ,
                    'bc': lambda x, y, w, h, world=0, pad=0: (x - w/2,     y - h + 2*world*h - pad + world*2*pad) ,
                    'br': lambda x, y, w, h, world=0, pad=0: (x - w - pad, y - h + 2*world*h - pad + world*2*pad)}

class Text(TextObjectMixin, DrawObject, ):
    """
    This class creates a text object, placed at the coordinates,
    x,y. the "Position" argument is a two charactor string, indicating
    where in relation to the coordinates the string should be oriented.

    The first letter is: t, c, or b, for top, center and bottom The
    second letter is: l, c, or r, for left, center and right The
    position refers to the position relative to the text itself. It
    defaults to "tl" (top left).

    Size is the size of the font in pixels, or in points for printing
    (if it ever gets implimented). Those will be the same, If you assume
    72 PPI.

    Family:
        Font family, a generic way of referring to fonts without
        specifying actual facename. One of:
            wx.DEFAULT:  Chooses a default font.
            wx.DECORATIVE: A decorative font.
            wx.ROMAN: A formal, serif font.
            wx.SCRIPT: A handwriting font.
            wx.SWISS: A sans-serif font.
            wx.MODERN: A fixed pitch font.
        NOTE: these are only as good as the wxWindows defaults, which aren't so good.
    Style:
        One of wx.NORMAL, wx.SLANT and wx.ITALIC.
    Weight:
        One of wx.NORMAL, wx.LIGHT and wx.BOLD.
    Underlined:
        The value can be True or False. At present this may have an an
        effect on Windows only.

    Alternatively, you can set the kw arg: Font, to a wx.Font, and the
    above will be ignored.

    The size is fixed, and does not scale with the drawing.

    The hit-test is done on the entire text extent

    """

    def __init__(self,String, xy,
                 Size =  14,
                 Color = "Black",
                 BackgroundColor = None,
                 Family = wx.MODERN,
                 Style = wx.NORMAL,
                 Weight = wx.NORMAL,
                 Underlined = False,
                 Position = 'tl',
                 InForeground = False,
                 Font = None):

        DrawObject.__init__(self,InForeground)

        self.String = String
        # Input size in in Pixels, compute points size from FontScaleinfo.
        # fixme: for printing, we'll have to do something a little different
        self.Size = Size * FontScale

        self.Color = Color
        self.BackgroundColor = BackgroundColor

        if not Font:
            FaceName = ''
        else:
            FaceName           =  Font.GetFaceName()
            Family             =  Font.GetFamily()
            Size               =  Font.GetPointSize()
            Style              =  Font.GetStyle()
            Underlined         =  Font.GetUnderlined()
            Weight             =  Font.GetWeight()
        self.SetFont(Size, Family, Style, Weight, Underlined, FaceName)

        self.BoundingBox = BBox.asBBox((xy, xy))

        self.XY = N.asarray(xy)
        self.XY.shape = (2,)

        (self.TextWidth, self.TextHeight) = (None, None)
        self.ShiftFun = self.ShiftFunDict[Position]

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        XY = WorldToPixel(self.XY)
        dc.SetFont(self.Font)
        dc.SetTextForeground(self.Color)
        if self.BackgroundColor:
            dc.SetBackgroundMode(wx.SOLID)
            dc.SetTextBackground(self.BackgroundColor)
        else:
            dc.SetBackgroundMode(wx.TRANSPARENT)
        if self.TextWidth is None or self.TextHeight is None:
            (self.TextWidth, self.TextHeight) = dc.GetTextExtent(self.String)
        XY = self.ShiftFun(XY[0], XY[1], self.TextWidth, self.TextHeight)
        dc.DrawTextPoint(self.String, XY)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(XY, (self.TextWidth, self.TextHeight) )

class ScaledText(TextObjectMixin, DrawObject, ):
    """
    This class creates a text object that is scaled when zoomed.  It is
    placed at the coordinates, x,y. the "Position" argument is a two
    charactor string, indicating where in relation to the coordinates
    the string should be oriented.

    The first letter is: t, c, or b, for top, center and bottom The
    second letter is: l, c, or r, for left, center and right The
    position refers to the position relative to the text itself. It
    defaults to "tl" (top left).

    Size is the size of the font in world coordinates.

    Family:
        Font family, a generic way of referring to fonts without
        specifying actual facename. One of:
            wx.DEFAULT:  Chooses a default font.
            wx.DECORATI: A decorative font.
            wx.ROMAN: A formal, serif font.
            wx.SCRIPT: A handwriting font.
            wx.SWISS: A sans-serif font.
            wx.MODERN: A fixed pitch font.
        NOTE: these are only as good as the wxWindows defaults, which aren't so good.
    Style:
        One of wx.NORMAL, wx.SLANT and wx.ITALIC.
    Weight:
        One of wx.NORMAL, wx.LIGHT and wx.BOLD.
    Underlined:
        The value can be True or False. At present this may have an an
        effect on Windows only.

    Alternatively, you can set the kw arg: Font, to a wx.Font, and the
    above will be ignored. The size of the font you specify will be
    ignored, but the rest of its attributes will be preserved.

    The size will scale as the drawing is zoomed.

    Bugs/Limitations:

    As fonts are scaled, the do end up a little different, so you don't
    get exactly the same picture as you scale up and doen, but it's
    pretty darn close.

    On wxGTK1 on my Linux system, at least, using a font of over about
    3000 pts. brings the system to a halt. It's the Font Server using
    huge amounts of memory. My work around is to max the font size to
    3000 points, so it won't scale past there. GTK2 uses smarter font
    drawing, so that may not be an issue in future versions, so feel
    free to test. Another smarter way to do it would be to set a global
    zoom limit at that point.

    The hit-test is done on the entire text extent. This could be made
    optional, but I haven't gotten around to it.

    """

    def __init__(self,
                 String,
                 XY,
                 Size,
                 Color = "Black",
                 BackgroundColor = None,
                 Family = wx.MODERN,
                 Style = wx.NORMAL,
                 Weight = wx.NORMAL,
                 Underlined = False,
                 Position = 'tl',
                 Font = None,
                 InForeground = False):

        DrawObject.__init__(self,InForeground)

        self.String = String
        self.XY = N.array( XY, N.float)
        self.XY.shape = (2,)
        self.Size = Size
        self.Color = Color
        self.BackgroundColor = BackgroundColor
        self.Family = Family
        self.Style = Style
        self.Weight = Weight
        self.Underlined = Underlined
        if not Font:
            self.FaceName = ''
        else:
            self.FaceName           =  Font.GetFaceName()
            self.Family             =  Font.GetFamily()
            self.Style              =  Font.GetStyle()
            self.Underlined         =  Font.GetUnderlined()
            self.Weight             =  Font.GetWeight()

        # Experimental max font size value on wxGTK2: this works OK on
        # my system. If it's a lot  larger, there is a crash, with the
        # message:
        #
        # The application 'FloatCanvasDemo.py' lost its
        # connection to the display :0.0; most likely the X server was
        # shut down or you killed/destroyed the application.
        #
        # Windows and OS-X seem to be better behaved in this regard.
        # They may not draw it, but they don't crash either!
        self.MaxFontSize = 1000

        self.ShiftFun = self.ShiftFunDict[Position]

        self.CalcBoundingBox()

    def LayoutText(self):
        # This will be called when the text is re-set
        # nothing much to be done here
        self.CalcBoundingBox()

    def CalcBoundingBox(self):
        ## this isn't exact, as fonts don't scale exactly.
        dc = wx.MemoryDC()
        bitmap = wx.EmptyBitmap(1, 1)
        dc.SelectObject(bitmap) #wxMac needs a Bitmap selected for GetTextExtent to work.
        DrawingSize = 40 # pts This effectively determines the resolution that the BB is computed to.
        ScaleFactor = float(self.Size) / DrawingSize
        self.SetFont(DrawingSize, self.Family, self.Style, self.Weight, self.Underlined, self.FaceName)
        dc.SetFont(self.Font)
        (w,h) = dc.GetTextExtent(self.String)
        w = w * ScaleFactor
        h = h * ScaleFactor
        x, y = self.ShiftFun(self.XY[0], self.XY[1], w, h, world = 1)
        self.BoundingBox = BBox.asBBox(((x, y-h ),(x + w, y)))

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        (X,Y) = WorldToPixel( (self.XY) )

        # compute the font size:
        Size = abs( ScaleWorldToPixel( (self.Size, self.Size) )[1] ) # only need a y coordinate length
        ## Check to see if the font size is large enough to blow up the X font server
        ## If so, limit it. Would it be better just to not draw it?
        ## note that this limit is dependent on how much memory you have, etc.
        Size = min(Size, self.MaxFontSize)
        self.SetFont(Size, self.Family, self.Style, self.Weight, self.Underlined, self.FaceName)
        dc.SetFont(self.Font)
        dc.SetTextForeground(self.Color)
        if self.BackgroundColor:
            dc.SetBackgroundMode(wx.SOLID)
            dc.SetTextBackground(self.BackgroundColor)
        else:
            dc.SetBackgroundMode(wx.TRANSPARENT)
        (w,h) = dc.GetTextExtent(self.String)
        # compute the shift, and adjust the coordinates, if neccesary
        # This had to be put in here, because it changes with Zoom, as
        # fonts don't scale exactly.
        xy = self.ShiftFun(X, Y, w, h)

        dc.DrawTextPoint(self.String, xy)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(xy, (w, h) )

class ScaledTextBox(TextObjectMixin, DrawObject):
    """
    This class creates a TextBox object that is scaled when zoomed.  It is
    placed at the coordinates, x,y.

    If the Width parameter is defined, the text will be wrapped to the width given.

    A Box can be drawn around the text, be specifying:
    LineWidth and/or  FillColor

    A space(margin) can be put all the way around the text, be specifying:
    the PadSize argument in world coordinates.

    The spacing between lines can be adjusted with the:
    LineSpacing argument.

    The "Position" argument is a two character string, indicating where
    in relation to the coordinates the Box should be oriented.
    -The first letter is: t, c, or b, for top, center and bottom.
    -The second letter is: l, c, or r, for left, center and right The
    position refers to the position relative to the text itself. It
    defaults to "tl" (top left).

    Size is the size of the font in world coordinates.

    Family:
        Font family, a generic way of referring to fonts without
        specifying actual facename. One of:
            wx.DEFAULT:  Chooses a default font.
            wx.DECORATIVE: A decorative font.
            wx.ROMAN: A formal, serif font.
            wx.SCRIPT: A handwriting font.
            wx.SWISS: A sans-serif font.
            wx.MODERN: A fixed pitch font.
        NOTE: these are only as good as the wxWindows defaults, which aren't so good.
    Style:
        One of wx.NORMAL, wx.SLANT and wx.ITALIC.
    Weight:
        One of wx.NORMAL, wx.LIGHT and wx.BOLD.
    Underlined:
        The value can be True or False. At present this may have an an
        effect on Windows only.

    Alternatively, you can set the kw arg: Font, to a wx.Font, and the
    above will be ignored. The size of the font you specify will be
    ignored, but the rest of its attributes will be preserved.

    The size will scale as the drawing is zoomed.

    Bugs/Limitations:

    As fonts are scaled, they do end up a little different, so you don't
    get exactly the same picture as you scale up and down, but it's
    pretty darn close.

    On wxGTK1 on my Linux system, at least, using a font of over about
    1000 pts. brings the system to a halt. It's the Font Server using
    huge amounts of memory. My work around is to max the font size to
    1000 points, so it won't scale past there. GTK2 uses smarter font
    drawing, so that may not be an issue in future versions, so feel
    free to test. Another smarter way to do it would be to set a global
    zoom limit at that point.

    The hit-test is done on the entire box. This could be made
    optional, but I haven't gotten around to it.

    """

    def __init__(self, String,
                 Point,
                 Size,
                 Color = "Black",
                 BackgroundColor = None,
                 LineColor = 'Black',
                 LineStyle = 'Solid',
                 LineWidth = 1,
                 Width = None,
                 PadSize = None,
                 Family = wx.MODERN,
                 Style = wx.NORMAL,
                 Weight = wx.NORMAL,
                 Underlined = False,
                 Position = 'tl',
                 Alignment = "left",
                 Font = None,
                 LineSpacing = 1.0,
                 InForeground = False):

        DrawObject.__init__(self,InForeground)

        self.XY = N.array(Point, N.float)
        self.Size = Size
        self.Color = Color
        self.BackgroundColor = BackgroundColor
        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.Width = Width
        if PadSize is None: # the default is just a little bit of padding
            self.PadSize = Size/10.0
        else:
            self.PadSize = float(PadSize)
        self.Family = Family
        self.Style = Style
        self.Weight = Weight
        self.Underlined = Underlined
        self.Alignment = Alignment.lower()
        self.LineSpacing = float(LineSpacing)
        self.Position = Position

        if not Font:
            self.FaceName = ''
        else:
            self.FaceName           =  Font.GetFaceName()
            self.Family             =  Font.GetFamily()
            self.Style              =  Font.GetStyle()
            self.Underlined         =  Font.GetUnderlined()
            self.Weight             =  Font.GetWeight()

        # Experimental max font size value on wxGTK2: this works OK on
        # my system. If it's a lot  larger, there is a crash, with the
        # message:
        #
        # The application 'FloatCanvasDemo.py' lost its
        # connection to the display :0.0; most likely the X server was
        # shut down or you killed/destroyed the application.
        #
        # Windows and OS-X seem to be better behaved in this regard.
        # They may not draw it, but they don't crash either!

        self.MaxFontSize = 1000
        self.ShiftFun = self.ShiftFunDict[Position]

        self.String = String
        self.LayoutText()
        self.CalcBoundingBox()

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(BackgroundColor, "Solid")


    def WrapToWidth(self):
        dc = wx.MemoryDC()
        bitmap = wx.EmptyBitmap(1, 1)
        dc.SelectObject(bitmap) #wxMac needs a Bitmap selected for GetTextExtent to work.
        DrawingSize = self.LayoutFontSize # pts This effectively determines the resolution that the BB is computed to.
        ScaleFactor = float(self.Size) / DrawingSize
        Width = (self.Width - 2*self.PadSize) / ScaleFactor #Width to wrap to
        self.SetFont(DrawingSize, self.Family, self.Style, self.Weight, self.Underlined, self.FaceName)
        dc.SetFont(self.Font)
        NewStrings = []
        for s in self.Strings:
            #beginning = True
            text = s.split(" ")
            text.reverse()
            LineLength = 0
            NewText = text[-1]
            del text[-1]
            while text:
                w  = dc.GetTextExtent(' ' + text[-1])[0]
                if LineLength + w <= Width:
                    NewText += ' '
                    NewText += text[-1]
                    LineLength = dc.GetTextExtent(NewText)[0]
                else:
                    NewStrings.append(NewText)
                    NewText = text[-1]
                    LineLength = dc.GetTextExtent(text[-1])[0]
                del text[-1]
            NewStrings.append(NewText)
        self.Strings = NewStrings

    def ReWrap(self, Width):
        self.Width = Width
        self.LayoutText()

    def LayoutText(self):
        """

        Calculates the positions of the words of text.

        This isn't exact, as fonts don't scale exactly.
        To help this, the position of each individual word
        is stored separately, so that the general layout stays
        the same in world coordinates, as the fonts scale.

        """
        self.Strings = self.String.split("\n")
        if self.Width:
            self.WrapToWidth()

        dc = wx.MemoryDC()
        bitmap = wx.EmptyBitmap(1, 1)
        dc.SelectObject(bitmap) #wxMac needs a Bitmap selected for GetTextExtent to work.

        DrawingSize = self.LayoutFontSize # pts This effectively determines the resolution that the BB is computed to.
        ScaleFactor = float(self.Size) / DrawingSize

        self.SetFont(DrawingSize, self.Family, self.Style, self.Weight, self.Underlined, self.FaceName)
        dc.SetFont(self.Font)
        TextHeight = dc.GetTextExtent("X")[1]
        SpaceWidth = dc.GetTextExtent(" ")[0]
        LineHeight = TextHeight * self.LineSpacing

        LineWidths = N.zeros((len(self.Strings),), N.float)
        y = 0
        Words = []
        AllLinePoints = []

        for i, s in enumerate(self.Strings):
            LineWidths[i] = 0
            LineWords = s.split(" ")
            LinePoints = N.zeros((len(LineWords),2), N.float)
            for j, word in enumerate(LineWords):
                if j > 0:
                    LineWidths[i] += SpaceWidth
                Words.append(word)
                LinePoints[j] = (LineWidths[i], y)
                w = dc.GetTextExtent(word)[0]
                LineWidths[i] += w
            y -= LineHeight
            AllLinePoints.append(LinePoints)
        TextWidth = N.maximum.reduce(LineWidths)
        self.Words = Words

        if self.Width is None:
            BoxWidth = TextWidth * ScaleFactor + 2*self.PadSize
        else: # use the defined Width
            BoxWidth = self.Width
        Points = N.zeros((0,2), N.float)

        for i, LinePoints in enumerate(AllLinePoints):
            ## Scale to World Coords.
            LinePoints *= (ScaleFactor, ScaleFactor)
            if self.Alignment == 'left':
                LinePoints[:,0] += self.PadSize
            elif self.Alignment == 'center':
                LinePoints[:,0] += (BoxWidth - LineWidths[i]*ScaleFactor)/2.0
            elif self.Alignment == 'right':
                LinePoints[:,0] += (BoxWidth - LineWidths[i]*ScaleFactor-self.PadSize)
            Points = N.concatenate((Points, LinePoints))

        BoxHeight = -(Points[-1,1] - (TextHeight * ScaleFactor)) + 2*self.PadSize
        #(x,y) = self.ShiftFun(self.XY[0], self.XY[1], BoxWidth, BoxHeight, world=1)
        Points += (0, -self.PadSize)
        self.Points = Points
        self.BoxWidth = BoxWidth
        self.BoxHeight = BoxHeight
        self.CalcBoundingBox()

    def CalcBoundingBox(self):

        """

        Calculates the Bounding Box

        """

        w, h = self.BoxWidth, self.BoxHeight
        x, y = self.ShiftFun(self.XY[0], self.XY[1], w, h, world=1)
        self.BoundingBox = BBox.asBBox(((x, y-h ),(x + w, y)))

    def GetBoxRect(self):
        wh = (self.BoxWidth, self.BoxHeight)
        xy = (self.BoundingBox[0,0], self.BoundingBox[1,1])

        return (xy, wh)

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        xy, wh = self.GetBoxRect()

        Points = self.Points + xy
        Points = WorldToPixel(Points)
        xy = WorldToPixel(xy)
        wh = ScaleWorldToPixel(wh) * (1,-1)

        # compute the font size:
        Size = abs( ScaleWorldToPixel( (self.Size, self.Size) )[1] ) # only need a y coordinate length
        ## Check to see if the font size is large enough to blow up the X font server
        ## If so, limit it. Would it be better just to not draw it?
        ## note that this limit is dependent on how much memory you have, etc.
        Size = min(Size, self.MaxFontSize)

        self.SetFont(Size, self.Family, self.Style, self.Weight, self.Underlined, self.FaceName)
        dc.SetFont(self.Font)
        dc.SetTextForeground(self.Color)
        dc.SetBackgroundMode(wx.TRANSPARENT)

        # Draw The Box
        if (self.LineStyle and self.LineColor) or self.BackgroundColor:
            dc.SetBrush(self.Brush)
            dc.SetPen(self.Pen)
            dc.DrawRectanglePointSize(xy , wh)

        # Draw the Text
        dc.DrawTextList(self.Words, Points)

        # Draw the hit box.
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(xy, wh)

class Bitmap(TextObjectMixin, DrawObject, ):
    """
    This class creates a bitmap object, placed at the coordinates,
    x,y. the "Position" argument is a two charactor string, indicating
    where in relation to the coordinates the bitmap should be oriented.

    The first letter is: t, c, or b, for top, center and bottom The
    second letter is: l, c, or r, for left, center and right The
    position refers to the position relative to the text itself. It
    defaults to "tl" (top left).

    The size is fixed, and does not scale with the drawing.

    """

    def __init__(self,Bitmap,XY,
                 Position = 'tl',
                 InForeground = False):

        DrawObject.__init__(self,InForeground)

        if type(Bitmap) == wx._gdi.Bitmap:
            self.Bitmap = Bitmap
        elif type(Bitmap) == wx._core.Image:
            self.Bitmap = wx.BitmapFromImage(Bitmap)

        # Note the BB is just the point, as the size in World coordinates is not fixed
        self.BoundingBox = BBox.asBBox( (XY,XY) )

        self.XY = XY

        (self.Width, self.Height) = self.Bitmap.GetWidth(), self.Bitmap.GetHeight()
        self.ShiftFun = self.ShiftFunDict[Position]

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        XY = WorldToPixel(self.XY)
        XY = self.ShiftFun(XY[0], XY[1], self.Width, self.Height)
        dc.DrawBitmapPoint(self.Bitmap, XY, True)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(XY, (self.Width, self.Height) )

class ScaledBitmap(TextObjectMixin, DrawObject, ):
    """

    This class creates a bitmap object, placed at the coordinates, XY,
    of Height, H, in World coorsinates. The width is calculated from the
    aspect ratio of the bitmap.

    the "Position" argument is a two charactor string, indicating
    where in relation to the coordinates the bitmap should be oriented.

    The first letter is: t, c, or b, for top, center and bottom The
    second letter is: l, c, or r, for left, center and right The
    position refers to the position relative to the text itself. It
    defaults to "tl" (top left).

    The size scales with the drawing

    """

    def __init__(self,
                 Bitmap,
                 XY,
                 Height,
                 Position = 'tl',
                 InForeground = False):

        DrawObject.__init__(self,InForeground)

        if type(Bitmap) == wx._gdi.Bitmap:
            self.Image = Bitmap.ConvertToImage()
        elif type(Bitmap) == wx._core.Image:
            self.Image = Bitmap

        self.XY = XY
        self.Height = Height
        (self.bmpWidth, self.bmpHeight) = self.Image.GetWidth(), self.Image.GetHeight()
        self.Width = self.bmpWidth / self.bmpHeight * Height
        self.ShiftFun = self.ShiftFunDict[Position]
        self.CalcBoundingBox()
        self.ScaledBitmap = None
        self.ScaledHeight = None

    def CalcBoundingBox(self):
        ## this isn't exact, as fonts don't scale exactly.
        w, h = self.Width, self.Height
        x, y = self.ShiftFun(self.XY[0], self.XY[1], w, h, world = 1)
        self.BoundingBox = BBox.asBBox( ( (x, y-h ), (x + w, y) ) )


    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        XY = WorldToPixel(self.XY)
        H = ScaleWorldToPixel(self.Height)[0]
        W = H * (self.bmpWidth / self.bmpHeight)
        if (self.ScaledBitmap is None) or (H <> self.ScaledHeight) :
            self.ScaledHeight = H
            Img = self.Image.Scale(W, H)
            self.ScaledBitmap = wx.BitmapFromImage(Img)

        XY = self.ShiftFun(XY[0], XY[1], W, H)
        dc.DrawBitmapPoint(self.ScaledBitmap, XY, True)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(XY, (W, H) )

class ScaledBitmap2(TextObjectMixin, DrawObject, ):
    """

    An alternative scaled bitmap that only scaled the required amount of
    the main bitmap when zoomed in: EXPERIMENTAL!

    """

    def __init__(self,
                 Bitmap,
                 XY,
                 Height,
                 Width=None,
                 Position = 'tl',
                 InForeground = False):

        DrawObject.__init__(self,InForeground)

        if type(Bitmap) == wx._gdi.Bitmap:
            self.Image = Bitmap.ConvertToImage()
        elif type(Bitmap) == wx._core.Image:
            self.Image = Bitmap

        self.XY = N.array(XY, N.float)
        self.Height = Height
        (self.bmpWidth, self.bmpHeight) = self.Image.GetWidth(), self.Image.GetHeight()
        self.bmpWH = N.array((self.bmpWidth, self.bmpHeight), N.int32)
        ## fixme: this should all accommodate different scales for X and Y
        if Width is None:
            self.BmpScale = float(self.bmpHeight) / Height
            self.Width = self.bmpWidth / self.BmpScale
        self.WH = N.array((self.Width, Height), N.float)
        ##fixme: should this have a y = -1 to shift to y-up?
        self.BmpScale = self.bmpWH / self.WH

        print "bmpWH:", self.bmpWH
        print "Width, Height:", self.WH
        print "self.BmpScale", self.BmpScale
        self.ShiftFun = self.ShiftFunDict[Position]
        self.CalcBoundingBox()
        self.ScaledBitmap = None # cache of the last existing scaled bitmap

    def CalcBoundingBox(self):
        ## this isn't exact, as fonts don't scale exactly.
        w,h = self.Width, self.Height
        x, y = self.ShiftFun(self.XY[0], self.XY[1], w, h, world = 1)
        self.BoundingBox = BBox.asBBox( ((x, y-h ), (x + w, y)) )

    def WorldToBitmap(self, Pw):
        """
        computes bitmap coords from World coords
        """
        delta = Pw - self.XY
        Pb = delta * self.BmpScale
        Pb *= (1, -1) ##fixme: this may only works for Yup projection!
                      ##       and may only work for top left position

        return Pb.astype(N.int_)

    def _DrawEntireBitmap(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc):
        """
        this is pretty much the old code

        Scales and Draws the entire bitmap.

        """
        XY = WorldToPixel(self.XY)
        H = ScaleWorldToPixel(self.Height)[0]
        W = H * (self.bmpWidth / self.bmpHeight)
        if (self.ScaledBitmap is None) or (self.ScaledBitmap[0] != (0, 0, self.bmpWidth, self.bmpHeight, W, H) ):
        #if True: #fixme: (self.ScaledBitmap is None) or (H <> self.ScaledHeight) :
            self.ScaledHeight = H
            print "Scaling to:", W, H
            Img = self.Image.Scale(W, H)
            bmp = wx.BitmapFromImage(Img)
            self.ScaledBitmap = ((0, 0, self.bmpWidth, self.bmpHeight , W, H), bmp)# this defines the cached bitmap
        else:
            print "Using Cached bitmap"
            bmp = self.ScaledBitmap[1]
        XY = self.ShiftFun(XY[0], XY[1], W, H)
        dc.DrawBitmapPoint(bmp, XY, True)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(XY, (W, H) )

    def _DrawSubBitmap(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc):
        """
        Subsets just the part of the bitmap that is visible
        then scales and draws that.

        """
        BBworld = BBox.asBBox(self._Canvas.ViewPortBB)
        BBbitmap = BBox.fromPoints(self.WorldToBitmap(BBworld))

        XYs = WorldToPixel(self.XY)
        # figure out subimage:
        # fixme: this should be able to be done more succinctly!

        if BBbitmap[0,0] < 0:
            Xb = 0
        elif BBbitmap[0,0] >  self.bmpWH[0]: # off the bitmap
            Xb = 0
        else:
            Xb = BBbitmap[0,0]
            XYs[0] = 0 # draw at origin

        if BBbitmap[0,1] < 0:
            Yb = 0
        elif BBbitmap[0,1] >  self.bmpWH[1]: # off the bitmap
            Yb = 0
            ShouldDraw = False
        else:
            Yb = BBbitmap[0,1]
            XYs[1] = 0 # draw at origin

        if BBbitmap[1,0] < 0:
            #off the screen --  This should never happen!
            Wb = 0
        elif BBbitmap[1,0] > self.bmpWH[0]:
            Wb = self.bmpWH[0] - Xb
        else:
            Wb = BBbitmap[1,0] - Xb

        if BBbitmap[1,1] < 0:
            # off the screen --  This should never happen!
            Hb = 0
            ShouldDraw = False
        elif BBbitmap[1,1] > self.bmpWH[1]:
            Hb = self.bmpWH[1] - Yb
        else:
            Hb = BBbitmap[1,1] - Yb

        FullHeight = ScaleWorldToPixel(self.Height)[0]
        scale = FullHeight / self.bmpWH[1]
        Ws = int(scale * Wb + 0.5) # add the 0.5 to  round
        Hs = int(scale * Hb + 0.5)
        if (self.ScaledBitmap is None) or (self.ScaledBitmap[0] != (Xb, Yb, Wb, Hb, Ws, Ws) ):
            Img = self.Image.GetSubImage(wx.Rect(Xb, Yb, Wb, Hb))
            Img.Rescale(Ws, Hs)
            bmp = wx.BitmapFromImage(Img)
            self.ScaledBitmap = ((Xb, Yb, Wb, Hb, Ws, Ws), bmp)# this defines the cached bitmap
            #XY = self.ShiftFun(XY[0], XY[1], W, H)
            #fixme: get the shiftfun working!
        else:
            print "Using cached bitmap"
            ##fixme: The cached bitmap could be used if the one needed is the same scale, but
            ##       a subset of the cached one.
            bmp = self.ScaledBitmap[1]
        dc.DrawBitmapPoint(bmp, XYs, True)

        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawRectanglePointSize(XYs, (Ws, Hs) )

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        BBworld = BBox.asBBox(self._Canvas.ViewPortBB)
        ## first see if entire bitmap is displayed:
        if  BBworld.Inside(self.BoundingBox):
            print "Drawing entire bitmap with old code"
            self._DrawEntireBitmap(dc , WorldToPixel, ScaleWorldToPixel, HTdc)
            return None
        elif BBworld.Overlaps(self.BoundingBox):
            #BBbitmap = BBox.fromPoints(self.WorldToBitmap(BBworld))
            print "Drawing a sub-bitmap"
            self._DrawSubBitmap(dc , WorldToPixel, ScaleWorldToPixel, HTdc)
        else:
            print "Not Drawing -- no part of image is showing"

class DotGrid:
    """
    An example of a Grid Object -- it is set on teh FloatCAnvas with one of: 
    
    FloatCanvas.GridUnder = Grid
    FloatCanvas.GridOver = Grid
    
    It will be drawn every time, regardless of the viewport.
    
    In its _Draw method, it computes what to draw, given the ViewPortBB
    of the Canvas it's being drawn on.
    
    """
    def __init__(self, Spacing, Size = 2, Color = "Black", Cross=False, CrossThickness = 1):

        self.Spacing = N.array(Spacing, N.float)
        self.Spacing.shape = (2,)
        self.Size = Size
        self.Color = Color
        self.Cross = Cross
        self.CrossThickness = CrossThickness

    def CalcPoints(self, Canvas):
        ViewPortBB = Canvas.ViewPortBB

        Spacing = self.Spacing

        minx, miny = N.floor(ViewPortBB[0] / Spacing) * Spacing
        maxx, maxy = N.ceil(ViewPortBB[1] / Spacing) * Spacing

        ##fixme: this could use vstack or something with numpy
        x = N.arange(minx, maxx+Spacing[0], Spacing[0]) # making sure to get the last point
        y = N.arange(miny, maxy+Spacing[1], Spacing[1]) # an extra is OK
        Points = N.zeros((len(y), len(x), 2), N.float)
        x.shape = (1,-1)
        y.shape = (-1,1)
        Points[:,:,0] += x
        Points[:,:,1] += y
        Points.shape = (-1,2)

        return Points

    def _Draw(self, dc, Canvas):
        Points = self.CalcPoints(Canvas)

        Points = Canvas.WorldToPixel(Points)

        dc.SetPen(wx.Pen(self.Color,self.CrossThickness))

        if self.Cross: # Use cross shaped markers
            #Horizontal lines
            LinePoints = N.concatenate((Points + (self.Size,0),Points + (-self.Size,0)),1)
            dc.DrawLineList(LinePoints)
            # Vertical Lines
            LinePoints = N.concatenate((Points + (0,self.Size),Points + (0,-self.Size)),1)
            dc.DrawLineList(LinePoints)
            pass
        else: # use dots
            ## Note: this code borrowed from Pointset -- itreally shouldn't be repeated here!.
            if self.Size <= 1:
                dc.DrawPointList(Points)
            elif self.Size <= 2:
                dc.DrawPointList(Points + (0,-1))
                dc.DrawPointList(Points + (0, 1))
                dc.DrawPointList(Points + (1, 0))
                dc.DrawPointList(Points + (-1,0))
            else:
                dc.SetBrush(wx.Brush(self.Color))
                radius = int(round(self.Size/2))
                ##fixme: I really should add a DrawCircleList to wxPython
                if len(Points) > 100:
                    xy = Points
                    xywh = N.concatenate((xy-radius, N.ones(xy.shape) * self.Size ), 1 )
                    dc.DrawEllipseList(xywh)
                else:
                    for xy in Points:
                        dc.DrawCircle(xy[0],xy[1], radius)



#---------------------------------------------------------------------------
class FloatCanvas(wx.Panel):
    """
    FloatCanvas.py

    This is a high level window for drawing maps and anything else in an
    arbitrary coordinate system.

    The goal is to provide a convenient way to draw stuff on the screen
    without having to deal with handling OnPaint events, converting to pixel
    coordinates, knowing about wxWindows brushes, pens, and colors, etc. It
    also provides virtually unlimited zooming and scrolling

    I am using it for two things:
    1) general purpose drawing in floating point coordinates
    2) displaying map data in Lat-long coordinates

    If the projection is set to None, it will draw in general purpose
    floating point coordinates. If the projection is set to 'FlatEarth', it
    will draw a FlatEarth projection, centered on the part of the map that
    you are viewing. You can also pass in your own projection function.

    It is double buffered, so re-draws after the window is uncovered by something
    else are very quick.

    It relies on NumPy, which is needed for speed (maybe, I havn't profiled it)

    Bugs and Limitations:
        Lots: patches, fixes welcome

    For Map drawing: It ignores the fact that the world is, in fact, a
    sphere, so it will do strange things if you are looking at stuff near
    the poles or the date line. so far I don't have a need to do that, so I
    havn't bothered to add any checks for that yet.

    Zooming:
    I have set no zoom limits. What this means is that if you zoom in really
    far, you can get integer overflows, and get wierd results. It
    doesn't seem to actually cause any problems other than wierd output, at
    least when I have run it.

    Speed:
    I have done a couple of things to improve speed in this app. The one
    thing I have done is used NumPy Arrays to store the coordinates of the
    points of the objects. This allowed me to use array oriented functions
    when doing transformations, and should provide some speed improvement
    for objects with a lot of points (big polygons, polylines, pointsets).

    The real slowdown comes when you have to draw a lot of objects, because
    you have to call the wx.DC.DrawSomething call each time. This is plenty
    fast for tens of objects, OK for hundreds of objects, but pretty darn
    slow for thousands of objects.

    The solution is to be able to pass some sort of object set to the DC
    directly. I've used DC.DrawPointList(Points), and it helped a lot with
    drawing lots of points. I havn't got a LineSet type object, so I havn't
    used DC.DrawLineList yet. I'd like to get a full set of DrawStuffList()
    methods implimented, and then I'd also have a full set of Object sets
    that could take advantage of them. I hope to get to it some day.

    Mouse Events:

    At this point, there are a full set of custom mouse events. They are
    just like the regular mouse events, but include an extra attribute:
    Event.GetCoords(), that returns the (x,y) position in world
    coordinates, as a length-2 NumPy vector of Floats.

    Copyright: Christopher Barker

    License: Same as the version of wxPython you are using it with

    Please let me know if you're using this!!!

    Contact me at:

    Chris.Barker@noaa.gov

    """

    def __init__(self, parent, id = -1,
                 size = wx.DefaultSize,
                 ProjectionFun = None,
                 BackgroundColor = "WHITE",
                 Debug = False):

        wx.Panel.__init__( self, parent, id, wx.DefaultPosition, size)

        self.ComputeFontScale()
        self.InitAll()

        self.BackgroundBrush = wx.Brush(BackgroundColor,wx.SOLID)

        self.Debug = Debug

        wx.EVT_PAINT(self, self.OnPaint)
        wx.EVT_SIZE(self, self.OnSize)

        wx.EVT_LEFT_DOWN(self, self.LeftDownEvent)
        wx.EVT_LEFT_UP(self, self.LeftUpEvent)
        wx.EVT_LEFT_DCLICK(self, self.LeftDoubleClickEvent)
        wx.EVT_MIDDLE_DOWN(self, self.MiddleDownEvent)
        wx.EVT_MIDDLE_UP(self, self.MiddleUpEvent)
        wx.EVT_MIDDLE_DCLICK(self, self.MiddleDoubleClickEvent)
        wx.EVT_RIGHT_DOWN(self, self.RightDownEvent)
        wx.EVT_RIGHT_UP(self, self.RightUpEvent)
        wx.EVT_RIGHT_DCLICK(self, self.RightDoubleCLickEvent)
        wx.EVT_MOTION(self, self.MotionEvent)
        wx.EVT_MOUSEWHEEL(self, self.WheelEvent)

        ## CHB: I'm leaving these out for now.
        #wx.EVT_ENTER_WINDOW(self, self. )
        #wx.EVT_LEAVE_WINDOW(self, self. )

        self.SetProjectionFun(ProjectionFun)
        self.GUIMode = None

        # timer to give a delay when re-sizing so that buffers aren't re-built too many times.
        self.SizeTimer = wx.PyTimer(self.OnSizeTimer)

        self.InitializePanel()
        self.MakeNewBuffers()

#        self.CreateCursors()

    def ComputeFontScale(self):
        ## A global variable to hold the scaling from pixel size to point size.
        global FontScale
        dc = wx.ScreenDC()
        dc.SetFont(wx.Font(16, wx.ROMAN, wx.NORMAL, wx.NORMAL))
        E = dc.GetTextExtent("X")
        FontScale = 16/E[1]
        del dc
        
    def InitAll(self):
        """
        InitAll() sets everything in the Canvas to default state.

        It can be used to reset the Canvas

        """

        self.HitColorGenerator = None
        self.UseHitTest = False

        self.NumBetweenBlits = 500

        ## create the Hit Test Dicts:
        self.HitDict = None
        self._HTdc = None

        self._DrawList = []
        self._ForeDrawList = []
        self._ForegroundBuffer = None
        self.BoundingBox = None
        self.BoundingBoxDirty = False
        self.MinScale = None
        self.MaxScale = None
        self.ViewPortCenter= N.array( (0,0), N.float)

        self.SetProjectionFun(None)

        self.MapProjectionVector = N.array( (1,1), N.float) # No Projection to start!
        self.TransformVector = N.array( (1,-1), N.float) # default Transformation

        self.Scale = 1
        self.ObjectUnderMouse = None

        self.GridUnder = None
        self.GridOver = None

        self._BackgroundDirty = True

    def SetProjectionFun(self,ProjectionFun):
        if ProjectionFun == 'FlatEarth':
            self.ProjectionFun = self.FlatEarthProjection
        elif callable(ProjectionFun):
            self.ProjectionFun = ProjectionFun
        elif ProjectionFun is None:
            self.ProjectionFun = lambda x=None: N.array( (1,1), N.float)
        else:
            raise FloatCanvasError('Projectionfun must be either:'
                                   ' "FlatEarth", None, or a callable object '
                                   '(function, for instance) that takes the '
                                   'ViewPortCenter and returns a MapProjectionVector')

    def FlatEarthProjection(self, CenterPoint):
        MaxLatitude = 75 # these were determined essentially arbitrarily
        MinLatitude = -75
        Lat = min(CenterPoint[1],MaxLatitude)
        Lat = max(Lat,MinLatitude)
        return N.array((N.cos(N.pi*Lat/180),1),N.float)

    def SetMode(self, Mode):
            '''
            Set the GUImode to any of the availble mode.
            '''
            # Set mode
            self.GUIMode = Mode
            #self.GUIMode.SetCursor()
            self.SetCursor(self.GUIMode.Cursor)

    def MakeHitDict(self):
        ##fixme: Should this just be None if nothing has been bound?
        self.HitDict = {EVT_FC_LEFT_DOWN: {},
                        EVT_FC_LEFT_UP: {},
                        EVT_FC_LEFT_DCLICK: {},
                        EVT_FC_MIDDLE_DOWN: {},
                        EVT_FC_MIDDLE_UP: {},
                        EVT_FC_MIDDLE_DCLICK: {},
                        EVT_FC_RIGHT_DOWN: {},
                        EVT_FC_RIGHT_UP: {},
                        EVT_FC_RIGHT_DCLICK: {},
                        EVT_FC_ENTER_OBJECT: {},
                        EVT_FC_LEAVE_OBJECT: {},
                        }

    def _RaiseMouseEvent(self, Event, EventType):
        """
        This is called in various other places to raise a Mouse Event
        """
        pt = self.PixelToWorld( Event.GetPosition() )
        evt = _MouseEvent(EventType, Event, self.GetId(), pt)
        self.GetEventHandler().ProcessEvent(evt)

    if wx.__version__ >= "2.8":
        HitTestBitmapDepth = 32
        #print "Using hit test code for 2.8"
        def GetHitTestColor(self, xy):
            if self._ForegroundHTBitmap:
                pdata = wx.AlphaPixelData(self._ForegroundHTBitmap)
            else:
                pdata = wx.AlphaPixelData(self._HTBitmap)
            if not pdata:
                raise RuntimeError("Trouble Accessing Hit Test bitmap")
            pacc = pdata.GetPixels()
            pacc.MoveTo(pdata, xy[0], xy[1])
            return pacc.Get()[:3]
    else:
        HitTestBitmapDepth = 24
        #print "using pre-2.8 hit test code"
        def GetHitTestColor(self,  xy ):
            dc = wx.MemoryDC()
            if self._ForegroundHTBitmap:
                dc.SelectObject(self._ForegroundHTBitmap)
            else:
                dc.SelectObject(self._HTBitmap)
            hitcolor = dc.GetPixelPoint( xy )
            return hitcolor.Get()

    def HitTest(self, event, HitEvent):
        if self.HitDict:
            # check if there are any objects in the dict for this event
            if self.HitDict[ HitEvent ]:
                xy = event.GetPosition()
                color = self.GetHitTestColor( xy )
                if color in self.HitDict[ HitEvent ]:
                    Object = self.HitDict[ HitEvent ][color]
                    ## Add the hit coords to the Object
                    Object.HitCoords = self.PixelToWorld( xy )
                    Object.HitCoordsPixel = xy
                    Object.CallBackFuncs[HitEvent](Object)
                    return True
            return False

    def MouseOverTest(self, event):
        ##fixme: Can this be cleaned up?
        if (self.HitDict and

                (self.HitDict[EVT_FC_ENTER_OBJECT ] or
                 self.HitDict[EVT_FC_LEAVE_OBJECT ]    )
            ):
            xy = event.GetPosition()
            color = self.GetHitTestColor( xy )
            OldObject = self.ObjectUnderMouse
            ObjectCallbackCalled = False
            if color in self.HitDict[ EVT_FC_ENTER_OBJECT ]:
                Object = self.HitDict[ EVT_FC_ENTER_OBJECT][color]
                if (OldObject is None):
                    try:
                        Object.CallBackFuncs[EVT_FC_ENTER_OBJECT](Object)
                        ObjectCallbackCalled =  True
                    except KeyError:
                        pass # this means the enter event isn't bound for that object
                elif OldObject == Object: # the mouse is still on the same object
                    pass
                    ## Is the mouse on a differnt object as it was...
                elif not (Object == OldObject):
                    # call the leave object callback
                    try:
                        OldObject.CallBackFuncs[EVT_FC_LEAVE_OBJECT](OldObject)
                        ObjectCallbackCalled =  True
                    except KeyError:
                        pass # this means the leave event isn't bound for that object
                    try:
                        Object.CallBackFuncs[EVT_FC_ENTER_OBJECT](Object)
                        ObjectCallbackCalled =  True
                    except KeyError:
                        pass # this means the enter event isn't bound for that object
                    ## set the new object under mouse
                self.ObjectUnderMouse = Object
            elif color in self.HitDict[ EVT_FC_LEAVE_OBJECT ]:
                Object = self.HitDict[ EVT_FC_LEAVE_OBJECT][color]
                self.ObjectUnderMouse = Object
            else:
                # no objects under mouse bound to mouse-over events
                self.ObjectUnderMouse = None
                if OldObject:
                    try:
                        OldObject.CallBackFuncs[EVT_FC_LEAVE_OBJECT](OldObject)
                        ObjectCallbackCalled =  True
                    except KeyError:
                        pass # this means the leave event isn't bound for that object
            return ObjectCallbackCalled
        return False

    ## fixme: There is a lot of repeated code here
    ##        Is there a better way?
    def LeftDoubleClickEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnLeftDouble(event)
        event.Skip()

    def MiddleDownEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnMiddleDown(event)
        event.Skip()

    def MiddleUpEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnMiddleUp(event)
        event.Skip()

    def MiddleDoubleClickEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnMiddleDouble(event)
        event.Skip()

    def RightDoubleCLickEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnRightDouble(event)
        event.Skip()

    def WheelEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnWheel(event)
        event.Skip()

    def LeftDownEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnLeftDown(event)
        event.Skip()

    def LeftUpEvent(self, event):
        if self.HasCapture():
            self.ReleaseMouse()
        if self.GUIMode:
            self.GUIMode.OnLeftUp(event)
        event.Skip()

    def MotionEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnMove(event)
        event.Skip()

    def RightDownEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnRightDown(event)
        event.Skip()

    def RightUpEvent(self, event):
        if self.GUIMode:
            self.GUIMode.OnRightUp(event)
        event.Skip()

    def MakeNewBuffers(self):
        self._BackgroundDirty = True
        # Make new offscreen bitmap:
        self._Buffer = wx.EmptyBitmap(*self.PanelSize)
        if self._ForeDrawList:
            self._ForegroundBuffer = wx.EmptyBitmap(*self.PanelSize)
            if self.UseHitTest:
                self.MakeNewHTBitmap()
            else:
                self._ForegroundHTBitmap = None
        else:
            self._ForegroundBuffer = None
            self._ForegroundHTBitmap = None

        if self.UseHitTest:
            self.MakeNewHTBitmap()
        else:
            self._HTBitmap = None
            self._ForegroundHTBitmap = None

    def MakeNewHTBitmap(self):
        """
        Off screen Bitmap used for Hit tests on background objects
        
        """
        self._HTBitmap = wx.EmptyBitmap(self.PanelSize[0],

                                        self.PanelSize[1],

                                        depth=self.HitTestBitmapDepth)

    def MakeNewForegroundHTBitmap(self):
        ## Note: the foreground and backround HT bitmaps are in separate functions
        ##       so that they can be created separate --i.e. when a foreground is
        ##       added after the backgound is drawn
        """
        Off screen Bitmap used for Hit tests on foreground objects
        
        """
        self._ForegroundHTBitmap = wx.EmptyBitmap(self.PanelSize[0],

                                                  self.PanelSize[1],

                                                  depth=self.HitTestBitmapDepth)

    def OnSize(self, event=None):
        self.InitializePanel()
        self.SizeTimer.Start(50, oneShot=True)

    def OnSizeTimer(self, event=None):
        self.MakeNewBuffers()
        self.Draw()

    def InitializePanel(self):
        self.PanelSize = self.GetClientSizeTuple()
        if self.PanelSize == (0,0):
            ## OS-X sometimes gives a Size event when the panel is size (0,0)
            self.PanelSize = (2,2)
        self.PanelSize  = N.array(self.PanelSize,  N.int32)
        self.HalfPanelSize = self.PanelSize / 2 # lrk: added for speed in WorldToPixel
        if self.PanelSize[0] == 0 or self.PanelSize[1] == 0:
            self.AspectRatio = 1.0
        else:
            self.AspectRatio = float(self.PanelSize[0]) / self.PanelSize[1]

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        if self._ForegroundBuffer:
            dc.DrawBitmap(self._ForegroundBuffer,0,0)
        else:
            dc.DrawBitmap(self._Buffer,0,0)

    def Draw(self, Force=False):
        """

        Canvas.Draw(Force=False)

        Re-draws the canvas.

        Note that the buffer will not be re-drawn unless something has
        changed. If you change a DrawObject directly, then the canvas
        will not know anything has changed. In this case, you can force
        a re-draw by passing int True for the Force flag:

        Canvas.Draw(Force=True)

        There is a main buffer set up to double buffer the screen, so
        you can get quick re-draws when the window gets uncovered.

        If there are any objects in self._ForeDrawList, then the
        background gets drawn to a new buffer, and the foreground
        objects get drawn on top of it. The final result if blitted to
        the screen, and stored for future Paint events.  This is done so
        that you can have a complicated background, but have something
        changing on the foreground, without having to wait for the
        background to get re-drawn. This can be used to support simple
        animation, for instance.

        """

        if N.sometrue(self.PanelSize <= 2 ):
            # it's possible for this to get called before being properly initialized.
            return
        if self.Debug: start = clock()
        ScreenDC =  wx.ClientDC(self)
        ViewPortWorld = N.array(( self.PixelToWorld((0,0)),
                                  self.PixelToWorld(self.PanelSize) )
                                     )
        self.ViewPortBB = N.array( ( N.minimum.reduce(ViewPortWorld),
                              N.maximum.reduce(ViewPortWorld) ) )
        #self.ViewPortWorld = ViewPortWorld

        dc = wx.MemoryDC()
        dc.SelectObject(self._Buffer)
        if self._BackgroundDirty or Force:
            dc.SetBackground(self.BackgroundBrush)
            dc.Clear()
            if self._HTBitmap is not None:
                HTdc = wx.MemoryDC()
                HTdc.SelectObject(self._HTBitmap)
                HTdc.Clear()
            else:
                HTdc = None
            if self.GridUnder is not None:
                self.GridUnder._Draw(dc, self)
            self._DrawObjects(dc, self._DrawList, ScreenDC, self.ViewPortBB, HTdc)
            self._BackgroundDirty = False
            del HTdc

        if self._ForeDrawList:
            ## If an object was just added to the Foreground, there might not yet be a buffer
            if self._ForegroundBuffer is None:
                self._ForegroundBuffer = wx.EmptyBitmap(self.PanelSize[0],
                                                        self.PanelSize[1])

            dc = wx.MemoryDC() ## I got some strange errors (linewidths wrong) if I didn't make a new DC here
            dc.SelectObject(self._ForegroundBuffer)
            dc.DrawBitmap(self._Buffer,0,0)
            if self._ForegroundHTBitmap is not None:
                ForegroundHTdc = wx.MemoryDC()
                ForegroundHTdc.SelectObject( self._ForegroundHTBitmap)
                ForegroundHTdc.Clear()
                if self._HTBitmap is not None:
                    #Draw the background HT buffer to the foreground HT buffer
                    ForegroundHTdc.DrawBitmap(self._HTBitmap, 0, 0)
            else:
                ForegroundHTdc = None
            self._DrawObjects(dc,
                              self._ForeDrawList,
                              ScreenDC,
                              self.ViewPortBB,
                              ForegroundHTdc)
        if self.GridOver is not None:
            self.GridOver._Draw(dc, self)
        ScreenDC.Blit(0, 0, self.PanelSize[0],self.PanelSize[1], dc, 0, 0)
        # If the canvas is in the middle of a zoom or move,
        # the Rubber Band box needs to be re-drawn
        ##fixme: maybe GUIModes should never be None, and rather have a Do-nothing GUI-Mode.
        if self.GUIMode is not None:
            self.GUIMode.UpdateScreen()

        if self.Debug: print "Drawing took %f seconds of CPU time"%(clock()-start)

        ## Clear the font cache. If you don't do this, the X font server
        ## starts to take up Massive amounts of memory This is mostly a
        ## problem with very large fonts, that you get with scaled text
        ## when zoomed in.
        DrawObject.FontList = {}

    def _ShouldRedraw(DrawList, ViewPortBB): # lrk: adapted code from BBCheck
        # lrk: Returns the objects that should be redrawn

        ## fixme: should this check be moved into the object?
        ##        also: a BB object would make this cleaner too
        BB2 = ViewPortBB
        redrawlist = []
        for Object in DrawList:
            BB1 = Object.BoundingBox
            ## note: this could use the Utilities.BBCheck function
            ##       butthis saves a function call
            if (BB1[1,0] > BB2[0,0] and BB1[0,0] < BB2[1,0] and
                 BB1[1,1] > BB2[0,1] and BB1[0,1] < BB2[1,1]):
                redrawlist.append(Object)
        #return redrawlist
        ##fixme: disabled this!!!!
        return redrawlist
    _ShouldRedraw = staticmethod(_ShouldRedraw)

    def MoveImage(self,shift,CoordType):
        """
        move the image in the window.

        shift is an (x,y) tuple, specifying the amount to shift in each direction

        It can be in any of three coordinates: Panel, Pixel, World,
        specified by the CoordType parameter

        Panel coordinates means you want to shift the image by some
        fraction of the size of the displaed image

        Pixel coordinates means you want to shift the image by some number of pixels

        World coordinates mean you want to shift the image by an amount
        in Floating point world coordinates

        """
        shift = N.asarray(shift,N.float)
        if CoordType == 'Panel':# convert from panel coordinates
            shift = shift * N.array((-1,1),N.float) *self.PanelSize/self.TransformVector
        elif CoordType == 'Pixel': # convert from pixel coordinates
            shift = shift/self.TransformVector
        elif CoordType == 'World': # No conversion
            pass
        else:
            raise FloatCanvasError('CoordType must be either "Panel", "Pixel", or "World"')

        self.ViewPortCenter = self.ViewPortCenter + shift
        self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
        self.TransformVector = N.array((self.Scale,-self.Scale),N.float) * self.MapProjectionVector
        self._BackgroundDirty = True
        self.Draw()

    def Zoom(self, factor, center = None, centerCoords="world"):

        """
        Zoom(factor, center) changes the amount of zoom of the image by factor.
        If factor is greater than one, the image gets larger.
        If factor is less than one, the image gets smaller.

        center is a tuple of (x,y) coordinates of the center of the viewport, after zooming.
        If center is not given, the center will stay the same.

        centerCoords is a flag indicating whether the center given is in pixel or world 
        coords. Options are: "world" or "pixel"
        
        """
        self.Scale = self.Scale*factor
        if not center is None:
            if centerCoords == "pixel":
                center = self.PixelToWorld( center )
            else:
                center = N.array(center,N.float)
            self.ViewPortCenter = center
        self.SetToNewScale()

    def ZoomToBB(self, NewBB=None, DrawFlag=True):

        """

        Zooms the image to the bounding box given, or to the bounding
        box of all the objects on the canvas, if none is given.

        """
        
        if NewBB is not None:
            BoundingBox = NewBB
        else:
            if self.BoundingBoxDirty:
                self._ResetBoundingBox()
            BoundingBox = self.BoundingBox
        if BoundingBox is not None:
            self.ViewPortCenter = N.array(((BoundingBox[0,0]+BoundingBox[1,0])/2,
                                         (BoundingBox[0,1]+BoundingBox[1,1])/2 ),N.float_)
            self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
            # Compute the new Scale
            BoundingBox = BoundingBox*self.MapProjectionVector # this does need to make a copy!
            try:
                self.Scale = min(abs(self.PanelSize[0] / (BoundingBox[1,0]-BoundingBox[0,0])),
                                 abs(self.PanelSize[1] / (BoundingBox[1,1]-BoundingBox[0,1])) )*0.95
            except ZeroDivisionError: # this will happen if the BB has zero width or height
                try: #width == 0
                    self.Scale = (self.PanelSize[0]  / (BoundingBox[1,0]-BoundingBox[0,0]))*0.95
                except ZeroDivisionError:
                    try: # height == 0
                        self.Scale = (self.PanelSize[1]  / (BoundingBox[1,1]-BoundingBox[0,1]))*0.95
                    except ZeroDivisionError: #zero size! (must be a single point)
                        self.Scale = 1

            if DrawFlag:
                self._BackgroundDirty = True
        else:
            # Reset the shifting and scaling to defaults when there is no BB
            self.ViewPortCenter= N.array( (0,0), N.float)
            self.Scale= 1
        self.SetToNewScale(DrawFlag=DrawFlag)

    def SetToNewScale(self, DrawFlag=True):
        Scale = self.Scale
        if self.MinScale is not None:
            Scale = max(Scale, self.MinScale)
        if self.MaxScale is not None:
            Scale = min(Scale, self.MaxScale)
        self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
        self.TransformVector = N.array((Scale,-Scale),N.float) * self.MapProjectionVector
        self.Scale = Scale
        self._BackgroundDirty = True
        if DrawFlag:
            self.Draw()

    def RemoveObjects(self, Objects):
        for Object in Objects:
            self.RemoveObject(Object, ResetBB=False)
        self.BoundingBoxDirty = True

    def RemoveObject(self, Object, ResetBB = True):
        ##fixme: Using the list.remove method is kind of slow
        if Object.InForeground:
            self._ForeDrawList.remove(Object)
            if not self._ForeDrawList:
                self._ForegroundBuffer = None
                self._ForegroundHTdc = None
        else:
            self._DrawList.remove(Object)
            self._BackgroundDirty = True
        if ResetBB:
            self.BoundingBoxDirty = True

    def ClearAll(self, ResetBB=True):
        """
        ClearAll(ResetBB=True)

        Removes all DrawObjects from the Canvas

        If ResetBB is set to False, the original bounding box will remain

        """
        self._DrawList = []
        self._ForeDrawList = []
        self._BackgroundDirty = True
        self.HitColorGenerator = None
        self.UseHitTest = False
        if ResetBB:
            self._ResetBoundingBox()
        self.MakeNewBuffers()
        self.HitDict = None

    def _ResetBoundingBox(self):
        if self._DrawList or self._ForeDrawList:
            bblist = []
            for (i, obj) in enumerate(self._DrawList):
                bblist.append(obj.BoundingBox)
            for (j, obj) in enumerate(self._ForeDrawList):
                bblist.append(obj.BoundingBox)
            self.BoundingBox = BBox.fromBBArray(bblist)
        else:
            self.BoundingBox = None
            self.ViewPortCenter= N.array( (0,0), N.float)
            self.TransformVector = N.array( (1,-1), N.float)
            self.MapProjectionVector = N.array( (1,1), N.float)
            self.Scale = 1
        self.BoundingBoxDirty = False

    def PixelToWorld(self, Points):
        """
        Converts coordinates from Pixel coordinates to world coordinates.

        Points is a tuple of (x,y) coordinates, or a list of such tuples,
        or a NX2 Numpy array of x,y coordinates.

        """
        return  (((N.asarray(Points, N.float) -
                   (self.PanelSize/2))/self.TransformVector) +
                 self.ViewPortCenter)

    def WorldToPixel(self,Coordinates):
        """
        This function will get passed to the drawing functions of the objects,
        to transform from world to pixel coordinates.
        Coordinates should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """
        #Note: this can be called by users code for various reasons, so N.asarray is needed.
        return  (((N.asarray(Coordinates,N.float) -
                   self.ViewPortCenter)*self.TransformVector)+
                 (self.HalfPanelSize)).astype('i')

    def ScaleWorldToPixel(self,Lengths):
        """
        This function will get passed to the drawing functions of the objects,
        to Change a length from world to pixel coordinates.

        Lengths should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """
        return  ( (N.asarray(Lengths, N.float)*self.TransformVector) ).astype('i')

    def ScalePixelToWorld(self,Lengths):
        """
        This function computes a pair of x.y lengths,
        to change then from pixel to world coordinates.

        Lengths should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """

        return  (N.asarray(Lengths,N.float) / self.TransformVector)

    def AddObject(self, obj):
        # put in a reference to the Canvas, so remove and other stuff can work
        obj._Canvas = self
        if  obj.InForeground:
            self._ForeDrawList.append(obj)
            self.UseForeground = True
        else:
            self._DrawList.append(obj)
            self._BackgroundDirty = True
        self.BoundingBoxDirty = True
        return True

    def AddObjects(self, Objects):
        for Object in Objects:
            self.AddObject(Object)

    def _DrawObjects(self, dc, DrawList, ScreenDC, ViewPortBB, HTdc = None):
        """
        This is a convenience function;
        This function takes the list of objects and draws them to specified
        device context.
        """
        dc.SetBackground(self.BackgroundBrush)
        dc.BeginDrawing()
        #i = 0
        PanelSize0, PanelSize1 = self.PanelSize # for speed
        WorldToPixel = self.WorldToPixel # for speed
        ScaleWorldToPixel = self.ScaleWorldToPixel # for speed
        Blit = ScreenDC.Blit # for speed
        NumBetweenBlits = self.NumBetweenBlits # for speed
        for i, Object in enumerate(self._ShouldRedraw(DrawList, ViewPortBB)):
            if Object.Visible:
                Object._Draw(dc, WorldToPixel, ScaleWorldToPixel, HTdc)
                if (i+1) % NumBetweenBlits == 0:
                    Blit(0, 0, PanelSize0, PanelSize1, dc, 0, 0)
        dc.EndDrawing()

    def SaveAsImage(self, filename, ImageType=wx.BITMAP_TYPE_PNG):
        """

        Saves the current image as an image file. The default is in the
        PNG format. Other formats can be specified using the wx flags:

        wx.BITMAP_TYPE_PNG
        wx.BITMAP_TYPE_JPG
        wx.BITMAP_TYPE_BMP
        wx.BITMAP_TYPE_XBM
        wx.BITMAP_TYPE_XPM
        etc. (see the wx docs for the complete list)

        """

        self._Buffer.SaveFile(filename, ImageType)


def _makeFloatCanvasAddMethods(): ## lrk's code for doing this in module __init__
    classnames = ["Circle", "Ellipse", "Rectangle", "ScaledText", "Polygon",
                  "Line", "Text", "PointSet","Point", "Arrow", "ArrowLine", "ScaledTextBox",
                  "SquarePoint","Bitmap", "ScaledBitmap", "Spline", "Group"]
    for classname in classnames:
        klass = globals()[classname]
        def getaddshapemethod(klass=klass):
            def addshape(self, *args, **kwargs):
                Object = klass(*args, **kwargs)
                self.AddObject(Object)
                return Object
            return addshape
        addshapemethod = getaddshapemethod()
        methodname = "Add" + classname
        setattr(FloatCanvas, methodname, addshapemethod)
        docstring = "Creates %s and adds its reference to the canvas.\n" % classname
        docstring += "Argument protocol same as %s class" % classname
        if klass.__doc__:
            docstring += ", whose docstring is:\n%s" % klass.__doc__
        FloatCanvas.__dict__[methodname].__doc__ = docstring

_makeFloatCanvasAddMethods()


