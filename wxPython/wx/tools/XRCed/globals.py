
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools.XRCed import globals
_rename(globals(), globals.__dict__, modulename='tools.XRCed.globals')
del globals
del _rename
