#!/usr/bin/env python2.2

from Numeric import array,Float,cos,pi,sum,minimum,maximum

from time import clock
from wxPython.wx import *
import types
import os        

ID_ZOOM_IN_BUTTON = wxNewId()
ID_ZOOM_OUT_BUTTON = wxNewId()
ID_ZOOM_TO_FIT_BUTTON = wxNewId()
ID_MOVE_MODE_BUTTON = wxNewId()
ID_TEST_BUTTON = wxNewId()
ID_ABOUT_MENU = wxNewId()          
ID_EXIT_MENU  = wxNewId() 
ID_ZOOM_TO_FIT_MENU = wxNewId()
ID_DRAWTEST_MENU = wxNewId()
ID_DRAWMAP_MENU = wxNewId()
ID_CLEAR_MENU = wxNewId()

ID_TEST = wxNewId()


### These are some functions for bitmaps of icons.
import cPickle, zlib

def GetHandData():
    return cPickle.loads(zlib.decompress(
'x\xda\xd3\xc8)0\xe4\nV72T\x00!\x05Cu\xae\xc4`u=\x85d\x05\xa7\x9c\xc4\xe4l0O\
\x01\xc8S\xb6t\x06A(\x1f\x0b\xa0\xa9\x8c\x9e\x1e6\x19\xa0\xa8\x1e\x88\xd4C\
\x97\xd1\x83\xe8\x80 \x9c2zh\xa6\xc1\x11X\n\xab\x8c\x02\x8a\x0cD!\x92\x12\
\x98\x8c\x1e\x8a\x8b\xd1d\x14\xf4\x90%\x90LC\xf6\xbf\x1e\xba\xab\x91%\xd0\
\xdc\x86C\x06\xd9m\xe8!\xaa\x87S\x86\x1a1\xa7\x07\x00v\x0f[\x17' ))

def GetHandBitmap():
    return wxBitmapFromXPMData(GetHandData())

#----------------------------------------------------------------------
def GetPlusData():
    return cPickle.loads(zlib.decompress(
'x\xda\xd3\xc8)0\xe4\nV72T\x00!\x05Cu\xae\xc4`u=\x85d\x05\xa7\x9c\xc4\xe4l0O\
\x01\xc8S\xb6t\x06A(\x1f\x0b RF\x0f\x08\xb0\xc9@D\xe1r\x08\x19\xb8j=l2`\r\
\xe82HF\xe9a\xc8\xe8\xe9A\x9c@\x8a\x0c\x0e\xd3p\xbb\x00\x8f\xab\xe1>\xd5\xd3\
\xc3\x15:P)l!\n\x91\xc2\x1a\xd6`)\xec\xb1\x00\x92\xc2\x11?\xb8e\x88\x8fSt\
\x19=\x00\x82\x16[\xf7' ))

def GetPlusBitmap():
    return wxBitmapFromXPMData(GetPlusData())

#----------------------------------------------------------------------
def GetMinusData():
    return cPickle.loads(zlib.decompress(
'x\xda\xd3\xc8)0\xe4\nV72T\x00!\x05Cu\xae\xc4`u=\x85d\x05\xa7\x9c\xc4\xe4l0O\
\x01\xc8S\xb6t\x06A(\x1f\x0b RF\x0f\x08\xb0\xc9@D\xe1r\x08\x19\xb8j=\xa2e\
\x10\x16@\x99\xc82zz\x10\'\x90"\x83\xc34r\xdc\x86\xf0\xa9\x9e\x1e\xae\xd0\
\x81Ja\x0bQ\x88\x14\xd6\xb0\x06Ka\x8f\x05\x90\x14\x8e\xf8\xc1-C|\x9c\xa2\xcb\
\xe8\x01\x00\xed\x0f[\x87' ))

def GetMinusBitmap():
    return wxBitmapFromXPMData(GetMinusData())

## This is a bunch of stuff for implimenting interactive use: catching
## when objects are clicked on by the mouse, etc. I've made a start, so if
## you are interesed in making that work, let me know, I may have gotten
## it going by then

#### I probably want a full set of events someday:
## #### mouse over, right click, left click mouse up etc, etc.
## ##FLOATCANVAS_EVENT_LEFT_DOWN = wxNewEventType()
## ##FLOATCANVAS_EVENT_LEFT_UP = wxNewEventType()
## ##FLOATCANVAS_EVENT_RIGHT_DOWN = wxNewEventType()
## ##FLOATCANVAS_EVENT_RIGHT_UP = wxNewEventType()
## ##FLOATCANVAS_EVENT_MOUSE_OVER = wxNewEventType()

##WXFLOATCANVASEVENT = wxNewEventType()

##def EVT_WXFLOATCANVASEVENT( window, function ): 

##    """Your documentation here""" 

##    window.Connect( -1, -1, WXFLOATCANVASEVENT, function ) 

##class wxFloatCanvasObjectEvent(wxPyCommandEvent):
##    def __init__(self, WindowID,Object):
##        wxPyCommandEvent.__init__(self, WXFLOATCANVASEVENT, WindowID)
##        self.Object = Object

##    def Clone( self ): 
##        self.__class__( self.GetId() ) 

##class ColorGenerator:

##    """ An instance of this class generates a unique color each time
##    GetNextColor() is called. Someday I will use a proper Python
##    generator for this class.

##    The point of this generator is for the hit-test bitmap, each object
##    needs to be a unique color. Also, each system can be running a
##    different number of colors, and it doesn't appear to be possible to
##    have a wxMemDC with a different colordepth as the screen so this
##    generates colors far enough apart that they can be distinguished on
##    a 16bit screen. Anything less than 16bits won't work.
##    """

##    def __init__(self,depth = 16):
##        self.r = 0
##        self.g = 0
##        self.b = 0
##        if depth == 16:
##            self.step = 8
##        elif depth >= 24:
##            self.step = 1
##        else:
##            raise "ColorGenerator does not work with depth = %s"%depth

