try:
    from Numeric import array,asarray,Float,cos, sin, pi,sum,minimum,maximum,Int32,zeros, ones, concatenate, sqrt, argmin, power, absolute, matrixmultiply, transpose, sometrue
except ImportError:
    try:
        from numarray import array, asarray, Float, cos, sin, pi, sum, minimum, maximum, Int32, zeros, concatenate, matrixmultiply, transpose, sometrue
    except ImportError:
        raise ImportError("I could not import either Numeric or numarray")

from time import clock, sleep

import wx

import types
import os        

## A global variable to hold the Pixels per inch that wxWindows thinks is in use
## This is used for scaling fonts.
## This can't be computed on module __init__, because a wx.App might not have iniitalized yet.
global ScreenPPI

## a custom Exceptions:

class FloatCanvasError(Exception):
    pass

## Create all the mouse events
# I don't see a need for these two, but maybe some day!
#EVT_FC_ENTER_WINDOW = wx.NewEventType()
#EVT_FC_LEAVE_WINDOW = wx.NewEventType()
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
EVT_FC_ENTER_OBJECT = wx.NewEventType()
EVT_FC_LEAVE_OBJECT = wx.NewEventType()

##Create all mouse event binding functions
#def EVT_ENTER_WINDOW( window, function ):
#    window.Connect( -1, -1, EVT_FC_ENTER_WINDOW, function ) 
#def EVT_LEAVE_WINDOW( window, function ):
#    window.Connect( -1, -1,EVT_FC_LEAVE_WINDOW , function ) 
def EVT_LEFT_DOWN( window, function ):  
    window.Connect( -1, -1,EVT_FC_LEFT_DOWN , function )
def EVT_LEFT_UP( window, function ):
    window.Connect( -1, -1,EVT_FC_LEFT_UP , function )
def EVT_LEFT_DCLICK  ( window, function ):
    window.Connect( -1, -1,EVT_FC_LEFT_DCLICK , function )
def EVT_MIDDLE_DOWN  ( window, function ):
    window.Connect( -1, -1,EVT_FC_MIDDLE_DOWN , function )
def EVT_MIDDLE_UP  ( window, function ):
    window.Connect( -1, -1,EVT_FC_MIDDLE_UP , function )
def EVT_MIDDLE_DCLICK  ( window, function ):
    window.Connect( -1, -1,EVT_FC_MIDDLE_DCLICK , function )
def EVT_RIGHT_DOWN  ( window, function ):
    window.Connect( -1, -1,EVT_FC_RIGHT_DOWN , function )
def EVT_RIGHT_UP( window, function ):
    window.Connect( -1, -1,EVT_FC_RIGHT_UP , function )
def EVT_RIGHT_DCLICK( window, function ):
    window.Connect( -1, -1,EVT_FC_RIGHT_DCLICK , function )
def EVT_MOTION( window, function ):
    window.Connect( -1, -1,EVT_FC_MOTION , function )
def EVT_MOUSEWHEEL( window, function ):
    window.Connect( -1, -1,EVT_FC_MOUSEWHEEL , function )

class _MouseEvent(wx.PyCommandEvent):

    """

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
    
# I don't think this is used.
#    def SetCoords(self,Coords):
#        self.Coords = Coords
        
    def GetCoords(self):
        return self.Coords

    def __getattr__(self, name):
        #return eval(self.NativeEvent.__getattr__(name) )
        return getattr(self._NativeEvent, name)

def _cycleidxs(indexcount, maxvalue, step):

    """
    Utility function used by _colorGenerator

    """
    if indexcount == 0:
        yield ()
    else:
        for idx in xrange(0, maxvalue, step):
            for tail in _cycleidxs(indexcount - 1, maxvalue, step):
                yield (idx, ) + tail

def _colorGenerator():

    """

    Generates a seris of unique colors used to do hit-tests with the HIt
    Test bitmap

    """
    import sys
    if sys.platform == 'darwin':
        depth = 24
    else:
        b = wx.EmptyBitmap(1,1)
        depth = b.GetDepth()
    if depth == 16:
        step = 8
    elif depth >= 24:
        step = 1
    else:
        raise "ColorGenerator does not work with depth = %s" % depth
    return _cycleidxs(indexcount=3, maxvalue=256, step=step)


#### I don't know if the Set objects are useful, beyond the pointset
#### object The problem is that when zoomed in, the BB is checked to see
#### whether to draw the object.  A Set object can defeat this. ONe day
#### I plan to write some custon C++ code to draw sets of objects

##class ObjectSetMixin:
##    """
##    A mix-in class for draw objects that are sets of objects

##    It contains methods for setting lists of pens and brushes

##    """
##    def SetPens(self,LineColors,LineStyles,LineWidths):
##        """
##        This method used when an object could have a list of pens, rather than just one
##        It is used for LineSet, and perhaps others in the future.

##        fixme: this should be in a mixin

##        fixme: this is really kludgy, there has got to be a better way!

##        """

##        length = 1
##        if type(LineColors) == types.ListType:
##            length = len(LineColors)
##        else:
##            LineColors = [LineColors]

##        if type(LineStyles) == types.ListType:
##            length = len(LineStyles)
##        else:
##            LineStyles = [LineStyles]

##        if type(LineWidths) == types.ListType:
##            length = len(LineWidths)
##        else:
##            LineWidths = [LineWidths]

##        if length > 1:
##            if len(LineColors) == 1:
##                LineColors = LineColors*length
##            if len(LineStyles) == 1:
##                LineStyles = LineStyles*length
##            if len(LineWidths) == 1:
##                LineWidths = LineWidths*length

##        self.Pens = []
##        for (LineColor,LineStyle,LineWidth) in zip(LineColors,LineStyles,LineWidths):
##            if LineColor is None or LineStyle is None:
##                self.Pens.append(wx.TRANSPARENT_PEN)
##                # what's this for?> self.LineStyle = 'Transparent'
##            if not self.PenList.has_key((LineColor,LineStyle,LineWidth)):
##                Pen = wx.Pen(LineColor,LineWidth,self.LineStyleList[LineStyle])
##                self.Pens.append(Pen)
##            else:
##                self.Pens.append(self.PenList[(LineColor,LineStyle,LineWidth)])
##        if length == 1:
##            self.Pens = self.Pens[0]

