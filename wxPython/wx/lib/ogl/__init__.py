"""
The Object Graphics Library provides for simple drawing and manipulation
of 2D objects.
"""

from _basic import *
from _diagram import *
from _canvas import *
from _lines import *
from _bmpshape import *
from _divided import *
from _composit import *
from _drawn import *


# Set things up for documenting with epydoc.  The __docfilter__ will
# prevent some things from being documented, and anything in __all__
# will appear to actually exist in this module.
import wx._core as _wx
__docfilter__ = _wx.__DocFilter(globals())
__all__ = [name for name in dir() if not name.startswith('_')]