##    def GetNextColor(self):
##        step = self.step
##        ##r,g,b = self.r,self.g,self.b
##        self.r += step
##        if self.r > 255:
##            self.r = step
##            self.g += step
##            if self.g > 255:
##                self.g = step
##                self.b += step
##                if self.b > 255:
##                    ## fixme: this should be a derived exception
##                    raise "Too many objects for HitTest"
##        return (self.r,self.g,self.b)


class draw_object:
        """
        This is the base class for all the objects that can be drawn.
        
        each object has the following properties; (incomplete)
        
        BoundingBox :  is of the form: array((min_x,min_y),(max_x,max_y)) 
        Pen
        Brush

        """

        def __init__(self,Foreground  = 0):
            self.Foreground = Foreground

            self._Canvas = None

        # I pre-define all these as class variables to provide an easier
        # interface, and perhaps speed things up by caching all the Pens
        # and Brushes, although that may not help, as I think wx now
        # does that on it's own. Send me a note if you know!

        BrushList = {
                ( None,"Transparent")  : wxTRANSPARENT_BRUSH,
                ("Blue","Solid")       : wxBLUE_BRUSH,
                ("Green","Solid")      : wxGREEN_BRUSH,
                ("White","Solid")      : wxWHITE_BRUSH,
                ("Black","Solid")      : wxBLACK_BRUSH,
                ("Grey","Solid")       : wxGREY_BRUSH,
                ("MediumGrey","Solid") : wxMEDIUM_GREY_BRUSH,
                ("LightGrey","Solid")  : wxLIGHT_GREY_BRUSH,
                ("Cyan","Solid")       : wxCYAN_BRUSH,
                ("Red","Solid")        : wxRED_BRUSH
                        }
        PenList = {
                (None,"Transparent",1)   : wxTRANSPARENT_PEN,
                ("Green","Solid",1)      : wxGREEN_PEN,
                ("White","Solid",1)      : wxWHITE_PEN,
                ("Black","Solid",1)      : wxBLACK_PEN,
                ("Grey","Solid",1)       : wxGREY_PEN,
                ("MediumGrey","Solid",1) : wxMEDIUM_GREY_PEN,
                ("LightGrey","Solid",1)  : wxLIGHT_GREY_PEN,
                ("Cyan","Solid",1)       : wxCYAN_PEN,
                ("Red","Solid",1)        : wxRED_PEN
                }
        
        FillStyleList = {
                "Transparent"    : wxTRANSPARENT,
                "Solid"          : wxSOLID,
                "BiDiagonalHatch": wxBDIAGONAL_HATCH,
                "CrossDiagHatch" : wxCROSSDIAG_HATCH,
                "FDiagonal_Hatch": wxFDIAGONAL_HATCH,
                "CrossHatch"     : wxCROSS_HATCH,
                "HorizontalHatch": wxHORIZONTAL_HATCH,
                "VerticalHatch"  : wxVERTICAL_HATCH
                }
        
        LineStyleList = {
                "Solid"      : wxSOLID,
                "Transparent": wxTRANSPARENT,
                "Dot"        : wxDOT,
                "LongDash"   : wxLONG_DASH,
                "ShortDash"  : wxSHORT_DASH,
                "DotDash"    : wxDOT_DASH,
                }
        
        def SetBrush(self,FillColor,FillStyle):
            if FillColor is None or FillStyle is None:
                self.Brush = wxTRANSPARENT_BRUSH
                self.FillStyle = "Transparent"
            else:
                if not self.BrushList.has_key((FillColor,FillStyle)):
                    self.BrushList[(FillColor,FillStyle)] = wxBrush(FillColor,self.FillStyleList[FillStyle])
                self.Brush = self.BrushList[(FillColor,FillStyle)]
                
        def SetPen(self,LineColor,LineStyle,LineWidth):
            if LineColor is None or LineStyle is None:
                self.Pen = wxTRANSPARENT_PEN
                self.LineStyle = 'Transparent'
            if not self.PenList.has_key((LineColor,LineStyle,LineWidth)):
                self.PenList[(LineColor,LineStyle,LineWidth)] = wxPen(LineColor,LineWidth,self.LineStyleList[LineStyle])
            self.Pen = self.PenList[(LineColor,LineStyle,LineWidth)]

        def SetPens(self,LineColors,LineStyles,LineWidths):
            """
            This method used when an object could have a list of pens, rather than just one
            It is used for LineSet, and perhaps others in the future.

            fixme: this is really kludgy, there has got to be a better way!
            
            """

            length = 1
            if type(LineColors) == types.ListType:
                length = len(LineColors)
            else:
                LineColors = [LineColors]

            if type(LineStyles) == types.ListType:
                length = len(LineStyles)
            else:
                LineStyles = [LineStyles]

            if type(LineWidths) == types.ListType:
                length = len(LineWidths)
            else:
                LineWidths = [LineWidths]

            if length > 1:
                if len(LineColors) == 1:
                    LineColors = LineColors*length
                if len(LineStyles) == 1:
                    LineStyles = LineStyles*length
                if len(LineWidths) == 1:
                    LineWidths = LineWidths*length

            self.Pens = []
            for (LineColor,LineStyle,LineWidth) in zip(LineColors,LineStyles,LineWidths):
                if LineColor is None or LineStyle is None:
                    self.Pens.append(wxTRANSPARENT_PEN)
                    # what's this for?> self.LineStyle = 'Transparent'
                if not self.PenList.has_key((LineColor,LineStyle,LineWidth)):
                    Pen = wxPen(LineColor,LineWidth,self.LineStyleList[LineStyle])
                    self.Pens.append(Pen)
                else:
                    self.Pens.append(self.PenList[(LineColor,LineStyle,LineWidth)])
            if length == 1:
                self.Pens = self.Pens[0]

        def PutInBackground(self):
            if self._Canvas and self.Foreground:
                self._Canvas._TopDrawList.remove(self)
                self._Canvas._DrawList.append(self)
                self._Canvas._BackgroundDirty = 1
                self.Foreground = 0

        def PutInForeground(self):
            if self._Canvas and (not self.Foreground):
                self._Canvas._TopDrawList.append(self)
                self._Canvas._DrawList.remove(self)
                self._Canvas._BackgroundDirty = 1
                self.Foreground = 1
            
        
