# This file was created automatically by SWIG.
import oglc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from events import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *

from oglbasic import *

from oglshapes import *

from oglshapes2 import *

from oglcanvas import *
import wx


#-------------- FUNCTION WRAPPERS ------------------

wxOGLInitialize = oglc.wxOGLInitialize

wxOGLCleanUp = oglc.wxOGLCleanUp



#-------------- VARIABLE WRAPPERS ------------------

KEY_SHIFT = oglc.KEY_SHIFT
KEY_CTRL = oglc.KEY_CTRL
ARROW_NONE = oglc.ARROW_NONE
ARROW_END = oglc.ARROW_END
ARROW_BOTH = oglc.ARROW_BOTH
ARROW_MIDDLE = oglc.ARROW_MIDDLE
ARROW_START = oglc.ARROW_START
ARROW_HOLLOW_CIRCLE = oglc.ARROW_HOLLOW_CIRCLE
ARROW_FILLED_CIRCLE = oglc.ARROW_FILLED_CIRCLE
ARROW_ARROW = oglc.ARROW_ARROW
ARROW_SINGLE_OBLIQUE = oglc.ARROW_SINGLE_OBLIQUE
ARROW_DOUBLE_OBLIQUE = oglc.ARROW_DOUBLE_OBLIQUE
ARROW_METAFILE = oglc.ARROW_METAFILE
ARROW_POSITION_END = oglc.ARROW_POSITION_END
ARROW_POSITION_START = oglc.ARROW_POSITION_START
CONTROL_POINT_VERTICAL = oglc.CONTROL_POINT_VERTICAL
CONTROL_POINT_HORIZONTAL = oglc.CONTROL_POINT_HORIZONTAL
CONTROL_POINT_DIAGONAL = oglc.CONTROL_POINT_DIAGONAL
CONTROL_POINT_ENDPOINT_TO = oglc.CONTROL_POINT_ENDPOINT_TO
CONTROL_POINT_ENDPOINT_FROM = oglc.CONTROL_POINT_ENDPOINT_FROM
CONTROL_POINT_LINE = oglc.CONTROL_POINT_LINE
FORMAT_NONE = oglc.FORMAT_NONE
FORMAT_CENTRE_HORIZ = oglc.FORMAT_CENTRE_HORIZ
FORMAT_CENTRE_VERT = oglc.FORMAT_CENTRE_VERT
FORMAT_SIZE_TO_CONTENTS = oglc.FORMAT_SIZE_TO_CONTENTS
LINE_ALIGNMENT_HORIZ = oglc.LINE_ALIGNMENT_HORIZ
LINE_ALIGNMENT_VERT = oglc.LINE_ALIGNMENT_VERT
LINE_ALIGNMENT_TO_NEXT_HANDLE = oglc.LINE_ALIGNMENT_TO_NEXT_HANDLE
LINE_ALIGNMENT_NONE = oglc.LINE_ALIGNMENT_NONE
SHADOW_NONE = oglc.SHADOW_NONE
SHADOW_LEFT = oglc.SHADOW_LEFT
SHADOW_RIGHT = oglc.SHADOW_RIGHT
OP_CLICK_LEFT = oglc.OP_CLICK_LEFT
OP_CLICK_RIGHT = oglc.OP_CLICK_RIGHT
OP_DRAG_LEFT = oglc.OP_DRAG_LEFT
OP_DRAG_RIGHT = oglc.OP_DRAG_RIGHT
OP_ALL = oglc.OP_ALL
ATTACHMENT_MODE_NONE = oglc.ATTACHMENT_MODE_NONE
ATTACHMENT_MODE_EDGE = oglc.ATTACHMENT_MODE_EDGE
ATTACHMENT_MODE_BRANCHING = oglc.ATTACHMENT_MODE_BRANCHING
BRANCHING_ATTACHMENT_NORMAL = oglc.BRANCHING_ATTACHMENT_NORMAL
BRANCHING_ATTACHMENT_BLOB = oglc.BRANCHING_ATTACHMENT_BLOB
gyCONSTRAINT_CENTRED_VERTICALLY = oglc.gyCONSTRAINT_CENTRED_VERTICALLY
gyCONSTRAINT_CENTRED_HORIZONTALLY = oglc.gyCONSTRAINT_CENTRED_HORIZONTALLY
gyCONSTRAINT_CENTRED_BOTH = oglc.gyCONSTRAINT_CENTRED_BOTH
gyCONSTRAINT_LEFT_OF = oglc.gyCONSTRAINT_LEFT_OF
gyCONSTRAINT_RIGHT_OF = oglc.gyCONSTRAINT_RIGHT_OF
gyCONSTRAINT_ABOVE = oglc.gyCONSTRAINT_ABOVE
gyCONSTRAINT_BELOW = oglc.gyCONSTRAINT_BELOW
gyCONSTRAINT_ALIGNED_TOP = oglc.gyCONSTRAINT_ALIGNED_TOP
gyCONSTRAINT_ALIGNED_BOTTOM = oglc.gyCONSTRAINT_ALIGNED_BOTTOM
gyCONSTRAINT_ALIGNED_LEFT = oglc.gyCONSTRAINT_ALIGNED_LEFT
gyCONSTRAINT_ALIGNED_RIGHT = oglc.gyCONSTRAINT_ALIGNED_RIGHT
gyCONSTRAINT_MIDALIGNED_TOP = oglc.gyCONSTRAINT_MIDALIGNED_TOP
gyCONSTRAINT_MIDALIGNED_BOTTOM = oglc.gyCONSTRAINT_MIDALIGNED_BOTTOM
gyCONSTRAINT_MIDALIGNED_LEFT = oglc.gyCONSTRAINT_MIDALIGNED_LEFT
gyCONSTRAINT_MIDALIGNED_RIGHT = oglc.gyCONSTRAINT_MIDALIGNED_RIGHT
DIVISION_SIDE_NONE = oglc.DIVISION_SIDE_NONE
DIVISION_SIDE_LEFT = oglc.DIVISION_SIDE_LEFT
DIVISION_SIDE_TOP = oglc.DIVISION_SIDE_TOP
DIVISION_SIDE_RIGHT = oglc.DIVISION_SIDE_RIGHT
DIVISION_SIDE_BOTTOM = oglc.DIVISION_SIDE_BOTTOM