class DrawObject:
    """
    This is the base class for all the objects that can be drawn.

    One must subclass from this (and an assortment of Mixins) to create
    a new DrawObject.

    """

    def __init__(self,InForeground  = False):
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

    def Bind(self, Event, CallBackFun):
        self.CallBackFuncs[Event] = CallBackFun
        self.HitAble = True
        self._Canvas.UseHitTest = True
        if not self._Canvas._HTdc:
            self._Canvas.MakeNewHTdc()
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
        self.SetBrush(FillColor,self.FillStyle)

    def SetFillStyle(self, FillStyle):
        self.FillStyle = FillStyle
        self.SetBrush(self.FillColor,FillStyle)
    
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
        
        Delta = asarray(Delta, Float)
        self.XY += Delta
        self.BoundingBox = self.BoundingBox + Delta
        if self._Canvas:
            self._Canvas.BoundingBoxDirty = True      

    def SetXY(self, x, y):
        self.XY = array( (x, y), Float)
        self.CalcBoundingBox()

    def CalcBoundingBox(self):
        ## This may get overwritten in some subclasses
        self.BoundingBox = array( (self.XY, self.XY), Float )

    def SetPoint(self, xy):
        self.XY = array( xy, Float)
        self.XY.shape = (2,)
        self.CalcBoundingBox()

class PointsObjectMixin:
    """

    This is a mixin class that provides some methods suitable for use
    with objects that have a set of (x,y) coordinate pairs.

    """


## This is code for the PointsObjectMixin object, it needs to be adapted and tested.
## Is the neccesary at all: you can always do:
##    Object.SetPoints( Object.Points + delta, copy = False)    
##    def Move(self, Delta ):
##        """

##        Move(Delta): moves the object by delta, where delta is an (dx,
##        dy) pair. Ideally a Numpy array of shape (2,)

##        """
        
##        Delta = array(Delta, Float)
##        self.XY += Delta
##        self.BoundingBox = self.BoundingBox + Delta##array((self.XY, (self.XY + self.WH)), Float)
##        if self._Canvas:
##            self._Canvas.BoundingBoxDirty = True      

    def CalcBoundingBox(self):
        self.BoundingBox = array(((min(self.Points[:,0]),
                                   min(self.Points[:,1]) ),
                                  (max(self.Points[:,0]),
                                   max(self.Points[:,1]) ) ), Float )
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
            self.Points = array(Points, Float)
            self.Points.shape = (-1,2) # Make sure it is a NX2 array, even if there is only one point
        else:
            self.Points = asarray(Points, Float)
        self.CalcBoundingBox()

     
class Polygon(DrawObject,PointsObjectMixin,LineAndFillMixin):

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
        DrawObject.__init__(self,InForeground)
        self.Points = array(Points,Float) # this DOES need to make a copy
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
        Points = WorldToPixel(self.Points)
        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        dc.DrawPolygon(Points)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
            HTdc.DrawPolygon(Points)
            
##class PolygonSet(DrawObject):
##    """
##    The PolygonSet class takes a Geometry.Polygon object.
##    so that Points[N] = (x1,y1) and Points[N+1] = (x2,y2). N must be an even number!
    
##    it creates a set of line segments, from (x1,y1) to (x2,y2)
    
##    """
    
##    def __init__(self,PolySet,LineColors,LineStyles,LineWidths,FillColors,FillStyles,InForeground = False):
##        DrawObject.__init__(self, InForeground)

##        ##fixme: there should be some error checking for everything being the right length.

        
##        self.Points = array(Points,Float)
##        self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)

##        self.LineColors = LineColors
##        self.LineStyles = LineStyles
##        self.LineWidths = LineWidths
##        self.FillColors = FillColors
##        self.FillStyles = FillStyles

##        self.SetPens(LineColors,LineStyles,LineWidths)

##    #def _Draw(self,dc,WorldToPixel,ScaleWorldToPixel):
##    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
##        Points = WorldToPixel(self.Points)
##        Points.shape = (-1,4)
##        dc.DrawLineList(Points,self.Pens)
 

class Line(DrawObject,PointsObjectMixin,LineOnlyMixin):
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


        self.Points = array(Points,Float)
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

class Arrow(DrawObject,XYObjectMixin,LineOnlyMixin):
    """

    Arrow(XY, # coords of origin of arrow (x,y)
          Length, # length of arrow in pixels
          theta, # angle of arrow in degrees: zero is straight up
                 # angle is to the right
          LineColor = "Black",
          LineStyle = "Solid",
          LineWidth    = 1, 
          ArrowHeadSize = 4,
          ArrowHeadAngle = 45,
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

        self.XY = array(XY, Float)
        self.XY.shape = (2,) # Make sure it is a 1X2 array, even if there is only one point
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
        
    def SetLength(self, Length):
        self.Length = Length
        self.CalcArrowPoints()

    def CalcArrowPoints(self):
        L = self.Length
        S = self.ArrowHeadSize
        phi = self.ArrowHeadAngle * pi / 360
        theta = (self.Direction-90.0) * pi / 180
        ArrowPoints = array( ( (0, L, L - S*cos(phi),L, L - S*cos(phi) ),
                               (0, 0, S*sin(phi),    0, -S*sin(phi)    ) ),
                             Float )
        RotationMatrix = array( ( ( cos(theta), -sin(theta) ),
                                  ( sin(theta), cos(theta) ) ),
                                Float
                                )
        ArrowPoints = matrixmultiply(RotationMatrix, ArrowPoints)
        self.ArrowPoints = transpose(ArrowPoints)

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        dc.SetPen(self.Pen)
        xy = WorldToPixel(self.XY)
        ArrowPoints = xy + self.ArrowPoints
        dc.DrawLines(ArrowPoints)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.DrawLines(ArrowPoints)

##class LineSet(DrawObject, ObjectSetMixin):
##    """
##    The LineSet class takes a list of 2-tuples, or a NX2 NumPy array of point coordinates.
##    so that Points[N] = (x1,y1) and Points[N+1] = (x2,y2). N must be an even number!
    