class Polygon(draw_object):

    """

    The Polygon class takes a list of 2-tuples, or a NX2 NumPy array of
    point coordinates.  so that Points[N][0] is the x-coordinate of
    point N and Points[N][1] is the y-coordinate or Points[N,0] is the
    x-coordinate of point N and Points[N,1] is the y-coordinate for
    arrays.

    """
    def __init__(self,Points,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground = 0):
        draw_object.__init__(self,Foreground)
        self.Points = array(Points,Float)
        self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)


    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        Points = WorldToPixel(self.Points)
        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        #dc.DrawPolygon(map(lambda x: (x[0],x[1]), Points.tolist()))
        dc.DrawPolygon(Points)

class PolygonSet(draw_object):
    """
    The PolygonSet class takes a Geometry.Polygon object.
    so that Points[N] = (x1,y1) and Points[N+1] = (x2,y2). N must be an even number!
    
    it creates a set of line segments, from (x1,y1) to (x2,y2)
    
    """
    
    def __init__(self,PolySet,LineColors,LineStyles,LineWidths,FillColors,FillStyles,Foreground = 0):
        draw_object.__init__(self, Foreground)

        ##fixme: there should be some error checking for everything being the right length.

        
        self.Points = array(Points,Float)
        self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)

        self.LineColors = LineColors
        self.LineStyles = LineStyles
        self.LineWidths = LineWidths
        self.FillColors = FillColors
        self.FillStyles = FillStyles

        self.SetPens(LineColors,LineStyles,LineWidths)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        Points = WorldToPixel(self.Points)
        Points.shape = (-1,4)
        dc.DrawLineList(Points,self.Pens)
 

class Line(draw_object):
    """
    The Line class takes a list of 2-tuples, or a NX2 NumPy array of point coordinates.
    so that Points[N][0] is the x-coordinate of point N and Points[N][1] is the y-coordinate
    or  Points[N,0] is the x-coordinate of point N and Points[N,1] is the y-coordinate for arrays.

    It will draw a straight line if there are two points, and a polyline if there are more than two.

    """
    def __init__(self,Points,LineColor,LineStyle,LineWidth,Foreground = 0):
        draw_object.__init__(self, Foreground)

        self.Points = array(Points,Float)
        self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth

        self.SetPen(LineColor,LineStyle,LineWidth)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        Points = WorldToPixel(self.Points)
        dc.SetPen(self.Pen)
        #dc.DrawLines(map(lambda x: (x[0],x[1]), Points.tolist()))
        dc.DrawLines(Points)


class LineSet(draw_object):
    """
    The LineSet class takes a list of 2-tuples, or a NX2 NumPy array of point coordinates.
    so that Points[N] = (x1,y1) and Points[N+1] = (x2,y2). N must be an even number!
    
    it creates a set of line segments, from (x1,y1) to (x2,y2)
    
    """
    
    def __init__(self,Points,LineColors,LineStyles,LineWidths,Foreground = 0):
        draw_object.__init__(self, Foreground)

        NumLines = len(Points) / 2
        ##fixme: there should be some error checking for everything being the right length.

        
        self.Points = array(Points,Float)
        self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)

        self.LineColors = LineColors
        self.LineStyles = LineStyles
        self.LineWidths = LineWidths

        self.SetPens(LineColors,LineStyles,LineWidths)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        Points = WorldToPixel(self.Points)
        Points.shape = (-1,4)
        dc.DrawLineList(Points,self.Pens)
 

class PointSet(draw_object):
        """
        The PointSet class takes a list of 2-tuples, or a NX2 NumPy array of point coordinates.
        so that Points[N][0] is the x-coordinate of point N and Points[N][1] is the y-coordinate
        or  Points[N,0] is the x-coordinate of point N and Points[N,1] is the y-coordinate for arrays.
        
        Each point will be drawn the same color and Diameter. The Diameter is in screen points,
        not world coordinates.
        
        """
        def __init__(self,Points,Color,Diameter,Foreground = 0):
            draw_object.__init__(self,Foreground)

            self.Points = array(Points,Float)
            self.Points.shape = (-1,2) # Make sure it is a NX2 array, even if there is only one point
            self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)
            
            self.Color = Color
            self.Diameter = Diameter
            
            self.SetPen(Color,"Solid",1)
            self.SetBrush(Color,"Solid")

        def SetPoints(self,Points):
            self.Points = Points
            self.BoundingBox = array(((min(self.Points[:,0]),min(self.Points[:,1])),(max(self.Points[:,0]),max(self.Points[:,1]))),Float)
            if self._Canvas:
                # It looks like this shouldn't be private
                self._Canvas.BoundingBoxDirty = 1
            
        def _Draw(self,dc,WorldToPixel,ScaleFunction):
            dc.SetPen(self.Pen)
            Points = WorldToPixel(self.Points)
            if self.Diameter <= 1:
                dc.DrawPointList(Points)
            elif self.Diameter <= 2:
                # A Little optimization for a diameter2 - point
                dc.DrawPointList(Points)
                dc.DrawPointList(Points + (1,0))
                dc.DrawPointList(Points + (0,1))
                dc.DrawPointList(Points + (1,1))
            else:
                dc.SetBrush(self.Brush)
                radius = int(round(self.Diameter/2))
                for (x,y) in Points:
                    dc.DrawEllipse((x - radius), (y - radius), self.Diameter, self.Diameter)


                    
