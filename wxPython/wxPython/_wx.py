
# Load all symbols that should appear in the wxPython.wx namespace
from core import *
from core import __version__
from gdi import *
from windows import *
from controls import *
from misc import *

# Cleanup this one.
del wx