##    it creates a set of line segments, from (x1,y1) to (x2,y2)
    
##    """
    
##    def __init__(self,Points,LineColors,LineStyles,LineWidths,InForeground = False):
##        DrawObject.__init__(self, InForeground)

##        NumLines = len(Points) / 2
##        ##fixme: there should be some error checking for everything being the right length.

        
##        self.Points = array(Points,Float)
##        self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)

##        self.LineColors = LineColors
##        self.LineStyles = LineStyles
##        self.LineWidths = LineWidths

##        self.SetPens(LineColors,LineStyles,LineWidths)

##    #def _Draw(self,dc,WorldToPixel,ScaleWorldToPixel):
##    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
##        Points = WorldToPixel(self.Points)
##        Points.shape = (-1,4)
##        dc.DrawLineList(Points,self.Pens)

class PointSet(DrawObject,PointsObjectMixin, ColorOnlyMixin):
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

        self.Points = array(Points,Float)
        self.Points.shape = (-1,2) # Make sure it is a NX2 array, even if there is only one point
        self.CalcBoundingBox()
        self.Diameter = Diameter

        self.HitLineWidth = self.MinHitLineWidth
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
        ## kind of ugly to minimize data copying
        d = self.Points - XY
        d = sum( power(d,2,d), 1 )
        d = absolute( d, d ) # don't need the real distance, just which is smallest
        #dist = sqrt( sum( (self.Points - XY)**2), 1) )
        return argmin(d)

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
            if len(Points) > 100:
                xy = Points
                xywh = concatenate((xy-radius, ones(xy.shape) * self.Diameter ), 1 )
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
                    xywh = concatenate((xy-radius, ones(xy.shape) * self.Diameter ), 1 )
                    HTdc.DrawEllipseList(xywh)
                else:
                    for xy in Points:
                        HTdc.DrawCircle(xy[0],xy[1], radius)

class Point(DrawObject,XYObjectMixin,ColorOnlyMixin):
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

        self.XY = array(XY, Float)
        self.XY.shape = (2,) # Make sure it is a 1X2 array, even if there is only one point
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

class RectEllipse(DrawObject, XYObjectMixin,LineAndFillMixin):
    def __init__(self,x,y,width,height,
                 LineColor = "Black",
                 LineStyle = "Solid",
                 LineWidth    = 1,
                 FillColor    = None,
                 FillStyle    = "Solid",
                 InForeground = False):
        
        DrawObject.__init__(self,InForeground)

        self.XY = array( (x, y), Float)
        self.WH = array( (width, height), Float )
        self.BoundingBox = array(((x,y), (self.XY + self.WH)), Float)
        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.HitLineWidth = max(LineWidth,self.MinHitLineWidth)

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def SetShape(self,x,y,width,height):
        self.XY = array( (x, y), Float)
        self.WH = array( (width, height), Float )
        self.CalcBoundingBox()


    def SetUpDraw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc):
        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        if HTdc and self.HitAble:
            HTdc.SetPen(self.HitPen)
            HTdc.SetBrush(self.HitBrush)
        return ( WorldToPixel(self.XY),
                 ScaleWorldToPixel(self.WH) )

    def CalcBoundingBox(self):
        self.BoundingBox = array((self.XY, (self.XY + self.WH) ), Float)
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
#    def __init__(*args, **kwargs):
#        RectEllipse.__init__(*args, **kwargs)

    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        ( XY, WH ) = self.SetUpDraw(dc,
                                    WorldToPixel,
                                    ScaleWorldToPixel,
                                    HTdc)
        dc.DrawEllipsePointSize(XY, WH)
        if HTdc and self.HitAble:
            HTdc.DrawEllipsePointSize(XY, WH)

class Circle(Ellipse):
    def __init__(self, x ,y, Diameter, **kwargs):
        self.Center = array((x,y),Float)
        RectEllipse.__init__(self ,
                             x-Diameter/2.,
                             y-Diameter/2.,
                             Diameter,
                             Diameter,
                             **kwargs)

    def SetDiameter(self, Diameter):
        x,y = self.Center - (Diameter/2.)
        self.SetShape(x,
                      y,
                      Diameter,
                      Diameter)
        
class TextObjectMixin(XYObjectMixin):
    """

    A mix in class that holds attributes and methods that are needed by
    the Text objects

    """
    
    ## I'm caching fonts, because on GTK, getting a new font can take a
    ## while. However, it gets cleared after every full draw as hanging
    ## on to a bunch of large fonts takes a massive amount of memory.

    FontList = {}

    def SetFont(self, Size, Family, Style, Weight, Underline, FaceName):
        self.Font = self.FontList.setdefault( (Size,
                                               Family,
                                               Style,
                                               Weight,
                                               Underline,
                                               FaceName),
                                               wx.Font(Size,
                                                       Family,
                                                       Style, 
                                                       Weight,
                                                       Underline,
                                                       FaceName) )
        return self.Font

    def SetColor(self, Color):
        self.Color = Color

    def SetBackgroundColor(self, BackgroundColor):
        self.BackgroundColor = BackgroundColor

    ## store the function that shift the coords for drawing text. The
    ## "c" parameter is the correction for world coordinates, rather
    ## than pixel coords as the y axis is reversed
    ShiftFunDict = {'tl': lambda x, y, w, h, world=0: (x, y) ,
                    'tc': lambda x, y, w, h, world=0: (x - w/2, y) , 
                    'tr': lambda x, y, w, h, world=0: (x - w, y) , 
                    'cl': lambda x, y, w, h, world=0: (x, y - h/2 + world*h) , 
                    'cc': lambda x, y, w, h, world=0: (x - w/2, y - h/2 + world*h) , 
                    'cr': lambda x, y, w, h, world=0: (x - w, y - h/2 + world*h) ,
                    'bl': lambda x, y, w, h, world=0: (x, y - h + 2*world*h) ,
                    'bc': lambda x, y, w, h, world=0: (x - w/2, y - h + 2*world*h) , 
                    'br': lambda x, y, w, h, world=0: (x - w, y - h + 2*world*h)}

