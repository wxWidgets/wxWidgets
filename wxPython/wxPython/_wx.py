
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



# The wx*DC_old classes have Draw* method signatures that are mostly
# compatible with 2.4, so assign the new classes to wx*DC_new and make
# the _old classes be the defaults with the normal names.

wxDC_new = wxDC; wxDC = wxDC_old                 
wxMemoryDC_new = wxMemoryDC; wxMemoryDC = wxMemoryDC_old           
wxBufferedDC_new = wxBufferedDC; wxBufferedDC = wxBufferedDC_old         
wxBufferedPaintDC_new = wxBufferedPaintDC; wxBufferedPaintDC = wxBufferedPaintDC_old    
wxScreenDC_new = wxScreenDC; wxScreenDC = wxScreenDC_old           
wxClientDC_new = wxClientDC; wxClientDC = wxClientDC_old           
wxPaintDC_new = wxPaintDC; wxPaintDC = wxPaintDC_old            
wxWindowDC_new = wxWindowDC; wxWindowDC = wxWindowDC_old           
wxMirrorDC_new = wxMirrorDC; wxMirrorDC = wxMirrorDC_old           
wxPostScriptDC_new = wxPostScriptDC; wxPostScriptDC = wxPostScriptDC_old       
wxMetaFileDC_new = wxMetaFileDC; wxMetaFileDC = wxMetaFileDC_old         
wxPrinterDC_new = wxPrinterDC; wxPrinterDC = wxPrinterDC_old          