class Dot(draw_object):
    """
    The Dot class takes an x.y coordinate pair, and the Diameter of the circle.
    The Diameter is in pixels, so it won't change with zoom.

    Also Fill and line data

    """
    def __init__(self,x,y,Diameter,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground = 0):
        draw_object.__init__(self,Foreground)

        self.X = x
        self.Y = y
        self.Diameter = Diameter
        # NOTE: the bounding box does not include the diameter of the dot, as that is in pixel coords.
        # If this is  a problem, perhaps you should use a circle, instead!
        self.BoundingBox = array(((x,y),(x,y)),Float)

        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        radius = int(round(self.Diameter/2))
        (X,Y) = WorldToPixel((self.X,self.Y))
        dc.DrawEllipse((X - radius), (Y - radius), self.Diameter, self.Diameter)


class Rectangle(draw_object):
    def __init__(self,x,y,width,height,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground = 0):
        draw_object.__init__(self,Foreground)

        self.X = x
        self.Y = y
        self.Width = width
        self.Height = height
        self.BoundingBox = array(((x,y),(x+width,y+height)),Float)
        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        (X,Y) = WorldToPixel((self.X,self.Y))
        (Width,Height) = ScaleFunction((self.Width,self.Height))

        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        dc.DrawRectangle(X,Y,Width,Height)

class Ellipse(draw_object):
    def __init__(self,x,y,width,height,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground = 0):
        draw_object.__init__(self,Foreground)

        self.X = x
        self.Y = y
        self.Width = width
        self.Height = height
        self.BoundingBox = array(((x,y),(x+width,y+height)),Float)
        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        (X,Y) = WorldToPixel((self.X,self.Y))
        (Width,Height) = ScaleFunction((self.Width,self.Height))

        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        dc.DrawEllipse(X,Y,Width,Height)

class Circle(draw_object):
    def __init__(self,x,y,Diameter,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground = 0):
        draw_object.__init__(self,Foreground)

        self.X = x
        self.Y = y
        self.Diameter = Diameter
        self.BoundingBox = array(((x-Diameter/2,y-Diameter/2),(x+Diameter/2,y+Diameter/2)),Float)
        self.LineColor = LineColor
        self.LineStyle = LineStyle
        self.LineWidth = LineWidth
        self.FillColor = FillColor
        self.FillStyle = FillStyle

        self.SetPen(LineColor,LineStyle,LineWidth)
        self.SetBrush(FillColor,FillStyle)

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        (X,Y) = WorldToPixel((self.X,self.Y))
        (Diameter,dummy) = ScaleFunction((self.Diameter,self.Diameter))

        dc.SetPen(self.Pen)
        dc.SetBrush(self.Brush)
        dc.DrawEllipse(X-Diameter/2,Y-Diameter/2,Diameter,Diameter)

class Text(draw_object):
    """

    This class creates a text object, placed at the coordinates,
    x,y. the "Position" argument is a two charactor string, indicating
    where in relation to the coordinates the string should be oriented.

    The first letter is:  t, c, or b, for top, center and bottom
    The second letter is: l, c, or r, for left, center and right

    I've provided arguments for Family, Style, and Weight of font, but
    have not yet implimented them, so all text is:  wxSWISS, wxNORMAL, wxNORMAL.
    I'd love it if someone would impliment that!

    The size is fixed, and does not scale with the drawing.

    """
    
    def __init__(self,String,x,y,Size,ForeGround,BackGround,Family,Style,Weight,Underline,Position,Foreground = 0):
        draw_object.__init__(self,Foreground)

        self.String = String
        self.Size = Size

        self.ForeGround = ForeGround
        if BackGround is None:
            self.BackGround = None
        else:
            self.BackGround = BackGround
        self.Family = Family
        self.Style  = Style
        self.Weight = Weight
        self.Underline = Underline
        self.Position = Position
        # fixme: this should use the passed in parameters!
        self.Font = wxFont(Size, wxMODERN, wxNORMAL, wxNORMAL, Underline, )

        self.BoundingBox = array(((x,y),(x,y)),Float)

        self.X = x
        self.Y = y
        self.x_shift = None
        self.y_shift = None

    def _Draw(self,dc,WorldToPixel,ScaleFunction):
        (X,Y) = WorldToPixel((self.X,self.Y))
        dc.SetFont(self.Font)
        dc.SetTextForeground(self.ForeGround)
        if self.BackGround:
            dc.SetBackgroundMode(wxSOLID)
            dc.SetTextBackground(self.BackGround)
        else:
            dc.SetBackgroundMode(wxTRANSPARENT)

        # compute the shift, and adjust the coordinates, if neccesary
        # This had to be put in here, becsuse there is no wxDC during __init__
        if self.x_shift is None or self.y_shift is None:
            if self.Position == 'tl':
                x_shift,y_shift = 0,0
            else:
                (w,h) = dc.GetTextExtent(self.String)
                if self.Position[0] == 't':
                    y_shift = 0
                elif self.Position[0] == 'c':
                    y_shift = h/2
                elif self.Position[0] == 'b':
                    y_shift = h
                else:
                    ##fixme: this should be a real derived exception
                    raise "Invalid value for Text Object Position Attribute"
                if self.Position[1] == 'l':
                    x_shift = 0
                elif self.Position[1] == 'c':
                    x_shift = w/2
                elif self.Position[1] == 'r':
                    x_shift = w
                else:
                    ##fixme: this should be a real derived exception
                    raise "Invalid value for Text Object Position Attribute"
            self.x_shift = x_shift
            self.y_shift = y_shift
        dc.DrawText(self.String, X-self.x_shift, Y-self.y_shift)


#---------------------------------------------------------------------------
    