class Text(DrawObject, TextObjectMixin):
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
    Underline:
        The value can be True or False. At present this may have an an
        effect on Windows only.

    Alternatively, you can set the kw arg: Font, to a wx.Font, and the above will be ignored.
    
    The size is fixed, and does not scale with the drawing.

    The hit-test is done on the entire text extent

    """
    
    def __init__(self,String,x,y,
                 Size =  12,
                 Color = "Black",
                 BackgroundColor = None,
                 Family = wx.MODERN,
                 Style = wx.NORMAL,
                 Weight = wx.NORMAL,
                 Underline = False,
                 Position = 'tl',
                 InForeground = False,
                 Font = None):
        
        DrawObject.__init__(self,InForeground)

        self.String = String
        # Input size in in Pixels, compute points size from PPI info.
        # fixme: for printing, we'll have to do something a little different
        self.Size = int(round(72.0 * Size / ScreenPPI))

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
        self.SetFont(Size, Family, Style, Weight, Underline, FaceName)

        self.BoundingBox = array(((x,y),(x,y)),Float)

        self.XY = ( x,y )

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

class ScaledText(DrawObject, TextObjectMixin):
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
    Underline:
        The value can be True or False. At present this may have an an
        effect on Windows only.

    Alternatively, you can set the kw arg: Font, to a wx.Font, and the
    above will be ignored. The size of the font you specify will be
    ignored, but the rest of it's attributes will be preserved.
    
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
    optional, but I havn't gotten around to it.

    """
    
    def __init__(self, String, x, y , Size,
                 Color = "Black",
                 BackgroundColor = None,
                 Family = wx.MODERN,
                 Style = wx.NORMAL,
                 Weight = wx.NORMAL,
                 Underline = False,
                 Position = 'tl',
                 Font = None,
                 InForeground = False):
        
        DrawObject.__init__(self,InForeground)

        self.String = String
        self.XY = array( (x, y), Float)
        self.Size = Size     
        self.Color = Color
        self.BackgroundColor = BackgroundColor
        self.Family = Family   
        self.Style = Style    
        self.Weight = Weight   
        self.Underline = Underline
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


    def CalcBoundingBox(self):
        ## this isn't exact, as fonts don't scale exactly.
        dc = wx.MemoryDC()
        bitmap = wx.EmptyBitmap(1, 1)
        dc.SelectObject(bitmap) #wxMac needs a Bitmap selected for GetTextExtent to work.
        DrawingSize = 40 # pts This effectively determines the resolution that the BB is computed to.
        ScaleFactor = float(self.Size) / DrawingSize
        dc.SetFont(self.SetFont(DrawingSize, self.Family, self.Style, self.Weight, self.Underline, self.FaceName) )
        (w,h) = dc.GetTextExtent(self.String)
        w = w * ScaleFactor
        h = h * ScaleFactor
        x, y = self.ShiftFun(self.XY[0], self.XY[1], w, h, world = 1)
        self.BoundingBox = array(((x, y-h ),(x + w, y)),Float)
        
    def _Draw(self, dc , WorldToPixel, ScaleWorldToPixel, HTdc=None):
        (X,Y) = WorldToPixel( (self.XY) )

        # compute the font size:
        Size = abs( ScaleWorldToPixel( (self.Size, self.Size) )[1] ) # only need a y coordinate length
        ## Check to see if the font size is large enough to blow up the X font server
        ## If so, limit it. Would it be better just to not draw it?
        ## note that this limit is dependent on how much memory you have, etc.
        Size = min(Size, self.MaxFontSize)
        dc.SetFont(self.SetFont(Size, self.Family, self.Style, self.Weight, self.Underline, self.FaceName))
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
    just like the rebulsr mouse events, but include an extra attribute:
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
        
        global ScreenPPI ## A global variable to hold the Pixels per inch that wxWindows thinks is in use.
        dc = wx.ScreenDC()
        ScreenPPI = dc.GetPPI()[0] # Assume square pixels
        del dc

        self.HitColorGenerator = None
        self.UseHitTest = None

        self.NumBetweenBlits = 500

        self.BackgroundBrush = wx.Brush(BackgroundColor,wx.SOLID)

        self.Debug = Debug

        wx.EVT_PAINT(self, self.OnPaint)
        wx.EVT_SIZE(self, self.OnSize)
        
        wx.EVT_LEFT_DOWN(self, self.LeftDownEvent ) 
        wx.EVT_LEFT_UP(self, self.LeftUpEvent ) 
        wx.EVT_LEFT_DCLICK(self, self.LeftDoubleClickEvent ) 
        wx.EVT_MIDDLE_DOWN(self, self.MiddleDownEvent ) 
        wx.EVT_MIDDLE_UP(self, self.MiddleUpEvent ) 
        wx.EVT_MIDDLE_DCLICK(self, self.MiddleDoubleClickEvent ) 
        wx.EVT_RIGHT_DOWN(self, self.RightDownEvent)
        wx.EVT_RIGHT_UP(self, self.RightUpEvent ) 
        wx.EVT_RIGHT_DCLICK(self, self.RightDoubleCLickEvent ) 
        wx.EVT_MOTION(self, self.MotionEvent ) 
        wx.EVT_MOUSEWHEEL(self, self.WheelEvent ) 

        ## CHB: I'm leaving these out for now.
        #wx.EVT_ENTER_WINDOW(self, self. ) 
        #wx.EVT_LEAVE_WINDOW(self, self. ) 

        ## create the Hit Test Dicts:
        self.HitDict = None


        self._DrawList = []
        self._ForeDrawList = []
        self._ForegroundBuffer = None
        self.BoundingBox = None
        self.BoundingBoxDirty = False
        self.ViewPortCenter= array( (0,0), Float)

        self.SetProjectionFun(ProjectionFun)
        
        self.MapProjectionVector = array( (1,1), Float) # No Projection to start!
        self.TransformVector = array( (1,-1), Float) # default Transformation
        
        self.Scale = 1

        self.GUIMode = None
        self.StartRBBox = None
        self.PrevRBBox = None
        self.StartMove = None
        self.PrevMoveXY = None
        self.ObjectUnderMouse = None
        
        # called just to make sure everything is initialized
        self.OnSize(None)

        self.InHereNum = 0

    def SetProjectionFun(self,ProjectionFun):
        if ProjectionFun == 'FlatEarth':
            self.ProjectionFun = self.FlatEarthProjection 
        elif type(ProjectionFun) == types.FunctionType:
            self.ProjectionFun = ProjectionFun 
        elif ProjectionFun is None:
            self.ProjectionFun = lambda x=None: array( (1,1), Float)
        else:
            raise FloatCanvasError('Projectionfun must be either: "FlatEarth", None, or a function that takes the ViewPortCenter and returns a MapProjectionVector')

    def FlatEarthProjection(self,CenterPoint):
        return array((cos(pi*CenterPoint[1]/180),1),Float)

    def SetMode(self,Mode):
        if Mode in ["ZoomIn","ZoomOut","Move","Mouse",None]:
            self.GUIMode = Mode
        else:
            raise FloatCanvasError('"%s" is Not a valid Mode'%Mode)

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
        #print "in Raise Mouse Event", Event
        pt = self.PixelToWorld( Event.GetPosition() )
        evt = _MouseEvent(EventType, Event, self.GetId(), pt)
        self.GetEventHandler().ProcessEvent(evt)       

    def HitTest(self, event, HitEvent):
        if self.HitDict:
            # check if there are any objects in the dict for this event
            if self.HitDict[ HitEvent ]:
                xy = event.GetPosition()
                if self._ForegroundHTdc:
                    hitcolor = self._ForegroundHTdc.GetPixelPoint( xy )
                else:
                    hitcolor = self._HTdc.GetPixelPoint( xy )
                color = ( hitcolor.Red(), hitcolor.Green(), hitcolor.Blue() )
                if color in self.HitDict[ HitEvent ]:
                    Object = self.HitDict[ HitEvent ][color]
                    ## Add the hit coords to the Object
                    Object.HitCoords = self.PixelToWorld( xy )
                    Object.CallBackFuncs[HitEvent](Object)
                    return True
            return False

    def MouseOverTest(self, event):
        ##fixme: Can this be cleaned up?
        if self.HitDict:
            xy = event.GetPosition()
            if self._ForegroundHTdc:
                hitcolor = self._ForegroundHTdc.GetPixelPoint( xy )
            else:
                hitcolor = self._HTdc.GetPixelPoint( xy )
            color = ( hitcolor.Red(), hitcolor.Green(), hitcolor.Blue() )
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


    ## fixme: There is a lot of repeated code here
    ##        Is there a better way?            
    def LeftDoubleClickEvent(self,event):
        if self.GUIMode == "Mouse":
            EventType = EVT_FC_LEFT_DCLICK
            if not self.HitTest(event, EventType):
                self._RaiseMouseEvent(event, EventType)


    def MiddleDownEvent(self,event):
        if self.GUIMode == "Mouse":
            EventType = EVT_FC_MIDDLE_DOWN
            if not self.HitTest(event, EventType):
                self._RaiseMouseEvent(event, EventType)

    def MiddleUpEvent(self,event):
        if self.GUIMode == "Mouse":
            EventType = EVT_FC_MIDDLE_UP
            if not self.HitTest(event, EventType):
                self._RaiseMouseEvent(event, EventType)

    def MiddleDoubleClickEvent(self,event):
        if self.GUIMode == "Mouse":
            EventType = EVT_FC_MIDDLE_DCLICK
            if not self.HitTest(event, EventType):
                self._RaiseMouseEvent(event, EventType)

    def RightUpEvent(self,event):
        if self.GUIMode == "Mouse":
            EventType = EVT_FC_RIGHT_UP
            if not self.HitTest(event, EventType):
                self._RaiseMouseEvent(event, EventType)

    def RightDoubleCLickEvent(self,event):
        if self.GUIMode == "Mouse":
            EventType = EVT_FC_RIGHT_DCLICK
            if not self.HitTest(event, EventType):
                self._RaiseMouseEvent(event, EventType)

    def WheelEvent(self,event):
        ##if self.GUIMode == "Mouse":
        ## Why not always raise this?
            self._RaiseMouseEvent(event, EVT_FC_MOUSEWHEEL)


    def LeftDownEvent(self,event):
        if self.GUIMode:
            if self.GUIMode == "ZoomIn":
                self.StartRBBox = array( event.GetPosition() )
                self.PrevRBBox = None
                self.CaptureMouse()
            elif self.GUIMode == "ZoomOut":
                Center = self.PixelToWorld( event.GetPosition() )
                self.Zoom(1/1.5,Center)
            elif self.GUIMode == "Move":
                self.StartMove = array( event.GetPosition() )
                self.PrevMoveXY = (0,0)
            elif self.GUIMode == "Mouse":
                ## check for a hit
                if not self.HitTest(event, EVT_FC_LEFT_DOWN):
                   self._RaiseMouseEvent(event,EVT_FC_LEFT_DOWN)
        else: 
            pass

    def LeftUpEvent(self,event):
    	if self.HasCapture():
            self.ReleaseMouse()
        if self.GUIMode:
            if self.GUIMode == "ZoomIn":
                if event.LeftUp() and not self.StartRBBox is None:
                    self.PrevRBBox = None
                    EndRBBox = event.GetPosition()
                    StartRBBox = self.StartRBBox
                    # if mouse has moved less that ten pixels, don't use the box.
                    if ( abs(StartRBBox[0] - EndRBBox[0]) > 10
                         and abs(StartRBBox[1] - EndRBBox[1]) > 10 ):
                        EndRBBox = self.PixelToWorld(EndRBBox)
                        StartRBBox = self.PixelToWorld(StartRBBox)
                        BB = array(((min(EndRBBox[0],StartRBBox[0]),
                                     min(EndRBBox[1],StartRBBox[1])),
                                    (max(EndRBBox[0],StartRBBox[0]),
                                     max(EndRBBox[1],StartRBBox[1]))),Float)
                        self.ZoomToBB(BB)
                    else:
                        Center = self.PixelToWorld(StartRBBox)
                        self.Zoom(1.5,Center)
                    self.StartRBBox = None
            elif self.GUIMode == "Move":
                if not self.StartMove is None:
                    StartMove = self.StartMove
                    EndMove = array((event.GetX(),event.GetY()))
                    if sum((StartMove-EndMove)**2) > 16:
                        self.MoveImage(StartMove-EndMove,'Pixel')
                    self.StartMove = None
            elif self.GUIMode == "Mouse":
                EventType = EVT_FC_LEFT_UP
                if not self.HitTest(event, EventType):
                   self._RaiseMouseEvent(event, EventType)
        else:
            pass

    def MotionEvent(self,event):
        if self.GUIMode:
            if self.GUIMode == "ZoomIn":
                if event.Dragging() and event.LeftIsDown() and not (self.StartRBBox is None):
                    xy0 = self.StartRBBox
                    xy1 = array( event.GetPosition() )
                    wh  = abs(xy1 - xy0)
                    wh[0] = max(wh[0], int(wh[1]*self.AspectRatio))
                    wh[1] = int(wh[0] / self.AspectRatio)
                    xy_c = (xy0 + xy1) / 2
                    dc = wx.ClientDC(self)
                    dc.BeginDrawing()
                    dc.SetPen(wx.Pen('WHITE', 2, wx.SHORT_DASH))
                    dc.SetBrush(wx.TRANSPARENT_BRUSH)
                    dc.SetLogicalFunction(wx.XOR)
                    if self.PrevRBBox:
                        dc.DrawRectanglePointSize(*self.PrevRBBox)
                    self.PrevRBBox = ( xy_c - wh/2, wh )
                    dc.DrawRectanglePointSize( *self.PrevRBBox )
                    dc.EndDrawing()
            elif self.GUIMode == "Move":
                if event.Dragging() and event.LeftIsDown() and not self.StartMove is None:
                    xy1 = array( event.GetPosition() )
                    wh = self.PanelSize
                    xy_tl = xy1 - self.StartMove
                    dc = wx.ClientDC(self)
                    dc.BeginDrawing()
                    x1,y1 = self.PrevMoveXY
                    x2,y2 = xy_tl
                    w,h = self.PanelSize
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
                    dc.SetBrush(self.BackgroundBrush)
                    dc.DrawRectangle(xa, ya, wa, ha)
                    dc.DrawRectangle(xb, yb, wb, hb)
                    self.PrevMoveXY = xy_tl
                    if self._ForegroundBuffer:
                        dc.DrawBitmapPoint(self._ForegroundBuffer,xy_tl)
                    else:
                        dc.DrawBitmapPoint(self._Buffer,xy_tl)
                    dc.EndDrawing()
            elif self.GUIMode == "Mouse":
                ## Only do something if there are mouse over events bound
                if self.HitDict and (self.HitDict[ EVT_FC_ENTER_OBJECT ] or self.HitDict[ EVT_FC_LEAVE_OBJECT ] ):
                    if not self.MouseOverTest(event):
                        self._RaiseMouseEvent(event,EVT_FC_MOTION)
            else:
                    pass
            self._RaiseMouseEvent(event,EVT_FC_MOTION)
        else:
            pass

    def RightDownEvent(self,event):
        if self.GUIMode:
            if self.GUIMode == "ZoomIn":
                Center = self.PixelToWorld((event.GetX(),event.GetY()))
                self.Zoom(1/1.5,Center)
            elif self.GUIMode == "ZoomOut":
                Center = self.PixelToWorld((event.GetX(),event.GetY()))
                self.Zoom(1.5,Center)
            elif self.GUIMode == "Mouse":
                EventType = EVT_FC_RIGHT_DOWN
                if not self.HitTest(event, EventType):
                   self._RaiseMouseEvent(event, EventType)
        else:
            pass
        
    def MakeNewBuffers(self):
        self._BackgroundDirty = True
        # Make new offscreen bitmap:
        self._Buffer = wx.EmptyBitmap(*self.PanelSize)
        #dc = wx.MemoryDC()
        #dc.SelectObject(self._Buffer)
        #dc.Clear()
        if self._ForeDrawList:
            self._ForegroundBuffer = wx.EmptyBitmap(*self.PanelSize)
        else:
            self._ForegroundBuffer = None
        if self.UseHitTest:
            self.MakeNewHTdc()
        else:
            self._HTdc = None
            self._ForegroundHTdc = None

    def MakeNewHTdc(self):
        ## Note: While it's considered a "bad idea" to keep a
        ## MemoryDC around I'm doing it here because a wx.Bitmap
        ## doesn't have a GetPixel method so a DC is needed to do
        ## the hit-test. It didn't seem like a good idea to re-create
        ## a wx.MemoryDC on every single mouse event, so I keep it
        ## around instead
        self._HTdc = wx.MemoryDC()
        self._HTBitmap = wx.EmptyBitmap(*self.PanelSize) 
        self._HTdc.SelectObject( self._HTBitmap )
        self._HTdc.SetBackground(wx.BLACK_BRUSH)
        if self._ForeDrawList:
            self._ForegroundHTdc = wx.MemoryDC()
            self._ForegroundHTBitmap = wx.EmptyBitmap(*self.PanelSize) 
            self._ForegroundHTdc.SelectObject( self._ForegroundHTBitmap )
            self._ForegroundHTdc.SetBackground(wx.BLACK_BRUSH)
        else:
           self._ForegroundHTdc = None 
    
    def OnSize(self,event):
        self.PanelSize  = array(self.GetClientSizeTuple(),Int32)
        self.HalfPanelSize = self.PanelSize / 2 # lrk: added for speed in WorldToPixel
        if self.PanelSize[0] == 0 or self.PanelSize[1] == 0:
            self.AspectRatio = 1.0
        else:
            self.AspectRatio = float(self.PanelSize[0]) / self.PanelSize[1]
        self.MakeNewBuffers()
        self.Draw()
        
    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        if self._ForegroundBuffer:
            dc.DrawBitmap(self._ForegroundBuffer,0,0)
        else:
            dc.DrawBitmap(self._Buffer,0,0)

    def Draw(self, Force=False):
        """
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
#        print "in Draw", self.PanelSize
        if sometrue(self.PanelSize < 1 ): # it's possible for this to get called before being properly initialized.
#        if self.PanelSize < (1,1): # it's possible for this to get called before being properly initialized.
            return
        if self.Debug: start = clock()
        ScreenDC =  wx.ClientDC(self)
        ViewPortWorld = ( self.PixelToWorld((0,0)),
                          self.PixelToWorld(self.PanelSize) )
        ViewPortBB = array( ( minimum.reduce(ViewPortWorld),
                              maximum.reduce(ViewPortWorld) ) )
        dc = wx.MemoryDC()
        dc.SelectObject(self._Buffer)
        if self._BackgroundDirty or Force:
            #print "Background is Dirty"
            dc.SetBackground(self.BackgroundBrush)
            dc.Clear()
            if self._HTdc:
                self._HTdc.Clear()
            self._DrawObjects(dc, self._DrawList, ScreenDC, ViewPortBB, self._HTdc)
            self._BackgroundDirty = False

        if self._ForeDrawList:
            ## If an object was just added to the Foreground, there might not yet be a buffer
            if self._ForegroundBuffer is None:
                self._ForegroundBuffer = wx.EmptyBitmap(self.PanelSize[0],
                                                        self.PanelSize[1])

            dc = wx.MemoryDC() ## I got some strange errors (linewidths wrong) if I didn't make a new DC here
            dc.SelectObject(self._ForegroundBuffer)
            dc.DrawBitmap(self._Buffer,0,0)
            if self._ForegroundHTdc is None:
                self._ForegroundHTdc = wx.MemoryDC()
                self._ForegroundHTdc.SelectObject( wx.EmptyBitmap(
                                                   self.PanelSize[0],
                                                   self.PanelSize[1]) )
            if self._HTdc:
                ## blit the background HT buffer to the foreground HT buffer
                self._ForegroundHTdc.Blit(0, 0,
                                          self.PanelSize[0], self.PanelSize[1],
                                          self._HTdc, 0, 0)
            self._DrawObjects(dc,
                              self._ForeDrawList,
                              ScreenDC,
                              ViewPortBB,
                              self._ForegroundHTdc)
        ScreenDC.Blit(0, 0, self.PanelSize[0],self.PanelSize[1], dc, 0, 0)
        # If the canvas is in the middle of a zoom or move, the Rubber Band box needs to be re-drawn
        # This seeems out of place, but it works.
        if self.PrevRBBox:
            ScreenDC.SetPen(wx.Pen('WHITE', 2,wx.SHORT_DASH))
            ScreenDC.SetBrush(wx.TRANSPARENT_BRUSH)
            ScreenDC.SetLogicalFunction(wx.XOR)
            ScreenDC.DrawRectanglePointSize(*self.PrevRBBox)
        if self.Debug: print "Drawing took %f seconds of CPU time"%(clock()-start)

        ## Clear the font cache
        ## IF you don't do this, the X font server starts to take up Massive amounts of memory
        ## This is mostly a problem with very large fonts, that you get with scaled text when zoomed in.
        DrawObject.FontList = {}

    def _ShouldRedraw(DrawList, ViewPortBB): # lrk: adapted code from BBCheck
        # lrk: Returns the objects that should be redrawn

        BB2 = ViewPortBB
        redrawlist = []
        for Object in DrawList:
            BB1 = Object.BoundingBox
            if (BB1[1,0] > BB2[0,0] and BB1[0,0] < BB2[1,0] and
                 BB1[1,1] > BB2[0,1] and BB1[0,1] < BB2[1,1]):
                redrawlist.append(Object)
        return redrawlist       
    _ShouldRedraw = staticmethod(_ShouldRedraw)


##    def BBCheck(self, BB1, BB2):
##        """

