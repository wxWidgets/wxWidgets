
# Load all symbols that should appear in the wxPython.wx namespace
from core import *
from core import __version__
from gdi import *
from windows import *
from controls import *
from misc import *

# Cleanup this one.
del wx


# Make some aliases to help backawrds compatibility for the old
# namespace.  This is only for code that is using tthe wxPython.wx
# package and using names and classes in the old way.  New code should
# use the wx namespace and the new names.

wxPyDefaultPosition = wxDefaultPosition
wxPyDefaultSize = wxDefaultSize
wxNoRefBitmap = wxBitmap
wxSystemSettings_GetSystemColour = wxSystemSettings_GetColour
wxSystemSettings_GetSystemFont   = wxSystemSettings_GetFont
wxSystemSettings_GetSystemMetric = wxSystemSettings_GetMetric
wxColor = wxColour
wxNamedColor = wxNamedColour    

NULL = None
TRUE  = true  = True
FALSE = false = False

def wxPyTypeCast(obj, typeStr):
    return obj

wxPy_isinstance = isinstance


# To get wxDC methods compatible with the old 2.4 wxDC uncomment these
# lines.  Note however that doing this will break any code that
# expects the new-style methods.  (Is there a way to do this that does
# not have that problem?  I suppose we could provide two versions of
# the DC classes and just rename them here...)

#wxDC.FloodFill = wxDC.FloodFillXY
#wxDC.GetPixel = wxDC.GetPixelXY
#wxDC.DrawLine = wxDC.DrawLineXY
#wxDC.CrossHair = wxDC.CrossHairXY
#wxDC.DrawArc = wxDC.DrawArcXY
#wxDC.DrawCheckMark = wxDC.DrawCheckMarkXY
#wxDC.DrawEllipticArc = wxDC.DrawEllipticArcXY
#wxDC.DrawPoint = wxDC.DrawPointXY
#wxDC.DrawRectangle = wxDC.DrawRectangleXY
#wxDC.DrawRoundedRectangle = wxDC.DrawRoundedRectangleXY
#wxDC.DrawCircle = wxDC.DrawCircleXY
#wxDC.DrawEllipse = wxDC.DrawEllipseXY
#wxDC.DrawIcon = wxDC.DrawIconXY
#wxDC.DrawBitmap = wxDC.DrawBitmapXY
#wxDC.DrawText = wxDC.DrawTextXY
#wxDC.DrawRotatedText = wxDC.DrawRotatedTextXY
#wxDC.Blit = wxDC.BlitXY
#wxDC.SetClippingRegion = wxDC.SetClippingRegionXY 