class FloatCanvas(wxPanel):
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

    It relies on NumPy, which is needed for speed

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
    you have to call the wxDC.DrawSomething call each time. This is plenty
    fast for tens of objects, OK for hundreds of objects, but pretty darn
    slow for thousands of objects.

    The solution is to be able to pass some sort of object set to the DC
    directly. I've used DC.DrawPointList(Points), and it helped a lot with
    drawing lots of points. I havn't got a LineSet type object, so I havn't
    used DC.DrawLineList yet. I'd like to get a full set of DrawStuffList()
    methods implimented, and then I'd also have a full set of Object sets
    that could take advantage of them. I hope to get to it some day.

    Copyright: Christopher Barker

    License: Same as wxPython

    Please let me know if you're using this!!!

    Contact me at:

    Chris.Barker@noaa.gov

    """ 
    
    def __init__(self, parent, id = -1,
                 size = wxDefaultSize,
                 ProjectionFun = None,
                 BackgroundColor = "WHITE",
                 Debug = 0,
                 EnclosingFrame = None,
                 UseToolbar = 1,
                 UseBackground = 0,
                 UseHitTest = 0):

        wxPanel.__init__( self, parent, id, wxDefaultPosition, size)

        if ProjectionFun == 'FlatEarth':
            self.ProjectionFun = self.FlatEarthProjection 
        elif type(ProjectionFun) == types.FunctionType:
            self.ProjectionFun = ProjectionFun 
        elif ProjectionFun is None:
            self.ProjectionFun = lambda x=None: array( (1,1), Float)
        else:
            raise('Projectionfun must be either: "FlatEarth", None, or a function that takes the ViewPortCenter and returns a MapProjectionVector')

        self.UseBackground = UseBackground
        self.UseHitTest = UseHitTest

        self.NumBetweenBlits = 40

        ## you can have a toolbar with buttons for zoom-in, zoom-out and
        ## move.  If you don't use the toolbar, you should provide your
        ## own way of navigating the canvas
        if UseToolbar:
            ## Create the vertical sizer for the toolbar and Panel
            box = wxBoxSizer(wxVERTICAL)
            box.Add(self.BuildToolbar(), 0, wxALL | wxALIGN_LEFT | wxGROW, 4)
            
            self.DrawPanel = wxWindow(self,-1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER)
            box.Add(self.DrawPanel,1,wxGROW)
            
            box.Fit(self)
            self.SetAutoLayout(True)
            self.SetSizer(box)
        else:
            self.DrawPanel = self

        self.DrawPanel.BackgroundBrush = wxBrush(BackgroundColor,wxSOLID)

        self.Debug = Debug

        self.EnclosingFrame = EnclosingFrame
        
        EVT_PAINT(self.DrawPanel, self.OnPaint)
        EVT_SIZE(self.DrawPanel, self.OnSize)
        
        EVT_LEFT_DOWN(self.DrawPanel, self.LeftButtonEvent)
        EVT_LEFT_UP(self.DrawPanel, self.LeftButtonEvent)
        EVT_RIGHT_DOWN(self.DrawPanel, self.RightButtonEvent)
        EVT_MOTION(self.DrawPanel,    self.LeftButtonEvent)
        
        
        self._DrawList = []
        if self.UseBackground:
            self._TopDrawList = []
        self.BoundingBox = None
        self.BoundingBoxDirty = 0
        self.ViewPortCenter= array( (0,0), Float)
        
        self.MapProjectionVector = array( (1,1), Float) # No Projection to start!
        self.TransformVector = array( (1,-1), Float) # default Transformation
        
        self.Scale = 1

        # called just to make sure everything is initialized
        self.OnSize(None)

        self.GUIMode = None
        self.StartRBBox = None
        self.StartMove = None

    def BuildToolbar(self):
        tb = wxToolBar(self,-1)
        self.ToolBar = tb
        
        tb.AddTool(ID_ZOOM_IN_BUTTON, GetPlusBitmap(), isToggle=true,shortHelpString = "Zoom In")
        EVT_TOOL(self, ID_ZOOM_IN_BUTTON, self.SetMode)
        
        tb.AddTool(ID_ZOOM_OUT_BUTTON, GetMinusBitmap(), isToggle=true,shortHelpString = "Zoom Out")
        EVT_TOOL(self, ID_ZOOM_OUT_BUTTON, self.SetMode)
        
        tb.AddTool(ID_MOVE_MODE_BUTTON, GetHandBitmap(), isToggle=true,shortHelpString = "Move")
        EVT_TOOL(self, ID_MOVE_MODE_BUTTON, self.SetMode)
        
        tb.AddSeparator()
        
        tb.AddControl(wxButton(tb, ID_ZOOM_TO_FIT_BUTTON, "Zoom To Fit",wxDefaultPosition, wxDefaultSize))
        EVT_BUTTON(self, ID_ZOOM_TO_FIT_BUTTON, self.ZoomToFit)

        tb.Realize()
        return tb

    def SetMode(self,event):
        for id in [ID_ZOOM_IN_BUTTON,ID_ZOOM_OUT_BUTTON,ID_MOVE_MODE_BUTTON]:
            self.ToolBar.ToggleTool(id,0)
        self.ToolBar.ToggleTool(event.GetId(),1)
        if event.GetId() == ID_ZOOM_IN_BUTTON:
            self.SetGUIMode("ZoomIn")
        elif event.GetId() == ID_ZOOM_OUT_BUTTON:
            self.SetGUIMode("ZoomOut")
        elif event.GetId() == ID_MOVE_MODE_BUTTON:
            self.SetGUIMode("Move")

    
    def SetGUIMode(self,Mode):
        if Mode in ["ZoomIn","ZoomOut","Move",None]:
            self.GUIMode = Mode
        else:
            raise "Not a valid Mode"
            
    def FlatEarthProjection(self,CenterPoint):
        return array((cos(pi*CenterPoint[1]/180),1),Float)
        
    def LeftButtonEvent(self,event):
        if self.EnclosingFrame:
            if event.Moving:
                position = self.PixelToWorld((event.GetX(),event.GetY()))
                self.EnclosingFrame.SetStatusText("%8.3f, %8.3f"%tuple(position))
        if self.GUIMode:
            if self.GUIMode == "ZoomIn":
                if event.LeftDown():
                    self.StartRBBox = (event.GetX(),event.GetY())
                    self.PrevRBBox = None
                elif event.Dragging() and event.LeftIsDown() and self.StartRBBox:
                    x0,y0 = self.StartRBBox
                    x1,y1 = event.GetX(),event.GetY()
                    w, h = abs(x1-x0),abs(y1-y0)
                    w = max(w,int(h*self.AspectRatio))
                    h = int(w/self.AspectRatio)
                    x_c, y_c = (x0+x1)/2 , (y0+y1)/2
                    dc = wxClientDC(self.DrawPanel)
                    dc.BeginDrawing()
                    dc.SetPen(wxPen('WHITE', 2,wxSHORT_DASH))
                    dc.SetBrush(wxTRANSPARENT_BRUSH)
                    dc.SetLogicalFunction(wxXOR)
                    if self.PrevRBBox:
                        dc.DrawRectangle(*self.PrevRBBox)
                    dc.DrawRectangle(x_c-w/2,y_c-h/2,w,h)
                    self.PrevRBBox = (x_c-w/2,y_c-h/2,w,h)
                    dc.EndDrawing()
                    
                elif event.LeftUp() and self.StartRBBox :
                    EndRBBox = (event.GetX(),event.GetY())
                    StartRBBox = self.StartRBBox
                    # if mouse has moved less that ten pixels, don't use the box.
                    if abs(StartRBBox[0] - EndRBBox[0]) > 10 and abs(StartRBBox[1] - EndRBBox[1]) > 10:
                        EndRBBox = self.PixelToWorld(EndRBBox)
                        StartRBBox = self.PixelToWorld(StartRBBox)
                        BB = array(((min(EndRBBox[0],StartRBBox[0]), min(EndRBBox[1],StartRBBox[1])),
                                    (max(EndRBBox[0],StartRBBox[0]), max(EndRBBox[1],StartRBBox[1]))),Float)
                        self.ZoomToBB(BB)
                    else:
                        Center = self.PixelToWorld(StartRBBox)
                        self.Zoom(1.5,Center)
                    self.StartRBBox = None
            if self.GUIMode == "ZoomOut":
                if event.LeftDown():
                    Center = self.PixelToWorld((event.GetX(),event.GetY()))
                    self.Zoom(1/1.5,Center)
            elif self.GUIMode == "Move":
                if event.LeftDown():
                    self.StartMove = array((event.GetX(),event.GetY()))
                    self.PrevMoveBox = None
                elif event.Dragging() and event.LeftIsDown() and self.StartMove:
                    x_1,y_1 = event.GetX(),event.GetY()
                    w, h = self.PanelSize
                    x_tl, y_tl = x_1 - self.StartMove[0], y_1 - self.StartMove[1]
                    dc = wxClientDC(self.DrawPanel)
                    dc.BeginDrawing()
                    dc.SetPen(wxPen('WHITE', 1,))
                    dc.SetBrush(wxTRANSPARENT_BRUSH)
                    dc.SetLogicalFunction(wxXOR)
                    if self.PrevMoveBox:
                        dc.DrawRectangle(*self.PrevMoveBox)
                    dc.DrawRectangle(x_tl,y_tl,w,h)
                    self.PrevMoveBox = (x_tl,y_tl,w,h)
                    dc.EndDrawing()
                  
                elif event.LeftUp() and self.StartMove:
                    StartMove = self.StartMove
                    EndMove = array((event.GetX(),event.GetY()))
                    if sum((StartMove-EndMove)**2) > 16:
                        self.Move(StartMove-EndMove,'Pixel')
                    self.StartMove = None
                    
    def RightButtonEvent(self,event):
        if self.GUIMode:
            if self.GUIMode == "ZoomIn":
                Center = self.PixelToWorld((event.GetX(),event.GetY()))
                self.Zoom(1/1.5,Center)
            elif self.GUIMode == "ZoomOut":
                Center = self.PixelToWorld((event.GetX(),event.GetY()))
                self.Zoom(1.5,Center)
            else:
                event.Skip()

    def MakeNewBuffers(self):
        # Make new offscreen bitmap:
        self._Buffer = wxEmptyBitmap(int(self.PanelSize[0]), int(self.PanelSize[1]))
        if self.UseBackground:
            self._BackBuffer = wxEmptyBitmap((self.PanelSize[0]), (self.PanelSize[1]))
            self._BackgroundDirty = 1
        else:
            pass
                
    def OnSize(self,event):
        self.PanelSize  = array(self.DrawPanel.GetClientSizeTuple(),Float)
        try:
            self.AspectRatio = self.PanelSize[0]/self.PanelSize[1]
        except ZeroDivisionError:
            self.AspectRatio = 1.0
        self.MakeNewBuffers()
        self.Draw()
        
    def OnPaint(self, event):
        #dc = wxBufferedPaintDC(self.DrawPanel, self._Buffer)
        dc = wxPaintDC(self.DrawPanel)
        dc.DrawBitmap(self._Buffer,0,0)

    def Draw(self):
        """
        The Draw method gets pretty complicated because of all the buffers

        There is a main buffer set up to double buffer the screen, so
        you can get quick re-draws when the window gets uncovered.

        If self.UseBackground is set, and an object is set up with the
        "ForeGround" flag, then it gets drawn to the screen after blitting
        the background. This is done so that you can have a complicated
        background, but have something changing on the foreground,
        without having to wait for the background to get re-drawn. This
        can be used to support simple animation, for instance.
        
        """
        if self.Debug: start = clock()
        ScreenDC =  wxClientDC(self.DrawPanel)
        if self.UseBackground:
            dc = wxMemoryDC()
            dc.SelectObject(self._BackBuffer)
            dc.SetBackground(self.DrawPanel.BackgroundBrush)
            if self._DrawList:
                if  self._BackgroundDirty:
                    dc.BeginDrawing()
                    dc.Clear()
                    i = 0
                    for Object in self._DrawList:
                        if self.BBCheck(Object.BoundingBox,ViewPortBB):
                            #print "object is in Bounding Box"
                            i+=1
                            Object._Draw(dc,self.WorldToPixel,self.ScaleFunction)
                            if i % self.NumBetweenBlits == 0:
                                ScreenDC.Blit(0, 0, self.PanelSize[0],self.PanelSize[1], dc, 0, 0)
                    dc.EndDrawing()
            else:
                dc.Clear()
            self._BackgroundDirty = 0
            dc.SelectObject(self._Buffer)
            dc.BeginDrawing()
            ##Draw Background on Main Buffer:
            dc.DrawBitmap(self._BackBuffer,0,0)
            #Draw the OnTop stuff
            i = 0
            for Object in self._TopDrawList:
                i+=1
                Object._Draw(dc,self.WorldToPixel,self.ScaleFunction)
                if i % self.NumBetweenBlits == 0:
                    ScreenDC.Blit(0, 0, self.PanelSize[0],self.PanelSize[1], dc, 0, 0)
            dc.EndDrawing()
        else: # not using a Background DC
            dc = wxMemoryDC()
            dc.SelectObject(self._Buffer)
            dc.SetBackground(self.DrawPanel.BackgroundBrush)
            if self._DrawList:
                dc.BeginDrawing()
                dc.Clear()
                i = 0
                ViewPortWorld = array((self.PixelToWorld((0,0)), self.PixelToWorld(self.DrawPanel.GetSizeTuple() ) ),Float )
                ViewPortBB = array( (minimum.reduce(ViewPortWorld), maximum.reduce(ViewPortWorld)) )
                for Object in self._DrawList:
                    if self.BBCheck(Object.BoundingBox,ViewPortBB):
                        #print "object is in Bounding Box"
                        i+=1
                        Object._Draw(dc,self.WorldToPixel,self.ScaleFunction)
                        if i % self.NumBetweenBlits == 0:
                            ScreenDC.Blit(0, 0, int(self.PanelSize[0]), int(self.PanelSize[1]), dc, 0, 0)
                print "there were %i objects drawn"%i
                dc.EndDrawing()
            else:
                dc.Clear()
        # now refresh the screen
        ScreenDC.DrawBitmap(self._Buffer,0,0)
        if self.Debug: print "Drawing took %f seconds of CPU time"%(clock()-start)

    def BBCheck(self, BB1, BB2):
        """

        BBCheck(BB1, BB2) returns True is the Bounding boxes intesect, False otherwise

        """
        if ( (BB1[1,0] > BB2[0,0]) and (BB1[0,0] < BB2[1,0]) and
             (BB1[1,1] > BB2[0,1]) and (BB1[0,1] < BB2[1,1]) ):
            return True
        else:
            return False

    def	Move(self,shift,CoordType):
        """
        move the image in the window.

        shift is an (x,y) tuple, specifying the amount to shift in each direction

        It can be in any of three coordinates: Panel, Pixel, World,
        specified by the CoordType parameter

        Panel coordinates means you want to shift the image by some
        fraction of the size of the displaed image

        Pixel coordinates means you want to shift the image by some number of pixels

        World coordinates meand you want to shift the image by an amount
        in Floating point world coordinates

        """
        
        shift = array(shift,Float)
        if CoordType == 'Panel':# convert from panel coordinates
            shift = shift * array((-1,1),Float) *self.PanelSize/self.TransformVector
        elif CoordType == 'Pixel': # convert from pixel coordinates
            shift = shift/self.TransformVector
        elif CoordType == 'World': # No conversion
            pass
        else:
            raise 'CoordType must be either "Panel", "Pixel", or "World"'
            
        self.ViewPortCenter = self.ViewPortCenter + shift 
        self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
        self.TransformVector = array((self.Scale,-self.Scale),Float)* self.MapProjectionVector
        self._BackgroundDirty = 1
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
        if center:
            self.ViewPortCenter = array(center,Float)
        self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
        self.TransformVector = array((self.Scale,-self.Scale),Float)* self.MapProjectionVector
        self._BackgroundDirty = 1
        self.Draw()
        
    def ZoomToFit(self,event):
        self.ZoomToBB()
        
    def ZoomToBB(self,NewBB = None,DrawFlag = 1):

        """

        Zooms the image to the bounding box given, or to the bounding
        box of all the objects on the canvas, if none is given.

        """
        
        if NewBB:
            BoundingBox = NewBB
        else:
            if self.BoundingBoxDirty:
                self._ResetBoundingBox()
            BoundingBox = self.BoundingBox
        if BoundingBox:
            self.ViewPortCenter = array(((BoundingBox[0,0]+BoundingBox[1,0])/2,
                                                                     (BoundingBox[0,1]+BoundingBox[1,1])/2 ),Float)
            self.MapProjectionVector = self.ProjectionFun(self.ViewPortCenter)
            # Compute the new Scale
            BoundingBox = BoundingBox * self.MapProjectionVector
            try:
                self.Scale = min((self.PanelSize[0]  / (BoundingBox[1,0]-BoundingBox[0,0])),
                                        (self.PanelSize[1] / (BoundingBox[1,1]-BoundingBox[0,1])))*0.95
            except ZeroDivisionError: # this will happen if the BB has zero width or height
                try: #width
                    self.Scale = (self.PanelSize[0]  / (BoundingBox[1,0]-BoundingBox[0,0]))*0.95
                except ZeroDivisionError:
                    try: # height
                        self.Scale = (self.PanelSize[1]  / (BoundingBox[1,1]-BoundingBox[0,1]))*0.95
                    except ZeroDivisionError: #zero size! (must be a single point)
                        self.Scale = 1
                        
            self.TransformVector = array((self.Scale,-self.Scale),Float)* self.MapProjectionVector
            if DrawFlag:
                self._BackgroundDirty = 1
                self.Draw()
                
    def RemoveObjects(self,Objects):
        for Object in Objects:
            self.RemoveObject(Object,ResetBB = 0)
        self.BoundingBoxDirty = 1
        
    def RemoveObject(self,Object,ResetBB = 1):
        if Object.Foreground:
            self._TopDrawList.remove(Object)
        else:
            self._DrawList.remove(Object)
            self._BackgroundDirty = 1

        if ResetBB:
            self.BoundingBoxDirty = 1

    def Clear(self, ResetBB = True):
        self._DrawList = []
        if self.UseBackground:
            self._TopDrawList = []
            self._BackgroundDirty = 1
        if ResetBB:
            self._ResetBoundingBox()

    def _AddBoundingBox(self,NewBB):
        if self.BoundingBox is None:
            self.BoundingBox = NewBB
            self.ZoomToBB(NewBB,DrawFlag = 0)
        else:
            self.BoundingBox = array(((min(self.BoundingBox[0,0],NewBB[0,0]),
                                       min(self.BoundingBox[0,1],NewBB[0,1])),
                                      (max(self.BoundingBox[1,0],NewBB[1,0]),
                                       max(self.BoundingBox[1,1],NewBB[1,1]))),Float)
    def _ResetBoundingBox(self):
        # NOTE: could you remove an item without recomputing the entire bounding box?
        self.BoundingBox = None
        if self._DrawList:
            self.BoundingBox = self._DrawList[0].BoundingBox
        for Object in self._DrawList[1:]:
            self._AddBoundingBox(Object.BoundingBox)
        if self.UseBackground:
            for Object in self._TopDrawList:
                self._AddBoundingBox(Object.BoundingBox)
        if self.BoundingBox is None:
            self.ViewPortCenter= array( (0,0), Float)
            self.TransformVector = array( (1,-1), Float)
            self.MapProjectionVector = array( (1,1), Float)			
            self.Scale = 1
        self.BoundingBoxDirty = 0

    def PixelToWorld(self,Points):
        """
        Converts coordinates from Pixel coordinates to world coordinates.
        
        Points is a tuple of (x,y) coordinates, or a list of such tuples, or a NX2 Numpy array of x,y coordinates.
        
        """
        return  (((array(Points,Float) - (self.PanelSize/2))/self.TransformVector) + self.ViewPortCenter)
        
    def WorldToPixel(self,Coordinates):
        """
        This function will get passed to the drawing functions of the objects,
        to transform from world to pixel coordinates.
        Coordinates should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """
        return  (((array(Coordinates,Float) - self.ViewPortCenter)*self.TransformVector)+(self.PanelSize/2)).astype('i')
        
    def ScaleFunction(self,Lengths):
        """
        This function will get passed to the drawing functions of the objects,
        to Change a length from world to pixel coordinates.
        
        Lengths should be a NX2 array of (x,y) coordinates, or
        a 2-tuple, or sequence of 2-tuples.
        """
        return  (array(Lengths,Float)*self.TransformVector).astype('i')
        
            
    ## This is a set of methods that add objects to the Canvas. It kind
    ## of seems like a lot of duplication, but I wanted to be able to
    ## instantiate the draw objects separatley form adding them, but
    ## also to be able to do add one oin one step. I'm open to better
    ## ideas...
    def AddRectangle(self,x,y,width,height,
                                     LineColor = "Black",
                                     LineStyle = "Solid",
                                     LineWidth    = 1,
                                     FillColor    = None,
                                     FillStyle    = "Solid",
                                     Foreground = 0):
        Object = Rectangle(x,y,width,height,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddEllipse(self,x,y,width,height,
                                     LineColor = "Black",
                                     LineStyle = "Solid",
                                     LineWidth    = 1,
                                     FillColor    = None,
                                     FillStyle    = "Solid",
                                     Foreground = 0):
        Object = Ellipse(x,y,width,height,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddCircle(self,x,y,Diameter,
                                     LineColor = "Black",
                                     LineStyle = "Solid",
                                     LineWidth    = 1,
                                     FillColor    = None,
                                     FillStyle    = "Solid",
                                     Foreground = 0):
        Object = Circle(x,y,Diameter,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddDot(self,x,y,Diameter,
                                     LineColor = "Black",
                                     LineStyle = "Solid",
                                     LineWidth    = 1,
                                     FillColor    = None,
                                     FillStyle    = "Solid",
                                     Foreground = 0):
        Object = Dot(x,y,Diameter,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddPolygon(self,Points,
                               LineColor = "Black",
                               LineStyle = "Solid",
                               LineWidth    = 1,
                               FillColor    = None,
                               FillStyle    = "Solid",
                                     Foreground = 0):
    
        Object = Polygon(Points,LineColor,LineStyle,LineWidth,FillColor,FillStyle,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddLine(self,Points,
                            LineColor = "Black",
                            LineStyle = "Solid",
                            LineWidth    = 1,
                                     Foreground = 0):
    
        Object = Line(Points,LineColor,LineStyle,LineWidth,Foreground)
        self.AddObject(Object)
        return Object

    def AddLineSet(self,Points,
                   LineColors = "Black",
                   LineStyles = "Solid",
                   LineWidths = 1,
                   Foreground = 0):
    
        Object = LineSet(Points,LineColors,LineStyles,LineWidths,Foreground)
        self.AddObject(Object)
        return Object

    def AddPointSet(self,Points,
                                    Color = "Black",
                                    Diameter = 1,
                                     Foreground = 0):
        
        Object = PointSet(Points,Color,Diameter,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddText(self,String,x,y,
                            Size = 20,
                            ForeGround = 'Black',
                            BackGround = None,
                            Family = 'Swiss',
                            Style = 'Normal',
                            Weight = 'Normal',
                            Underline = 0,
                            Position = 'tl',
                                     Foreground = 0):
        Object = Text(String,x,y,Size,ForeGround,BackGround,Family,Style,Weight,Underline,Position,Foreground)
        self.AddObject(Object)
        return Object
        
    def AddObject(self,obj):
        # put in a reference to the Canvas, so remove and other stuff can work
        obj._Canvas = self
        if  obj.Foreground and self.UseBackground:
            self._TopDrawList.append(obj)
        else:
            self._DrawList.append(obj)
            self._backgrounddirty = 1
        self._AddBoundingBox(obj.BoundingBox)
        return None
        





