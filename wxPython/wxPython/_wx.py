
# Load all symbols that should appear in the wxPython.wx namespace
from _core import *
from _core import __version__
from _gdi import *
from _windows import *
from _controls import *
from _misc import *

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