##        BBCheck(BB1, BB2) returns True is the Bounding boxes intesect, False otherwise

##        """
##        if ( (BB1[1,0] > BB2[0,0]) and (BB1[0,0] < BB2[1,0]) and
##             (BB1[1,1] > BB2[0,1]) and (BB1[0,1] < BB2[1,1]) ):
##            return True
##        else:
##            return False

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
        
        shift = asarray(shift,Float)
        #print "shifting by:", shift
        if CoordType == 'Panel':# convert from panel coordinates
            shift = shift * array((-1,1),Float) *self.PanelSize/self.TransformVector
        elif CoordType == 'Pixel': # convert from pixel coordinates
            shift = shift/self.TransformVector
        elif CoordType == 'World': # No conversion
            pass
        else:
            raise FloatCanvasError('CoordType must be either "Panel", "Pixel", or "World"')

        #print "shifting by:", shift
        
        self.ViewPortCenter = self.ViewPortCenter + shift 
        self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
        self.TransformVector = array((self.Scale,-self.Scale),Float) * self.MapProjectionVector
        self._BackgroundDirty = True
        self.Draw()

    def Zoom(self,factor,center = None):
    
        """
        Zoom(factor, center) changes the amount of zoom of the image by factor.
        If factor is greater than one, the image gets larger.
        If factor is less than one, the image gets smaller.
        
        Center is a tuple of (x,y) coordinates of the center of the viewport, after zooming.
        If center is not given, the center will stay the same.
        
        """
        self.Scale = self.Scale*factor
        if not center is None:
            self.ViewPortCenter = array(center,Float)
        self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
        self.TransformVector = array((self.Scale,-self.Scale),Float) * self.MapProjectionVector
        self._BackgroundDirty = True
        self.Draw()
        
    def ZoomToBB(self, NewBB = None, DrawFlag = True):

        """

        Zooms the image to the bounding box given, or to the bounding
        box of all the objects on the canvas, if none is given.

        """
        
        if not  NewBB is None:
            BoundingBox = NewBB
        else:
            if self.BoundingBoxDirty:
                self._ResetBoundingBox()
            BoundingBox = self.BoundingBox
        if not BoundingBox is None:
            self.ViewPortCenter = array(((BoundingBox[0,0]+BoundingBox[1,0])/2,
                                         (BoundingBox[0,1]+BoundingBox[1,1])/2 ),Float)
            self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
            # Compute the new Scale
            BoundingBox = BoundingBox * self.MapProjectionVector
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
                     
            self.TransformVector = array((self.Scale,-self.Scale),Float)* self.MapProjectionVector
            if DrawFlag:
                self._BackgroundDirty = True
                self.Draw()
        else:
            # Reset the shifting and scaling to defaults when there is no BB
            self.ViewPortCenter= array( (0,0), Float)
            self.MapProjectionVector = array( (1,1), Float) # No Projection to start!
            self.TransformVector = array( (1,-1), Float) # default Transformation
            self.Scale = 1
            
    def RemoveObjects(self, Objects):
        for Object in Objects:
            self.RemoveObject(Object, ResetBB = False)
        self.BoundingBoxDirty = True
        
    def RemoveObject(self, Object, ResetBB = True):
        ##fixme: Using the list.remove method is kind of slow
        if Object.InForeground:
            self._ForeDrawList.remove(Object)
        else:
            self._DrawList.remove(Object)
            self._BackgroundDirty = True
        if ResetBB:
            self.BoundingBoxDirty = True

    def ClearAll(self, ResetBB = True):
        self._DrawList = []
        self._ForeDrawList = []
        self._BackgroundDirty = True
        self.HitColorGenerator = None
        self.UseHitTest = False
        if ResetBB:
            self._ResetBoundingBox()
        self.MakeNewBuffers()
        self.HitDict = None