#-------------- USER INCLUDE -----------------------


wxShapeCanvas =       wxPyShapeCanvas
wxShapeEvtHandler =   wxPyShapeEvtHandler
wxShape =             wxPyShape
wxRectangleShape =    wxPyRectangleShape
wxBitmapShape =       wxPyBitmapShape
wxDrawnShape =        wxPyDrawnShape
wxCompositeShape =    wxPyCompositeShape
wxDividedShape =      wxPyDividedShape
wxDivisionShape =     wxPyDivisionShape
wxEllipseShape =      wxPyEllipseShape
wxCircleShape =       wxPyCircleShape
wxLineShape =         wxPyLineShape
wxPolygonShape =      wxPyPolygonShape
wxTextShape =         wxPyTextShape


# Stuff these names into the wx namespace so wxPyConstructObject can find them
import wx
wx.wxPyShapeCanvasPtr     = wxPyShapeCanvasPtr
wx.wxPyShapeEvtHandlerPtr = wxPyShapeEvtHandlerPtr
wx.wxPyShapePtr           = wxPyShapePtr
wx.wxPyRectangleShapePtr  = wxPyRectangleShapePtr
wx.wxPyBitmapShapePtr     = wxPyBitmapShapePtr
wx.wxPyDrawnShapePtr      = wxPyDrawnShapePtr
wx.wxPyCompositeShapePtr  = wxPyCompositeShapePtr
wx.wxPyDividedShapePtr    = wxPyDividedShapePtr
wx.wxPyDivisionShapePtr   = wxPyDivisionShapePtr
wx.wxPyEllipseShapePtr    = wxPyEllipseShapePtr
wx.wxPyCircleShapePtr     = wxPyCircleShapePtr
wx.wxPyLineShapePtr       = wxPyLineShapePtr
wx.wxPyPolygonShapePtr    = wxPyPolygonShapePtr
wx.wxPyTextShapePtr       = wxPyTextShapePtr
wx.wxShapeRegionPtr       = wxShapeRegionPtr
wx.wxOGLConstraintPtr     = wxOGLConstraintPtr
wx.wxControlPointPtr      = wxControlPointPtr