##    No longer called
##    def _AddBoundingBox(self,NewBB):
##        if self.BoundingBox is None:
##            self.BoundingBox = NewBB
##            self.ZoomToBB(NewBB,DrawFlag = False)
##        else:
##            self.BoundingBox = array( ( (min(self.BoundingBox[0,0],NewBB[0,0]),
##                                         min(self.BoundingBox[0,1],NewBB[0,1])),
##                                        (max(self.BoundingBox[1,0],NewBB[1,0]),
##                                         max(self.BoundingBox[1,1],NewBB[1,1]))),
##                                      Float)

    def _getboundingbox(bboxarray): # lrk: added this

        upperleft = minimum.reduce(bboxarray[:,0])
        lowerright = maximum.reduce(bboxarray[:,1])
        return array((upperleft, lowerright), Float)

    _getboundingbox = staticmethod(_getboundingbox)

    def _ResetBoundingBox(self):
        if self._DrawList or self._ForeDrawList:
            bboxarray = zeros((len(self._DrawList)+len(self._ForeDrawList), 2, 2),Float) 
            i = -1 # just in case _DrawList is empty
            for (i, BB) in enumerate(self._DrawList):
                bboxarray[i] = BB.BoundingBox
            for (j, BB) in enumerate(self._ForeDrawList):
                bboxarray[i+j+1] = BB.BoundingBox
            self.BoundingBox = self._getboundingbox(bboxarray)
        else:
            self.BoundingBox = None
            self.ViewPortCenter= array( (0,0), Float)
            self.TransformVector = array( (1,-1), Float)
            self.MapProjectionVector = array( (1,1), Float)			
            self.Scale = 1        
        self.BoundingBoxDirty = False

    def PixelToWorld(self,Points):
        """
        Converts coordinates from Pixel coordinates to world coordinates.
        
        Points is a tuple of (x,y) coordinates, or a list of such tuples, or a NX2 Numpy array of x,y coordinates.
        
        """
        return  (((asarray(Points,Float) - (self.PanelSize/2))/self.TransformVector) + self.ViewPortCenter)
        
    def WorldToPixel(self,Coordinates):
        """
        This function will get passed to the drawing functions of the objects,
        to transform from world to pixel coordinates.
        Coordinates should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """
        #Note: this can be called by users code for various reasons, so asarray is needed.
        return  (((asarray(Coordinates,Float) -
                   self.ViewPortCenter)*self.TransformVector)+
                 (self.HalfPanelSize)).astype('i')

    def ScaleWorldToPixel(self,Lengths):
        """
        This function will get passed to the drawing functions of the objects,
        to Change a length from world to pixel coordinates.
        
        Lengths should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """
        return  ( (asarray(Lengths,Float)*self.TransformVector) ).astype('i')

    def ScalePixelToWorld(self,Lengths):
        """
        This function computes a pair of x.y lengths,
        to change then from pixel to world coordinates.
        
        Lengths should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """

        return  (asarray(Lengths,Float) / self.TransformVector)
    
    def AddObject(self,obj):
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
            Object._Draw(dc, WorldToPixel, ScaleWorldToPixel, HTdc)
            if i+1 % NumBetweenBlits == 0:
                Blit(0, 0, PanelSize0, PanelSize1, dc, 0, 0)
        dc.EndDrawing()

##    ## This is a way to automatically add a AddObject method for each
##    ## object type This code has been replaced by Leo's code above, so
##    ## that it happens at module init, rather than as needed. The
##    ## primary advantage of this is that dir(FloatCanvas) will have
##    ## them, and docstrings are preserved. Probably more useful
##    ## exceptions if there is a problem, as well.
##    def __getattr__(self, name):
##        if name[:3] == "Add":
##            func=globals()[name[3:]]
##            def AddFun(*args, **kwargs):
##                Object = func(*args, **kwargs)
##                self.AddObject(Object)
##                return Object
##            ## add it to FloatCanvas' dict for future calls.
##            self.__dict__[name] = AddFun
##            return AddFun
##        else:
##            raise AttributeError("FloatCanvas has no attribute '%s'"%name)

def _makeFloatCanvasAddMethods(): ## lrk's code for doing this in module __init__
    classnames = ["Circle", "Ellipse", "Rectangle", "ScaledText", "Polygon",
               "Line", "Text", "PointSet","Point", "Arrow"]
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


